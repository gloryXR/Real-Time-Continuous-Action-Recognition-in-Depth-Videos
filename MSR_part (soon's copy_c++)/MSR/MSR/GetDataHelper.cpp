#include "GetDataHelper.h"


extern MatrixXi skeletonJoints;


GetDataHelper::GetDataHelper(){
	lo.resize(1,30);
	lo<<11,13,30,32,49,51, 7,9,26,28,45,47,  17,19,36,38,55,57,  14,16,33,35,52,54,   1,5,20,24,39,43;
}

MatrixXd GetDataHelper::getData(MatrixXi list,int t,MatrixXd aver){
	

	int cols= 1 + (6*8 + 6*8 + 6*t*2)*5;
	MatrixXd trainData;
	for(int i = 0 ;i<list.rows();i++){
		MatrixXd A = Tools::readData(list(i,0),list(i,1),list(i,2));

		if(A.size()==0)
			continue;
		A = normi(A,aver,skeletonJoints);
	
		int l = A.rows();
		int len = cols;
		if(l-2*t<1)   //unlike matlab,we need to avoid the situation
			continue;
		MatrixXd tempdata= MatrixXd::Ones(l-2*t,len);
		
		int count;
		for(int j = t;j<l-t;j++){      //handle frames  
			count = 0;
			for(int p = 0;p<5;p++){
				for(int k = 0;k<6;k++){
					for(int p1 = 0;p1<5;p1++){
						if(p == p1)
							continue;
						tempdata(j-t,count+1) = A(j,lo(p*6+k))-A(j,lo(p1*6 + int(floor(k/2))*2 + 0));
						tempdata(j-t,count+2) = A(j,lo(p*6+k))-A(j,lo(p1*6 + int(floor(k/2))*2 + 1));
						count += 2;
					}
				}
				//trajectory
				MatrixXd tra;
				for(int k = 0;k<6;k++){
					tra = A.block(j-t,lo(p*6+k),2*t+1,1);
					FFT<double> fft;
					MatrixXcd B(tra.rows(),1);
					B.col(0) = fft.fwd(tra.col(0));
					tempdata.block(j-t,count+1,1,4) = B.middleRows(1,4).real().transpose();
					tempdata.block(j-t,count+5,1,4) = B.middleRows(1,4).imag().transpose();
					count += 8;
				}
				//dynamic feature
				for(int tt = 1;tt<=t;tt++)
					for(int k = 0;k<6;k++){
						tempdata(j-t,count+1) = A(j,lo(p*6+k))-A(j-tt,lo(p*6+k));
						tempdata(j-t,count+2) = A(j,lo(p*6+k))-A(j+tt,lo(p*6+k));
						count += 2; 
					}
				
			}
			 tempdata(j-t,0) = A(j,0); 
		}

		trainData.conservativeResize(trainData.rows()+tempdata.rows(),len);
		trainData.bottomRows(tempdata.rows()) = tempdata;
	
		//MatrixXd tt =trainData.transpose();
		//saveMat2Txt(trainData,"test");
	}

	return trainData;
}

MatrixXd GetDataHelper::getDataFromAFile(string fn,int t,MatrixXd aver){
	

	int cols= 1 + (6*8 + 6*8 + 6*t*2)*5;
	MatrixXd trainData;

		MatrixXd A = Tools::readDataByFileName(fn);

		A = normi(A,aver,skeletonJoints);

		int l = A.rows();
		int len = cols;
		MatrixXd tempdata= MatrixXd::Ones(l-2*t,len);
		
		int count;
		for(int j = t;j<l-t;j++){      //handle frames  
			count = 0;
			for(int p = 0;p<5;p++){
				for(int k = 0;k<6;k++){
					for(int p1 = 0;p1<5;p1++){
						if(p == p1)
							continue;
						tempdata(j-t,count+1) = A(j,lo(p*6+k))-A(j,lo(p1*6 + int(floor(k/2))*2 + 0));
						tempdata(j-t,count+2) = A(j,lo(p*6+k))-A(j,lo(p1*6 + int(floor(k/2))*2 + 1));
						count += 2;
					}
				}
				//trajectory
				MatrixXd tra;
				for(int k = 0;k<6;k++){
					tra = A.block(j-t,lo(p*6+k),2*t+1,1);
					FFT<double> fft;
					MatrixXcd B(tra.rows(),1);
					B.col(0) = fft.fwd(tra.col(0));
					tempdata.block(j-t,count+1,1,4) = B.middleRows(1,4).real().transpose();
					tempdata.block(j-t,count+5,1,4) = B.middleRows(1,4).imag().transpose();
					count += 8;
				}
				//dynamic feature
				for(int tt = 1;tt<=t;tt++)
					for(int k = 0;k<6;k++){
						tempdata(j-t,count+1) = A(j,lo(p*6+k))-A(j-tt,lo(p*6+k));
						tempdata(j-t,count+2) = A(j,lo(p*6+k))-A(j+tt,lo(p*6+k));
						count += 2; 
					}
				
			}
			 tempdata(j-t,0) = A(j,0); 
		}

		trainData.conservativeResize(trainData.rows()+tempdata.rows(),len);
		trainData.bottomRows(tempdata.rows()) = tempdata;

	return trainData;
}

MatrixXd GetDataHelper::normi(MatrixXd A,MatrixXd aver,MatrixXi J){
	MatrixXd B = A;
	int rows = A.rows();
	for(int f = 0;f<rows;f++){
		for(int i = 0;i<JOINT_NUM;i++){
			MatrixXd st(1,3),en(1,3),r(1,3),c(1,3);
			st << A(f,J(0,i)),A(f,J(0,i)+SKE_NUM),A(f,J(0,i)+SKE_NUM*2);
			en << A(f,J(1,i)),A(f,J(1,i)+SKE_NUM),A(f,J(1,i)+SKE_NUM*2);
			r = st-en;
			if(sqrt((r*(r.transpose()))(0,0))==0){
				c << 0,0,0;
			}
			else
				c = -1*aver(i,0)*r/ sqrt((r*(r.transpose()))(0,0));
			B(f,J(1,i))=B(f,J(0,i))+ c(0,0);
			B(f,J(1,i)+SKE_NUM)=B(f,J(0,i)+SKE_NUM)+ c(0,1);
			B(f,J(1,i)+SKE_NUM*2)=B(f,J(0,i)+SKE_NUM*2)+ c(0,2);
		}
	}
	return B;
}
