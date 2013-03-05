#include "StdAfx.h"
#include ".\bcstroke.h"
#include "DHPaper.h"
#include "dhstroke.h"
#include <algorithm>
#include "FColor.h"
#include "FCSinglePixelProcessBase.h"

#include "GraphicsGems.h"
#include <vector>



CBCStroke::CBCStroke(void)
{
	m_lpStrokeNext=NULL;
	m_lpStrokePre=NULL;
	m_zoompaper=1;
	m_zoompress=1;
}
CBCStroke::CBCStroke(CBCStroke& stroke)
{
	DISCURVE& disCurve=stroke.GetDisCurve();
	DISCURVE::iterator it;
	for(it=disCurve.begin();it!=disCurve.end();++it){
		m_disCurve.push_back(*it);
	}
	BATTRILIST::iterator itTmp;
	BATTRILIST& pList=stroke.GetAttriList();
	for(itTmp=pList.begin();itTmp!=pList.end();++itTmp){
		m_pressList.push_back(*itTmp);
	}
	m_lpPaper=stroke.GetPaper();
	m_c=stroke.GetColor();
	m_penWidth=stroke.GetPenWidth();
	m_isErase=stroke.IsErase();
	m_rcSave=stroke.GetRect();
	BEZIERSTROKE& bezStroke=stroke.GetStroke();
	BEZIERSTROKE::iterator it2;
	for(it2=bezStroke.begin();it2!=bezStroke.end();++it2){
		m_bezStroke.push_back(*it2);
	}
	m_lpStrokeNext=stroke.m_lpStrokeNext;
	m_lpStrokePre=stroke.m_lpStrokePre;
	m_zoompress=stroke.m_zoompress;
	m_zoompaper=stroke.m_zoompaper;
}
CBCStroke::~CBCStroke(void)
{
	
}
/*
*  FitCurve :
*  	Fit a Bezier curve to a set of digitized points 
*/
void CBCStroke::FitCurve(
DISCURVE& d,			/*  Array of digitized points	*/
int		nPts,		/*  Number of digitized points	*/
double	error)		/*  User-defined error squared	*/
{
	Vector2	tHat1, tHat2;	/*  Unit tangent vectors at endpoints */

	tHat1 = ComputeLeftTangent(d, 0);
	tHat2 = ComputeRightTangent(d, nPts - 1);
	FitCubic(d, 0, nPts - 1, tHat1, tHat2, error);
}
/*
*  FitCubic :
*  	Fit a Bezier curve to a (sub)set of digitized points
*/
void CBCStroke::FitCubic(
DISCURVE& d,			/*  Array of digitized points */
int		first,int last,/* Indices of first and last pts in region */
Vector2	tHat1,Vector2 tHat2,	/* Unit tangent vectors at endpoints */
double	error)		/*  User-defined error squared	   */
{
	BezierCurve	bezCurve; /*Control points of fitted Bezier curve*/
	double	*u;		/*  Parameter values for point  */
	double	*uPrime;	/*  Improved parameter values */
	double	maxError;	/*  Maximum fitting error	 */
	int		splitPoint;	/*  Point to split point set at	 */
	int		nPts;		/*  Number of points in subset  */
	double	iterationError; /*Error below which you try iterating  */
	int		maxIterations = 5; /*  Max times to try iterating  */
	Vector2	tHatCenter;   	/* Unit tangent vector at splitPoint */
	int		i;		

	iterationError = error * error;
	nPts = last - first + 1;

	/*  Use heuristic if region only has two points in it */
	if (nPts == 2) {
		double dist = V2DistanceBetween2Points(&d[last], &d[first]) / 3.0;

		bezCurve = (Point2 *)malloc(4 * sizeof(Point2));
		bezCurve[0] = d[first];
		bezCurve[3] = d[last];
		V2Add(&bezCurve[0], V2Scale(&tHat1, dist), &bezCurve[1]);
		V2Add(&bezCurve[3], V2Scale(&tHat2, dist), &bezCurve[2]);
		//DrawBezierCurve(, bezCurve);
		double u[2];
		u[0]=0;
		u[1]=1;
		PARAMERLIST paramList;
		CopyParamList(first,last,u,paramList);
		AddBezierCurve(3,bezCurve,paramList);

		free((void *)bezCurve);
		return;
	}

	/*  Parameterize points, and attempt to fit curve */
	u = ChordLengthParameterize(d, first, last);
	bezCurve = GenerateBezier(d, first, last, u, tHat1, tHat2);

	/*  Find max deviation of points to fitted curve */
	maxError = ComputeMaxError(d, first, last, bezCurve, u, &splitPoint);
	if (maxError < error) {
		//DrawBezierCurve(3, bezCurve);
		PARAMERLIST paramList;
		CopyParamList(first,last,u,paramList);
		AddBezierCurve(3,bezCurve,paramList);
		free((void *)u);
		free((void *)bezCurve);
		return;
	}


	/*  If error not too large, try some reparameterization  */
	/*  and iteration */
	if (maxError < iterationError) {
		for (i = 0; i < maxIterations; i++) {
			uPrime = Reparameterize(d, first, last, u, bezCurve);
			bezCurve = GenerateBezier(d, first, last, uPrime, tHat1, tHat2);
			maxError = ComputeMaxError(d, first, last,
				bezCurve, uPrime, &splitPoint);
			if (maxError < error) {
				//DrawBezierCurve(3, bezCurve);
				PARAMERLIST paramList;
				CopyParamList(first,last,u,paramList);
				AddBezierCurve(3,bezCurve,paramList);

				free((void *)u);
				free((void *)bezCurve);
				return;
			}
			free((void *)u);
			u = uPrime;
		}
	}

	/* Fitting failed -- split at max error point and fit recursively */
	free((void *)u);
	free((void *)bezCurve);
	tHatCenter = ComputeCenterTangent(d, splitPoint);
	FitCubic(d, first, splitPoint, tHat1, tHatCenter, error);
	V2Negate(&tHatCenter);
	FitCubic(d, splitPoint, last, tHatCenter, tHat2, error);
}


/*
*  GenerateBezier :
*  Use least-squares method to find Bezier control points for region.
*
*/
BezierCurve CBCStroke::GenerateBezier(
DISCURVE& d,			/*  Array of digitized points	*/
int		first,int last,		/*  Indices defining region	*/
double	*uPrime,		/*  Parameter values for region */
Vector2	tHat1,Vector2 tHat2)	/*  Unit tangents at endpoints	*/
{
	int 	i;
	Vector2 	A[MAXPOINTS][2];	/* Precomputed rhs for eqn	*/
	int 	nPts;			/* Number of pts in sub-curve */
	double 	C[2][2];			/* Matrix C		*/
	double 	X[2];			/* Matrix X			*/
	double 	det_C0_C1,		/* Determinants of matrices	*/
		det_C0_X,
		det_X_C1;
	double 	alpha_l,		/* Alpha values, left and right	*/
		alpha_r;
	Vector2 	tmp;			/* Utility variable		*/
	BezierCurve	bezCurve;	/* RETURN bezier curve ctl pts	*/

	bezCurve = (Point2 *)malloc(4 * sizeof(Point2));
	nPts = last - first + 1;


	/* Compute the A's	*/
	for (i = 0; i < nPts; i++) {
		Vector2		v1, v2;
		v1 = tHat1;
		v2 = tHat2;
		V2Scale(&v1, B1(uPrime[i]));
		V2Scale(&v2, B2(uPrime[i]));
		A[i][0] = v1;
		A[i][1] = v2;
	}

	/* Create the C and X matrices	*/
	C[0][0] = 0.0;
	C[0][1] = 0.0;
	C[1][0] = 0.0;
	C[1][1] = 0.0;
	X[0]    = 0.0;
	X[1]    = 0.0;

	for (i = 0; i < nPts; i++) {
		C[0][0] += V2Dot(&A[i][0], &A[i][0]);
		C[0][1] += V2Dot(&A[i][0], &A[i][1]);
		/*					C[1][0] += V2Dot(&A[i][0], &A[i][1]);*/	
		C[1][0] = C[0][1];
		C[1][1] += V2Dot(&A[i][1], &A[i][1]);

		tmp = V2SubII(d[first + i],
			V2AddII(
			V2ScaleIII(d[first], B0(uPrime[i])),
			V2AddII(
			V2ScaleIII(d[first], B1(uPrime[i])),
			V2AddII(
			V2ScaleIII(d[last], B2(uPrime[i])),
			V2ScaleIII(d[last], B3(uPrime[i]))))));


		X[0] += V2Dot(&A[i][0], &tmp);
		X[1] += V2Dot(&A[i][1], &tmp);
	}

	/* Compute the determinants of C and X	*/
	det_C0_C1 = C[0][0] * C[1][1] - C[1][0] * C[0][1];
	det_C0_X  = C[0][0] * X[1]    - C[0][1] * X[0];
	det_X_C1  = X[0]    * C[1][1] - X[1]    * C[0][1];

	/* Finally, derive alpha values	*/
	if (det_C0_C1 == 0.0) {
		det_C0_C1 = (C[0][0] * C[1][1]) * 10e-12;
	}
	alpha_l = det_X_C1 / det_C0_C1;
	alpha_r = det_C0_X / det_C0_C1;


	/*  If alpha negative, use the Wu/Barsky heuristic (see text) */
	/* (if alpha is 0, you get coincident control points that lead to
	* divide by zero in any subsequent NewtonRaphsonRootFind() call. */
	if (alpha_l < 1.0e-6 || alpha_r < 1.0e-6) {
		double	dist = V2DistanceBetween2Points(&d[last], &d[first]) /
			3.0;

		bezCurve[0] = d[first];
		bezCurve[3] = d[last];
		V2Add(&bezCurve[0], V2Scale(&tHat1, dist), &bezCurve[1]);
		V2Add(&bezCurve[3], V2Scale(&tHat2, dist), &bezCurve[2]);
		return (bezCurve);
	}

	/*  First and last control points of the Bezier curve are */
	/*  positioned exactly at the first and last data points */
	/*  Control points 1 and 2 are positioned an alpha distance out */
	/*  on the tangent vectors, left and right, respectively */
	bezCurve[0] = d[first];
	bezCurve[3] = d[last];
	V2Add(&bezCurve[0], V2Scale(&tHat1, alpha_l), &bezCurve[1]);
	V2Add(&bezCurve[3], V2Scale(&tHat2, alpha_r), &bezCurve[2]);
	return (bezCurve);
}


/*
*  Reparameterize:
*	Given set of points and their parameterization, try to find
*   a better parameterization.
*
*/
double *CBCStroke::Reparameterize(
							  DISCURVE& d,			/*  Array of digitized points	*/
							  int		first,int last,	/*  Indices defining region	*/
							  double	*u,			/*  Current parameter values	*/
							  BezierCurve	bezCurve)	/*  Current fitted curve	*/
{
	int 	nPts = last-first+1;	
	int 	i;
	double	*uPrime;		/*  New parameter values	*/

	uPrime = (double *)malloc(nPts * sizeof(double));
	for (i = first; i <= last; i++) {
		uPrime[i-first] = NewtonRaphsonRootFind(bezCurve, d[i], u[i-first]);
	}
	return (uPrime);
}



/*
*  NewtonRaphsonRootFind :
*	Use Newton-Raphson iteration to find better root.
*/
double CBCStroke::NewtonRaphsonRootFind(
										BezierCurve	Q,			/*  Current fitted curve	*/
										Point2 		P,		/*  Digitized point		*/
										double 		u)		/*  Parameter value for "P"	*/
{
	double 		numerator, denominator;
	Point2 		Q1[3], Q2[2];	/*  Q' and Q''			*/
	Point2		Q_u, Q1_u, Q2_u; /*u evaluated at Q, Q', & Q''	*/
	double 		uPrime;		/*  Improved u			*/
	int 		i;

	/* Compute Q(u)	*/
	Q_u = BezierII(3, Q, u);

	/* Generate control vertices for Q'	*/
	for (i = 0; i <= 2; i++) {
		Q1[i].x = (Q[i+1].x - Q[i].x) * 3.0;
		Q1[i].y = (Q[i+1].y - Q[i].y) * 3.0;
	}

	/* Generate control vertices for Q'' */
	for (i = 0; i <= 1; i++) {
		Q2[i].x = (Q1[i+1].x - Q1[i].x) * 2.0;
		Q2[i].y = (Q1[i+1].y - Q1[i].y) * 2.0;
	}

	/* Compute Q'(u) and Q''(u)	*/
	Q1_u = BezierII(2, Q1, u);
	Q2_u = BezierII(1, Q2, u);

	/* Compute f(u)/f'(u) */
	numerator = (Q_u.x - P.x) * (Q1_u.x) + (Q_u.y - P.y) * (Q1_u.y);
	denominator = (Q1_u.x) * (Q1_u.x) + (Q1_u.y) * (Q1_u.y) +
		(Q_u.x - P.x) * (Q2_u.x) + (Q_u.y - P.y) * (Q2_u.y);

	/* u = u - f(u)/f'(u) */
	uPrime = u - (numerator/denominator);
	return (uPrime);
}



/*
*  Bezier :
*  	Evaluate a Bezier curve at a particular parameter value
* 
*/
Point2 CBCStroke::BezierII(
						int		degree,		/* The degree of the bezier curve	*/
						Point2 	*V,		/* Array of control points		*/
						double 	t)		/* Parametric value to find point for	*/
{
	int 	i, j;		
	Point2 	Q;	        /* Point on curve at parameter t	*/
	Point2 	*Vtemp;		/* Local copy of control points		*/

	/* Copy array	*/
	Vtemp = (Point2 *)malloc((unsigned)((degree+1) 
		* sizeof (Point2)));
	for (i = 0; i <= degree; i++) {
		Vtemp[i] = V[i];
	}

	/* Triangle computation	*/
	for (i = 1; i <= degree; i++) {	
		for (j = 0; j <= degree-i; j++) {
			Vtemp[j].x = (1.0 - t) * Vtemp[j].x + t * Vtemp[j+1].x;
			Vtemp[j].y = (1.0 - t) * Vtemp[j].y + t * Vtemp[j+1].y;
		}
	}

	Q = Vtemp[0];
	free((void *)Vtemp);
	return Q;
}


/*
*  B0, B1, B2, B3 :
*	Bezier multipliers
*/
double CBCStroke::B0(double u)
{
	double tmp = 1.0 - u;
	return (tmp * tmp * tmp);
}


double CBCStroke::B1(double u)
{
	double tmp = 1.0 - u;
	return (3 * u * (tmp * tmp));
}

double CBCStroke::B2(double u)
{
	double tmp = 1.0 - u;
	return (3 * u * u * tmp);
}

double CBCStroke::B3(double u)
{
	return (u * u * u);
}



/*
* ComputeLeftTangent, ComputeRightTangent, ComputeCenterTangent :
*Approximate unit tangents at endpoints and "center" of digitized curve
*/
Vector2 CBCStroke::ComputeLeftTangent(
									 DISCURVE& d,			/*  Digitized points*/
									  int		end)		/*  Index to "left" end of region */
{
	Vector2	tHat1;
	tHat1 = V2SubII(d[end+1], d[end]);
	tHat1 = *V2Normalize(&tHat1);
	return tHat1;
}

Vector2 CBCStroke::ComputeRightTangent(
									   DISCURVE& d,			/*  Digitized points		*/
									   int		end)		/*  Index to "right" end of region */
{
	Vector2	tHat2;
	tHat2 = V2SubII(d[end-1], d[end]);
	tHat2 = *V2Normalize(&tHat2);
	return tHat2;
}


Vector2 CBCStroke::ComputeCenterTangent(
										DISCURVE& d,			/*  Digitized points			*/
										int		center)		/*  Index to point inside region	*/
{
	Vector2	V1, V2, tHatCenter;

	V1 = V2SubII(d[center-1], d[center]);
	V2 = V2SubII(d[center], d[center+1]);
	tHatCenter.x = (V1.x + V2.x)/2.0;
	tHatCenter.y = (V1.y + V2.y)/2.0;
	tHatCenter = *V2Normalize(&tHatCenter);
	return tHatCenter;
}


/*
*  ChordLengthParameterize :
*	Assign parameter values to digitized points 
*	using relative distances between points.
*/
double *CBCStroke::ChordLengthParameterize(
											DISCURVE& d,			/* Array of digitized points */
											int		first,int last)		/*  Indices defining region	*/
{
	int		i;	
	double	*u;			/*  Parameterization		*/

	u = (double *)malloc((unsigned)(last-first+1) * sizeof(double));

	u[0] = 0.0;
	for (i = first+1; i <= last; i++) {
		u[i-first] = u[i-first-1] +
			V2DistanceBetween2Points(&d[i], &d[i-1]);
	}

	for (i = first + 1; i <= last; i++) {
		u[i-first] = u[i-first] / u[last-first];
	}

	return(u);
}
/*
*  ComputeMaxError :
*	Find the maximum squared distance of digitized points
*	to fitted curve.
*/
double CBCStroke::ComputeMaxError(
								  DISCURVE& d,			/*  Array of digitized points	*/
								  int		first,int last,		/*  Indices defining region	*/
								  BezierCurve	bezCurve,		/*  Fitted Bezier curve		*/
								  double	*u,			/*  Parameterization of points	*/
								  int		*splitPoint)		/*  Point of maximum error	*/
{
	int		i;
	double	maxDist;		/*  Maximum error		*/
	double	dist;		/*  Current error		*/
	Point2	P;			/*  Point on curve		*/
	Vector2	v;			/*  Vector from point to curve	*/

	*splitPoint = (last - first + 1)/2;
	maxDist = 0.0;
	for (i = first + 1; i < last; i++) {
		P = BezierII(3, bezCurve, u[i-first]);
		v = V2SubII(P, d[i]);
		dist = V2SquaredLength(&v);
		if (dist >= maxDist) {
			maxDist = dist;
			*splitPoint = i;
		}
	}
	return (maxDist);
}
void CBCStroke::CopyParamList(int first,int last,double* u,PARAMERLIST& paramList)
{
	for (int i = first; i <=last; i++) {
		PARAMER p;
		p.u=u[i-first];
		p.press=m_pressList[i].press;
		//paramList.push_back(p);
		InsertParamer(p,paramList);
	}
}
void CBCStroke::InsertParamer(PARAMER& p,PARAMERLIST& paramList)
{
	int size=paramList.size();
	if(paramList.empty()){
		paramList.push_back(p);
	}else{
		if(p.u>=paramList[size-1].u){
			paramList.push_back(p);
			return;
		}
		int i=0,j=size-1,tmp=0;
		while(1){
			tmp=(i+j)>>1;
			if(tmp<=i)
				break;
			if(paramList[tmp].u==p.u)
				return;
			else if(paramList[tmp].u>p.u)
				j=tmp;
			else i=tmp;
		}
		paramList.insert(paramList.begin()+tmp+1,p);
	}
}
Vector2 CBCStroke::V2AddII(Vector2 a,Vector2 b)
{
	Vector2	c;
	c.x = a.x + b.x;  c.y = a.y + b.y;
	return (c);
}
Vector2 CBCStroke::V2ScaleIII(Vector2 v,double s)
{
	Vector2 result;
	result.x = v.x * s; result.y = v.y * s;
	return (result);
}

Vector2 CBCStroke::V2SubII(Vector2 a,Vector2 b)
{
	Vector2	c;
	c.x = a.x - b.x; c.y = a.y - b.y;
	return (c);
}
void CBCStroke::AddBezierCurve(int degree,BezierCurve bezCurve,PARAMERLIST& paramList)
{
	BezierPressCurve bezpCurve;
	bezpCurve.degree=degree;
	bezpCurve.bezCurve = (Point2 *)malloc((unsigned)((degree+1) 
		* sizeof (Point2)));
	//for(int i=0;i<=degree;++i){
	//	bezpCurve.bezCurve[i].x=bezCurve[i].x;
	//	bezpCurve.bezCurve[i].y=bezCurve[i].y;
	//}
	memcpy(bezpCurve.bezCurve,bezCurve,(degree+1) * sizeof (Point2));
	double numpoint=0;
	if(degree>=2){
		numpoint=V2DistanceBetween2Points(&(bezCurve[0]),&(bezCurve[1]));
		for (int i =1; i <degree; i++) {
			numpoint+=V2DistanceBetween2Points(&(bezCurve[i]),&(bezCurve[i+1]));
		}
	}
	bezpCurve.numpoint=numpoint*2;
	//copy(paramList.begin(),paramList.end(),bezpCurve.uList.begin());
	bezpCurve.uList.swap(paramList);
//	paramList.clear();
	m_bezStroke.push_back(bezpCurve);
}
void CBCStroke::Zoom(double ratio,double zoompress)
{
	if(ratio==1&&zoompress==1)
		return;
	m_zoompaper=ratio;
	m_zoompress=zoompress;
	m_rcSave.bottom=m_rcSave.bottom*ratio;
	m_rcSave.left=m_rcSave.left*ratio;
	m_rcSave.right=m_rcSave.right*ratio;
	m_rcSave.top=m_rcSave.top*ratio;
	BEZIERSTROKE::iterator it;
	for(it=m_bezStroke.begin();it!=m_bezStroke.end();++it){
		//DrawBezierCurve(img,*it,alpha,zoompaper*3,zoompress*3);
		BezierCurve bc=it->bezCurve;
		for(int i=0;i<=it->degree;++i){
			bc[i].x*=ratio;
			bc[i].y*=ratio;
		}
		PARAMERLIST& uList=it->uList;
		PARAMERLIST::iterator it;
		if(zoompress==1)
			continue;
		for(it=uList.begin();it!=uList.end();++it){
			it->press*=zoompress;
		}
	}
}
void CBCStroke::Draw(CDC* pDC)
{
	BEZIERSTROKE::iterator it;
	for(it=m_bezStroke.begin();it!=m_bezStroke.end();++it){
		DrawBezierCurve(pDC,*it);
	}
}


//********************************************************************************************************
void CBCStroke::Draw(FCObjImage& img,BYTE alpha,float zoompaper,float zoompress)
{
	//m_vb.BeginOneStrokeDraw(img.GetPaper());
	BEZIERSTROKE::iterator it;
	int w=img.Width();
	int h=img.Height();
	img.Stretch(w*3,h*3);
	for(it=m_bezStroke.begin();it!=m_bezStroke.end();++it){
		DrawBezierCurve(img,*it,alpha,zoompaper*3,zoompress*3);
	}
	img.AntiAliased_Zoom(3);
	//m_vb.EndOneStrokeDraw(img.GetPaper());
}

void CBCStroke::Draw(FCObjImage& img,float zoompaper,float zoompress)
{
	if(m_bezStroke.empty())
		return;
	FCObjImage imgBlock;
	RECT rcTmp;
	rcTmp.bottom=m_rcSave.bottom*zoompaper;
	rcTmp.left=m_rcSave.left*zoompaper;
	rcTmp.right=m_rcSave.right*zoompaper;
	rcTmp.top=m_rcSave.top*zoompaper;
	img.GetSubBlock (&imgBlock,rcTmp);
	
//	if(m_Undo.IsValidImage()==false)
//		m_Undo=imgBlock;
	// 制作大小一样的alpha位图，注意：位图数据初始化为0（也即黑色）
	FCObjImage		imgAlpha (imgBlock.Width(), imgBlock.Height(), 8) ;

	Draw(imgAlpha,255,zoompaper,zoompress);
	
	// 制作纯色位图
	FCObjImage          imgCover (imgBlock.Width(), imgBlock.Height(), 32) ;
	RGBQUAD c;
	c.rgbRed=GetRValue(m_c);
	c.rgbGreen=GetGValue(m_c);
	c.rgbBlue=GetBValue(m_c);
	FCPixelFillColor    cmdFillCr(c) ;
	imgCover.SinglePixelProcessProc (cmdFillCr) ;
	imgCover.AppendAlphaChannel (imgAlpha) ;

	imgBlock.CombineAlphaImg (imgCover);
	img.CoverBlock(imgBlock,rcTmp.left,rcTmp.top);
	//std::sort(
}

void CBCStroke::DrawBezierCurve(CDC* pDC,BezierPressCurve& bezpCurve)
{
	double t,delt;
	delt=(double)1/bezpCurve.numpoint;
	TPosition position;
	t=0.0;
	int x1,y1,x2,y2;
	x1=y1=x2=y2=0;
	for(int i=0;i<=bezpCurve.numpoint;++i){
		Point2 p=CBCStroke::BezierII(bezpCurve.degree,bezpCurve.bezCurve,t);
		x1=p.x;
		y1=p.y;
		if(x1!=x2||y1!=y2){
			PARAMER param;
			//CStrokeTransform::GenParamer(bezpCurve.uList,t,param);
			position.press=param.press;
			position.m_angle_z=param.m_angle_z;
			position.m_angle_xy=param.m_angle_xy;
			position.m_x=x1;
			position.m_y=y1;
			m_lpPaper->DrawSection(pDC,position,m_c);
			x2=x1;
			y2=y1;
		}	
		t+=delt;
	}
}
void CBCStroke::DrawHarryBrush(FCObjImage& img,BezierPressCurve& bezpCurve,BYTE alpha,float zoom)
{
	double t,delt;
	t=0.0;
	int x1,y1,x2,y2;
	x1=y1=x2=y2=0;
	for(int i=0;i<=bezpCurve.degree;++i){
		bezpCurve.bezCurve[i].x=zoom*(bezpCurve.bezCurve[i].x);
		bezpCurve.bezCurve[i].y=zoom*(bezpCurve.bezCurve[i].y);
	}
	float numpoint=0;
	if(bezpCurve.degree>=2){
		numpoint=V2DistanceBetween2Points(&(bezpCurve.bezCurve[0]),&(bezpCurve.bezCurve[1]));
		for (int i =1; i <bezpCurve.degree; i++) {
			numpoint+=V2DistanceBetween2Points(&(bezpCurve.bezCurve[i]),&(bezpCurve.bezCurve[i+1]));
		}
	}
	numpoint=numpoint*2;
	if(numpoint<=0)
		return;
	delt=(double)1/numpoint;
	bezpCurve.numpoint=0;
	int count=0;
	TPosition tp[3];
	std::vector<TPosition> pList;
	for(int i=0;i<=numpoint;++i){				
		Point2 p=CBCStroke::BezierII(bezpCurve.degree,bezpCurve.bezCurve,t);
		x1=p.x;
		y1=p.y;
		if(x1!=x2||y1!=y2){
			PARAMER param;
			//CStrokeTransform::GenParamer(bezpCurve.uList,t,param);
			TPosition position;
			
			position.press=(param.press<1)?1:param.press;
			position.m_angle_z=param.m_angle_z;
			position.m_angle_xy=param.m_angle_xy;
			position.m_x=x1;
			position.m_y=y1;
			//m_disCurve.
			pList.push_back(position);
			//m_lpPaper->DrawSection(img,position,alpha);
			x2=x1;
			y2=y1;
			bezpCurve.numpoint++;
		}	
		t+=delt;
	}
	int size=pList.size();
	if(size<=3)
		return;
	std::vector<TPosition> pList2;
	int i=0;
	for(;i<size/2-1;++i)
	{
		pList2.push_back(pList[2*i]);
	}
	pList2.push_back(pList[size-1]);

	Vector2 v,v_cur,v_pre;
	v.x=pList2[1].m_x-pList2[0].m_x;
	v.y=pList2[1].m_y-pList2[0].m_y;
	V2MakePerpendicular(&v,&v_pre);
	int j=0;
	for(j=2;j<pList2.size();++j)
	{
		Point2 A,B,C;
		A.x=pList[j-2].m_x;
		A.y=pList[j-2].m_y;
		B.x=pList[j-1].m_x;
		B.y=pList[j-1].m_y;
		C.x=pList[j].m_x;
		C.y=pList[j].m_y;
		v_cur=AngleBisectors(A,B,C);
		m_lpPaper->DrawSection(img,pList2[j-2],v_pre,pList2[j-1],v_cur,alpha);
		v_pre=v_cur;
	}
	if(i<=2)
		return;
	v.x=pList2[j-1].m_x-pList2[j-2].m_x;
	v.y=pList2[j-1].m_y-pList2[j-2].m_y;
	V2MakePerpendicular(&v,&v_cur);
	m_lpPaper->DrawSection(img,pList2[j-2],v_pre,pList2[j-1],v_cur,alpha);
}
void CBCStroke::DrawBezierCurve(FCObjImage& img,BezierPressCurve& bezpCurve,BYTE alpha,float zoompaper,float zoompress)
{
	double t,delt;
	//delt=(double)1/bezpCurve.numpoint;
	TPosition position;
	t=0.0;
	int x1,y1,x2,y2;
	x1=y1=x2=y2=0;
	BezierCurve bezCurve=new Point2[bezpCurve.degree+1];
	for(int i=0;i<=bezpCurve.degree;++i){
		bezCurve[i].x=zoompaper*(bezpCurve.bezCurve[i].x);
		bezCurve[i].y=zoompaper*(bezpCurve.bezCurve[i].y);
	}
/*	for(int i=0;i<=bezpCurve.degree;++i){
		bezpCurve.bezCurve[i].x=zoompaper*(bezpCurve.bezCurve[i].x);
		bezpCurve.bezCurve[i].y=zoompaper*(bezpCurve.bezCurve[i].y);
	}*/
	float numpoint=0;
	if(bezpCurve.degree>=2){
		numpoint=V2DistanceBetween2Points(&(bezCurve[0]),&(bezCurve[1]));
		for (int i =1; i <bezpCurve.degree; i++) {
			numpoint+=V2DistanceBetween2Points(&(bezCurve[i]),&(bezCurve[i+1]));
		}
	}
	
	numpoint=numpoint*2;
	if(numpoint<=0)
		return;
	delt=(double)1/numpoint;
	bezpCurve.numpoint=0;
	for(int i=0;i<=numpoint;++i){				
		Point2 p=CBCStroke::BezierII(bezpCurve.degree,bezCurve,t);
		//if(i==numpoint)
		//	continue;
		x1=p.x;
		y1=p.y;
		if(x1!=x2||y1!=y2){
			PARAMER param;
			//CStrokeTransform::GenParamer(bezpCurve.uList,t,param);
			position.press=param.press*zoompress;
			position.press=(position.press<1)?1:position.press;
			position.m_angle_z=param.m_angle_z;
			position.m_angle_xy=param.m_angle_xy;
			position.m_x=x1;
			position.m_y=y1;
			m_lpPaper->DrawSection(img,position,alpha);
			x2=x1;
			y2=y1;
			bezpCurve.numpoint++;
		}	
		t+=delt;
	}														
}

/*
void  CBCStroke::ReDraw(FCObjImage& img)
{
	CBCStroke* ptr=this;
	FCObjImage imgSrc(img);
	if(m_Undo.IsValidImage())
		imgSrc.CoverBlock(m_Undo,m_rcSave.left,m_rcSave.top);
	while(ptr!=NULL){
		FCObjImage imgTmp;
		RECT rcRet,rc=ptr->GetRect();
		if(::IntersectRect(&rcRet,&rc,&m_rcSave)){
			imgSrc.GetSubBlock(&imgTmp,m_rcSave);
			ptr->UpDateUndo(imgTmp,m_rcSave);
			ptr->Draw(imgSrc);	
		}
		ptr=ptr->m_lpStrokeNext;
	}
	FCObjImage imgBlock;
	imgSrc.GetSubBlock(&imgBlock,m_rcSave);
	img.CoverBlock(imgBlock,m_rcSave.left,m_rcSave.top);
}
void CBCStroke::UpDateUndo(FCObjImage& imgSrc,RECT rcSrc)
{
	if(m_Undo.IsValidImage()==false)
		return;
	RECT rc;
	::NormalizeRect(rcSrc);
	if(::IntersectRect(&rc,&rcSrc,&m_rcSave)){
		RECT rcBlock;
		rcBlock.left=rc.left-rcSrc.left;
		rcBlock.right=rc.right-rcSrc.left;
		rcBlock.top=rc.top-rcSrc.top;
		rcBlock.bottom=rc.bottom-rcSrc.top;
		FCObjImage imgSub;
		imgSrc.GetSubBlock(&imgSub,rcBlock);
		rcBlock.left=rc.left-m_rcSave.left;
		rcBlock.top=rc.top-m_rcSave.top;
		m_Undo.CoverBlock(imgSub,rcBlock.left,rcBlock.top);
	}
}
*/
BOOL CBCStroke::IsInRgn(CPoint p)
{
	CRect rect;
	POINT pos;
	pos.x=p.x-m_rcSave.left;
	pos.y=p.y-m_rcSave.top;
	BEZIERSTROKE::iterator it;
	for(it=m_bezStroke.begin();it!=m_bezStroke.end();it++)
	{
		POINT pt;
		pt.x=it->bezCurve[0].x;
		pt.y=it->bezCurve[0].y;
		rect=CRect(pt,pt);
		rect.InflateRect(5,5);
		if(rect.PtInRect(pos))
			return true;
	}
	if(it!=m_bezStroke.begin())
	{
		it--;
		POINT pt;
		pt.x=it->bezCurve[3].x;
		pt.y=it->bezCurve[3].y;
		rect=CRect(pt,pt);
		rect.InflateRect(20,20);
		if(rect.PtInRect(pos))
			return true;
	}
	return false;
}
//----------------------------------------------------------------------------------------------
//----------------------------类bezierPressCurve的实现------------------------------------------

BezierPressCurve::BezierPressCurve(const BezierPressCurve& bezpCurve){
	numpoint=bezpCurve.numpoint;
	degree=bezpCurve.degree;
	bezCurve = (Point2 *)malloc((unsigned)((degree+1) 
		* sizeof (Point2)));
	memcpy(bezCurve,bezpCurve.bezCurve,(degree+1) * sizeof (Point2));
	int size=bezpCurve.uList.size();
	for(int i=0;i<size;++i){
		uList.push_back(bezpCurve.uList[i]);
	}        
}
//------------------------------------------------------
//----计算u为t时的bezier曲线的近似长度。
double BezierPressCurve::GetLengthFromParam(double t,double t_begin,double error)
{
	Point2 p1=CBCStroke::BezierII(degree,bezCurve,t_begin);	
	Point2 p2=CBCStroke::BezierII(degree,bezCurve,t);
	double length=V2DistanceBetween2Points(&p1,&p2);
	if((length>error)||(fabs(t-t_begin)>=0.25)){
		length=GetLengthFromParam((t+t_begin)/2,t_begin,error)+GetLengthFromParam(t,(t+t_begin)/2,error);
	}
	return length;
}
//------------------------------------------------------
//----计算长度为len时的u 值.
double BezierPressCurve::GetParamFromLength(double len,double error)
{
	if(len<=error)
		return 0.0;
	double lengthTmp=GetLengthFromParam(1.0);
	if(fabs(len-lengthTmp)<=error)
		return 1.0;
	else{
		double u=0.0,i=0.0,j=1.0;
		while(1){
			u=(i+j)/2;
			lengthTmp=GetLengthFromParam(u);
			if(fabs(len-lengthTmp)<=error){
				return u;
			}
			if(len<lengthTmp){
				j=u;
			}
			else i=u;			
		}
	}
}
