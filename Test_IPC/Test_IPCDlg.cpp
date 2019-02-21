
// Test_IPCDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "Test_IPC.h"
#include "Test_IPCDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// 레지스트리에 등록하기 위한 변수
UINT nRegSendMsg;
UINT nRegAckMsg;


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestIPCDlg 대화 상자



CTestIPCDlg::CTestIPCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TEST_IPC_DIALOG, pParent),
	CBaseLayer("ChatDlg"),
	m_bSendReady(FALSE),
	m_nAckReady(-1)
{
	m_unDstAddr = 0;
	m_unSrcAddr = 0;
	m_stMessage = _T("");

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_LayerMgr.AddLayer(new CChatAppLayer("ChatApp"));
	m_LayerMgr.AddLayer(new CEthernetLayer("Ethernet"));
	m_LayerMgr.AddLayer(new CFileLayer("File"));
	m_LayerMgr.AddLayer(this);

	m_LayerMgr.ConnectLayers("File ( *Ethernet ( *ChatApp  ( *ChatDlg ) ) )");

	//////////////////////// fill the blank ///////////////////////////////
	//      레이어를 연결한다.
	//		예) m_LayerMgr.ConnectLayers( "layer1_name ( *layer2_name ( *layer3_name ) )" ) ;
	//   	
	//                            ...
	//                            ...
	//                            ...
	///////////////////////////////////////////////////////////////////////

	m_ChatApp = (CChatAppLayer*)mp_UnderLayer;
}

void CTestIPCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CHAT, m_ListChat);
	DDX_Text(pDX, IDC_EDIT_DST, m_unDstAddr);
	DDX_Text(pDX, IDC_EDIT_SRC, m_unSrcAddr);
	DDX_Text(pDX, IDC_EDIT_MSG, m_stMessage);
}




BEGIN_MESSAGE_MAP(CTestIPCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnSendMessage)
	ON_BN_CLICKED(IDC_BUTTON_ADDR, OnButtonAddrSet)
	ON_BN_CLICKED(IDC_CHECK_TOALL, OnCheckBroadcast)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP

	ON_REGISTERED_MESSAGE(nRegSendMsg, OnRegSendMsg)
	//////////////////////// fill the blank ///////////////////////////////

	ON_REGISTERED_MESSAGE(nRegAckMsg, OnRegAckMsg)

	///////////////////////////////////////////////////////////////////////


END_MESSAGE_MAP()



// CTestIPCDlg 메시지 처리기

BOOL CTestIPCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetRegstryMessage();
	SetDlgState(IPC_INITIALIZING);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CTestIPCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE)
	{
		if (MessageBox("Are you sure ?",
			"Question",
			MB_YESNO | MB_ICONQUESTION)
			== IDNO)
			return;
		else EndofProcess();
	}

	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CTestIPCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CTestIPCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestIPCDlg::OnSendMessage()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (!m_stMessage.IsEmpty())
	{
		SetTimer(1, 2000, NULL);
		m_nAckReady = 0;

		SendData();
		m_stMessage = "";

		(CEdit*)GetDlgItem(IDC_EDIT_MSG)->SetFocus();

		//////////////////////// fill the blank ///////////////////////////////
			
		::SendMessage(HWND_BROADCAST, nRegSendMsg, 0, 0);
		

		///////////////////////////////////////////////////////////////////////

	}

	UpdateData(FALSE);
}

void CTestIPCDlg::OnButtonAddrSet()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (!m_unDstAddr ||
		!m_unSrcAddr)
	{
		MessageBox("주소를 설정 오류발생",
			"경고",
			MB_OK | MB_ICONSTOP);

		return;
	}

	if (m_bSendReady) {
		SetDlgState(IPC_ADDR_RESET);
		SetDlgState(IPC_INITIALIZING);
	}
	else {
		m_ChatApp->SetSourceAddress(m_unSrcAddr);
		m_ChatApp->SetDestinAddress(m_unDstAddr);

		SetDlgState(IPC_ADDR_SET);
		SetDlgState(IPC_READYTOSEND);
	}

	m_bSendReady = !m_bSendReady;
}

void CTestIPCDlg::OnCheckBroadcast()
{
	// TODO: Add your control notification handler code here
	CButton* pChkButton = (CButton*)GetDlgItem(IDC_CHECK_TOALL);

	if (pChkButton->GetCheck())	SetDlgState(IPC_BROADCASTMODE);
	else							SetDlgState(IPC_UNICASTMODE);
}

void CTestIPCDlg::SetRegstryMessage()
{
	nRegSendMsg = RegisterWindowMessage("Send IPC Message");
	//////////////////////// fill the blank ///////////////////////////////
	nRegAckMsg = RegisterWindowMessage("ACK IPC MSG");
	///////////////////////////////////////////////////////////////////////
}

void CTestIPCDlg::SendData()
{
	CString MsgHeader;
	if (m_unDstAddr == (unsigned int)0xff)
		MsgHeader.Format("[%d:BROADCAST] ", m_unSrcAddr);
	else
		MsgHeader.Format("[%d:%d] ", m_unSrcAddr, m_unDstAddr);

	m_ListChat.AddString(MsgHeader + m_stMessage);

	//////////////////////// fill the blank ///////////////////////////////
	int nlength = m_stMessage.GetLength();
	unsigned char* ppayload = new unsigned char[nlength + 1];
	memcpy(ppayload, (unsigned char*)(LPCTSTR)m_stMessage, nlength);
	ppayload[nlength] = '\0';

	m_ChatApp->Send(ppayload, m_stMessage.GetLength());
	///////////////////////////////////////////////////////////////////////
}

BOOL CTestIPCDlg::Receive(unsigned char *ppayload)
{
	if (m_nAckReady == -1)
	{
		//////////////////////// fill the blank ///////////////////////////////
		//                            ...
		//                            ...
		//                            ...
		///////////////////////////////////////////////////////////////////////
	}

	m_ListChat.AddString((char*)ppayload);

	return TRUE;
}


void CTestIPCDlg::SetDlgState(int state)
{
	UpdateData(TRUE);

	CButton*	pChkButton = (CButton*)GetDlgItem(IDC_CHECK_TOALL);

	CButton*	pSendButton = (CButton*)GetDlgItem(IDC_BUTTON_SEND);
	CButton*	pSetAddrButton = (CButton*)GetDlgItem(IDC_BUTTON_ADDR);
	CEdit*		pMsgEdit = (CEdit*)GetDlgItem(IDC_EDIT_MSG);
	CEdit*		pSrcEdit = (CEdit*)GetDlgItem(IDC_EDIT_SRC);
	CEdit*		pDstEdit = (CEdit*)GetDlgItem(IDC_EDIT_DST);

	switch (state)
	{
	case IPC_INITIALIZING:
		pSendButton->EnableWindow(FALSE);
		pMsgEdit->EnableWindow(FALSE);
		m_ListChat.EnableWindow(FALSE);
		break;
	case IPC_READYTOSEND:
		pSendButton->EnableWindow(TRUE);
		pMsgEdit->EnableWindow(TRUE);
		m_ListChat.EnableWindow(TRUE);
		break;
	case IPC_WAITFORACK:	break;
	case IPC_ERROR:		break;
	case IPC_UNICASTMODE:
		m_unDstAddr = 0x0;
		pDstEdit->EnableWindow(TRUE);
		break;
	case IPC_BROADCASTMODE:
		m_unDstAddr = 0xff;
		pDstEdit->EnableWindow(FALSE);
		break;
	case IPC_ADDR_SET:
		pSetAddrButton->SetWindowText("재설정(&R)");
		pSrcEdit->EnableWindow(FALSE);
		pDstEdit->EnableWindow(FALSE);
		pChkButton->EnableWindow(FALSE);
		break;
	case IPC_ADDR_RESET:
		pSetAddrButton->SetWindowText("설정(&O)");
		pSrcEdit->EnableWindow(TRUE);
		if (!pChkButton->GetCheck())
			pDstEdit->EnableWindow(TRUE);
		pChkButton->EnableWindow(TRUE);
		break;
	}

	UpdateData(FALSE);
}


void CTestIPCDlg::EndofProcess()
{
	m_LayerMgr.DeAllocLayer();
}

LRESULT CTestIPCDlg::OnRegSendMsg(WPARAM wParam, LPARAM lParam)
{
	//////////////////////// fill the blank ///////////////////////////////
	if(m_nAckReady)
		if (m_LayerMgr.GetLayer("File")->Receive()){
			::SendMessage(HWND_BROADCAST, nRegAckMsg, 0, 0);

}
	///////////////////////////////////////////////////////////////////////

	return 0;
}

LRESULT CTestIPCDlg::OnRegAckMsg(WPARAM wParam, LPARAM lParam)
{
	if (!m_nAckReady) {
		m_nAckReady = -1;
		KillTimer(1);
	}

	return 0;
}

void CTestIPCDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	m_ListChat.AddString(">> The last message was time-out..");
	m_nAckReady = -1;
	KillTimer(1);

	CDialog::OnTimer(nIDEvent);
}

