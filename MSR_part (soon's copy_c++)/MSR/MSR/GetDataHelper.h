#pragma once

#include "Includes.h"
#include "Tools.h"
#include <cmath>



class GetDataHelper{
public:
	MatrixXi lo;
	
	GetDataHelper(void);
	//~GetDataHelper(void);

	MatrixXd getData(MatrixXi list,int t,MatrixXd aver);
	MatrixXd getDataFromAFile(string fn,int t,MatrixXd aver);
	MatrixXd normi(MatrixXd A,MatrixXd aver,MatrixXi J);
};