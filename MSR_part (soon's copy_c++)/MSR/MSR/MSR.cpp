#include "Includes.h"
#include <fstream>
#include <vector>
#include "Tools.h"
#include "GetDataHelper.h"
#include "ForestBuilder.h"
#include <ctime>

#define _SCL_SECURE_NO_WARNINGS
#define MIN(a,b) (a<b)?(a):(b)

MatrixXi skeletonJoints(2,JOINT_NUM);
MatrixXi dataSet[2];
MatrixXd trainData;

bool rebuildForest = false;
int forest_num = 5;
int tree_num = 100;
int act_num = 10;
int max_height = 20;

int main()
{   
	skeletonJoints<<1, 2 ,3 , 4 , 5 , 14 ,15 , 5 , 17 , 18 , 3 , 6 , 7 , 8 , 3  ,10 , 11 , 12,
					2, 3 ,4 , 5 , 14 , 15 ,16, 17 , 18, 19 , 6 , 7 , 8 , 9 , 10 ,11 , 12 , 13;
	
	//following part for extracting data
	//MatrixXi extractData;
	//extractData = Tools::readFile("skeleton_data/MSR_ske/1_JiangExperimentFileList.txt");
	//extractData = Tools::loadMatXiFromTxt("skeleton_data/MSR_ske/total_data_index.txt");
	
	//Tools::separateData(dataSet,extractData);

	//Tools::saveMatXi2Txt(dataSet[MSR_TRAIN],"list_train_data.txt");
	//Tools::saveMatXi2Txt(dataSet[MSR_TEST],"list_test_data.txt");
	//-------------------------------------------------------------------------------
	int start = clock();
	printf("1.Prepare dataSet\n");
	dataSet[MSR_TRAIN] = Tools::loadMatXiFromTxt("list_train_data.txt");
	dataSet[MSR_TEST] = Tools::loadMatXiFromTxt("list_test_data.txt");
	printf("  time:%3lf\n",double(clock()-start)/CLOCKS_PER_SEC);
	//-------------------------------------------------------------------------------
	printf("2.Calculate aver\n");
	start = clock();
	MatrixXd aver = Tools::loadMatXdFromTxt("aver.txt");
	//MatrixXd aver = Tools::average_limbs();
	//Tools::saveMatXd2Txt(aver,"aver.txt");
	cout <<aver<<endl;
	printf("  time:%3lf\n",double(clock()-start)/CLOCKS_PER_SEC);

	//-------------------------------------------------------------------------------
	printf("3.Get data\n");
	start = clock();
	GetDataHelper *gd = new GetDataHelper();
	//trainData = gd->getData(dataSet[MSR_TRAIN],LENGTH_OF_TRAJECTORY,aver);
	//Tools::saveMatXd2Txt(trainData,"train_data.txt");
	
	trainData =  Tools::loadMatXdFromTxt("train_data.txt");
	Tools::displaySize(trainData);
	printf("  time:%.3lf\n",double(clock()-start)/CLOCKS_PER_SEC);

	//-------------------------------------------------------------------------------
	printf("4.Build forest\n");
	start = clock();

	ForestBuilder *fd = new ForestBuilder(trainData);
	fd->begin();

	printf("  time:%.3lf\n",double(clock()-start)/CLOCKS_PER_SEC);;
	//-------------------------------------------------------------------------------
	printf("5.predict video\n");
	start = clock();

	//fd->predictVideo(dataSet[MSR_TEST],gd,aver,LENGTH_OF_TRAJECTORY);
	printf("  time:%.3lf\n",double(clock()-start)/CLOCKS_PER_SEC);

	cout<<endl<<fd->predictVideo("skeleton_data/MSR_ske/a02_s01_e01_skeleton3D.txt",gd,aver)<<endl;
	
	delete gd;
	delete fd;
}
