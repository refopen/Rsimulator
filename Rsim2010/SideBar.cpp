// SideBar.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DrawCli.h"
#include "SideBar.h"


// SideBar ��ȭ �����Դϴ�.

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


// SideBar �޽��� ó�����Դϴ�.
