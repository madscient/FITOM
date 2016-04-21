#include "STDAFX.H"
#include "Port.h"

void CDblPort::write(UINT16 reg, UINT16 val, int v)
{
	(reg < 0x100) ? port[0]->write(reg, val, v) : port[1]->write(reg & 0xff, val, v);
}

UINT8 CDblPort::read(UINT16 reg, int v)
{
	return (reg < 0x100) ? port[0]->read(reg, v) : port[1]->read(reg & 0xff, v);
}

UINT8 CDblPort::status()
{
	return port[0]->status();
}

CPort* CDblPort::GetSubPort(int idx)
{
	return port[idx&1];
}

int CDblPort::GetDesc(char* str)
{
	int off = port[0]->GetDesc(str);
	int res = port[1]->GetDesc(&str[off]);
	return strlen(str);
}

CSCCIPort::CSCCIPort() : regsize(0), pInterface(0), pChip(0)
{
}

CSCCIPort::CSCCIPort(SoundInterface* pif, SoundChip* pchip, size_t maxreg) : pInterface(pif), pChip(pchip), regsize(maxreg)
{
	regbak = new UINT8[maxreg];
	::ZeroMemory(regbak, maxreg);
}


void CSCCIPort::write(UINT16 addr, UINT16 data, int v)
{
	if (pChip && addr < regsize) {
		if (v || (data != regbak[addr])) {
			pChip->setRegister(DWORD(addr), DWORD(data));
		}
		regbak[addr] = UINT8(data);
	}
}

UINT8 CSCCIPort::read(UINT16 addr, int v)
{
	if (pChip && addr < regsize) {
		return v ? UINT8(pChip->getRegister(DWORD(addr))) : regbak[addr];
	}
	return 255;
}

UINT8 CSCCIPort::status()
{
	return 0;
}

void CSCCIPort::reset()
{
	if (pChip) {
		pChip->init();
		::ZeroMemory(regbak, regsize);
	}
}

int CSCCIPort::GetDesc(TCHAR* str)
{
	SCCI_SOUND_CHIP_INFO* pci = pChip->getSoundChipInfo();
	wsprintf(str, _T("SCCI:%02i:%02i"), pci->dBusID, pci->dSoundLocation);
	return 0;
}

CDebugPort::CDebugPort() : chipname(0), regsize(0), regbak(0)
{
}

CDebugPort::CDebugPort(const TCHAR* name, size_t maxreg) : chipname(name), regsize(maxreg)
{
	regbak = new UINT8[maxreg];
	::ZeroMemory(regbak, maxreg);
}

UINT8 CDebugPort::status()
{
	return 0;
}

int CDebugPort::GetDesc(TCHAR* str)
{
	wsprintf(str, _T("%s"), chipname);
	return 0;
}

void CDebugPort::write(UINT16 addr, UINT16 data, int v)
{
	TCHAR str[80];
	wsprintf(str, _T("reg %s %03x %02x\n"), chipname, addr, data);
	OutputDebugString(str);
	regbak[addr] = UINT8(data);
}

UINT8 CDebugPort::read(UINT16 addr, int v)
{
	return (addr < regsize) ? regbak[addr] : 0;
}

void CDebugPort::reset()
{
	TCHAR str[80];
	wsprintf(str, _T("reset %s\n"), chipname);
	OutputDebugString(str);
	::ZeroMemory(regbak, regsize);
}