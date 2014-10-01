#include "stdafx.h"
#include "util.h"

void sqrtm_2by2(CvMat *A, CvMat *X)
{
	CvMat *e = cvCreateMat(2, 1, CV_64FC1);
	CvMat *ev = cvCreateMat(2, 2, CV_64FC1);
	CvMat *eye = cvCreateMat(2, 2, CV_64FC1);
	CvMat *P = cvCreateMat(2, 2, CV_64FC1);

	cvZero(eye);
	eye->data.db[0] = 1; eye->data.db[3] = 1;

	P = cvCloneMat(A);
	cvEigenVV(P, ev, e);

	double m, p;
	double e1, e2;
	e1 = e->data.db[0]; 
	e2 = e->data.db[1];

	//printf("[sqrtm_2by2]eigen values : %.3f %.3f\n", e1, e2);

	if(e1 != e2)
	{
		m = (sqrt(e2) - sqrt(e1)) / (e2-e1);
		p = (e2*sqrt(e1) - e1*sqrt(e2)) / (e2-e1);

		//printf("[sqrtm_2by2]m : %.3f p : %.3f\n", m, p);
	}
	else if(e1 == e2 && e1 != 0)
	{
		m = 1/(4*e1);
		p = sqrt(e1)/2;
	}
	else
	{
		AfxMessageBox("sqrtm_2by2 error");
	}

	//PrintCvMat(A, "A");

	cvScale(A, A, m);
	cvScale(eye, eye, p);
	cvAdd(A, eye, X);

	//PrintCvMat(A, "Am");
	//PrintCvMat(eye, "eyep");
	//PrintCvMat(X, "X");

	cvReleaseMat(&e);
	cvReleaseMat(&ev);
	cvReleaseMat(&eye);
	cvReleaseMat(&P);
}

int choldc(CvMat *mata, CvMat *matp, CvMat *matchol)
{
	int i,j,k;
	double sum;
	double *a, *p, *chol;
	a = mata->data.db;
	p = matp->data.db;
	chol = matchol->data.db;

	int n = mata->rows;
	for (i=0;i<n;i++){
		for (j=i;j<n;j++){
			for (sum=a[i*n+j],k=i-1;k>=0;k--) sum -= a[i*n+k]*a[j*n+k];
			if (i == j) {
				if (sum <= 0.0)
				{
					TRACE("choldc failed\n");
					PrintCvMat(mata, "mata");
					return -1;
				}
				p[i]=sqrt(sum);
			} else a[j*n+i]=sum/p[i];
		}
	}

    for (i=0;i<n;i++){
      for (j=0;j<n;j++){
        if (i == j) 
			chol[i*n+i]=p[i];
        else 
			chol[i*n+j]=(i > j ? a[i*n+j] : 0.0);
      }
    }

	return 0;
}

//////////////////////////////////////
// Print CvMat when Debug Mode
//////////////////////////////////////
void PrintCvMat(CvMat *cvArr, CString title)
{
// 	int i,j;
// 	TRACE("name: %s\n", title);
// 	for (i=0; i<cvArr->rows; i++)
// 	{
// 		for (j=0; j<cvArr->cols; j++)
// 		{
// 			TRACE("%.4f\t", cvArr->data.db[cvArr->cols*i+j]);
// 		}
// 		TRACE("\n");
// 	}

	int i,j;
	printf("name: %s\n", title);
	for (i=0; i<cvArr->rows; i++)
	{
		for (j=0; j<cvArr->cols; j++)
		{
			printf("%.4f ", cvArr->data.db[cvArr->cols*i+j]);
		}
		printf("\n");
	}
}

//-----------------------------------------------------------------------------
// Name: AbsoluteAngleBetweenTwoPoints
// Args:
// Rtn :
// Desc: 두 점이 만드는 직선이 x축과 몇도의 각을 가지는가를 계산
//-----------------------------------------------------------------------------
double AbsoluteAngleBetweenTwoPoints(double x1, double y1, double x2, double y2)
{
	double angle;
	
	if(x1 == x2)
	{
		if(y1 > y2)	angle = 270.0;
		else if(y1 < y2) angle = 90.0;
		else angle = 0;
	}
	else angle = atan(fabs((y2-y1)/(x2-x1))) * 180.0 / PI;
		
	if((x2>x1) && (y2>=y1)) ;
	else if((x2>x1) && (y2<y1)) angle = 360.0 - angle;
	else if((x2<x1) && (y2>=y1)) angle = 180.0 - angle;
	else if((x2<x1) && (y2<y1)) angle = 180.0 + angle;	
	
	return angle;
}

//-----------------------------------------------------------------------------
// Name: AbsoluteAngleDiff
// Args:
// Rtn :
// Desc: 주어진 두 절대각도의 각도차를 계산한다. 
//       첫번째 인자를 기준으로 두번째 인자의 각도차를 계산한다.
//-----------------------------------------------------------------------------
double AbsoluteAngleDiff(double startangle, double targetangle)
{	
	double temp, temp1=0;
	double diffangle=0;
	
	startangle = NormalizeAngle(startangle);
	targetangle = NormalizeAngle(targetangle);
	
	temp = fabs(startangle - targetangle);
	if(temp > 180.0) diffangle = 360.0 - temp;
	else if(temp <= 180.0) diffangle = temp;
		
	temp = startangle - diffangle;
	if(temp < 0) temp = temp + 360.0;
			
	temp = fabs(temp - targetangle);
	if(temp > 180.0) temp1 = 360.0 - temp;
	else if(temp <= 180.0) temp1 = temp;
	
	if(temp1 < 0.1) diffangle *= -1.0;
		
	return diffangle;	
}

double NormalizeAngle(double angle)
{
	while(angle < 0)
		angle += 360;
	while(angle >= 360)
		angle -= 360;

	return angle;		

// 	double retangle, temp1, temp2;
// 
// 	temp1 = angle/360.0;
// 	temp2 = (double)((int)temp1);
// 	retangle = (temp1 - temp2) * 360.0;
// 	
// 	if(retangle < 0.0) retangle += 360.0;
// 	if(retangle >= 360.0) retangle = retangle - 360.0;		
// 
//	return retangle;	
}

// twopi = 2*pi;
// angle = angle - twopi*fix(angle/twopi); % this is a stripped-down version of rem(angle, 2*pi)
// 
// i = find(angle > pi);
// angle(i) = angle(i) - twopi;
// 
// i = find(angle < -pi);
// angle(i) = angle(i) + twopi;

double PI2PI(double angle)
{
	while(angle < -PI)
		angle += 2*PI;
	while(angle > PI)
		angle -= 2*PI;

	return angle;	
}

double NormalizeAngleInRadian(double angle)
{
	while(angle < 0)
		angle += 2*PI;
	while(angle >= 2*PI)
		angle -= 2*PI;

	return angle;	
}

// incremental stop watch
double StopWatch(char *str)
{
	static int64 tStart;
	int64 ego = cvGetTickCount() - tStart;
	tStart = cvGetTickCount();

	TRACE("%s : %5.3f ms\n", str, 0.001*ego / cvGetTickFrequency() );

	return 0.0;
}

//#define LOG

#ifdef LOG

void PRINT(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	TRACE(fmt, args);
	va_end(args);
}

#else

void PRINT(const char *fmt, ...)
{
	return;
}
#endif