// refopen.blogspot.com
//
// OccupancyGrid.cpp: implementation of the OccupancyGrid class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OccupancyGrid.h"
#include "robot.h"
#include "util.h"

// All of parameters needs to be converted to radian from degree.
// double soffset[3] = {18.5, 19, 18.5};
// double sangle[3] = {42.4, 0.0, -42.4};
// double sdir[3] = {42.4, 0.0, -42.4};


// 이 줄은 github 테스트 메시지 입니다.


OccupancyGrid::OccupancyGrid(void)
: Map(NULL)
{
	Map = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	Rmap = new unsigned char[WIDTH*HEIGHT];
	memset(Rmap, 0, sizeof(char)*WIDTH*HEIGHT);

	memset(Map->imageData, 128, sizeof(char)*MAPSIZE*MAPSIZE);
	cvNamedWindow("Occupancy Grid", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("Occupancy Grid", 0, 0);

	int i;
	for(i=0; i<10; i++)	sdir[i] = i*36*PI/180.0;
	for(i=0; i<10; i++)	soffset[i] = ROBOTRADIUS;
	for(i=0; i<10; i++)	sangle[i] = i*36*PI/180.0;

	FILE *fd;
	int x, y, index;
	char str[100];
	double t[201*201];
	double temp;

	// load distance table	
	fopen_s(&fd, "./table/distance_txt_201.table", "r");
	if(fd == NULL) AfxMessageBox("[LoadTable] distance_txt_201.table File open error!\n");		
	index = 0;
	while(fgets(str, 100, fd))
	{
		if(sscanf_s(str, "%lf", &temp) < 0) break;
		t[index++] = temp;
	}	
	fclose(fd);
	for(y=0; y<201; y++)
		for(x=0; x<201; x++)
			Dtable[y*201 + x] = (t[y*201 + x] / 100.0) * CELLSIZE;

	// load angle index table
	fopen_s(&fd, "./table/index_txt_201.table", "r");
	if(fd == NULL) AfxMessageBox("[LoadTable] index_txt_201.table File open error!\n");		
	index = 0;
	while(fgets(str, 100, fd))
	{
		if(sscanf_s(str, "%lf", &temp) < 0) break;
		t[index++] = temp;
	}	
	fclose(fd);
	for(y=0; y<201; y++)
		for(x=0; x<201; x++)
			Itable[y*201 + x] = (int)t[y*201 + x];	

	mm=cvCreateImage(cvSize(MAPSIZE*2,MAPSIZE*2), IPL_DEPTH_8U, 1);
}

OccupancyGrid::~OccupancyGrid(void)
{
	delete Rmap;
	cvReleaseImage(&mm);
}

void OccupancyGrid::ViewMap(void)
{
// 	memcpy(Map->imageData, Rmap, sizeof(char)*WIDTH*HEIGHT);
// 	cvNamedWindow("Grid");
// 	cvShowImage("Grid", Map);

//	IplImage* mm;
//	mm=cvCreateImage(cvSize(MAPSIZE*2,MAPSIZE*2), IPL_DEPTH_8U, 1);

	cvResize(Map, mm, 0);
	cvShowImage("Occupancy Grid", mm);
//	cvReleaseImage(&mm);
}

int OccupancyGrid::UpdateMap(double *s, double x, double y, double t)
{
	// 기타 임시변수
	int k, xi, yi;
//	int sxi, syi;
	int xi_start, yi_start, xi_length, yi_length;
	double sx, sy;
	double dist;
	double pO, pO_, pSO;//, pSO_;
	double temp;
	//double ox, oy;
	//int oxi, oyi;
	double xx, yy, tt, diffangle;

 	for(k=0; k<10; k++)		
	{	
		sx = x + soffset[k]*cos((t+sangle[k]));
		sy = y + soffset[k]*sin((t+sangle[k]));

		//ox = sx + s[k]*cos(s[k]);
		//oy = sy + s[k]*sin(s[k]);
		//RealP2GridP(ox, oy, &oxi, &oyi);
		
		if(s[k] > SONARLIMIT) s[k] = SONARLIMIT + CELLSIZE;
		
		CalculateSonarArea(sx, sy, t+sdir[k], s[k]+3.0, &xi_start, &yi_start, &xi_length, &yi_length);

		for(yi=yi_start; yi<(yi_start+yi_length); yi++)
		{
			for(xi=xi_start; xi<(xi_start+xi_length); xi++)
			{
				if(xi < 0 || yi < 0 || xi >= MAPSIZE || yi >= MAPSIZE) continue;

				GridP2RealP(xi, yi, &xx, &yy);
				tt = atan2(yy-sy, xx-sx);
				tt = NormalizeAngleInRadian(tt);
				diffangle = fabs(tt - NormalizeAngleInRadian(t+sangle[k]));
				if(diffangle > DEG2RAD(15.0)) continue;

				dist = GetDistance(sx, sy, xx, yy);
				if(dist > SONARLIMIT) continue;
						
				pO = (double)Rmap[yi*MAPSIZE + xi] / 100.0;
				pO_ = 1.0 - pO;

				if(dist < (s[k] + 0.0 - (CELLSIZE/1.0)))	// Region II
					pSO = 0.5 - ((OCCUPANCYNP*(SONARLIMIT-dist)/SONARLIMIT) * exp(diffangle));
				else if(dist >= (s[k] + 0.0 - (CELLSIZE/1.0)) && dist < (s[k] + 0.0 +(CELLSIZE/1.0)))
					pSO = 0.5 + ((OCCUPANCYP*(SONARLIMIT-dist)/SONARLIMIT) * exp(diffangle));
				else
					pSO = 0.5;
																	
				temp = 1.0 / (1.0 + ((1.0-pSO)/pSO)*((1.0-pO)/pO));							
				
				if(temp > 0.95) temp = 0.95;
				if(temp < 0.3)  temp = 0.3;
				
				if(temp < 0.0) Rmap[yi*MAPSIZE + xi] = 50;//0.5;
				else if(temp > 1.0) Rmap[yi*MAPSIZE + xi] = 50;//0.5;
				else Rmap[yi*MAPSIZE + xi] = (int)(temp*100.0);
				
				int color = (int)((100 - Rmap[yi*MAPSIZE + xi]) * 2);

// 				Map->imageData[yi*MAPSIZE*3 + xi*3 + 0] = color;
// 				Map->imageData[yi*MAPSIZE*3 + xi*3 + 1] = color;
// 				Map->imageData[yi*MAPSIZE*3 + xi*3 + 2] = color;

				Map->imageData[yi*MAPSIZE + xi] = color;
			}
		}
	}
	return 0;
}

int OccupancyGrid::RealP2GridP(double x, double y, int *xi, int *yi)
{
	*xi = (int)floor((x/CELLSIZE)+0.5) + HALFMAPSIZE;
	*yi = HALFMAPSIZE - (int)floor((y/CELLSIZE)+0.5);
	
	if(*xi < 0 || *xi >= MAPSIZE || *yi < 0 || *yi >= MAPSIZE)
	{
		TRACE("### RealP2GridP() Function Error! -- *xi[%d] *yi[%d]###\n", *xi, *yi);
		return -1;
	}
	return 0;
}

void OccupancyGrid::GridP2RealP(int xi, int yi, double *x, double *y)
{
 	*x = (xi - HALFMAPSIZE) * CELLSIZE;
	*y = (HALFMAPSIZE - yi) * CELLSIZE;
}

void OccupancyGrid::CalculateSonarArea(double x, double y, double a, double d, int *xi, int *yi, int *xl, int *yl)
{
	int i;
	double xx[4], yy[4];
	double minx, miny, maxx, maxy;

	xx[0] = x;
	yy[0] = y;

	xx[1] = x + d*cos(a);
	yy[1] = y + d*sin(a);

	xx[2] = x + d*cos(a+SONARANGLE);
	yy[2] = y + d*sin(a+SONARANGLE);

	xx[3] = x + d*cos(a-SONARANGLE);
	yy[3] = y + d*sin(a-SONARANGLE);

	minx=5000.0;
	miny=5000.0;
	maxx=-5000.0;
	maxy=-5000.0;
	for(i=0; i<4; i++)
	{
		if(xx[i] < minx) minx = xx[i];
		if(yy[i] < miny) miny = yy[i];

		if(xx[i] > maxx) maxx = xx[i];
		if(yy[i] > maxy) maxy = yy[i];
	}

	*xi = (int)floor((minx/CELLSIZE)+0.5) + HALFMAPSIZE;
	*yi = HALFMAPSIZE - (int)floor((maxy/CELLSIZE)+0.5);

	*xl = (int)floor(((maxx-minx)/CELLSIZE)+0.5);
	*yl = (int)floor(((maxy-miny)/CELLSIZE)+0.5);
}

double OccupancyGrid::GetDistance(double x, double y, double x1, double y1)
{
	return sqrt((x-x1)*(x-x1) + (y-y1)*(y-y1));					
}

double OccupancyGrid::GetVFF(double xx, double yy, double gx, double gy, CRobotControl *robot)
{
	int xi, yi;
	int i;
	double distpow, Fc, Fri = 0.0, Frj = 0.0, Fai = 0.0, Faj = 0.0, Fi = 0.0, Fj = 0.0, dist, angle;
	static double fangle = 0.0;
	double Frc = 1.0;

	LocalHistogram(xx, yy);
	for(i=0; i<360; i++)
	{
		if(ph[i] < 300.0)
		{
			dist = GetDistance(histxy[i].x, histxy[i].y, xx, yy);
			distpow = dist*dist;
			RealP2GridP(histxy[i].x, histxy[i].y, &xi, &yi);

			Fc = Frc*(Rmap[yi*MAPSIZE+xi]/distpow);
			Fri -= Fc*(histxy[i].x - xx)/dist;
			Frj -= Fc*(histxy[i].y - yy)/dist;
		}
	}

	// adjusted repulsive force
	double w = 0.75, cost, Vi, Vj;
// 	Vi = robot->rv*cos(DEG2RAD(robot->angle));
// 	Vj = robot->rv*sin(DEG2RAD(robot->angle));

	Vi = MAXVELOCITY*cos(DEG2RAD(robot->angle));
 	Vj = MAXVELOCITY*sin(DEG2RAD(robot->angle));

	if((sqrt(Vi*Vi+Vj*Vj)*sqrt(Fri*Fri+Frj*Frj)) != 0)
	{
		cost = (Vi*Fri + Vj*Frj)/(sqrt(Vi*Vi+Vj*Vj)*sqrt(Fri*Fri+Frj*Frj));
		Fri = w*Fri + (1-w)*Fri*(-cost);
		Frj = w*Frj + (1-w)*Frj*(-cost);
	}

/*
	//현재위치를 장애물이 있는 것으로 표기해 repulsive force를 발생시킴.
	RealP2GridP(ri->rPos.x, ri->rPos.y, &xi, &yi);
	ri->AMap[yi*D_GLOBAL_GRID_SIZE+xi] = 50;
	LocalHistogram2();
	for(i=0; i<360; i++)
	{
		if(ri->ph[i] < 100.0)
		{
			dist = GetDistance(ri->histxy[i].x, ri->histxy[i].y, xx, yy);
			distpow = dist*dist;
			RealP2GridP(ri->histxy[i].x, ri->histxy[i].y, &xi, &yi);

			Fc = (ri->AMap[yi*D_GLOBAL_GRID_SIZE+xi]/distpow);
			Fri2 -= Fc*(ri->histxy[i].x - xx)/dist;
			Frj2 -= Fc*(ri->histxy[i].y - yy)/dist;
		}
	}
*/	
	//ri->CollisionFlag = 1;
	//TRACE("Fri: %f, Frj: %f\n", Fri, Frj);

	dist = GetDistance(gx, gy, xx, yy);
	Fai = (gx - xx)/dist;
	Faj = (gy - yy)/dist;
	//TRACE("Fai: %.1f, Faj: %.1f\n", Fai, Faj);
	// test attraction force를 빼기 위함.
	Fi = Fri + Fai;
	Fj = Frj + Faj;

/*
	Fi = (Fri+Fri2)/2.0;
	Fj = (Frj+Frj2)/2.0;
*/
/*
	if(fabs(Fi) < 0.001 && fabs(Fj) < 0.001)
	{
		AfxMessageBox("끝났나?");
	}
*/
	angle = AbsoluteAngleBetweenTwoPoints(0, 0, Fi, Fj);
	//TRACE("repulsive angle : %1.f\n", angle);

	fangle = ((0.05*angle)+(0.1-0.05)*fangle)/0.1;

	if(Fri==0 && Frj==0)	robot->dv = MAXVELOCITY;
	else	robot->dv = MAXVELOCITY*(1-cost);

	return fangle;
}

int OccupancyGrid::Mapping(double x, double y)
{
	int xi, yi;

	if(RealP2GridP(x, y, &xi, &yi) < 0)
		return -1;
//	Rmap[yi*MAPSIZE+xi]++;
	Rmap[yi*MAPSIZE+xi] = 255;

	return 0;
}

void OccupancyGrid::LocalHistogram(double xx, double yy)
{
	int x, y, D, i, j; 

	for(x=0; x<360; x++) 
	{
		ph[x] = 300.0;
		histxy[x].x = 900000.0;
		histxy[x].y = 900000.0;
	}
			
// 	xx = ri->rPos.x;
//	yy = ri->rPos.y;

	D = (D_GRID_TABLE_SIZE - D_LOCAL_GRID_SIZE) / 2;
		
	i = HALFMAPSIZE + (int)floor((xx/CELLSIZE)+0.5) - D_LOCAL_GRID_HALF_SIZE;
	j = HALFMAPSIZE - (int)floor((yy/CELLSIZE)+0.5) - D_LOCAL_GRID_HALF_SIZE;
	
	for(y=j; y<j+D_LOCAL_GRID_SIZE; y++)
	{
		for(x=i; x<i+D_LOCAL_GRID_SIZE; x++)
		{
			if(Rmap[y*MAPSIZE + x] >= D_THRESHOLD)
			{
				if(Dtable[((y-j)+D)*D_GRID_TABLE_SIZE + ((x-i)+D)] < 
					ph[Itable[((y-j)+D)*D_GRID_TABLE_SIZE + ((x-i)+D)]])
				{
					ph[Itable[((y-j)+D)*D_GRID_TABLE_SIZE + ((x-i)+D)]] = 
						Dtable[((y-j)+D)*D_GRID_TABLE_SIZE + ((x-i)+D)];
					histxy[Itable[((y-j)+D)*D_GRID_TABLE_SIZE + ((x-i)+D)]].x = 
						((double)(x-i)-D_LOCAL_GRID_HALF_SIZE)*CELLSIZE + xx;
					histxy[Itable[((y-j)+D)*D_GRID_TABLE_SIZE + ((x-i)+D)]].y = 
					 	(D_LOCAL_GRID_HALF_SIZE-(double)(y-j))*CELLSIZE + yy;
				}
			}
		}
	}		
}

int OccupancyGrid::ShowHistogram()
{
	int maxHeight = 300;
	IplImage *hist;
	int i;

	hist = cvCreateImage(cvSize(360, maxHeight), IPL_DEPTH_8U, 3);

	for(i=0;i<360; i++)
	{
		cvLine(hist, cvPoint(i, maxHeight-sph[i]), cvPoint(i, maxHeight), CV_RGB(128,0,0));
	}

	cvNamedWindow("hist");
	cvShowImage("hist", hist);
	cvReleaseImage(&hist);

	return 0;
}

int OccupancyGrid::SmoothPolarHistogramDensity(CRobotControl *robot)
{
	int i, j;
	int sum;
	int l = 5;
	int idx;
	
	for(i=0; i<360; i++)
	{
		sum = 0;
		for(j=-l; j<l; j++)
		{
			idx = NormalizeAngle(i+j);
			if(ph[idx] != 300)	sum += ph[idx];
		}
		sph[i] = sum / (2*l+1);
	}

	return 0;
}

int OccupancyGrid::GetVFH(double x, double y, double gx, double gy)
{
	struct  
	{
		int center;
		int start;
		int end;
		int no;
	}clusterst[360];
	memset(&clusterst, 0, sizeof(clusterst));

	int i = 0, j = 0, k = 0;
	int remainangle = 360;
	int clusteridx = 0;
	int start=0, end=360;
	char flag = 0;

	//while(remainangle > 0)
	for(i=start; i<end; i++)
	{
		if(remainangle <= 0) break;
		j = i;
		flag = 0;
		while(sph[i] == 0)
		{
			if(remainangle <= 0) break;
			clusterst[clusteridx].no++;
			clusterst[clusteridx].end = i;
			i--;
			i = NormalizeAngle(i);
			remainangle--;
			flag = 1;
		}
		while(sph[j] == 0)
		{
			if(remainangle <= 0) break;
			clusterst[clusteridx].no++;
			clusterst[clusteridx].start = j;
			j++;
			j = NormalizeAngle(j);
			remainangle--;
			flag = 1;
		}
		if(flag)
		{
			clusterst[clusteridx].no -= 1;
			i = clusterst[clusteridx].start;
			clusteridx++;
		}
		else remainangle--;
	}

	double targetAngle = AbsoluteAngleBetweenTwoPoints(x, y, gx, gy);

	for(i=0; i<clusteridx; i++)
	{
		
	}

	return 0;
}
