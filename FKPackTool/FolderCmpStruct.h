//*************************************************************************
//	创建日期:	2014-10-16   20:02
//	文件名称:	FolderCmpStruct.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include <bitset>
#include <string>
#include <fstream>
#include <strstream>
#include <vector>
using namespace std;

// Include <afxtempl.h> or not
#if !defined(__AFXTEMPL_H__)
#include  <afxtempl.h>
#endif // !__AFXTEMPL_H__

#define FCS_SAFEDEL(p) if (p != NULL){ delete p; p = NULL; }

inline DWORD FCS_GetFileSize(LPCTSTR lpszFile)
{
	ASSERT(lpszFile != NULL);
	if (lpszFile == NULL)
		return 0;

	ifstream in(lpszFile);
	ASSERT(in != NULL);
	in.seekg(0, ios::end);
	streampos pos = in.tellg();
	return (DWORD)pos; 
}

//////////////////////////////////////////////////////////////////////////
// Declarations

// Folder compare function
typedef int (*PFNFCCMPFUNC)(LPARAM, LPARAM, LPARAM);

class CFldCmpFileNode;
class CFldCmpNode;
typedef CFldCmpFileNode *CFldCmpFileNodePtr;
typedef CFldCmpNode *CFldCmpNodePtr;
typedef CTypedPtrArray<CPtrArray, CFldCmpFileNodePtr> CFldCmpFileNodePtrArray;
typedef CTypedPtrArray<CPtrArray, CFldCmpNodePtr> CFldCmpNodePtrArray;

// Compare item type
typedef enum
{
	FCT_MATCH,  // Match
	FCT_LNEW,  // Left newer
	FCT_RNEW,  // Right newer
	FCT_LORPHAN, // Left is orphane
	FCT_RORPHAN  // Right is orpane
}
FldCmpType;


struct SPackChangeInfo
{
	CString			m_szFileFullName;
	CString			m_szNewFileName;
	ENUM_SubPackFileState	m_eFileState;
};
//////////////////////////////////////////////////////////////////////////
// class CFldCmpFileNode
class CFldCmpFileNode
{
	// Constructor
public:
	CFldCmpFileNode();
	virtual ~CFldCmpFileNode();
	// ===========================================================================
	// Note :
	// If you use the Copy Constructor or operator '=' to assign the Node, it may
	// leads the memory, for it not release the children.
	// In fact, I strongly suggest use the CFldCmpFileTree instead of build a 
	// tree manualy.
	// ===========================================================================
	CFldCmpFileNode(const CFldCmpFileNode &fn);
	CFldCmpFileNode &operator =(const CFldCmpFileNode &fn);

	// Operation
public:
	// Clean data
	void CleanData();
	// Get family nodes
	UINT GetFamilyNodeNumber() const;

	// Members
public:
	// File item
	CString  m_strName;
	BOOL  m_bFolder;
	COleDateTime m_dtModified;
	DWORD  m_dwSize;

	// Used to build the tree
	CFldCmpFileNode *m_pFather;
	CFldCmpFileNodePtrArray m_arChildPtrs;
	UINT  m_nLevel;

	// No used
	DWORD  m_dwData;
};

//////////////////////////////////////////////////////////////////////////
// class CFldCmpFileTree
class CFldCmpFileTree
{
	// Constructor
public:
	CFldCmpFileTree();
	virtual ~CFldCmpFileTree();

	// Opertion
public:
	// Parse folder
	BOOL ParseFolder(
		LPCTSTR lpszPath, 
		CFldCmpFileNodePtr pFa = NULL, 
		UINT nLevel = 0
		);
	// Clean data
	void CleanData();
	// Get node number
	UINT GetNodeNumber() const;
	// Get root array
	const CFldCmpFileNodePtrArray &GetRootArray() const {  return m_arRootPtrs; }

#ifdef _DEBUG
	// Display tree info
	void DisplayDebugInfo(CFldCmpFileNodePtr pFather = NULL);
#endif // _DEBUG

	// Members
protected:
	// Root item array
	CFldCmpFileNodePtrArray m_arRootPtrs;
};

//////////////////////////////////////////////////////////////////////////
// class CFldCmpNode
class CFldCmpNode
{
	// Constructor
public:
	CFldCmpNode();
	virtual ~CFldCmpNode();
	CFldCmpNode(const CFldCmpNode &cn);
	CFldCmpNode &operator =(const CFldCmpNode &cn);

	// Operation
public:
	// Clean data
	void CleanData();
	// Get family nodes
	UINT GetFamilyNodeNumber() const;
	// Compare two node, used for sort
	int Compare(const CFldCmpNode &nc) const;

	// Members
public:
	FldCmpType  m_type;   // Type
	CString   m_strName;  // Name
	COleDateTime m_dtLeft;  // Left last modified time
	COleDateTime m_dtRight;  // Right last modified time
	DWORD   m_dwLeftSize; // Left file size
	DWORD   m_dwRightSize; // Right file size
	BOOL   m_bFolder;  // Folder or not

	// Used to build the tree
	CFldCmpNodePtr m_pFather;   // Pointer of father node
	CFldCmpNodePtrArray m_arChildPtrs; // Children node pointer array
	UINT   m_nLevel;   // Level, from the root

	// No used
	DWORD   m_dwData;
};

//////////////////////////////////////////////////////////////////////////
// class CFldCmpTree
class CFldCmpTree
{
	// Constrcution
public:
	CFldCmpTree();
	virtual ~CFldCmpTree();

	// Interface
public:
	// Clean data
	void CleanData();
#ifdef _DEBUG
	// Display tree info
	void DisplayDebugInfo(CFldCmpNodePtr pFather = NULL);
#endif // _DEBUG
	// Get node number
	UINT GetNodeNumber() const;
	// Combine file tree
	BOOL ParseFolder(
		LPCTSTR lpszLeft,
		LPCTSTR lpszRight,
		vector<SPackChangeInfo>& vecDiffenect
		);
	// Get root array
	const CFldCmpNodePtrArray &GetRootArray() const {  return m_arRootPtrs; }
	// Sort 
	void Sort();
	// Compare function 
	void SetCustomSortFunc(PFNFCCMPFUNC);
	// Get node full path
	CString GetNodeFullPath(
		const CFldCmpNodePtr pNode,
		BOOL bLeft = TRUE
		) const;

	// Operations
protected:
	// Combine file tree
	BOOL CombineFileTrees(
		const CFldCmpFileTree &treeLeft,
		const CFldCmpFileTree &treeRight,
		vector<SPackChangeInfo>& vecDiffenect
		);
	// Copy left file tree
	BOOL CopyLeftFileTree(
		const CFldCmpFileNodePtrArray &arNodePtrs,
		CFldCmpNodePtr pFather = NULL
		);
	// Combine right file tree
	BOOL CombineRightTree(
		const CFldCmpFileNodePtrArray &arFileNodePtrs,
		vector<SPackChangeInfo>& vecDiffenect,
		CFldCmpNodePtr pFather = NULL
		);
	// Sort function
	void SortArray(  CFldCmpNodePtrArray &arArray  );

	// Members
protected:
	// Root item array
	CFldCmpNodePtrArray m_arRootPtrs;
	// File's compare function
	PFNFCCMPFUNC m_pFnFileCmp;
	// Left path
	CString m_strLeftPath;
	// Right path
	CString m_strRightPath;
};
//-------------------------------------------------------------------------