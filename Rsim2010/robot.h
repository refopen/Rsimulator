#include <vector>
#include "UKF.h"

#ifndef __ROBOT_H__
#define __ROBOT_H__

class CDrawView;

#define WIDTH	500
#define HEIGHT	500

#define MAXVELOCITY			200
#define SONARMAXLIMIT		300
#define SONARBEAMANGLE		0.61	//Radians(35.0)
#define ROBOTRADIUS			15.0

class CRobotControl
{
// Construction
public:
	CRobotControl();

// Attributes
public:

// Operations
public:

// Implementation
public:
	int ControlStart();
	int InitialUpdate(CDrawView *pView);
	void LaserMeasurement(CDrawView *pView);
	double LaserMeasurement_Linesegment(DPOINT lxy, DPOINT lcxy, DPOINT line0, DPOINT line1);
	double y;
	double x;
	double CrossPoint2(double x1, double y1, double x2, double y2, 
			double xp, double yp, double *xc, double *yc);
	int IsCrossLine(double x1, double y1, double x2, double y2, 
			double x3, double y3, double x4, double y4, double *xc, double *yc, double *UA, double *UB);
	double SonarMeasurement_Linesegment(DPOINT sxy, DPOINT scxy, DPOINT slxy, DPOINT srxy, DPOINT line0, DPOINT line1);
	void SonarMeasurement(CDrawView *pView);
	void DistanceTransform(int Width, int Height, int sx, int sy, int gx, int gy);
	BOOL Recursion(int i, int j, BOOL direction);
	BOOL Iterative(int i, int j, int Width, int Height);
	BOOL SizeOfObstacle(int n, int m, int i[2]);
	double MoveTo(CDrawView *pView, double x, double y);
	void Update(CDrawView *pView);
	std::vector<CPoint> Path;
	int m_nTable[WIDTH][HEIGHT];
	BOOL m_bChecked[WIDTH][HEIGHT];
	CUKF *m_pUKF;
	
	int dv;			// desired translational velocity
	int rv;			// translational speed
	int vl;
	int vr;
	int angle;
	CPoint goal;
	double sonarDirection[10];
	double sonarOffset[10];
	double sonarAngle[10];
	DPOINT sonarPos[10];
	DPOINT laserPos[360];
	double laserDirection[360];
	double s[10];
	double l[360];

	int Uflag;

	virtual ~CRobotControl();
 
protected:
public:
};

#endif // _ROBOT_H__