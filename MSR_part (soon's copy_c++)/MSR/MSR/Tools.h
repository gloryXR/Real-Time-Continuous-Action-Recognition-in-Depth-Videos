#pragma once

#include "Includes.h"
#include <vector>
#include <sstream>

class Tools{
public :
	static char *buf ;
	static Matrix<int,Dynamic,3> readFile(string fn);
	static void separateData(MatrixXi dataSet[],MatrixXi extractData);
	static void saveMatXi2Txt(MatrixXi &m,string fn);
	static void saveMatXd2Txt(MatrixXd &m,string fn);

	static void  loadtest(string fn);
	static vector<string> ExtractDataText(string textLine);
	static MatrixXi loadMatXiFromTxt(string fn);
	static MatrixXd loadMatXdFromTxt(string fn);
	static MatrixXd splitBufToMat(char *buf,int len,int row,int col);



	static void split(const string& src, const string& separator, vector<string>& dest);

	static MatrixXd readData(int a,int s,int e);
	static MatrixXd readDataByFileName(string fn);

	static int getFileColumns(string fileName);
	static int getFileRows(string fileName);

	static void displaySize(const MatrixXd &m);

	static MatrixXd ske();
	static MatrixXd average_limbs();
};



