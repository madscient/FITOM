#include "stdafx.h"
#include "FITOM.h"
#include "MIDIDEV.h"

CPort* CFITOM::CreatePort(int devtype, int fs, TCHAR* params)
{
	CPort* ret = 0;
	TCHAR iftype[16];
	TCHAR portdef[16];
	sscanf(params, _T("%[^:]: %s"), iftype, portdef);
	if (lstrcmp(iftype, _T("SCCI")) == 0) {
		SC_CHIP_TYPE chiptype = GetScChipType(devtype);
		if (lstrcmp(portdef, _T("AUTO")) == 0) {
			SoundChip* chip = pManager->getSoundChip(chiptype, 0);
			if (chip) {
				SCCI_SOUND_CHIP_INFO* sci = chip->getSoundChipInfo();
				SoundInterface* psi = pManager->getInterface(sci->dBusID);
				ret = new CSCCIPort(psi, chip, size_t(GetRegSize(devtype)));
			}
		}
		else {
			int ifid, slid;
			sscanf(portdef, _T("%i : %i"), &ifid, &slid);
			SoundInterface* psi = pManager->getInterface(ifid);
			if (psi) {
				SoundChip* chip = psi->getSoundChip(slid);
				if (chip) {
					ret = new CSCCIPort(psi, chip, size_t(GetRegSize(devtype)));
				}
			}
		}
	}
	if (!ret) {
		ret = new CDebugPort(GetDeviceNameFromID(devtype), size_t(GetRegSize(devtype)));
	}
	return ret;
}

int CFITOM::LoadMidiConfig(FILE* fp)
{
	TCHAR tmp[80];
	CMidiIn* pmi;
	CMidiOut* pmo;
	while(fgets(tmp, 80, fp) != NULL) {

		TCHAR name[80];
		if (strncmp(_T("MCIMIDIIN:"), tmp, 10) == 0) {
			sscanf(&tmp[10], _T("%[^\n]"), name);
			pmi = new CW32MidiIn(name);
			if (pmi) {
				AddDevice(pmi);
				fprintf(stderr, _T("MIDI IN device \"%s\" as port %i...\n"), name, mpus);
			}
		}
		if (strncmp(_T("COMMIDIIN:"), tmp, 10) == 0) {
			sscanf(&tmp[10], _T("%s"), name);
			pmi = new CW32RsMidi(name);
			if (pmi) {
				AddDevice(pmi);
				fprintf(stderr, _T("RS-MIDI UART setting on %s as port %i...\n"), mpus);
			}
		}
		if (strncmp(_T("MCIMIDIOUT:"), tmp, 11) == 0) {
			sscanf(&tmp[11], _T("%[^\n]"), name);
			pmo = new CW32MidiOut(name);
			if (pmo) {
				AddDevice(pmo);
				fprintf(stderr, _T("MIDI OUT device \"%s\" as port %i...\n"), name, mpus);
			}
		}
		if (strncmp(_T("COMMIDIOUT:"), tmp, 10) == 0) {
			sscanf(&tmp[10], _T("%s"), name);
			pmo = new CW32RsMidi(name);
			if (pmo) {
				AddDevice(pmo);
				fprintf(stderr, _T("RS-MIDI UART setting on %s as port %i...\n"), mpus);
			}
		}
		if (strncmp(_T("COMMIDI:"), tmp, 8) == 0) {
			sscanf(&tmp[8], "%s", name);
			pmi = new CW32RsMidi(name);
			if (pmi) {
				AddDevice((CMidiOut*)AddDevice(pmi));
				fprintf(stderr, _T("RS-MIDI UART setting on %s as port %i...\n"), mpus);
			}
		}
	}
	if (!mpus) { return -1; }
	if (!devs) { return -2; }
	return 0;
}

int CFITOM::InitSCCI()
{
	int rescode = 1;
	HMODULE	hScci = NULL;

	// scci.dllの読込み
	if (!(hScci = ::LoadLibrary((LPCSTR)_T("scci.dll")))) {
		fprintf(stderr, _T("Failed to load scci.dll\n"));
		return -1;
	}
	fprintf(stderr, _T("LoadLibrary...OK\n"));

	// サウンドインターフェースマネージャー取得用関数アドレス取得
	SCCIFUNC getSoundInterfaceManager  = (SCCIFUNC)(::GetProcAddress(hScci, _T("getSoundInterfaceManager")));
	if(getSoundInterfaceManager == NULL){
		fprintf(stderr, _T("Failed to get interface\n"));
		return -1;
	}
	fprintf(stderr, _T("GetProcAddress...OK\n"));
	// サウンドインターフェースマネージャー取得
	if (!(pManager = getSoundInterfaceManager())) {
		fprintf(stderr, _T("Failed to get Manager\n"));
		return -1;
	}
	fprintf(stderr, _T("getSoundInterfaceManager...OK\n"));

	// サウンドインターフェースマネージャーインスタンス初期化
	pManager->initializeInstance();

	// リセットを行う
	pManager->init();
	pManager->reset();
	pManager->setLevelDisp(TRUE);

	//pManager->setDelay(0);
	return 0;
}

void CFITOM::ExitSCCI()
{
	if (pManager) {
		// 一括開放する場合（チップ一括開放の場合）
		pManager->releaseAllSoundChip();

		// サウンドインターフェースマネージャーインスタンス開放
		// FreeLibraryを行う前に必ず呼び出ししてください
		pManager->releaseInstance();
	}
}