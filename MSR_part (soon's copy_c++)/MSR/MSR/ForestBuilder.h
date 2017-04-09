#pragma once

#include "Includes.h"
#include "Tools.h"
#include "GetDataHelper.h"
#include <fstream>

#define MAX(a,b) (a>b)?(a):(b)

struct Node{
	bool isLeaf;
	double shang;
	double zengyi;
	
	MatrixXd prob;
	MatrixXd feature;

	Node *left,*right;
	Node(){
		left = NULL;
		right = NULL;}
};

class ForestBuilder{

private :


	
	const int FOREST_NUM ;
	const int PART_NUM;
	const int PART_PRO_NUM;
	const int TREE_NUM;
	const int ACT_NUM;

	const double DATA_RATIO;
	const double FEATURE_RATIO;
	const int MAX_HEIGHT;
	const double THRES; 

	MatrixXi part;
	MatrixXd trainData;
	MatrixXd partTrainData;
	MatrixXd partTrainFeature;

	Node*** forests;

	int nodeCount;

public:
	


	ForestBuilder(MatrixXd &td);

	//~GetDataHelper(void);
	void begin();
	MatrixXd getFeature(MatrixXd &td);
	double var(MatrixXd &m);

	Node** createForest(const MatrixXd &trainData,const MatrixXd &trainFeature,const int &treeNum,
	const double &dataRatio, const double &featureRatio, const int &maxHeight,const double &thres);
	
	//Node* createTree(MatrixXd trainData,MatrixXd trainFeature,int maxHeight,double thres);
	void buildTree( const MatrixXd &trainData,const MatrixXd &trainFeature,const int height,Node* &node);
	MatrixXd filter(const double &ratio,const MatrixXd &x);

	MatrixXd getProb(const MatrixXd &labels,const MatrixXd &weight);
	double getShang(MatrixXd prob);
	double getShang(const MatrixXd &labels,const MatrixXd &weight);
	MatrixXd log2(const MatrixXd &m);

	void splitData(const MatrixXd &trainData,const MatrixXd &trainFeature,MatrixXd &left,MatrixXd &right);

	void predictVideo(const MatrixXi &testDataList,GetDataHelper* &gd,const MatrixXd &aver,const int t);
	MatrixXd getVideoPro(const MatrixXd &testData,const int actNum,Node** &forest);
	MatrixXd getFramePro(const MatrixXd &frameData,const int actNum,Node** &forest);

	void saveForest2Txt(Node** forest,string fn,int num);
	void saveTree2Txt(Node* node,FILE* out);

	void readTree(Node* &node,ifstream &fin);
	void readForestFromTxt(Node** &node,string fn,int num);

	int predictVideo(string fn,GetDataHelper *&gd,MatrixXd &aver);
};