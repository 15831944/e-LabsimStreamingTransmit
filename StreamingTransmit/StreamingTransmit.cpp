// StreamingTransmit.cpp : 定义 DLL 的初始化例程。
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


// CStreamingTransmitApp 构造

CStreamingTransmitApp::CStreamingTransmitApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CStreamingTransmitApp 对象

CStreamingTransmitApp theApp;


// CStreamingTransmitApp 初始化

BOOL CStreamingTransmitApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

//创建一个算法对象，并返回算法对象指针
void *LtCreateObject()
{
	CMediaShowDlg * pDlg = new CMediaShowDlg();
	pDlg->Create(IDD_DATADLG,NULL);
	pDlg->ShowWindow( SW_SHOW );
	return static_cast<void*>( pDlg );
}

//删除一个算法对象，此函数期待的参数（pObject）正是LtCreateObject()的返回值
void LtDestroyObject( void * pObject )
{
	ASSERT( pObject != NULL );
	ASSERT( !IsBadWritePtr( pObject, sizeof(CMediaShowDlg) ) );
	CMediaShowDlg * pDlg = static_cast<CMediaShowDlg *>( pObject );
	pDlg->DestroyWindow();
	delete pDlg;	//删除一个算法对象
}

//算法主函数
//此函数第一个参数（pObject）是LtCreateObject()的返回值
//此函数第二个参数（pdInput）是指向输入数组的指针
//此函数第三个参数（pdOutput）是指向输出数组的指针
void LtDLLMain(  void * pObject , const bool *pbIsPortUsing, const double *pdInput, double *pdOutput)
{
	ASSERT( pObject != NULL );
	CMediaShowDlg * pDlg = static_cast<CMediaShowDlg *>( pObject );
	pDlg->RunAlgorithm( pdInput, pdOutput );
}

//显示算法操作界面(仅当算法对象是继承自CDialog类,才需编写此函数)
void LtShowWindow( void * pObject )
{
	ASSERT( pObject != NULL );
	ASSERT( !IsBadWritePtr( pObject, sizeof(CMediaShowDlg) ) );
	CMediaShowDlg * pDlg = static_cast<CMediaShowDlg *>( pObject );
	pDlg->ShowWindow( SW_SHOW || SW_RESTORE );
}