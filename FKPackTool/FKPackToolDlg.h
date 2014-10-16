
// FKPackToolDlg.h : ͷ�ļ�
//

#pragma once


// CFKPackToolDlg �Ի���
class CFKPackToolDlg : public CDialogEx
{
// ����
public:
	CFKPackToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FKPACKTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
public:
	CString m_strSource;
	CString m_strDestination;
	CFKPacket* m_pFKPacket;
	CListCtrl* m_pFileList;
private:
	int	__CheckSourceFileName(CString strFileName);
	int __CheckDestinationFileName(CString strFileName);
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};