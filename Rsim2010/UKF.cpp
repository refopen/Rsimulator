// refopen.blogspot.com
//
//cvCloneMat, cvCloneImage 를 사용할 때 주의해야 한다.
//포인터를 생성하는 순간 copy되지 않으면 새로운 구조체를 할당하기 때문에
//CvMat *temp = cvCloneMat(src); 와 같이 사용해야한다.
//아니면 cvCopy를 쓰거나...
// UKF.cpp: implementation of the CUKF class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "drawcli.h"
#include "UKF.h"

#include "drawdoc.h"
#include "drawobj.h"
#include "drawvw.h"

#include "drawobj.h"
#include "drawtool.h"
#include "mainfrm.h"

#include "util.h"
#include <vector>

#define DATAPATH	"./"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define PI	3.141592

//#define NOISEFREE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUKF::CUKF(CDrawDoc *pDoc, CDrawView *pView)
{
	m_sigmaPointst_1 = cvCreateMat(1, 1, CV_64FC1);
	m_sigmaPointst = cvCreateMat(1, 1, CV_64FC1);

	Z = cvCreateMat(1, 1, CV_64FC1);
	xtrue = cvCreateMat(3, 1, CV_64FC1);

	sigmaV = 0.3;
	sigmaG = 3.0;
	//sigmaV = 3.0;
	//sigmaG = 10.0;
	sigmaR = 0.1;
	sigmaB = 1.0;

	SetNoiseParameter();

#ifdef NOISEFREE
	cvZero(Q);
	cvZero(R);
#endif //NOISEFREE

	// gaussianNoise test
// 	FILE *fd;
// 	fd = fopen("gaussian.txt", "w+");
// 	for(int i=0; i<1000; i++)
// 		fprintf(fd, "%.3f\n", GaussianRandom());
// 	fclose(fd);

	G = 0;

	XX = cvCreateMat(3, 1, CV_64FC1);
	PX = cvCreateMat(3, 3, CV_64FC1);
	Xodometry = cvCreateMat(3, 1, CV_64FC1);
	cvZero(Xodometry);

	m_pDoc = pDoc;

	// landmark position init.
	//POSITION pos = m_pDoc->m_objects.GetHeadPosition();
	//while (pos != NULL)
	//{
	//	CDrawRect* pObj = (CDrawRect*)(m_pDoc->m_objects.GetNext(pos));
	//	CRect rect = pObj->m_position;
	//	CPoint p;
	//	DPOINT p1, p2;
	//	DPOINT m;
	//	p = rect.TopLeft();
	//	p1.x = p.x; p1.y = p.y;
	//	p = rect.BottomRight();
	//	p2.x = p.x; p2.y = p.y;
	//	switch (pObj->m_nShape)
	//	{
	//	case pObj->rectangle:
	//	case pObj->roundRectangle:
	//	case pObj->ellipse:
	//	case pObj->line:
	//		m.x = (p1.x+p2.x)/2;
	//		m.y = (p1.y+p2.y)/2;
	//		lm.push_back(m);
	//		daTable.push_back(0);
	//		break;
	//		
	//	case pObj->robot:
	//	case pObj->goal:
	//	default:;
	//	}
	//}

	// load landmark, waypoint data load
	double tempx[500], tempy[500], tempx2[500], tempy2[500];
	CString path = DATAPATH;
	char str[500];
	FILE *fd;
	path += "lmdata2.dat";
	fopen_s(&fd, path, "r");
	if(fd == NULL) AfxMessageBox(str);
	int index = 0;
	while(fgets(str, 100, fd))
	{
		if(sscanf_s(str, "%lf	%lf", &tempx[index], &tempy[index]) < 0) break;
		index++;
	}	
	fclose(fd);
	int n1 = index;

	path = DATAPATH;
	path += "wpdata2.dat";
	fopen_s(&fd, path, "r");
	if(fd == NULL) AfxMessageBox(str);
	index = 0;
	while(fgets(str, 100, fd))
	{
		if(sscanf_s(str, "%lf	%lf", &tempx2[index], &tempy2[index]) < 0) break;
		index++;
	}	
	fclose(fd);
	int n2 = index;

	lm.clear();
	pDoc->goalVector.clear();

	// lm, wp test
	//double tempx[15] = 	
	//{
	//	-14.86175115207373	,-1.497695852534569	,21.774193548387103	,19.239631336405523	,-22.60705289672544	,33.18639798488665,	25.503778337531486,	41.12090680100755,	21.599496221662463,	-19.20654911838791,	-34.823677581863976,	-36.08312342569269,	-30.66750629722922,	-21.599496221662466	,-6.738035264483628
	//};
	//double tempy[15] = 
	//{
	//	5.994152046783626,	-9.795321637426895,	-8.62573099415205,	15.935672514619881	,-1.113172541743964	,18.181818181818187,	3.710575139146577,	-10.018552875695718,	-37.476808905380324	,29.499072356215223	,16.141001855287584,	-3.896103896103888,	-23.37662337662337,	-34.32282003710574,	-27.45825602968459
	//};
	//double tempx2[15] = 	
	//{
	//	0,	6.234256926952142,	12.657430730478595,	25,	30.793450881612088	,17.82115869017632,	7.241813602015114,	0.115207373271893,	-4.262672811059908,	-17.44332493702771	,-30.037783375314863,	-28.652392947103277	,-19.962216624685137,	-5.414746543778797,	0.345622119815666
	//};
	//double tempy2[15] = 
	//{
	//	0,	-14.842300556586267	,-24.675324675324674,	-18.738404452690162,	4.452690166975891,	17.439703153988887,	14.285714285714292,	-0.438596491228068,	-7.456140350877192,	-25.788497217068638	,-12.059369202226335,	7.421150278293148,	20.779220779220793,	10.380116959064331,	-0.146198830409354
	//};

	int i;
	DPOINT dp;	
	CPoint cp;
	for(i=0; i<n1; i++)
	{
		DPOINT dp;
		dp.x = tempx[i]*DRAW_SCALE; 
		dp.y = tempy[i]*DRAW_SCALE;
		lm.push_back(dp);
		daTable.push_back(0);
	}
	for(i=0; i<n2; i++)
	{
		cp.x = tempx2[i]*DRAW_SCALE; 
		cp.y = tempy2[i]*DRAW_SCALE;
		dp.x = tempx2[i]*DRAW_SCALE; 
		dp.y = tempy2[i]*DRAW_SCALE;
		pDoc->goalVector.push_back(cp);
		wp.push_back(dp);
	}
	pView->m_Control->goal.x = tempx2[0]*DRAW_SCALE;
	pView->m_Control->goal.y = tempy2[0]*DRAW_SCALE;

	EllipseV = cvCreateMat(2, EP, CV_64FC1);
	cvZero(EllipseV);
	for(i=0; i<n1; i++)
	{
		EllipseF[i] = cvCreateMat(2, EP, CV_64FC1);
		cvZero(EllipseF[i]);
		CDrawRect* pObj = new CDrawRect(CRect(CPoint(lm[i].x, lm[i].y), CSize(10, 10)));
		pObj->m_nShape = CDrawRect::ellipse;
		pDoc->Add(pObj);
	}
}

CUKF::~CUKF()
{
	for(int i=0; i<wp.size(); i++) cvReleaseMat(&EllipseF[i]);
	cvReleaseMat(&EllipseV);
	cvReleaseMat(&Z);
	cvReleaseMat(&XX);
	cvReleaseMat(&PX);
	cvReleaseMat(&xtrue);
	cvReleaseMat(&Q);
	cvReleaseMat(&R);
}

int CUKF::AddControlNoise()
{
  	Vn= D_V + GaussianRandom()*sqrt(Q->data.db[0]);
  	Gn= G + GaussianRandom()*sqrt(Q->data.db[3]);
// 	Vn = D_V + 0.0001;
// 	Gn = G + 0.0001;
#ifdef NOISEFREE
 	Vn= D_V;
 	Gn= G;
#endif //NOISEFREE

	return 0;
}

double CUKF::GaussianRandom(void)
{
  double v1, v2, s;
  //srand(time(NULL));

  do 
  {
    v1 =  2 * ((double) rand() / RAND_MAX) - 1;      // -1.0 ~ 1.0 까지의 값
    v2 =  2 * ((double) rand() / RAND_MAX) - 1;      // -1.0 ~ 1.0 까지의 값
    s = v1 * v1 + v2 * v2;
  } while (s >= 1 || s == 0);
  
  s = sqrt( (-2 * log(s)) / s );
  
  return v1 * s;
}

int CUKF::Prediction()
{
	int rows = XX->rows;
	int cols = XX->cols;
	int i, j;

	//temp1 = cvCreateMat(rows, cols, CV_64FC1);
	//cvCopy(XX, temp1);
	//cvReleaseMat(&XX);
	CvMat *temp1 = cvCloneMat(XX);
	XX = cvCreateMat(rows+2, cols, CV_64FC1);

	for(i=0; i<rows; i++)	XX->data.db[i] = temp1->data.db[i];
	XX->data.db[rows+2-2] = Vn;
	XX->data.db[rows+2-1] = Gn;

	rows = PX->rows;
	cols = PX->cols;

	CvMat *temp2 = cvCloneMat(PX);
	cvReleaseMat(&PX);
	PX = cvCreateMat(rows+2, cols+2, CV_64FC1);
	cvZero(PX);
	for(i=0; i<rows; i++)	
		for(j=0; j<cols; j++)	
			PX->data.db[i*PX->cols+j] = temp2->data.db[i*cols+j];
	i = PX->rows-2;	PX->data.db[i*PX->cols+i] = Q->data.db[0];
	i = PX->rows-1;	PX->data.db[i*PX->cols+i] = Q->data.db[3];

	cvReleaseMat(&temp1);
 	cvReleaseMat(&temp2);

// 	PrintCvMat(XX, "Prediction : XX");
// 	PrintCvMat(PX, "Prediction : PX");

	UnscentedTransform(MOTION_MODEL);

	return 0;
}

int CUKF::VehicleModel()
{
	xtrue->data.db[0] = xtrue->data.db[0] + D_V*DT_CONTROLS*cos(G+xtrue->data.db[2]);
	xtrue->data.db[1] = xtrue->data.db[1] + D_V*DT_CONTROLS*sin(G+xtrue->data.db[2]);
	xtrue->data.db[2] = xtrue->data.db[2] + D_V*DT_CONTROLS*sin(G)/(double)WHEELBASE;
	xtrue->data.db[2] = PI2PI(xtrue->data.db[2]);

//	TRACE("xtrue : %.3f %.3f %.3f	%.3f\n", xtrue->data.db[0], xtrue->data.db[1], xtrue->data.db[2], G);

	Xodometry->data.db[0] = Xodometry->data.db[0] + Vn*DT_CONTROLS*cos(Gn+Xodometry->data.db[2]);
	Xodometry->data.db[1] = Xodometry->data.db[1] + Vn*DT_CONTROLS*sin(Gn+Xodometry->data.db[2]);
	Xodometry->data.db[2] = Xodometry->data.db[2] + Vn*DT_CONTROLS*sin(Gn)/(double)WHEELBASE;
	Xodometry->data.db[2] = PI2PI(Xodometry->data.db[2]);

	return 0;
}

int CUKF::UnscentedTransform(int model)
{
	// Set up some variables
	int dim = cvGetSize(XX).height;
	int N = 2*dim+1;
	double scale = 3;
	int kappa = scale-dim;

	// Create samples
	// SVD
	CvSize P = cvGetSize(PX);
	CvMat *D = cvCreateMat(P.height, P.width, CV_64FC1);
	CvMat *U = cvCreateMat(P.height, P.height, CV_64FC1);
	CvMat *V = cvCreateMat(P.width, P.width, CV_64FC1);
	CvMat *Ds = cvCreateMat(P.height, P.width, CV_64FC1);
	CvMat *UD = cvCreateMat(P.height, P.width, CV_64FC1);
	CvMat *Ps = cvCreateMat(P.height, P.width, CV_64FC1);
	cvSVD(PX, D, U, V, CV_SVD_V_T);
	MatSqrt(D, Ds);
	cvMatMul(U, Ds, UD);
	cvScale(UD, Ps, sqrt(scale), 0);
	
// 	PrintCvMat(U, "U");
// 	PrintCvMat(D, "D");
// 	PrintCvMat(V, "V");
// 	PrintCvMat(Ds, "Ds");
// 	PrintCvMat(UD, "UD");
// 	PrintCvMat(PX, "PX");
// 	PrintCvMat(Ps, "Ps");

	CvMat *ss = cvCreateMat(dim, N, CV_64FC1);

	//for sigma points display
	//cvReleaseMat(&m_sigmaPointst_1);
	//cvReleaseMat(&m_sigmaPointst);
	//m_sigmaPointst_1 = cvCreateMat(dim, N, CV_64FC1);
	//m_sigmaPointst = cvCreateMat(dim, N, CV_64FC1);
	//
	int i,j,jj;
	int row = dim;
	int col = N;
	cvRepeat(XX, ss);
	double* ssdb;
	double* Psdb;

	ssdb = ss->data.db;
	Psdb = Ps->data.db;
	for(i=0; i<row; i++)
	{
		for(j=1, jj=0; j<dim+1; j++, jj++)
		{
			//ss->data.db[i*ss->cols+j] += Ps->data.db[i*Ps->cols+jj];
			ssdb[i*ss->cols+j] += Psdb[i*Ps->cols+jj];
		}
	}

	for(i=0; i<row; i++)
	{
		for(j=1+dim, jj=0; j<ss->cols; j++, jj++)
		{
			//ss->data.db[i*ss->cols+j] -= Ps->data.db[i*Ps->cols+jj];
			ssdb[i*ss->cols+j] -= Psdb[i*Ps->cols+jj];
		}
	}

	//cvRepeat(ss, m_sigmaPointst_1);

	// Transform samples according to motion model
	CvMat *ys;
	CvMat *base;
	CvMat *delta;
	CvMat *reptemp;
	int ysdim;
	double *ysdb;
	double *basedb;
	double *deltadb;
	double *reptempdb;

	switch(model)
	{
	case MOTION_MODEL:
		ysdim = dim-2;
		ys = cvCreateMat(ysdim, N, CV_64FC1);
		base = cvCreateMat(ysdim, N, CV_64FC1);
		delta = cvCreateMat(ysdim, N, CV_64FC1);
		reptemp = cvCreateMat(ysdim, 1, CV_64FC1);
		cvRepeat(ss, ys);
		ysdb = ys->data.db;
		basedb = base->data.db;
		deltadb = delta->data.db;
		reptempdb = reptemp->data.db;
		for(j=0; j<N; j++)
		{
			double v = ssdb[(dim-2)*N+j];
			double g = ssdb[(dim-1)*N+j];
			ysdb[0*N+j] = ssdb[0*N+j] + v*DT_CONTROLS*cos(g+ssdb[2*N+j]);
			ysdb[1*N+j] = ssdb[1*N+j] + v*DT_CONTROLS*sin(g+ssdb[2*N+j]);
			ysdb[2*N+j] = ssdb[2*N+j] + v*DT_CONTROLS*sin(g)/WHEELBASE;		
		}
		for(i=0; i<ys->rows; i++)	reptempdb[reptemp->cols*i] = ysdb[ys->cols*i];
 		cvRepeat(reptemp, base);
 		cvSub(base, ys, delta);
		for(i=0; i<base->cols; i++)	basedb[2*base->cols+i] = PI2PI(basedb[2*base->cols+i]);
		cvSub(base, delta, ys);
		//PrintCvMat(ys, "ys");
 		//PrintCvMat(reptemp, "reptemp");
		//PrintCvMat(base, "base");
		//PrintCvMat(delta, "delta");
		//PrintCvMat(ys, "ys");
		cvReleaseMat(&base);
		cvReleaseMat(&delta);
		cvReleaseMat(&reptemp);

		//cvRepeat(ys, m_sigmaPointst);
		break;
	case AUGMENT_MODEL:
		//PrintCvMat(ss, "ss");
		ysdim = dim;
		ys = cvCreateMat(dim, N, CV_64FC1);
		double phi, r, b, s, c;
		//PrintCvMat(XX, "XX");
		for(j=0; j<N; j++)
		{
			phi = ssdb[2*ss->cols+j];
			r = ssdb[(ss->rows-2)*ss->cols+j];
			b = ssdb[(ss->rows-1)*ss->cols+j];
			s = sin(phi+b);
			c = cos(phi+b);
			
			ssdb[(ss->rows-2)*ss->cols+j] = ssdb[0*ss->cols+j] + r*c;
			ssdb[(ss->rows-1)*ss->cols+j] = ssdb[1*ss->cols+j] + r*s;
		}
		cvCopy(ss, ys);
		
 		//PrintCvMat(Ps, "Ps");
 		//PrintCvMat(ss, "ss");
		break;
	}

	CvMat *y = cvCreateMat(ysdim, 1, CV_64FC1);
	CvMat *dy = cvCreateMat(ysdim, N, CV_64FC1);
	CvMat *dyu = cvCreateMat(ysdim, 1, CV_64FC1);
	CvMat *dytu = cvCreateMat(1, ysdim, CV_64FC1);
	CvMat *dyl = cvCreateMat(ysdim, N-1, CV_64FC1);
	CvMat *dytl = cvCreateMat(N-1, ysdim, CV_64FC1);
	CvMat *temp = cvCreateMat(ysdim, ysdim, CV_64FC1);
	CvMat *Y = cvCreateMat(ysdim, ysdim, CV_64FC1);
	CvMat *temp1 = cvCreateMat(ysdim, 1, CV_64FC1);
	CvMat *repy = cvCreateMat(ysdim, N, CV_64FC1);

	double *ydb = y->data.db;
	double *dydb = dy->data.db;
	double *dyudb = dyu->data.db;
	double *dytudb = dytu->data.db;
	double *dyldb = dyl->data.db;
	double *dytldb = dytl->data.db;
	double *tempdb = temp->data.db;
	double *Ydb = Y->data.db;
	double *temp1db = temp1->data.db;
	double *repydb = repy->data.db;
	ysdb = ys->data.db;

	for(i=0; i<y->rows; i++)	ydb[i] = 2*kappa*ysdb[ys->cols*i];
	for(i=0; i<y->rows; i++)
	{
		for(j=1; j<N; j++)
		{
			ydb[i] += ysdb[ys->cols*i+j];
		}
	}
	for(i=0; i<y->rows; i++)	ydb[i] /= (2*scale);
	for(i=0; i<y->rows; i++)	temp1db[i] = ydb[y->cols*i];
	cvRepeat(temp1, repy);
	cvSub(ys, repy, dy);

// 	PrintCvMat(temp1, "temp1");
// 	PrintCvMat(repy, "repy");
// 	PrintCvMat(dy, "dy");

// 	for(j=0; j<N; j++)
// 	{
// 		dy->data.db[0*N+j] = ys->data.db[0*N+j] - y->data.db[0];
// 		dy->data.db[1*N+j] = ys->data.db[1*N+j] - y->data.db[1];
// 		dy->data.db[2*N+j] = ys->data.db[2*N+j] - y->data.db[2];
// 	}

	for(i=0; i<ysdim; i++)	dyudb[i] = dydb[i*dy->cols];
	for(i=0; i<ysdim; i++)	for(j=1, jj=0; j<N; j++, jj++)
			dyldb[i*dyl->cols+jj] = dydb[i*dy->cols+j];

// 	PrintCvMat(ys, "ys");
// 	PrintCvMat(dy, "dy");
// 	PrintCvMat(dyu, "dyu");
// 	PrintCvMat(dyl, "dyl");

	cvT(dyu, dytu);
	cvT(dyl, dytl);

	cvMatMul(dyu, dytu, Y);
//	PrintCvMat(Y, "Y");
	cvScale(Y, Y, 2*kappa);
//	PrintCvMat(Y, "Y");
	cvMatMul(dyl, dytl, temp);
//	PrintCvMat(temp, "temp");
	cvAdd(Y, temp, Y);
//	PrintCvMat(Y, "Y");
	cvScale(Y, Y, 1/(double)(2*scale));
//	PrintCvMat(Y, "Y");

	cvReleaseMat(&XX);
	cvReleaseMat(&PX);
	XX = cvCreateMat(y->rows, y->cols, CV_64FC1);
	PX = cvCreateMat(Y->rows, Y->cols, CV_64FC1);

	cvCopy(y, XX);
	cvCopy(Y, PX);

//	PrintCvMat(XX, "XX");
// 	PrintCvMat(PX, "PX");

	cvReleaseMat(&D);
	cvReleaseMat(&U);
	cvReleaseMat(&V);
	cvReleaseMat(&Ds);
	cvReleaseMat(&UD);
	cvReleaseMat(&Ps);
	cvReleaseMat(&ss);
	cvReleaseMat(&ys);
	cvReleaseMat(&y);
	cvReleaseMat(&dy);
	cvReleaseMat(&dyu);
	cvReleaseMat(&dytu);
	cvReleaseMat(&dyl);
	cvReleaseMat(&dytl);
	cvReleaseMat(&temp);
	cvReleaseMat(&Y);
	cvReleaseMat(&temp1);
	cvReleaseMat(&repy);

	return 0;
}

int CUKF::VehicleDiff()
{
	return 0;
}

void CUKF::MatSqrt(CvMat *a, CvMat *b)
{
	int row = a->height;
	int col = a->width;

	int i,j;
	for (i=0; i<row; i++)	for (j=0; j<col; j++)	
		b->data.db[i*col+j] = sqrt(a->data.db[i*col+j]);
}

void CUKF::GetVisibleLandmark()
{
	std::vector<DPOINT> d;

	vlm.clear();
	z.clear();
	for(int i=0; i<lm.size(); i++)
	{
		DPOINT temp1;
		temp1.x = lm[i].x - xtrue->data.db[0];
		temp1.y = lm[i].y - xtrue->data.db[1];
		double phi = xtrue->data.db[2];
		d.push_back(temp1);

// 		TRACE("%.3f	%.3f	%.3f	%.3f\n",
// 			fabs(d[i].x), fabs(d[i].y),
// 			(d[i].x*cos(phi) + d[i].y*sin(phi)),
//			(d[i].x*d[i].x + d[i].y*d[i].y));

		if(	fabs(d[i].x) < MAX_RANGE && fabs(d[i].y) < MAX_RANGE &&
			(d[i].x*cos(phi) + d[i].y*sin(phi)) > 0 &&
			(d[i].x*d[i].x + d[i].y*d[i].y) < MAX_RANGE*MAX_RANGE)
		{
			VISLM temp2;
			temp1.x = lm[i].x; temp1.y = lm[i].y;
			temp2.x = lm[i].x; temp2.y = lm[i].y; temp2.idx = i;
			vlm.push_back(temp2);
			double r, phi;
			ComputeRangeBearing(r, phi, temp1);
			temp2.x = r;
			temp2.y = phi;
			z.push_back(temp2);
		}
	}
}

int CUKF::ComputeRangeBearing(double &r, double &phi, DPOINT lm)
{
	double dx, dy, p;
	dx = lm.x - xtrue->data.db[0];
	dy = lm.y - xtrue->data.db[1];
	p = xtrue->data.db[2];

	r = sqrt(dx*dx + dy*dy);
	phi = atan2(dy, dx) - p;

	return 0;
}

int CUKF::AddObservationNoise()
{
	int len = vlm.size();

	if(len == 0) return -1;
	cvReleaseMat(&Z);
	Z = cvCreateMat(2, len, CV_64FC1);
	for(int i=0; i<len; i++)
	{
		Z->data.db[0*2+i] = z[i].x + sqrt(R->data.db[0]);
		Z->data.db[1*2+i] = z[i].y + sqrt(R->data.db[3]);

// 		Z->data.db[0*2+i] = z[i].x + 0.0001;
//		Z->data.db[1*2+i] = z[i].y + 0.0001;
#ifdef NOISEFREE
		Z->data.db[0*2+i] = z[i].x;
		Z->data.db[1*2+i] = z[i].y;
#endif //NOISEFREE
		z[i].x = Z->data.db[0*2+i];
		z[i].y = Z->data.db[1*2+i];
	}

	return 0;
}

int CUKF::KnownDataAssociation()
{
	zn.clear();
	zf.clear();

	CvSize size = cvGetSize(XX);
	int Nxv = 3;
	int Nf = (size.height - Nxv)/2;
//	TRACE("vlm[i].size : %d\n", vlm.size());

	for(int i=0; i<vlm.size(); i++)
	{
//		TRACE("vlm[i].idx : %d\n", vlm[i].idx);
		if(daTable[vlm[i].idx] == 0)	// new feature
		{
			printf("new vlm[i].idx : %d	x : %.4f y : %.4f lm : %.4f	%.4f\n", vlm[i].idx, vlm[i].x, vlm[i].y, lm[vlm[i].idx], lm[vlm[i].idx], lm[vlm[i].idx]);
			zn.push_back(z[i]);
			daTable[vlm[i].idx] = ++Nf;
		}
		else
		{
			zf.push_back(z[i]);
		}
	}
	
	return 0;
}

int CUKF::Update()
{
//	TRACE("zf.size() : %d\n", zf.size());
	for(int i=0; i<zf.size(); i++)
	{
//		TRACE("zf[%d] : %.4f %.4f %d\n", i, zf[i].x, zf[i].y, zf[i].idx);
//		TRACE("update index : %d\n", daTable[zf[i].idx]);
		UnscentedUpdate(daTable[zf[i].idx], i);
//		PrintCvMat(XX, "XX");
	}
	return 0;
}

int CUKF::UnscentedUpdate(int idf, int idx)
{
	// Set up some variables
	int dim = XX->rows;
	int N = 2*dim+1;
	double scale = 3;
	int kappa = scale-dim;

	// Create samples
	// SVD
	CvSize P = cvGetSize(PX);
	CvMat *D = cvCreateMat(P.height, P.width, CV_64FC1);
	CvMat *U = cvCreateMat(P.height, P.height, CV_64FC1);
	CvMat *V = cvCreateMat(P.width, P.width, CV_64FC1);
	CvMat *Ds = cvCreateMat(P.height, P.width, CV_64FC1);
	CvMat *UD = cvCreateMat(P.height, P.width, CV_64FC1);
	CvMat *Ps = cvCreateMat(P.height, P.width, CV_64FC1);
	cvSVD(PX, D, U, V, CV_SVD_V_T);
	MatSqrt(D, Ds);
	cvMatMul(U, Ds, UD);
	cvScale(UD, Ps, sqrt(scale), 0);

// 	CvMat *ss = cvCreateMat(dim, N, CV_64FC1);
// 	int i,j,jj;
// 	int row = dim;
// 	int col = N;
// 	cvRepeat(XX, ss);
// // 	for (i=0; i<row; i++)	for (j=0; j<col; j++)
// // 		ss->data.db[i*col+j] = XXA->data.db[i];
// 
// 	col -= dim;
// 	for(i=0; i<row; i++)	for(j=1, jj=0; j<col+1; j++, jj++)
// 		ss->data.db[i*col+j] += Ps->data.db[i*col+jj];
// 	for(i=0; i<row; i++)	for(j=1+dim, jj=0; j<col+1+dim; j++, jj++)
//		ss->data.db[i*col+j] -= Ps->data.db[i*col+jj];

	CvMat *ss = cvCreateMat(dim, N, CV_64FC1);
	int i,j,jj;
	int row = dim;
	int col = N;
	cvRepeat(XX, ss);

	double *ssdb = ss->data.db;
	double *Psdb = Ps->data.db;

	for(i=0; i<row; i++)
	{
		for(j=1, jj=0; j<dim+1; j++, jj++)
		{
			ssdb[i*ss->cols+j] += Psdb[i*Ps->cols+jj];
		}
	}

	for(i=0; i<row; i++)
	{
		for(j=1+dim, jj=0; j<ss->cols; j++, jj++)
		{
			ssdb[i*ss->cols+j] -= Psdb[i*Ps->cols+jj];
		}
	}

	// Transform samples according to observation model to obtain the predicted observation samples
	CvMat *zs = cvCreateMat(2, N, CV_64FC1);
//	CvMat *base = cvCreateMat(3, EP, CV_64FC1);
	int Nxv = 3;
	int f = Nxv + idf*2 - 2;

//	PrintCvMat(ss, "ss");
	
	double *zsdb = zs->data.db;

	for(j=0; j<N; j++)
	{
		double dx = ssdb[f*ss->cols+j] - ssdb[0*ss->cols+j];
		double dy = ssdb[(f+1)*ss->cols+j] - ssdb[1*ss->cols+j];
		double d2 = dx*dx + dy*dy;
		double d = sqrt(d2);

		zsdb[0*zs->cols+j] = d;
		zsdb[1*zs->cols+j] = atan2(dy,dx) - ssdb[2*ss->cols+j];
	}

// zz = repvec(z,N);
// dz = feval(dzfunc, zz, zs); % compute correct residual
// zs = zz - dz;               % offset zs from z according to correct residual

//	PrintCvMat(zs, "zs");

	CvMat *zz = cvCreateMat(2, N, CV_64FC1);
	CvMat *dz = cvCreateMat(2, N, CV_64FC1);
	CvMat *zprev = cvCreateMat(2, 1, CV_64FC1);
	zprev->data.db[0] = zf[idx].x;
	zprev->data.db[1] = zf[idx].y;
	cvRepeat(zprev, zz);
	cvSub(zz, zs, dz);
	double *dzdb = dz->data.db;
	for(j=0; j<dz->cols; j++) dzdb[1*dz->cols+j] = PI2PI(dzdb[1*dz->cols+j]);
	cvSub(zz, dz, zs);

//	PrintCvMat(zs, "zs");

	CvMat *zm = cvCreateMat(2, 1, CV_64FC1);
	CvMat *dx = cvCreateMat(dim ,N, CV_64FC1);
//	CvMat *dz = cvCreateMat(2, N, CV_64FC1);
	CvMat *repx = cvCreateMat(dim ,N, CV_64FC1);
	CvMat *repzm = cvCreateMat(2, N, CV_64FC1);
	CvMat *Pxz = cvCreateMat(dim, 2, CV_64FC1);
	CvMat *Pzz = cvCreateMat(2, 2, CV_64FC1);
	CvMat *dxu = cvCreateMat(dim, 1, CV_64FC1);
	CvMat *dxl = cvCreateMat(dim, N-1, CV_64FC1);
	CvMat *dzu = cvCreateMat(2, 1, CV_64FC1);
	CvMat *dzl = cvCreateMat(2, N-1, CV_64FC1);
	CvMat *dztu = cvCreateMat(1, 2, CV_64FC1);
	CvMat *dztl = cvCreateMat(N-1, 2, CV_64FC1);
	CvMat *dxdzu = cvCreateMat(dim, 2, CV_64FC1);
	CvMat *dxdzl = cvCreateMat(dim, 2, CV_64FC1);
	CvMat *dzdzu = cvCreateMat(2, 2, CV_64FC1);
	CvMat *dzdzl = cvCreateMat(2, 2, CV_64FC1);

	double *zmdb = zm->data.db;

	zmdb[0] = kappa*zs->data.db[0*zs->cols];
	zmdb[1] = kappa*zs->data.db[1*zs->cols];

	for(j=1; j<N; j++)
	{
		zm->data.db[0] += 0.5*zsdb[0*zs->cols+j];
		zm->data.db[1] += 0.5*zsdb[1*zs->cols+j];
	}
	cvScale(zm, zm, 1/(double)(scale));

	// Calculate predicted observation mean
	cvRepeat(XX, repx);
	cvRepeat(zm, repzm);

//	PrintCvMat(ss, "ss");
//	PrintCvMat(zs, "zs");
//	PrintCvMat(repx, "repx");
//	PrintCvMat(repzm, "repzm");

	// Calculate observation covariance and the state-observation correlation matrix
	cvSub(ss, repx, dx);
	cvSub(zs, repzm, dz);

//	PrintCvMat(dx, "dx");
//	PrintCvMat(dz, "dz");

	double *dxdb = dx->data.db;
	double *dzudb = dzu->data.db;
	double *dzldb = dzl->data.db;
	double *dxudb = dxu->data.db;
	double *dxldb = dxl->data.db;

	// dx, dz를 1열과 나머지 열로 분할
	for(i=0; i<2; i++)	dzudb[i] = dzdb[i*dz->cols];
	for(i=0; i<2; i++)	for(j=1, jj=0; j<N; j++, jj++)
			dzldb[i*dzl->cols+jj] = dzdb[i*dz->cols+j];

	for(i=0; i<dim; i++)	dxudb[i] = dxdb[i*dx->cols];
	for(i=0; i<dim; i++)	for(j=1, jj=0; j<N; j++, jj++)
			dxldb[i*dxl->cols+jj] = dxdb[i*dx->cols+j];

	cvT(dzu, dztu);
	cvT(dzl, dztl);

//	PrintCvMat(dxu, "dxu");
//	PrintCvMat(dztu, "dztu");
//	PrintCvMat(dxl, "dxl");
//	PrintCvMat(dztl, "dztl");

	cvMatMul(dxu, dztu, dxdzu);
	cvScale(dxdzu, dxdzu, 2*kappa);
	cvMatMul(dxl, dztl, dxdzl);
	cvAdd(dxdzu, dxdzl, Pxz);
	cvScale(Pxz, Pxz, 1/(double)(2*scale));

	cvMatMul(dzu, dztu, dzdzu);
	cvScale(dzdzu, dzdzu, 2*kappa);
	cvMatMul(dzl, dztl, dzdzl);
	cvAdd(dzdzu, dzdzl, Pzz);
	cvScale(Pzz, Pzz, 1/(double)(2*scale));

//	PrintCvMat(dx, "dx");
//	PrintCvMat(dz, "dz");
//	PrintCvMat(dzu, "dzu");
//	PrintCvMat(dztu, "dztu");
//	PrintCvMat(dzl, "dzl");
//	PrintCvMat(dztl, "dztl");
//	PrintCvMat(dxu, "dxu");
//	PrintCvMat(dxl, "dxl");
//	PrintCvMat(dxdzu, "dxdzu");
//	PrintCvMat(dxdzl, "dxdzl");
//	PrintCvMat(dzdzu, "dzdzu");
//	PrintCvMat(dzdzl, "dzdzl");
//	PrintCvMat(Pxz, "Pxz");
//	PrintCvMat(Pzz, "Pzz");

	// Compute Kalman gain
	CvMat *S = cvCreateMat(2, 2, CV_64FC1);
	CvMat *p = cvCreateMat(2, 2, CV_64FC1);
	CvMat *Sct = cvCreateMat(2, 2, CV_64FC1);
	CvMat *Sc = cvCreateMat(2, 2, CV_64FC1);
	CvMat *Sci = cvCreateMat(2, 2, CV_64FC1);
	CvMat *Scit = cvCreateMat(2, 2, CV_64FC1);
	CvMat *Wc = cvCreateMat(dim, 2, CV_64FC1);
	CvMat *W = cvCreateMat(dim, 2, CV_64FC1);
	CvMat *Wz = cvCreateMat(dim, 1, CV_64FC1);
	CvMat *Wct = cvCreateMat(2, dim, CV_64FC1);
	CvMat *WcWc = cvCreateMat(dim, dim, CV_64FC1);

// % Compute Kalman gain
	cvAdd(Pzz, R, S);
	//cholesky decomposition이 실패하면 업데이트는 하지 않는다.
	if(choldc(S, p, Sct) < 0)
	{
		TRACE("idf : %d\n", idf);
		PrintCvMat(UD, "UD");
		PrintCvMat(U, "U");
		PrintCvMat(D, "D");
		PrintCvMat(V, "V");
		PrintCvMat(zprev, "zprev");
		PrintCvMat(XX, "XX");
		PrintCvMat(PX, "PX");
		PrintCvMat(Ps, "Ps");
		PrintCvMat(ss, "ss");
		for(j=0; j<N; j++)
		{
			double dx = ss->data.db[f*ss->cols+j] - ss->data.db[0*ss->cols+j];
			double dy = ss->data.db[(f+1)*ss->cols+j] - ss->data.db[1*ss->cols+j];
			double d2 = dx*dx + dy*dy;
			double d = sqrt(d2);
			double angle = atan2(dy,dx) - ss->data.db[2*ss->cols+j];

			TRACE("%.4f %.4f %.4f %.4f %.4f %.4f %.4f\n", dx, dy, d2, d, angle, atan2(dy, dx), ss->data.db[2*ss->cols+j]);
		}
		PrintCvMat(zs, "zs");
		PrintCvMat(zz, "zz");
		PrintCvMat(zm, "zm");
		PrintCvMat(dx, "dx");
		PrintCvMat(dz, "dz");
		PrintCvMat(Pxz, "Pxz");
		PrintCvMat(Pzz, "Pzz");
		PrintCvMat(R, "R");
		return -1;
	}
	cvT(Sct, Sc);
	cvInv(Sc, Sci);
	cvT(Sci, Scit);

//	PrintCvMat(S, "S");
//	PrintCvMat(Sct, "Sct");
//	PrintCvMat(Sc, "Sc");
//	PrintCvMat(Sci, "Sci");

	cvMatMul(Pxz, Sci, Wc);

//	PrintCvMat(Wc, "Wc");

	cvMatMul(Wc, Scit, W);

//	PrintCvMat(W, "W");
	
	cvSub(zprev, zm, zprev);
	cvMatMul(W, zprev, Wz);
	cvAdd(XX, Wz, XX);
	cvT(Wc, Wct);
	cvMatMul(Wc, Wct, WcWc);
	cvSub(PX, WcWc, PX);
	
// 	PrintCvMat(Pzz, "Pzz");
// 	PrintCvMat(R, "R");
// 	PrintCvMat(S, "S");
// 	PrintCvMat(Pxz, "Pxz");
// 	PrintCvMat(Sc, "Sc");
// 	PrintCvMat(Sct, "Sct");
// 	PrintCvMat(Sci, "Sci");
// 	PrintCvMat(Scit, "Scit");
// 	PrintCvMat(W, "W");	
// 	PrintCvMat(zprev, "zprev");
// 	PrintCvMat(zm, "zm");
// 	PrintCvMat(Wc, "Wc");
// 	PrintCvMat(Wct, "Wct");
// 	PrintCvMat(WcWc, "WcWc");
// 	PrintCvMat(PX, "Px");
// 
// 	PrintCvMat(XX, "XX");
// 	PrintCvMat(PX, "PX");

	cvReleaseMat(&D);
	cvReleaseMat(&U);
	cvReleaseMat(&V);
	cvReleaseMat(&Ds);
	cvReleaseMat(&UD);
	cvReleaseMat(&Ps);
	cvReleaseMat(&ss);
	cvReleaseMat(&zs);
	cvReleaseMat(&zz);
	cvReleaseMat(&dz);
	cvReleaseMat(&zprev);
	cvReleaseMat(&zm);
	cvReleaseMat(&dx);
	cvReleaseMat(&repx);
	cvReleaseMat(&repzm);
	cvReleaseMat(&Pxz);
	cvReleaseMat(&Pzz);
	cvReleaseMat(&dxu);
	cvReleaseMat(&dxl);
	cvReleaseMat(&dzu);
	cvReleaseMat(&dzl);
	cvReleaseMat(&dztu);
	cvReleaseMat(&dztl);
	cvReleaseMat(&dxdzu);
	cvReleaseMat(&dxdzl);
	cvReleaseMat(&dzdzu);
	cvReleaseMat(&dzdzl);
	cvReleaseMat(&S);
	cvReleaseMat(&p);
	cvReleaseMat(&Sct);
	cvReleaseMat(&Sc);
	cvReleaseMat(&Sci);
	cvReleaseMat(&Scit);
	cvReleaseMat(&Wc);
	cvReleaseMat(&W);
	cvReleaseMat(&Wz);
	cvReleaseMat(&Wct);
	cvReleaseMat(&WcWc);

	return 0;
}

int CUKF::Augment()
{
	int i, /*ii,*/ j, k, l;

	int newzno = zn.size();
	if(newzno == 0) return -1;

	for(l=1; l<newzno+1; l++)
	{
		printf("zn index : %d\n", zn[l-1].idx);
		int rows = XX->rows;
		int cols = XX->cols;
		int dim = XX->rows;
		CvMat *temp1 = cvCloneMat(XX);
		cvReleaseMat(&XX);
		XX = cvCreateMat(dim+2, 1, CV_64FC1);
		for(i=0; i<rows; i++)	XX->data.db[i] = temp1->data.db[i];
// 		for(i=rows, ii=l-1; i<XX->rows; i+=2, ii++)
// 		{
			XX->data.db[rows] = zn[l-1].x;
			XX->data.db[rows+1] = zn[l-1].y;
//		}

		rows = PX->rows;
		cols = PX->cols;
		CvMat *temp2= cvCloneMat(PX);
		cvReleaseMat(&PX);
		PX = cvCreateMat(rows+2, cols+2, CV_64FC1);

		cvZero(PX);
		for(i=0; i<rows; i++)	for(j=0; j<cols; j++)
			PX->data.db[i*PX->cols+j] = temp2->data.db[i*cols+j];

		for(i=rows, k=0; i<PX->rows; i++, k++)	for(j=cols; j<PX->cols; j++)
		{
			if(i==j)
			{
				if(k%2==0)	PX->data.db[i*PX->cols+j] = R->data.db[0];
				else		PX->data.db[i*PX->cols+j] = R->data.db[3];
			}
		}

		cvReleaseMat(&temp1);
		cvReleaseMat(&temp2);

//  	 	PrintCvMat(XX, "XX");
//  		PrintCvMat(PX, "PX");

		UnscentedTransform(AUGMENT_MODEL);
	}

	return 0;
}

int CUKF::Doit()
{
//	VehicleModel();
	//StopWatch("start");
	AddControlNoise();
	//StopWatch("AddControlNoise");
 	Prediction();
	//StopWatch("Prediction");
// 	PrintCvMat(XX, "after prediction : XX");
// 	PrintCvMat(PX, "after prediction : PX");

 	static double dtsum=0;
	dtsum += DT_CONTROLS;
 	if(dtsum >= DT_OBSERVE)
 	{
//		TRACE("xtrue : %.4f %.4f %.4f\n", xtrue->data.db[0], xtrue->data.db[1], xtrue->data.db[2]);
 		dtsum = 0;
 		GetVisibleLandmark();
		//StopWatch("GetVisibleLandmark");
 		AddObservationNoise();
		//StopWatch("AddObservationNoise");
 		KnownDataAssociation();
		//StopWatch("KnownDataAssociation");

// 		PrintCvMat(XX, "before update : XX");
// 		PrintCvMat(PX, "before update : PX");

 		Update();
		//StopWatch("Update");

// 		PrintCvMat(XX, "after update : XX");
// 		PrintCvMat(PX, "after update : PX");

		Augment();
		//StopWatch("Augment");

 		//PrintCvMat(XX, "after augment : XX");
 		//PrintCvMat(PX, "after augment : PX");
 		//TRACE("XX dim : %d %d\n", XX->rows, XX->cols);
 		//TRACE("PX dim : %d %d\n", PX->rows, PX->cols);
// 		TRACE("==== datable ====\n");
// 		for(int i=0; i<lm.size(); i++)
// 			TRACE("%d\t", daTable[i]);
// 		TRACE("=================\n");
	}

	EllipseFitting();
	//StopWatch("EllipseFitting");
	
	return 0;
}

double CUKF::ComputeSteering(double x, double y)
{
//  compute change in G to point towards current waypoint
// 	double angle = AbsoluteAngleBetweenTwoPoints(xtrue->data.db[0], xtrue->data.db[1], x, y);
// 	double deltaG = DEG2RAD(AbsoluteAngleDiff(RAD2DEG(xtrue->data.db[2]), angle));

	double deltaG = PI2PI(atan2(y-xtrue->data.db[1], x-xtrue->data.db[0]) - xtrue->data.db[2] - G);

	double maxDelta = RATEG*DT_CONTROLS;
	if(fabs(deltaG) > maxDelta)
	{
		if(deltaG < 0) deltaG = -maxDelta;
		else if (deltaG > 0) deltaG = maxDelta;
		else deltaG = 0;	// maxDelta?
	}

	G = G+deltaG;
	if(fabs(G) > MAXG)
	{
		if(G < 0) G = -MAXG;
		else if (G > 0) G = MAXG;
		else G = 0;	// maxDelta?		
	}

	return G;
}

int CUKF::EllipseFitting()
{
	int i;
	double n = EP-1;
	double inc = 2*PI/n;

	CvMat *p2by2 = cvCreateMat(2, 2, CV_64FC1);
//	CvMat *phi = cvCreateMat(1, EP, CV_64FC1);
	CvMat *circ = cvCreateMat(2, EP, CV_64FC1);
	CvMat *p = cvCreateMat(2, EP, CV_64FC1);
	CvMat *r = cvCreateMat(2, 2, CV_64FC1);
	CvMat *a = cvCreateMat(2, EP, CV_64FC1);

	cvRepeat(PX, p2by2);
	//PrintCvMat(p2by2, "p2by2");

	for(i=0; i<circ->cols; i++)
	{
		circ->data.db[i] = 2*cos(inc*i);
		circ->data.db[circ->cols+i] = 2*sin(inc*i);
	}
	sqrtm_2by2(p2by2, r);
	cvMatMul(r, circ, a);
	for(i=0; i<p->cols; i++)
	{
		EllipseV->data.db[p->cols+i] = a->data.db[a->cols+i]*DRAW_SCALE + XX->data.db[1];
		EllipseV->data.db[i] = a->data.db[i]*DRAW_SCALE + XX->data.db[0];
	}

	//PrintCvMat(XX, "XX");
	//PrintCvMat(PX, "PX");
	////PrintCvMat(p2by2, "p2by2");
	//PrintCvMat(circ, "circ");
	//PrintCvMat(r, "r");
	//PrintCvMat(a, "a");
 //	PrintCvMat(EllipseV, "EllipseV");

	//<--------------vehicle ellipse fitting

	int nf = (XX->rows-3)/2;	// number of features
	for(int f=0; f<nf; f++)
	{
		p2by2->data.db[0] = PX->data.db[(3+f*2)*PX->cols+(3+f*2)];
		p2by2->data.db[1] = PX->data.db[(3+f*2)*PX->cols+(3+f*2)+1];
		p2by2->data.db[2] = PX->data.db[(3+f*2+1)*PX->cols+(3+f*2)];
		p2by2->data.db[3] = PX->data.db[(3+f*2+1)*PX->cols+(3+f*2)+1];

// 		PrintCvMat(PX, "PX");
// 		PrintCvMat(p2by2, "p2by2");

		sqrtm_2by2(p2by2, r);
		cvMatMul(r, circ, a);
		for(i=0; i<p->cols; i++)
		{
			EllipseF[f]->data.db[p->cols+i] = a->data.db[a->cols+i]*DRAW_SCALE + XX->data.db[(f*2)+4];
			EllipseF[f]->data.db[i] = a->data.db[i]*DRAW_SCALE + XX->data.db[(f*2)+3];
		}

// 		PrintCvMat(p2by2, "p2by2");
// 		PrintCvMat(circ, "circ");
// 		PrintCvMat(r, "r");
// 		PrintCvMat(a, "a");
		//PrintCvMat(EllipseF[f], "EllipseF");
	}

	//<--------------feature ellipse fitting

	cvReleaseMat(&p2by2);
	cvReleaseMat(&circ );
	cvReleaseMat(&p);
	cvReleaseMat(&r);
	cvReleaseMat(&a);

	return 0;
}

int CUKF::SetNoiseParameter()
{
	//// control noises
 //	sigmaV= 0.3;			// m/s
 //	sigmaG= (3.0*PI/180);	// radians
	//Q = cvCreateMat(2, 2, CV_64FC1);
	//Q->data.db[0] = sigmaV*sigmaV;
	//Q->data.db[1] = 0;
	//Q->data.db[2] = 0;
	//Q->data.db[3] = sigmaG*sigmaG;
	//
	//// observation noises
 //	sigmaR= 0.1;			// meters
 //	sigmaB= (1.0*PI/180);	// radians
	//R = cvCreateMat(2, 2, CV_64FC1);
	//R->data.db[0] = sigmaR*sigmaR;
	//R->data.db[1] = 0;
	//R->data.db[2] = 0;
	//R->data.db[3] = sigmaB*sigmaB;

	//sigmaV= 0.3;			// m/s
 	//sigmaG= (3.0*PI/180);	// radians
	Q = cvCreateMat(2, 2, CV_64FC1);
	Q->data.db[0] = sigmaV*sigmaV;
	Q->data.db[1] = 0;
	Q->data.db[2] = 0;
	Q->data.db[3] = DEG2RAD(sigmaG)*DEG2RAD(sigmaG);
	
	// observation noises
 	//sigmaR= 0.1;			// meters
 	//sigmaB= (1.0*PI/180);	// radians
	R = cvCreateMat(2, 2, CV_64FC1);
	R->data.db[0] = sigmaR*sigmaR;
	R->data.db[1] = 0;
	R->data.db[2] = 0;
	R->data.db[3] = DEG2RAD(sigmaB)*DEG2RAD(sigmaB);

	return 0;
}
