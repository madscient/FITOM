#include "stdafx.h"
#include "GridCtrl.h"

CGridEdit::CGridEdit()
{
}

CGridEdit::~CGridEdit()
{
}

CGridCtrl::CGridCtrl()
{
}


CGridCtrl::~CGridCtrl()
{
}

BEGIN_MESSAGE_MAP(CGridCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CGridCtrl::OnNMDblclk)
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, &CGridCtrl::OnNMKillfocus)
END_MESSAGE_MAP()


void CGridCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = 0;
}


void CGridCtrl::OnNMKillfocus(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = 0;
}
