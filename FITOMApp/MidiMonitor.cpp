// MidiMonitor.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOM.h"
#include "MIDIDEV.h"
#include "SoundDev.h"
#include "MIDI.h"
#include "FITOMApp.h"
#include "MidiMonitor.h"
#include "afxdialogex.h"
#include "FITOMAppDlg.h"
#include "MidiChCtrlDlg.h"


// CMidiMonitor ダイアログ

IMPLEMENT_DYNAMIC(CMidiMonitor, CDialogEx)

CMidiMonitor::CMidiMonitor(CFITOM* pFitom, CWnd* pParent /*=NULL*/)
: CDialogEx(CMidiMonitor::IDD, pParent), theFitom(pFitom)
{
	Create(IDD, pParent);
	for (int i = 0; i < 16; i++) {
		prevstat[i].bank = -1;
		prevstat[i].dev = -1;
		prevstat[i].note = -1;
		prevstat[i].pan = -1;
		prevstat[i].prog = -1;
		prevstat[i].vol = -1;
	}
}

CMidiMonitor::~CMidiMonitor()
{
}

void CMidiMonitor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MIDIIN, cmbMidiIn);
	DDX_Control(pDX, IDC_LIST_MIDICH, lstMidiMon);
}


BEGIN_MESSAGE_MAP(CMidiMonitor, CDialogEx)
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_MIDICH, &CMidiMonitor::OnDblclkListMidich)
END_MESSAGE_MAP()


// CMidiMonitor メッセージ ハンドラー


void CMidiMonitor::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。

}


void CMidiMonitor::PostNcDestroy()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	DestroyWindow();
	delete this;
}


void CMidiMonitor::OnCancel()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	CWnd* btn = m_pParentWnd->GetDlgItem(IDC_CHECK_MIDIMON);
	btn->SetFocus();
	btn->PostMessage(BM_CLICK, 0, 0);
}


BOOL CMidiMonitor::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: ここに初期化を追加してください
	cmbMidiIn.ResetContent();
	int numMidIn = theFitom->GetMidiInputs();
	for (int i = 0; i < numMidIn; i++) {
		cmbMidiIn.AddString(theFitom->GetMidiIn(i)->GetDescriptor());
	}
	lstMidiMon.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	lstMidiMon.InsertColumn(0, _T("CH"), LVCFMT_LEFT, 28);
	lstMidiMon.InsertColumn(1, _T("Device"), LVCFMT_LEFT, 64);
	lstMidiMon.InsertColumn(2, _T("Note"), LVCFMT_LEFT, 40);
	lstMidiMon.InsertColumn(3, _T("Pan"), LVCFMT_LEFT, 32);
	lstMidiMon.InsertColumn(4, _T("Vol"), LVCFMT_LEFT, 32);
	lstMidiMon.InsertColumn(5, _T("Bank"), LVCFMT_LEFT, 128);
	lstMidiMon.InsertColumn(6, _T("Prog"), LVCFMT_LEFT, 128);

	for (int i = 0; i < 16; i++) {
		char tmp[8];
		wsprintf(tmp, "%i", i+1);
		lstMidiMon.InsertItem(i, tmp);
		lstMidiMon.SetItemData(i, i);
	}
	cmbMidiIn.SetCurSel(0);
	hTimer = SetTimer(0, 10, 0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CMidiMonitor::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	if (this->IsWindowVisible()) {
		for (int i = 0; i < 16; i++) {
			chstat stat;
			GetCurrentStat(cmbMidiIn.GetCurSel(), i, &stat);
			if (1/*CompareStat(prevstat[i], stat) != 0*/) {
				UpdateStat(i, &stat);
			}
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

int CMidiMonitor::CompareStat(chstat& stat1, chstat& stat2)
{
	int ret = 0;
	ret = (stat1.dev == stat2.dev) ? 0 : !0;
	ret = (stat1.note == stat2.note) ? 0 : !0;
	ret = (stat1.vol == stat2.vol) ? 0 : !0;
	ret = (stat1.pan == stat2.pan) ? 0 : !0;
	ret = (stat1.bank == stat2.bank) ? 0 : !0;
	ret = (stat1.prog == stat2.prog) ? 0 : !0;
	ret = ret ? ((stat1.dev + stat1.bank + stat1.prog + stat1.note + stat1.pan + stat1.vol)
		- (stat2.dev + stat2.bank + stat2.prog + stat2.note + stat2.pan + stat2.vol)) : 0;
	return ret;
}

void CMidiMonitor::GetCurrentStat(int ifno, int ch, chstat* stat)
{
	CMidiCh* midich = theFitom->GetMidiInst(ifno)->GetMidiCh(ch);
	if (midich) {
		stat->note = midich->GetLastNote();
		stat->vol = midich->GetTrackVolume();
		stat->pan = midich->GetPanpot();
		stat->dev = midich->GetDeviceID();
		stat->bank = midich->GetBankNo();
		stat->prog = midich->GetProgramNo();
	}
}

const char* NoteName[] = {
	"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B",
};

void CMidiMonitor::UpdateStat(int ch, chstat* stat)
{
	char tmp[100];
	if (prevstat[ch].dev != stat->dev) {
		lstMidiMon.SetItemText(ch, 1, theFitom->GetDeviceNameFromID(stat->dev));
	}
	if (prevstat[ch].note != stat->note) {
		if (stat->note < 128) {
			wsprintf(tmp, "%s%i", NoteName[stat->note % 12], (stat->note / 12 - 2));
		}
		else {
			strcpy(tmp, "----");
		}
		lstMidiMon.SetItemText(ch, 2, tmp);
	}
	if (prevstat[ch].pan != stat->pan) {
		wsprintf(tmp, "%i", stat->pan);
		lstMidiMon.SetItemText(ch, 3, tmp);
	}
	if (prevstat[ch].vol != stat->vol) {
		wsprintf(tmp, "%i", stat->vol);
		lstMidiMon.SetItemText(ch, 4, tmp);
	}
	if (prevstat[ch].bank != stat->bank) {
		if (stat->dev != 0xff) {
			wsprintf(tmp, "%i:%s", stat->bank, theFitom->GetBankName(stat->dev, stat->bank));
		}
		else {
			wsprintf(tmp, "%s", _T("RHYTHM"));
		}
		lstMidiMon.SetItemText(ch, 5, tmp);
		prevstat[ch].prog = -1;
	}
	if (prevstat[ch].prog != stat->prog) {
		if (stat->dev != 0xff) {
			FMVOICE voice;
			theFitom->GetVoice(&voice, stat->dev, stat->bank, stat->prog);
			wsprintf(tmp, "%i:%s", stat->prog, voice.name);
		}
		else {
			wsprintf(tmp, "%i:%s", stat->prog, theFitom->GetBankName(stat->dev, stat->prog));
		}
		lstMidiMon.SetItemText(ch, 6, tmp);
	}
	prevstat[ch] = *stat;
}

void CMidiMonitor::OnDblclkListMidich(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = 0;
	CMidiChCtrlDlg* pDlg = new CMidiChCtrlDlg(theFitom, cmbMidiIn.GetCurSel(), pNMItemActivate->iItem, this);
	pDlg->ShowWindow(SW_SHOW);
}


BOOL CMidiMonitor::PreTranslateMessage(MSG* pMsg)
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

