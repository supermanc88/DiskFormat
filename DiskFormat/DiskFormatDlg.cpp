
// DiskFormatDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "DiskFormat.h"
#include "DiskFormatDlg.h"
#include "afxdialogex.h"

#include "DiskOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CDiskFormatDlg 对话框



CDiskFormatDlg::CDiskFormatDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DISKFORMAT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDiskFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_REMOVEABLE_DISK_LIST, m_comboBox);
	DDX_Control(pDX, IDC_STATIC_FORMAT_STATUS, m_staticFromatStatus);
}

BEGIN_MESSAGE_MAP(CDiskFormatDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_DISK_FORMAT, &CDiskFormatDlg::OnBnClickedButtonDiskFormat)
END_MESSAGE_MAP()


// CDiskFormatDlg 消息处理程序

BOOL CDiskFormatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//初始化移动介质列表
	char letterList[MAX_PATH] = { 0 };
	DWORD count;
	if (!GetAllDrive(letterList, &count))
	{
		MessageBox(L"Init fail", L"Error", MB_OK);
		ExitProcess(0);
	}

	for (int i = 0; i < count; i++)
	{
		WORD type = 0;
		char drive[MAX_PATH] = { 0 };
		sprintf(drive, "\\\\.\\%c:", letterList[i]);
		GetDriveTypeByBus(drive, &type);
		if (type == 7) //BusTypeUsb
		{
			m_comboBox.AddString(CString(letterList[i]));
		}
	}
	


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDiskFormatDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDiskFormatDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDiskFormatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



HBRUSH CDiskFormatDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_NOTICE)
	{
		pDC->SetTextColor(RGB(0, 0, 250));
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CDiskFormatDlg::OnBnClickedButtonDiskFormat()
{
	// TODO: 在此添加控件通知处理程序代码

	// 获取选择的盘符
	int nIndex = m_comboBox.GetCurSel();
	CString strCBText;
	m_comboBox.GetLBText(nIndex, strCBText);

	//获取所选择盘符所在物理磁盘上的所有分区驱动器
	PWCHAR cbBuffer = strCBText.GetBuffer();
	char currentLetter[MAX_PATH] = { 0 };
	wcstombs(currentLetter, cbBuffer, MAX_PATH);



	DWORD physicalDriveNumber = 0;
	GetPhysicalNum(currentLetter[0], &physicalDriveNumber);

	//判断是否有保留扇区
	if (HaveReserveSectors(physicalDriveNumber))
	{
		//用于查看物理磁盘号
// 		CString str;
// 		str.Format(L"physicalDriveNum:%d", physicalDriveNumber);
// 		MessageBox(str, NULL, NULL);
		MessageBox(L"此介质已不需要制作", L"信息", MB_OK);
		return;
	}

	DRIVEINFO driveInfo = { 0 };
	GetPhysicalDriveInfo(physicalDriveNumber, &driveInfo);

	CHAR letterList[MAX_PATH] = { 0 };
	DWORD count = 0;
	GetLetterFromPhysicalDrive(physicalDriveNumber, letterList, &count);

	CString strMsg;
	strMsg.Format(L"本操作会格式化该磁盘上的所有分区,请确认", letterList);

	int msgRet = MessageBox(strMsg, L"信息", MB_OKCANCEL);

	if (msgRet == IDOK)
	{
		//开始格式化
		SetDlgItemTextW(IDC_STATIC_FORMAT_STATUS, L"开始格式化，请耐心等待");

		//操作之前先解除锁定，以免出问题
		for (int i = 0; i < count; i++)
		{
			UnlockVolume(letterList[i]);
		}


		//锁定和卸载所有卷
		for (int i = 0; i < count; i++)
		{
			LockVolum(letterList[i]);
			DisMountVolum(letterList[i]);
		}

		DWORD ret = DestroyDisk(physicalDriveNumber);

		if (ret != 0)
		{
			MessageBox(L"扇区分区信息失败", L"信息", MB_OK);
		}



		//开始写磁盘
		WritePhysicalDrive(letterList[0], physicalDriveNumber, &driveInfo);

//		WritePhysicalDrive1(physicalDriveNumber);



		//格式化磁盘
 		DiskFormat(letterList[count-1]);


		//格式化之后解锁
		for (int i = 0; i < count; i++)
		{
			UnlockVolume(letterList[i]);
		}


		//重新加载所有盘
		GetLogicalDrives();

		SetDlgItemTextW(IDC_STATIC_FORMAT_STATUS, L"格式化完成");
	}
	else
	{
		//没有点击确认按钮，不格式化磁盘
		MessageBox(L"1", L"2", MB_OK);
	}

	//弹出提示框 提示该物理磁盘上的所有分区均被格式化

}
