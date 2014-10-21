//*************************************************************************
//	创建日期:	2014-10-14   19:10
//	文件名称:	FKPacket.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include <string>
#include <vector>
#include <stdio.h>
#include "../ZIP/zlib.h"
//-------------------------------------------------------------------------
using namespace std;
//-------------------------------------------------------------------------
typedef char FKCHAR;
enum ENUM_SubPackFileState
{
	eFileState_Unknown		= 0,
	eFileState_Delete		= 1,
	eFileState_Add			= 2,
	eFileState_Change		= 3,
};
struct SPatchFileInfo
{
	FKCHAR					m_szFileFullPath[256];
	FKCHAR					m_szFillName[128];
	ENUM_SubPackFileState	m_eFileState;
};
//-------------------------------------------------------------------------
class CFKPacket
{
private:
	// 包头
	struct SPacketHead
	{
		FKCHAR				m_szFileID[6];				// 标头 FKPAK
		FKCHAR				m_szVersion[4];				// 版本号
		int					m_nFileNum;					// 文件个数
		bool				m_bIsUseEncrypt;			// 是否对每字节进行加密
		FKCHAR				m_cEncryptVal;				// 加密单元
		bool				m_bIsZipComperssed;			// 是否zip压缩了
		bool				m_bIsSubPack;				// 是否是补丁包
		int					m_nCurVersion;				// 当前版本号
	};
	// 文件单元信息
	struct SFileEntry
	{
		FKCHAR				m_szFileName[128];			// 文件名 或者是 LeftFileName
		FKCHAR				m_szFileFullPath[256];		// 完整的文件路径 或者是 RightFileName
		unsigned int		m_unSize;					// 文件大小
		unsigned int		m_unCompressedSize;			// 文件压缩后的大小
		unsigned int		m_unOffset;					// 文件在pak中的偏移量
		ENUM_SubPackFileState		m_eState;					// 该信息仅对补丁包有效
	};
	// 文件尾
	struct SFileTail
	{
		unsigned int		m_unFileEntryOffset;		// 文件单元信息距文件头的偏移量
	};
	enum ENUM_FileState
	{
		eFS_Deleted			= -1,
		eFS_Normal			= 0,
		eFS_Added			= 1,
		eFS_Changed			= 2,
	};
private:
	string					m_szPakName;				// 包名			
	bool					m_bIsLoadedPak;				// 是否已经加载本包
	SPacketHead				m_tagPakHead;				// 包头
	vector<SFileEntry>		m_vecFileEntries;			// 文件上下文表
	vector<ENUM_FileState>	m_vecChanges;				// 文件状态
	SFileTail				m_tagFileTail;				// 文件尾信息
private:
	// 字符串分割，获得需要的文件类型
	vector<string>			__FileTypes( string p_szTypes );
	// 创建文件单元信息
	bool					__CreateEntry( string p_szRootPath, string p_szRelativePath, 
		string p_szFileName, ENUM_SubPackFileState p_eType = eFileState_Unknown );
	bool					__CreateEntry( string p_szFileFullName, string p_szFileName,
		ENUM_SubPackFileState p_eType = eFileState_Unknown );
	// 获取子文件夹列表
	vector<string>			__GetSubDir( string p_szParentDir, string p_szRootDir );
	// 判断一个文件是否需要压缩（因为图片格式压缩率过低，不划算，默认不压缩）
	bool					__IsNeedCompress( string p_szFileName );
public:
	CFKPacket();
	~CFKPacket();
public:
	// 打包一个pak
	// 如果需要对指定种类的文件进行打包，第三个参数可如下类似传入 ".jpg|.png|.bmp"
	bool					CreatePAK( string p_szPakName, string p_szSrcRootPath,
		bool p_bIsUseCompress = true, bool p_bIsEntry = false, string p_szType = "" );
	// 打包一个补丁pak
	bool					CreatePAH( string p_szPakName, vector<SPatchFileInfo> p_vecPathFile,
		int p_nVersion, bool p_bIsUseCompress = true, bool p_bIsEntry = false );
	// 加载一个pak
	bool					ReadPAK( string p_szPakPath );
	// 合并一个pak
	bool					MergePAH( string p_szPahPath );
public:
	//----------------------------------------------------------
	// 下列函数必须在 ReadPAK() 之后才有意义
	//----------------------------------------------------------
	// 为pak中添加/删除一个文件
	// 注意：调用后请调用 RebuildPAK() 来保证重新打包
	bool					AppendFile( string p_szRootPath, string p_szRelativePath, string p_szFileName );
	bool					AppendFile( string p_szFileName, string p_szFilePathName );
	bool					RemoveFile( string p_szFileName, string p_szFilePathName );
	bool					ChangeFile( string p_szFileName, string p_szFilePathName );
	// 获取补丁号
	int						GetCurPathVersion();
	// 重新打包
	// 注意：若包内信息无任何更改，依然返回false
	bool					RebuildPAK();
	// 查找并获取一个文件指针（解压后）
	FKCHAR*					GetFileDataFromPAK( string p_szFileName );
	// 查找并获取一个文件信息在PAK中的位置指针
	SFileEntry*				GetFileInfoFromPAK( string p_szFileName );
	// 获得某文件在包内大小
	int						GetFileSize( string p_szName );
	int						GetFileCompressSize( string p_szName );
	// 获取某文件的修改类型
	ENUM_SubPackFileState	GetFileChangeType( string p_szName );
	// 获得包内全部文件名称列表
	vector<string>			GetAllFileNameInPAK();
	vector<string>			GetAllFilePathNameInPAK();
	// 获取包内文件数量
	int						GetFileNumInPAK();
	// 解包一个PAK内的文件
	bool					UnpakEntry( string p_szFileName, string p_szFilePath );
	// 清空全部数据
	void					Clear();
};
//-------------------------------------------------------------------------