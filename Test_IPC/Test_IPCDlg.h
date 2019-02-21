
// Test_IPCDlg.h: 헤더 파일
//

#if !defined(AFX_IPCAPPDLG_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_)
#define AFX_IPCAPPDLG_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_


#include "LayerManager.h"	// Added by ClassView
#include "ChatAppLayer.h"	// Added by ClassView
#include "EthernetLayer.h"	// Added by ClassView
#include "FileLayer.h"		// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// CTestIPCDlg 대화 상자
class CTestIPCDlg : public CDialogEx, public CBaseLayer
{
// 생성입니다.
public:
	CTestIPCDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEST_IPC_DIALOG };
#endif
	CListBox	m_ListChat;
	UINT		m_unDstAddr;
	UINT		m_unSrcAddr;
	CString		m_stMessage;

	BOOL			Receive(unsigned char* ppayload);
	inline void		SendData();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSendMessage();
	afx_msg void OnButtonAddrSet();
	afx_msg void OnCheckBroadcast();
	afx_msg void OnTimer(UINT nIDEvent);
public:
	CLayerManager	m_LayerMgr;
	int				m_nAckReady;
	enum {
		IPC_INITIALIZING,
		IPC_READYTOSEND,
		IPC_WAITFORACK,
		IPC_ERROR,
		IPC_BROADCASTMODE,
		IPC_UNICASTMODE,
		IPC_ADDR_SET,
		IPC_ADDR_RESET
	};

	void			SetDlgState(int state);
	inline void		EndofProcess();
	inline void		SetRegstryMessage();
	LRESULT			OnRegSendMsg(WPARAM wParam, LPARAM lParam);
	LRESULT			OnRegAckMsg(WPARAM wParam, LPARAM lParam);

	BOOL			m_bSendReady;

	// Object App
	CChatAppLayer*	m_ChatApp;

	// Implementation
	UINT			m_wParam;
	DWORD			m_lParam;

};

#endif // !defined(AFX_IPCAPPDLG_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_)
