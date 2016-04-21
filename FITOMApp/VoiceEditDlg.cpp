// VoiceEditDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOM.h"
#include "FITOMApp.h"
#include "VoiceEditDlg.h"
#include "VoicePicker.h"
#include "afxdialogex.h"


// データテーブル
CVoiceEditDlg::VoiceItem CVoiceEditDlg::commonItem[] = {
	{ _T("Feedback"), 0, 0, 7, VOICE_GROUP_OPM | VOICE_GROUP_OPNA | VOICE_GROUP_OPL2 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetFB, &CVoiceEditDlg::GetFB, },
	{ _T("Feedback1"), 0, 0, 7, VOICE_GROUP_OPL3, &CVoiceEditDlg::SetFB1, &CVoiceEditDlg::GetFB1, },
	{ _T("Feedback2"), 0, 0, 7, VOICE_GROUP_OPL3, &CVoiceEditDlg::SetFB2, &CVoiceEditDlg::GetFB2, },
	{ _T("Algorhythm"), 0, 0, 7, VOICE_GROUP_OPM, &CVoiceEditDlg::SetAL3, &CVoiceEditDlg::GetAL3, },
	{ _T("Algorhythm"), 0, 0, 7, VOICE_GROUP_OPNA, &CVoiceEditDlg::SetAL, &CVoiceEditDlg::GetAL3, },
	{ _T("Algorhythm"), 0, 0, 1, VOICE_GROUP_OPL2, &CVoiceEditDlg::SetAL, &CVoiceEditDlg::GetAL1, },
	{ _T("Algorhythm"), 0, 0, 11, VOICE_GROUP_OPL3, &CVoiceEditDlg::SetAL, &CVoiceEditDlg::GetAL4, },
	{ _T("ROM Tone"), 0, 0, 15, VOICE_GROUP_OPLL, &CVoiceEditDlg::SetROMTN, &CVoiceEditDlg::GetAL4, },
	{ _T("Tone/Noise"), 0, 0, 3, VOICE_GROUP_PSG, &CVoiceEditDlg::SetAL, &CVoiceEditDlg::GetAL, },
	{ _T("Noise Enable"), 0, 0, 3, VOICE_GROUP_OPM, &CVoiceEditDlg::SetNE, &CVoiceEditDlg::GetNE, },
	{ _T("Noise Freq"), 0, 0, 63, VOICE_GROUP_OPM | VOICE_GROUP_PSG, &CVoiceEditDlg::SetNFreq, &CVoiceEditDlg::GetNFreq, },
	{ _T("Noise Freq2"), 0, 0, 63, VOICE_GROUP_PSG, &CVoiceEditDlg::SetNFreq2, &CVoiceEditDlg::GetNFreq2, },
	{ _T("AM Secse"), 0, 0, 3, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetAMS, &CVoiceEditDlg::GetAMS, },
	{ _T("PM Secse"), 0, 0, 7, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetPMS, &CVoiceEditDlg::GetPMS, },
	{ _T("P-LFO Wave"), 0, 0, 14, VOICE_GROUP_ALL, &CVoiceEditDlg::SetLFOWave, &CVoiceEditDlg::GetLFOWave, },
	{ _T("P-LFO Depth"), 0, -8192, 8191, VOICE_GROUP_ALL, &CVoiceEditDlg::SetLFODepth, &CVoiceEditDlg::GetLFODepth, },
	{ _T("P-LFO Freq"), 0, 0, 15, VOICE_GROUP_ALL, &CVoiceEditDlg::SetLFOFreq, &CVoiceEditDlg::GetLFOFreq, },
	{ _T("P-LFO Delay"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetLFODelay, &CVoiceEditDlg::GetLFODelay, },
	{ _T("P-LFO Rate"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetLFORate, &CVoiceEditDlg::GetLFORate, },
	{ 0, 0, 0, 0, VOICE_GROUP_NONE, 0, 0, },
};

CVoiceEditDlg::VoiceItem CVoiceEditDlg::operatorItem[] = {
	{ _T("Attack Rate"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetAR, &CVoiceEditDlg::GetAR, },
	{ _T("Decay Rate"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetDR, &CVoiceEditDlg::GetDR, },
	{ _T("Sustain Level"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetSL, &CVoiceEditDlg::GetSL, },
	{ _T("Sustain Rate"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetSR, &CVoiceEditDlg::GetSR, },
	{ _T("Release Rate"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetRR, &CVoiceEditDlg::GetRR, },
	{ _T("Reverbation"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetRV, &CVoiceEditDlg::GetRV, },
	{ _T("Total Level"), 0, 0, 127, VOICE_GROUP_OPM | VOICE_GROUP_OPNA | VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetTL, &CVoiceEditDlg::GetTL, },
	{ _T("EG-Bias"), 0, 0, 127, VOICE_GROUP_OPM | VOICE_GROUP_PSG, &CVoiceEditDlg::SetEGS, &CVoiceEditDlg::GetEGS, },
	{ _T("KS-Level"), 0, 0, 3, VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetKSL, &CVoiceEditDlg::GetKSL, },
	{ _T("KS-Rate"), 0, 0, 3, VOICE_GROUP_OPM | VOICE_GROUP_OPNA | VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetKSR, &CVoiceEditDlg::GetKSR, },
	{ _T("SSG-EG"), 0, 0, 31, VOICE_GROUP_OPNA | VOICE_GROUP_PSG, &CVoiceEditDlg::SetEG, &CVoiceEditDlg::GetEG, },
	{ _T("AM Enable"), 0, 0, 1, VOICE_GROUP_OPM | VOICE_GROUP_OPNA | VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetAME, &CVoiceEditDlg::GetAME, },
	{ _T("VIB Enable"), 0, 0, 1, VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetVIB, &CVoiceEditDlg::GetVIB, },
	{ _T("Osc Fix"), 0, 0, 1, VOICE_GROUP_OPM, &CVoiceEditDlg::SetVIB, &CVoiceEditDlg::GetVIB, },
	{ _T("Wave Select"), 0, 0, 127, VOICE_GROUP_OPM | VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL | VOICE_GROUP_PSG, &CVoiceEditDlg::SetWS, &CVoiceEditDlg::GetWS, },
	{ _T("Multiple"), 0, 0, 15, VOICE_GROUP_OPM | VOICE_GROUP_OPNA | VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetML, &CVoiceEditDlg::GetML, },
	{ _T("Detune1"), 0, 0, 7, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetDT1, &CVoiceEditDlg::GetDT1, },
	{ _T("Detune2"), 0, 0, 15, VOICE_GROUP_OPM, &CVoiceEditDlg::SetDT2, &CVoiceEditDlg::GetDT2, },
	{ _T("Pseudo Detune"), 0, -8192, 8191, VOICE_GROUP_OPL3, &CVoiceEditDlg::SetPDT, &CVoiceEditDlg::GetPDT, },
	{ _T("Fixed Freq"), 0, 0, 32640, VOICE_GROUP_OPM, 0, &CVoiceEditDlg::GetFix, },
	{ _T("TL-LFO Wave"), 0, 0, 6, VOICE_GROUP_ALL, &CVoiceEditDlg::SetOPLFOWave, &CVoiceEditDlg::GetOPLFOWave, },
	{ _T("TL-LFO Depth"), 0, -64, 63, VOICE_GROUP_ALL, &CVoiceEditDlg::SetOPLFODepth, &CVoiceEditDlg::GetOPLFODepth, },
	{ _T("TL-LFO Freq"), 0, 0, 15, VOICE_GROUP_ALL, &CVoiceEditDlg::SetOPLFOFreq, &CVoiceEditDlg::GetOPLFOFreq, },
	{ _T("TL-LFO Delay"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetOPLFODelay, &CVoiceEditDlg::GetOPLFODelay, },
	{ _T("TL-LFO Rate"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetOPLFORate, &CVoiceEditDlg::GetOPLFORate, },
	{ 0, 0, 0, 0, VOICE_GROUP_NONE, 0, 0, },
};



// CVoiceEditDlg ダイアログ

IMPLEMENT_DYNAMIC(CVoiceEditDlg, CDialogEx)

CVoiceEditDlg::CVoiceEditDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVoiceEditDlg::IDD, pParent)
	, theDevice(-1), theBank(-1), theProg(-1), theFitom(CFITOM::GetInstance())
	, bInit(FALSE), bModified(FALSE), pICh(0)
{
	lstctls[0] = &lstCommon;
	lstctls[1] = &lstOper1;
	lstctls[2] = &lstOper2;
	lstctls[3] = &lstOper3;
	lstctls[4] = &lstOper4;
}

CVoiceEditDlg::~CVoiceEditDlg()
{
	delete pICh;
}

void CVoiceEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CPARAM, lstCommon);
	DDX_Control(pDX, IDC_LIST_OPPARAM1, lstOper1);
	DDX_Control(pDX, IDC_LIST_OPPARAM2, lstOper2);
	DDX_Control(pDX, IDC_LIST_OPPARAM3, lstOper3);
	DDX_Control(pDX, IDC_LIST_OPPARAM4, lstOper4);
	DDX_Control(pDX, IDC_EDIT_NAME, edtName);
	DDX_Control(pDX, IDC_EDIT_DEVICE, edtDevice);
	DDX_Control(pDX, IDC_EDIT_BANK, edtBank);
	DDX_Control(pDX, IDC_EDIT_PROG_NO, edtProg);
	DDX_Control(pDX, IDC_BTN_TEST, btnTest);
	DDX_Control(pDX, IDC_SPIN_VELO, spnVelocity);
	DDX_Control(pDX, IDC_SPIN_NOTE, spnNote);
	DDX_Control(pDX, IDC_EDIT_INPLACE, edtInplace);
	DDX_Control(pDX, IDC_SPIN_INPLACE, spnInplace);
}


BEGIN_MESSAGE_MAP(CVoiceEditDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_PICK, &CVoiceEditDlg::OnBnClickedButtonPick)
	ON_EN_UPDATE(IDC_EDIT_NAME, &CVoiceEditDlg::OnUpdateEditName)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CPARAM, &CVoiceEditDlg::OnDblclkListCparam)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPPARAM1, &CVoiceEditDlg::OnDblclkListOpparam1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPPARAM2, &CVoiceEditDlg::OnDblclkListOpparam2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPPARAM3, &CVoiceEditDlg::OnDblclkListOpparam3)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPPARAM4, &CVoiceEditDlg::OnDblclkListOpparam4)
	ON_EN_CHANGE(IDC_EDIT_INPLACE, &CVoiceEditDlg::OnEnChangeEditInplace)
	ON_EN_KILLFOCUS(IDC_EDIT_INPLACE, &CVoiceEditDlg::OnKillfocusEditInplace)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_INPLACE, &CVoiceEditDlg::OnDeltaposSpinInplace)
	ON_BN_CLICKED(IDCANCEL, &CVoiceEditDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


void CVoiceEditDlg::SetDevice(int dev)
{
	theDevice = dev;
	if (bInit && theDevice >= 0) {
		TCHAR tmp[256];
		wsprintf(tmp, _T("%02X:%s"), theDevice, theFitom->GetDeviceNameFromID(theDevice));
		edtDevice.SetWindowText(tmp);
	}
}

void CVoiceEditDlg::SetBank(int bank)
{
	theBank = bank;
	if (bInit && theDevice >= 0) {
		TCHAR tmp[256];
		wsprintf(tmp, _T("%03i:%s"), theBank, theFitom->GetBankName(theDevice, theBank));
		edtBank.SetWindowText(tmp);
	}
}

void CVoiceEditDlg::SetProg(int prog)
{
	theProg = prog;
	theFitom->GetVoice(&theVoice, theDevice, theBank, theProg);
	if (bInit && theBank >= 0 && theDevice >= 0) {
		TCHAR tmp[256];
		edtProg.SetWindowText(tmp);
		wsprintf(tmp, _T("%s"), theVoice.name);
		edtName.SetWindowText(tmp);
	}
}

void CVoiceEditDlg::UpdateListCtrl(int op, BOOL bInit)
{
	if (op <= 4 && op >= 0) {
		int i = 0;
		if (bInit) {
			lstctls[op]->DeleteAllItems();
		}
		for (VoiceItem* items = (op == 0) ? commonItem : operatorItem; items->caption; items++) {
			if (items->mask & CFITOM::GetVoiceGroupMask(theDevice)) {
				if (bInit) {
					lstctls[op]->InsertItem(i, items->caption);
					lstctls[op]->SetItemData(i, DWORD_PTR(items));
				}
				lstctls[op]->SetItemData(i, DWORD_PTR(items));
				TCHAR tmp[80];
				if (items->pGetter) {
					wsprintf(tmp, "%i", (this->*(items->pGetter))(op-1));
				}
				else {
					wsprintf(tmp, "-");
				}
				lstctls[op]->SetItemText(i, 1, tmp);
				i++;
			}
		}
	}
}

// CVoiceEditDlg メッセージ ハンドラー

BOOL CVoiceEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: ここに初期化を追加してください
	for (int op = 0; op < 5; op++) {
		lstctls[op]->SetExtendedStyle(LVS_EX_FULLROWSELECT);
		lstctls[op]->InsertColumn(0, _T("Parameter"), LVCFMT_LEFT, 84);
		lstctls[op]->InsertColumn(1, _T("Value"), LVCFMT_RIGHT, 46);
	}

	UpdateVoiceView();
	spnNote.SetRange(0, 127);
	spnVelocity.SetRange(0, 127);
	spnNote.SetPos(60);
	spnVelocity.SetPos(127);

	pICh = new CInstCh(0, theFitom);
	pICh->BankSelLSB(theDevice);
	pICh->BankSelMSB(theBank);
	pICh->SetVolume(100);
	pICh->SetExpress(127);
	pICh->SetPanpot(64);
	spnVelocity.SetRange32(0, 127);
	spnVelocity.SetPos32(127);
	spnNote.SetRange32(0, 127);
	spnNote.SetPos32(60);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CVoiceEditDlg::UpdateVoiceView()
{
	TCHAR tmp[256];
	theFitom->GetVoice(&theVoice, theDevice, theBank, theProg);
	wsprintf(tmp, _T("%02X:%s"), theDevice, theFitom->GetDeviceNameFromID(theDevice));
	edtDevice.SetWindowText(tmp);
	wsprintf(tmp, _T("%03i:%s"), theBank, theFitom->GetBankName(theDevice, theBank));
	edtBank.SetWindowText(tmp);
	wsprintf(tmp, _T("%3i"), theProg);
	edtProg.SetWindowText(tmp);
	memcpy(tmp, theVoice.name, 16);
	tmp[16] = 0;
	wsprintf(tmp, _T("%s"), tmp);
	edtName.SetWindowText(tmp);
	for (int op = 0; op < 5; op++) {
		UpdateListCtrl(op, TRUE);
	}
}

void CVoiceEditDlg::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	theFitom->SetVoice(&theVoice, theDevice, theBank, theProg);
	theFitom->SaveVoice(theDevice, theBank);
	bModified = FALSE;
}

void CVoiceEditDlg::OnBnClickedButtonPick()
{
	if (!bModified || ::AfxMessageBox(IDS_CONFIRM_VOICE_DISCARD, MB_YESNO) == IDYES) {
		CDlgVoicePicker dlg;
		dlg.SetDevice(theDevice);
		dlg.SetBank(theBank);
		dlg.SetProg(theProg);
		if (dlg.DoModal() == IDOK) {
			SetDevice(dlg.GetDevice());
			SetBank(dlg.GetBank());
			SetProg(dlg.Getprog());
			UpdateVoiceView();
			bModified = FALSE;
		}
	}
}


void CVoiceEditDlg::OnUpdateEditName()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加してください。
	TCHAR tmp[17];
	edtName.GetWindowTextA(tmp, 17);
	tmp[16] = _T(0);
	memcpy(theVoice.name, tmp, 16);
	bModified = TRUE;
}

void CVoiceEditDlg::OnTestButtonDown()
{
	pICh->BankSelLSB(theDevice);
	pICh->SetVoiceData(&theVoice);
	pICh->NoteOn(spnNote.GetPos(), spnVelocity.GetPos());
}

void CVoiceEditDlg::OnTestButtonUp()
{
	pICh->NoteOff(spnNote.GetPos());
}

BOOL CVoiceEditDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if (wParam == IDC_BTN_TEST) {
		NMHDR* pnms = (NMHDR*)lParam;
		switch (pnms->code) {
		case WM_LBUTTONDOWN:
			OnTestButtonDown();
			break;
		case WM_LBUTTONUP:
			OnTestButtonUp();
			break;
		}
	}
	return CDialogEx::OnNotify(wParam, lParam, pResult);
}


void CVoiceEditDlg::OnDblclkListCparam(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = OnDblclkListParam(pNMItemActivate, 0);
}


void CVoiceEditDlg::OnDblclkListOpparam1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = OnDblclkListParam(pNMItemActivate, 1);
}


void CVoiceEditDlg::OnDblclkListOpparam2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = OnDblclkListParam(pNMItemActivate, 2);
}


void CVoiceEditDlg::OnDblclkListOpparam3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = OnDblclkListParam(pNMItemActivate, 3);
}


void CVoiceEditDlg::OnDblclkListOpparam4(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = OnDblclkListParam(pNMItemActivate, 4);
}

LRESULT CVoiceEditDlg::OnDblclkListParam(LPNMITEMACTIVATE pNMLV, int op)
{
	CListCtrl* plst = lstctls[op];
	if (pNMLV->iItem < 0) {
		return -1;
	}

	VoiceItem* pvi = (VoiceItem*)(plst->GetItemData(pNMLV->iItem));
	if (pvi->pSetter) {
		CRect ColumnRect;
		plst->GetSubItemRect(pNMLV->iItem, 1, LVIR_BOUNDS, ColumnRect);
		plst->ClientToScreen(ColumnRect);
		this->ScreenToClient(ColumnRect);
		editting_item = pvi;
		editting_op = op;
		edtInplace.SetWindowPos(plst, ColumnRect.left, ColumnRect.top, ColumnRect.Width(), ColumnRect.Height(), SWP_SHOWWINDOW | SWP_DRAWFRAME);
		edtInplace.ShowWindow(TRUE);
		edtInplace.SetWindowText(plst->GetItemText(pNMLV->iItem, 1));
		edtInplace.SetSel(0, -1);
		spnInplace.SetWindowPos(plst, ColumnRect.left, ColumnRect.top, 0, ColumnRect.Height(), SWP_SHOWWINDOW | SWP_DRAWFRAME);
		spnInplace.ShowWindow(TRUE);
		spnInplace.SetRange32(pvi->min, pvi->max);
		spnInplace.SetPos32((this->*(pvi->pGetter))(op - 1));
		//	spnInplace.SetBuddy(&edtInplace);

		edtInplace.SetFocus();
	}
	return 0;
}


void CVoiceEditDlg::OnEnChangeEditInplace()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加してください。
	if (editting_item) {
		bModified = TRUE;
		/*
		edtInplace.SetSel(0, -1);
		int val = spnInplace.GetPos32();
		if (editting_item->min <= val && editting_item->max >= val && editting_item->pSetter) {
			(this->*(editting_item->pSetter))(editting_op - 1, val);
			theFitom->SetVoice(&theVoice, theDevice, theBank, theProg);
		}
		*/
	}
}


void CVoiceEditDlg::OnKillfocusEditInplace()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	if (editting_item) {
		TCHAR tmp1[40], tmp2[40], tmp3[40];
		edtInplace.GetWindowText(tmp1, 40);
		lstctls[editting_op]->GetItemText(lstctls[editting_op]->GetSelectionMark(), 1, tmp3, 40);
		if (lstrcmp(tmp1, tmp3) != 0) {
			bModified = TRUE;
			int val = 0;
			::_itoa((val = atoi(tmp1)), tmp2, 10);
			if (lstrcmp(tmp1, tmp2) == 0) {
				if (editting_item->min <= val && editting_item->max >= val && editting_item->pSetter) {
					spnInplace.SetPos32(val);
					(this->*(editting_item->pSetter))(editting_op - 1, val);
					pICh->SetVoiceData(&theVoice);
					UpdateListCtrl(editting_op, FALSE);
				}
			}
		}
	}
	edtInplace.ShowWindow(FALSE);
	spnInplace.ShowWindow(FALSE);
	editting_item = 0;
	editting_op = -1;
}


void CVoiceEditDlg::OnDeltaposSpinInplace(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	if (editting_item != 0) {
		edtInplace.SetSel(0, -1);
		if (editting_item->pSetter) {
			int val = spnInplace.GetPos32();
			if (editting_item->min <= val && editting_item->max >= val && editting_item->pSetter) {
				(this->*(editting_item->pSetter))(editting_op - 1, val);
				pICh->SetVoiceData(&theVoice);
			}
		}
		bModified = TRUE;
	}
	else{
		int nothing = 0;
	}
	*pResult = 0;
}


void CVoiceEditDlg::OnClickedButtonBank()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}


BOOL CVoiceEditDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if (WM_KEYDOWN == pMsg->message)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			if (edtInplace.IsWindowVisible()) {
				OnKillfocusEditInplace();
			}
			return FALSE;
		case VK_ESCAPE:
			return FALSE;
		default:
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CVoiceEditDlg::OnBnClickedCancel()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	if (!bModified || ::AfxMessageBox(IDS_CONFIRM_VOICE_DISCARD, MB_YESNO) == IDYES) {
		CDialogEx::OnCancel();
	}
}
