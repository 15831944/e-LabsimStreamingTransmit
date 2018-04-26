// MediaShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "StreamingTransmit.h"
#include "MediaShowDlg.h"
#include "utility.h"

// CMediaShowDlg 对话框

IMPLEMENT_DYNAMIC(CMediaShowDlg, CDialog)

CMediaShowDlg::CMediaShowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMediaShowDlg::IDD, pParent)
{
	m_nClkState = 0;
	m_nSendBufferSize = 0;
	m_nFrameHeaderCount = 0;
	m_nBitCount = 0;
	m_nOutCount = 0;
	m_nFrameEndCount = 0;
	m_bRevOK = FALSE;
	m_bFrameHeader = FALSE;
	m_nReClkState = 0;
	m_nFrameRevCount = 0;
	m_nRevBitCount = 0;
	memset( m_nRevBitBuffer, 0, sizeof( m_nRevBitBuffer ) );
	memset( m_RevByteBuffer, 0, sizeof( m_RevByteBuffer ) );
	m_nRevByteCount = 0;
	m_bRevOK = false;
	memset( m_RevBuffer, 0, sizeof( m_RevBuffer ) );
	m_nTestCount = 0;
}

CMediaShowDlg::~CMediaShowDlg()
{
}

void CMediaShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_TextInputEdit);
	DDX_Control(pDX, IDC_EDIT2, m_TextShowEdit);
}


BEGIN_MESSAGE_MAP(CMediaShowDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CMediaShowDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMediaShowDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CMediaShowDlg 消息处理程序

void CMediaShowDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
//	OnOK();
	CString strText;
	m_TextInputEdit.GetWindowText( strText );
	std::string SendBuffer = utility::WideCharToMultiChar(strText.GetString());
	memset( m_nSendBuffer, 0, sizeof(m_nSendBuffer) );
	m_nSendBufferSize = SendBuffer.size();
	if( m_nSendBufferSize < 4096 )
	{
		strcpy_s( m_nSendBuffer, SendBuffer.c_str() );
		m_nSendBuffer[m_nSendBufferSize] = 0;
	}
	else
		m_nSendBufferSize = 0;
}

void CMediaShowDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
//	OnCancel();
	ShowWindow( SW_HIDE );
}

void CMediaShowDlg::RunAlgorithm( const double *pdInput, double *pdOutput )
{
	//输入时钟
	//数据发送
	double nClk = pdInput[IN1];
	if( nClk > 1.0 && m_nClkState < 1.0 )	//时钟上升沿输出数据
	{
		if( m_nOutCount == 0 || m_nOutCount == 1 )	//重新开始传输,首先开始传输帧头,帧头为0xffff，占2字节
		{
			pdOutput[OUT1] = 3.3;
			m_nFrameHeaderCount++;
			if( m_nFrameHeaderCount > 15 )
				m_nOutCount = 2;

		}
		else if( m_nOutCount < m_nSendBufferSize+2 )	//发送数据
		{
			int nBitData[8] = {0};
			int nData = m_nSendBuffer[m_nOutCount-2];
			for( int i = 0; i < 8; ++i )
			{
				if( (nData&0x80) > 0 )
					nBitData[i] = 1;
				else
					nBitData[i] = 0;
				nData = nData << 1;
			}
			pdOutput[OUT1] = nBitData[m_nBitCount]*3.3;
			m_nBitCount++;
			if( m_nBitCount > 7 )
			{
				m_nBitCount = 0;
				m_nOutCount++;
			}
		}
		else if( m_nOutCount > m_nSendBufferSize+1 )
		{
			//发送完毕后，发送帧尾,帧尾为0xfffe,占2字节
			if( m_nFrameEndCount < 15 )
			{	
				pdOutput[OUT1] = 3.3;
				m_nFrameEndCount++;
			}
			else
			{
				pdOutput[OUT1] = 0;
				m_nFrameEndCount = 0;
				m_nOutCount = 0;
				m_nBitCount = 0;
				m_nFrameHeaderCount = 0;
			}
		}

		m_RevTestBuffer[m_nTestCount] = pdOutput[OUT1];
		m_nTestCount++;
		if( m_nTestCount > 63 )
			m_nTestCount = 0;
	}
	m_nClkState = nClk;

	//数据接收
	double nReClk = pdInput[IN2];
	if( nReClk < 1.0 && m_nReClkState > 1.0 )	//时钟下降沿处理数据
	{
		//接收缓冲
		//首先接收帧头0xffff,当接收完帧头后，数据以8bit为单位组成一个字节放入char数组缓冲中
		if( m_bFrameHeader == TRUE )
		{
			//帧尾为0xfffe
			if( pdInput[IN3] > 1.0 )
				m_nRevBitBuffer[m_nRevBitCount] = 1;
			else
				m_nRevBitBuffer[m_nRevBitCount] = 0;
			pdOutput[OUT2] = m_nRevBitBuffer[m_nRevBitCount]*3.3;
			m_nRevBitCount++;
			if( m_nRevBitCount > 7 )	//接收完一字节
			{
				m_RevByteBuffer[m_nRevByteCount] = m_nRevBitBuffer[0]*128+m_nRevBitBuffer[1]*64+m_nRevBitBuffer[2]*32+m_nRevBitBuffer[3]*16+m_nRevBitBuffer[4]*8+m_nRevBitBuffer[5]*4+m_nRevBitBuffer[6]*2+m_nRevBitBuffer[7];
				m_nRevBitCount = 0;
				if( m_RevByteBuffer[m_nRevByteCount] == 0xfe && m_RevByteBuffer[m_nRevByteCount-1] == 0xff )	//接收到帧尾,一帧数据接收完毕
				{
					//将数据赋值给字符串
					for( int i = 0; i < m_nRevByteCount-1; ++i )
					{
						m_RevBuffer[i] = m_RevByteBuffer[i];
					}
					std::string strRevText = m_RevBuffer;
					std::wstring wstrRevText;
					utility::Ansi2Unicode( strRevText, wstrRevText );
					m_TextShowEdit.SetWindowText( wstrRevText.c_str() );
					m_bRevOK = TRUE;
					m_bFrameHeader = FALSE;
				}
				m_nRevByteCount++;
				if( m_nRevByteCount > 500 )
				{
					m_nRevByteCount = 0;
					m_bFrameHeader = FALSE;
				}
			}
		}
		else
		{
			if( pdInput[IN3] > 1.0 )
				m_nFrameRevCount++;
			else
				m_nFrameRevCount = 0;
			if( m_nFrameRevCount < 0 )
				m_nFrameRevCount = 0;
			if( m_nFrameRevCount == 16 )
			{
				m_bFrameHeader = TRUE;
				m_nRevBitCount = 0;
				m_nRevByteCount = 0;
				m_nFrameRevCount = 0;
				memset( m_RevByteBuffer, 0, sizeof( m_RevByteBuffer ) );
				memset( m_nRevBitBuffer, 0, sizeof( m_nRevBitBuffer ) );
				memset( m_RevBuffer, 0, sizeof( m_RevBuffer ) );
			}
		}
	}
	m_nReClkState = nReClk;
}
