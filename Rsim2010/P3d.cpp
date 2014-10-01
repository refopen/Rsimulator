// P3d.cpp : implementation file
//

#include "stdafx.h"
#include "drawcli.h"
#include "P3d.h"
#include "Robot.h"

/////////////////////////////////////////////////////////////////////////////
// P3d

P3d::P3d()
{
}

P3d::~P3d()
{
}

int P3d::LoadPoints()
{
	int idx=0;
	char str[100];
	FILE *fd;
	if(!(fopen_s(&fd, "ThreedPoints.data", "r")))
	{
		//AfxMessageBox("ThreedPoints.data open error");
		return -1;
	}
	while(fgets(str, 100, fd))
	{
		if(str[0] == '#') continue;
		if(sscanf_s(str, "%lf %lf %lf", &x[idx], &y[idx], &z[idx]) < 0) break;
		idx++;
	}
	fclose(fd);

	no = idx;

	return 0;
}

int P3d::ShowPoints()
{
	int i;
	IplImage *threed;
	threed = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	memset(threed->imageData, 255, sizeof(char)*WIDTH*HEIGHT);

	for (i=0; i<no; i++)
	{
		cvCircle(threed, 
			cvPoint(
			(int)(WIDTH/2+(double)x[i]/25.0), 
			(int)(HEIGHT/2-(double)z[i]/25.0)),
			1,
			CV_RGB(128, 128, 128), 
			CV_FILLED
			);
	}

	cvNamedWindow("threed");
	cvShowImage("threed", threed);
	cvReleaseImage(&threed);
	return 0;
}
