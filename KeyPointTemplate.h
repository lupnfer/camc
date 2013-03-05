#pragma once
#include <vector>
#include "ShapeAnalysis.h"

using namespace cv;
using namespace std;
struct DDMParams
{
	DDMParams() : detectorType("SURF"), descriptorType("SURF"), matcherType("FlannBased") {}
	DDMParams( const string _detectorType, const string _descriptorType, const string& _matcherType ) :
	detectorType(_detectorType), descriptorType(_descriptorType), matcherType(_matcherType){}
	void read( const FileNode& fn )
	{
		fn["detectorType"] >> detectorType;
		fn["descriptorType"] >> descriptorType;
		fn["matcherType"] >> matcherType;
	}
	void write( FileStorage& fs ) const
	{
		fs << "detectorType" << detectorType;
		fs << "descriptorType" << descriptorType;
		fs << "matcherType" << matcherType;
	}
	void print() const
	{
		cout << "detectorType: " << detectorType << endl;
		cout << "descriptorType: " << descriptorType << endl;
		cout << "matcherType: " << matcherType << endl;
	}

	string detectorType;
	string descriptorType;
	string matcherType;
};

struct VocabTrainParams
{
	VocabTrainParams() : trainObjClass("chair"), vocabSize(1000), memoryUse(200), descProportion(0.3f) {}
	VocabTrainParams( const string _trainObjClass, size_t _vocabSize, size_t _memoryUse, float _descProportion ) :
	trainObjClass(_trainObjClass), vocabSize(_vocabSize), memoryUse(_memoryUse), descProportion(_descProportion) {}
	void read( const FileNode& fn )
	{
		fn["trainObjClass"] >> trainObjClass;
		fn["vocabSize"] >> vocabSize;
		fn["memoryUse"] >> memoryUse;
		fn["descProportion"] >> descProportion;
	}
	void write( FileStorage& fs ) const
	{
		fs << "trainObjClass" << trainObjClass;
		fs << "vocabSize" << vocabSize;
		fs << "memoryUse" << memoryUse;
		fs << "descProportion" << descProportion;
	}
	void print() const
	{
		cout << "trainObjClass: " << trainObjClass << endl;
		cout << "vocabSize: " << vocabSize << endl;
		cout << "memoryUse: " << memoryUse << endl;
		cout << "descProportion: " << descProportion << endl;
	}


	string trainObjClass; // Object class used for training visual vocabulary.
	// It shouldn't matter which object class is specified here - visual vocab will still be the same.
	int vocabSize; //number of visual words in vocabulary to train
	int memoryUse; // Memory to preallocate (in MB) when training vocab.
	// Change this depending on the size of the dataset/available memory.
	float descProportion; // Specifies the number of descriptors to use from each image as a proportion of the total num descs.
};
struct KpProb{
	int kpIdx;
	double prob;
	KpProb():kpIdx(-1),prob(0.0){}
	static bool Greater(KpProb& p1,KpProb p2){
		return (p1.prob>p2.prob);
	}
};
struct PartKpDes{
	vector<KeyPoint> keyPoints;
	Mat              kpDescriptors;
};

typedef std::vector<PartKpDes>		KpDesVec;
typedef std::vector<KpProb>			KpProbVec;
typedef std::vector<std::string>	StrVec1D;
typedef std::vector<StrVec1D*>		StrVec2D;

static const double DistThreshold=100.0;
class CKeyPointTemplate
{
public:
	CKeyPointTemplate(void);
	~CKeyPointTemplate(void);
private:
	Ptr<FeatureDetector>			m_featureDetector;
	Ptr<DescriptorExtractor>		m_descriptorExtractor;
	Ptr<DescriptorMatcher>			m_descriptorMatcher;
	Ptr<BOWImgDescriptorExtractor>	m_bowExtractor;

	
	

	Mat                         m_vocabulary;

	int*                        m_matchCounts;
	int                         m_size;
public:
	bool CreateDetectorDescriptorMatcher( );
	bool WriteVocabulary( const string& filename, const Mat& vocabulary );
	bool ReadVocabulary( const string& filename, Mat& vocabulary );

	void ReadUsedParams( const FileNode& fn, string& vocName, DDMParams& ddmParams, VocabTrainParams& vocabTrainParams );
	void WriteUsedParams( FileStorage& fs, const string& vocName, const DDMParams& ddmParams, const VocabTrainParams& vocabTrainParams);

	void TrainVocabulary();

	void DetectKeyPoints(const Mat& image, vector<KeyPoint>& keypoints);
	void ComputeDescriptors(const Mat& image,vector<KeyPoint>& queryKeypoints, Mat& descriptors);
	int Match(Mat& queryDescriptors,vector<DMatch>& matches);
	double Match(Mat& queryDes,Mat& trainDes);
	void DrawKeyPoints(Mat& image,vector<KeyPoint>& keypoints,Scalar color);

	void Add(vector<Mat>& trainDescriptors);
//-----------------实现稳定特征点的选择-----------------------------
public:

	void BeforeTrain();
	void DoTrain();
	void AfterTrain(vector<KeyPoint>& kps,Mat& descriptors);
	Mat  ComputeNormalizedKeyPointsAndDescriptors(IplImage* pImg,IplImage* pMask,vector<KeyPoint>& keypoints,Mat& descriptors);

	void CptProb(vector<KeyPoint>& qryKeyPoints,Mat& qryDes,vector<KeyPoint>& trnKeyPoints,Mat& trnDes,KpProbVec& probs,Mat& desc);
	void ChooseKeyPoints(vector<KeyPoint>& kps,Mat& des,KpProbVec& probs,float num,vector<KeyPoint>& kpsChose,Mat& desChose);


private:
	Mat  SpaceAdjust(vector<DMatch>& matches,vector<KeyPoint>& qryKps,vector<KeyPoint>& trnKps);
	BOOL Ignore(DMatch& dm,int *idxArray,int sz);

private:
	KpProbVec			m_kpProbVec;
	vector<KeyPoint>    m_kps;
	Mat                 m_descriptor;
	Mat					m_partImage;
//--------------------------------------------------------------------

	StrVec2D			m_trainImagesArray;
	

	CShapeAnalysis      m_spAnalysis;

	KpDesVec            m_kpDesVec;
public:
	DDMParams			m_ddmParams;
	VocabTrainParams    m_vocTrainParams;
};
