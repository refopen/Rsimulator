// Elapsed.cpp: implementation of the CElapsed class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Elapsed.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElapsed::CElapsed()
{
	// get the frequency of the counter
	m_iInitialized = QueryPerformanceFrequency( (LARGE_INTEGER *)&m_iFrequency );
}

CElapsed::~CElapsed()
{

}

BOOL CElapsed::Begin()    // start timing
{
	if (!m_iInitialized)
		return 0;   // error - couldn't get frequency

	// get the starting counter value
	return QueryPerformanceCounter( (LARGE_INTEGER *)&m_iBeginTime );
}

double CElapsed::End()    // stop timing and get elapsed time in seconds
{
	if (!m_iInitialized )
		return 0.0; // error - couldn't get frequency

	// get the ending counter value
	__int64 endtime;
	QueryPerformanceCounter( (LARGE_INTEGER *)&endtime );

	// determine the elapsed counts
	__int64 elapsed = endtime - m_iBeginTime;

	// convert counts to time in seconds and return it
	m_dElapsed = (double)elapsed / (double)m_iFrequency;
	return m_dElapsed;
}

BOOL CElapsed::Available()  // returns true if the perf counter is available
{ 
	return m_iInitialized; 
}

__int64 CElapsed::GetFreq() // return perf counter frequency as large int
{
	return m_iFrequency; 
}

void CElapsed::Display(char *str)
{
	CString szTime;
	szTime.Format("Time = %.3fms", m_dElapsed*1000);
	printf("%s Time = %.3fms\n", str, m_dElapsed*1000);
	//AfxMessageBox(szTime);
}
