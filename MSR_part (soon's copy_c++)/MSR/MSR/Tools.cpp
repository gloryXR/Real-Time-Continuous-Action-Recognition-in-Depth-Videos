#include "Tools.h"
#include <fstream>

//Matrix Operation


#define  MAX  10000
extern MatrixXi skeletonJoints;
char* Tools::buf = new char[MAXS];

 Matrix<int,Dynamic,3> Tools::readFile(string fn){
	ifstream inFile(fn.c_str());
	if(!inFile){
		cout<<"Unable to open file"<<endl;
		exit(1);
	}
	Matrix<int,Dynamic,3> extractData(0,3);
	
	while(!inFile.eof()){
		string s ;
		getline(inFile,s);
		if(s.length()<10)
			continue;

		MatrixXi temp = extractData;
		extractData.resize(extractData.rows()+1,3);
		extractData<<temp,
					atoi(s.substr(1,2).c_str()),atoi(s.substr(5,2).c_str()),atoi(s.substr(9,2).c_str());
	}
	inFile.close();
	return extractData;
}

void Tools::separateData(MatrixXi dataSet[],MatrixXi extractData){
	for(int i = 0;i<extractData.rows();i++){
		MatrixXi temp;
		if(extractData(i,1)%2 == 1){
			temp = dataSet[MSR_TRAIN];
			dataSet[MSR_TRAIN].resize(dataSet[MSR_TRAIN].rows()+1,3);
			dataSet[MSR_TRAIN]<<temp,
				extractData.row(i);
		}
		else{
			temp = dataSet[MSR_TEST];
			dataSet[MSR_TEST].resize(dataSet[MSR_TEST].rows()+1,3);
			dataSet[MSR_TEST]<<temp,
				extractData.row(i);
		}
	}
}

void Tools::saveMatXi2Txt(MatrixXi &m,string fn){

	FILE *out;
	if ((out = fopen(fn.data(), "wt")) == NULL)
	{
		fprintf(stderr, "创建文件失败.\n");
		return;
	} 
	int row = m.rows(),col = m.cols();
	for(int i = 0;i<row;i++){
		for(int j = 0;j<col;j++)
			if(j!=col-1)
				 fprintf(out,"%d ",m(i,j));
			else
				 fprintf(out,"%d",m(i,j));
		if(i != row-1)
			fprintf(out,"\n");
	}
	fclose(out);
}

void Tools::saveMatXd2Txt(MatrixXd &m,string fn){

	FILE *out;
	if ((out = fopen(fn.data(), "wt")) == NULL)
	{
		fprintf(stderr, "创建文件失败.\n");
		return;
	} 
	int row = m.rows(),col = m.cols();
	for(int i = 0;i<row;i++){
		for(int j = 0;j<col;j++)
			if(j!=col-1)
				 fprintf(out,"%lf ",m(i,j));
			else
				 fprintf(out,"%lf",m(i,j));
		if(row-1 != i)
			fprintf(out,"\n");
	}
	fclose(out);
}

MatrixXd  Tools::loadMatXdFromTxt(string fn){
	FILE *fp;
	int row,col,flag;
	char s[MAX],ch='A';

	if((fp=fopen(fn.data(),"r"))==NULL)
	{
		printf("can not open this file!\n");
		exit(1);
	}

	row=0;
	while(fgets(s,MAX,fp)!=NULL)//读行数
		row++;

	rewind(fp);//回文件起始位置

	col=0;//读列数
	flag=0;
	while(ch!='\n')//(ch=fgetc(fp))!='\n'&&(ch=fgetc(fp))!='\r'
	{
		ch=fgetc(fp);
		if((ch==' '||ch=='\t')&&flag==0)//判断读到的是空格还是tab，并防止多个空格和tab连续出现
		{
			flag=1;
			col++;
		}
		if((ch!=' '&&ch!='\t')&&flag==1)
			flag=0;
	}
	col++;//补上最后一列因为最后一列后面没有空格
	
	fclose(fp);

	freopen(fn.data(),"rt",stdin);
	int len = fread(buf,1,MAXS,stdin);
	buf[len] = '\0';
	return splitBufToMat(buf,len,row,col);
}

MatrixXd Tools::splitBufToMat(char *buf,int len,int row,int col){
	MatrixXd x(row,col);
	int i = 0,j = 0;
	x(i,j) =0;
	int flag = false;  //true 表示小数后
	int sub = 1;
	int zhengfu = 1;
	for (char *p=buf;*p && p-buf<len;p++){
		//printf("ASCII&char:%d,%c \n",*p,*p);
		if(*p == '\n'){
			x(i,j) *= zhengfu;
			zhengfu = 1;
			i++;
			if(i>=row){
				return x;
			
			}
			x(i,j = 0) = 0;
			flag = false;
			sub = 1;
		}
	    else if (*p == ' '){
			x(i,j) *= zhengfu;
			zhengfu = 1;
			x(i,++j) = 0;
			flag = false;
			sub = 1;
		}
		else if(*p == '.')
			flag = true;
		else if(*p == '-'){
			zhengfu = -1;
		}
		else{
			if(flag){
				x(i,j)  +=  (*p - '0')* pow(0.1,sub);
				sub++;
			}
			else
				x(i,j) = x(i,j)*10+*p - '0';
		}
	}
	x(i,j) *= zhengfu;
	
	return x;
}

MatrixXi  Tools::loadMatXiFromTxt(string fn){
	FILE *fp;
	int cols = getFileColumns(fn);
	int rows = getFileRows(fn);
	int tmp;

	if((fp=fopen(fn.data(),"r"))==NULL)
	{
		printf("can not open this file!\n");
		exit(1);
	}

	MatrixXi  loadMat(rows,cols);
	for(int i=0;i<rows;i++)//矩阵读入数据
		for(int j=0;j<cols;j++){
			fscanf(fp,"%d",&loadMat(i,j));
		}

	fclose(fp);
	return loadMat;
}

void Tools::split(const string& src, const string& separator, vector<string>& dest)
{
	string str = src;
	string substring;
	string::size_type start = 0, index;

	do
	{
		index = str.find_first_of(separator,start);
		if (index != string::npos)
		{    
			substring = str.substr(start,index-start);
			dest.push_back(substring);
			start = str.find_first_not_of(separator,index);
			if (start == string::npos) return;
		}
	}while(index != string::npos);
	
	//the last token
	substring = str.substr(start);
	dest.push_back(substring);
}

/**
	 * 从文本中分解数据
	 * @param textLine
	 * @return 文本中分解后的字符数组
	 */
vector<string> Tools::ExtractDataText(string textLine) {
	vector<string> result_vector;
	int start_position = 0;
	int char_count = 0;

	for (int i = 0; i < (int)textLine.length(); i++) {
		if (textLine.at(i) == ' ') {
			result_vector.push_back(
					textLine.substr(start_position, char_count));

			start_position = i + 1;
			char_count = 0;
		} else {
			char_count++;
		}
	}

	result_vector.push_back(
			textLine.substr(
					start_position,
					textLine.length() - start_position));

	return result_vector;
}

MatrixXd Tools::readData(int a,int s,int e){

	char loadName[50];
	sprintf(loadName, "skeleton_data/MSR_ske/a%02d_s%02d_e%02d_skeleton3D.txt", a,s,e); 

	//cout<<loadName<<endl;

	MatrixXd tmpMat = loadMatXdFromTxt(loadName);
	MatrixXd loadMat = tmpMat.leftCols(3);
	int frame_num = loadMat.rows()/SKE_NUM;
	MatrixXd returnMat(frame_num,SKE_NUM*COORD_NUM+1);
	for(int k = 0;k<frame_num;k++){
		MatrixXd subLoadMat = loadMat.middleRows(k*SKE_NUM,SKE_NUM);
		MatrixXd subMat(1,1+SKE_NUM*COORD_NUM);
		subMat(0,0)=a;
		for(int j = 0;j < COORD_NUM;j++)
			for(int i = 0;i < SKE_NUM;i++){
				subMat(0,1+i+j*SKE_NUM) = subLoadMat(i,j);
			}

		returnMat.middleRows(k,1) = subMat;
	}

	return returnMat;
}

MatrixXd Tools::readDataByFileName(string fn){

	MatrixXd tmpMat = loadMatXdFromTxt(fn);
	MatrixXd loadMat = tmpMat.leftCols(3);
	int frame_num = loadMat.rows()/SKE_NUM;
	MatrixXd returnMat(frame_num,SKE_NUM*COORD_NUM+1);
	for(int k = 0;k<frame_num;k++){
		MatrixXd subLoadMat = loadMat.middleRows(k*SKE_NUM,SKE_NUM);
		MatrixXd subMat(1,1+SKE_NUM*COORD_NUM);
		subMat(0,0)=1;
		for(int j = 0;j < COORD_NUM;j++)
			for(int i = 0;i < SKE_NUM;i++){
				subMat(0,1+i+j*SKE_NUM) = subLoadMat(i,j);
			}

		returnMat.middleRows(k,1) = subMat;
	}

	return returnMat;
}

int Tools::getFileColumns(string fileName){  
	ifstream fileStream(fileName.c_str());  

	double tmp;  
	char c;  
	int count=0;  
	for(int i=0;i<10000;i++){  
		fileStream>>tmp;  
		++count;  
		c=fileStream.peek();  
		if('\n'==c)  
		{  
			break;  
		}  
	}  
	fileStream.close();  
	return count;  
}  
int Tools::getFileRows(string fileName){  
	ifstream fileStream(fileName.c_str());  
	string tmp;  
	int count=0;  

	if(fileStream){  
	 while(getline(fileStream,tmp,'\n')){  
		 count++;  
	 }  
	 fileStream.close();}  


	return count ;  
}

void Tools::displaySize(const MatrixXd &m){
	cout<<"row,col:"<<m.rows()<<" "<<m.cols()<<endl;
}

MatrixXd Tools::ske(){
	int sampleObjectNum = 10;
	int sampleFrameNum = 10;
	int sampleActionLabel = 6;
	MatrixXd C (sampleObjectNum*sampleFrameNum*SKE_NUM,COORD_NUM);
	for(int i = 1;i <= sampleObjectNum;++i){
		MatrixXd B = readData(sampleActionLabel,i,1);
		for(int j = 0;j<sampleFrameNum;++j){
			MatrixXd D(SKE_NUM,COORD_NUM);
			D.col(0) = B.block(j,1,1,SKE_NUM).transpose();
			D.col(1) = B.block(j,SKE_NUM+1,1,SKE_NUM).transpose();
			D.col(2) = B.block(j,2*SKE_NUM+1,1,SKE_NUM).transpose();
			C.middleRows((i-1)*sampleFrameNum*SKE_NUM+j*SKE_NUM,SKE_NUM) = D;
		}
	}
	return C;
}
MatrixXd Tools::average_limbs(){
	MatrixXd A = ske();
	MatrixXd aver = MatrixXd::Zero(JOINT_NUM,1);
	int rows = A.rows();
	for(int frame = 0 ;frame < rows/SKE_NUM;++frame){
		int d = SKE_NUM*frame;
		for(int i = 0;i < JOINT_NUM;++i){	
			MatrixXd r = A.row(d+skeletonJoints(0,i)-1)
				-  A.row(d+skeletonJoints(1,i)-1); 
			aver(i,0) += sqrt((r*(r.transpose()))(0,0));	
		}
	}
	aver = aver*SKE_NUM/rows;
	return aver;
}