// StreamingTransmit.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "StreamingTransmit.h"
#include "MediaShowDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CStreamingTransmitApp

BEGIN_MESSAGE_MAP(CStreamingTransmitApp, CWinApp)
END_MESSAGE_MAP()


// CStreamingTransmitApp ����

CStreamingTransmitApp::CStreamingTransmitApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CStreamingTransmitApp ����

CStreamingTransmitApp theApp;


// CStreamingTransmitApp ��ʼ��

BOOL CStreamingTransmitApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

//����һ���㷨���󣬲������㷨����ָ��
void *LtCreateObject()
{
	CMediaShowDlg * pDlg = new CMediaShowDlg();
	pDlg->Create(IDD_DATADLG,NULL);
	pDlg->ShowWindow( SW_SHOW );
	return static_cast<void*>( pDlg );
}

//ɾ��һ���㷨���󣬴˺����ڴ��Ĳ�����pObject������LtCreateObject()�ķ���ֵ
void LtDestroyObject( void * pObject )
{
	ASSERT( pObject != NULL );
	ASSERT( !IsBadWritePtr( pObject, sizeof(CMediaShowDlg) ) );
	CMediaShowDlg * pDlg = static_cast<CMediaShowDlg *>( pObject );
	pDlg->DestroyWindow();
	delete pDlg;	//ɾ��һ���㷨����
}

//�㷨������
//�˺�����һ��������pObject����LtCreateObject()�ķ���ֵ
//�˺����ڶ���������pdInput����ָ�����������ָ��
//�˺���������������pdOutput����ָ����������ָ��
void LtDLLMain(  void * pObject , const bool *pbIsPortUsing, const double *pdInput, double *pdOutput)
{
	ASSERT( pObject != NULL );
	CMediaShowDlg * pDlg = static_cast<CMediaShowDlg *>( pObject );
	pDlg->RunAlgorithm( pdInput, pdOutput );
}

//��ʾ�㷨��������(�����㷨�����Ǽ̳���CDialog��,�����д�˺���)
void LtShowWindow( void * pObject )
{
	ASSERT( pObject != NULL );
	ASSERT( !IsBadWritePtr( pObject, sizeof(CMediaShowDlg) ) );
	CMediaShowDlg * pDlg = static_cast<CMediaShowDlg *>( pObject );
	pDlg->ShowWindow( SW_SHOW || SW_RESTORE );
}