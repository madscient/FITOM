#ifndef __ADPCM_H__
#define __ADPCM_H__

#include "SoundDev.h"

class CAdPcmBase : public CSoundDevice
{
protected:
	struct ADPCMVOICE {
		UINT32 voiceID;	//Voice ID
		UINT32 staddr;	//Start address
		UINT32 length;	//length
		UINT32 loopstart;
		UINT32 loopend;
		UINT16 prescl;	//prescalar
		UINT16 deltan;	//delta N
		UINT16 freq;	//Original frequency
		UINT16 rate;	//sampling rate
	} adpcmvoice[128];
	UINT32 maxmem;
	UINT32 usedmem;
	UINT32 nextst;
public:
	CAdPcmBase(CPort* pt, UINT32 fsamp, UINT16 memsize, UINT16 ext);
	virtual void LoadVoice(UINT8 num, UINT16 rate, UINT8* data, UINT32 len, UINT8 rom=0, UINT8 inst=0);
};

#if 0
class CAdPcm3801 : public CAdPcmBase	//Y8950
{
};

class CAdPcm2608 : public CAdPcmBase	//YM2608
{
};

class CAdPcm2610A : public CAdPcmBase	//YM2610(B)
{
};

class CAdPcm2610B : public CAdPcmBase	//YM2610(B)
{
};

class CAdPcm263 : public CAdPcmBase	//YMZ263
{
};

class CAdPcm705 : public CAdPcmBase	//YMZ705
{
};

class CAdPcm280 : public CAdPcmBase //YMZ280
{
};

#endif

#endif
