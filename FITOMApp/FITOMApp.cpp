
// FITOMApp.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "FITOMAppDlg.h"
#include "FITOM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFITOMApp

BEGIN_MESSAGE_MAP(CFITOMApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFITOMApp コンストラクション

CFITOMApp::CFITOMApp() : hTimer(0), hPolling(0), theFitom(0), bRunning(FALSE)
{
	// 再起動マネージャーをサポートします
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CFITOMApp オブジェクトです。

CFITOMApp theApp;


// CFITOMApp 初期化

BOOL CFITOMApp::InitInstance()
{
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// ダイアログにシェル ツリー ビューまたはシェル リスト ビュー コントロールが
	// 含まれている場合にシェル マネージャーを作成します。
	CShellManager *pShellManager = new CShellManager;

	// MFC コントロールでテーマを有効にするために、"Windows ネイティブ" のビジュアル マネージャーをアクティブ化
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	SetRegistryKey(_T("FITOM"));
#ifdef _DEBUG
	::AllocConsole();
	::freopen("CONOUT$", "w", stdout);
	::freopen("CONOUT$", "w", stderr);
#endif
	FITOMStart();
	CFITOMAppDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <キャンセル> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: ダイアログの作成に失敗しました。アプリケーションは予期せずに終了します。\n");
		TRACE(traceAppMsg, 0, "警告: ダイアログで MFC コントロールを使用している場合、#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS を指定できません。\n");
	}
	FITOMStop();

#ifdef _DEBUG
	::FreeConsole();
#endif
	// 上で作成されたシェル マネージャーを削除します。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

int CFITOMApp::FITOMStart()
{
	char profilepath[MAX_PATH];
	::GetPrivateProfileString(_T("Profile"), _T("Path"), _T("."), profilepath, MAX_PATH, _T(".\\FITOM.ini"));
	theFitom = CFITOM::GetInstance();
	theFitom->InitInstance(profilepath);
	::timeBeginPeriod(1);
	hTimer = timeSetEvent(10 - 1, 0, TimerProc, (DWORD)this, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
	hPolling = (HANDLE)_beginthreadex(0, 0, PollingProc, (void*)this, 0, 0);
	bRunning = TRUE;
	return 0;
}

int CFITOMApp::FITOMStop()
{
	bRunning = FALSE;
	timeKillEvent(hTimer);
	timeEndPeriod(1);
	::Sleep(100);
	::TerminateThread(hPolling, 0);
	::Sleep(100);
	theFitom->ExitInstance();
	return 0;
}

void CALLBACK CFITOMApp::TimerProc(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dummy1, DWORD dummy2)
{
	CFITOMApp* theFitomApp = (CFITOMApp*)dwUser;
	if (theFitomApp->bRunning && theFitomApp->theFitom) {
		theFitomApp->theFitom->TimerCallBack(theFitomApp->lltickcount);
		theFitomApp->lltickcount++;
	}
}

unsigned int CFITOMApp::PollingProc(void* params)
{
	CFITOMApp* theFitomApp = (CFITOMApp*)params;
	while (theFitomApp->bRunning && theFitomApp->theFitom) {
		theFitomApp->theFitom->PollingCallBack();
		//::Sleep(1);
	}
	return 0;
}

