// StreamingTransmit.h : StreamingTransmit DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CStreamingTransmitApp
// �йش���ʵ�ֵ���Ϣ������� StreamingTransmit.cpp
//

class CStreamingTransmitApp : public CWinApp
{
public:
	CStreamingTransmitApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
