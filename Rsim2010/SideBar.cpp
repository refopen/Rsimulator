// SideBar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DrawCli.h"
#include "SideBar.h"


// SideBar 대화 상자입니다.

IMPLEMENT_DYNAMIC(SideBar, CDialog)

SideBar::SideBar(CWnd* pParent /*=NULL*/)
	: CDialog(SideBar::IDD, pParent)
{

}

SideBar::~SideBar()
{
}

void SideBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SideBar, CDialog)
END_MESSAGE_MAP()


// SideBar 메시지 처리기입니다.
