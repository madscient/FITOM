// CDlgVoicePicker.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "FITOM.h"
#include "SoundDev.h"
#include "MIDIDEV.h"
#include "MIDI.h"
#include "FITOMApp.h"
#include "VoicePicker.h"
#include "afxdialogex.h"


// CdlgCoicePicker ダイアログ

IMPLEMENT_DYNAMIC(CDlgVoicePicker, CDialogEx)

CDlgVoicePicker::CDlgVoicePicker(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgVoicePicker::IDD, pParent), theFitom(CFITOM::GetInstance()),
theDevice(-1), theBank(-1), theProg(-1), bInit(FALSE), pICh(0)
{

}

CDlgVoicePicker::~CDlgVoicePicker()
{
	delete pICh;
}

void CDlgVoicePicker::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEVICE, cmbDevice);
	DDX_Control(pDX, IDC_COMBO_BANK, cmbBank);
	DDX_Control(pDX, IDC_LIST_PROGS, lstProg);
}


BEGIN_MESSAGE_MAP(CDlgVoicePicker, CDialogEx)
	ON_CBN_SELENDOK(IDC_COMBO_DEVICE, &CDlgVoicePicker::OnSelendokComboDevice)
	ON_CBN_SELENDOK(IDC_COMBO_BANK, &CDlgVoicePicker::OnSelendokComboBank)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROGS, &CDlgVoicePicker::OnDblclkListProgs)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PROGS, &CDlgVoicePicker::OnClickListProgs)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CDlgVoicePicker::OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, &CDlgVoicePicker::OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, &CDlgVoicePicker::OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CDlgVoicePicker::OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_DELETE, &CDlgVoicePicker::OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, &CDlgVoicePicker::OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_INSERT, &CDlgVoicePicker::OnEditInsert)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT, &CDlgVoicePicker::OnUpdateEditInsert)
	ON_COMMAND(ID_EDIT_PASTE, &CDlgVoicePicker::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CDlgVoicePicker::OnUpdateEditPaste)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROGS, &CDlgVoicePicker::OnRclickListProgs)
	ON_NOTIFY(NM_RDBLCLK, IDC_LIST_PROGS, &CDlgVoicePicker::OnRdblclkListProgs)
END_MESSAGE_MAP()


// CDlgVoicePicker メッセージ ハンドラー


void CDlgVoicePicker::OnSelendokComboDevice()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	SetDevice(cmbDevice.GetItemData(cmbDevice.GetCurSel()));
	SetBank(0);
	RefreshProg();
}


void CDlgVoicePicker::OnSelendokComboBank()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	SetBank(cmbBank.GetCurSel());
	RefreshProg();
}

void CDlgVoicePicker::SetDevice(int dev)
{
	theDevice = dev;
	if (bInit) {
		RefreshDevice();
	}
}

void CDlgVoicePicker::SetBank(int bank)
{
	theBank = bank;
	if (bInit) {
		RefreshBank();
	}
}

void CDlgVoicePicker::SetProg(int prog)
{
	theProg = prog;
	if (bInit) {
		RefreshProg();
	}
}

BOOL CDlgVoicePicker::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	bInit = TRUE;
	// TODO: ここに初期化を追加してください
	lstProg.SetExtendedStyle(LVS_EX_AUTOSIZECOLUMNS | LVS_EX_LABELTIP | LVS_EX_JUSTIFYCOLUMNS | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT);
	RefreshDevice();
	RefreshBank();
	RefreshProg();
	pICh = new CInstCh(0, theFitom);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CDlgVoicePicker::RefreshDevice()
{
	cmbDevice.ResetContent();
	for (int i = 0; i < theFitom->GetInstDevs(); i++) {
		TCHAR tmp[256];
		CSoundDevice* pDev = theFitom->GetInstDeviceFromIndex(i);
		int devid = pDev->GetDevice();
		wsprintf(tmp, _T("%02X:%s"), devid, theFitom->GetDeviceNameFromID(devid));
		cmbDevice.AddString(tmp);
		cmbDevice.SetItemData(i, devid);
		if (devid == theDevice) {
			cmbDevice.SetCurSel(i);
		}
	}
}

void CDlgVoicePicker::RefreshBank()
{
	cmbBank.ResetContent();
	for (int i = 0; i < 128; i++) {
		TCHAR tmp[256];
		const TCHAR* pBankName = theFitom->GetBankName(theDevice, i);
		if (pBankName) {
			wsprintf(tmp, "%03i:%s", i, pBankName);
			int n = cmbBank.AddString(tmp);
			cmbBank.SetItemData(n, i);
			if (n == theBank) {
				cmbBank.SetCurSel(n);
			}
		}
	}
}

void CDlgVoicePicker::RefreshProg()
{
	lstProg.DeleteAllItems();
	for (int i = 0; i < 128; i++) {
		TCHAR tmp[256];
		TCHAR vname[17];
		FMVOICE voice;
		theFitom->GetVoice(&voice, theDevice, theBank, i);
		lstrcpyn(vname, voice.name, 16);
		vname[16] = _T('\0');
		wsprintf(tmp, "%03i:%s", i, vname);
		lstProg.InsertItem(i, tmp);
		lstProg.EnsureVisible(i, TRUE);
		if (i == theProg) {
			lstProg.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			lstProg.SetSelectionMark(i);
		}
	}
}

void CDlgVoicePicker::OnDblclkListProgs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = 0;
	OnOK();
}


void CDlgVoicePicker::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	theProg = lstProg.GetSelectionMark();
	CDialogEx::OnOK();
}


void CDlgVoicePicker::OnClickListProgs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = 0;
}


void CDlgVoicePicker::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnEditCopy()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnEditCut()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnEditDelete()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnUpdateEditDelete(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnEditInsert()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnUpdateEditInsert(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnEditPaste()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnRclickListProgs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = 0;
}


void CDlgVoicePicker::OnRdblclkListProgs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	theProg = lstProg.GetSelectionMark();
	BeginWaitCursor();
	pICh->BankSelLSB(theDevice);
	pICh->BankSelMSB(theBank);
	pICh->ProgChange(theProg);
	pICh->SetVolume(100);
	pICh->SetExpress(127);
	pICh->SetPanpot(64);
	pICh->NoteOn(64, 127);
	::Sleep(1000);
	pICh->NoteOff(64);
	EndWaitCursor();
	*pResult = 0;
}
