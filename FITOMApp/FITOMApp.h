
// FITOMApp.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CFITOMApp:
// このクラスの実装については、FITOMApp.cpp を参照してください。
//

class CFITOM;

class CFITOMApp : public CWinApp
{
protected:
	UINT hTimer;
	HANDLE hPolling;
	volatile UINT32 lltickcount;
	volatile BOOL bRunning;
	CFITOM* theFitom;

public:
	CFITOMApp();
	int FITOMStart();
	int FITOMStop();
	CFITOM* GetFitom() { return theFitom; };

//スタティック
	static void CALLBACK TimerProc(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dummy1, DWORD dummy2);
	static unsigned int CALLBACK PollingProc(void* params);

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CFITOMApp theApp;
