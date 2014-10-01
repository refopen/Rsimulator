// Elapsed.h: interface for the CElapsed class.
//
// CElapsed class wraps up windows queryperformancecounter functionalities
// Provides microseconds timing accuracy.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DEF_ELAPSED)
#define DEF_ELAPSED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CElapsed
{
public:
	void Display(char *str);
	CElapsed();
	virtual ~CElapsed();

	BOOL Begin();    // start timing
	double End();
	BOOL Available();
	__int64 GetFreq();

	double m_dElapsed;
private :
    int m_iInitialized;
    __int64 m_iFrequency;
    __int64 m_iBeginTime;

};

#endif
