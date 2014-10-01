#ifndef __UTIL_H__
#define __UTIL_H__

#define PI	3.141592

int choldc(CvMat *mata, CvMat *matp, CvMat *matchol);
void PrintCvMat(CvMat *cvArr, CString title="");
double AbsoluteAngleBetweenTwoPoints(double x1, double y1, double x2, double y2);
double AbsoluteAngleDiff(double startangle, double targetangle);
double NormalizeAngle(double angle);
double PI2PI(double angle);
double NormalizeAngleInRadian(double angle);
void sqrtm_2by2(CvMat *A, CvMat *X);
double StopWatch(char *str);
void PRINT(const char *fmt, ...);

//#define TRACE(fmt, x...)				{ \
//											time_t the_time; \
//											struct tm *tm_ptr; \
//											char timestr[100]; \
//											log_fd = fopen(log_filename1, "a+"); \
//											if(log_fd != 0) \
//											{ \
//												time(&the_time); \
//												tm_ptr = localtime(&the_time); \
//												sprintf(timestr, "%04d-%02d-%02d %02d:%02d:%02d", \
//																tm_ptr->tm_year+1900, \
//																tm_ptr->tm_mon+1, \
//																tm_ptr->tm_mday, \
//																tm_ptr->tm_hour, \
//																tm_ptr->tm_min, \
//																tm_ptr->tm_sec); \
//												fprintf(log_fd, "[%s] ", timestr); \
//												fprintf(log_fd, fmt, ## x); \
//												fflush(log_fd); \
//												fclose(log_fd); \
//												TRACE("[%s] ", timestr); \
//												TRACE(fmt, ## x); \
//											} \
//											else TRACE("파일 생성 실패\n");\
//										}
#endif
