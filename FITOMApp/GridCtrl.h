#pragma once
#include "afxcmn.h"

class CGridEdit : public CEdit
{
public:
	CGridEdit();
	~CGridEdit();
};

class CGridCtrl :
	public CListCtrl
{
protected:
	CGridEdit m_Edit;
	CSpinButtonCtrl m_Spin;
public:
	CGridCtrl();
	~CGridCtrl();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocus(NMHDR *pNMHDR, LRESULT *pResult);
};

