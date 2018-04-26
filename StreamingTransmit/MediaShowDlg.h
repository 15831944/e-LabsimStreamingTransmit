#pragma once
#include "afxwin.h"


// CMediaShowDlg 对话框

class CMediaShowDlg : public CDialog
{
	DECLARE_DYNAMIC(CMediaShowDlg)

public:
	CMediaShowDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMediaShowDlg();

// 对话框数据
	enum { IDD = IDD_DATADLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	enum IN_PortName{IN1=0, IN2, IN3, IN4, IN5, IN6, IN7, IN8, IN9, IN10};
	enum OUT_PortName{OUT1=0, OUT2, OUT3, OUT4, OUT5, OUT6, OUT7, OUT8, OUT9, OUT10};
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	void RunAlgorithm( const double *pdInput, double *pdOutput );
	CEdit m_TextInputEdit;
	CEdit m_TextShowEdit;
private:
	int m_nSendBufferSize;
	int m_nFrameEndCount;
	double m_nClkState;
	int m_nBitCount;
	int m_nOutCount;
	char m_nSendBuffer[4096];
	int m_nFrameHeaderCount;

	bool m_bFrameHeader;
	double m_nReClkState;
	int m_nFrameRevCount;
	int m_nRevBitCount;
	int m_nRevBitBuffer[8];
	int m_RevByteBuffer[4096];
	int m_nRevByteCount;
	bool m_bRevOK;
	char m_RevBuffer[4096];

	double m_RevTestBuffer[64];
	int m_nTestCount;
};
