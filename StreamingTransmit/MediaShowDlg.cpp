// MediaShowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "StreamingTransmit.h"
#include "MediaShowDlg.h"
#include "utility.h"

// CMediaShowDlg �Ի���

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


// CMediaShowDlg ��Ϣ�������

void CMediaShowDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	OnCancel();
	ShowWindow( SW_HIDE );
}

void CMediaShowDlg::RunAlgorithm( const double *pdInput, double *pdOutput )
{
	//����ʱ��
	//���ݷ���
	double nClk = pdInput[IN1];
	if( nClk > 1.0 && m_nClkState < 1.0 )	//ʱ���������������
	{
		if( m_nOutCount == 0 || m_nOutCount == 1 )	//���¿�ʼ����,���ȿ�ʼ����֡ͷ,֡ͷΪ0xffff��ռ2�ֽ�
		{
			pdOutput[OUT1] = 3.3;
			m_nFrameHeaderCount++;
			if( m_nFrameHeaderCount > 15 )
				m_nOutCount = 2;

		}
		else if( m_nOutCount < m_nSendBufferSize+2 )	//��������
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
			//������Ϻ󣬷���֡β,֡βΪ0xfffe,ռ2�ֽ�
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

	//���ݽ���
	double nReClk = pdInput[IN2];
	if( nReClk < 1.0 && m_nReClkState > 1.0 )	//ʱ���½��ش�������
	{
		//���ջ���
		//���Ƚ���֡ͷ0xffff,��������֡ͷ��������8bitΪ��λ���һ���ֽڷ���char���黺����
		if( m_bFrameHeader == TRUE )
		{
			//֡βΪ0xfffe
			if( pdInput[IN3] > 1.0 )
				m_nRevBitBuffer[m_nRevBitCount] = 1;
			else
				m_nRevBitBuffer[m_nRevBitCount] = 0;
			pdOutput[OUT2] = m_nRevBitBuffer[m_nRevBitCount]*3.3;
			m_nRevBitCount++;
			if( m_nRevBitCount > 7 )	//������һ�ֽ�
			{
				m_RevByteBuffer[m_nRevByteCount] = m_nRevBitBuffer[0]*128+m_nRevBitBuffer[1]*64+m_nRevBitBuffer[2]*32+m_nRevBitBuffer[3]*16+m_nRevBitBuffer[4]*8+m_nRevBitBuffer[5]*4+m_nRevBitBuffer[6]*2+m_nRevBitBuffer[7];
				m_nRevBitCount = 0;
				if( m_RevByteBuffer[m_nRevByteCount] == 0xfe && m_RevByteBuffer[m_nRevByteCount-1] == 0xff )	//���յ�֡β,һ֡���ݽ������
				{
					//�����ݸ�ֵ���ַ���
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
