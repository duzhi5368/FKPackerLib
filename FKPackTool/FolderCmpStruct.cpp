#include "stdafx.h"
#include "FolderCmpStruct.h"
//////////////////////////////////////////////////////////////////////////
// Default file compare function
// Support a default compare function
int FCS_DefaultFileCmp(
  LPARAM lpNode,
  LPARAM lpszLeftFile,
  LPARAM lpszRightFile
 )
{ 
  ASSERT((lpNode != NULL) && (lpszLeftFile != NULL) && (lpszRightFile != NULL));
  if((lpNode == NULL) || (lpszLeftFile == NULL) || (lpszRightFile == NULL))
    return FALSE;
 
  CFldCmpNodePtr pNode = (CFldCmpNodePtr)lpNode;
  if (pNode->m_bFolder) // Folder ? 
    return 0;
  DWORD dwSizeLeft = pNode->m_dwLeftSize, dwSizeRight = pNode->m_dwRightSize;

  int nRst = 0;
  BOOL bContinue = FALSE;
  if (dwSizeLeft == dwSizeRight)
  {
	// TODO:
    if (!bContinue)    return 0;
  }
 
  // No same, compare it by the time
  if(pNode->m_dtLeft > pNode->m_dtRight)     nRst = 1;
  else if (pNode->m_dtLeft < pNode->m_dtRight)    nRst = -1;
  else nRst = 0;

  return nRst;
}

//////////////////////////////////////////////////////////////////////////
// class CFldCmpFileNode

// Constructor
CFldCmpFileNode::CFldCmpFileNode()
{
  m_strName = _T("");
  m_bFolder = FALSE;
  m_dtModified = COleDateTime::GetCurrentTime();
  m_dwSize = 0;

  // Used to build the tree
  m_pFather = NULL;
  m_arChildPtrs.SetSize(0);
  m_nLevel = (UINT)-1;

  // Used for arrange
  m_dwData = 0;
}

// Decomstructor
CFldCmpFileNode::~CFldCmpFileNode() { CleanData(); }

// Copy constructor
CFldCmpFileNode::CFldCmpFileNode(const CFldCmpFileNode &fn) {   *this = fn; }

// Override operator =
CFldCmpFileNode &CFldCmpFileNode::operator =(const CFldCmpFileNode &fn)
{
  if ( this != &fn)
  {
    m_strName = fn.m_strName;
    m_bFolder = fn.m_bFolder;
    m_dtModified = fn.m_dtModified;
    m_dwSize = fn.m_dwSize;

    // Used to build the tree
    m_pFather = fn.m_pFather;
    m_arChildPtrs.RemoveAll();
    m_arChildPtrs.Copy(fn.m_arChildPtrs);
    m_nLevel = fn.m_nLevel;

    // Used for arrange
    m_dwData = fn.m_dwData;
  }

  return *this;
}

// Clean data
void CFldCmpFileNode::CleanData()
{
  for(int i = 0; i < m_arChildPtrs.GetSize(); i++)
  {
    CFldCmpFileNodePtr pNode = m_arChildPtrs.GetAt(i);
    ASSERT(pNode != NULL);
    pNode->CleanData();
    FCS_SAFEDEL(pNode);
 }

  m_arChildPtrs.RemoveAll();
}

// Get node family number
UINT CFldCmpFileNode::GetFamilyNodeNumber() const
{
  UINT nCount = 0;
  for(int i = 0; i < m_arChildPtrs.GetSize(); i ++)
    nCount += m_arChildPtrs.GetAt(i)->GetFamilyNodeNumber();
 
  nCount += 1; // Himself
  return nCount;
}

//////////////////////////////////////////////////////////////////////////
// class CFldCmpFileTree

// Constructor
CFldCmpFileTree::CFldCmpFileTree() { m_arRootPtrs.SetSize(0);}

// Decomstructor
CFldCmpFileTree::~CFldCmpFileTree() { CleanData();}

// Get node number
UINT CFldCmpFileTree::GetNodeNumber() const
{
  UINT nCount = 0;
  for(int i = 0; i < m_arRootPtrs.GetSize(); i++)
    nCount += m_arRootPtrs.GetAt(i)->GetFamilyNodeNumber();

  return nCount;
}

// Parse folder
BOOL CFldCmpFileTree::ParseFolder(
  LPCTSTR lpszPath, 
  CFldCmpFileNodePtr pFa, 
  UINT nLevel
 )
{
  ASSERT(lpszPath != NULL);
  if(lpszPath == NULL)    return FALSE;

  CString str = lpszPath;
  if (str.IsEmpty())   return FALSE;
 
  str.TrimRight();
  str.TrimRight(_T('//'));
  str += _T("//*.*");
 
  // start working for files
  CFileFind finder;
  BOOL bWorking = finder.FindFile(str);
  while (bWorking)
  {
     bWorking = finder.FindNextFile();
  
     if (finder.IsDots())    continue;
  
     WIN32_FILE_ATTRIBUTE_DATA fad;
     CFldCmpFileNodePtr pNode = new CFldCmpFileNode;
     ASSERT(pNode != NULL);
     if (pNode == NULL)   return FALSE;

     pNode->m_strName = finder.GetFileName();
     pNode->m_nLevel = nLevel;
     pNode->m_pFather = pFa;

     if (finder.IsDirectory())
     {
        pNode->m_bFolder = TRUE;
   
       // Add it into the array   
       if (pFa == NULL)    m_arRootPtrs.Add(pNode);
      else    pFa->m_arChildPtrs.Add(pNode);
   
       // Recursiving...
      if (!ParseFolder(
        finder.GetFilePath(),
        pNode,
        nLevel + 1))
         return FALSE;
     }
     else
     {
		 pNode->m_bFolder = FALSE;

		 if (GetFileAttributesEx(finder.GetFilePath(), GetFileExInfoStandard, &fad) != FALSE)
		 {
			 pNode->m_dtModified = (COleDateTime)fad.ftLastWriteTime;
			 pNode->m_dwSize = FCS_GetFileSize(finder.GetFilePath());
		 }

		 // Add it
		 if (pFa == NULL)    m_arRootPtrs.Add(pNode);
		 else    pFa->m_arChildPtrs.Add(pNode);
	 }  
  }

  return TRUE;
}

// Clean data
void CFldCmpFileTree::CleanData()
{
  for(int i = 0; i < m_arRootPtrs.GetSize(); i++)
  {
    CFldCmpFileNodePtr pNode = m_arRootPtrs.GetAt(i);
    ASSERT(pNode != NULL);
    pNode->CleanData();
    FCS_SAFEDEL(pNode);
  }

  m_arRootPtrs.RemoveAll();
}

#ifdef _DEBUG
// Display debug info
void CFldCmpFileTree::DisplayDebugInfo(CFldCmpFileNodePtr pFather)
{
  CFldCmpFileNodePtrArray &rNods = (pFather == NULL)? m_arRootPtrs : pFather->m_arChildPtrs;
  for( int i = 0; i < rNods.GetSize(); i ++)
    DisplayDebugInfo(rNods.GetAt(i));
 
  // Then clean itselft
  if(pFather != NULL)
  {
    TRACE("Tree Level %d: %s, %s, Father %s, Size %d, Children number %d/n",
       pFather->m_nLevel, 
       pFather->m_strName,
       pFather->m_bFolder? _T("Folder") : _T("File"),
        (pFather->m_pFather == NULL) ? _T("None") : pFather->m_pFather->m_strName,
       pFather->m_dwSize,
       pFather->m_arChildPtrs.GetSize()
      );
   }
}
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////////
// class CFldCmpNode

// Constructor
CFldCmpNode::CFldCmpNode()
{
	m_type = FCT_MATCH;   // Type
	m_strName = _T("");   // Name
	m_dtLeft = COleDateTime::GetCurrentTime();  // Left last modified time
	m_dtRight = COleDateTime::GetCurrentTime();  // Right last modified time
	m_dwLeftSize = 0;   // Left file size
	m_dwRightSize = 0;   // Right file size
	m_bFolder = FALSE;   // Folder or not

	// Used to build the tree
	m_pFather = NULL;   // Pointer of father node
	m_arChildPtrs.SetSize(0); // Children node pointer array
	m_nLevel = (UINT)-1;   // Level, from the root

	// Used data
	m_dwData = 0;
}

CFldCmpNode::~CFldCmpNode() {  CleanData(); }

CFldCmpNode::CFldCmpNode(const CFldCmpNode &cn)  {  *this = cn; }

CFldCmpNode&CFldCmpNode::operator =(const CFldCmpNode &cn)
{
	if ( this != &cn)
	{
		m_type = cn.m_type;     // Type
		m_strName = cn.m_strName;   // Name
		m_dtLeft = cn.m_dtLeft;    // Left last modified time
		m_dtRight = cn.m_dtRight;   // Right last modified time
		m_dwLeftSize = cn.m_dwLeftSize;  // Left file size
		m_dwRightSize = cn.m_dwRightSize; // Right file size
		m_bFolder = cn.m_bFolder;   // Folder or not

		// Used to build the tree
		m_pFather = cn.m_pFather;   // Pointer of father node
		m_arChildPtrs.RemoveAll();
		m_arChildPtrs.Copy(cn.m_arChildPtrs);
		m_nLevel = cn.m_nLevel;    // Level, from the root

		// Used for arrange
		m_dwData = cn.m_dwData;
	}

	return *this;
}

// Clean data
void CFldCmpNode::CleanData()
{
	for(int i = 0; i < m_arChildPtrs.GetSize(); i++)
	{
		CFldCmpNodePtr pNode = m_arChildPtrs.GetAt(i);
		ASSERT(pNode != NULL);
		pNode->CleanData();
		FCS_SAFEDEL(pNode);
	}

	m_arChildPtrs.RemoveAll();
}

// Get family nodes
UINT CFldCmpNode::GetFamilyNodeNumber() const
{
	UINT nCount = 0;
	for(int i = 0; i < m_arChildPtrs.GetSize(); i ++)
		nCount += m_arChildPtrs.GetAt(i)->GetFamilyNodeNumber();

	nCount += 1; // Himself
	return nCount;
}

// Compare
int CFldCmpNode::Compare(const CFldCmpNode &nc) const
{
	int nRst = 0;
	if (m_bFolder && (!nc.m_bFolder))  nRst = -1;
	else if((!m_bFolder) && (nc.m_bFolder))  nRst = 1;
	else  {  nRst = lstrcmpi(m_strName, nc.m_strName); }

	return nRst;
}

//////////////////////////////////////////////////////////////////////////
// class CFldCmpTree

// Constructor
CFldCmpTree::CFldCmpTree()
{
	m_arRootPtrs.SetSize(0);
	m_pFnFileCmp = FCS_DefaultFileCmp;
}

// Deconstructor
CFldCmpTree::~CFldCmpTree() {  CleanData(); }

// Clean data
void CFldCmpTree::CleanData()
{
	for(int i = 0; i < m_arRootPtrs.GetSize(); i++)
	{
		CFldCmpNodePtr pNode = m_arRootPtrs.GetAt(i);
		ASSERT(pNode != NULL);
		pNode->CleanData();
		FCS_SAFEDEL(pNode);
	}

	m_arRootPtrs.RemoveAll();
}

#ifdef _DEBUG
// Display tree info
void CFldCmpTree::DisplayDebugInfo(CFldCmpNodePtr pFather)
{
	CFldCmpNodePtrArray &rNods = (pFather == NULL)?m_arRootPtrs : pFather->m_arChildPtrs;

	// Then clean itselft
	if(pFather != NULL)
	{
		printf("Tree Level %d: %s, %s, Father %s, Left Size %d, Right Size %d, Children number %d\n",
			pFather->m_nLevel, 
			pFather->m_strName,
			pFather->m_bFolder? _T("Folder") : _T("File"),
			(pFather->m_pFather == NULL) ? _T("None") : pFather->m_pFather->m_strName,
			pFather->m_dwLeftSize,
			pFather->m_dwRightSize,
			pFather->m_arChildPtrs.GetSize()
			);
	}
	for( int i = 0; i < rNods.GetSize(); i ++)
		DisplayDebugInfo(rNods.GetAt(i));
}
#endif // _DEBUG

// Copy left file tree
BOOL CFldCmpTree::CopyLeftFileTree(
	const CFldCmpFileNodePtrArray &arNodePtrs,
	CFldCmpNodePtr pFather
	)
{
	int nSize = arNodePtrs.GetSize();

	int i = 0;
	for(; i < nSize; i ++)
	{
		// Add it
		CFldCmpFileNodePtr pFile = arNodePtrs.GetAt(i);
		CFldCmpNodePtr pNode = new CFldCmpNode;
		ASSERT(pNode != NULL);
		if (pNode == NULL)
		{
			CleanData();
			return FALSE;
		}
		pNode->m_strName = pFile->m_strName;
		pNode->m_bFolder = pFile->m_bFolder;
		pNode->m_dtLeft = pFile->m_dtModified;
		pNode->m_dwLeftSize = pFile->m_dwSize;
		pNode->m_nLevel = pFile->m_nLevel;
		pNode->m_pFather = pFather;
		pNode->m_type = FCT_LORPHAN;
		if(pFather != NULL)
			pFather->m_arChildPtrs.Add(pNode);
		else
			m_arRootPtrs.Add(pNode);

		// Add it's children
		CopyLeftFileTree(pFile->m_arChildPtrs, pNode);
	} 

	return TRUE;
}

// Get node number
UINT CFldCmpTree::GetNodeNumber() const
{
	UINT nCount = 0;
	for(int i = 0; i < m_arRootPtrs.GetSize(); i++)
		nCount += m_arRootPtrs.GetAt(i)->GetFamilyNodeNumber();

	return nCount;
}

// Combine the right tree
BOOL CFldCmpTree::CombineRightTree(
	const CFldCmpFileNodePtrArray &arFileNodePtrs, 
	vector<SPackChangeInfo>& vecDiffenect,
	CFldCmpNodePtr pFather
	)
{
	CFldCmpNodePtrArray &arNodes = (pFather == NULL) ? m_arRootPtrs 
		: pFather->m_arChildPtrs;
	BOOL bExist = FALSE;
	int i = 0;
	int nArSize = arNodes.GetSize();

	vector<int> vecIndexList;
	vecIndexList.clear();
	for(; i < arFileNodePtrs.GetSize(); i ++)
	{
		// Process each element
		CFldCmpFileNodePtr pFileNode = arFileNodePtrs.GetAt(i);
		CFldCmpNodePtr pNode = NULL;

		bExist = FALSE;
		int j = 0;

		for(; j < nArSize; j ++)
		{   
			if (arNodes.GetAt(j)->m_strName == pFileNode->m_strName)
			{
				vecIndexList.push_back( j );
				bExist = TRUE;
				break;
			}
		}

		if (bExist)
		{
			// Set the info
			pNode = arNodes.ElementAt(j);
			pNode->m_dtRight = pFileNode->m_dtModified;
			pNode->m_dwRightSize = pFileNode->m_dwSize;
			CString strLeft = GetNodeFullPath(pNode);
			CString strRight = GetNodeFullPath(pNode, FALSE);
			ASSERT(m_pFnFileCmp != NULL);
			int nRst = (m_pFnFileCmp)((LPARAM)pNode, 
				(LPARAM)(LPCTSTR)strLeft,
				(LPARAM)(LPCTSTR)strRight);   
			if(nRst == 0)
				pNode->m_type = FCT_MATCH;
			else if (nRst > 0 )
			{
				pNode->m_type = FCT_LNEW;

				SPackChangeInfo tagInfo;
				tagInfo.m_szFileFullName = strRight;
				tagInfo.m_szNewFileName = strRight;
				tagInfo.m_eFileState = eFileState_Change;
				vecDiffenect.push_back( tagInfo );
			}
			else
			{
				pNode->m_type = FCT_RNEW;  

				SPackChangeInfo tagInfo;
				tagInfo.m_szFileFullName = strLeft;
				tagInfo.m_szNewFileName = strRight;
				tagInfo.m_eFileState = eFileState_Change;
				vecDiffenect.push_back( tagInfo );
			}
		}
		else
		{
			pNode = new CFldCmpNode;
			ASSERT(pNode != NULL);
			pNode->m_type = FCT_RORPHAN;
			pNode->m_dtRight = pFileNode->m_dtModified;
			pNode->m_dwRightSize = pFileNode->m_dwSize;
			pNode->m_nLevel = pFileNode->m_nLevel;
			pNode->m_strName = pFileNode->m_strName;
			pNode->m_bFolder = pFileNode->m_bFolder;
			pNode->m_pFather = pFather;   
			arNodes.Add(pNode);   

			for(; j < arNodes.GetSize(); j ++)
			{   
				if (arNodes.GetAt(j)->m_strName == pFileNode->m_strName)
				{
					vecIndexList.push_back( j );
					break;
				}
			}

			CString strRight = GetNodeFullPath(pNode, FALSE);
			SPackChangeInfo tagInfo;
			tagInfo.m_szFileFullName = strRight;
			tagInfo.m_eFileState = eFileState_Add;
			vecDiffenect.push_back( tagInfo );
		}

		// The children pointers
		CombineRightTree(pFileNode->m_arChildPtrs, vecDiffenect, pNode);
	}

	bool bFind = false;
	for( int i = 0; i < arNodes.GetSize(); ++i )
	{
		bFind = false;
		for( int j = 0; j < vecIndexList.size(); ++j )
		{
			if( vecIndexList[j] == i )
			{
				bFind = true;
				break;
			}
		}
		if( !bFind )
		{
			CString strName = GetNodeFullPath( arNodes.GetAt(i));
			SPackChangeInfo tagInfo;
			tagInfo.m_szFileFullName = strName;
			tagInfo.m_eFileState = eFileState_Delete;
			vecDiffenect.push_back( tagInfo );
		}
	}

	return TRUE;
}

// Combine the file tree
BOOL CFldCmpTree::CombineFileTrees(
	const CFldCmpFileTree &treeLeft, 
	const CFldCmpFileTree &treeRight,
	vector<SPackChangeInfo>& vecDiffenect
	)
{
	CleanData();

	// First: Copy the left
	CopyLeftFileTree(treeLeft.GetRootArray());

	// Trace info
	printf("\nAfter copy left file tree... ===================================\n");
	printf("\tTotal number is %d... ======================\n\n", GetNodeNumber());
	DisplayDebugInfo();
	printf("\n");

	// Second: Combine the right tree
	CombineRightTree(treeRight.GetRootArray(), vecDiffenect);
	// Trace info
	printf("\nAfter combine right file tree... ===================================\n");
	printf("\tTotal number is %d... ======================\n\n", GetNodeNumber());
	DisplayDebugInfo();
	printf("\n");

	return TRUE;
}

// Sort the array
void CFldCmpTree::SortArray(
	CFldCmpNodePtrArray &arArray
	)
{ 
	int nSize = arArray.GetSize();
	int i = 0;

	// Sort it
	for(; i < nSize; i ++)
	{
		for(int j = i + 1; j < nSize; j ++)
		{   
			CFldCmpNodePtr pNode1 = arArray.ElementAt(i);
			CFldCmpNodePtr pNode2 = arArray.ElementAt(j);
			if (pNode1->Compare(*pNode2) > 0)
			{
				// Exchange
				CFldCmpNode node;
				node = *pNode1;
				*pNode1 = *pNode2;
				*pNode2 = node;
				node.m_arChildPtrs.RemoveAll();
			}
		}
	}

	// Recursive sort
	for( i = 0; i < nSize; i ++)  {     SortArray(arArray.ElementAt(i)->m_arChildPtrs);  }
}

// Sort
void CFldCmpTree::Sort() {   SortArray(m_arRootPtrs); }

// Parse folder
BOOL CFldCmpTree::ParseFolder(LPCTSTR lpszLeft, LPCTSTR lpszRight, vector<SPackChangeInfo>& vecDiffenect)
{
	ASSERT((lpszLeft != NULL) && (lpszRight != NULL));
	if((lpszLeft == NULL) || (lpszRight == NULL))
		return FALSE;

	m_strLeftPath = lpszLeft;
	m_strRightPath = lpszRight;
	if (m_strLeftPath.IsEmpty() || m_strRightPath.IsEmpty())
	{
		m_strLeftPath.Empty();
		m_strRightPath.Empty();
		return FALSE;
	}

	CFldCmpFileTree treeLeft, treeRight;
	treeLeft.ParseFolder(lpszLeft);
	treeRight.ParseFolder(lpszRight);

	return CombineFileTrees(treeLeft, treeRight, vecDiffenect);
}

// Get node full path
CString CFldCmpTree::GetNodeFullPath(const CFldCmpNodePtr pNode, BOOL bLeft) const
{
	ASSERT(pNode != NULL);

	CString str = _T("");
	if(pNode->m_pFather != NULL)
		str = GetNodeFullPath(pNode->m_pFather, bLeft);
	else
		str = bLeft ? m_strLeftPath : m_strRightPath;

	str.TrimRight();
	str.TrimRight(_T("//"));
	str += _T("\\");
	str += pNode->m_strName;

	return str;
}

// Set custom compare function
void CFldCmpTree::SetCustomSortFunc(PFNFCCMPFUNC pFunc)
{
	ASSERT(pFunc != NULL);
	m_pFnFileCmp = pFunc;
}