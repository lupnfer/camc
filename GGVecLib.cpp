/* 
2d and 3d Vector C Library 
by Andrew Glassner
from "Graphics Gems", Academic Press, 1990
*/
#include"stdAfx.h"
#include <math.h>
#include "GraphicsGems.h"
#include "BCStroke.h"
/******************/
/*   2d Library   */
/******************/

/* returns squared length of input vector */	
double V2SquaredLength(Vector2 *a) 
{	
	return((a->x * a->x)+(a->y * a->y));
}
	
/* returns length of input vector */
double V2Length(Vector2 *a) 
{
	return(sqrt(V2SquaredLength(a)));
}
	
/* negates the input vector and returns it */
Vector2 *V2Negate(Vector2 *v) 
{
	v->x = -v->x;  v->y = -v->y;
	return(v);
}

/* normalizes the input vector and returns it */
Vector2 *V2Normalize(Vector2 *v)
{
	double len = V2Length(v);
	if (len != 0.0) { v->x /= len;  v->y /= len; }
	return(v);
}


/* scales the input vector to the new length and returns it */
Vector2 *V2Scale(Vector2 *v,double newlen) 
{
	double len = V2Length(v);
	if (len != 0.0) { v->x *= newlen/len;   v->y *= newlen/len; }
	return(v);
}

/* return vector sum c = a+b */
Vector2* V2Add(Vector2 *a, Vector2 *b,Vector2 *c)
{
	c->x = a->x+b->x;  c->y = a->y+b->y;
	return(c);
}
	
/* return vector difference c = a-b */
Vector2 *V2Sub(Vector2 *a,Vector2 *b,Vector2 *c)
{
	c->x = a->x-b->x;  c->y = a->y-b->y;
	return(c);
}
/* return the dot product of vectors a and b */
double V2Dot(Vector2 *a,Vector2 *b) 
{
	return((a->x*b->x)+(a->y*b->y));
}

/* linearly interpolate between vectors by an amount alpha */
/* and return the resulting vector. */
/* When alpha=0, result=lo.  When alpha=1, result=hi. */
Vector2 *V2Lerp(Vector2 *lo,Vector2 *hi,Vector2 *result,double alpha)
{
	result->x = LERP(alpha, lo->x, hi->x);
	result->y = LERP(alpha, lo->y, hi->y);
	return(result);
}


/* make a linear combination of two vectors and return the result. */
/* result = (a * ascl) + (b * bscl) */
Vector2 *V2Combine (Vector2* a,Vector2* b,Vector2* result,double ascl,double bscl) 
{
	result->x = (ascl * a->x) + (bscl * b->x);
	result->y = (ascl * a->y) + (bscl * b->y);
	return(result);
}

/* multiply two vectors together component-wise */
Vector2 *V2Mul (Vector2 *a,Vector2 * b,Vector2 * result) 
{
	result->x = a->x * b->x;
	result->y = a->y * b->y;
	return(result);
}

/* return the distance between two points */
double V2DistanceBetween2Points(Point2 *a,Point2 *b)
{
	double dx = a->x - b->x;
	double dy = a->y - b->y;
	return(sqrt((dx*dx)+(dy*dy)));
}

/* return the vector perpendicular to the input vector a */
Vector2 *V2MakePerpendicular(Vector2 *a,Vector2 *ap)
{
	ap->x = -a->y;
	ap->y = a->x;
	return(ap);
}

/* create, initialize, and return a new vector */
Vector2 *V2New(double x,double y)
{
	Vector2 *v = NEWTYPE(Vector2);
	v->x = x;  v->y = y; 
	return(v);
}
	

/* create, initialize, and return a duplicate vector */
Vector2 *V2Duplicate(Vector2 *a)
{
	Vector2 *v = NEWTYPE(Vector2);
	v->x = a->x;  v->y = a->y; 
	return(v);
}
	
/* multiply a point by a matrix and return the transformed point */
Point2 *V2MulPointByMatrix(Point2 *p,Matrix3 *m)
{
	double w;
	Point2 ptmp;
	ptmp.x = (p->x * m->element[0][0]) + 
             (p->y * m->element[1][0]) + m->element[2][0];
	ptmp.y = (p->x * m->element[0][1]) + 
             (p->y * m->element[1][1]) + m->element[2][1];
	w    = (p->x * m->element[0][2]) + 
             (p->y * m->element[1][2]) + m->element[2][2];
	if (w != 0.0) { ptmp.x /= w;  ptmp.y /= w; }
	*p = ptmp;
	return(p);
}

/* multiply together matrices c = ab */
/* note that c must not point to either of the input matrices */
Matrix3 *V2MatMul(Matrix3 *a,Matrix3 *b,Matrix3 *c)
{
	int i, j, k;
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			c->element[i][j] = 0;
		for (k=0; k<3; k++) c->element[i][j] += 
				a->element[i][k] * b->element[k][j];
			}
		}
	return(c);
}



/* binary greatest common divisor by Silver and Terzian.  See Knuth */
/* both inputs must be >= 0 */
int gcd(int u,int v)
{
	int t, f;
	if ((u<0) || (v<0)) return(1); /* error if u<0 or v<0 */
	f = 1;
	while ((0 == (u%2)) && (0 == (v%2))) {
		u>>=1;  v>>=1,  f*=2;
		}
	if (u&01) { t = -v;  goto B4; } else { t = u; }
	B3: if (t > 0) { t >>= 1; } else { t = -((-t) >> 1); }
	B4: if (0 == (t%2)) goto B3;

	if (t > 0) u = t; else v = -t;
	if (0 != (t = u - v)) goto B3;
	return(u*f);
}	

Vector2 AngleBisectors(Point2 A,Point2 B,Point2 C)
{
	
	double a=V2DistanceBetween2Points(&C,&B);
	double b=V2DistanceBetween2Points(&A,&C);
	double c=V2DistanceBetween2Points(&A,&B);
	Vector2 As=CBCStroke::V2ScaleIII(A,a);
	Vector2 Bs=CBCStroke::V2ScaleIII(B,b);
	Vector2 Cs=CBCStroke::V2ScaleIII(C,c);
	Vector2 tmp,result;
	V2Add(&As,&Bs,&tmp);
	V2Add(&tmp,&Cs,&result);
	result.x/=(a+b+c);
	result.y/=(a+b+c);
	result.x=result.x-B.x;
	result.y=result.y-B.y;
	if(result.x==0&&result.y==0){
		Vector2 tmp;
		tmp.x=B.x-A.x;
		tmp.y=B.y-A.y;
		V2MakePerpendicular(&tmp,&result);
	}
	return result;
}
void MidpointEllipse(float a,float b,std::vector<Point2>& pList)
{
	int x,y;
	float d1,d2;
	x=0; y=b;
	d1=b*b+a*a*(-b+0.25);
	int p1=2*b*b*x+2*b*b+a*a;
	int p2=2*a*a*y;
	Point2 p;
	p.x=x;
	p.y=y;
	pList.push_back(p);
	while(p1<p2){
		if(d1<0){
			d1+=b*b*(2*x+3);
			x++;
		}
		else{
			d1+=b*b*(2*x+3)+a*a*((-2)*y+2);
			x++;y--;
			p2-=2*a*a;
		}
		p1+=2*b*b;
		Point2 p;
		p.x=x;
		p.y=y;
		pList.push_back(p);
	}
	d2=sqrt((double)(b*(x+0.5)))+a*(y-1)-a*b;
	while(y>0){
		if(d2<0)
		{
			d2+=b*b*(2*x+2)+a*a*((-2)*y+3);
			x++;y--;
		}else{
			d2+=a*a*(-2*y+3);
			y--;
		}
		Point2 p;
		p.x=x;
		p.y=y;
		pList.push_back(p);
	}
}