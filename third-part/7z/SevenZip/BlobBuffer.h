/********************************************************************
  Created Date: 2009-11-05
  Author: wuding
  Version: 2.0
  Last Version Date: 2010-06-13 
  Modified History:
  version Date      Author    Description  
  2009-11-05        wuding     version 1.0  加入堆内存，为了缓解内存碎片问题
  2010-06-13        wuding     version 2.0  修正几个LoadFromFile引起的bug,重写IncreaseSize,SetBlobContent等方法，优化内存分配。
  2012-12-13        wuding     version 3.0  调整接口名称以适应image
	
********************************************************************/

#pragma once

#ifndef __MEMORY_BUFFER_H
#define __MEMORY_BUFFER_H 
 
#pragma warning(disable: 4786)


class   BlobBuffer
{
public:
	enum { BLOB_BUFFER_BLOCK_SIZE = 8 * 1204 };
private:
	unsigned char *m_BlobPtr; //实际数据指针
	unsigned long m_lBlobLen;          //实际数据总大小
	unsigned long m_lBufferLen;          //缓冲区总大小 
	unsigned long m_ReadLen;   //读取位置
	unsigned char m_BlobBuffer[BLOB_BUFFER_BLOCK_SIZE];

	void Reset();
public:
	BlobBuffer();
	~BlobBuffer();
	void Clear();
	void ClearContent();
	void ResetRead();
	/*
	将缓冲区再扩大nIncreaseSize,bCopyOldData==false表示，分配新的空间后，旧的数据不要拷贝到新空间上
	bAllocBufLen true表示设置缓冲内容长度，false表示函数调完后不影响原有GetBlobLength
	*/
	void IncreaseBufferSize(unsigned long nIncreaseSize, bool bCopyOldData = true);
	void AllocBuffer(unsigned long nBufSize);
	unsigned long GetBlobLength() const;
	unsigned long GetBlobBufferLength();
	const unsigned char *GetBlobPtr();
	unsigned char *GetBlobRealPtr();
	void SetBlobContent(const unsigned char *BlobPtr, unsigned long nBlobLen);
	void AppendBlobContent(const unsigned char *BlobPtr, unsigned long nBlobLen);
	void AppendBlobContentChar(const unsigned char BlobChar);
	bool LoadBlobFromFile(const char *sFileName);
	bool SaveBlobToFile(const char *sFileName);
	bool LoadBlobFromFile(const wchar_t *sFileName);
	bool SaveBlobToFile(const wchar_t *sFileName);
};

#endif