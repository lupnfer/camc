#pragma once

#include "Hvidicon.h"

#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}
#pragma comment(lib, "Winmm.lib")

namespace SXF{
	char drive[_MAX_DRIVE]={0};
	char dir[_MAX_DIR]={0};
	char szMainName[_MAX_FNAME]={0};
	char ext[_MAX_EXT]={0};
	DWORD dwDibSize;
	unsigned char *pR, *pG, *pB, *pRGBMax, *pRGBMaxThumbnail, *pRatio;
	BITMAPFILEHEADER BFH;
	BITMAPINFOHEADER bmih;
	int *pbIsForeground;	// background = 0, foreground = 1, center of foreground = 2; byte i is result of algorithm i
	int nHeight, nWidth, nBytesPerLineSource, npPaletteSize;
	int nBytesPerLineSourceRemain;
	const double pi = 3.141592753589793;
	const int nCornerPointLimit = 1024;
	int nCornerPointCount;
	int nCornerX[nCornerPointLimit];
	int nCornerY[nCornerPointLimit];
	//bool bBoundaryFilter = 0;
	struct SCalibPoint
	{
		double u;
		double v;
		double x;
		double y;
	};
	struct SBlock
	{
		int x;
		int y;
		int area;
		int start;
		int end;
		int left;
		int right;
		int low;
		int high;
	};

	struct SPoint2D
	{
		int x;
		int y;
	};

	SCalibPoint CalibPoint[nCornerPointLimit];
	double X, Y, U, V, XX, XY, YY, XU, YU, XV, YV, XXU, XYU, YYU, XXV, XYV, YYV;
	double XUU, YUU, XVV, YVV, XXUU, XYUU, YYUU, XXVV, XYVV, YYVV;

	bool SaveBmp(const char *pszFilename, int nType=1, bool bIsRatio=1)
	{
		FILE *fp;
		if(!(fp = fopen(pszFilename, "wb")))
			return FALSE;

		// Attempt to allocate the Dib memory.
		unsigned char *pDib	 = (unsigned char*)GlobalAlloc(GPTR, dwDibSize);
		memset(pDib, 192, dwDibSize);

		if(pDib == NULL)
		{
			fclose(fp);
			return FALSE;
		}

		fwrite(&BFH, 1, sizeof(BITMAPFILEHEADER), fp);
		memcpy(pDib, &bmih, sizeof(BITMAPINFOHEADER)+npPaletteSize);

		// Point m_pDibBits to the actual Dib bits data.
		unsigned char *pDibBits = pDib + sizeof(BITMAPINFOHEADER) + npPaletteSize;
		int i, j, nIndex;
		int nType1 = nType & 1;
		for(i = 0, nIndex = -1; i < nHeight; i++)
		{
			for(j = 0; j < nWidth; j++)
			{
				if(bIsRatio)
				{
					int nIsForeground = pbIsForeground[++nIndex];
					nIsForeground = (nIsForeground >> (8 * (nType >> 1))) & 255;
					if(nIsForeground == !nType1 || (nIsForeground == 2 && nType1 == 0))
						pDibBits += 3;
					else if(nIsForeground == nType1)
					{
						*pDibBits++ = pB[nIndex];
						*pDibBits++ = pG[nIndex];
						*pDibBits++ = pR[nIndex];
					}
					else
					{
						pDibBits += 2;
						*pDibBits++ = 255;
					}
					if((pbIsForeground[nIndex] >> 16) >= 2)
						pDibBits[-3] = 255;
					if((pbIsForeground[nIndex] >> 24) == 1)
						pDibBits[-2] = 255;
				}
				else
				{
					if(pRatio)
					{
						*pDibBits++ = pRatio[++nIndex];
						*pDibBits++ = pRatio[nIndex];
						*pDibBits++ = pRatio[nIndex];
					}
				}
			}
			pDibBits += nBytesPerLineSourceRemain;
		}

		fwrite(pDib , 1, dwDibSize, fp);
		GlobalFree(pDib);
		fclose(fp);
		return TRUE;
	}

	IplImage* SaveBmp1()
	{
		IplImage* pMask=AutoStorage->CreateIplImage(nWidth,nHeight,8,1);
		BYTE* pDibBits=(BYTE*)(pMask->imageData);
		int i, j, nIndex;
		for(i = 0, nIndex = -1; i < nHeight; i++)
		{
			for(j = 0; j < nWidth; j++)
			{
				int nIsForeground = pbIsForeground[++nIndex];
				nIsForeground = nIsForeground  & 255;
				if(nIsForeground)
				{
					*pDibBits++ = 255;
					
				}
				else
					pDibBits += 1;
			}
			//pDibBits += pMask->widthStep;
		}
		return pMask;
	}

	bool SaveBmpChessBoard(const char *pszFilename)
	{
		FILE *fp;
		if(!(fp = fopen(pszFilename, "wb")))
			return FALSE;

		// Attempt to allocate the Dib memory.
		unsigned char *pDib	 = (unsigned char*)GlobalAlloc(GPTR, dwDibSize);

		if(pDib == NULL)
		{
			fclose(fp);
			return FALSE;
		}

		fwrite(&BFH, 1, sizeof(BITMAPFILEHEADER), fp);
		memcpy(pDib, &bmih, sizeof(BITMAPINFOHEADER)+npPaletteSize);

		// Point m_pDibBits to the actual Dib bits data.
		unsigned char *pDibBits = pDib + sizeof(BITMAPINFOHEADER) + npPaletteSize;
		int i, j, nIndex;
		for(i = 0, nIndex = -1; i < nHeight; i++)
		{
			for(j = 0; j < nWidth; j++)
			{
				int a = pbIsForeground[++nIndex];
				if(a)
				{
					pDibBits[0] = 0;
					pDibBits[1] = 0;
					pDibBits[2] = 0;
					if(a <= 3)
						pDibBits[pbIsForeground[nIndex]-1] = 255;
					else
					{
						pDibBits[0] = 255;
						pDibBits[1] = 255;
					}
					pDibBits += 3;
				}
				else
				{
					*pDibBits++ = pRGBMax[nIndex];
					*pDibBits++ = pRGBMax[nIndex];
					*pDibBits++ = pRGBMax[nIndex];
				}
			}
			pDibBits += nBytesPerLineSourceRemain;
		}

		fwrite(pDib , 1, dwDibSize, fp);
		GlobalFree(pDib);
		fclose(fp);
		return TRUE;
	}

	BOOL LoadBmp(IplImage* pI)
	{
		if(!pI)
			return FALSE;

		// Read in the Dib header and data.
		
		nWidth = pI->width;
		nHeight = pI->height;
		nBytesPerLineSourceRemain = (4 - (nBytesPerLineSource & 3)) & 3;
		nBytesPerLineSource = nWidth * 3 + nBytesPerLineSourceRemain;

		
		pbIsForeground = (int *)GlobalAlloc(GPTR, nWidth*nHeight*sizeof(int));
		pR = (BYTE *)GlobalAlloc(GPTR, nWidth*nHeight);
		pG = (BYTE *)GlobalAlloc(GPTR, nWidth*nHeight);
		pB = (BYTE *)GlobalAlloc(GPTR, nWidth*nHeight);
		pRGBMax = (BYTE *)GlobalAlloc(GPTR, nWidth*nHeight);

		if(!pbIsForeground || !pR || !pG || !pB || !pRGBMax)
		{
			
			if(pbIsForeground)
				GlobalFree(pbIsForeground);
			if(pR)
				GlobalFree(pR);
			if(pG)
				GlobalFree(pG);
			if(pB)
				GlobalFree(pB);
			if(pRGBMax)
				GlobalFree(pRGBMax);
			return FALSE;
		}
		// Point m_pDibBits to the actual Dib bits data.
		BYTE *pDibBits = (BYTE*)(pI->imageData);
		int i, j, nIndex;
		for(i = 0, nIndex = -1; i < nHeight; i++)
		{
			for(j = 0; j < nWidth; j++)
			{
				pB[++nIndex] = *pDibBits++;
				pG[nIndex] = *pDibBits++;
				pR[nIndex] = *pDibBits++;
				pRGBMax[nIndex] = pB[nIndex] > pG[nIndex] ? pB[nIndex] : pG[nIndex];
				pRGBMax[nIndex] = pRGBMax[nIndex] > pR[nIndex] ? pRGBMax[nIndex] : pR[nIndex];
			}
			pDibBits += nBytesPerLineSourceRemain;
		}
		return TRUE;
	}



	int FindConnectedRegion(int *pbIsForeground, SBlock *pBlock, SPoint2D *pPixelPosition, bool *pbIsVisited, int left, int right, int low, int high)
	{
		ZeroMemory(pbIsVisited, nWidth*nHeight);
		int nIndex = 0, nVistiedPixelCount = 0, nBlockCount = 0;
		for(int i = low; i < high; i++)
		{
			nIndex = i*nWidth+left;
			for(int j = left; j < right; j++, nIndex++)
				if(pbIsVisited[nIndex] == 0)
				{
					pbIsVisited[nIndex] = 1;
					SBlock &rnCurrentBlock = pBlock[nBlockCount];
					int nStart = rnCurrentBlock.start = nVistiedPixelCount++;
					pPixelPosition[nStart].x = rnCurrentBlock.x = j;
					pPixelPosition[nStart].y = rnCurrentBlock.y = i;
					rnCurrentBlock.left = rnCurrentBlock.right = j;
					rnCurrentBlock.low = rnCurrentBlock.high = i;
					bool bForeOrBack = pbIsForeground[nIndex] & 255;
					while(nStart < nVistiedPixelCount)
					{
						int jj = pPixelPosition[nStart].x;
						int ii = pPixelPosition[nStart].y;
						for(int k = ii-1; k <= ii+1; k++)
							for(int l = jj-1; l <= jj+1; l++)
								if(k >= low && k < high && l >= left && l < right && !pbIsVisited[k*nWidth+l])
									if((pbIsForeground[k*nWidth+l] & 255) == bForeOrBack)
									{
										pPixelPosition[nVistiedPixelCount].x = l;
										pPixelPosition[nVistiedPixelCount].y = k;
										pbIsVisited[k*nWidth+l] = 1;
										rnCurrentBlock.left = rnCurrentBlock.left < l ? rnCurrentBlock.left : l;
										rnCurrentBlock.right = rnCurrentBlock.right > l ? rnCurrentBlock.right : l;
										rnCurrentBlock.low = rnCurrentBlock.low < k ? rnCurrentBlock.low : k;
										rnCurrentBlock.high = rnCurrentBlock.high > k ? rnCurrentBlock.high : k;
										nVistiedPixelCount++;
									}
									nStart++;  
					} 
					rnCurrentBlock.end = nVistiedPixelCount - 1;
					rnCurrentBlock.area = nVistiedPixelCount - rnCurrentBlock.start;
					nBlockCount++;
				}
		}
		return nBlockCount;
	}


	int ConnectionFilter()
	{
		bool *pbIsVisited = (bool *)GlobalAlloc(GPTR, nWidth*nHeight);
		SBlock *pBlock = (SBlock *)GlobalAlloc(GPTR, nWidth*nHeight*sizeof(SBlock));
		SPoint2D *pPixelPosition = (SPoint2D *)GlobalAlloc(GPTR, nWidth*nHeight*sizeof(SPoint2D));
		int i, j, nBlockCount;
		if(pbIsVisited && pBlock && pPixelPosition)	// 连通过滤
		{
			while(1)
			{
				nBlockCount = FindConnectedRegion(pbIsForeground, pBlock, pPixelPosition, pbIsVisited, 0, nWidth, 0, nHeight);
				bool bNoNewSmallArea = 1;
				for(i = 0; i < nBlockCount; i++)
					if(pBlock[i].area < 600)	// 删除面积过小的区域
					{
						for(j = pBlock[i].start; j <= pBlock[i].end; j++)
							pbIsForeground[pPixelPosition[j].y*nWidth + pPixelPosition[j].x] = !pbIsForeground[pPixelPosition[j].y*nWidth + pPixelPosition[j].x];
						bNoNewSmallArea = 0;
					}
					if(bNoNewSmallArea == 1)
					{	
						if(HVidicom->m_bBoundaryFilter)
						{
							for(i = 0; i < nBlockCount; i++)
							{
								bool bIsBoundary = 0;
								j = pBlock[i].start;
								if(pbIsForeground[pPixelPosition[j].y*nWidth + pPixelPosition[j].x])
								{
									for(; j <= pBlock[i].end; j++)
									{
										bIsBoundary |= pPixelPosition[j].x == 0 || pPixelPosition[j].x == nWidth-1;
										bIsBoundary |= pPixelPosition[j].y == 0 || pPixelPosition[j].y == nHeight-1;
									}
									if(bIsBoundary)
										for(j = pBlock[i].start; j <= pBlock[i].end; j++)
										{
											pbIsForeground[pPixelPosition[j].y*nWidth + pPixelPosition[j].x] = 0;
										}
								}
							}
						}
						break;
					}
			}
		}

		if(pBlock)
			GlobalFree(pBlock);
		if(pPixelPosition)
			GlobalFree(pPixelPosition);
		if(pbIsVisited)
			GlobalFree(pbIsVisited);

		return 1;
	}

	int Process()
	{
		int i, j, nIndex;
		pRatio = (BYTE *)GlobalAlloc(GPTR, nWidth*nHeight*sizeof(BYTE));
		char   temp[MAX_PATH];
		int Choose_BG;

		GetCurrentDirectory(MAX_PATH,temp); 
		
		CString   strTmp; 
		CString   str,c   ; 
		c.Format( "%s",temp); 
		str=c+"\\config.ini ";
		
		Choose_BG=GetPrivateProfileInt( "test ", "qty",4,str);  
		//choose_BG=GetPrivateProfileInt("test","qty",1,"\\abc.ini");
		for(i = 0, nIndex = 0; i < nHeight; i++)
			for(j = 0; j < nWidth; j++, nIndex++)
			{
				double dRatio = 0;
				/*	if(pRGBMax[nIndex] >= 30)
				{
				dRatio = pG[nIndex];// + pR[nIndex];
				dRatio /= pR[nIndex];
				}
				pbIsForeground[nIndex] = (dRatio <= 2.0 || dRatio >= 4.5); // 3.9
				pbIsForeground[nIndex] &= (pR[nIndex] > 15 || pG[nIndex] <= 50); // 3.9	*/



				//底色为绿色
				if (1==Choose_BG)
				{
					if(pR[nIndex] < 140 && pB[nIndex] < 150 && pG[nIndex] > 80)     //底色
					{
						dRatio = pG[nIndex] / (double)pB[nIndex] + pG[nIndex] / (double)pR[nIndex];
					}
					pbIsForeground[nIndex] = (dRatio <= 2.9);// || dRatio >= 4.5);           //1为非底色  0为底色

				}
				
				//底色为红色
				if (2==Choose_BG)
				{
					if(pG[nIndex] < 140 && pB[nIndex] < 150 && pR[nIndex] > 80)     //底色
					{
						dRatio = pR[nIndex] / (double)pB[nIndex] + pR[nIndex] / (double)pG[nIndex];
					}
					pbIsForeground[nIndex] = (dRatio <= 2.9);// || dRatio >= 4.5);           //1为非底色  0为底色
				}
				

				//底色为蓝色
				if (3==Choose_BG)
				{
					if(pR[nIndex] < 173 && pG[nIndex] > 110 && pB[nIndex] > 100)     //底色
					{
						dRatio = pB[nIndex] / (double)pR[nIndex] + pB[nIndex] / (double)pG[nIndex]*(7/5);
					}
					pbIsForeground[nIndex] = (dRatio <= 2.9);// || dRatio >= 4.5);           //1为非底色  0为底色
				}
				
			//底色为白色
				if (4==Choose_BG)
				{
					if(pR[nIndex] >150 && pG[nIndex] > 150 && pB[nIndex] > 150)     //底色
					{
						dRatio = pB[nIndex] / (double)pR[nIndex] + pB[nIndex] / (double)pG[nIndex];
						dRatio=2*dRatio;
					}
					pbIsForeground[nIndex] = (dRatio <= 2.8);// || dRatio >= 4.5);           //1为非底色  0为底色
				}


				if(pRatio)
				{
					dRatio = dRatio * 100 - 100;
					dRatio = dRatio > 0 ? dRatio : 0;
					dRatio = dRatio < 255 ? dRatio : 255;     //确定范围在0-255 
					pRatio[nIndex] = (BYTE)dRatio;
				}
			}
			//char szFileName[256];
			////sprintf(szFileName, "%s%s%s-0.BMP", drive, dir, szMainName);
			//SaveBmp(szFileName, 1, 0);	
			if(pRatio)
				GlobalFree(pRatio);
			ConnectionFilter();
			return 1;
	}
	inline double distance(int i, int j)
	{
		return sqrt(double(nCornerX[i]-nCornerX[j])*(nCornerX[i]-nCornerX[j]) + (nCornerY[i]-nCornerY[j])*(nCornerY[i]-nCornerY[j]));
	}

	// Xcosa + Ysina + C = 0	=>	dX + dYtana = 0
	// 直线上两点(p1,p2)，那么直线方程就是(y1-y2)*X+(x2-x1)*Y+x1*y2-x2*y1=0
	inline void ConnectPointsIntoLine(double x1, double y1, double x2, double y2, double &dAngle, double &dOffset)
	{
		dAngle = atan2(x1-x2, y2-y1);
		dOffset = -x1*cos(dAngle) - y1*sin(dAngle);
	}

	inline void ConnectPointsIntoLine(int nLeftBottomID, int nRightBottomID, double &dAngle, double &dOffset)
	{
		ConnectPointsIntoLine(nCornerX[nLeftBottomID], nCornerY[nLeftBottomID],
			nCornerX[nRightBottomID], nCornerY[nRightBottomID],
			dAngle, dOffset);
	}

	inline double PointLineDistance(double x, double y, double dAngle, double dOffset)
	{
		return x*cos(dAngle) + y*sin(dAngle) + dOffset;
	}

	inline double PointLineDistanceAbsolute(double x, double y, double dAngle, double dOffset)
	{
		return fabs(x*cos(dAngle) + y*sin(dAngle) + dOffset);
	}

	inline double PointLineDistance(int nPointID, double dAngle, double dOffset)
	{
		return PointLineDistance(nCornerX[nPointID], nCornerY[nPointID], dAngle, dOffset);
	}

	inline double PointLineDistanceAbsolute(int nPointID, double dAngle, double dOffset)
	{
		return PointLineDistanceAbsolute(nCornerX[nPointID], nCornerY[nPointID], dAngle, dOffset);
	}

	int ProcessChessboard()
	{
		int nSmallRatio = 2;
		int nThumbnailWidth = nWidth / nSmallRatio;
		int nThumbnailHeight = nHeight / nSmallRatio;
		pRGBMaxThumbnail = (BYTE *)GlobalAlloc(GPTR, nThumbnailWidth*nThumbnailHeight);
		int *pValueThumbnail = (int *)GlobalAlloc(GPTR, nThumbnailWidth*nThumbnailHeight*sizeof(int));
		int nMin = 65536, nMax = 0, nTh;
		nCornerPointCount = 0;
		if(pRGBMaxThumbnail && pValueThumbnail)
		{
			int i, j, nIndex, nIndexThumbnail = 0;
			for(i = 0, nIndex = 0; i < nHeight; i += nSmallRatio)
			{
				nIndex = i*nWidth;
				for(j = 0; j < nWidth; j += nSmallRatio, nIndex += nSmallRatio)
					pRGBMaxThumbnail[nIndexThumbnail++] = pRGBMax[nIndex];
			}
			for(i = 1; i < nThumbnailHeight-1; i++)
				for(j = 1; j < nThumbnailWidth-1; j++)//, nIndex += 4)
				{
					int tmp = abs(pRGBMaxThumbnail[(i+1)*nThumbnailWidth+j-1]
					+ pRGBMaxThumbnail[(i-1)*nThumbnailWidth+j+1]
					- pRGBMaxThumbnail[(i+1)*nThumbnailWidth+j+1]
					- pRGBMaxThumbnail[(i-1)*nThumbnailWidth+j-1]);
					nMin = nMin < tmp ? nMin : tmp;
					nMax = nMax > tmp ? nMax : tmp;
					pValueThumbnail[i*nThumbnailWidth+j] = tmp;
				}
				nTh = (nMax + nMin) / 2;
				for(i = 1; i < nThumbnailHeight-1; i++)
					for(j = 1; j < nThumbnailWidth-1; j++)
						if(pValueThumbnail[i*nThumbnailWidth+j] > nTh)
						{	// 找到大概角点位置
							pbIsForeground[nSmallRatio*i*nWidth+nSmallRatio*j] = 1;
							nCornerX[nCornerPointCount] = nSmallRatio*j;
							nCornerY[nCornerPointCount++] = nSmallRatio*i;
						}
						bool bDistance[10000] = {0};
						for(i = 0; i < nCornerPointCount; i++)
							for(j = 0; j < i; j++)
								bDistance[(int)distance(i, j)] |= 1;

						int nMinGridSize = 0;
						for(i = 9999; i >=0 && bDistance[i] == 0; i--);
						for(j = 0; i >= 0; i--)
							if(bDistance[i])
							{
								nMinGridSize = nMinGridSize > j ? nMinGridSize : j;
								j = 0;
							}
							else
								j++;

						printf("Grid size: %d\t", nMinGridSize);

						int nThreshlod = nMinGridSize/2;
						bool bIsProcessed[10000] = {0};
						int nCornerPointCountGroup[nCornerPointLimit] = {0};
						int nCornerGroupID[nCornerPointLimit][10];
						int nCornerPointCountMerged = 0;
						for(i = 0; i < nCornerPointCount; i++)
						{
							// 按照位置关系合并相邻角点
							if(bIsProcessed[i])
								continue;
							nCornerGroupID[nCornerPointCountMerged][nCornerPointCountGroup[nCornerPointCountMerged]++] = i;
							bIsProcessed[i] = 1;
							for(j = 0; j < nCornerPointCount; j++)
								if(bIsProcessed[j] == 0 && distance(i, j) < nThreshlod)
								{
									bIsProcessed[j] = 1;
									nCornerGroupID[nCornerPointCountMerged][nCornerPointCountGroup[nCornerPointCountMerged]++] = j;
								}
								nCornerPointCountMerged++;
						}
						printf("Corner count: %d\n", nCornerPointCountMerged);

						int nCornerX1[nCornerPointLimit];
						int nCornerY1[nCornerPointLimit];
						for(i = 0; i < nCornerPointCountMerged; i++)
						{
							double dMaxDistance = -1;
							for(j = 0; j < nCornerPointCountGroup[i]; j++)
								for(int k = 0; k < nCornerPointCountGroup[i]; k++)
								{
									int jj = nCornerGroupID[i][j];
									int kk = nCornerGroupID[i][k];
									if(distance(jj, kk) > dMaxDistance)
									{
										dMaxDistance = distance(jj, kk);
										nCornerX1[i] = (nCornerX[jj] + nCornerX[kk]) / 2;
										nCornerY1[i] = (nCornerY[jj] + nCornerY[kk]) / 2;
									}
								}
						}

						for(i = 0; i < nCornerPointCountMerged; i++)
						{	// 得到合并后的角点
							nCornerX[i] = nCornerX1[i];
							nCornerY[i] = nCornerY1[i];
							pbIsForeground[nCornerY[i]*nWidth+nCornerX[i]] = 2;
						}
						nCornerPointCount = nCornerPointCountMerged;

						double dMinDistance1 = 1e12;
						double dMinDistance2 = 1e12;
						double dMinDistance3 = 1e12;
						int nLeftBottomID = 0;
						int nRightBottomID = 0;
						int nLeftTopID = 0;
						for(i = 0; i < nCornerPointCount; i++)	// 得到左下角、右下角和左上角坐标
						{	
							double dDistance2 = nCornerX[i]*nCornerX[i] + nCornerY[i]*nCornerY[i];
							if(dDistance2 < dMinDistance1)
							{
								dMinDistance1 = dDistance2;
								nLeftBottomID = i;
							}
							dDistance2 = (nCornerX[i]-nWidth)*(nCornerX[i]-nWidth) + nCornerY[i]*nCornerY[i];
							if(dDistance2 < dMinDistance2)
							{
								dMinDistance2 = dDistance2;
								nRightBottomID = i;
							}
							dDistance2 = nCornerX[i]*nCornerX[i] + (nCornerY[i]-nHeight)*(nCornerY[i]-nHeight);
							if(dDistance2 < dMinDistance3)
							{
								dMinDistance3 = dDistance2;
								nLeftTopID = i;
							}
						}
						double dAngle1, dOffset1, dAngle2, dOffset2;
						ConnectPointsIntoLine(nLeftBottomID, nRightBottomID, dAngle1, dOffset1);
						ConnectPointsIntoLine(nLeftBottomID, nLeftTopID, dAngle2, dOffset2);

						double dGridSize;
						for(i = 0; i < nCornerPointCount; i++)
						{	
							double dDistanceX = PointLineDistance(i, dAngle2, dOffset2);
							if(dDistanceX > nMinGridSize && dDistanceX < 2*nMinGridSize)
							{
								dGridSize = dDistanceX;
								break;
							}
						}	

						// 复杂数x的算法
						double dPreviousX = 0;
						double dCurrentX = 0;
						int nPreviousX = 0;
						bool bPointExistX;
						do {
							bPointExistX = 0;
							for(i = 0; i < nCornerPointCount; i++)
							{	
								double dDistanceX = PointLineDistanceAbsolute(i, dAngle2, dOffset2) / dGridSize;
								CalibPoint[i].u = nCornerX[i];
								if(fabs(dDistanceX - dPreviousX) < 0.5)
								{
									bPointExistX = 1;
									dCurrentX = dDistanceX > dCurrentX ? dDistanceX : dCurrentX;
									CalibPoint[i].x = nPreviousX;
								}
							}
							dPreviousX = dCurrentX + 1;
							nPreviousX++;
						} while(bPointExistX);

						// 复杂数y的算法
						double dPreviousY = 0;
						double dCurrentY = 0;
						int nPreviousY = 0;
						bool bPointExistY;
						do {
							bPointExistY = 0;
							for(i = 0; i < nCornerPointCount; i++)
							{	
								double dDistanceY = PointLineDistanceAbsolute(i, dAngle1, dOffset1) / dGridSize;
								CalibPoint[i].v = nCornerY[i];
								if(fabs(dDistanceY - dPreviousY) < 0.5)
								{
									bPointExistY = 1;
									dCurrentY = dDistanceY > dCurrentY ? dDistanceY : dCurrentY;
									CalibPoint[i].y = nPreviousY;
								}
							}
							dPreviousY = dCurrentY + 1;
							nPreviousY++;
						} while(bPointExistY);

						// 打印x、y、u和v
						for(i = 0; i < nCornerPointCount; i++)
							printf("u:%.0f\tv:%.0f\tx:%.0f\ty:%.0f\n", CalibPoint[i].u, CalibPoint[i].v, CalibPoint[i].x, CalibPoint[i].y);

						pbIsForeground[nCornerY[nLeftBottomID]*nWidth+nCornerX[nLeftBottomID]] = 3;
						pbIsForeground[nCornerY[nRightBottomID]*nWidth+nCornerX[nRightBottomID]] = 3;
						pbIsForeground[nCornerY[nLeftTopID]*nWidth+nCornerX[nLeftTopID]] = 3;

		}

		if(pRGBMaxThumbnail)
			GlobalFree(pRGBMaxThumbnail);
		if(pValueThumbnail)
			GlobalFree(pValueThumbnail);
		return 1;
	}

	inline double ComputeF(double *m)
	{
		double f = 0;
		for(int i = 0; i < nCornerPointCount; i++)
		{	
			double u = CalibPoint[i].u;
			double v = CalibPoint[i].v;
			double x = CalibPoint[i].x;
			double y = CalibPoint[i].y;
			double a = u*(m[7]*x+m[8]*y+1) - (m[1]*x+m[2]*y+m[3]);
			double b = v*(m[7]*x+m[8]*y+1) - (m[4]*x+m[5]*y+m[6]);
			f += a*a + b*b;
		}
		return f;
	}

	inline double ComputeG(double *m, double &max)
	{
		double g = 0;
		max = 0;
		for(int i = 0; i < nCornerPointCount; i++)
		{	
			double u = CalibPoint[i].u;
			double v = CalibPoint[i].v;
			double x = CalibPoint[i].x;
			double y = CalibPoint[i].y;
			double a = u - (m[1]*x+m[2]*y+m[3]) / (m[7]*x+m[8]*y+1);
			double b = v - (m[4]*x+m[5]*y+m[6]) / (m[7]*x+m[8]*y+1);
			double tmp = a*a + b*b;
			g += tmp;
			max = max > tmp ? max : tmp;
		}
		max = sqrt(max);
		return g;
	}

	inline void ComputePartialFNumerical(double *m, double *PF)
	{
		double mLess[10], mMore[10];
		double eps = 1e-2;
		double less = 1 - eps;
		double more = 1 + eps;
		for(int i = 1; i <= 8; i++)
		{
			memcpy(mLess, m, 9*sizeof(double));
			memcpy(mMore, m, 9*sizeof(double));
			mLess[i] *= less;
			mMore[i] *= more;
			PF[i] = (ComputeF(mMore) - ComputeF(mLess)) / (m[i] * eps * 2);
		}
	}

	inline void ComputePartialFDirect(double *m, double *PF)
	{
		PF[1] = m[1]*XX + m[2]*XY + m[3]*X - m[7]*XXU - m[8]*XYU - XU;
		PF[2] = m[1]*XY + m[2]*YY + m[3]*Y - m[7]*XYU - m[8]*YYU - YU;
		PF[3] = m[1]*X + m[2]*Y + m[3]*nCornerPointCount - m[7]*XU - m[8]*YU - U;
		PF[4] = m[4]*XX + m[5]*XY + m[6]*X - m[7]*XXV - m[8]*XYV - XV;
		PF[5] = m[4]*XY + m[5]*YY + m[6]*Y - m[7]*XYV - m[8]*YYV - YV;
		PF[6] = m[4]*X + m[5]*Y + m[6]*nCornerPointCount - m[7]*XV - m[8]*YV - V;
		PF[7] = m[1]*XXU + m[2]*XYU + m[3]*XU - m[7]*XXUU - m[8]*XYUU - XUU
			+ m[4]*XXV + m[5]*XYV + m[6]*XV - m[7]*XXVV - m[8]*XYVV - XVV;
		PF[8] = m[1]*XYU + m[2]*YYU + m[3]*YU - m[7]*XYUU - m[8]*YYUU -YUU
			+ m[4]*XYV + m[5]*YYV + m[6]*YV - m[7]*XYVV - m[8]*YYVV - YVV;
	}

	inline void ComputeXYUV()
	{
		X = 0, Y = 0, U = 0, V = 0;
		XX = 0, XY = 0, YY = 0;
		XU = 0, YU = 0, XV = 0, YV = 0;
		XXU = 0, XYU = 0, YYU = 0;
		XXV = 0, XYV = 0, YYV = 0;
		XUU = 0, YUU = 0, XVV = 0, YVV = 0;
		XXUU = 0, XYUU = 0, YYUU = 0;
		XXVV = 0, XYVV = 0, YYVV = 0;
		for(int i = 0; i < nCornerPointCount; i++)
		{	
			double u = CalibPoint[i].u;
			double v = CalibPoint[i].v;
			double x = CalibPoint[i].x;
			double y = CalibPoint[i].y;
			X += x, Y += y, U += u, V += v;
			XX += x*x, XY += x*y, YY += y*y;
			XU += x*u, YU += y*u, XV += x*v, YV += y*v;
			XXU += x*x*u, XYU += x*y*u, YYU += y*y*u;
			XXV += x*x*v, XYV += x*y*v, YYV += y*y*v;
			XUU += x*u*u, YUU += y*u*u, XVV += x*v*v, YVV += y*v*v;
			XXUU += x*x*u*u, XYUU += x*y*u*u, YYUU += y*y*u*u;
			XXVV += x*x*v*v, XYVV += x*y*v*v, YYVV += y*y*v*v;
		}	
	}


#define NR_END 1
#define FREE_ARG char*

	inline void free_ivector(int *v, long nl, long nh)
		/* free an int vector allocated with ivector() */
	{
		free((FREE_ARG) (v+nl-NR_END));
	}

	inline void nrerror(char error_text[])
		/* Numerical Recipes standard error handler */
	{
		fprintf(stderr,"Numerical Recipes run-time error...\n");
		fprintf(stderr,"%s\n",error_text);
		fprintf(stderr,"...now exiting to system...\n");
		//	exit(1);
	}

	inline int *ivector(long nl, long nh)
		/* allocate an int vector with subscript range v[nl..nh] */
	{
		int *v;

		v=(int *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(int)));
		if (!v) nrerror("allocation failure in ivector()");
		return v-nl+NR_END;
	}



	bool gaussj(double **a, int n, double **b, int m)
	{
		// 用Gauss-Jordan消去法求解线性方程组AY=B
		// a[1..n][1..n]为输入矩阵
		// b[1..n][1..m]为包含m个右端项向量的输入矩阵
		// 输出时，a被其逆矩阵代替，b被相应的解向量代替
		int *indxc,*indxr,*ipiv;
		int i,icol,irow,j,k,l,ll;
		double big,dum,pivinv,temp;

		indxc=ivector(1,n);
		indxr=ivector(1,n);
		ipiv=ivector(1,n);
		for (j=1;j<=n;j++)
			ipiv[j]=0;
		for (i=1;i<=n;i++)
		{
			big=0.0;
			for (j=1;j<=n;j++)
				if (ipiv[j] != 1)
					for (k=1;k<=n;k++)
					{
						if (ipiv[k] == 0)
						{
							if (fabs(a[j][k]) >= big)
							{
								big=fabs(a[j][k]);
								irow=j;
								icol=k;
							}
						}
						else if (ipiv[k] > 1)
						{
							nrerror("gaussj: Singular Matrix-1");
							return false;
						}
					}
					++(ipiv[icol]);
					if (irow != icol)
					{
						for (l=1;l<=n;l++)
							SWAP(a[irow][l],a[icol][l])
							for (l=1;l<=m;l++)
								SWAP(b[irow][l],b[icol][l])
					}
					indxr[i]=irow;
					indxc[i]=icol;
					if (a[icol][icol] == 0.0)
					{
						nrerror("gaussj: Singular Matrix-2");
						return false;
					}
					pivinv=1.0/a[icol][icol];
					a[icol][icol]=1.0;
					for (l=1;l<=n;l++) a[icol][l] *= pivinv;
					for (l=1;l<=m;l++) b[icol][l] *= pivinv;
					for (ll=1;ll<=n;ll++)
						if (ll != icol)
						{
							dum=a[ll][icol];
							a[ll][icol]=0.0;
							for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
							for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
						}
		}
		for (l=n;l>=1;l--) {
			if (indxr[l] != indxc[l])
				for (k=1;k<=n;k++)
					SWAP(a[k][indxr[l]],a[k][indxc[l]]);
		}
		free_ivector(ipiv,1,n);
		free_ivector(indxr,1,n);
		free_ivector(indxc,1,n);
		return 1;
	}


	bool CalibrationE(double *mE)
	{
		double A[5][3];
		double B[4][3];

		A[1][1] = XX;
		A[2][1] = XY;
		A[3][1] = X;

		A[1][2] = XY;
		A[2][2] = YY;
		A[3][2] = Y;

		A[1][3] = X;
		A[2][3] = Y;
		A[3][3] = nCornerPointCount;

		B[1][1] = XU;
		B[1][2] = XV;
		B[2][1] = YU;
		B[2][2] = YV;
		B[3][1] = U;
		B[3][2] = V;

		double *a[4], *b[4];
		for(int i = 1; i <= 3; i++)
		{
			a[i] = A[i];
			b[i] = B[i];
		}
		if(gaussj(a, 3, b, 2))
		{
			mE[1] = B[1][1];
			mE[2] = B[2][1];
			mE[3] = B[3][1];
			mE[4] = B[1][2];
			mE[5] = B[2][2];
			mE[6] = B[3][2];
			mE[7] = 0;
			mE[8] = 0;
			mE[9] = 1;
			return true;
		}
		return false;
	}

	bool CalibrationF(double *m)
	{
		double A[9][9];
		int i, j;
		for(i = 1; i <= 6; i++)
			for(j = 1; j <= 6; j++)
				A[i][j] = 0;

		A[1][1] = A[4][4] = XX;
		A[1][2] = A[2][1] = A[4][5] = A[5][4] = XY;
		A[2][2] = A[5][5] = YY;
		A[1][3] = A[3][1] = A[4][6] = A[6][4] = X;
		A[2][3] = A[3][2] = A[5][6] = A[6][5] = Y;
		A[3][3] = A[6][6] = nCornerPointCount;

		A[1][7] = -XXU;
		A[1][8] = A[2][7] = -XYU;
		A[2][8] = -YYU;
		A[3][7] = -XU;
		A[3][8] = -YU;

		A[4][7] = -XXV;
		A[4][8] = A[5][7] = -XYV;
		A[5][8] = -YYV;
		A[6][7] = -XV;
		A[6][8] = -YV;

		A[7][1] = XXU;
		A[8][1] = A[7][2] = XYU;
		A[8][2] = YYU;
		A[7][3] = XU;
		A[8][3] = YU;

		A[7][4] = XXV;
		A[8][4] = A[7][5] = XYV;
		A[8][5] = YYV;
		A[7][6] = XV;
		A[8][6] = YV;

		A[7][7] = -XXUU-XXVV;
		A[7][8] = A[8][7] = -XYUU-XYVV;
		A[8][8] = -YYUU-YYVV;

		double B[9][2];
		B[1][1] = XU;
		B[2][1] = YU;
		B[3][1] = U;
		B[4][1] = XV;
		B[5][1] = YV;
		B[6][1] = V;
		B[7][1] = XUU+XVV;
		B[8][1] = YUU+YVV;

		double *a[9], *b[9];
		for(i = 1; i <= 8; i++)
		{
			a[i] = A[i];
			b[i] = B[i];
		}
		gaussj(a, 8, b, 1);
		for(i = 1; i <= 8; i++)
			m[i] = B[i][1];
		m[9] = 1;

		return 1;
	}

	void Calibration()
	{
		ComputeXYUV();

		double m[10];
		int i;

		double PFD[10], PFN[10], PFR[10];

		double mE[10], max;
		CalibrationE(mE);
		ComputePartialFDirect(mE, PFD);
		ComputePartialFNumerical(mE, PFN);

		double RMSE = ComputeF(mE);
		double RMSG = ComputeG(mE, max);
		printf("Max error:%.3f\n", max);

		double mF[10];
		CalibrationF(mF);
		ComputePartialFDirect(mF, PFD);
		ComputePartialFNumerical(mF, PFN);
		RMSG = ComputeG(mF, max);
		printf("Max error:%.3f\n", max);


		memcpy(m, mF, 9*sizeof(double));

		for(i = 0; i < nCornerPointCount; i++)
		{	
			double x = CalibPoint[i].x;
			double y = CalibPoint[i].y;
			int u = (m[1]*x+m[2]*y+m[3]) / (m[7]*x+m[8]*y+1);
			int v = (m[4]*x+m[5]*y+m[6]) / (m[7]*x+m[8]*y+1);
			if(u >= 0 && u < nWidth && v >= 0 && v < nHeight)
				pbIsForeground[v*nWidth+u] = 4;
		}
	}

	IplImage* ProcessBMP(IplImage* pI)
	{
		if(LoadBmp(pI))
		{

			SXF::Process();
			IplImage* pMask = SaveBmp1();
		//	cvShowImage("saf",pMask);
		//	cvWaitKey(0);

			if(pbIsForeground)
				GlobalFree(pbIsForeground);        //释放  指定内存块
			if(pR)
				GlobalFree(pR);
			if(pG)
				GlobalFree(pG);
			if(pB)
				GlobalFree(pB);
			if(pRGBMax)
				GlobalFree(pRGBMax);
			return pMask;
		}
		
	}
	

}