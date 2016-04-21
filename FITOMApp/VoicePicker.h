#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CdlgCoicePicker ダイアログ

class CDlgVoicePicker : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVoicePicker)

public:
	CDlgVoicePicker(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDlgVoicePicker();

// ダイアログ データ
	enum { IDD = IDD_DLG_VPICK };

protected:
	CFITOM* theFitom;
	CInstCh* pICh;
	BOOL bInit;
	int theDevice;
	int theBank;
	int theProg;
	void RefreshDevice();
	void RefreshBank();
	void RefreshProg();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	void SetDevice(int dev);
	void SetBank(int bank);
	void SetProg(int prog);
	int GetDevice() { return theDevice; };
	int GetBank() { return theBank; };
	int Getprog() { return theProg; };
	CComboBox cmbDevice;
	CComboBox cmbBank;
	CListCtrl lstProg;
	afx_msg void OnSelendokComboDevice();
	afx_msg void OnSelendokComboBank();
	virtual BOOL OnInitDialog();
//	afx_msg void OnOK(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkListProgs(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnOK();
	afx_msg void OnClickListProgs(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI *pCmdUI);
	afx_msg void OnEditInsert();
	afx_msg void OnUpdateEditInsert(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnRclickListProgs(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRdblclkListProgs(NMHDR *pNMHDR, LRESULT *pResult);
};
