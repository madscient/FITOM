// MidiChCtrlDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOM.h"
#include "SoundDev.h"
#include "MIDIDEV.h"
#include "MIDI.h"
#include "FITOMApp.h"
#include "MidiChCtrlDlg.h"
#include "afxdialogex.h"
#include "VoicePicker.h"
#include "VoiceEditDlg.h"

// CMidiChCtrlDlg ダイアログ

CMidiChCtrlDlg::CMultiCtrl::CMultiCtrl() : value(-1), pGetter(0), pSetter(0)
{

}

CMidiChCtrlDlg::CMultiCtrl::CMultiCtrl
(CMidiChCtrlDlg* par, int idedt, int idspn, int idsld, int(CMidiChCtrlDlg::*pget)(), void(CMidiChCtrlDlg::*pset)(int), int min, int max)
{
	Attach(par, idedt, idspn, idsld, pget, pset);
	SetRange(min, max);
}

void CMidiChCtrlDlg::CMultiCtrl::Attach(CMidiChCtrlDlg* par, int idedt, int idspn, int idsld, int(CMidiChCtrlDlg::*pget)(), void(CMidiChCtrlDlg::*pset)(int))
{
	pParent = par;
	Edit.SubclassDlgItem(idedt, pParent);
	Spin.SubclassDlgItem(idspn, pParent);
	Slider.SubclassDlgItem(idsld, pParent);
	Spin.SetBase(10);
	Spin.SetBuddy(&Edit);
	pGetter = pget;
	pSetter = pset;
}

void CMidiChCtrlDlg::CMultiCtrl::SetValue()
{
	if (pGetter) {
		int val = (pParent->*pGetter)();
		if (value != val) {
			TCHAR tmp[32];
			wsprintf(tmp, _T("%i"), val);
			value = val;
			Spin.SetPos32(val);
			Slider.SetPos(val);
			Edit.SetWindowText(tmp);
		}
	}
}

void CMidiChCtrlDlg::CMultiCtrl::SetRange(int min, int max)
{
	Spin.SetRange32(min, max);
	Slider.SetRange(min, max, TRUE);
}

BOOL CMidiChCtrlDlg::CMultiCtrl::IsMember(int id)
{
	if (id == Edit.GetDlgCtrlID() || id == Spin.GetDlgCtrlID() || id == Slider.GetDlgCtrlID()) {
		return TRUE;
	}
	return FALSE;
}

void CMidiChCtrlDlg::CChecker::Attach(CMidiChCtrlDlg* par, int id, int(CMidiChCtrlDlg::*pget)(), void(CMidiChCtrlDlg::*pset)(int))
{
	pParent = par;
	Btn.SubclassDlgItem(id, par);
	pGetter = pget;
	pSetter = pset;
}

IMPLEMENT_DYNAMIC(CMidiChCtrlDlg, CDialogEx)

CMidiChCtrlDlg::CMidiChCtrlDlg(CFITOM* pFitom, int ifid, int ch, CWnd* pParent /*=NULL*/)
: CDialogEx(CMidiChCtrlDlg::IDD, pParent), theFitom(pFitom), iIF(ifid), iCH(ch)
{
	theInst = theFitom->GetMidiInst(iIF);
	theCh = theInst->GetMidiCh(iCH);
	Create(IDD, pParent);
	pMulCtrl[0] = new CMultiCtrl(this, IDC_EDIT_VOLUME, IDC_SPIN_VOLUME, IDC_SLIDER_VOLUME, &CMidiChCtrlDlg::GetVolume, &CMidiChCtrlDlg::SetVolume, 0, 127);
	pMulCtrl[1] = new CMultiCtrl(this, IDC_EDIT_EXPR, IDC_SPIN_EXPR, IDC_SLIDER_EXPR, &CMidiChCtrlDlg::GetExpr, &CMidiChCtrlDlg::SetExpr, 0, 127);
	pMulCtrl[2] = new CMultiCtrl(this, IDC_EDIT_MODUR, IDC_SPIN_MODUR, IDC_SLIDER_MODUR, &CMidiChCtrlDlg::GetModur, &CMidiChCtrlDlg::SetModur, 0, 127);
	pMulCtrl[3] = new CMultiCtrl(this, IDC_EDIT_FOOT, IDC_SPIN_FOOT, IDC_SLIDER_FOOT, &CMidiChCtrlDlg::GetFoot, &CMidiChCtrlDlg::SetFoot, 0, 127);
	pMulCtrl[4] = new CMultiCtrl(this, IDC_EDIT_PORTA, IDC_SPIN_PORTA, IDC_SLIDER_PORTA, &CMidiChCtrlDlg::GetPorta, &CMidiChCtrlDlg::SetPorta, 0, 127);
	pMulCtrl[5] = new CMultiCtrl(this, IDC_EDIT_RANGE, IDC_SPIN_RANGE, IDC_SLIDER_RANGE, &CMidiChCtrlDlg::GetRange, &CMidiChCtrlDlg::SetRange, 0, 127);
	pMulCtrl[6] = new CMultiCtrl(this, IDC_EDIT_BEND, IDC_SPIN_BEND, IDC_SLIDER_BEND, &CMidiChCtrlDlg::GetBend, &CMidiChCtrlDlg::SetBend, -8192, 8191);
	pMulCtrl[7] = new CMultiCtrl(this, IDC_EDIT_TUNE, IDC_SPIN_TUNE, IDC_SLIDER_TUNE, &CMidiChCtrlDlg::GetTune, &CMidiChCtrlDlg::SetTune, -8192, 8191);
	pMulCtrl[8] = new CMultiCtrl(this, IDC_EDIT_POLY, IDC_SPIN_POLY, IDC_SLIDER_POLY, &CMidiChCtrlDlg::GetPoly, &CMidiChCtrlDlg::SetPoly, 0, 127);
	pMulCtrl[9] = new CMultiCtrl(this, IDC_EDIT_PHYCH, IDC_SPIN_PHYCH, IDC_SLIDER_PHYCH, &CMidiChCtrlDlg::GetTune, &CMidiChCtrlDlg::SetTune, 0, 127);
	pMulCtrl[10] = new CMultiCtrl(this, IDC_EDIT_PAN, IDC_SPIN_PAN, IDC_SLIDER_PAN, &CMidiChCtrlDlg::GetPan, &CMidiChCtrlDlg::SetPan, 0, 127);
	pChecker[0] = new CChecker(this, IDC_CHECK_LEGATO, &CMidiChCtrlDlg::GetLegato, &CMidiChCtrlDlg::SetLegato);
	pChecker[1] = new CChecker(this, IDC_CHECK_PORTA, &CMidiChCtrlDlg::GetPortOn, &CMidiChCtrlDlg::SetPortOn);
	pChecker[2] = new CChecker(this, IDC_CHECK_MONO, &CMidiChCtrlDlg::GetMono, &CMidiChCtrlDlg::SetMono);
	pChecker[3] = new CChecker(this, IDC_CHECK_SUSTAIN, &CMidiChCtrlDlg::GetSustain, &CMidiChCtrlDlg::SetSustain);
	pChecker[4] = new CChecker(this, IDC_CHECK_DAMP, &CMidiChCtrlDlg::GetDamp, &CMidiChCtrlDlg::SetDamp);
	pChecker[5] = new CChecker(this, IDC_CHECK_SOST, &CMidiChCtrlDlg::GetSoste, &CMidiChCtrlDlg::SetSoste);
}

CMidiChCtrlDlg::~CMidiChCtrlDlg()
{
}

BOOL CMidiChCtrlDlg::CMultiCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL res = FALSE;
	int val = value;
	TCHAR tmp[32];
	switch (wParam) {
	case UDN_DELTAPOS:
		val = Spin.GetPos32();
		break;
	case EN_CHANGE:
		Edit.GetWindowTextA(tmp, 32);
		val = atoi(tmp);
		break;
	case WM_HSCROLL:
		val = Slider.GetPos();
		break;
	default:
		return FALSE;
	}
	if (value != val) {
		(pParent->*pSetter)(val);
		return TRUE;
	}
	return FALSE;
}

void CMidiChCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEXT_MIDICH, edtMidiIfCh);
	DDX_Control(pDX, IDC_COMBO_DEVICE, cmbDevice);
	DDX_Control(pDX, IDC_COMBO_BANK, cmbBank);
	DDX_Control(pDX, IDC_COMBO_PROG, cmbProg);
}


BEGIN_MESSAGE_MAP(CMidiChCtrlDlg, CDialogEx)
	ON_CBN_SELENDOK(IDC_COMBO_DEVICE, &CMidiChCtrlDlg::OnSelendokComboDevice)
	ON_CBN_SELENDOK(IDC_COMBO_BANK, &CMidiChCtrlDlg::OnSelendokComboBank)
	ON_CBN_SELENDOK(IDC_COMBO_PROG, &CMidiChCtrlDlg::OnSelendokComboProg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_VEDIT, &CMidiChCtrlDlg::OnBnClickedBtnVedit)
	ON_BN_CLICKED(IDC_BUTTON_PICK, &CMidiChCtrlDlg::OnBnClickedButtonPick)
	ON_WM_HSCROLL()
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_VOLUME, IDC_EDIT_PHYCH, &CMidiChCtrlDlg::OnEnChangeEdit)
	ON_NOTIFY_RANGE(UDN_DELTAPOS, IDC_SPIN_VOLUME, IDC_SPIN_PHYCH, &CMidiChCtrlDlg::OnDeltaposSpin)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_LEGATO, IDC_CHECK_SOST, &CMidiChCtrlDlg::OnBnClickedCheck)
END_MESSAGE_MAP()


// CMidiChCtrlDlg メッセージ ハンドラー


void CMidiChCtrlDlg::OnCancel()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。

	OnOK();
}


void CMidiChCtrlDlg::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	KillTimer(hTimer);
	Sleep(100);
	DestroyWindow();
}


BOOL CMidiChCtrlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	::SetWindowLong(cmbBank.GetSafeHwnd(), GWL_USERDATA, -1);
	::SetWindowLong(cmbProg.GetSafeHwnd(), GWL_USERDATA, -1);
	// TODO: ここに初期化を追加してください
	if (theCh != 0) {
		InitDevice();
	}

	hTimer = SetTimer(0, 10, 0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CMidiChCtrlDlg::PostNcDestroy()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	::Sleep(100);
	delete this;
}

void CMidiChCtrlDlg::InitDevice()
{
	TCHAR tmp[256];
	wsprintf(tmp, _T("CH%i - %i:%s"), iCH, iIF, theFitom->GetMidiIn(iIF)->GetDescriptor());
	edtMidiIfCh.SetWindowText(tmp);
	int devs = theFitom->GetInstDevs();
	int curdev = theCh->GetDeviceID();
	cmbDevice.ResetContent();
	for (int i = 0; i < devs; i++) {
		TCHAR tmp[256];
		CSoundDevice* pDev = theFitom->GetInstDeviceFromIndex(i);
		int devid = pDev->GetDevice();
		wsprintf(tmp, _T("%02X:%s"), devid, theFitom->GetDeviceNameFromID(devid));
		cmbDevice.AddString(tmp);
		cmbDevice.SetItemData(i, devid);
		if (devid == curdev) {
			cmbDevice.SetCurSel(i);
		}
	}
	{
		int n = cmbDevice.AddString(_T("FF:RHYTHM"));
		cmbDevice.SetItemData(n, 0xff);
		if (curdev == 0xff) {
			cmbDevice.SetCurSel(n);
		}
	}
	SetBank();
	SetProg();
}

void CMidiChCtrlDlg::SetBank()
{
	if (!cmbBank.GetDroppedState()) {
		int curdev = theCh->GetDeviceID();
		int curbnk = theCh->GetBankNo();
		if (::GetWindowLong(cmbBank.GetSafeHwnd(), GWL_USERDATA) != curdev) {
			cmbBank.ResetContent();
			::SetWindowLong(cmbBank.GetSafeHwnd(), GWL_USERDATA, curdev);
			for (int i = 0; i < 128; i++) {
				TCHAR tmp[256];
				const TCHAR* pBankName = theFitom->GetBankName(curdev, i);
				if (pBankName) {
					wsprintf(tmp, "%03i:%s", i, pBankName);
					int n = cmbBank.AddString(tmp);
					cmbBank.SetItemData(n, i);
					if (n == curbnk) {
						cmbBank.SetCurSel(n);
					}
				}
			}
		}
		else {
			if (cmbBank.GetCurSel() != curbnk) {
				cmbBank.SetCurSel(curbnk);
			}
		}
	}
}

void CMidiChCtrlDlg::SetProg()
{
	if (!cmbProg.GetDroppedState()) {
		int curdev = theCh->GetDeviceID();
		int curbnk = theCh->GetBankNo();
		int curprg = theCh->GetProgramNo();
		int prop = MAKELONG(curbnk, curdev);
		if (::GetWindowLong(cmbProg.GetSafeHwnd(), GWL_USERDATA) != prop) {
			cmbProg.ResetContent();
			::SetWindowLong(cmbProg.GetSafeHwnd(), GWL_USERDATA, prop);
			for (int i = 0; i < 128; i++) {
				TCHAR tmp[256];
				TCHAR vname[17];
				FMVOICE voice;
				theFitom->GetVoice(&voice, curdev, curbnk, i);
				lstrcpyn(vname, voice.name, 16);
				vname[16] = _T('\0');
				wsprintf(tmp, "%03i:%s", i, vname);
				int n = cmbProg.AddString(tmp);
				if (n == curprg) {
					cmbProg.SetCurSel(n);
				}
			}
		}
		else {
			if (cmbProg.GetCurSel() != curprg) {
				cmbProg.SetCurSel(curprg);
			}
		}
	}
}

void CMidiChCtrlDlg::SetDevice()
{
	int dev = theCh->GetDeviceID();
	if (!cmbDevice.GetDroppedState()) {
		if (cmbDevice.GetItemData(cmbDevice.GetCurSel()) != dev) {
			for (int i = 0; i < cmbDevice.GetCount(); i++) {
				if (cmbDevice.GetItemData(i) == dev) {
					cmbDevice.SetCurSel(i);
				}
			}
		}
	}
	SetBank();
	SetProg();
}

void CMidiChCtrlDlg::OnSelendokComboDevice()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	theCh->BankSelLSB(UINT8(cmbDevice.GetItemData(cmbDevice.GetCurSel())));
	SetDevice();
}

void CMidiChCtrlDlg::OnSelendokComboBank()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	theCh->BankSelMSB(cmbBank.GetCurSel());
	SetBank();
	SetProg();
}

void CMidiChCtrlDlg::OnSelendokComboProg()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	theCh->ProgChange(cmbProg.GetCurSel());
	SetProg();
}

void CMidiChCtrlDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	if (theCh) {
		SetDevice();
		for (int i = 0; i < numMultiCtrl; i++) {
			pMulCtrl[i]->SetValue();
		}
		for (int i = 0; i < numChecker; i++) {
			pChecker[i]->UpdateState();
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CMidiChCtrlDlg::OnBnClickedBtnVedit()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CVoiceEditDlg dlg(m_pParentWnd);
	dlg.SetDevice(cmbDevice.GetItemData(cmbDevice.GetCurSel()));
	dlg.SetBank(cmbBank.GetCurSel());
	dlg.SetProg(cmbProg.GetCurSel());
	if (dlg.DoModal() == IDOK) {

	}
}

void CMidiChCtrlDlg::OnBnClickedButtonPick()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDlgVoicePicker dlg;
	dlg.SetDevice(cmbDevice.GetItemData(cmbDevice.GetCurSel()));
	dlg.SetBank(cmbBank.GetCurSel());
	dlg.SetProg(cmbProg.GetCurSel());
	if (dlg.DoModal() == IDOK) {
		theCh->BankSelLSB(dlg.GetDevice());
		theCh->BankSelMSB(dlg.GetBank());
		theCh->ProgChange(dlg.Getprog());
	}
}

void CMidiChCtrlDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	for (int i = 0; i < numMultiCtrl; i++) {
		if (pMulCtrl[i]->IsMember(pScrollBar->GetDlgCtrlID())) {
			NMUPDOWN nms;
			nms.hdr.hwndFrom = pScrollBar->GetSafeHwnd();
			nms.hdr.idFrom = pScrollBar->GetDlgCtrlID();
			nms.hdr.code = nSBCode;
			nms.iPos = nPos;
			nms.iDelta = 0;
			LRESULT res = 0;
			pMulCtrl[i]->OnNotify(WM_HSCROLL, LPARAM(&nms), &res);
			return;
		}
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMidiChCtrlDlg::OnEnChangeEdit(UINT id)
{
	for (int i = 0; i < numMultiCtrl; i++) {
		if (pMulCtrl[i]->IsMember(id)) {
			NMHDR nms;
			nms.code = EN_CHANGE;
			nms.hwndFrom = GetDlgItem(id)->GetSafeHwnd();
			nms.idFrom = id;
			LRESULT res = 0;
			pMulCtrl[i]->OnNotify(EN_CHANGE, LPARAM(&nms), &res);
		}
	}
}

void CMidiChCtrlDlg::OnDeltaposSpin(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	for (int i = 0; i < numMultiCtrl; i++) {
		if (pMulCtrl[i]->IsMember(id)) {
			pMulCtrl[i]->OnNotify(UDN_DELTAPOS, LPARAM(pNMHDR), pResult);
		}
	}
	*pResult = 0;
}

void CMidiChCtrlDlg::OnBnClickedCheck(UINT id)
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	for (int i = 0; i < numChecker; i++){
		if (pChecker[i]->IsMember(id)) {
			pChecker[i]->OnClicked();
		}
	}
}

BOOL CMidiChCtrlDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if (WM_KEYDOWN == pMsg->message)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			return FALSE;
		case VK_ESCAPE:
			return FALSE;
		default:
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
