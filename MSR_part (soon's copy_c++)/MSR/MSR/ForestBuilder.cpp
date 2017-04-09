#include "ForestBuilder.h"

extern bool rebuildForest;

extern int forest_num;
extern int tree_num;
extern int act_num;
extern int max_height;

ForestBuilder::ForestBuilder(MatrixXd &td):
	FOREST_NUM(forest_num),
	PART_NUM(5),
	PART_PRO_NUM(180),
	TREE_NUM (tree_num),
	ACT_NUM (act_num),
	DATA_RATIO (0.65),
	FEATURE_RATIO (0.1),
	MAX_HEIGHT (max_height),
	THRES (0.00001) 
{
	trainData = td;
	part = Tools::loadMatXiFromTxt("part.txt");
	nodeCount = 0;
}

void ForestBuilder::begin(){
	forests = new Node**[FOREST_NUM];
	for(int r = 0;r<FOREST_NUM;++r){

		int row = trainData.rows();
		partTrainData.resize(row,1);
		partTrainData.col(0) = trainData.col(0);

		for(int i = 0;i<PART_NUM;++i){
			if(part(r,i)!=0){
				partTrainData.conservativeResize(row,partTrainData.cols()+PART_PRO_NUM);
				partTrainData.rightCols(PART_PRO_NUM) = trainData.middleCols(1+PART_PRO_NUM*(part(r,i)-1),PART_PRO_NUM);
			}
			else
				break;
		}
		
		partTrainData.conservativeResize(partTrainData.rows(),partTrainData.cols()+1);
		partTrainData.rightCols(1) = MatrixXd::Ones(partTrainData.rows(),1);
		char c[40];
		sprintf(c,"trainData/partTrainData%02d.txt",r);
		//saveMat2Txt(partTrainData,c);
		//printf("saving %s\n",c);

		partTrainFeature = getFeature(partTrainData);
		sprintf(c,"trainData/partTrainFeature%02d.txt",r);
		//saveMat2Txt(partTrainFeature,c);
		//printf("saving %s\n",c);

		printf("forest %02d ",r);
		sprintf(c,"forest/forest%02d.txt",r);
		
		if(!rebuildForest)
			readForestFromTxt(forests[r],c,TREE_NUM);
		else{
			forests[r] = createForest(partTrainData,partTrainFeature,TREE_NUM,DATA_RATIO,FEATURE_RATIO,MAX_HEIGHT,THRES);
			saveForest2Txt(forests[r],c,TREE_NUM);
		}
	}
		
}

Node** ForestBuilder::createForest(const MatrixXd &trainData,const MatrixXd &trainFeature,const int &treeNum,const double &dataRatio,
				 const double &featureRatio,const int &maxHeight,const double &thres){

	Node** forest = new Node*[treeNum];
	printf("creating tree ");
	for(int i = 0;i<treeNum;++i){
		MatrixXd fTrainData = filter(dataRatio,trainData);
		MatrixXd fTrainFeature = filter(featureRatio,trainFeature);
		printf("%d ",i);
		buildTree(fTrainData,fTrainFeature,0,forest[i]);
	}
	printf("\n");
	return forest;
}

void ForestBuilder::buildTree(const MatrixXd &trainData, const MatrixXd &trainFeature,const int height,Node* &node){
	MatrixXd prob = getProb(trainData.col(0),trainData.rightCols(1));
	
	double shang = getShang(prob);
	if(height == MAX_HEIGHT||shang < THRES){
		node = new Node();
		node->isLeaf = true;
		node->shang = shang;
		node->prob = prob;
		return;
	}
	double max = 0;
	int maxIndex = 0;
	for(int j = 0;j<trainFeature.rows();++j){
		MatrixXd left,right;
		splitData(trainData,trainFeature.row(j),left,right);
		if(left.rows() == 0 || right.rows() == 0)
			continue;
		double Temp = shang - (left.rows()*getShang(left.col(0),left.rightCols(1)) + right.rows()* getShang(right.col(0),right.rightCols(1)))/trainData.rows();
	
		if(Temp >max){
			max = Temp;
			maxIndex = j;
		}
	}
	if(max<THRES){
		node = new Node();
		node->isLeaf = true;
		node->shang = shang;
		node->prob = prob;
		return;
	}
	node = new Node();
	node->isLeaf = false;
	node->feature = trainFeature.row(maxIndex);
	node->zengyi = max;
	MatrixXd right,left;
	splitData(trainData,trainFeature.row(maxIndex),left,right);
	buildTree(left,trainFeature,height+1,node->left);
	buildTree(right,trainFeature,height+1,node->right);
}

void ForestBuilder::splitData(const MatrixXd &trainData,const MatrixXd &trainFeature,MatrixXd &left,MatrixXd &right){
	int label = (int)trainFeature(0,0);
	int col = trainData.cols();
	int row = trainData.rows();
	left.resize(row,col);
	right.resize(row,col);
	int leftRowCount = 0,rightRowCount = 0;
	for(int i = 0;i<row;++i){
		double temp = trainData(i,label+1);
		if(temp < trainFeature(0,1)){
			left.row(leftRowCount) = trainData.row(i);
			++leftRowCount;
		}
		else{
			right.row(rightRowCount) = trainData.row(i);
			++rightRowCount;
		}
	}
	left.conservativeResize(leftRowCount,col);
	right.conservativeResize(rightRowCount,col);
}

double ForestBuilder::getShang(const MatrixXd &labels, const MatrixXd &weight){

	MatrixXd prob = getProb(labels,weight);
	double shang = (prob*-1*(log2(prob.array()+0.0000000001).transpose()))(0,0);

	return MAX(shang,0);
}
double ForestBuilder::getShang(MatrixXd prob){
	double shang = (prob*-1*(log2(prob.array()+0.0000000001).transpose()))(0,0);
	return MAX(shang,0);
}
MatrixXd ForestBuilder::getProb(const MatrixXd &labels,const MatrixXd &weight){
	//we now ignore weight
	int num  = (int)labels.maxCoeff();
	MatrixXd N = MatrixXd::Zero(1,num);
	for(int j = 0;j<labels.rows();++j)
		N((int)labels(j,0)-1)+=1;
	return N/(N.sum());
}
MatrixXd ForestBuilder::log2(const MatrixXd &m){
	MatrixXd tmp = m;
	/*m = m.array().log();
	m /= log(2);*/
	tmp = tmp.array().log(); 
	tmp /= log(2);
	return tmp;
}

MatrixXd ForestBuilder::filter(const double &ratio,const MatrixXd &x){
	int row = x.rows();
	int col = x.cols();
	MatrixXd R = MatrixXd::Random(row,1);
	R.array()+=1;
	R/=2;
	for(int i = 0;i<row;++i){
		if(R(i,0)<ratio)
			R(i,0) = 1;
		else
			R(i,0) = 0;
	}
	MatrixXd m (row,col);
	int count = 0;
	for(int i = 0;i<row;++i){
		if(R(i,0)==1){
			m.row(count) = x.row(i);
			count++;
		}
	}
	m.conservativeResize(count,col);
	return m;
}

MatrixXd ForestBuilder::getFeature(MatrixXd &td){
	double arrayX[] = {-1.65,-1.04,-0.68,-0.39,-0.13,0.13,0.39,0.68,1.04,1.65};
	MatrixXd X = MatrixXd::Map(arrayX,1,10);
	//cout<<"X："<<X<<endl;
	int len = td.cols();
	MatrixXd train_feature = MatrixXd::Zero(10*(len-2),2);  
	MatrixXd temp,XX;
	double u,delta;
	for(int k = 0;k<len-2;++k){
		temp = td.col(k+1);
		u = temp.mean();
		delta = var(temp);
		XX = sqrt(delta)*X;
		XX.array() += u;
		MatrixXd K(10,1);K.fill(k);
		train_feature.block(k*10,0,10,1) = K;
		train_feature.block(k*10,1,10,1) = XX.transpose();
	}
	return train_feature;
}

double ForestBuilder::var(MatrixXd &m){
	double u = m.mean();
	double sum = 0;
	for(int i = 0;i<m.rows();++i)
		for(int j = 0;j<m.cols();++j)
			sum += pow(m(i,j)-u,2);
	return sum/((m.rows()*m.cols())-1);
}

void ForestBuilder::predictVideo(const MatrixXi &testDataList,GetDataHelper* &gd,const MatrixXd &aver,const int t){
	MatrixXd testData;
	MatrixXi resultMat(testDataList.rows(),2);
	MatrixXd videoPro = MatrixXd::Zero(testDataList.rows(),ACT_NUM);
	MatrixXd allStateMat(FOREST_NUM,4);
	int accu = 0;
	for(int r = 0;r<FOREST_NUM;++r){
		accu = 0;
		for(int i = 0;i<testDataList.rows();++i){
			testData = gd->getData(testDataList.row(i),t,aver);
			MatrixXd tempData(testData.rows(),testData.cols());
			int colCount = 1;
			for(int p = 0;p<PART_NUM;++p){
				if(part(r,p)!=0){
					tempData.middleCols(colCount,PART_PRO_NUM) = testData.middleCols(1+PART_PRO_NUM*(part(r,p)-1),PART_PRO_NUM);
					colCount+=PART_PRO_NUM;
				}
				else
					break;
			}
			tempData.conservativeResize(testData.rows(),colCount);
			
			int r_max,c_max;
			
			videoPro.row(i)= getVideoPro(tempData,ACT_NUM,forests[r]);
			videoPro.row(i).maxCoeff(&r_max,&c_max);
			resultMat(i,0) = testDataList(i,0);
			resultMat(i,1) = c_max+1;
			if(resultMat(i,0) == resultMat(i,1))
				++accu;
		}
		char c[40];
		sprintf(c,"proResult/videoPro%02d.txt",r);
		Tools::saveMatXd2Txt(videoPro,c);

		sprintf(c,"proResult/resultMat%02d.txt",r);
		Tools::saveMatXi2Txt(resultMat,c);
		cout <<"forest"<<r<<" : predicts "<<accu<<" right in "<<resultMat.rows()<<" with accuracy of "<<float(accu)/float(resultMat.rows())<<endl;

		allStateMat(r,0) = r;
		allStateMat(r,1) = accu;
		allStateMat(r,2) = resultMat.rows();
		allStateMat(r,3) = float(accu)/float(resultMat.rows());
	}
	char c[40];
	sprintf(c,"allState.txt");
	Tools::saveMatXd2Txt(allStateMat,c);
}

MatrixXd ForestBuilder::getVideoPro(const MatrixXd &testData,const int actNum,Node** &forest){
	MatrixXd proDistribution = MatrixXd::Zero(testData.rows(),actNum);
	for(int i = 0;i<testData.rows();++i)
		proDistribution.row(i) = getFramePro(testData.row(i),actNum,forest);
	Tools::saveMatXd2Txt(proDistribution,"proDistribution.txt");
	return proDistribution.colwise().sum();
}
MatrixXd ForestBuilder::getFramePro(const MatrixXd &frameData,const int actNum,Node** &forest){
	MatrixXd proo =MatrixXd::Zero(TREE_NUM,actNum);
	for(int i = 0;i<TREE_NUM;++i){
		Node* node = forest[i];
		while(!(node->isLeaf)){
			MatrixXd feature = node->feature;
			if(frameData(0,(int)feature(0,0)+1)<feature(0,1)){
				node = node->left;
			}
			else{
				node = node->right;
			}
		}
		proo.block(i,0,1,node->prob.cols()) = node->prob;
		//cout<<node->prob<<endl;
	}

	//-------------***----------unfinished
	return proo.colwise().sum()/TREE_NUM;
}

void ForestBuilder::saveTree2Txt(Node* node,FILE* out){
	if(!node)
		fprintf(out,"# ");
	else{
		MatrixXd x;
		int col;
		if(node->isLeaf){
			fprintf(out,"^ ");  //represents leaf node
			x = node->prob;
		}
		else{
			fprintf(out,"* ");  //represents intermediate node
			x = node->feature;
		}
		col = x.cols();
		fprintf(out,"%d ",col);
		for(int i = 0;i<col;i++)
			fprintf(out,"%lf ",x(0,i));   
		saveTree2Txt(node->left,out);
		saveTree2Txt(node->right,out);
	}
}

void ForestBuilder::saveForest2Txt(Node** forest,string fn,int num){
	FILE *out;
	if ((out = fopen(fn.data(), "wt")) == NULL)
	{
		fprintf(stderr, "创建文件失败.\n");
		return;
	}
	for(int i = 0;i<num;i++){
		saveTree2Txt(forest[i],out);
		//fprintf(out,")"); //represents the end of the tree  
		fprintf(out,"\n");
	}
	
	fclose(out);
}

void ForestBuilder::readTree(Node* &node,ifstream &fin){
	char c;
	fin>>c;
	if(c == '#')
		node = NULL;
	else {
		node = new Node();
		if(c == '*'){
			node -> isLeaf = false;
			int col;
			double tmp;
			fin>>col;
			MatrixXd m(1,col);
			for(int i = 0;i<col;i++){
				fin>>tmp;
				m(0,i) = tmp;
			}
			node->feature = m;
		}
		else if(c == '^'){
			node -> isLeaf = true;
			int col;
			double tmp;
			fin>>col;
			MatrixXd m(1,col);
			for(int i = 0;i<col;i++){
				fin>>tmp;
				m(0,i) = tmp;
			}
			node->prob = m;
		}
		readTree(node->left,fin);
		readTree(node->right,fin);
	}
}

void ForestBuilder::readForestFromTxt(Node** &node,string fn,int num){
	ifstream fin(fn.c_str());
	node = new Node*[num];
	for(int i = 0;i<num;i++){
		readTree(node[i],fin);
	}
	fin.close();
}

int ForestBuilder::predictVideo(string fn,GetDataHelper *&gd,MatrixXd &aver){
	int t = 7;
	MatrixXd testData;
	MatrixXi resultMat(1,2);
	MatrixXd videoPro = MatrixXd::Zero(1,ACT_NUM);
	int accu = 0;
	for(int r = 0;r<FOREST_NUM;++r){
			testData = gd->getDataFromAFile(fn,t,aver);
			MatrixXd tempData(testData.rows(),testData.cols());
			int colCount = 1;
			for(int p = 0;p<PART_NUM;++p){
				if(part(r,p)!=0){
					tempData.middleCols(colCount,PART_PRO_NUM) = testData.middleCols(1+PART_PRO_NUM*(part(r,p)-1),PART_PRO_NUM);
					colCount+=PART_PRO_NUM;
				}
				else
					break;
			}
			tempData.conservativeResize(testData.rows(),colCount);
			//testData = tempData;
			int r_max,c_max;
			//(getVideoPro(tempData,ACT_NUM,forests[r]).colwise().sum()).maxCoeff(&r,&c);
			videoPro.row(0)= getVideoPro(tempData,ACT_NUM,forests[r]);
			videoPro.row(0).maxCoeff(&r_max,&c_max);
			
			return c_max+1;
	}
	return 0;
}
