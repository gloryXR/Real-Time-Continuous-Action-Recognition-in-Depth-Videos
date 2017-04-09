#pragma once

#include <string>
#include <iostream>
#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>
#include <stdio.h>

#define  MAXS  60*1024*1024

#define MSR_TRAIN  0
#define MSR_TEST   1


#define SKE_NUM 19
#define COORD_NUM 3
#define JOINT_NUM 18

#define LENGTH_OF_TRAJECTORY 7

using namespace Eigen;
using namespace std;

