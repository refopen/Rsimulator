#include "stdafx.h"
#include <afxpriv.h>

#include "drawcli.h"

#include "drawdoc.h"
#include "drawobj.h"
#include "drawvw.h"

#include "drawobj.h"
#include "drawtool.h"
#include "mainfrm.h"
#include "robot.h"

#include <math.h>
#include "util.h"

#define MODE 4

CRobotControl::CRobotControl()
{
	int i;
	angle = 0;

	for(i=0; i<10; i++)	sonarDirection[i] = i*36*PI/180.0;
	for(i=0; i<10; i++)	sonarOffset[i] = ROBOTRADIUS;
	for(i=0; i<10; i++)	sonarAngle[i] = i*36*PI/180.0;
	for(i=0; i<10; i++)
	{
		sonarPos[i].x = ROBOTRADIUS * cos(sonarAngle[i]);
		sonarPos[i].y = ROBOTRADIUS * sin(sonarAngle[i]);
	}

	for(i=0; i<360; i++)	laserDirection[i] = i*PI/180.0;
	for(i=0; i<360; i++)
	{
		laserPos[i].x = ROBOTRADIUS * cos(i*PI/180.0);
		laserPos[i].y = ROBOTRADIUS * sin(i*PI/180.0);
	}
	Uflag = 0;
}

CRobotControl::~CRobotControl()
{
}

/* Mathematically precise moving function */
double CRobotControl::MoveTo(CDrawView *pView ,double x, double y)
{
	int desired_angle, theta_e;
	double dx, dy, distance_e, Kd=1.0, Ka=0.1;			//theta_e, theta_d변수 선언 

	CDrawRect *robot = pView->GetDocument()->m_robot;

	dx =  x - robot->CenterPtF().X;						//x-coordinate error
	dy =  y - robot->CenterPtF().Y;						//y-coordinate error 
	
	distance_e = sqrt(dx*dx + dy*dy);					//원하는 위치와 로봇과의 거리차 

	//적절한 제어기 계수를 설정한다 
	if( distance_e > 100)		Kd = 1.0, Ka = 0.32;	//거리차가 100 이상일때 
	else if( distance_e > 50)	Kd = 1.2, Ka = 0.35;	//거리차가 100 이상일때 
	else if( distance_e > 30)	Kd = 1.4, Ka = 0.43;	//거리차가 30 이상일때 
	else if( distance_e > 20)	Kd = 2.0 ,Ka = 0.47;	//거리차가 20 이상일때 
	else if( distance_e > 10)	Kd = 4.0 ,Ka = 0.55;	//거리차가 20 이상일때  // 30.0
	else  Kd = 5.0 ,Ka = 0.6;							//거리차가 10 이상일때

	if(dx == 0 && dy == 0) desired_angle = 90;			//dx가 0으로 나누어지는 것을 방지 
	else	desired_angle = (int)(180/PI*atan2((double)dy,(double)dx));	//x,y 거리 오차에서 원하는 각도 구함

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	if(pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_PLANNING_TYPE1));
	//if(MODE == 1);
	else if(pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_PLANNING_TYPE2))
	//else if(MODE == 2)
		desired_angle = pView->OC.GetVFF(robot->CenterPtF().X, robot->CenterPtF().Y, x, y, this);
	else if(pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_PLANNING_TYPE3))
	//else if(MODE == 3)
	{
		desired_angle = pView->OC.GetVFF(robot->CenterPtF().X, robot->CenterPtF().Y, x, y, this);
		pView->OC.SmoothPolarHistogramDensity(this);
		pView->OC.ShowHistogram();
		pView->OC.GetVFH(robot->CenterPtF().X, robot->CenterPtF().Y, x, y);
	}
	else if(pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_PLANNING_TYPE4))
	//else if(MODE == 4)
	{
		m_pUKF->ComputeSteering(x, y);
//		TRACE("xview : %.3f %.3f\n", robot->CenterPtF().X, robot->CenterPtF().Y);
		m_pUKF->VehicleModel();
		angle = RAD2DEG(m_pUKF->xtrue->data.db[2]);
	}
	
	theta_e = desired_angle - angle;					//Theta_e = Theta_d - Theta_r
	while(theta_e > 180) theta_e -= 360;				//Theat_e 를 -180 에서 180으로 제한 
	while(theta_e < -180) theta_e += 360; 

	/*	로봇은 전방향으로만 이동하는 것으로 가정한다.
	if(theta_e > 95 || theta_e < -95)
	{
		theta_e += 180;
		if(theta_e > 180) theta_e -= 360;
		if(theta_e > 80) theta_e = 80;
		if(theta_e < -80) theta_e = -80;
		distance_e = -distance_e;
		vr = (int)(Kd*distance_e + Ka*(double)theta_e); //오른쪽 바퀴의 제어입력함수 
		vl = (int)(Kd*distance_e - Ka*(double)theta_e); //왼쪽 바퀴의 제어입력함수 
	}

	else if(theta_e < 85 && theta_e > -85)				//로봇의 방향의 앞뒤로 변하는 것을 고려
	*/
	
	if(theta_e < 85 && theta_e > -85)					//로봇의 방향의 앞뒤로 변하는 것을 고려 
	{
  		vr = (int)(Kd*distance_e + Ka*(double)theta_e); //오른쪽 바퀴의 제어입력함수 
  		vl = (int)(Kd*distance_e - Ka*(double)theta_e); //왼쪽 바퀴의 제어입력함수 
	}
	
	/* 제자리 회전 */
	else
	{
		vr = (int)(+0.17*theta_e);
		vl = (int)(-0.17*theta_e);
	}

	return distance_e;
}

void CRobotControl::Update(CDrawView *pView)
{
	if(!pView->GetDocument()->m_bRobotActivated)
		return;

	if(!Uflag)
	{
		Uflag = 1;
		goal.x = 0; goal.y = 0;
	}

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CDrawDoc* pDoc = pView->GetDocument();
	CDrawRect *robot = pDoc->m_robot;

	if(pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_PLANNING_TYPE1))
	//if(MODE == 1)
	{
		CPoint IntermediateGoal(goal);

		CRect rect;
		rect.left = -pDoc->GetSize().cx / 2;
		rect.top = -pDoc->GetSize().cy / 2;
		rect.right = rect.left + pDoc->GetSize().cx;
		rect.bottom = rect.top + pDoc->GetSize().cy;
		
		rect.left = -500;
		rect.right = 500;
		rect.top = -500;
		rect.bottom = 500;

		CPoint ptGridsize;
		ptGridsize.x = pFrame->m_wndDialogBar.GetDlgItemInt(IDC_COMBO_HORIZON);
		ptGridsize.y = pFrame->m_wndDialogBar.GetDlgItemInt(IDC_COMBO_VERTICAL);

		ptGridsize.x = (rect.right - rect.left) / ptGridsize.x;
		ptGridsize.y = (rect.bottom - rect.top) / ptGridsize.y;

		BOOL done = FALSE;
		int min = 999;
		/* recalculate intermediate goal based on the generated map */
		for (int x = rect.left / ptGridsize.x * ptGridsize.x, i = 0; x < rect.right; i++, x += ptGridsize.x){
			for (int y = rect.top / ptGridsize.y * ptGridsize.y, j = 0; y < rect.bottom; j++, y += ptGridsize.y){
				CRect rectUnit(x, y, x+ptGridsize.x, y-ptGridsize.y);
				rectUnit.NormalizeRect();
				if( rectUnit.PtInRect(CPoint((int)robot->CenterPtF().X, (int)robot->CenterPtF().Y)) ){

					/* 주변 8방의 cell중 거리값이 최소인 곳의 중앙점을 경유 goal로 지정 */
					for(int n = -1; n<2; n++){
						for(int m = -1; m<2; m++){
							if(m_nTable[i+n][j+m] < min && !(n==0 && m==0) && !((i+n)<0) && !((j+m)<0) ){
								min = m_nTable[i+n][j+m];
								IntermediateGoal.x = x + (ptGridsize.x * n) + (int)ptGridsize.x/2;
								IntermediateGoal.y = y + (ptGridsize.y * m) - (int)ptGridsize.y/2;
							}
						}
					}
					done = TRUE;
					break;
				}
			}
			if(done)
				break;
		}
		MoveTo(pView, IntermediateGoal.x, IntermediateGoal.y);
	}
	else if(pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_PLANNING_TYPE2))
	//else if(MODE == 2)
	{
		MoveTo(pView, goal.x, goal.y);
	}
	else if(pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_PLANNING_TYPE3))
	//else if(MODE == 3)
	{
		MoveTo(pView, goal.x, goal.y);
	}
	else if(pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_PLANNING_TYPE4))
	//else if(MODE == 4)
	{
		static int cnt = 0;
		double dist;
		double dx =  goal.x - m_pUKF->xtrue->data.db[0];						//x-coordinate error
		double dy =  goal.y - m_pUKF->xtrue->data.db[1];						//y-coordinate error 
		dist = sqrt(dx*dx + dy*dy);									//distance error
		// 목표를 재설정한다.
		if(dist < AT_WAYPOINT)
		{
			TRACE("goalVector size : %d\n", pView->GetDocument()->goalVector.size());
			TRACE("cnt : %d\n", cnt);
			if(++cnt >= pView->GetDocument()->goalVector.size())	cnt = 0;
			goal = pView->GetDocument()->goalVector[cnt];
			TRACE("new goal asserted : %d %d\n", goal.x, goal.y);
		}

		MoveTo(pView, goal.x, goal.y);
	}

	int w;			// angular speed
	int cx, cy;

	rv = (int)(vr + vl)/2;
    w = (int)(vr - vl)/WHEELBASE;

	angle += w;

	/* normalization */
	if(angle > 359)
		angle %= 360;
	if(angle < 0){
		angle %= 360;
		angle += 360;
	}

 	cx = (int)((int)(rv*cos((double)(PI*angle)/180)*0.1));
 	cy = (int)((int)(rv*sin((double)(PI*angle)/180)*0.1));

	CPoint delta(cx, cy);
	CRect position(robot->m_position);
	position += delta;
	robot->m_position = position;

	CRect rectClient;
	pView->GetClientRect(&rectClient);

	BOOL bIntersect = FALSE;

	/* Do not permit to intersect each object */
//	POSITION pos = pView->GetDocument()->m_objects.GetHeadPosition();
//	while (pos != NULL && !bIntersect)
//	{
//		CDrawObj* pObj = pView->GetDocument()->m_objects.GetNext(pos);
//		CDrawRect* pRect = (CDrawRect *)pObj;
//		if( position == pObj->m_position )
//			continue;
//		// goal과 로봇이 겹치지 못하게 한다.
// 		if(	robot->Intersects(pObj->m_position) && pRect->m_nShape == CDrawRect::goal)
// 			bIntersect = TRUE;
//	}
//
//	CRect Rect(position);
//	pView->DocToClient(Rect);
//	if( (Rect & rectClient) != Rect )
//		bIntersect = TRUE;
//
//	robot->m_position -= delta;
//
	if(pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_PLANNING_TYPE4))
	//if(MODE == 4)
	{
		int w = position.Width();
		int h = position.Height();
		cx = m_pUKF->xtrue->data.db[0];
		cy = m_pUKF->xtrue->data.db[1];
		position.top = cy-h/(double)2;
		position.bottom = cy+h/(double)2;
		position.left = cx-w/(double)2;
		position.right = cx+w/(double)2;
		robot->m_position = position;
	}

	if(!bIntersect)
		robot->MoveTo(position, pView);

	/* 로봇의 상태창에 현재 정보를 표시한다. */
	pFrame->m_wndDialogBar.SetDlgItemInt(IDC_X, (UINT)robot->CenterPtF().X);
	pFrame->m_wndDialogBar.SetDlgItemInt(IDC_Y, (UINT)robot->CenterPtF().Y);
	pFrame->m_wndDialogBar.SetDlgItemInt(IDC_VEL, rv);
	pFrame->m_wndDialogBar.SetDlgItemInt(IDC_ANGLE, angle);
	pFrame->m_wndDialogBar.SetDlgItemInt(IDC_ANGLE_VEL, w);
	pFrame->m_wndDialogBar.SetDlgItemInt(IDC_VL, vl);
	pFrame->m_wndDialogBar.SetDlgItemInt(IDC_VR, vr);
	pFrame->m_wndDialogBar.SetDlgItemInt(IDC_GOAL_X, goal.x);
	pFrame->m_wndDialogBar.SetDlgItemInt(IDC_GOAL_Y, goal.y);

	// UKF parts
	m_pUKF->Doit();
}

/* 나중에 지워도 됨 */
/* 호출된 위치를 기준으로 장애물의 크기를 판별한다. */
BOOL CRobotControl::SizeOfObstacle(int n, int m, int i[4])
{
	while(m_nTable[++n][m] == 999)
		i[0]++;
	n--;
	while(m_nTable[n][++m] == 999)
		i[1]++;
	m--;
	i[2] = i[0];
	i[3] = i[1];
	while(1){
		if(--n<0){
			n = 0;
			break;
		}
		if(m_nTable[n][m] == 999)
			i[2]--;
		else
			break;
	}
	if(++n==1) n = 0;
	while(1){
		if(--m<0){
			m = 0;
			break;
		}
		if(m_nTable[n][m] == 999)
			i[3]--;
		else
			break;
	}
	return TRUE;
}

/*	
	Distance-Transform-Based Path-Planning Method 
	orthogonal direction -> 3
	diagonal direction -> 4
	Recursive function
*/
BOOL CRobotControl::Recursion(int i, int j, BOOL direction)
{
	int n, m;
	// 맵 외곽 이면 return
	if(i<0 || i>=WIDTH || j<0 || j>=HEIGHT) return FALSE;
	//장애물이 있을경우나 이전에 방문한 셀이면 return
	if(m_nTable[i][j] == 999 || m_bChecked[i][j] == TRUE)
		return FALSE;
	for(n = -1; n < 2; n++){
		for(m = -1; m < 2; m++){
			/* center */
			if(n==0 && m==0) continue;
			/* out of the screen */
			if(i+n<0 || i+n>=WIDTH || j+m<0 || j+m>=HEIGHT) continue;
			/* obstacle */
			if(m_nTable[i+n][j+m] == 999) continue;
			/* diagonal direction */
			if(abs(n)==abs(m)){
				if(m_nTable[i][j] + 4 < m_nTable[i+n][j+m])
					m_nTable[i+n][j+m] = m_nTable[i][j] + 4;
			}
			/* othogonal direction */
			else if(m_nTable[i][j] + 3 < m_nTable[i+n][j+m])
				m_nTable[i+n][j+m] = m_nTable[i][j] + 3;
		}
	}

	m_bChecked[i][j] = TRUE;

	/*
		현재위치를 기준으로 8방향의 cell에 대해서 재귀적으로 호출
		__________________________
		| -1, 1  | 0, 1  | 1, 1  |
		| -1, 0  | 0, 0  | 1, 0  |
		| -1, -1 | 0, -1 | 1, -1 |
		--------------------------
	*/
	if(direction){
		for(n = -1; n < 2; n++){
			for(m = -1; m < 2; m++){
				Recursion(i+n, j+m, direction);
			}
		}
	}
	
	else{
		for(n = 1; n > -2; n--){
			for(m = 1; m > -2; m--){
				Recursion(i+n, j+m, direction);
			}
		}
	}

	return TRUE;
}

/*	
	Distance-Transform-Based Path-Planning Method 
	orthogonal direction -> 3
	diagonal direction -> 4
	iterative function
*/
BOOL CRobotControl::Iterative(int i, int j, int Width, int Height)
{
	if(i<0 || i>=Width || j<0 || j>=Height) return FALSE;
	if(m_nTable[i][j] == 999 || m_nTable[i][j] == 998)
		return FALSE;

	for(int n = -1; n < 2; n++){
		for(int m = -1; m < 2; m++){
			if(n==0 && m==0) continue;
			if(i+n<0 || i+n>=Width || j+m<0 || j+m>=Height) continue;
			if(m_nTable[i+n][j+m] == 999) continue;
			if(abs(n)==abs(m)){
				if(m_nTable[i][j] + 4 < m_nTable[i+n][j+m])
					m_nTable[i+n][j+m] = m_nTable[i][j] + 4;
			}
			
			else if(m_nTable[i][j] + 3 < m_nTable[i+n][j+m])
				m_nTable[i+n][j+m] = m_nTable[i][j] + 3;
		}
	}

	return TRUE;
}

void CRobotControl::DistanceTransform(int Width, int Height, int sx, int sy, int gx, int gy)
{
	int i, j;

	if(m_nTable[gx][gy] == 999)
		return;
	m_nTable[gx][gy] = 0;
	for(i=0; i<Width; i++){
		for(j=0; j<Height; j++){
			m_bChecked[i][j] = FALSE;
		}
	}
	Recursion(gx, gy, TRUE);
	for(i=0; i<Width; i++){
		for(j=0; j<Height; j++){
			m_bChecked[i][j] = FALSE;
		}
	}
	Recursion(gx, gy, FALSE);

	for(i=1; i<Width; i++){
		for(int n=-i; n<i+1; n++){
			for(int m=-i; m<i+1; m++){
				if(n==-i || n==i)
					Iterative(gx+n, gy+m, Width, Height);
				else if(m==-i || m==i)
					Iterative(gx+n, gy+m, Width, Height);
			}
		}
	}

	for(i=1; i<Width; i++){
		for(int n=i; n>=-i; n--){
			for(int m=i; m>-i; m--){
				if(n==-i || n==i)
					Iterative(gx+n, gy+m, Width, Height);
				else if(m==-i || m==i)
					Iterative(gx+n, gy+m, Width, Height);
			}
		}
	}

	// generate shortest path
	int m = 0;
	int x = sx, y = sy;
	int minidx;
	int n;
	int idx[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
	int idy[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
	int min = 998;
	Path.clear();
	while(m < WIDTH*HEIGHT)
	{
		Path.push_back(CPoint(x, y));
		TRACE("%d, %d\n", x, y);
 		if(x == gx && y == gy)
 			break;
 		minidx = 8;
		for(n=0; n<8; n++)
		{
			if(y+idy[n] < 0 || x+idx[n] < 0 ||
				y+idy[n] >= HEIGHT || x+idx[n] >= WIDTH) continue;

			if(min > m_nTable[x+idx[n]][y+idy[n]])
			{
				min = m_nTable[x+idx[n]][y+idy[n]];
				minidx = n;
			}
		}
		if(minidx == 8)
		{
			int temp = 0;
			for(n=0; n<8; n++)
			{
				if(m_nTable[x+idx[n]][y+idy[n]] == 998) temp++;
				TRACE("[%d][%d]:%d\n", x+idx[n], y+idy[n], m_nTable[x+idx[n]][y+idy[n]]);
			}
			if(temp != 8)
			{
				TRACE("주변셀이 쉴드나 장애물로 둘러싸임.\n");
				break;
			}
			else
			{
				TRACE("주변셀이 모두 초기화 상태임.\n");
				break;
			}
		}
		min = 998;
		x += idx[minidx];
		y += idy[minidx];
	}
}

void CRobotControl::SonarMeasurement(CDrawView *pView)
{
	DPOINT sonXY[10];
	DPOINT sonLXY[10];
	DPOINT sonCXY[10];
	DPOINT sonRXY[10];
	double sonA[10];
	double turretA;
	int i, si;

	turretA = angle * (PI / 180.0);

	CDrawRect *robot = pView->GetDocument()->m_robot;
	CDrawDoc* pDoc = pView->GetDocument();
	ASSERT_VALID(pDoc);

	// 초음파 센서 위치 좌표 변환과 현재 향하고 있는 각 계산, 초음파 거리 초기화
	for(i=0; i<10; i++)
	{
		sonXY[i].x = robot->CenterPtF().X + (cos(turretA)*sonarPos[i].x - sin(turretA)*sonarPos[i].y);
		sonXY[i].y = robot->CenterPtF().Y + (sin(turretA)*sonarPos[i].x + cos(turretA)*sonarPos[i].y);
	
		sonA[i] = sonarDirection[i] + turretA;

		sonCXY[i].x = sonXY[i].x + 10000.0*cos(sonA[i]);
		sonCXY[i].y = sonXY[i].y + 10000.0*sin(sonA[i]);

		sonLXY[i].x = sonXY[i].x + 10000.0*cos(sonA[i]+SONARBEAMANGLE);
		sonLXY[i].y = sonXY[i].y + 10000.0*sin(sonA[i]+SONARBEAMANGLE);

		sonRXY[i].x = sonXY[i].x + 10000.0*cos(sonA[i]-SONARBEAMANGLE);
		sonRXY[i].y = sonXY[i].y + 10000.0*sin(sonA[i]-SONARBEAMANGLE);

		s[i] = SONARMAXLIMIT;
	}

	for(si=0; si<10; si++)
	{
		POSITION pos = pDoc->m_objects.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawRect* pObj = (CDrawRect*)(pDoc->m_objects.GetNext(pos));
			CRect rect = pObj->m_position;
			CPoint p;
			DPOINT p1, p2;
			DPOINT tp1, tp2;
			double dist;
			p = rect.TopLeft();
			p1.x = p.x; p1.y = p.y;
			p = rect.BottomRight();
			p2.x = p.x; p2.y = p.y;
			switch (pObj->m_nShape)
			{
			case pObj->rectangle:
			case pObj->roundRectangle:
			case pObj->ellipse:
				tp1 = p1; tp2.x = p2.x; tp2.y = p1.y;
				dist = SonarMeasurement_Linesegment(sonXY[si], sonCXY[si], sonLXY[si], sonRXY[si], tp1, tp2);
				if(dist >= 0.0 && dist < s[si]) s[si] = dist;
				tp1 = tp2; tp2 = p2;
				dist = SonarMeasurement_Linesegment(sonXY[si], sonCXY[si], sonLXY[si], sonRXY[si], tp1, tp2);
				if(dist >= 0.0 && dist < s[si]) s[si] = dist;
				tp1 = tp2; tp2.x = p1.x; tp2.y = p2.y;
				dist = SonarMeasurement_Linesegment(sonXY[si], sonCXY[si], sonLXY[si], sonRXY[si], tp1, tp2);
				if(dist >= 0.0 && dist < s[si]) s[si] = dist;
				tp1 = tp2; tp2 = p1;
				dist = SonarMeasurement_Linesegment(sonXY[si], sonCXY[si], sonLXY[si], sonRXY[si], tp1, tp2);
				if(dist >= 0.0 && dist < s[si]) s[si] = dist;
				break;

			case pObj->line:
				dist = SonarMeasurement_Linesegment(sonXY[si], sonCXY[si], sonLXY[si], sonRXY[si], p1, p2);
				if(dist >= 0.0 && dist < s[si]) s[si] = dist;
				break;
				
			case pObj->robot:
			case pObj->goal:
			default:;
			}
		}

// 		// line 형태 장애물 검사
// 		for(i=0; i<m_Obst->lineN; i++)
// 		{
// 			sdist[si] = SonarMeasurement_Line(sonXY[si], sonCXY[si], sonLXY[si], sonRXY[si], m_Obst->line[i], sdist[si]);
// 		}
// 
// 		// box 형태 장애물 검사 
// 		for(i=0; i<m_Obst->boxN; i++)
// 		{			
// 			sdist[si] = SonarMeasurement_Box(sonXY[si], sonCXY[si], sonLXY[si], sonRXY[si], m_Obst->box[i], sdist[si]);
// 		}
// 
// 		// cylinder 형태 장애물 검사
// 		for(i=0; i<m_Obst->cylinderN; i++)
// 		{
// 			sdist[si] = SonarMeasurement_Cylinder(sonXY[si], sonCXY[si], sonLXY[si], sonRXY[si], m_Obst->cylinder[i], sdist[si]);
// 		}
	}
}

double CRobotControl::SonarMeasurement_Linesegment(DPOINT sxy, DPOINT scxy, DPOINT slxy, DPOINT srxy, DPOINT line0, DPOINT line1)
{
	double vertxc, vertyc;
	double limit2 = SONARMAXLIMIT*SONARMAXLIMIT;
	double vertD;					// 초음파 중심에서 선분까지의 수직 거리(제곱 값)
	double lineD0, lineD1;			// 초음파 중심에서 선분 양 끝점까지 거리(제곱 값)
	double u1;
	double xc0, yc0, xc1, yc1, xc2, yc2;
	double ua0, ua1, ua2, ub0, ub1, ub2;
	int ret0, ret1, ret2;
	double beam_tan = tan(SONARBEAMANGLE);
	double beam_tan2 = beam_tan*beam_tan;
	double tempD;

	// 선분(직선)까지 가장 가까운 거리가 초음파 한계 거리보다 멀면 검사 안함 
	u1 = CrossPoint2(line0.x, line0.y, line1.x, line1.y, sxy.x, sxy.y, &vertxc, &vertyc);
	vertD = (sxy.x-vertxc)*(sxy.x-vertxc) + (sxy.y-vertyc)*(sxy.y-vertyc);
	if(vertD > (limit2)) return -1.0;
	
	// 초음파 한계 거리보다 가깝지만 선분의 양끝점까지 거리가 한계 거리보다 멀면 검사 안함
	lineD0 = (sxy.x-line0.x)*(sxy.x-line0.x) + (sxy.y-line0.y)*(sxy.y-line0.y);
	lineD1 = (sxy.x-line1.x)*(sxy.x-line1.x) + (sxy.y-line1.y)*(sxy.y-line1.y);
	if((lineD0 > limit2) && (lineD1 > limit2)) return -1.0;

	ret0 = IsCrossLine(sxy.x, sxy.y, slxy.x, slxy.y, line0.x, line0.y, line1.x, line1.y, &xc0, &yc0, &ua0, &ub0);
	ret1 = IsCrossLine(sxy.x, sxy.y, scxy.x, scxy.y, line0.x, line0.y, line1.x, line1.y, &xc1, &yc1, &ua1, &ub1);
	ret2 = IsCrossLine(sxy.x, sxy.y, srxy.x, srxy.y, line0.x, line0.y, line1.x, line1.y, &xc2, &yc2, &ua2, &ub2);

	if(ret1 == -2 || ret1 == -1) return -1.0;			// 초음파 빔과 선분이 평행하면 검사 안함
	if((ret1 == 2) && (ua1 < 0.0)) return -1.0;			// 초음파 빔 뒤쪽으로 선분이 있으면 검사 안함 

	if(u1>=0.0 && u1<=1.0)
	{
		tempD = (vertxc-xc1)*(vertxc-xc1) + (vertyc-yc1)*(vertyc-yc1);
		if((tempD/vertD) > beam_tan2) return -1.0;
		
		return sqrt(vertD);
	}
	else
	{
		return -1.0;
	}
}

//=================================================================================================
// Rtn : 직선범위안에 점이 있으면 0과1사이, 밖에 있으면 0이하 또는 1이상
// Desc: 한점에서 직선까지의 최단거리와 그 교차점 계산
//       (x1, y1)과 (x2, y2)가 이루는 직선과 점(xp, yp) 사이의 최단거리를 이루는
//       교차점은 점(xc, yc) 이다.
//       (xc, yc)가 (x1, y1)이면 u는 0
//       (xc, yc)가 (x2, y2)이면 u는 1
//=================================================================================================
double CRobotControl::CrossPoint2(double x1, double y1, double x2, double y2, 
                  double xp, double yp, double *xc, double *yc)
{
	double u;

	u = ((xp-x1)*(x2-x1)+(yp-y1)*(y2-y1)) / ((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
	*xc = x1 + u*(x2-x1);
	*yc = y1 + u*(y2-y1);	

	return u;
}

//=================================================================================================
// 두 선분이 교차하는가?
// 1 : 교차        0 : 선분의 끝점교차     2 : 선분 외부에서 교차   -1 : 두 선분이 같다     -2 : 두 선분 평행
//=================================================================================================
int CRobotControl::IsCrossLine(double x1, double y1, double x2, double y2, 
                double x3, double y3, double x4, double y4, double *xc, double *yc, double *UA, double *UB)
{
	double den, ua_num, ub_num;
	double ua, ub;	
		
// 	TRACE("(%.1f, %.1f)  (%.1f, %.1f)    (%.1f, %.1f)  (%.1f, %.1f)\n", x1, y1, x2, y2, x3, y3, x4, y4);
	
	den = (y4-y3)*(x2-x1) - (x4-x3)*(y2-y1);
	ua_num = (x4-x3)*(y1-y3) - (y4-y3)*(x1-x3);
	ub_num = (x2-x1)*(y1-y3) - (y2-y1)*(x1-x3);
		
// 	TRACE("den:%f   ua_num:%f   ub_num:%f\n", den ,ua_num, ub_num);
	
	// 두 선분이 같다
	if(fabs(den) < 0.0001 && fabs(ua_num) < 0.0001 && fabs(ub_num) < 0.0001) 
	{
		*UA = 0.0;
		*UB = 0.0;
		return -1;
	}
	
	// 두 선분이 평행
	if(fabs(den) < 0.0001) 
	{
		*UA = 0.0;
		*UB = 0.0;
		return -2;
	}
		
	ua = ua_num / den;
	ub = ub_num / den;
		
// 	TRACE("ua: %f  ub: %f\n\n", ua, ub);
	
	// 한 선분의 끝 점에 다른 선분이 교차한다. 선분 내에서 교차하지 않는다.
	if((fabs(ua-1.0)<0.0001) || (fabs(ua)<0.0001) || (fabs(ub)<0.0001) || (fabs(ub)<0.0001)) 
	{
		*xc = x1 + ua*(x2 - x1);
		*yc = y1 + ua*(y2 - y1);		
		*UA = ua;
		*UB = ub;
		return 0;
	}	
	else if(ua>0.0 && ua<1.0 && ub>0.0 && ub<1.0) 
	{
		*xc = x1 + ua*(x2 - x1);
		*yc = y1 + ua*(y2 - y1);		
		*UA = ua;
		*UB = ub;
		return 1;
	}
	else
	{
		*xc = x1 + ua*(x2 - x1);
		*yc = y1 + ua*(y2 - y1);		
		*UA = ua;
		*UB = ub;
		return 2;		
	}
}

double CRobotControl::LaserMeasurement_Linesegment(DPOINT lxy, DPOINT lcxy, DPOINT line0, DPOINT line1)
{
	double vertxc, vertyc;
	double limit2 = SONARMAXLIMIT*SONARMAXLIMIT;
	double vertD;					// 레이져 중심에서 선분까지의 수직 거리(제곱 값)
	double lineD0, lineD1;			// 레이져 중심에서 선분 양 끝점까지 거리(제곱 값)
	double u1;
	//double xc0, yc0, xc1, yc1, xc2, yc2;
	//double ua0, ua1, ua2, ub0, ub1, ub2;
	double xc1, yc1;
	double ua1, ub1;
	int ret1;
	double beam_tan = tan(SONARBEAMANGLE);
	double beam_tan2 = beam_tan*beam_tan;
	double tempD;

 	u1 = CrossPoint2(line0.x, line0.y, line1.x, line1.y, lxy.x, lxy.y, &vertxc, &vertyc);
 	vertD = (lxy.x-vertxc)*(lxy.x-vertxc) + (lxy.y-vertyc)*(lxy.y-vertyc);
	// 선분(직선)까지 가장 가까운 거리가 레이져 한계 거리보다 멀면 검사 안함 
 	if(vertD > (limit2)) return -1.0;
	
	// 레이져 한계 거리보다 가깝지만 선분의 양끝점까지 거리가 한계 거리보다 멀면 검사 안함
 	lineD0 = (lxy.x-line0.x)*(lxy.x-line0.x) + (lxy.y-line0.y)*(lxy.y-line0.y);
 	lineD1 = (lxy.x-line1.x)*(lxy.x-line1.x) + (lxy.y-line1.y)*(lxy.y-line1.y);
	if((lineD0 > limit2) && (lineD1 > limit2)) return -1.0;

	//ret0 = IsCrossLine(lxy.x, lxy.y, slxy.x, slxy.y, line0.x, line0.y, line1.x, line1.y, &xc0, &yc0, &ua0, &ub0);
	ret1 = IsCrossLine(lxy.x, lxy.y, lcxy.x, lcxy.y, line0.x, line0.y, line1.x, line1.y, &xc1, &yc1, &ua1, &ub1);
	//ret2 = IsCrossLine(lxy.x, lxy.y, srxy.x, srxy.y, line0.x, line0.y, line1.x, line1.y, &xc2, &yc2, &ua2, &ub2);

 	if(ret1 == -2 || ret1 == -1) return -1.0;			// 레이져 빔과 선분이 평행하면 검사 안함
 	if((ret1 == 2) && (ua1 < 0.0)) return -1.0;			// 레이져 빔 뒤쪽으로 선분이 있으면 검사 안함 

	if(u1>=0.0 && u1<=1.0)
	{
		tempD = (vertxc-xc1)*(vertxc-xc1) + (vertyc-yc1)*(vertyc-yc1);
		if((tempD/vertD) > beam_tan2) return -1.0;
//		return sqrt(vertD);

		tempD = (lxy.x-xc1)*(lxy.x-xc1) + (lxy.y-yc1)*(lxy.y-yc1);
		return sqrt(tempD);
	}
	else
	{
		return -1.0;
	}
}

void CRobotControl::LaserMeasurement(CDrawView *pView)
{
	DPOINT laserXY[360];
	DPOINT laserCXY[360];
	double laserA[360];
	double turretA;
	int i, li;

	turretA = angle * (PI / 180.0);

	CDrawRect *robot = pView->GetDocument()->m_robot;
	CDrawDoc* pDoc = pView->GetDocument();
	ASSERT_VALID(pDoc);

	// 레이져 센서 위치 좌표 변환과 현재 향하고 있는 각 계산, 레이져 거리 초기화
	for(i=0; i<360; i++)
	{
		laserXY[i].x = robot->CenterPtF().X + (cos(turretA)*laserPos[i].x - sin(turretA)*laserPos[i].y);
		laserXY[i].y = robot->CenterPtF().Y + (sin(turretA)*laserPos[i].x + cos(turretA)*laserPos[i].y);
	
		laserA[i] = laserDirection[i] + turretA;

		laserCXY[i].x = laserXY[i].x + 10000.0*cos(laserA[i]);
		laserCXY[i].y = laserXY[i].y + 10000.0*sin(laserA[i]);

		l[i] = SONARMAXLIMIT;
	}

	for(li=0; li<360; li++)
	{
		POSITION pos = pDoc->m_objects.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawRect* pObj = (CDrawRect*)(pDoc->m_objects.GetNext(pos));
			CRect rect = pObj->m_position;
			CPoint p;
			DPOINT p1, p2;
			DPOINT tp1, tp2;
			double dist;
			p = rect.TopLeft();
			p1.x = p.x; p1.y = p.y;
			p = rect.BottomRight();
			p2.x = p.x; p2.y = p.y;
			switch (pObj->m_nShape)
			{
			case pObj->rectangle:
			case pObj->roundRectangle:
			case pObj->ellipse:
				tp1 = p1; tp2.x = p2.x; tp2.y = p1.y;
				dist = LaserMeasurement_Linesegment(laserXY[li], laserCXY[li], tp1, tp2);
				if(dist >= 0.0 && dist < l[li]) l[li] = dist;
				tp1 = tp2; tp2 = p2;
				dist = LaserMeasurement_Linesegment(laserXY[li], laserCXY[li], tp1, tp2);
				if(dist >= 0.0 && dist < l[li]) l[li] = dist;
				tp1 = tp2; tp2.x = p1.x; tp2.y = p2.y;
				dist = LaserMeasurement_Linesegment(laserXY[li], laserCXY[li], tp1, tp2);
				if(dist >= 0.0 && dist < l[li]) l[li] = dist;
				tp1 = tp2; tp2 = p1;
				dist = LaserMeasurement_Linesegment(laserXY[li], laserCXY[li], tp1, tp2);
				if(dist >= 0.0 && dist < l[li]) l[li] = dist;
				break;

			case pObj->line:
				dist = LaserMeasurement_Linesegment(laserXY[li], laserCXY[li], p1, p2);
				if(dist >= 0.0 && dist < l[li]) l[li] = dist;
				break;
				
			case pObj->robot:
			case pObj->goal:
			default:;
			}
		}

// 		// line 형태 장애물 검사
// 		for(i=0; i<m_Obst->lineN; i++)
// 		{
// 			sdist[si] = SonarMeasurement_Line(sonXY[si], sonCXY[si], sonLXY[si], sonRXY[si], m_Obst->line[i], sdist[si]);
// 		}
// 
// 		// box 형태 장애물 검사 
// 		for(i=0; i<m_Obst->boxN; i++)
// 		{			
// 			sdist[si] = SonarMeasurement_Box(sonXY[si], sonCXY[si], sonLXY[si], sonRXY[si], m_Obst->box[i], sdist[si]);
// 		}
// 
// 		// cylinder 형태 장애물 검사
// 		for(i=0; i<m_Obst->cylinderN; i++)
// 		{
// 			sdist[si] = SonarMeasurement_Cylinder(sonXY[si], sonCXY[si], sonLXY[si], sonRXY[si], m_Obst->cylinder[i], sdist[si]);
// 		}
	}
}

int CRobotControl::InitialUpdate(CDrawView *pView)
{
// initialize UKF
// 	m_pUKF->xtrue->data.db[0] = pView->GetDocument()->m_robot->CenterPtF().X;
// 	m_pUKF->xtrue->data.db[1] = pView->GetDocument()->m_robot->CenterPtF().Y;
// 	m_pUKF->xtrue->data.db[2] = 0;
// 
// 	m_pUKF->XX->data.db[0] = pView->GetDocument()->m_robot->CenterPtF().X;
// 	m_pUKF->XX->data.db[1] = pView->GetDocument()->m_robot->CenterPtF().Y;
// 	m_pUKF->XX->data.db[2] = 0;


	m_pUKF->xtrue->data.db[0] = 0;
	m_pUKF->xtrue->data.db[1] = 0;
	m_pUKF->xtrue->data.db[2] = 0;

	m_pUKF->XX->data.db[0] = 0;
	m_pUKF->XX->data.db[1] = 0;
	m_pUKF->XX->data.db[2] = 0;

	cvZero(m_pUKF->PX);
	m_pUKF->PX->data.db[0] = 2.2204e-016;
	m_pUKF->PX->data.db[4] = 2.2204e-016;
	m_pUKF->PX->data.db[8] = 2.2204e-016;

	m_pUKF->Vn = D_V;
	m_pUKF->Gn = 0;
	return 0;
}

int CRobotControl::ControlStart()
{
	return 0;
}

