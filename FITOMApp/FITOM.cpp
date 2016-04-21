#include "stdafx.h"
#include "FITOM.h"
#include "OPN.h"
#include "OPM.h"
#include "OPL.h"
#include "OPLL.h"
#include "SSG.h"
#include "MIDIDEV.h"
#include "MIDI.h"

#include "tables.h"

const UINT8 CFITOM::GetDeviceIDFromName(TCHAR* name)
{
	for (int i=0; i<(sizeof(ROM::devmap)/sizeof(ROM::DEVIDMAP)); i++) {
		if (lstrcmp(ROM::devmap[i].chipname, name) == 0 || lstrcmp(ROM::devmap[i].chipcode, name) == 0) {
			return ROM::devmap[i].devid;
		}
	}
	return DEVICE_NONE;
}

const TCHAR* CFITOM::GetDeviceNameFromID(UINT8 devid)
{
	if (devid == 0xff) {
		return _T("RHYTHM");
	}
	for (int i=0; i<(sizeof(ROM::devmap)/sizeof(ROM::DEVIDMAP)); i++) {
		if (ROM::devmap[i].devid == devid) {
			return ROM::devmap[i].chipname;
		}
	}
	return _T("-----");
}

const SC_CHIP_TYPE CFITOM::GetScChipType(UINT8 devid)
{
	for (int i=0; i<(sizeof(ROM::devmap)/sizeof(ROM::DEVIDMAP)); i++) {
		if (ROM::devmap[i].devid == devid) {
			return ROM::devmap[i].sctype;
		}
	}
	return SC_CHIP_TYPE::SC_TYPE_NONE;
}

const UINT8 CFITOM::GetVoiceType(UINT8 devid)
{
	for (int i=0; i<(sizeof(ROM::devmap)/sizeof(ROM::DEVIDMAP)); i++) {
		if (ROM::devmap[i].devid == devid) {
			return ROM::devmap[i].voicetype;
		}
	}
	return 0;
}

const DWORD CFITOM::GetRegSize(UINT8 devid)
{
	for (int i=0; i<(sizeof(ROM::devmap)/sizeof(ROM::DEVIDMAP)); i++) {
		if (ROM::devmap[i].devid == devid) {
			return ROM::devmap[i].regsize;
		}
	}
	return 0;
}

const UINT8 CFITOM::GetVoiceGroupMask(UINT8 devid)
{
	for (int i = 0; i<(sizeof(ROM::devmap) / sizeof(ROM::DEVIDMAP)); i++) {
		if (ROM::devmap[i].devid == devid) {
			return ROM::devmap[i].voicegroup;
		}
	}
	return 0;
}

int CFITOM::InitInstance(TCHAR* profilepath)
{
	lstrcpyn(profile, profilepath, MAX_PATH);
	LoadConfig();
	for (int i=0; i<devs; i++) {
		fmdev[i]->Reset();
		FMVOICE voice;
		GetVoice(&voice, fmdev[i]->GetDevice(), 0, 0);
		for (int j=0; j<fmdev[i]->GetAvailable(); j++) {
			//fmdev[i]->ResetVoice(NULL, NULL, 0);
			fmdev[i]->SetVoice(j, &voice);
		}
	}
	ResetAllCtrl();
	AllNoteOff();
	return 0;
}

CFITOM::CFITOM() : mpus(0), ompus(0), devs(0), MasterVolume(127), redraw(1), timerprocessing(0), pollprocessing(0), timerskipped(0)
{
	fprintf(stderr, _T("FM Instruments Total Operationg Middleware\n\n"));
	for (int i = 0; i < MAX_BANK; i++) {
		opmbank[i] = 0;
		opnabank[i] = 0;
		opl2bank[i] = 0;
		opl3bank[i] = 0;
		opllbank[i] = 0;
		psgbank[i] = 0;
		drummap[i] = 0;
	}
	InitSCCI();
}

CFITOM::~CFITOM()
{
	for (int i = 0; i < MAX_BANK; i++) {
		delete opnabank[i];
		delete opmbank[i];
		delete opl2bank[i];
		delete opl3bank[i];
		delete opllbank[i];
		delete psgbank[i];
		delete drummap[i];
	}
	ExitSCCI();
}

void CFITOM::ExitInstance(int save)
{
	UINT8 i;
	for (i = 0; i < mpus; i++) {
		delete mpu[i];
		delete midi[i];
	}
	for (i = 0; i < devs; i++) {
		delete fmdev[i];
	}
}

CSoundDevice* CFITOM::GetInstDeviceFromID(UINT8 devid)
{
	for (UINT8 i=0; i<devs; i++) {
		if (fmdev[i]->GetDevice() == devid) {
			return fmdev[i];
		}
	}
	if (devid < (sizeof(ROM::compatilist)/sizeof(UINT8*))) {
		UINT8* cmplst = ROM::compatilist[devid];
		for (int k=0; cmplst && cmplst[k] != DEVICE_NONE; k++) {
			for (UINT8 i=0; i<devs; i++) {
				if (fmdev[i]->GetDevice() == cmplst[k]) {
					return fmdev[i];
				}
			}
		}
	}
	return NULL;
}

const int CFITOM::GetInstDeviceIDFromIndex(UINT8 i) const
{
	return (i < devs) ? fmdev[i]->GetDevice() : DEVICE_NONE;
}


int CFITOM::GetInstDeviceIndex(const CSoundDevice* pdev)
{
	for (int i=0; i<devs; i++) {
		if (pdev == fmdev[i]) {
			return i;
		}
	}
	return -1;
}

int CFITOM::GetInstDeviceIndex(UINT8 devid)
{
	for (int i=0; i<devs; i++) {
		if (fmdev[i]->GetDevice() == devid) {
			return i;
		}
	}
	return -1;
}

CFITOM::CVoiceBank* CFITOM::GetVoiceMem(UINT8 voiceid, UINT8 bank)
{
	CVoiceBank** xmem = NULL;
	switch(voiceid) {
	case VOICE_TYPE_OPN:
	case VOICE_TYPE_OPNA:
		xmem = opnabank;
		break;
	case VOICE_TYPE_OPM:
		xmem = opmbank;
		break;
	case VOICE_TYPE_OPL:
	case VOICE_TYPE_OPL2:
		xmem = opl2bank;
		break;
	case VOICE_TYPE_OPLL:
		xmem = opllbank;
		break;
	case VOICE_TYPE_OPL3:
		xmem = opl3bank;
		break;
	case VOICE_TYPE_SSG:
	case VOICE_TYPE_APSG:
	case VOICE_TYPE_DCSG:
		xmem = psgbank;
		break;
	}
	return xmem[bank];
}

int CFITOM::GetVoice(FMVOICE* voice, UINT8 dev, UINT8 bank, UINT8 prog)
{
	CVoiceBank* xmem = NULL;
	UINT8 vtype = GetVoiceType(dev);
	if (bank < MAX_BANK && prog < 128 && (xmem = GetVoiceMem(vtype, bank))) {
		xmem->GetVoice(prog, voice);
	}
	if (xmem == NULL) {
		memset(voice, 0, sizeof(FMVOICE));
		voice->ID = (0xff000000L) | (UINT16(bank) << 8) | prog;
		wsprintf(voice->name, _T("%03i:%03i"), bank, prog);
	}
	return 0;
}

int CFITOM::SetVoice(FMVOICE* voice, UINT8 dev, UINT8 bank, UINT8 prog)
{
	CVoiceBank* xmem = NULL;
	UINT8 vtype = GetVoiceType(dev);
	if (bank < MAX_BANK && prog < 128 && (xmem = GetVoiceMem(vtype, bank))) {
		xmem->SetVoice(prog, voice);
	} else {
		return -1;
	}
	return 0;
}

int CFITOM::GetDrum(DRUMMAP* drum, UINT8 bank, UINT8 prog, UINT8 note)
{
	int ret = -1;
	if (drum) {
		prog &= 0x7f;
		note &= 0x7f;
		if (drummap[prog]) {
			drummap[prog]->GetNote(note, drum);
			ret = 0;
		}
	}
	return ret;
}

const TCHAR* CFITOM::GetBankName(UINT8 devid, UINT8 bank)
{
	bank &= 0x7f;
	if (devid == 0xff) {
		if (drummap[bank] == 0 || drummap[bank]->GetBankName() == 0) {
			return _T("--------");
		}
		return drummap[bank]->GetBankName();
	}
	CVoiceBank* vbank = GetVoiceMem(GetVoiceType(devid), bank);
	return (vbank) ? vbank->GetBankName() : _T("--------");
}

void CFITOM::AllNoteOff()
{
	for (int i=0; i<devs; i++) {
		if (fmdev[i]) {
			fmdev[i]->Reset();
		}
	}
}

void CFITOM::ResetAllCtrl()
{
	for (int i=0; i<mpus; i++) {
		for (int j=0; j<16; j++) {
			CMidiCh* mch = midi[i]->GetMidiCh(j);
			if (mch) {
				mch->ResetAllCtrl();
			}
		}
	}
}

int CFITOM::PollingCallBack()
{
	int i;
	int ret = 0;
	if (!timerprocessing && !pollprocessing) {
		pollprocessing = 1;
		for (i=0; i<mpus; i++) {
			ret |= midi[i]->PollingCallBack();
		}
		for (i=0; i<devs; i++) {
			fmdev[i]->PollingCallBack();
		}
		pollprocessing = 0;
	}
	return ret;
}

void CFITOM::TimerCallBack(UINT32 tick)
{
	if (!timerprocessing) {
		timerprocessing = 1;
		DWORD tfrom = timeGetTime();
		int wait = 0;
		for (wait = 0; pollprocessing; wait++);
#ifdef _DEBUG
		if (wait) {
		//	DWORD delay = timeGetTime() - tfrom;
		//	fprintf(stderr, _T("%i: poller waited for %icounts (%ims)\n"), tick, wait, delay);
		}
#endif
		int i;
		for (i=0; i<mpus; i++) {
			midi[i]->TimerCallBack(tick);
		}
		for (i=0; i<devs; i++) {
			fmdev[i]->TimerCallBack(tick);
		}
		timerprocessing = 0;
	}
	else {
		fprintf(stderr, _T("%i: Interrupt passed\n"), tick);
	}
}
