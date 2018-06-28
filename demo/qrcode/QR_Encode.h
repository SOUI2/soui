// QR_Encode.h : CQR_Encode 僋儔僗愰尵偍傛傃僀儞僞乕僼僃僀僗掕媊
// Date 2006/05/17	Ver. 1.22	Psytec Inc.

#if !defined(AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD37__INCLUDED_)
#define AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// 掕悢

// 更正水平 
#define QR_LEVEL_L	0
#define QR_LEVEL_M	1
#define QR_LEVEL_Q	2
#define QR_LEVEL_H	3

// /数据模式 
#define QR_MODE_NUMERAL		0
#define QR_MODE_ALPHABET	1
#define QR_MODE_8BIT		2
#define QR_MODE_KANJI		3

// 版本（模型）组 
#define QR_VRESION_S	0 // 1~9 
#define QR_VRESION_M	1 // 10 ~ 26
#define QR_VRESION_L	2 // 27 ~ 40

#define MAX_ALLCODEWORD	 3706 // 最大的码字总数 
#define MAX_DATACODEWORD 2956 // 最大数据码字（版本40 - L）的 
#define MAX_CODEBLOCK	  153 // 最大数量 Burokkudetakodowado（RS码字等） 
#define MAX_MODULESIZE	  177 // 模块的最大数量的边 

// 绘制位图在边际 
#define QR_MARGIN	4


/////////////////////////////////////////////////////////////////////////////
typedef struct tagRS_BLOCKINFO
{
	int ncRSBlock;		// 遥感块 
	int ncAllCodeWord;	// 块的码字 
	int ncDataCodeWord;	// 数字数据码字（字数的代码 - RS码字数）


} RS_BLOCKINFO, *LPRS_BLOCKINFO;


/////////////////////////////////////////////////////////////////////////////
//  QR码版本（模型）的信息 

typedef struct tagQR_VERSIONINFO
{
	int nVersionNo;	   // 版本（型号）的数量（1-40） 
	int ncAllCodeWord; // 总人数的码字 

	// 下面是一个数组索引错误矫正率 (0 = L, 1 = M, 2 = Q, 3 = H) 
	int ncDataCodeWord[4];	//数据码字（码字数量的总数 - RS码字数） 

	int ncAlignPoint;	// 数字坐标Araimentopatan 
	int nAlignPoint[6];	// 中心坐标Araimentopatan 

	RS_BLOCKINFO RS_BlockInfo1[4]; // 遥感信息块（1） 
	RS_BLOCKINFO RS_BlockInfo2[4]; // 遥感块信息（2） 

} QR_VERSIONINFO, *LPQR_VERSIONINFO;


/////////////////////////////////////////////////////////////////////////////
// CQR_Encode 僋儔僗

class CQR_Encode
{
// 峔抸/徚柵
public:
	CQR_Encode();
	~CQR_Encode();

public:
	int m_nLevel;		//错误更正水平 
	int m_nVersion;		// 版本（型号） 
	BOOL m_bAutoExtent;	// 版本（型号）标志指定自动扩展 
	int m_nMaskingNo;	// /掩蔽模式的数量 

public:
	int m_nSymbleSize;
	BYTE m_byModuleData[MAX_MODULESIZE][MAX_MODULESIZE]; // [x][y]
	// Bit5：功能模块（不包括由掩蔽）标志
	// Bit4：显示数据的功能模块
	// 04期Bit1：编码数据
	// Bit0：经过编码掩模图形数据
	//通过逻辑和功能模块，11小时，并提请逻辑（布尔和最终的判断20小时） 

private:
	int m_ncDataCodeWordBit; // 长度Detakodowadobitto 
	BYTE m_byDataCodeWord[MAX_DATACODEWORD]; // 输入Detaenkodoeria


	int m_ncDataBlock;
	BYTE m_byBlockMode[MAX_DATACODEWORD];
	int m_nBlockLength[MAX_DATACODEWORD];

	int m_ncAllCodeWord; // 总人数的码字（遥感数据，包括错误修正） 
	BYTE m_byAllCodeWord[MAX_ALLCODEWORD]; // / /计算总面积三字代码 
	BYTE m_byRSWork[MAX_CODEBLOCK]; // RS码字计算工作 

// 功能相关的数据编码 
public:
	BOOL EncodeData(int nLevel, int nVersion, BOOL bAutoExtent, int nMaskingNo, LPCSTR lpsSource, int ncSource = 0);

private:
	int GetEncodeVersion(int nVersion, LPCSTR lpsSource, int ncLength);
	BOOL EncodeSourceData(LPCSTR lpsSource, int ncLength, int nVerGroup);

	int GetBitLength(BYTE nMode, int ncData, int nVerGroup);

	int SetBitStream(int nIndex, WORD wData, int ncData);

	BOOL IsNumeralData(unsigned char c);
	BOOL IsAlphabetData(unsigned char c);
	BOOL IsKanjiData(unsigned char c1, unsigned char c2);

	BYTE AlphabetToBinaly(unsigned char c);
	WORD KanjiToBinaly(WORD wc);

	void GetRSCodeWord(LPBYTE lpbyRSWork, int ncDataCodeWord, int ncRSCodeWord);

// 功能模块相关的安置 
private:
	void FormatModule();

	void SetFunctionModule();
	void SetFinderPattern(int x, int y);
	void SetAlignmentPattern(int x, int y);
	void SetVersionPattern();
	void SetCodeWordPattern();
	void SetMaskingPattern(int nPatternNo);
	void SetFormatInfoPattern(int nPatternNo);
	int CountPenalty();
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD37__INCLUDED_)
