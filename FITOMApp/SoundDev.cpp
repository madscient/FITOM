#include "stdafx.h"
#include "Port.h"
#include "SoundDev.h"
#include "MIDI.h"
#include "FITOM.h"

namespace ROM {
extern const UINT16 Fnum399[];
extern const UINT16 Fnum358[];
extern const UINT16 Fnum400[];
extern const UINT16 Fnum409[];
extern const UINT8 VolCurveLin[];
extern const UINT8 VolCurveInv[];
};

ISoundDevice::FNUM::FNUM() : block(0), fnum(0)
{
}

ISoundDevice::FNUM::FNUM(UINT8 bl, UINT16 fn) : block(bl), fnum(fn)
{
}

ISoundDevice::CHATTR::CHATTR() :
	status(CHATTR::EMPTY), express(255), volume(255), velocity(255), panpot(127),
	lastfnum(FNUM(0, 0)), lastvel(0), lastnote(255), finefreq(0), count(0), parent(0)
{
}

ISoundDevice::LFORESOURCE::LFORESOURCE() : used(0), parent(0), amrate(0), amdepth(0), pmrate(0), pmdepth(0)
{
}

void CSoundDevice::CHATTR::Init()
{
	status = IsEnable() ? CHATTR::EMPTY : CHATTR::DISABLED;
	lastnote = 255;
	lastvel = 0;
	lastfnum = FNUM(0, 0);
	panpot = 127;
	express = 255;
	volume = 255;
	velocity = 255;
	count = 0;
}

void CSoundDevice::CHATTR::Assign(CMidiCh* parch)
{
	if (IsEnable()) {
		status = ASSIGNED;
		parent = parch;
		noteon = 0;
	}
}

void CSoundDevice::CHATTR::Run()
{
	if (IsEnable()) {
		status = RUNNING;
		noteon = 1;
	}
}

void CSoundDevice::CHATTR::Stop()
{
	if (IsRunning()) {
		status = ASSIGNED;
	}
}

void CSoundDevice::CHATTR::Release(int forgetparent)
{
	if (IsEnable()) {
		status = EMPTY;
		noteon = 0;
		if (forgetparent) {
			parent = 0;
		}
	}
}
void CSoundDevice::CHATTR::Enable(int ena)
{
	if (ena && !IsEnable()) {
		status = EMPTY;
	} else {
		status = DISABLED;
	}
}

void CSoundDevice::CHATTR::SetVoice(FMVOICE* vc)
{
	voice = *vc;
}

void CSoundDevice::CHATTR::SetVoiceID(UINT32 vcid)
{
	voice.ID = vcid;
}

SINT16 CSoundDevice::CHATTR::GetChLFOValue()
{
	SINT16 ret = 0;
	if (chlfo.GetStatus() != CSoundDevice::CLFOControl::LFO_NONE) {
		SINT16 lev = SINT16(chlfo.GetValue()); // <-- 0~127 absolute level
		if (lev) {
			SINT32 val = SINT16(GetLFOWave(voice.LWF, voice.LFO, chlfo.GetCount())); // <-- -120 ~ +120
			val = (val > 120) ? 120 : val;
			val = (val < -120) ? -120 : val;
			SINT32 dep = ((UINT16(voice.LDM&0x7f)<<7)|(voice.LDL&0x7f));
			if (dep > 8191) { dep -= 16384; } // <-- 0~16383 as -8191~+8191
			val = val * lev / 128;
			val = val * dep / 120;
			val = (val > 8191) ? 8191 : val;
			val = (val < -8192) ? -8192 : val;
			ret = SINT16(val);
		}
	}
	return ret;
}

ISoundDevice::FNUM CSoundDevice::CHATTR::GetFnumber(const UINT16* fnumtbl, SINT16 offset)
{
	FNUM ret;
	fnumtbl = fnumtbl ? fnumtbl : ROM::Fnum358;
	if (lastnote < 128) {
		SINT16 note = lastnote % 12;
		SINT16 oct = (lastnote / 12) - 2;
		SINT16 cent = finefreq + GetChLFOValue() + offset;

		// normalize
		if (cent < 0) {
			cent = -cent;
			note -= ((cent + 63) / 64);
			cent = (cent % 64) ? (64 - (cent % 64)) : 0;
		}
		if (cent > 63) {
			note += cent / 64;
			cent = cent % 64;
		}
		if (note < 0) {
			note = -note;
			oct -= ((note+11) / 12);
			note = (note % 12) ? (12 - (note % 12)) : 0;
		}
		if (note > 11) {
			oct += note /12;
			note = note % 12;
		}

		ret.fnum = fnumtbl[note * 64 + cent];

		if (oct < 0) {
			ret.block = 0;
			ret.fnum = (ret.fnum >> (-oct)) & 0x7ff;
		}
		else if (oct >= 7) {
			ret.block = 7;
			ret.fnum = ((ret.fnum << (oct - 7)) | 1) & 0x7ff;
		}
		else {
			ret.block = UINT8(oct);
		}
	}
	return ret;
}

void CSoundDevice::CHATTR::UpdateFnumber(ISoundDevice::FNUM* fnum)
{
	lastfnum = fnum ? *fnum : GetFnumber();
}

UINT8 CSoundDevice::CHATTR::GetEffectiveLevel()
{
	return ::CalcVolExpVel(volume, express, velocity);
}

void CSoundDevice::CHATTR::SetNote(UINT8 note, SINT16 fine)
{
	lastnote = note;
	finefreq = fine;
}

/*----------*/
CSoundDevice::CSoundDevice(UINT8 devid, UINT8 maxchs, UINT8 fsamp, CPort* pt) :
	device(devid), chs(maxchs), port(pt), rhythmvol(100), ops(4), prior_ch(0), rhythmcap(0)
{
	if (maxchs) {
		chattr = new CHATTR[int(maxchs)];
	} else {
		chattr = 0;
	}
	switch (fsamp) {
	case 0:
	default:
		Fnum = ROM::Fnum399;
		break;
	case 1:
		Fnum = ROM::Fnum400;
		break;
	case 2:
		Fnum = ROM::Fnum358;
		break;
	case 3:
		Fnum = ROM::Fnum409;
		break;
	}
}

CSoundDevice::~CSoundDevice()
{
	if (chs)
	{
		delete[] chattr;
		chattr = 0;
	}
}

void CSoundDevice::OverrideAttribute(ISoundDevice::CHATTR* attr)
{
	delete[] chattr;
	chattr = attr;
}

ISoundDevice::CHATTR* CSoundDevice::GetChAttribute(UINT8 ch) const
{
	return (ch < chs) ? &chattr[ch] : 0;
}

void CSoundDevice::Reset()
{
	for (int i=0; i<chs; i++) {
		NoteOff(i);
		CHATTR* attr = GetChAttribute(i);
		if (attr) {
			attr->Init();
		}
	}
	if (port) port->reset();
}

void CSoundDevice::SetDevice(UINT8 devid)
{
	device = devid;
}

void CSoundDevice::SetReg(UINT16 reg, UINT8 data, UINT8 v)
{
	port ? port->write(reg, data, v) : 0;
}

UINT8 CSoundDevice::GetReg(UINT16 reg, UINT8 v)
{
	UINT8 ret = 0;
	ret = port ? port->read(reg, (int)v) : 0;
	return ret;
}

void CSoundDevice::SetSustain(UINT8 ch, UINT8 sus, int update)
{
	if (ch < chs) {
		if (update) {
			UpdateSustain(ch);
		}
	}
}

void CSoundDevice::SetVoice(UINT8 ch, FMVOICE* voice, int update)
{
	if (ch < chs) {
		CHATTR* attr = GetChAttribute(ch);
		if (attr->GetVoiceID() != voice->ID || update < 1) {
			//if (attr->GetVoice()->AL & 0x40) {//Rhythm note mode
			//	UpdateKey(ch, 0);
			//}
			attr->SetVoice(voice);
			for (int i=0; i<4; i++) {
				attr->baseTL[i] = voice->op[i].TL;
			}
			if (1/*update*/) {
				UpdateVoice(ch);
			}
		} else {
			int oh_my_god = 1;
		}
	}
}

void CSoundDevice::ResetVoice(CMidiCh* pch, FMVOICE* voice, int update)
{
	for (int i=0; i<chs; i++) {
		if (GetChAttribute(i)->GetParent() == pch || !pch) {
			if (voice) {
				SetVoice(i, voice, -1);
			} else {
				GetChAttribute(i)->SetVoiceID(0xffffffffL);
			}
		}
	}
}

void CSoundDevice::SetVolume(UINT8 ch, UINT8 vol, int update)
{
	if (ch < chs) {
		if (GetChAttribute(ch)->volume != vol) {
			GetChAttribute(ch)->volume = vol;
			if (update) {
				UpdateVolExp(ch);
			}
		}
	}
}

void CSoundDevice::SetRhythmVol(UINT8 vol, int update)
{
	rhythmvol = vol;
	if (update) {
		UpdateRhythmVol();
	}
}

void CSoundDevice::SetExpress(UINT8 ch, UINT8 exp, int update)
{
	if (ch < chs) {
		if (GetChAttribute(ch)->express != exp) {
			GetChAttribute(ch)->express = exp;
			if (update) {
				UpdateVolExp(ch);
			}
		}
	}
}

void CSoundDevice::SetPanpot(UINT8 ch, UINT8 pan, int update)
{
	if (ch < chs)
	{
		SINT8 panpot = pan - 64;
		if (GetChAttribute(ch)->panpot != panpot) {
			GetChAttribute(ch)->panpot = panpot;
			if (update) {
				UpdatePanpot(ch);
			}
		}
	}
}

void CSoundDevice::SetMasterVolume(UINT8 vol, int update)
{
	if (volume != vol) {
		volume = vol;
		for (int i=0; update && i<chs; i++)
		{
			if (GetChAttribute(i)->IsEnable()) {
				UpdateVolExp((UINT8)i);
			}
		}
	}
}

UINT8 CSoundDevice::AllocCh(CMidiCh* parent, FMVOICE* voice)
{
	UINT8 ret = 0xff;
	ret = QueryCh(parent, voice, 1);
	if (ret == 0xff) {
		ret = QueryCh(NULL, voice, 1);
	}
	if (ret == 0xff) {
		ret = QueryCh(NULL, NULL, 1);
	}
	if (ret == 0xff) {
		DWORD count = 0;
		for (int i = 0; i < chs; i++) {
			CHATTR* attr = GetChAttribute(i);
			if (count <= attr->noteon) {
				ret = i;
			}
		}
	}
	/*
	if (ret == 0xff) {
		ret = QueryCh(parent, NULL, 0);
	}
	if (ret == 0xff) {
		ret = QueryCh(NULL, voice, 0);
	}
	if (ret == 0xff) {
		ret = QueryCh(NULL, NULL, 0);
	}*/
	Assign(ret, parent, voice);
	return (ret);
}

UINT8 CSoundDevice::Assign(UINT8 ch, CMidiCh* parent, FMVOICE* voice)
{
	CHATTR* attr = GetChAttribute(ch);
	if (ch < chs) {
		if (attr->IsRunning()) {
			if (attr->GetParent()) {
				if (!attr->GetParent()->IsRhythm()) {
					attr->GetParent()->NoteOff(attr->GetLastNote());
				} else {
					((CRhythmCh*)(attr->GetParent()))->NoteOff(this, ch);
				}
			} else {
				NoteOff(ch);	// should never come here...
			}
		}
		if (attr->IsEnable()) {
			attr->Assign(parent);
			SetVoice(ch, voice);
			return ch;
		} else {
			return 0xff;
		}
	} else {
		return 0xff;
	}
}

UINT8 CSoundDevice::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	UINT8 ret = 0xff;
	UINT8 tmp = 0xff;
	CHATTR* attr = 0;
	if (parent && voice) {
		tmp = prior_ch;
		for (int i=0; i<chs; i++) {
			attr = GetChAttribute(tmp);
			if (attr && (mode ? attr->IsAvailable() : attr->IsEnable()) && attr->GetVoiceID() == voice->ID && attr->GetParent() == parent) {
				ret = tmp;
				break;
			}
			tmp = (prior_ch + i) % chs;
		}
	} else if (voice) {
		tmp = prior_ch;
		for (int i = 0; i<chs; i++) {
			attr = GetChAttribute(tmp);
			if (attr && (mode ? attr->IsAvailable() : attr->IsEnable()) && attr->GetVoiceID() == voice->ID) {
				ret = tmp;
				break;
			}
			tmp = (prior_ch + i) % chs;
		}
	} else {
		tmp = prior_ch;
		for (int i=0; i<chs; i++) {
			attr = GetChAttribute(tmp);
			if (attr && (mode ? attr->IsAvailable() : 1) && attr->IsEnable()) {
				ret = tmp;
				break;
			}
			tmp = (prior_ch + i) % chs;
		}
	}
	if (ret == 0xff) {
		prior_ch = (prior_ch + 1) % chs;
	}
	return ret;
}

void CSoundDevice::ReleaseCh(UINT8 ch)
{
	if (ch < chs)
	{
		if (GetChAttribute(ch)->IsEnable()) {
			GetChAttribute(ch)->Release(0);
		} else {
			int oh_my_god = 1;
		}
		//chattr[ch].parent = 0;
	}
}

void CSoundDevice::EnableCh(UINT8 ch, UINT8 ena)
{
	if (ch < chs)
	{
		GetChAttribute(ch)->Enable(ena);
	}
}

void CSoundDevice::SetVelocity(UINT8 ch, UINT8 vel, int update)
{
	if (ch < chs && vel < 128) {
		GetChAttribute(ch)->velocity = vel;
		if (update) {
			UpdateVolExp(ch);
		}
	}
}

void CSoundDevice::SetNote(UINT8 ch, UINT8 note, SINT16 fine, int update)
{
	if (ch < chs) {
		GetChAttribute(ch)->SetNote(note, fine);
		UpdateFnumber(ch, update);
	}
}

UINT8 CSoundDevice::GetAvailable()
{
	UINT8 ret = 0;
	for (int i=0; i<chs; i++) {
		if (GetChAttribute(i)->IsEnable()) { ret++; }
	}
	return ret;
}

UINT8 CSoundDevice::GetAssignable()
{
	UINT8 ret = 0;
	for (int i=0; i<chs; i++) {
		if (GetChAttribute(i)->IsAvailable()) { ret++; }
	}
	return ret;
}

UINT32 CSoundDevice::GetCurrentVoiceID(UINT8 ch) const
{
	UINT32 ret = 0;
	if (ch < chs) {
		ret = GetChAttribute(ch)->GetVoiceID();
	}
	return ret;
}

FMVOICE* CSoundDevice::GetCurrentVoice(UINT8 ch) const
{
	FMVOICE* ret = 0;
	if (ch < chs) {
		ret = GetChAttribute(ch)->GetVoice();
	}
	return ret;
}

UINT8 CSoundDevice::GetCurrentNote(UINT8 ch) const
{
	UINT8 ret = 255;
	if (ch < chs && GetChAttribute(ch)->IsRunning()) {
		ret = GetChAttribute(ch)->GetLastNote();
	}
	return ret;
}

ISoundDevice::FNUM CSoundDevice::GetCurrentFnum(UINT8 ch) const
{
	FNUM ret;
	if (ch < chs && GetChAttribute(ch)->IsRunning()) {
		ret = *GetChAttribute(ch)->GetLastFnumber();
	}
	return ret;
}

void CSoundDevice::GetDescriptor(char* str)
{
	wsprintf(str, "%02X:%s (%ich)", device, CFITOM::GetDeviceNameFromID(device), chs);
}

void CSoundDevice::PollingCallBack()
{
}

void CSoundDevice::TimerCallBack(UINT32 tick)
{
	int tflag = (tick&1);
	if (/*tflag == 0*/1) {
		for (int i=0; i<chs; i++) {
			CHATTR* attr = GetChAttribute(i);
			if (attr->IsRunning()) {
				attr->noteon++;
				if (attr->chlfo.Update()) {
					UpdateFnumber(i);
				}
				for (int j=0; j<4; j++) {
					if (attr->oplfo[j].Update()) {
						UpdateOpLFO(i, j);
					}
				}
			}
		}
	}
}

ISoundDevice::FNUM CSoundDevice::GetFnumber(UINT8 ch)
{
	return GetChAttribute(ch)->GetFnumber(Fnum);
}

void CSoundDevice::UpdateFnumber(UINT8 ch, int update)
{
	if (ch < chs) {
		FNUM fnum = GetFnumber(ch);
		GetChAttribute(ch)->UpdateFnumber(&fnum);
		if (update) {
			UpdateFreq(ch);
		}
	}
}

void CSoundDevice::UpdateOpLFO(UINT8 ch, UINT8 op)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	SINT16 lev = SINT16(attr->oplfo[op].GetValue());
	if (lev) {
		SINT16 val = SINT16(GetLFOWave(voice->op[op].SLY, voice->op[op].SLF, attr->oplfo[op].GetCount()));
		SINT16 dep = SINT16(voice->op[op].SLD);
		if (dep > 63) { dep -= 128; }
		val = val * lev / 128; 
		val = val * dep / 120;
		val += attr->baseTL[op];
		val = (val < 0) ? 0 : val;
		val = (val > 127) ? 127 : val;
		UpdateTL(ch, op, UINT8(val));
	} else {
		UpdateTL(ch, op,  attr->baseTL[op]);
	}
}

void CSoundDevice::NoteOn(UINT8 ch, UINT8 vel)
{
	if (ch < chs) {
		CHATTR* attr = GetChAttribute(ch);
		FMVOICE* voice = attr->GetVoice();
		if (attr->IsEnable()) {
			if (voice->AL & 0x40) {
				UpdateKey(ch, 0);
			}
			SetVelocity(ch, vel);
			attr->Run();
			if (voice->LFR) {
				attr->chlfo.Start(voice->LFD, voice->LFR);
			}
			for (int op=0; op<ops; op++) {
				if (voice->op[op].SLR) {
					attr->oplfo[op].Start(voice->op[op].SLY, voice->op[op].SLR);
				}
			}
			UpdateKey(ch, 1);
		} else {
			int oh_my_god = 1;
		}
	}
}

void CSoundDevice::NoteOff(UINT8 ch)
{
	if (ch < chs) {
		CHATTR* attr = GetChAttribute(ch);
		if (attr) {
			attr->Release(0);
			attr->chlfo.Stop();
			for (int op=0; op<ops; op++) {
				attr->oplfo[op].Stop();
			}
			UpdateKey(ch, 0);
		} else {
			int oh_my_god = 1;
		}
	}
}

#if 0
CMidiCh* CSoundDevice::GetParent(UINT8 ch) const
{
	if (ch < chs) {
		CHATTR* attr = GetChAttribute(ch);
		if (attr) {
			return attr->parent;
		}
	}
	return NULL;
}

ISoundDevice::CHATTR::CHSTAT CSoundDevice::GetChStatus(UINT8 ch) const
{
	if (ch < chs) {
		CHATTR* attr = GetChAttribute(ch);
		if (attr) {
			return attr->status;
		}
	}
	return CHATTR::EMPTY;
}
#endif
