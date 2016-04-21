#include "stdafx.h"
#include "FITOM.h"
#include "OPN.h"
#include "OPM.h"
#include "OPL.h"
#include "OPLL.h"
#include "OPK.h"
#include "SSG.h"
#include "MultiDev.h"
#include "MIDIDEV.h"
#include "MIDI.h"

CSoundDevice* CFITOM::AddDevice(CSoundDevice* pdev)
{
	TCHAR tmp1[80], tmp2[80], tmp3[80], tmp4[80];
	if (devs >= MAX_DEVS || !pdev) return 0;
	pdev->GetDescriptor(tmp1);
	if (pdev->GetDevPort()) {
		pdev->GetDevPort()->GetDesc(tmp2);
	}
	for (int i=0; i<devs; i++) {
		if (pdev->GetDevice() == fmdev[i]->GetDevice()) {
			fmdev[i]->GetDescriptor(tmp3);
			CPort* pt = fmdev[i]->GetDevPort();
			if (pt) {
				pt->GetDesc(tmp4);
				fprintf(stderr, _T("Dev.%i: %s+%s (spanned) port=%s%s\n"), i, tmp1, tmp3, tmp2, tmp4);
				CSpanDevice* pspan = new CSpanDevice(pdev, fmdev[i], 0);
				fmdev[i] = pspan;
			} else { //already spanned
				((CSpanDevice*)fmdev[i])->AddDevice(pdev);
			}
			return fmdev[i];
		}
	}
	fprintf(stderr, _T("Dev.%i: %s port=%s\n"), devs, tmp1, tmp2);
	fmdev[devs++] = pdev;
	return pdev;
}

int CFITOM::AddDevice(CMidiIn* pmpu)
{
	int ret = -1;
	if (mpus < MAX_MPUS && pmpu) {
		ret = mpus++;
		mpu[ret] = pmpu;
		midi[ret] = new CMidiInst(pmpu, this);
	}
	return ret;
}

int CFITOM::AddDevice(CMidiOut* pmpu)
{
	int ret = -1;
	if (ompus < MAX_MPUS && pmpu) {
		ret = ompus++;
		ompu[ret] = pmpu;
	}
	return ret;
}

int CFITOM::CreateSingleDevice(int devtype, TCHAR* param)
{
	int res = 0;
	int md, fs;
	TCHAR portparams[80];
	sscanf(param, _T("%i : %i, %s"), &md, &fs, portparams);
	CPort* pt = 0;
	CPort* pt2 = 0;
	if ((pt = CreatePort(devtype, fs, (TCHAR*)portparams))) {
		switch (devtype) {
		case DEVICE_OPN:
			if (!(md & 1)) {
				AddDevice(new COPN(pt, UINT8(fs)));
			}
			if (!(md & 2)) {
				AddDevice(new CSSG(pt, UINT8(fs)));
			}
			break;
		case DEVICE_OPNC:
			if (!(md & 1)) {
				AddDevice(new COPNC(pt, UINT8(fs)));
			}
			if (!(md & 2)) {
				AddDevice(new CSSG(pt, UINT8(fs)));
			}
			break;
		case DEVICE_OPM:
			AddDevice(new COPM(pt, UINT8(fs)));
			break;
		case DEVICE_OPP:
			AddDevice(new COPP(pt, UINT8(fs)));
			break;
		case DEVICE_OPZ:
			AddDevice(new COPZ(pt, UINT8(fs)));
			break;
		case DEVICE_OPN2:
			pt2 = new COffsetPort(pt, 0x100);
			AddDevice(new COPN2(pt, pt2, UINT8(fs)));
			break;
		case DEVICE_OPN2C:
			pt2 = new COffsetPort(pt, 0x100);
			AddDevice(new COPN2C(pt, pt2, UINT8(fs)));
			break;
		case DEVICE_OPN2L:
			pt2 = new COffsetPort(pt, 0x100);
			AddDevice(new COPN2L(pt, pt2, UINT8(fs)));
			break;
		case DEVICE_OPNB:
			pt2 = new COffsetPort(pt, 0x100);
			AddDevice(new COPNB(pt, pt2, UINT8(fs)));
			break;
		case DEVICE_F286:
			pt2 = new COffsetPort(pt, 0x100);
			AddDevice(new CF286(pt, pt2, UINT8(fs)));
			break;
		case DEVICE_2610B:
			pt2 = new COffsetPort(pt, 0x100);
			AddDevice(new C2610B(pt, pt2, UINT8(fs)));
			break;
		case DEVICE_OPNA:
			pt2 = new COffsetPort(pt, 0x100);
			if (!(md & 1)) {
				AddDevice(new COPNA(pt, pt2, UINT8(fs)));
			}
			if (!(md & 2)) {
				AddDevice(new CSSG(pt, UINT8(fs)));
			}
			if (!(md & 4)) {
				//AddDevice(new CAdPcm2608(((CDblPort*)pt)->GetSubPort(1), UINT8(fs)));
			}
			break;
		case DEVICE_OPN3L:
			pt2 = new COffsetPort(pt, 0x100);
			if (!(md & 1)) {
				AddDevice(new COPN3L(pt, pt2, UINT8(fs)));
			}
			if (!(md & 2)) {
				AddDevice(new CSSG(pt, UINT8(fs)));
			}
			break;
		case DEVICE_OPL3:
			pt2 = new COffsetPort(pt, 0x100);
			switch (md) {
			case 0:	// 6op4 + 6op2
				AddDevice(new COPL3(pt, UINT8(fs)));
				AddDevice(new COPL3_2(pt, pt2, 0, UINT8(fs)));
				break;
			case 1:	// 6op4 + 3op2 + 5rhy
				AddDevice(new COPL3(pt, UINT8(fs)));
				AddDevice(new COPL(pt, 3, UINT8(fs)));
				AddDevice(new COPL2(pt2, 2, UINT8(fs)));
				break;
			case 2:	// 6op4 + 4op2 + 4rhy
				AddDevice(new COPL3(pt, UINT8(fs)));
				AddDevice(new COPL3_2(pt, pt2, 1, UINT8(fs)));
				AddDevice(new COPL(pt2, 7, UINT8(fs)));
				break;
			case 3:	// 18op2
				AddDevice(new COPL3_2(pt, pt2, 7, UINT8(fs)));
				break;
			case 4:	// 15op2 + 5rhy
				AddDevice(new COPL3_2(pt, pt2, 2, UINT8(fs)));
				AddDevice(new COPL(pt2, 3, UINT8(fs)));
				break;
			case 5:	//16op2 + 4rhy
				AddDevice(new COPL3_2(pt, pt2, 3, UINT8(fs)));
				AddDevice(new COPL(pt2, 7, UINT8(fs)));
				break;
			}
			break;
		case DEVICE_OPL:
			AddDevice(new COPL(pt, UINT8(md), UINT8(fs)));
			break;
		case DEVICE_Y8950:
			AddDevice(new C3801(pt, UINT8(md), UINT8(fs)));
			break;
		case DEVICE_OPL2:
			AddDevice(new COPL2(pt, UINT8(md), UINT8(fs)));
			break;
		case DEVICE_OPLL:
			AddDevice(new COPLL(pt, UINT8(md), UINT8(fs)));
			break;
		case DEVICE_OPLL2:
			AddDevice(new COPLL2(pt, UINT8(md), UINT8(fs)));
			break;
		case DEVICE_OPLLP:
			AddDevice(new COPLLP(pt, UINT8(md), UINT8(fs)));
			break;
		case DEVICE_OPLLX:
			AddDevice(new COPLLX(pt, UINT8(md), UINT8(fs)));
			break;
		case DEVICE_OPK:
			AddDevice(new COPK(pt, UINT8(md), UINT8(fs)));
			break;
		case DEVICE_OPK2:
			AddDevice(new COPK2(pt, UINT8(md), UINT8(fs)));
			break;
		case DEVICE_DSG:
			AddDevice(new CDSG(new COffsetPort(pt, 0x80)));
			break;
		case DEVICE_SSG:
			AddDevice(new CSSG(pt, UINT8(fs)));
			break;
		case DEVICE_APSG:
			AddDevice(new CAPSG(pt, UINT8(fs)));
			break;
		case DEVICE_DCSG:
			AddDevice(new CDCSG(pt, UINT8(fs)));
			break;
		default:
			delete pt;
		}
	}
	return res;
}

int CFITOM::CreateDualDevice(int devtype, TCHAR* param)
{
	int res = 0;
#if 0
	CPort* pt1 = 0;
	CPort* pt2 = 0;
	CSoundDevice* dev1 = 0;
	CSoundDevice* dev2 = 0;
	int md, fs;
	char portpara1[80];
	char portpara2[80];
	sscanf(param, "%i : %i, %[^,],%s", &md, &fs, portpara1, portpara2);
	pt1 = CreatePort(portpara1);
	pt2 = CreatePort(portpara2);
	while (pt1&&pt2) {
		switch (devtype) {
		case DEVICE_OPN:
			if (!(md & 1)) {
				dev1 = new COPN(pt1, UINT8(fs));
				dev2 = new COPN(pt2, UINT8(fs));
			}
			if (!(md & 2)) {
				devtype = DEVICE_SSG;
			} else {
				pt1 = pt2 = 0;
			}
			break;
		case DEVICE_OPNC:
			if (!(md & 1)) {
				dev1 = new COPNC(pt1, UINT8(fs));
				dev2 = new COPNC(pt2, UINT8(fs));
			}
			if (!(md & 2)) {
				devtype = DEVICE_SSG;
			} else {
				pt1 = pt2 = 0;
			}
			break;
		case DEVICE_OPL:
			dev1 = new COPL(pt1, UINT8(md), UINT8(fs));
			dev2 = new COPL(pt2, UINT8(md), UINT8(fs));
			pt1 = pt2 = 0;
			break;
		case DEVICE_Y8950:
			dev1 = new C3801(pt1, UINT8(md), UINT8(fs));
			dev2 = new C3801(pt2, UINT8(md), UINT8(fs));
			pt1 = pt2 = 0;
			break;
		case DEVICE_OPL2:
			dev1 = new COPL2(pt1, UINT8(md), UINT8(fs));
			dev2 = new COPL2(pt2, UINT8(md), UINT8(fs));
			pt1 = pt2 = 0;
			break;
		case DEVICE_OPLL:
			dev1 = new COPLL(pt1, UINT8(md), UINT8(fs));
			dev2 = new COPLL(pt2, UINT8(md), UINT8(fs));
			pt1 = pt2 = 0;
			break;
		case DEVICE_OPLL2:
			dev1 = new COPLL2(pt1, UINT8(md), UINT8(fs));
			dev2 = new COPLL2(pt2, UINT8(md), UINT8(fs));
			pt1 = pt2 = 0;
			break;
		case DEVICE_OPLLP:
			dev1 = new COPLLP(pt1, UINT8(md), UINT8(fs));
			dev2 = new COPLLP(pt2, UINT8(md), UINT8(fs));
			pt1 = pt2 = 0;
			break;
		case DEVICE_OPLLX:
			dev1 = new COPLLX(pt1, UINT8(md), UINT8(fs));
			dev2 = new COPLLX(pt2, UINT8(md), UINT8(fs));
			pt1 = pt2 = 0;
			break;
		case DEVICE_SSG:
			dev1 = new CSSG(pt1, UINT8(fs));
			dev2 = new CSSG(pt2, UINT8(fs));
			pt1 = pt2 = 0;
			break;
		}
		if (dev1 && dev2) {
			AddDevice(new CSplitPan(dev1, dev2, UINT8(fs)));
		}
	}
#endif
	return res;
}

int CFITOM::LoadDeviceConfig(FILE* fp)
{
	TCHAR tmp[80];
	while(fgets(tmp, 80, fp) != NULL) {
		TCHAR devtyp[16];
		TCHAR params[80];

		if (strncmp(_T("DUAL_"), devtyp, 5) == 0) {
			sscanf(&tmp[5], "%[^:]: %s", devtyp, params);
		} else {
			sscanf(tmp, _T("%[^:]: %s"), devtyp, params);
			int devnum = GetDeviceIDFromName((TCHAR*)devtyp);
			CreateSingleDevice(devnum, params);
		}
	}
	if (!devs) { return -2; }
	return 0;
}

int CFITOM::LoadMapConfig(FILE* fp)
{
	int ret = 0;
	TCHAR tmp[80];
	int inst, ch, timbres;
	TCHAR dev[10];
	inst = -1;
	if (mpus) {
		while(fgets(tmp, 80, fp) != NULL) {
			sscanf(tmp, _T("%i : %[^,\n] , %i"), &ch, dev, &timbres);
			ch--;
			inst = ch >> 4;
			ch = ch & 0xf;
			if (inst < mpus) {
				if (strcmp(dev, _T("RHYTHM"))==0) {
					midi[inst]->AssignRhythm(ch);
				} else {
					CSoundDevice* chip = GetInstDeviceFromID(GetDeviceIDFromName(dev));
					if (chip != NULL && timbres <= chip->GetChs()) {
						midi[inst]->Assign(ch, chip, timbres);
						fprintf(stderr, _T("port%i: ch.%i: %s(%i)\n"), inst, ch, dev, timbres);
					}
				}
			}
		}
	}
	return ret;
}

int CFITOM::LoadVoiceConfig(FILE* fp)
{
	int ret = 0;
	TCHAR tmp[512];
	TCHAR filename[MAX_PATH];
	TCHAR bankname[80];
	TCHAR devname[80];
	int bank;
	while(fgets(tmp, 80, fp) != NULL) {
		sscanf(tmp, _T(" %[^:] : %i : %[^:,\n] , %[^\n]"), devname, &bank, filename, bankname);
		UINT8 vtype = GetVoiceType(GetDeviceIDFromName(devname));
		switch (vtype) {
		case VOICE_TYPE_OPN:
		case VOICE_TYPE_OPNA:
			if (bank < MAX_BANK) {
				fprintf(stderr, _T("Loading OPN/OPN2/OPNA voice bank \"%s\"\n"), filename);
				LoadVoice(vtype, bank, opnabank, filename, bankname);
			}
			break;
		case VOICE_TYPE_OPM:
			if (bank < MAX_BANK) {
				fprintf(stderr, _T("Loading OPM voice bank \"%s\"\n"), filename);
				LoadVoice(vtype, bank, opmbank, filename, bankname);
			}
			break;
		case VOICE_TYPE_OPL3:
			if (bank < MAX_BANK) {
				fprintf(stderr, _T("Loading OPL3(4op) voice bank \"%s\"\n"), filename);
				LoadVoice(vtype, bank, opl3bank, filename, bankname);
			}
			break;
		case VOICE_TYPE_OPL2:
			if (bank < MAX_BANK) {
				fprintf(stderr, _T("Loading OPL/OPL2 voice bank \"%s\"\n"), filename);
				LoadVoice(vtype, bank, opl2bank, filename, bankname);
			}
			break;
		case VOICE_TYPE_OPLL:
			if (bank < MAX_BANK) {
				fprintf(stderr, _T("Loading OPLL voice bank \"%s\"\n"), filename);
				LoadVoice(vtype, bank, opllbank, filename, bankname);
			}
			break;
		case VOICE_TYPE_SSG:
			if (bank == 0) {
				fprintf(stderr, "Loading PSG voice bank \"%s\"\n", filename);
				LoadVoice(vtype, bank, psgbank, filename, bankname);
			}
			break;
		default:
			if ((lstrcmp(_T("SSGDRUM:"), devname) == 0 ||
				lstrcmp(_T("PSGDRUM"), devname) == 0)) {
			}
			if (lstrcmp(_T("RHYTHM"), devname) == 0) {
				fprintf(stderr, _T("Loading Rhythm bank \"%s\"\n"), filename);
				LoadDrumMap(bank, filename, bankname);
			}
			break;
		}
	}
	return ret;
}

int CFITOM::LoadVoice(int group, int bank, CVoiceBank* voice[], TCHAR* filename, TCHAR* bankname)
{
	int ret = 0;
	if (bank < MAX_BANK && bank >= 0) {
		if (voice[bank] == 0) {
			voice[bank] = new CVoiceBank(filename, bankname);
		}
		else {
			voice[bank]->LoadBank(filename, bankname);
		}
		voice[bank]->Renumber(group, bank);
	}
	 else {
		fprintf(stderr, _T("Error loading \"%s\"\n"), filename);
		ret = -1;
	}
	return ret;
}

int CFITOM::SaveVoice(int bank, CVoiceBank* voice, TCHAR* filename)
{
	int ret = 0;
	if (voice) {
		voice->SaveBank(filename);
	} else {
		fprintf(stderr, _T("Error saving \"%s\"\n"), filename);
		ret = -1;
	}
	return ret;
}

int CFITOM::SaveVoice(UINT8 dev, UINT8 bank, TCHAR* filename)
{
	CVoiceBank* xmem = NULL;
	UINT8 vtype = GetVoiceType(dev);
	if (bank < MAX_BANK && (xmem = GetVoiceMem(vtype, bank))) {
		xmem->SaveBank(filename);
		fprintf(stderr, _T("Bank save: dev=%02X, bank=%i, filename=\"%s\"\n"), dev, bank, filename);
	}
	else {
		return -1;
	}
	return 0;
}

int CFITOM::InitVoice(int type, int banks, CVoiceBank* voice)
{
	int ret = 0;
	FMVOICE buf;
	memset((void*)&buf, 0, sizeof(FMVOICE));
	for (int j=0; j<banks; j++) {
		for (int i=0; i<128; i++) {
			buf.ID = UINT32(0xff000000L) | UINT32(j << 8) | i;
			voice[j].SetVoice(i, &buf);
		}
	}
	return ret;
}

int CFITOM::LoadDrumMap(int mapnum, TCHAR* filename, TCHAR*mapname)
{
	int ret = 0;
	if (mapnum < 128 && mapnum >= 0) {
		if (drummap[mapnum] == 0) {
			drummap[mapnum] = new CDrumBank(filename, mapname);
		}
		else {
			drummap[mapnum]->LoadBank(filename, mapname);
		}
	} else {
		fprintf(stderr, _T("Error loading \"%s\"\n"), filename);
		ret = -1;
	}
	return ret;
}

int CFITOM::LoadConfig()
{
	int ret = 0;
	FILE* cfg = NULL;
	TCHAR tmp[MAX_PATH];
	wsprintf(tmp, _T("%s\\DEVICE.CFG"), profile);
	if ((cfg=fopen(tmp, "rt"))) {
		fprintf(stderr, _T("Loading device settings...\n"));
		ret = LoadDeviceConfig(cfg);
		fclose(cfg);
	}
	else {
		fprintf(stderr, _T("Failed to open '%s'\n"), tmp);
	}
	wsprintf(tmp, _T("%s\\MIDIIF.CFG"), profile);
	if ((cfg = fopen(tmp, "rt"))) {
		fprintf(stderr, _T("Loading device settings...\n"));
		ret = LoadMidiConfig(cfg);
		fclose(cfg);
	}
	else {
		fprintf(stderr, _T("Failed to open '%s'\n"), tmp);
	}
	wsprintf(tmp, _T("%s\\MIDIMAP.CFG"), profile);
	if ((cfg = fopen(tmp, _T("rt")))) {
		fprintf(stderr, _T("Loading MIDI channel map settings...\n"));
		ret = LoadMapConfig(cfg);
		fclose(cfg);
	}
	else {
		fprintf(stderr, _T("Failed to open '%s'\n"), tmp);
	}
	wsprintf(tmp, _T("%s\\VOICEMAP.CFG"), profile);
	if ((cfg = fopen(tmp, _T("rt")))) {
		fprintf(stderr, _T("Loading preset voice settings...\n"));
		ret = LoadVoiceConfig(cfg);
		fclose(cfg);
	}
	else {
		fprintf(stderr, _T("Failed to open '%s'\n"), tmp);
	}

	return ret;
}
