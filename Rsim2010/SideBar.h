#pragma once


// SideBar ��ȭ �����Դϴ�.

class SideBar : public CDialog
{
	DECLARE_DYNAMIC(SideBar)

public:
	SideBar(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~SideBar();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ROBOT_STATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
};
