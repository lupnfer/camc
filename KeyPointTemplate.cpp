#include "StdAfx.h"
#include "KeyPointTemplate.h"
#include "PartsDatabase.h"

CKeyPointTemplate::CKeyPointTemplate(void)
{
	m_matchCounts=NULL;
}

CKeyPointTemplate::~CKeyPointTemplate(void)
{
	if(m_matchCounts)
		delete[] m_matchCounts;
}
bool CKeyPointTemplate::CreateDetectorDescriptorMatcher()
{	
	Ptr<FeatureDetector> pFD(new SurfFeatureDetector(500));
	m_featureDetector = pFD;
	m_descriptorExtractor = DescriptorExtractor::create( m_ddmParams.descriptorType );
	m_descriptorMatcher = DescriptorMatcher::create( m_ddmParams.matcherType );
	bool isCreated = !( m_featureDetector.empty() || m_descriptorExtractor.empty() || m_descriptorMatcher.empty() );
	return isCreated;
}
bool CKeyPointTemplate::WriteVocabulary( const string& filename, const Mat& vocabulary )
{
	FileStorage fs( filename, FileStorage::WRITE );
	if( fs.isOpened() )
	{
		fs << "vocabulary" << vocabulary;
		return true;
	}
	return false;
}
bool CKeyPointTemplate::ReadVocabulary( const string& filename, Mat& vocabulary )
{
	FileStorage fs( filename, FileStorage::READ );
	if( fs.isOpened() )
	{
		fs["vocabulary"] >> vocabulary;
		cout << "done" << endl;
		return true;
	}
	return false;
}
void CKeyPointTemplate::ReadUsedParams( const FileNode& fn, string& vocName, DDMParams& ddmParams, VocabTrainParams& vocabTrainParams )
{
	fn["vocName"] >> vocName;

	FileNode currFn = fn;

	currFn = fn["ddmParams"];
	ddmParams.read( currFn );

	currFn = fn["vocabTrainParams"];
	vocabTrainParams.read( currFn );
	
}

void CKeyPointTemplate::WriteUsedParams( FileStorage& fs, const string& vocName, const DDMParams& ddmParams, const VocabTrainParams& vocabTrainParams)
{
	fs << "vocName" << vocName;

	fs << "ddmParams" << "{";
	ddmParams.write(fs);
	fs << "}";

	fs << "vocabTrainParams" << "{";
	vocabTrainParams.write(fs);
	fs << "}";

}
void CKeyPointTemplate::TrainVocabulary()
{
	if(m_featureDetector.empty()||m_descriptorExtractor.empty())
		return;
	
}
void CKeyPointTemplate::DetectKeyPoints(const Mat& image, vector<KeyPoint>& keypoints)
{
	if(m_featureDetector.empty())
		return;	
	m_featureDetector->detect(image,keypoints);
	
}
void CKeyPointTemplate::ComputeDescriptors(const Mat& image,vector<KeyPoint>& keypoints, Mat& descriptors)
{
	if(m_descriptorExtractor.empty())
		return;
	m_descriptorExtractor->compute(image,keypoints,descriptors);
}
double CKeyPointTemplate::Match(Mat& queryDes,Mat& trainDes)
{
	vector<DMatch> matches;
	m_descriptorMatcher->match(trainDes,queryDes,matches);
	if(matches.empty())
		return DBL_MAX;
	vector<DMatch>::iterator it=matches.begin();
	double diff=0.0;
	for(;it!=matches.end();++it){
		diff=diff+it->distance;
	}
	return diff/(matches.size());
	cv::MSER;
}
int CKeyPointTemplate::Match(Mat& queryDescriptors,vector<DMatch>& matches)
{
	for(int i=0;i<m_size;++i)
		m_matchCounts[i]=0;
	m_descriptorMatcher->match(queryDescriptors,matches);
	vector<DMatch>::iterator it=matches.begin();
	for(;it!=matches.end();++it){
		m_matchCounts[it->imgIdx]++;
	}
	int max=0,idx=-1;
	for(int i=0;i<m_size;++i){
		if(max<m_matchCounts[i]){
			max=m_matchCounts[i];
			idx=i;
		}
	}
	return idx;
}
void CKeyPointTemplate::DrawKeyPoints(Mat& img,vector<KeyPoint>& keypoints,Scalar color)
{
	vector<KeyPoint>::iterator it=keypoints.begin();
	for(;it!=keypoints.end();++it){
		circle(img,Point(it->pt.x,it->pt.y),5,color,2);
	}
}
void CKeyPointTemplate::Add(vector<Mat>& trainDescriptors)
{
	m_descriptorMatcher->add(trainDescriptors);
	if(m_matchCounts)
		delete[] m_matchCounts;
	m_matchCounts=new int[trainDescriptors.size()];
	for(int i=0;i<trainDescriptors.size();++i)
		m_matchCounts[i]=0;
	m_size=trainDescriptors.size();
}
void CKeyPointTemplate::BeforeTrain()
{
	if(!m_trainImagesArray.empty()){
		StrVec2D::iterator it=m_trainImagesArray.begin();
		for(;it!=m_trainImagesArray.end();++it){
			(*it)->clear();
			delete (*it);
		}
		m_trainImagesArray.clear();
	}
	int idxArray[12]={8,16,23,31,36,44,49,58,65,71,77,83};
	int j=0,k=1;
	for(int i=1;i<=12;++i,++k){
		StrVec1D* partImages=new StrVec1D();
		for(;j<=idxArray[i-1];++j){
			std::stringstream strMask,strImg;
			if(i==11){
				k=13;
			}
			
			strMask<<PartsDB->GetCurrentPath()<<"—µ¡∑—˘±æÕºœÒ\\p-m-"<<k<<"-"<<j<<".bmp";
			strImg<<PartsDB->GetCurrentPath()<<"—µ¡∑—˘±æÕºœÒ\\p-ii-"<<k<<"-"<<j<<".bmp";
			partImages->push_back(strImg.str());
			partImages->push_back(strMask.str());
			if(j==55){
				j=59;
				break;
			}
			if(j==78)
			{
				j=81;
			}
		}
		m_trainImagesArray.push_back(partImages);
	}
}
void CKeyPointTemplate::DoTrain()
{
	
	StrVec2D::iterator it2d=m_trainImagesArray.begin();
	int num=0;
	for (;it2d!=m_trainImagesArray.end();++it2d,++num)
	{
		float count=1;
		StrVec1D* pImgStr=(*it2d);
		StrVec1D::iterator it1d=pImgStr->begin();
		IplImage* pImgTmple=cvLoadImage((*it1d).c_str());
		IplImage* pMaskTmple=cvLoadImage((*(it1d+1)).c_str(),CV_LOAD_IMAGE_GRAYSCALE);	
		if(pMaskTmple==NULL)
			continue;
		/*cvShowImage("img",pImgTmple);
		cv::waitKey(0);
		cvShowImage("mask",pMaskTmple);
		cv::waitKey(0);*/

		vector<KeyPoint> keypointsTmple;
		Mat descriptorsTmple;
		Mat roi=ComputeNormalizedKeyPointsAndDescriptors(pImgTmple,pMaskTmple,keypointsTmple,descriptorsTmple);
		KpProbVec probs(keypointsTmple.size());
		KpProbVec::iterator kpit=probs.begin();
		for(int i=0;kpit!=probs.end();++kpit,++i){
			kpit->kpIdx=i;
			kpit->prob=0.0;
		}

		it1d=it1d+2;
		while(it1d!=pImgStr->end())
		{
			++count;
			IplImage* pImgTrain=cvLoadImage((*it1d).c_str());
			IplImage* pMaskTrain=cvLoadImage((*(it1d+1)).c_str(),CV_LOAD_IMAGE_GRAYSCALE);	
			StrVec1D::iterator itTmp=it1d+1;

			it1d=it1d+2;
			vector<KeyPoint> keypointsTrain;
			Mat descriptorsTrain;
			Mat descBackup;
			ComputeNormalizedKeyPointsAndDescriptors(pImgTrain,pMaskTrain,keypointsTrain,descriptorsTrain);
			CptProb(keypointsTmple,descriptorsTmple,keypointsTrain,descriptorsTrain,probs,descBackup);
			
			cvReleaseImage(&pMaskTrain);
			cvReleaseImage(&pImgTrain);
			
			
		}
		PartsInfo ptInfo;
		Mat desc;
		ChooseKeyPoints(keypointsTmple,descriptorsTmple,probs,count,ptInfo.keyPoints,desc);
		//m_kpDesVec.push_back(pkd);
		ComputeDescriptors(roi,ptInfo.keyPoints,ptInfo.kpDescriptors);
		PartsDB->Insert(pImgTmple,pMaskTmple,ptInfo);
		cvReleaseImage(&pMaskTmple);
		cvReleaseImage(&pImgTmple);
		
		/*std::vector<KeyPoint>::iterator it=pkd.keyPoints.begin();
		CvPoint2D32f center={roi.cols/2.0,roi.rows/2.0};
		for(;it!=pkd.keyPoints.end();++it){
			it->pt.x=it->pt.x+center.x;
			it->pt.y=it->pt.y+center.y;
		}
		DrawKeyPoints(roi,pkd.keyPoints,CV_RGB(255,0,0));

		cv::imshow("test",roi);
		cv::waitKey(0);*/
	}

	/*MV_DEL_IMG(pImgDst);
	MV_DEL_IMG(pMskDst);*/
}
void CKeyPointTemplate::AfterTrain(vector<KeyPoint>& kps,Mat& descriptors)
{
	if(m_kpProbVec.empty())
		return;
	std::sort(m_kpProbVec.begin(),m_kpProbVec.end(),KpProb::Greater);
	KpProbVec::iterator it=m_kpProbVec.begin();
	int sz=FMin<int>(FMax<int>(m_kpProbVec.size()*0.6,3),m_kpProbVec.size()); 	
	for(int i=0;i<sz;++i,++it){
		kps.push_back(m_kps[it->kpIdx]);
	}
	ComputeDescriptors(m_partImage,kps,descriptors);
	DrawKeyPoints(m_partImage,kps,Scalar(255,0,0));
	cv::imshow("surf",m_partImage);
	
	cvWaitKey(0);
}
BOOL CKeyPointTemplate::Ignore(DMatch& dm,int* idxArray,int sz)
{
	if(dm.distance==0)
		return TRUE;
	for(int i=0;i<sz;++i){
		if(dm.trainIdx==idxArray[i])
			return TRUE;
	}
	return FALSE;
}
Mat CKeyPointTemplate::SpaceAdjust(vector<DMatch>& matches,
									vector<KeyPoint>& qryKps,vector<KeyPoint>& trnKps)
{	
	
	vector<DMatch>::iterator iit=matches.begin();
	for(int i=0;iit!=matches.end();++iit,++i){
		vector<DMatch>::iterator jit=iit+1;
		for(int j=i+1;jit!=matches.end();++jit,++j){
			vector<DMatch>::iterator kit=jit+1;
			for (int k=j+1;kit!=matches.end();++k,++kit)
			{
				Point2f src[3]={qryKps[iit->queryIdx].pt,qryKps[jit->queryIdx].pt,qryKps[kit->queryIdx].pt};
				Point2f dst[3]={trnKps[iit->trainIdx].pt,trnKps[jit->trainIdx].pt,trnKps[kit->trainIdx].pt};
				Mat warp_mat=cv::getAffineTransform(src,dst);
				
				vector<KeyPoint>::iterator it=qryKps.begin();
				Mat qryMat(qryKps.size(),1,CV_32FC3);
				Mat dstMat(qryKps.size(),1,CV_32FC2);
				for(int r=0;it!=qryKps.end();++it,++r){
					float* Mr=qryMat.ptr<float>(r);	
					Mr[0]=it->pt.x;
					Mr[1]=it->pt.y;
					Mr[2]=1.0;
				}
				cv::transform(qryMat,dstMat,warp_mat);			

			}
		}
	}
//	/*for(int i=0;i<3;++i){
//		double minDiff=DBL_MAX;
//		vector<DMatch>::iterator it=matches.begin();
//		int jTmp=0;
//		for(int j=0;it!=matches.end();++it,++j){
//			if(Ignore(*it,trnIdx,i))
//				continue;
//			if(minDiff>it->distance){
//				minDiff=it->distance;
//				qryIdx[i]=it->queryIdx;
//				trnIdx[i]=it->trainIdx;
//				jTmp=j;
//			}
//		}
//		matches[jTmp].distance=0;
//	}*/
//	Point2f src[3]={qryKps[qryIdx[0]].pt,qryKps[qryIdx[1]].pt,qryKps[qryIdx[2]].pt};
//	Point2f dst[3]={trnKps[trnIdx[0]].pt,trnKps[trnIdx[1]].pt,trnKps[trnIdx[2]].pt};
//	Mat warp_mat=cv::getAffineTransform(src,dst);
////*********************************************************≤‚ ‘¥˙¬Î
//	cv::Mat imgDst;
//	//cv::transform(m_partImage,imgDst,warp_mat);
//	//cvGetQuadrangleSubPix(m_partImage.operator IplImage(),);
//	
//	cv::warpAffine(m_partImage,imgDst,warp_mat,cv::Size(m_partImage.cols*2,m_partImage.rows*2));
//	cv::imshow("surf",imgDst);
//	cvWaitKey(0);
//
////**********************************************************/
//	vector<KeyPoint>::iterator it=qryKps.begin();
//	Mat qryMat(qryKps.size(),1,CV_32FC3);
//	Mat dstMat(qryKps.size(),1,CV_32FC2);
//	for(int r=0;it!=qryKps.end();++it,++r){
//		 float* Mr=qryMat.ptr<float>(r);	
//		 Mr[0]=it->pt.x;
//		 Mr[1]=it->pt.y;
//		 Mr[2]=1.0;
//	}
//	cv::transform(qryMat,dstMat,warp_mat);
	Mat dstMat;
	return dstMat;
}

//double
//CKeyPointTemplate::compareSURFDescriptors( const float* d1, const float* d2, double best, int length )
//{
//	double total_cost = 0;
//	assert( length % 4 == 0 );
//	for( int i = 0; i < length; i += 4 )
//	{
//		double t0 = d1[i] - d2[i];
//		double t1 = d1[i+1] - d2[i+1];
//		double t2 = d1[i+2] - d2[i+2];
//		double t3 = d1[i+3] - d2[i+3];
//		total_cost += t0*t0 + t1*t1 + t2*t2 + t3*t3;
//		if( total_cost > best )
//			break;
//	}
//	return total_cost;
//}
//
//
//int
//CKeyPointTemplate::naiveNearestNeighbor( const float* vec, int laplacian,
//					                     const CvSeq* model_keypoints,
//					                     const CvSeq* model_descriptors )
//{
//	int length = (int)(model_descriptors->elem_size/sizeof(float));
//	int i, neighbor = -1;
//	double d, dist1 = 1e6, dist2 = 1e6;
//	CvSeqReader reader, kreader;
//	cvStartReadSeq( model_keypoints, &kreader, 0 );
//	cvStartReadSeq( model_descriptors, &reader, 0 );
//
//	for( i = 0; i < model_descriptors->total; i++ )
//	{
//		const CvSURFPoint* kp = (const CvSURFPoint*)kreader.ptr;
//		const float* mvec = (const float*)reader.ptr;
//		CV_NEXT_SEQ_ELEM( kreader.seq->elem_size, kreader );
//		CV_NEXT_SEQ_ELEM( reader.seq->elem_size, reader );
//		if( laplacian != kp->laplacian )
//			continue;
//		d = compareSURFDescriptors( vec, mvec, dist2, length );
//		if( d < dist1 )
//		{
//			dist2 = dist1;
//			dist1 = d;
//			neighbor = i;
//		}
//		else if ( d < dist2 )
//			dist2 = d;
//	}
//	if ( dist1 < 0.6*dist2 )
//		return neighbor;
//	return -1;
//}
//
//void
//CKeyPointTemplate::findPairs( const CvSeq* objectKeypoints, const CvSeq* objectDescriptors,
//		                      const CvSeq* imageKeypoints, const CvSeq* imageDescriptors, vector<int>& ptpairs )
//{
//	int i;
//	CvSeqReader reader, kreader;
//	cvStartReadSeq( objectKeypoints, &kreader );
//	cvStartReadSeq( objectDescriptors, &reader );
//	ptpairs.clear();
//
//	for( i = 0; i < objectDescriptors->total; i++ )
//	{
//		const CvSURFPoint* kp = (const CvSURFPoint*)kreader.ptr;
//		const float* descriptor = (const float*)reader.ptr;
//		CV_NEXT_SEQ_ELEM( kreader.seq->elem_size, kreader );
//		CV_NEXT_SEQ_ELEM( reader.seq->elem_size, reader );
//		int nearest_neighbor = naiveNearestNeighbor( descriptor, kp->laplacian, imageKeypoints, imageDescriptors );
//		if( nearest_neighbor >= 0 )
//		{
//			ptpairs.push_back(i);
//			ptpairs.push_back(nearest_neighbor);
//		}
//	}
//}

Mat CKeyPointTemplate::ComputeNormalizedKeyPointsAndDescriptors(IplImage* pImg,IplImage* pMask,
																   vector<KeyPoint>& keypoints,Mat& descriptors)
{
	Mat roi;
	if(!pImg||!pMask)
		return roi;	
	IplConvKernel* pElement=cvCreateStructuringElementEx(21,21,10,10,CV_SHAPE_ELLIPSE);
	cvDilate(pMask,pMask,pElement);
	cvReleaseStructuringElement(&pElement);


	IplImage* pImgDst=MV_NEW_IMG(pImg->width,pImg->height,pImg->depth,pImg->nChannels);
	IplImage* pMskDst=MV_NEW_IMG(pMask->width,pMask->height,pMask->depth,pMask->nChannels);

	m_spAnalysis.Rotate(pImg,pMask,pImgDst,pMskDst);

	//********************************************************************

	//**********************************************************************/
/********************************************************************
	cvShowImage("img",pImgDst);
	cvWaitKey(0);
//**********************************************************************/
	cv::Mat img(pImgDst,TRUE);
	
	pElement=cvCreateStructuringElementEx(5,5,2,2,CV_SHAPE_ELLIPSE);
	cvMorphologyEx(pMskDst,pMskDst,NULL,pElement,CV_MOP_CLOSE);
	cvReleaseStructuringElement(&pElement);

	pElement=cvCreateStructuringElementEx(17,17,8,8,CV_SHAPE_ELLIPSE);
	cvErode(pMskDst,pMskDst,pElement);
	cvReleaseStructuringElement(&pElement);

	/*cvShowImage("img",pMskDst);
	cvWaitKey(0);*/

	CBlobResult br;
	br.BwLabeling(pMskDst,NULL,0);
	br.Filter(br,B_EXCLUDE,CBlobGetArea(), B_LESS,1000);
	CBlob* pBlob=br.GetBlob(0);
	CvRect rc=pBlob->GetBoundingBox();
	rc.x=FMax<int>(0,rc.x-8);
	rc.y=FMax<int>(0,rc.y-8);
	rc.width=FMin<int>(rc.width+17,pImg->width-rc.x);
	rc.height=FMin<int>(rc.height+17,pImg->height-rc.y);
	roi=Mat(img,Rect(rc));
	Mat retImg=roi.clone();
	DetectKeyPoints(roi,keypoints);
	ComputeDescriptors(roi,keypoints,descriptors);
	
/********************************************************************
	DrawKeyPoints(roi,keypoints,CV_RGB(255,0,0));
	cv::imshow("img",img);
	cvWaitKey(0);
//**********************************************************************/

	std::vector<KeyPoint>::iterator it=keypoints.begin();
	CvPoint2D32f center={pBlob->GetMinBoundingRect().size.width/2,pBlob->GetMinBoundingRect().size.height/2};
	for(;it!=keypoints.end();++it){
		it->pt.x=it->pt.x-center.x;
		it->pt.y=it->pt.y-center.y;
	}
	
	MV_DEL_IMG(pImgDst);
	MV_DEL_IMG(pMskDst);

	return retImg;
}
void CKeyPointTemplate::CptProb(vector<KeyPoint>& qryKeyPoints,
								  Mat& qryDes,vector<KeyPoint>& trnKeyPoints,Mat& trnDes,KpProbVec& probVec,Mat& desc)
{
	vector<DMatch> matches;
	m_descriptorMatcher->match(qryDes,trnDes,matches);
	vector<KeyPoint>::iterator qryIt=qryKeyPoints.begin();

	vector<DMatch>::iterator it=matches.begin();
	KpProbVec::iterator kpIt=probVec.begin();
	for(;it!=matches.end();++it,++qryIt,++kpIt){
		kpIt->kpIdx=it->queryIdx;
		Point2f qryPt=qryIt->pt;
		Point2f trnPt=trnKeyPoints[it->trainIdx].pt;
		double dis=(qryPt.x-trnPt.x)*(qryPt.x-trnPt.x)+(qryPt.y-trnPt.y)*(qryPt.y-trnPt.y);
		if(dis<=DistThreshold){
			kpIt->prob+=1.0;			
		}
		if(!desc.empty())
			desc.row(it->queryIdx)=desc.row(it->queryIdx)+trnDes.row(it->trainIdx);
	}
}
void 
CKeyPointTemplate::ChooseKeyPoints(vector<KeyPoint>& kps,Mat& des,
								    KpProbVec& probs,float num,vector<KeyPoint>& kpsChose,Mat& desChose)

{
	//std::sort(probs.begin(),probs.end(),KpProb::Greater);
	double T=num/3.0;
	KpProbVec::iterator it=probs.begin();
	int i=0;
	for(;it!=probs.end();++it){
		if(it->prob>=T){
			kpsChose.push_back(kps[it->kpIdx]);
			++i;
		}
	}
	desChose.create(i,des.rows,des.type());
	it=probs.begin();
	for(i=0;it!=probs.end();++it){
		if(it->prob>=T){
			desChose.row(i)=des.row(it->kpIdx);
			++i;
		}
	}
}