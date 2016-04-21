#include "stdafx.h"
#include "FITOM.h"
#include "SoundDev.h"

CFITOM::CDrumBank::CDrumBank() : drummap(0), mapname(0), filename(0)
{
}

CFITOM::CDrumBank::CDrumBank(const char* fname, const char* bname) : CDrumBank()
{
	LoadBank(fname, bname);
}

CFITOM::CDrumBank::~CDrumBank()
{
	if (filename) {
		delete[] filename;
	}
	if (mapname) {
		delete[] mapname;
	}
	if (drummap) {
		delete[] drummap;
	}
}

int CFITOM::CDrumBank::LoadBank(const char* fname, const char* bname)
{
	if (fname) {
		if (filename) {
			delete[] filename;
		}
		filename = new char[lstrlen(fname) + 1];
		lstrcpy(filename, fname);
	}
	if (bname) {
		if (mapname) {
			delete[] mapname;
		}
		mapname = new char[lstrlen(bname) + 1];
		lstrcpy(mapname, bname);
	}
	if (drummap) {
		delete[] drummap;
	}
	drummap = new DRUMMAP[128];
	::ZeroMemory((void*)drummap, sizeof(DRUMMAP) * 128);

	int ret = 0;
	FILE* fp = NULL;
	char tmp[80];
	if ((fp = fopen(filename, "rb"))) {
		while (fgets(tmp, 80, fp) != NULL) {
			if (tmp[0] == '@') {
				if (mapname) {
					delete[] mapname;
				}
				mapname = new char[lstrlen(&tmp[1])];
				lstrcpy(mapname, &tmp[1]);
			}
			else if (tmp[0] != '\n') {
				int note;
				int pan;
				int gate;
				int bank;
				int prog;
				int num;
				int fnum;
				char devname[80];
				char notename[80];
				char strnote[8];
				sscanf(tmp, " %i , %[^,], %[^,], %i, %i, %[^,] , %i , %i ", &note, notename, devname, &bank, &prog, strnote, &pan, &gate);
				if (note >= 0 && note < 128) {
					if (strnote[0] == '#') {
						sscanf(&strnote[1], "%i:%x", &num, &fnum);
						num |= 0x80;
					}
					else {
						sscanf(strnote, "%i", &num);
						fnum = 0;
					}
					lstrcpyn((char*)&drummap[note].name, notename, 16);
					drummap[note].devID = GetDeviceIDFromName(devname);
					drummap[note].device = GetInstance()->GetInstDeviceFromID(drummap[note].devID);
					drummap[note].bank = UINT8(bank);
					drummap[note].prog = UINT8(prog);
					drummap[note].pan = SINT8(pan);
					drummap[note].gate = UINT16(gate);
				}
			}
		}
		fclose(fp);
	}
	else {
		fprintf(stderr, "Error loading \"%s\"\n", filename);
		ret = -1;
	}
	return ret;
}

void CFITOM::CDrumBank::GetNote(int num, CFITOM::DRUMMAP* note)
{
	if (num < 128 && num >= 0 && drummap != 0) {
		*note = drummap[num];
	}
}

void CFITOM::CDrumBank::SetNote(int num, const CFITOM::DRUMMAP* note)
{
	if (num < 128 && num >= 0 && drummap != 0) {
		drummap[num] = *note;
	}
}

int CFITOM::CDrumBank::SaveBank(const char* fname)
{
	int ret = -1;
	const char* filepath = (fname != 0) ? fname : filename;
	FILE* fp;
	if (drummap && filepath && (fp=fopen(filepath, _T("wb")))) {
		fputc('@', fp);
		fputs(mapname, fp);
		fputc('\n', fp);
		for (int i = 0; i < 128; i++) {
			if (drummap[i].device != 0){
				char tmp[256];
				char note[20];
				if (drummap[i].num & 0x80) {
					if (drummap[i].fnum) {
						wsprintf(note, "%#%i:%04X", drummap[i].num & 0x7f, drummap[i].fnum);
					}
					else {
						wsprintf(note, "%#%i", drummap[i].num & 0x7f);
					}
				}
				else {
					wsprintf(note, "%i", drummap[i].num);
				}
				wsprintf(tmp, "%i,%s,%s,%i,%i,%s,%i,%i\n",
					i, drummap[i].name, GetDeviceNameFromID(drummap[i].devID),
					drummap[i].bank, drummap[i].prog, note, drummap[i].pan, drummap[i].gate);
			}
		}
		fclose(fp);
		ret = 0;
	}
	return ret;
}

//---------------------------------------

CFITOM::CVoiceBank::CVoiceBank() : vbank(0), bankname(0), filename(0)
{
}

CFITOM::CVoiceBank::CVoiceBank(const char* fname, const char* bname) : CFITOM::CVoiceBank()
{
	LoadBank(fname, bname);
}

CFITOM::CVoiceBank::~CVoiceBank()
{
	if (vbank) {
		delete[] vbank;
	}
	if (bankname) {
		delete[] bankname;
	}
	if (filename) {
		delete[] filename;
	}
}

void CFITOM::CVoiceBank::SetVoice(int prog, const FMVOICE* voice)
{
	if (vbank && prog >= 0 && prog < 128) {
		vbank[prog] = *voice;
	}
}

void CFITOM::CVoiceBank::GetVoice(int prog, FMVOICE* voice)
{
	if (vbank && prog >= 0 && prog < 128) {
		*voice = vbank[prog];
	}
}

int CFITOM::CVoiceBank::LoadBank(const char* fname, const char* bname)
{
	if (fname) {
		if (filename) {
			delete[] filename;
		}
		filename = new char[lstrlen(fname) + 1];
		lstrcpy(filename, fname);
	}
	if (bname) {
		if (bankname) {
			delete[] bankname;
		}
		bankname = new char[lstrlen(bname) + 1];
		lstrcpy(bankname, bname);
	}
	if (vbank) {
		delete[] vbank;
	}
	vbank = new FMVOICE[128];
	::ZeroMemory((void*)vbank, sizeof(FMVOICE) * 128);

	int ret = 0;
	FILE* fp = NULL;
	if ((fp = fopen(filename, "rb"))) {
		fread((void*)vbank, sizeof(FMVOICE), 128, fp);
		fclose(fp);
	}
	else {
		fprintf(stderr, "Error loading \"%s\"\n", filename);
		ret = -1;
	}
	return ret;
}

int CFITOM::CVoiceBank::SaveBank(const char* fname)
{
	int ret = 0;
	const char* filepath = (fname != 0) ? fname : filename;
	FILE* fp;
	if (vbank && filepath && (fp=fopen(filepath, _T("wb")))) {
		fwrite((void*)vbank, sizeof(FMVOICE), 128, fp);
		fclose(fp);
	}
	else {
		ret = -1;
	}
	return ret;
}

void CFITOM::CVoiceBank::Renumber(int grp, int bnk)
{
	if (vbank) {
		for (int i = 0; i < 128; i++) {
			vbank[i].ID = ((grp & 0xff) << 24) | ((bnk & 0xff) << 8) | (i & 0xff);
		}
	}
}