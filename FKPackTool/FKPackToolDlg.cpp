
// FKPackToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FKPackTool.h"
#include "FKPackToolDlg.h"
#include "afxdialogex.h"
#include "FolderCmpStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFKPackToolDlg 对话框



CFKPackToolDlg::CFKPackToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFKPackToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFKPackToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFKPackToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CFKPackToolDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CFKPackToolDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CFKPackToolDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CFKPackToolDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CFKPackToolDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CFKPackToolDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CFKPackToolDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CFKPackToolDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CFKPackToolDlg::OnBnClickedButton9)
END_MESSAGE_MAP()


// CFKPackToolDlg 消息处理程序

BOOL CFKPackToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_pFKPacket = new CFKPacket();
	m_pFileList = (CListCtrl*)GetDlgItem( IDC_LIST1 );
	m_pFileList->SetExtendedStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFKPackToolDlg::OnPaint()
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
HCURSOR CFKPackToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFKPackToolDlg::OnBnClickedButton1()
{
	BROWSEINFO binfo;
	memset(&binfo,0x00,sizeof(binfo));
	binfo.hwndOwner=GetSafeHwnd();
	TCHAR szSrcPath[MAX_PATH]={0};
	binfo.lpszTitle=_T("请选择源文件/目录");
	binfo.ulFlags=BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;//BIF_BROWSEINCLUDEFILES;//BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST lpDlist;
	lpDlist=SHBrowseForFolder(&binfo);
	if (NULL!=lpDlist)
	{
		SHGetPathFromIDList(lpDlist,szSrcPath);
		m_vecStrSource.clear();
		m_strSource=szSrcPath;
		GetDlgItem( IDC_EDIT1 )->SetWindowText( m_strSource );
	}
}


void CFKPackToolDlg::OnBnClickedButton2()
{
	TCHAR szFilter[] = _T("FKPacket包文件 (*.PAK)||");
	CFileDialog FileDlg(FALSE,
		_T("pak"),
		NULL,
		OFN_HIDEREADONLY,
		szFilter,
		NULL,
		0,
		TRUE);
	if (IDOK==FileDlg.DoModal())
	{
		int n = m_pFileList->GetHeaderCtrl()->GetItemCount();
		for( int i = 0; i < n; ++i )
		{
			m_pFileList->DeleteColumn( 0 );
		}
		m_pFileList->DeleteAllItems();
		m_pFileList->InsertColumn(0, "文件路径", LVCFMT_LEFT, 200);
		m_pFileList->InsertColumn(1, "源文件大小", LVCFMT_LEFT, 90);
		m_pFileList->InsertColumn(2, "压缩后大小", LVCFMT_LEFT, 90);
		m_pFileList->InsertColumn(3, "压缩比", LVCFMT_LEFT, 60);
		m_pFileList->InsertColumn(4, "修改类型", LVCFMT_LEFT, 60);

		m_vecStrDestination.clear();
		m_strDestination=FileDlg.GetPathName();
		GetDlgItem( IDC_EDIT2 )->SetWindowText( m_strDestination );
	}
}

// 打包
void CFKPackToolDlg::OnBnClickedButton3()
{
	CString strMsg;
	if( m_pFKPacket == NULL )
	{
		strMsg=_T("不可预料的错误，请重启本软件");
		MessageBox(strMsg);
		return;
	}
	if( m_vecStrDestination.size() > 0 && m_vecStrSource.size() > 0 && 
		m_vecStrDestination.size() == m_vecStrSource.size() )
	{
		for( unsigned int i = 0; i < m_vecStrSource.size(); ++i )
		{
			m_pFKPacket->Clear();
			if( !m_pFKPacket->CreatePAK( LPCSTR(m_vecStrDestination[i]), LPCSTR(m_vecStrSource[i]) ) )
			{
				m_pFKPacket->Clear();
				strMsg=_T("压缩失败，请联系开发人员");
				MessageBox(strMsg);
				return;
			}
		}
	}
	else
	{
		int nSrcType=__CheckSourceFileName(m_strSource);
		if (-1 == nSrcType)
		{
			strMsg=_T("请选择资源文件/目录");
			MessageBox(strMsg);
			return;
		}
		int nDesType=__CheckDestinationFileName(m_strDestination);
		if (-1 == nDesType)
		{
			strMsg=_T("请选择目标文件");
			MessageBox(strMsg);
			return;
		}

		m_pFKPacket->Clear();
		if( !m_pFKPacket->CreatePAK( LPCSTR(m_strDestination), LPCSTR(m_strSource) ) )
		{
			m_pFKPacket->Clear();
			strMsg=_T("压缩失败，请联系开发人员");
			MessageBox(strMsg);
			return;
		}
	}

	m_pFKPacket->Clear();
	strMsg=_T("压缩完成");
	MessageBox(strMsg);
	return;
}


int CFKPackToolDlg::__CheckSourceFileName(CString strFileName)
{
	if (""==strFileName)
		return -1;

	if (PathIsDirectory(strFileName))
	{
		return 0;
	}
	else
	{
		// 检查文件是否存在
		CFileStatus FileStatus;
		if (!CFile::GetStatus(strFileName,FileStatus))
		{
			return -1;
		}

		return 0;
	}
	return -1;
}

int CFKPackToolDlg::__CheckDestinationFileName(CString strFileName)
{
	if (""==strFileName)
		return -1;

	if (PathIsDirectory(strFileName))
	{
		return -1;
	}
	else
	{
		// 检查目录是否正确
		int nPosition=strFileName.Find('/');
		if (-1==nPosition)
		{
			nPosition=strFileName.Find('\\');
		}
		if (-1==nPosition)
		{
			return -1;
		}
		CString strPath=strFileName.Left(nPosition);
		if (PathIsDirectory(strPath))
		{
			return 0;
		}

		return -1;
	}
	return -1;
}

// 查看包文件
void CFKPackToolDlg::OnBnClickedButton4()
{
	CString strMsg;
	int nDesType=__CheckDestinationFileName(m_strDestination);
	if (-1 == nDesType)
	{
		strMsg=_T("请选择包文件");
		MessageBox(strMsg);
		return;
	}
	m_pFKPacket->Clear();
	if( !m_pFKPacket->ReadPAK( LPCSTR(m_strDestination) ) )
	{
		m_pFKPacket->Clear();
		strMsg=_T("读取包文件失败，请联系开发人员");
		MessageBox(strMsg);
		return;
	}

	int n = m_pFileList->GetHeaderCtrl()->GetItemCount();
	for( int i = 0; i < n; ++i )
	{
		m_pFileList->DeleteColumn( 0 );
	}
	m_pFileList->DeleteAllItems();
	m_pFileList->InsertColumn(0, "文件路径", LVCFMT_LEFT, 200);
	m_pFileList->InsertColumn(1, "源文件大小", LVCFMT_LEFT, 90);
	m_pFileList->InsertColumn(2, "压缩后大小", LVCFMT_LEFT, 90);
	m_pFileList->InsertColumn(3, "压缩比", LVCFMT_LEFT, 60);
	m_pFileList->InsertColumn(4, "修改类型", LVCFMT_LEFT, 60);

	vector<string> vecFileList = m_pFKPacket->GetAllFileNameInPAK();
	vector<string>::iterator Ite = vecFileList.begin();
	CString tmp;

	int nSrcTotal = 0;
	int nDstTotal = 0;
	for( ; Ite != vecFileList.end(); ++Ite )
	{
		int nIndex = 0;
		m_pFileList->InsertItem( 0, (*Ite).c_str(), 0 );
		int nSrcSize = m_pFKPacket->GetFileSize((*Ite).c_str());
		nSrcTotal += nSrcSize;
		tmp.Format("%d b", nSrcSize);
		m_pFileList->SetItemText( 0, ++nIndex, tmp );
		int nDstSize = m_pFKPacket->GetFileCompressSize((*Ite).c_str());
		nDstTotal += nDstSize;
		tmp.Format("%d b", nDstSize );
		m_pFileList->SetItemText( 0, ++nIndex, tmp );
		int nValue = nDstSize * 100 / nSrcSize;
		tmp.Format("%d%%", nValue );
		m_pFileList->SetItemText( 0, ++nIndex, tmp );

		ENUM_SubPackFileState nType = m_pFKPacket->GetFileChangeType((*Ite).c_str());
		switch ( nType )
		{
		case eFileState_Unknown:
			m_pFileList->SetItemText( 0, ++nIndex, _T("正常") );
			break;
		case eFileState_Delete:
			m_pFileList->SetItemText( 0, ++nIndex, _T("删除") );
			break;
		case eFileState_Add:
			m_pFileList->SetItemText( 0, ++nIndex, _T("添加") );
			break;
		case eFileState_Change:
			m_pFileList->SetItemText( 0, ++nIndex, _T("更改") );
			break;
		default:
			m_pFileList->SetItemText( 0, ++nIndex, _T("未知") );
			break;
		}
	}

	m_pFileList->InsertItem( 0, _T("总计"), 0 );
	tmp.Format("%d Kb", nSrcTotal / 1000);
	m_pFileList->SetItemText( 0, 1, tmp );
	tmp.Format("%d Kb", nDstTotal / 1000);
	m_pFileList->SetItemText( 0, 2, tmp );
	float fValue = (float)( nDstTotal / 1000 ) / (float)(nSrcTotal / 1000);
	tmp.Format("%.2f%%", fValue * 100.0f );
	m_pFileList->SetItemText( 0, 3, tmp );
	return;
}

// 多目录配置加载
void CFKPackToolDlg::OnBnClickedButton5()
{
	TCHAR szFilter[] = _T("源目录配置文件 (*.cfg)||");
	CFileDialog FileDlg(FALSE,
		_T("cfg"),
		NULL,
		OFN_HIDEREADONLY,
		szFilter,
		NULL,
		0,
		TRUE);
	if (IDOK==FileDlg.DoModal())
	{
		m_strSource="";
		m_strDestination="";
		m_vecStrSource.clear();
		m_vecStrDestination.clear();

		CString szCfg;
		szCfg=FileDlg.GetPathName();
		int nDirNum = 0;
		nDirNum = GetPrivateProfileInt( "需要打包的文件夹", "总数", 0, szCfg );
		for( int i = 0; i < nDirNum; ++i )
		{
			CString szTmp;
			CString szSectionName;
			szSectionName.Format("%d", i+1 );
			GetPrivateProfileString( "需要打包的文件夹", szSectionName, NULL, szTmp.GetBuffer(100), 100, szCfg );
			m_vecStrSource.push_back( szTmp );
			

			CString szDstTmp;
			szDstTmp.Format("%s.pak", szTmp );
			m_vecStrDestination.push_back( szDstTmp );
		}


		for( unsigned int i = 0; i < m_vecStrSource.size(); ++i )
		{
			CString tmp = m_strSource;
			if( i == 0 )
			{
				m_strSource.Format( "%s", m_vecStrSource[i] );
			}
			else
			{
				m_strSource.Format( "%s%s", tmp, m_vecStrSource[i] );
			}
			m_strSource += ";";
		}
		GetDlgItem( IDC_EDIT1 )->SetWindowText( m_strSource );

		for( unsigned int i = 0; i < m_vecStrDestination.size(); ++i )
		{
			CString tmp = m_strDestination;
			if( i == 0 )
			{
				m_strDestination.Format( "%s", m_vecStrDestination[i] );
			}
			else
			{
				m_strDestination.Format( "%s%s", tmp, m_vecStrDestination[i] );
			}
			m_strDestination += ";";
		}
		GetDlgItem( IDC_EDIT2 )->SetWindowText( m_strDestination );
	}
}

// 选择补丁包
void CFKPackToolDlg::OnBnClickedButton6()
{
	TCHAR szFilter[] = _T("FK补丁包文件 (*.PAK)||");
	CFileDialog FileDlg(FALSE,
		_T("pak"),
		NULL,
		OFN_HIDEREADONLY,
		szFilter,
		NULL,
		0,
		TRUE);
	if (IDOK==FileDlg.DoModal())
	{
		int n = m_pFileList->GetHeaderCtrl()->GetItemCount();
		for( int i = 0; i < n; ++i )
		{
			m_pFileList->DeleteColumn( 0 );
		}
		m_pFileList->DeleteAllItems();
		m_pFileList->InsertColumn(0, "文件路径", LVCFMT_LEFT, 200);
		m_pFileList->InsertColumn(1, "源文件大小", LVCFMT_LEFT, 90);
		m_pFileList->InsertColumn(2, "压缩后大小", LVCFMT_LEFT, 90);
		m_pFileList->InsertColumn(3, "压缩比", LVCFMT_LEFT, 60);
		m_pFileList->InsertColumn(4, "修改类型", LVCFMT_LEFT, 60);


		m_strPack=FileDlg.GetPathName();
		GetDlgItem( IDC_EDIT3 )->SetWindowText( m_strPack );

		CString strMsg;
		int nDesType=__CheckDestinationFileName(m_strPack);
		if (-1 == nDesType)
		{
			strMsg=_T("读取补丁包文件失败，请联系开发人员");
			MessageBox(strMsg);
			return;
		}
		m_pFKPacket->Clear();
		if( !m_pFKPacket->ReadPAK( LPCSTR(m_strPack) ) )
		{
			m_pFKPacket->Clear();
			strMsg=_T("读取补丁包文件失败，请联系开发人员");
			MessageBox(strMsg);
			return;
		}

		vector<string> vecFileList = m_pFKPacket->GetAllFileNameInPAK();
		vector<string>::iterator Ite = vecFileList.begin();
		CString tmp;

		int nSrcTotal = 0;
		int nDstTotal = 0;
		for( ; Ite != vecFileList.end(); ++Ite )
		{
			int nIndex = 0;
			m_pFileList->InsertItem( 0, (*Ite).c_str(), 0 );
			int nSrcSize = m_pFKPacket->GetFileSize((*Ite).c_str());
			nSrcTotal += nSrcSize;
			tmp.Format("%d b", nSrcSize);
			m_pFileList->SetItemText( 0, ++nIndex, tmp );
			int nDstSize = m_pFKPacket->GetFileCompressSize((*Ite).c_str());
			nDstTotal += nDstSize;
			tmp.Format("%d b", nDstSize );
			m_pFileList->SetItemText( 0, ++nIndex, tmp );
			int nValue = nDstSize * 100 / nSrcSize;
			tmp.Format("%d%%", nValue );
			m_pFileList->SetItemText( 0, ++nIndex, tmp );

			ENUM_SubPackFileState nType = m_pFKPacket->GetFileChangeType((*Ite).c_str());
			switch ( nType )
			{
			case eFileState_Unknown:
				m_pFileList->SetItemText( 0, ++nIndex, _T("正常") );
				break;
			case eFileState_Delete:
				m_pFileList->SetItemText( 0, ++nIndex, _T("删除") );
				break;
			case eFileState_Add:
				m_pFileList->SetItemText( 0, ++nIndex, _T("添加") );
				break;
			case eFileState_Change:
				m_pFileList->SetItemText( 0, ++nIndex, _T("更改") );
				break;
			default:
				m_pFileList->SetItemText( 0, ++nIndex, _T("未知") );
				break;
			}
		}

		m_pFileList->InsertItem( 0, _T("总计"), 0 );
		tmp.Format("%d Kb", nSrcTotal / 1000);
		m_pFileList->SetItemText( 0, 1, tmp );
		tmp.Format("%d Kb", nDstTotal / 1000);
		m_pFileList->SetItemText( 0, 2, tmp );
		float fValue = (float)( nDstTotal / 1000 ) / (float)(nSrcTotal / 1000);
		tmp.Format("%.2f%%", fValue * 100.0f );
		m_pFileList->SetItemText( 0, 3, tmp );
		return;
	}
}

// 合并到主包
void CFKPackToolDlg::OnBnClickedButton7()
{
	CString strMsg;
	if( m_strPack.IsEmpty() )
	{
		strMsg=_T("请选择补丁包");
		MessageBox(strMsg);
		return;
	}
	if( m_strDestination.IsEmpty() )
	{
		strMsg=_T("请选择主包，注意，不要同时处理多个");
		MessageBox(strMsg);
		return;
	}

	m_pFKPacket->Clear();
	if( !m_pFKPacket->ReadPAK( LPCSTR(m_strDestination) ) )
	{
		m_pFKPacket->Clear();
		strMsg=_T("读取主包文件失败，请联系开发人员");
		MessageBox(strMsg);
		return;
	}

	if( !m_pFKPacket->MergePAH( LPCSTR(m_strPack) ) )
	{
		m_pFKPacket->Clear();
		strMsg=_T("合并补丁包文件失败，请联系开发人员");
		MessageBox(strMsg);
		return;
	}

	strMsg=_T("合包完成，请联系开发人员");
	MessageBox(strMsg);
	return;
}

// 选择文件夹
void CFKPackToolDlg::OnBnClickedButton8()
{
	BROWSEINFO binfo;
	memset(&binfo,0x00,sizeof(binfo));
	binfo.hwndOwner=GetSafeHwnd();
	TCHAR szSrcPath[MAX_PATH]={0};
	binfo.lpszTitle=_T("请选择对比源文件/目录");
	binfo.ulFlags=BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;//BIF_BROWSEINCLUDEFILES;//BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST lpDlist;
	lpDlist=SHBrowseForFolder(&binfo);
	if (NULL!=lpDlist)
	{
		SHGetPathFromIDList(lpDlist,szSrcPath);
		m_strNewSourceDir=szSrcPath;
		GetDlgItem( IDC_EDIT4 )->SetWindowText( m_strNewSourceDir );

		CString strMsg;
		if( m_strSource.IsEmpty() )
		{
			strMsg=_T("请选择源文件夹");
			MessageBox(strMsg);
			return;
		}
		if( m_strNewSourceDir.IsEmpty() )
		{
			strMsg=_T("请选择对比文件夹");
			MessageBox(strMsg);
			return;
		}

		CFldCmpTree tree;
		tree.ParseFolder( m_strSource, m_strNewSourceDir, m_vecPackChangeInfo );

		int n = m_pFileList->GetHeaderCtrl()->GetItemCount();
		for( int i = 0; i < n; ++i )
		{
			m_pFileList->DeleteColumn( 0 );
		}
		m_pFileList->DeleteAllItems();
		m_pFileList->InsertColumn(0, "更变类型", LVCFMT_LEFT, 60);
		m_pFileList->InsertColumn(1, "更变的文件名", LVCFMT_LEFT, 350);

		vector<SPackChangeInfo>::iterator Ite = m_vecPackChangeInfo.begin();
		for( ; Ite != m_vecPackChangeInfo.end(); ++Ite )
		{
			int nIndex = 0;
			switch ( (*Ite).m_eFileState )
			{
			case eFileState_Add:
				m_pFileList->InsertItem( 0, "增加", 0 );
				break;
			case eFileState_Delete:
				m_pFileList->InsertItem( 0, "删除", 0 );
				break;
			case eFileState_Change:
				m_pFileList->InsertItem( 0, "更变", 0 );
				break;
			default:
				break;
			}

			m_pFileList->SetItemText( 0, ++nIndex, (*Ite).m_szFileFullName );
		}
	}
}

// 生成补丁包
void CFKPackToolDlg::OnBnClickedButton9()
{
	CString strMsg;
	if( m_vecPackChangeInfo.empty() )
	{
		strMsg=_T("请选择原文件夹和对比文件夹");
		MessageBox(strMsg);
		return;
	}
	if( m_pFKPacket == NULL )
	{
		strMsg=_T("不可预料的错误，请重启本软件");
		MessageBox(strMsg);
		return;
	}

	CEdit* pVersion = (CEdit*)( GetDlgItem(IDC_EDIT5));
	if( pVersion == NULL )
	{
		strMsg=_T("不可预料的错误，请重启本软件");
		MessageBox(strMsg);
		return;
	}
	CString szVersion;
	pVersion->GetWindowText(szVersion);
	int nVersion = _ttoi(szVersion);

	m_pFKPacket->Clear();

	char chPath[MAX_PATH];
	GetModuleFileName(NULL,chPath,MAX_PATH); //得到执行程序的文件名（包括路径）；
	CString szExePath;
	szExePath.Format("%s",chPath); //转换成字符串；
	int nPos=szExePath.ReverseFind('\\');//从右边找到第一个“\\”字符，返回其数组下标的位置
	szExePath=szExePath.Left(nPos+1); //保留字符串的前nPos+1个字符（包括“\\”）；

	szVersion.Format("%sPATCH_%d.pak", szExePath, nVersion);
	vector<SPatchFileInfo> vecFileList;
	CString noDirFileName;
	for( unsigned int i = 0; i < m_vecPackChangeInfo.size(); ++i )
	{
		SPatchFileInfo tmp;
		tmp.m_eFileState = m_vecPackChangeInfo[i].m_eFileState;
		noDirFileName = m_vecPackChangeInfo[i].m_szFileFullName;
		if( noDirFileName.Find( m_strSource ) >= 0  )
		{
			noDirFileName.Delete( noDirFileName.Find(m_strSource), m_strSource.GetLength()+1 );
		}
		else if( noDirFileName.Find( m_strNewSourceDir ) >= 0 )
		{
			noDirFileName.Delete( noDirFileName.Find(m_strNewSourceDir), m_strNewSourceDir.GetLength()+1 );
		}
		strcpy( tmp.m_szFillName,  noDirFileName );
		strcpy( tmp.m_szFileFullPath, m_vecPackChangeInfo[i].m_szFileFullName );
		vecFileList.push_back( tmp );
	}
	if( !m_pFKPacket->CreatePAH( LPCSTR(szVersion), vecFileList, nVersion ) )
	{
		m_pFKPacket->Clear();
		strMsg=_T("打包失败，请联系开发人员");
		MessageBox(strMsg);
		return;
	}

	strMsg=_T("打包完成");
	MessageBox(strMsg);
	return;
}
