// UKF.h: interface for the CUKF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UKF_H__BCF25FEC_6D62_416C_B06A_C8174908DDF7__INCLUDED_)
#define AFX_UKF_H__BCF25FEC_6D62_416C_B06A_C8174908DDF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

// control parameters

#define D_V  10   // m/s
#define MAXG  30*PI/180   // radians, maximum steering angle (-MAXG < g < MAXG)
#define RATEG  20*PI/180   // rad/s, maximum rate of change in steer angle
#define WHEELBASE  4   // metres, vehicle wheel-base
#define DT_CONTROLS  0.075   // seconds, time interval between control signals

// observation parameters
#define MAX_RANGE  30.0*DRAW_SCALE*1.4142   // meters
#define DT_OBSERVE  8*DT_CONTROLS   // seconds, time interval between observations

// waypoint proximity
#define AT_WAYPOINT  5.0   // metres, distance from current waypoint at which to switch to next waypoint
#define NUMBER_LOOPS  2   // number of loops through the waypoint list

// switches
#define SWITCH_CONTROL_NOISE  1   // if 0, velocity and gamma are perfect
#define SWITCH_SENSOR_NOISE   1   // if 0, measurements are perfect
#define SWITCH_INFLATE_NOISE  0   // if 1, the estimated Q and R are inflated (ie, add stabilising noise)
#define SWITCH_HEADING_KNOWN  0   // if 1, the vehicle heading is observed directly at each iteration
#define SWITCH_SEED_RANDOM  0   // if not 0, seed the randn() with its value at beginning of simulation (for repeatability)
#define SWITCH_PROFILE  0   // if 1, turn on MatLab profiling to measure time consumed by simulator functions
#define SWITCH_GRAPHICS  1   // if 0, avoids plotting most animation data to maximise simulation speed

#define EP			30		// # of ellipse points

class CDrawDoc;
class CDrawView;

typedef struct tag_VisibleLandMark 
{
	double x;
	double y;
	int	   idx;
}VISLM;

typedef struct tagDPOINT
{
	double x;
	double y;
} DPOINT;

#define MOTION_MODEL	0
#define AUGMENT_MODEL	1


class CUKF  
{
public:
	int EllipseFitting();
	double ComputeSteering(double x, double y);
	int Doit();
	int Augment();
	int UnscentedUpdate(int idf, int idx);
	int Update();
	int KnownDataAssociation();
	int AddObservationNoise();
	std::vector<VISLM> zn;
	std::vector<VISLM> zf;
	std::vector<DPOINT> lm;
	std::vector<DPOINT> wp;
	std::vector<VISLM> vlm;
	std::vector<VISLM> z;
	std::vector<int> daTable;
	int ComputeRangeBearing(double &r, double &phi, DPOINT lm);
	void GetVisibleLandmark();
	CDrawDoc *m_pDoc;
	CvMat *Q;
	CvMat *R;
	CvMat *XX;
	CvMat *PX;
// 	CvMat *XXA;
// 	CvMat *PXA;
	CvMat *Z;
	CvMat *xtrue;
	CvMat *Xodometry;
	CvMat *EllipseV;
	CvMat *EllipseF[100];
	void MatSqrt(CvMat *a, CvMat *b);
	int VehicleDiff();
	int UnscentedTransform(int model);
	int VehicleModel();
	int Prediction();
	double GaussianRandom(void);
	int AddControlNoise();
	// control noises
	double sigmaV;
	double sigmaG;

	// observation noises
	double sigmaR;
	double sigmaB;

	double V;
	double G;
	double Vn;
	double Gn;
	CUKF(CDrawDoc *pDoc, CDrawView *pView);
	virtual ~CUKF();
	int SetNoiseParameter();
	CvMat *m_sigmaPointst_1;
	CvMat *m_sigmaPointst;
};

#endif // !defined(AFX_UKF_H__BCF25FEC_6D62_416C_B06A_C8174908DDF7__INCLUDED_)
