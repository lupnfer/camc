#ifndef _DIBAPI_H_
#define _DIBAPI_H_



//#define max(a,b) 
double WINAPI ComputeHSVSimilarity(float h1, float s1, float v1, float h2, float s2, float v2);
double WINAPI ComputeRGBSimilarity(int r1, int g1, int b1, int r2, int g2, int b2);
double WINAPI ComputeRGBDifference(int r1, int g1, int b1, int r2, int g2, int b2);
void WINAPI ConvertHSV(double ho, double so, double vo, double hn, double sn, double vn,
					  double h1, double s1, double v1, double &h, double &s, double &v);
void WINAPI ConvertRGB(BYTE ro, BYTE go, BYTE bo, BYTE rn, BYTE gn, BYTE bn,
					   BYTE r1, BYTE g1, BYTE b1, BYTE &r, BYTE &g, BYTE &b);
void WINAPI  RGB_to_HSV(BYTE rb,BYTE gb,BYTE bb,double &h,double &s,double &v);
void WINAPI  HSV_to_RGB(double h, double s, double v,BYTE &r, BYTE &g, BYTE &b);
void WINAPI  RGB_to_HLS(BYTE r, BYTE g, BYTE b,double &h, double &l, double &s);
void WINAPI  HLS_to_RGB(double h, double l, double s,BYTE &r, BYTE &g, BYTE &b);
void WINAPI  RGB_to_GRAY(BYTE r,BYTE g,BYTE b,BYTE& gray);
double value(double n1,double n2,double hue);

//void ConvertToGray(CImage *image);

#endif
