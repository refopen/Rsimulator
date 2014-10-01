//
//		CGDIRect
//
//
//
//		Written by Jason Hattingh
//
//		http://www.greystonefx.com
//
//		jhattingh@greystonefx.com
//
//
//
//
//		Use freely...
//
//
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDIRECT_H__2AC1B0EE_504D_4BF8_B567_0766B3555F88__INCLUDED_)
#define AFX_GDIRECT_H__2AC1B0EE_504D_4BF8_B567_0766B3555F88__INCLUDED_

#include <gdiplus.h>
using namespace Gdiplus;

#pragma message("     _Adding library: gdiplus.lib" ) 
#pragma comment(lib, "gdiplus.lib")

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGDIRect
{
public:
	//
	// Construction / Destruction:
	//

	CGDIRect( int nValue = 0 )				{	Assimilate( nValue );	};

	CGDIRect( REAL fValue )					{	Assimilate( fValue );	};

	CGDIRect( Rect rcInit )					{	Assimilate( rcInit );	};

	CGDIRect( RectF rcInit )				{	Assimilate( rcInit );	};

	CGDIRect( CRect rcInit )				{	Assimilate( rcInit );	};

	CGDIRect( CPoint point, CSize size )	{	Assimilate( point.x, point.y, point.x + size.cx, point.y + size.cy );			};

	CGDIRect( Point point, Size size )		{	Assimilate( point.X, point.Y, point.X + size.Width, point.Y + size.Height );	};

	CGDIRect( PointF point, SizeF size )	{	Assimilate( point.X, point.Y, point.X + size.Width, point.Y + size.Height );	};

	CGDIRect( int nLeft, int nTop, int nRight, int nBottom )	
	{	
		Assimilate( nLeft, nTop, nRight, nBottom );
	};

	CGDIRect( REAL Left, REAL Top, REAL Right, REAL Bottom )	
	{	
		Assimilate( Left, Top, Right, Bottom );
	};

	virtual ~CGDIRect()			{							};


	//
	// Operators:
	//

	/////////////////////////////////////////////////////////////////////////////////////////// Function Header
	CGDIRect& operator=( CGDIRect& rhs )
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		return Assimilate( rhs.left, rhs.top, rhs.right, rhs.bottom );
	};

	/////////////////////////////////////////////////////////////////////////////////////////// Function Header
	CGDIRect& operator|=( CGDIRect& rhs )
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	Join with rhs in holy matrimony ( Union )
	//
	{
		Rect u;

		if ( u.Union( u, *this, rhs ) )
			return Assimilate( u );
		else
			return Assimilate( 0 );
	};

	/////////////////////////////////////////////////////////////////////////////////////////// Function Header
	CGDIRect& operator&=( CGDIRect& rhs )
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	Until intersections do us part...
	//
	{
		Rect i;

		if ( i.Intersect( i, *this, rhs ) )
			return Assimilate( i );
		else
			return Assimilate( 0 );
	};

	/////////////////////////////////////////////////////////////////////////////////////////// Function Header
	CGDIRect& operator=( int nValue )
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		return Assimilate( nValue );
	};

	/////////////////////////////////////////////////////////////////////////////////////////// Function Header
	CGDIRect& operator=( REAL Value )
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		return Assimilate( Value );
	};

	//
	//		A d d i t i o n   O v e r l o a d s
	//

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator+( CRect& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	top		+ (REAL)rhs.top, 
								left	+ (REAL)rhs.left,
								right	+ (REAL)rhs.right,
								bottom	+ (REAL)rhs.bottom );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator+( Rect& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	top		+ (REAL)rhs.GetTop(),
								left	+ (REAL)rhs.GetLeft(),
								right	+ (REAL)rhs.GetRight(),
								bottom	+ (REAL)rhs.GetBottom() );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator+( RectF& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	top		+ rhs.GetTop(),
								left	+ rhs.GetLeft(),
								right	+ rhs.GetRight(),
								bottom	+ rhs.GetBottom() );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator+( CGDIRect& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	top		+ rhs.top, 
								left	+ rhs.left,
								right	+ rhs.right,
								bottom	+ rhs.bottom );
		};

		CGDIRect& operator+=( CRect& rhs )		{ return Assimilate( CGDIRect(*this) + CGDIRect(rhs) );			};

		CGDIRect& operator+=( Rect& rhs )		{ return Assimilate( CGDIRect(*this) + CGDIRect(rhs) );			};
		
		CGDIRect& operator+=( RectF& rhs )		{ return Assimilate( CGDIRect(*this) + CGDIRect(rhs) );			};
		
		CGDIRect& operator+=( CGDIRect& rhs )	{ return Assimilate( CGDIRect(*this) + CGDIRect(rhs) );			};

		CGDIRect& operator+=( int nValue )		{ return Assimilate( CGDIRect(*this) + CGDIRect(nValue) );		};

		CGDIRect& operator+=( REAL fValue )		{ return Assimilate( CGDIRect(*this) + CGDIRect(fValue) );		};

	//
	//		S u b t r a c t i o n   O v e r l o a d s
	//
	
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator-( CRect& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	top		- (REAL)rhs.top, 
								left	- (REAL)rhs.left,
								right	- (REAL)rhs.right,
								bottom	- (REAL)rhs.bottom );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator-( Rect& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	top		- (REAL)rhs.GetTop(),
								left	- (REAL)rhs.GetLeft(),
								right	- (REAL)rhs.GetRight(),
								bottom	- (REAL)rhs.GetBottom() );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator-( RectF& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	top		- rhs.GetTop(),
								left	- rhs.GetLeft(),
								right	- rhs.GetRight(),
								bottom	- rhs.GetBottom() );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator-( CGDIRect& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	top		- rhs.top, 
								left	- rhs.left,
								right	- rhs.right,
								bottom	- rhs.bottom );
		};

		CGDIRect& operator-=( CRect& rhs )		{ return Assimilate( CGDIRect(*this) - CGDIRect(rhs) );			};

		CGDIRect& operator-=( Rect& rhs )		{ return Assimilate( CGDIRect(*this) - CGDIRect(rhs) );			};
		
		CGDIRect& operator-=( RectF& rhs )		{ return Assimilate( CGDIRect(*this) - CGDIRect(rhs) );			};
		
		CGDIRect& operator-=( CGDIRect& rhs )	{ return Assimilate( CGDIRect(*this) - CGDIRect(rhs) );			};

		CGDIRect& operator-=( int nValue )		{ return Assimilate( CGDIRect(*this) - CGDIRect(nValue) );		};

		CGDIRect& operator-=( REAL fValue )		{ return Assimilate( CGDIRect(*this) - CGDIRect(fValue) );		};

	//
	//	C o m p a r i s o n   O v e r l o a d s :
	//

		// Is Equal To?
		bool operator==( REAL Value )		{ return top == Value  && bottom == Value  && left == Value  && right == Value;				};
		bool operator==( int nValue )		{ return top == nValue && bottom == nValue && left == nValue && right == nValue;			};
		bool operator==( CGDIRect& rhs )	{ return top == rhs.top && bottom == rhs.bottom && left == rhs.left && right == rhs.right;	};
		bool operator==( CRect& rhs )		{ return CGDIRect(rhs) == *this;	};
		bool operator==( Rect&  rhs )		{ return CGDIRect(rhs) == *this;	};
		bool operator==( RectF& rhs )		{ return CGDIRect(rhs) == *this;	};

		// Is Different To?

		bool operator!=( REAL Value )		{ return top != Value && bottom != Value && left != Value && right != Value;				};
		bool operator!=( int nValue )		{ return top != nValue && bottom != nValue && left != nValue && right != nValue;			};
		bool operator!=( CGDIRect& rhs )	{ return top != rhs.top || bottom != rhs.bottom	|| left	!= rhs.left || right != rhs.right;	};
		bool operator!=( CRect& rhs )		{ return CGDIRect(rhs) != *this;	};
		bool operator!=( Rect&  rhs )		{ return CGDIRect(rhs) != *this;	};
		bool operator!=( RectF& rhs )		{ return CGDIRect(rhs) != *this;	};

	//
	//	S i z e   a n d   P o s i t i o n  :
	//

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void ReplicateBelow( CGDIRect rcSource, REAL nOffset = 0)
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			top		= rcSource.bottom + nOffset;

			left	= rcSource.left;

			SetSize( rcSource );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void ReplicateAbove( CGDIRect rcSource, REAL Offset = 0)
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			top		= rcSource.top - Height() - Offset;

			left	= rcSource.left;

			SetSize( rcSource );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void ReplicateLeft( CGDIRect rcSource, REAL Offset = 0)
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			top		= rcSource.top;

			left	= rcSource.left - rcSource.Width() - Offset;

			SetSize( rcSource );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void ReplicateRight( CGDIRect rcSource, REAL Offset = 0)
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			top		= rcSource.top;

			left	= rcSource.right + rcSource.Width() + Offset;

			SetSize( rcSource );
		};

		void ReplicateBelow( CGDIRect rcSource, int nOffset = 0)	{ ReplicateBelow( rcSource, (REAL)nOffset);	};

		void ReplicateAbove( CGDIRect rcSource, int nOffset = 0)	{ ReplicateAbove( rcSource, (REAL)nOffset);	};

		void ReplicateLeft( CGDIRect rcSource, int nOffset = 0)		{ ReplicateLeft( rcSource, (REAL)nOffset);	};

		void ReplicateRight( CGDIRect rcSource, int nOffset = 0)	{ ReplicateRight( rcSource, (REAL)nOffset);	};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetSize( CGDIRect rcSource )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			SetWidth(	rcSource.Width() );
			SetHeight(	rcSource.Height() );
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetSize( CSize size )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			SetWidth(	size.cx );
			SetHeight(	size.cy );
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetSize( Size size )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			SetWidth(	size.Width );
			SetHeight(	size.Height );
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetSize( SizeF size )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			SetWidth(	size.Width );
			SetHeight(	size.Height );
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetWidth( REAL nValue, bool bMaintainRight=false )			
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			if (bMaintainRight)
				left	= right - nValue;
			else
				right	= left + nValue;
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetHeight( REAL Value, bool bMaintainBottom=false )			
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			if (bMaintainBottom)
				top		= bottom - Value;
			else
				bottom	= top + Value;
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetWidth( int nValue, bool bMaintainRight=false )			
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			SetWidth( (REAL)nValue, bMaintainRight );
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetHeight( int nValue, bool bMaintainBottom=false )			
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			SetHeight( (REAL)nValue, bMaintainBottom );
		};

		REAL		Width()		{ return right - left;			};
		REAL		Height()	{ return bottom - top;			};

		int			WidthInt()	{ return (int)(right - left);	};
		int			HeightInt()	{ return (int)(bottom - top);	};

		operator	CRect()		{ return CRect( (int)left, (int)top, (int)right, (int)bottom );				};
		operator	RectF()		{ return RectF( left, top, Width(), Height() );								};
		operator	Rect()		{ return Rect(  (int)left, (int)top, (int)Width(), (int)Height() );			};

		operator	CPoint()	{ return CPoint((int)left, (int)top );										};
		operator	Point()		{ return Point((int)left, (int)top );										};
		operator	PointF()	{ return PointF((REAL)left, (REAL)top );									};

		operator	CSize()		{ return CSize( WidthInt(),	HeightInt() );									};
		operator	Size()		{ return Size(	WidthInt(),	HeightInt() );									};
		operator	SizeF()		{ return SizeF(	Width(),	Height() );										};

		//		Inflate and Deflate Methods:

		void	InflateWidth(	 REAL x )		{ left -= x;			right += x;							};
		void	InflateWidthInt( int nX )		{ left -= (REAL)nX;		right += (REAL)nX;					};

		void	InflateHeight( REAL y )			{ top -= y;				bottom += y;						};
		void	InflateHeightInt( int nY )		{ top -= nY;			bottom += (REAL)nY;					};

		void	Inflate( CSize Size )			{ InflateWidthInt( Size.cx );	  InflateHeightInt( Size.cy );	    };
		void	Inflate( Size  Size )			{ InflateWidthInt( Size.Width ); InflateHeightInt( Size.Height );	};
		void	Inflate( SizeF Size )			{ InflateWidth( Size.Width ); InflateHeight( Size.Height );	};
		void	Inflate( REAL X, REAL Y)		{ InflateWidth( X );		InflateHeight( Y );				};
		void	Inflate( REAL Val)				{ InflateWidth( Val );		InflateHeight( Val );			};
		void	InflateInt( int nX, int nY)		{ InflateWidthInt( nX );	InflateHeightInt( nY );			};
		void	InflateInt( int nVal)			{ InflateWidthInt( nVal );	InflateHeightInt( nVal );		};

		void	Deflate( CSize Size )			{ DeflateWidthInt( Size.cx ); InflateHeightInt( Size.cy );	};
		void	Deflate( Size  Size )			{ DeflateWidthInt( Size.Width ); InflateHeightInt( Size.Height );	};
		void	Deflate( SizeF Size )			{ DeflateWidth( Size.Width ); InflateHeight( Size.Height );	};
		void	Deflate( REAL X, REAL Y )		{ DeflateWidth( X );		DeflateHeight( Y );				};
		void	Deflate( REAL Val)				{ DeflateWidth( Val );		DeflateHeight( Val );			};
		void	DeflateInt( int nX, int nY )	{ DeflateWidthInt( nX );	DeflateHeightInt(nY);			};
		void	DeflateInt( int nVal)			{ DeflateWidthInt( nVal );	DeflateHeightInt(nVal);			};

		void	DeflateWidth(  REAL X )			{ left += X;			right -= X;							};
		void	DeflateWidthInt(  int nX )		{ left += (REAL)nX;		right -= (REAL)nX;					};

		void	DeflateHeight( REAL Y )			{ top += Y;				bottom -= Y;						};
		void	DeflateHeightInt( int nY )		{ top += (REAL)nY;		bottom -= (REAL)nY;					};

		//		Offset, Extend and Collapse

		void	Offset(		REAL nX, REAL nY )	{ Assimilate( top+nY, bottom+nY, left+nX, right+nX);							};
		void	OffsetInt(	int nX, int nY )	{ Assimilate( top+(REAL)nY, bottom+(REAL)nY, left+(REAL)nX, right+(REAL)nX);	};

		void	Extend(		REAL nX, REAL nY )	{ Assimilate( top, bottom+nY, left, right+nX);									};
		void	ExtendInt( int nX, int nY )		{ Assimilate( top, bottom+(REAL)nY, left, right+(REAL)nX);						};

		void	Collapse(	 REAL nX, REAL nY )	{ Assimilate( top, bottom-nY, left, right-nX);									};
		void	CollapseInt( int nX, int nY )	{ Assimilate( top, bottom-(REAL)nY, left, right-(REAL)nX);						};

	//
	//	P o i n t   R e t r i e v a l:
	//

		CPoint	TopLeftCPoint()			{ return (CPoint)(*this);							};
		Point	TopLeftPoint()			{ return (Point)(*this);							};
		PointF	TopLeftPointF()			{ return (PointF)(*this);							};

		CPoint	TopRightCPoint()		{ return CPoint((int)right, (int)top);				};
		Point	TopRightPoint()			{ return Point((int)right, (int)top);				};
		PointF	TopRightPointF()		{ return PointF(right, top);						};

		CPoint	BottomRightCPoint()		{ return CPoint((int)right, (int)bottom);			};
		Point	BottomRightPoint()		{ return Point((int)right, (int)bottom);			};
		PointF	BottomRightPointF()		{ return PointF(right, bottom);						};

		CPoint	BottomLeftCPoint()		{ return CPoint((int)left, (int)bottom);			};
		Point	BottomLeftPoint()		{ return Point((int)left, (int)bottom);				};
		PointF	BottomLeftPointF()		{ return PointF(left, bottom);						};

	//
	//		Various Others:
	//

		bool	HitTest( CPoint point )	{ return CRect(*this).PtInRect( point )==TRUE;						};
		bool	HitTest( Point  point )	{ return CRect(*this).PtInRect( CPoint(point.X, point.Y) )==TRUE;	};
		bool	HitTest( PointF point )	{ return CRect(*this).PtInRect( CPoint((int)point.X, (int)point.Y) )==TRUE;	};

		void	Dump()
		{
			TRACE0("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\nCGDIRect Values:\n");
			TRACE("\ttop:\t%.2f",		top);
			TRACE("\tleft:\t%.2f\n",	left);
			TRACE("\tright:\t%.2f\t",	right);
			TRACE("\tbottom:\t%.2f\n",	bottom);
			TRACE("\tWidth:\t%d",		WidthInt());
			TRACE("\tHeight:\t%d\n\n",	HeightInt());
			TRACE0("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
		}

	//
	//		Private Methods:
	//
private:

	CGDIRect& Assimilate( Point location, Size size)		
	{
		return Assimilate( Rect( location, size ) );
	};

	CGDIRect& Assimilate( CPoint location, CSize size)		
	{
		return Assimilate( CRect( location, size ) );
	};

	CGDIRect& Assimilate( int nValue )		
	{	
		top		= (REAL)nValue; 
		bottom	= (REAL)nValue; 
		left	= (REAL)nValue; 
		right	= (REAL)nValue; 

		return	*this;
	};

	CGDIRect& Assimilate( REAL fValue )		
	{	
		top		= fValue; 
		bottom	= fValue;
		left	= fValue;
		right	= fValue;

		return	*this;
	};

	CGDIRect& Assimilate( CRect rcData )		
	{	
		top		= (REAL)rcData.top; 
		bottom	= (REAL)rcData.bottom; 
		left	= (REAL)rcData.left; 
		right	= (REAL)rcData.right; 

		return	*this;
	};

	CGDIRect& Assimilate( REAL rleft, REAL rtop, REAL rright, REAL rbottom)		
	{	
		top		= rtop; 
		bottom	= rbottom; 
		left	= rleft; 
		right	= rright; 

		return	*this;
	};

	CGDIRect& Assimilate( int nleft, int ntop, int nright, int nbottom)		
	{	
		top		= (REAL)ntop; 
		bottom	= (REAL)nbottom; 
		left	= (REAL)nleft; 
		right	= (REAL)nright; 

		return	*this;
	};

	CGDIRect& Assimilate( Rect rcData )		
	{	
		top		= (REAL)rcData.GetTop(); 
		bottom	= (REAL)rcData.GetBottom(); 
		left	= (REAL)rcData.GetLeft(); 
		right	= (REAL)rcData.GetRight(); 

		return	*this;
	};

	CGDIRect& Assimilate( RectF rcData )		
	{	
		top		= rcData.GetTop(); 
		bottom	= rcData.GetBottom(); 
		left	= rcData.GetLeft(); 
		right	= rcData.GetRight(); 

		return	*this;
	};

	CGDIRect& Assimilate( CGDIRect& rcData )		
	{	
		top		= rcData.top; 
		bottom	= rcData.bottom; 
		left	= rcData.left; 
		right	= rcData.right; 

		return	*this;
	};

	//
	//		V a r i a b l e s
	//
public:

	REAL		top,
				bottom,
				left,
				right;
};

#endif // !defined(AFX_GDIRECT_H__2AC1B0EE_504D_4BF8_B567_0766B3555F88__INCLUDED_)
