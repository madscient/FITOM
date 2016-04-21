#ifndef __FMIDI98_H__
#define __FMIDI98_H__

//Resource code
#define	DEVICE_NONE	0
#define	DEVICE_SSG	1	//YM2149
#define	DEVICE_OPN	2	//YM2203
#define	DEVICE_OPN2	3	//YM2612
#define	DEVICE_OPNA	4	//YM2608
#define	DEVICE_OPM	5	//YM2151
#define	DEVICE_OPLL	6	//YM2413
#define	DEVICE_OPL	7	//YM3526
#define	DEVICE_OPL2	8	//YM3812
#define	DEVICE_OPL3	9	//YMF262
#define DEVICE_OPN3L 10	//YMF288
#define DEVICE_OPNB 11	//YM2610
#define DEVICE_SAA	12	//SAA1099
#define DEVICE_DSG	13	//YM2163
#define	DEVICE_PSG	15	//AY-3-891x
#define	DEVICE_DCSG	16	//SN76489
#define	DEVICE_SCC	17	//SCC with ROM
#define DEVICE_SCCP	18	//SCC for SNATCHER
#define	DEVICE_SSGS	19	//YMZ705
#define	DEVICE_APSG	20	//AY8930
#define DEVICE_SSGL 21	//YMZ284
#define DEVICE_SSGLP 22	//YMZ294
#define DEVICE_SID	23	//MOS6581/8580
#define	DEVICE_Y8950	24	//YM3801 a.k.a. MSX-AUDIO
#define	DEVICE_OPL3_2	25	//2op ch of OPL3
#define	DEVICE_OPP	26	//YM2164
#define	DEVICE_OPZ	27	//YM2414
#define	DEVICE_OPZ2	28	//YM2424
#define	DEVICE_OPLLP	29	//YMF281
#define	DEVICE_OPLL2	30	//YM2420
#define DEVICE_OPNC 31	//YMF264
#define DEVICE_OPN2C 32	//YM3438
#define DEVICE_OPN2L 33	//YMF276
#define DEVICE_2610B 34	//YM2610B
#define DEVICE_F286 35	//YMF286-K
#define DEVICE_OPN3 36	//YMF297
#define DEVICE_OPN3_L3 37	//OPL mode of YMF297
#define DEVICE_OPN3_N3 38	//OPN mode of YMF297
#define DEVICE_OPLLX	39	//YM2423/MS1823
#define DEVICE_OPK 40	//YM7116
#define DEVICE_OPK2 41	//YM7129
#define DEVICE_OPQ	42	//YM3806/YM3533
#define DEVICE_RYP4	43	//YM2154
#define DEVICE_RYP6	44	//YM3301/YM3302
#define DEVICE_FMS	45	//YMZ735/YMZ736
#define DEVICE_5232	47	//MSM5232
#define DEVICE_PCMD8	64	//YMZ280

#define	DEVICE_ADPCM	0x101
#define	DEVICE_ADPCMA	0x102
#define	DEVICE_ADPCMB	0x103

#define VOICE_TYPE_OPM  0x10
#define VOICE_TYPE_OPN  0x11
#define VOICE_TYPE_OPNA	 0x12
#define VOICE_TYPE_OPZ  0x13

#define VOICE_TYPE_OPL2 0x20
#define VOICE_TYPE_OPL  0x21
#define VOICE_TYPE_OPLL 0x22
#define VOICE_TYPE_OPK	0x23

#define VOICE_TYPE_OPL3 0x30
#define VOICE_TYPE_MA_3	0x31

#define VOICE_TYPE_SSG  0x40
#define VOICE_TYPE_APSG 0x41
#define VOICE_TYPE_DCSG 0x42
#define VOICE_TYPE_DSG	0x43
#define VOICE_TYPE_SAA	0x44
#define VOICE_TYPE_SCC	0x48

#define VOICE_TYPE_DRUM 0x50

#define VOICE_GROUP_NONE 0x00
#define VOICE_GROUP_OPM  0x01
#define VOICE_GROUP_OPNA 0x02
#define VOICE_GROUP_OPL2 0x04
#define VOICE_GROUP_OPL3 0x08
#define VOICE_GROUP_OPLL 0x10
#define VOICE_GROUP_PSG  0x20
#define VOICE_GROUP_ALL  0xff

#define DEVICE_CAP_NONE 0x00
#define DEVICE_CAP_FM   0x01
#define DEVICE_CAP_PSG  0x02
#define DEVICE_CAP_RHY  0x04
#define DEVICE_CAP_PCM  0x08

#define MAX_BANK 64
#define MAX_MPUS 4
#define MAX_DEVS 64
#define MAX_IOIF 16
#define MAX_SCREEN 16

class CSoundDevice;
class CMidiInst;
class CMidiIn;
class CMidiOut;
class IScreen;

#include "Port.h"

struct FMVOICE;

class CFITOM {
public:
	struct DRUMMAP {
		CSoundDevice* device;
		TCHAR name[16];
		UINT8 devID;
		UINT8 bank;
		UINT8 prog;
		SINT8 pan;
		UINT8 num;
		UINT8 alt[3];
		UINT16 fnum;
		UINT16 gate;
	};
	class CDrumBank {
	protected:
		DRUMMAP* drummap;
		TCHAR* mapname;
		TCHAR* filename;
	public:
		CDrumBank();
		CDrumBank(const TCHAR* fname, const TCHAR* bname);
		~CDrumBank();
		void SetNote(int num, const DRUMMAP* map);
		void GetNote(int num, DRUMMAP* map);
		int LoadBank(const TCHAR* fname, const TCHAR* bname);
		int SaveBank(const TCHAR* fname = 0);
		const TCHAR* GetBankName() { return (const TCHAR*)mapname; };
		const TCHAR* GetFileName() { return (const TCHAR*)filename; };
	};
	class CVoiceBank {
	protected:
		FMVOICE* vbank;
		TCHAR* bankname;
		TCHAR* filename;
	public:
		CVoiceBank();
		CVoiceBank(const TCHAR* fname, const TCHAR* bname);
		~CVoiceBank();
		int LoadBank(const TCHAR* fname, const TCHAR* bname);
		int SaveBank(const TCHAR* fname = 0);
		const TCHAR* GetBankName() { return (const TCHAR*)bankname; };
		const TCHAR* GetFileName() { return (const TCHAR*)filename; };
		void GetVoice(int prog, FMVOICE* voice);
		void SetVoice(int prog, const FMVOICE* voice);
		void Renumber(int grp, int bnk);
	};
protected:
	CSoundDevice* fmdev[MAX_DEVS];
	CMidiIn* mpu[MAX_MPUS];
	CMidiOut* ompu[MAX_MPUS];
	CMidiInst* midi[MAX_MPUS];
	UINT8 devs;
	UINT8 mpus;
	UINT8 ompus;
	UINT8 ioifs;
 	UINT16 MasterVolume;
	volatile int redraw;
	volatile int timerprocessing;
	volatile int pollprocessing;
	UINT16 timerskipped;
	TCHAR profile[MAX_PATH];
	SoundInterfaceManager* pManager;

	//Configuration
	CPort* CreatePort(int devtype, int fs, TCHAR* params);
	int CreateSingleDevice(int devtype, TCHAR* param);
	int CreateDualDevice(int devtype, TCHAR* param);
	int LoadConfig();
	int LoadDeviceConfig(FILE* fp);
	int LoadMidiConfig(FILE* fp);
	int LoadVoiceConfig(FILE* fp);
	int LoadMapConfig(FILE* fp);
	int LoadVoice(int group, int bank, CVoiceBank* voice[], TCHAR* filename, TCHAR* bankname);
	int LoadDrumMap(int bank, TCHAR*filename, TCHAR* bankname);

	//Device management
	CSoundDevice* AddDevice(CSoundDevice* pdev);
	int AddDevice(CMidiIn* pmpu);
	int AddDevice(CMidiOut* pmpu);
	int InitSCCI();
	void ExitSCCI();
	void ResetSCCI();

	//Data management
	CVoiceBank* opmbank[MAX_BANK];	//OPM/OPP/OPZ
	CVoiceBank* opnabank[MAX_BANK];	//OPN/OPNA/OPNB/OPN2/OPN3
	CVoiceBank* opl2bank[MAX_BANK];	//OPL/OPL2/Y8950
	CVoiceBank* opl3bank[MAX_BANK];	//OPL3/MA-x
	CVoiceBank* opllbank[MAX_BANK];	//OPLL/OPLL2/OPLLP/OPLLX
	CVoiceBank* psgbank[MAX_BANK];	//PSG/APSG/SSG/SSGS/SSGL/DSG/DCSG
	CDrumBank* drummap[MAX_BANK];
public:
	//Initialize and Finalize
	static CFITOM* GetInstance() {
		static CFITOM theInstance;
		return & theInstance;
	};
	int InitInstance(TCHAR* profilepath);
	void ExitInstance(int save=0);

	//Query
	int GetVoice(FMVOICE* voice, UINT8 dev, UINT8 bank, UINT8 num);
	int GetDrum(DRUMMAP* drum, UINT8 bank, UINT8 prog, UINT8 note);
	const UINT8 GetMidiInputs() const { return mpus; };
	const UINT8 GetMidiOutputs() const { return ompus; };
	const UINT8 GetInstDevs() const { return devs; };
	CMidiIn* GetMidiIn(UINT8 i) const { return (i<mpus) ? mpu[i] : NULL; };
	CMidiOut* GetMidiOut(UINT8 i) const { return (i<ompus) ? ompu[i] : NULL; };
	CMidiInst* GetMidiInst(UINT8 i) const { return (i<mpus) ? midi[i] : NULL; };
	CSoundDevice* GetInstDeviceFromIndex(UINT8 i) const { return (i<devs) ? fmdev[i] : NULL; };
	CSoundDevice* GetInstDeviceFromID(UINT8 devid);
	const int GetInstDeviceIDFromIndex(UINT8 i) const;
	int GetInstDeviceIndex(const CSoundDevice* pdev);
	int GetInstDeviceIndex(UINT8 devid);
	CVoiceBank* GetVoiceMem(UINT8 voiceid, UINT8 bank);
	const TCHAR* GetBankName(UINT8 devid, UINT8 bank);

	int SetVoice(FMVOICE* voice, UINT8 dev, UINT8 bank, UINT8 num);
	int SaveVoice(UINT8 dev, UINT8 bank, TCHAR* filename=0);
	int SaveVoice(int bank, CVoiceBank* voice, TCHAR* filename);
	int SaveDrum(int prog, CDrumBank* drumset, TCHAR* filename);
	int InitVoice(int type, int banks, CVoiceBank* voice);

	//Callback
	int PollingCallBack();
	void TimerCallBack(UINT32 tick);

	//Utility
	static const UINT8 GetDeviceIDFromName(TCHAR* name);
	static const TCHAR* GetDeviceNameFromID(UINT8 devid);
	static const UINT8 GetVoiceType(UINT8 device);
	static const UINT8 GetVoiceGroupMask(UINT8 device);
	static const SC_CHIP_TYPE GetScChipType(UINT8 devid);
	static const DWORD GetRegSize(UINT8 devid);
	void AllNoteOff();
	void ResetAllCtrl();
private:
	CFITOM();
	CFITOM& operator=(CFITOM& rhs);
	CFITOM(CFITOM& rhs);
	~CFITOM();
};

#endif //__FMIDI98_H__
