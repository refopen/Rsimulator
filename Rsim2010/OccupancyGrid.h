#include "robot.h"

#pragma once

#define MAPSIZE					500
#define CELLSIZE				5.0
#define HALFMAPSIZE				250
#define SONARANGLE				0.52			// 30degree in radians
#define	SONARLIMIT				100.0
#define OCCUPANCYNP			 	(double)0.15
#define OCCUPANCYP				(double)0.9
#define FORCEFIELDSIZE			15
#define D_THRESHOLD				(double)80

#define DEG2RAD(x)	((x)*(PI)/(180.0))
#define RAD2DEG(x)	((x)*(180.0)/(PI))

#define D_GRID_TABLE_SIZE		(int)201
#define D_GRID_TABLE_HALF_SIZE  (int)100
#define D_LOCAL_GRID_SIZE		(int)101
#define D_LOCAL_GRID_HALF_SIZE	(int)50

class OccupancyGrid
{
public:
	IplImage* mm;
	DPOINT histxy[360];
	double sph[360];
	double ph[360];
	double soffset[10];
	double sangle[10];
	double sdir[10];
	double Dtable[D_GRID_TABLE_SIZE*D_GRID_TABLE_SIZE];
	int Itable[D_GRID_TABLE_SIZE*D_GRID_TABLE_SIZE];
	int GetVFH(double x, double y, double gx, double gy);
	int SmoothPolarHistogramDensity(CRobotControl *robot);
	int ShowHistogram();
	void LocalHistogram(double xx, double yy);
	int Mapping(double x, double y);
	OccupancyGrid(void);
	~OccupancyGrid(void);
	void ViewMap(void);
	unsigned char *Rmap;
	IplImage *Map;
	IplImage *VFF;
	int UpdateMap(double * s, double x, double y, double t);
	int RealP2GridP(double x, double y, int *xi, int *yi);
	void GridP2RealP(int xi, int yi, double *x, double *y);
	void CalculateSonarArea(double x, double y, double a, double d, int *xi, int *yi, int *xl, int *yl);
	double GetDistance(double x, double y, double x1, double y1);
	double GetVFF(double x, double y, double gx, double gy, CRobotControl *robot);
	double GetAngleInMap(int gxi, int gyi, int rxi, int ryi, double cangle);
};
