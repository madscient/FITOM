#include "STDAFX.h"
#include "FITOM.h"
#include "OPM.h"
#include "MIDI.h"

UINT8 COPM::KeyCode[] = { 0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14, };
UINT8 COPM::map[] = {0, 2, 1, 3};
UINT8 COPM::carmsk[] = { 0x8, 0x8, 0x8, 0x8, 0xa, 0xe, 0xe, 0xf, };

#define GET_AR(v,o)	(v->op[o].AR >> 2)
#define GET_DR(v,o)	(v->op[o].DR >> 2)
#define GET_SR(v,o)	(v->op[o].SR >> 2)
#define GET_RR(v,o)	(v->op[o].RR >> 3)
#define GET_SL(v,o)	(v->op[o].SL >> 3)
#define GET_TL(v,o)	(v->op[o].TL)
#define GET_RV(v,o)	(v->op[o].REV >> 3)

COPM::COPM(CPort* pt, UINT8 fsamp) : CSoundDevice(DEVICE_OPM, 8, fsamp, pt), lfos(1)
{
	lfores = new LFORESOURCE[1];
	switch (fsamp) {
	case 0: //3.9936MHz
		MasterTune = -122;
		break;
	case 1: //4.0000MHz
		MasterTune = -123;
		break;
	case 2: //3.579545MHz
		MasterTune = 0;
		break;
	}
	SetReg(0x01, 0x00, 1);
	SetReg(0x14, 0x00, 1);
}

ISoundDevice::FNUM COPM::GetFnumber(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FNUM ret;
	SINT16 note = attr->GetLastNote()-1;
	SINT16 oct = (note / 12) - 1;
	SINT16 cent = attr->GetLastFineFreq() + attr->GetChLFOValue() + MasterTune;
	note %= 12;

	// normalize
	if (cent < 0) {
		cent = -cent;
		note -= ((cent+63) / 64);
		cent = 64 - (cent % 64);
	}
	if (cent > 63) {
		note += (cent / 64);
		cent = cent % 64;
	}
	if (note < 0) {
		note = -note;
		oct -= ((note+11) / 12);
		note = 12 - (note % 12);
	}
	if (note > 11) {
		oct += (note / 12);
		note = note % 12;
	}
	if (oct >= 0 && oct < 8) {
		ret.block = (oct << 4) | COPM::KeyCode[note];
		ret.fnum = (attr->GetLastNote() << 8) | (UINT8(cent & 0xff) << 2);	//Key Fraction
	} else {
		ret.block = 0;
		ret.fnum = 0;
	}
	return ret;
}

void COPM::UpdateVoice(UINT8 ch)
{
	UINT8	tmp;
	UINT8	i;
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();

	tmp = (GetReg(0x20 + ch, 0) & 0xc0) | (voice->FB << 3) | (voice->AL & 7);
	SetReg(0x20 + ch, tmp);
	SetReg(0x38 + ch, 0);	//LFO disable

	for (i=0; i<4; i++)
	{
		UINT8 ofm = voice->op[map[i]].VIB;
		tmp = ((voice->op[map[i]].DT1 & 0x7) << 4) | (voice->op[map[i]].MUL & 0xf);
		SetReg(0x40 + i * 8 + ch, tmp);
		SetReg(0x60 + i * 8 + ch, GET_TL(voice, map[i]));
		tmp = ((voice->op[map[i]].KSR & 0x3) << 6) | GET_AR(voice, map[i]) | (ofm ? 0x20 : 0);
		SetReg(0x80 + i * 8 + ch, tmp);
		tmp = ((voice->op[map[i]].AM & 0x1) << 7) | GET_DR(voice, map[i]);
		SetReg(0xa0 + i * 8 + ch, tmp);
		tmp = (ofm ? 0 : ((voice->op[map[i]].DT2 & 0x3) << 6)) | GET_SR(voice, map[i]);
		SetReg(0xc0 + i * 8 + ch, tmp);
		tmp = (GET_SL(voice, map[i]) << 4) | GET_RR(voice, map[i]);
		SetReg(0xe0 + i * 8 + ch, tmp);
	}
	if (ch == 7) {
		if (voice->AL & 0x8) {
			SetReg(0xf, 0x80 | voice->NFQ);
		} else {
			SetReg(0xf, 0);
		}
	}
	UpdatePanpot(ch);
	UpdateVolExp(ch);
}

void COPM::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	UINT8 evol = attr->GetEffectiveLevel();
	FMVOICE* voice = attr->GetVoice();
	for (int i=0; i<4; i++) {
		if (carmsk[voice->AL&7] & (1<<i)) {
			UINT8 tl = CalcEffectiveLevel(evol, voice->op[i].TL);
			SetReg(0x60 + map[i] * 8 + ch, tl);
			attr->baseTL[i] = tl;
		}
	}
}

void COPM::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	SetReg(0x28 + ch, fnum->block, 1);	// kc
	SetReg(0x30 + ch, (UINT8(fnum->fnum) & 0xfc) | (GetReg(0x30 + ch, 0) & 0x3), 1);	// kf
}

void COPM::UpdatePanpot(UINT8 ch)
{
	int pan = (GetChAttribute(ch)->panpot) / 16;
	UINT8 chena = 0;
	if (pan == 0) { //C
		chena = 0xc0;
	} else if (pan > 0) { //R
		chena = 0x40;
	} else if (pan < 0) { //L
		chena = 0x80;
	}
	if ((GetReg(0x20 + ch, 0) & 0xc0) != chena) {
		SetReg(0x20 + ch, (GetReg(0x20 + ch, 0) & 0x3f) | chena, 1);
	}
}

void COPM::UpdateSustain(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	for (int i=0; i<4; i++) {
		UINT8 rr = ((parent && parent->GetSustain()) ? GET_RV(voice, map[i]) : GET_RR(voice, map[i])) & 0xf;
		UINT8 tmp = (GET_SL(voice, map[i]) << 4) | rr;
		SetReg(0xe0 + (i * 8) + ch, tmp);
	}
}

void COPM::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	SetReg(0x60 + map[op] * 8 + ch, lev);
}

void COPM::UpdateKey(UINT8 ch, UINT8 keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	if (keyon && parent && parent->GetForceDamp()) {
		for (int i=0; i<4; i++) {
			UINT8 tmp = (GET_SL(voice, map[i]) << 4) | 0xf;
			SetReg(0xe0 + (i * 8) + ch, tmp);
		}
	} else {
		UpdateSustain(ch);
	}
	SetReg(0x08, (keyon ? 0x78 : 0) | ch, 1);
}

UINT8 COPM::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	CHATTR* attr7 = GetChAttribute(7);
	if (voice && (voice->AL & 0x8)) {//Noise enabled
		if (mode ? attr7->IsAvailable() : attr7->IsEnable()) {
			return 7;
		} else {
			return 0xff;
		}
	}
	return CSoundDevice::QueryCh(parent, voice, mode);
}

void COPM::EnableDevPM(UINT8 ch, UINT8 on)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	if (ch < chs && on) {
		if (lfores[0].parent != parent) {
			for (int i=0; i<chs; i++) {
				if (lfores[0].used & (1 << i)) {
					//SetReg(0xa0 + i, GetReg(0xa0 + i, 0) & 0x7f);
					//SetReg(0xa8 + i, GetReg(0xa8 + i, 0) & 0x7f);
					//SetReg(0xb0 + i, GetReg(0xb0 + i, 0) & 0x7f);
					//SetReg(0xb8 + i, GetReg(0xb8 + i, 0) & 0x7f);
					SetReg(0x38+i, GetReg(0x38+i, 0) & 3);
				}
			}
			lfores[0].used = 0L;
		}
		lfores[0].parent = parent;
		lfores[0].used |= (1 << ch);
		SetReg(0x38 + ch, (GetReg(0x38 + ch, 0) & 0x3) | (voice->PMS << 4));
	} else if (ch < chs) {
		SetReg(0x38 + ch, (GetReg(0x38 + ch, 0) & 0x3));
		lfores[0].used &= ~UINT32(1 << ch);
		if (lfores[0].used == 0L) {
			lfores[0].parent = 0;
		}
	}
}

void COPM::EnableDevAM(UINT8 ch, UINT8 on)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	if (ch < chs && on) {
		if (lfores[0].parent != parent) {
			for (int i=0; i<chs; i++) {
				if (lfores[0].used & (1 << i)) {
					SetReg(0x38+i, GetReg(0x38+i, 0) & 0x70);
				}
			}
			lfores[0].used = 0L;
		}
		lfores[0].parent = parent;
		lfores[0].used |= (1 << ch);
		SetReg(0x38 + ch, (GetReg(0x38 + ch, 0) & 0x70) | (voice->AMS));
	} else if (ch < chs) {
		SetReg(0x38 + ch, (GetReg(0x38 + ch, 0) & 0x70));
		lfores[0].used &= ~UINT32(1 << ch);
		if (lfores[0].used == 0L) {
			lfores[0].parent = 0;
		}
	}
}

void COPM::SetDevPMDepth(UINT8 ch, UINT8 dep)
{
	if (ch < chs) {
		if (lfores[0].used && lfores[0].parent != GetChAttribute(ch)->GetParent()) {
			EnableDevPM(ch, dep);
		}
		if (lfores[0].pmdepth != dep) {
			lfores[0].pmdepth = dep;
			SetReg(0x19, 0x80 | dep);
		}
	}
}

void COPM::SetDevAMDepth(UINT8 ch, UINT8 dep)
{
	if (ch < chs) {
		if (lfores[0].used && lfores[0].parent != GetChAttribute(ch)->GetParent()) {
			EnableDevAM(ch, dep);
		}
		if (lfores[0].amdepth != dep) {
			lfores[0].amdepth = dep;
			SetReg(0x19, dep & 0x7f);
		}
	}
}

void COPM::SetDevAMRate(UINT8 ch, UINT8 rate)
{
	if (lfores[0].amrate != rate) {
		lfores[0].amrate = rate;
		SetReg(0x18, rate << 1);
	}
}

void COPM::SetDevPMRate(UINT8 ch, UINT8 rate)
{
	if (lfores[0].pmrate != rate) {
		lfores[0].pmrate = rate;
		SetReg(0x18, rate << 1);
	}
}

//-----

COPP::COPP(CPort* pt, UINT8 fsamp) : COPM(pt, fsamp)
{
	SetDevice(DEVICE_OPP);
}

//-----
COPZ::COPZ(CPort* pt, UINT8 fsamp) : COPM(pt, fsamp)
{
	SetDevice(DEVICE_OPZ);
}

void COPZ::UpdatePanpot(UINT8 ch)
{
	int pan = (GetChAttribute(ch)->panpot) / 16;
	UINT8 chena = 0;
	UINT8 mono = 0;
	if (pan == 0) { //C
		chena = 0x80;
		mono = 0x1;
	}
	else if (pan > 0) { //R
		chena = 0x80;
	}
	else if (pan < 0) { //L
	}
	SetReg(0x20 + ch, (GetReg(0x20 + ch, 0) & 0x3f) | chena, 1);
	SetReg(0x30 + ch, (GetReg(0x30 + ch, 0) & 0xfe) | mono, 1);
}

void COPZ::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	UINT8 evol = ::CalcVolExpVel(127, attr->express, attr->velocity);;
	FMVOICE* voice = attr->GetVoice();
	for (int i = 0; i<4; i++) {
		if (carmsk[voice->AL & 7] & (1 << i)) {
			UINT8 tl = CalcEffectiveLevel(evol, voice->op[i].TL);
			SetReg(0x60 + map[i] * 8 + ch, tl);
			attr->baseTL[i] = tl;
		}
	}
	SetReg(0x0 + ch, attr->volume, 1);
}

void COPZ::UpdateVoice(UINT8 ch)
{
	COPM::UpdateVoice(ch);
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	for (int i = 0; i < 4; i++) {
		UINT8 tmp;
		UINT8 ofm = voice->op[map[i]].VIB;
		tmp = 0x80 | ((voice->op[map[i]].WS & 0x7) << 4) | (ofm ? (voice->op[map[i]].DT2 & 0xf) : 0);
		SetReg(0x80 + i * 8 + ch, tmp);
		tmp = ((voice->op[map[i]].EGS & 0x3) << 6) | (voice->op[map[i]].REV >> 4) | 0x20;
		SetReg(0xc0 + i * 8 + ch, tmp);
	}
}
