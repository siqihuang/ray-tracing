#include"kdtree.h"

kdtree::kdtree(){}

kdtree::~kdtree(){}

kdtree::kdtree(int depth,float xmax,float xmin,float ymax,float ymin,float zmax,float zmin,vector<unsigned int> *verIdx){
	this->depth=depth;
	this->xmax=xmax;
	this->xmin=xmin;
	this->ymax=ymax;
	this->ymin=ymin;
	this->zmax=zmax;
	this->zmin=zmin;
	this->verIdx=verIdx;
	mesh=-1;//default nothing
	lc=nullptr;
	rc=nullptr;
	//cout<<"!"<<endl;
}

void kdtree::createTree(vector<glm::vec3> *ver,vector<unsigned int> *idx){
	if(verIdx->size()<=1){//cannot split anymore
		mesh=(*verIdx)[0];
		return;
	}
	sort(ver,idx,depth);
	findBoundary(ver,idx,depth);
	lc=new kdtree(depth+1,lxmax,lxmin,lymax,lymin,lzmax,lzmin,lcVerIdx);
	rc=new kdtree(depth+1,rxmax,rxmin,rymax,rymin,rzmax,rzmin,rcVerIdx);
	lc->createTree(ver,idx);
	rc->createTree(ver,idx);
	//cout<<"!"<<endl;
}

void kdtree::test(kdtree *root){
	if(root->lc!=nullptr) test(root->lc);
	if(root->rc!=nullptr) test(root->rc);
}

void kdtree::quickSort(int low,int high,int half,vector<glm::vec3> *ver,vector<unsigned int> *idx,int depth){
	if(low<high){
		//cout<<"!"<<endl;
		int mid=quickPass(low,high,ver,idx,depth);
		//cout<<low<<","<<mid<<","<<high<<endl;
		//getchar();
		if(mid==half) return;
		else if(mid>half) quickSort(low,mid-1,half,ver,idx,depth);
		else quickSort(mid+1,high,half,ver,idx,depth);
	}
}

int kdtree::quickPass(int low,int high,vector<glm::vec3> *ver,vector<unsigned int> *idx,int depth){
	unsigned int ini;
	float key;
	if(depth%3==0){
		ini=(*verIdx)[low];
		float min;
		int index0,index1,index2;
		index0=(*idx)[3*(*verIdx)[low]];
		index1=(*idx)[3*(*verIdx)[low]+1];
		index2=(*idx)[3*(*verIdx)[low]+2];
			
		if((*ver)[index0].x>(*ver)[index1].x) key=(*ver)[index1].x;
		else key=(*ver)[index0].x;
		if(key>(*ver)[index2].x) key=(*ver)[index2].x;

		while(low<high){
			//if(depth==12) cout<<high<<","<<verIdx->size()<<endl;
			index0=(*idx)[3*(*verIdx)[high]];
			//if(depth==15) cout<<"!"<<endl;
			index1=(*idx)[3*(*verIdx)[high]+1];
			index2=(*idx)[3*(*verIdx)[high]+2];
			if((*ver)[index0].x>(*ver)[index1].x) min=(*ver)[index1].x;
			else min=(*ver)[index0].x;
			if(min>(*ver)[index2].x) min=(*ver)[index2].x;
			//min
			while(low<high&&min>=key){
				high--;
				index0=(*idx)[3*(*verIdx)[high]];
				index1=(*idx)[3*(*verIdx)[high]+1];
				index2=(*idx)[3*(*verIdx)[high]+2];
				if((*ver)[index0].x>(*ver)[index1].x) min=(*ver)[index1].x;
				else min=(*ver)[index0].x;
				if(min>(*ver)[index2].x) min=(*ver)[index2].x;
				//min
			}
			(*verIdx)[low]=(*verIdx)[high];

			index0=(*idx)[3*(*verIdx)[low]];
			index1=(*idx)[3*(*verIdx)[low]+1];
			index2=(*idx)[3*(*verIdx)[low]+2];
			if((*ver)[index0].x>(*ver)[index1].x) min=(*ver)[index1].x;
			else min=(*ver)[index0].x;
			if(min>(*ver)[index2].x) min=(*ver)[index2].x;
			//min
			while(low<high&&min<=key){
				low++;
				index0=(*idx)[3*(*verIdx)[low]];
				index1=(*idx)[3*(*verIdx)[low]+1];
				index2=(*idx)[3*(*verIdx)[low]+2];
				if((*ver)[index0].x>(*ver)[index1].x) min=(*ver)[index1].x;
				else min=(*ver)[index0].x;
				if(min>(*ver)[index2].x) min=(*ver)[index2].x;
				//min
				(*verIdx)[high]=(*verIdx)[low];
			}
			(*verIdx)[low]=ini;
		}
	}
	if(depth%3==1){
		ini=(*verIdx)[low];
		float min;
		int index0,index1,index2;
		index0=(*idx)[3*(*verIdx)[low]];
		index1=(*idx)[3*(*verIdx)[low]+1];
		index2=(*idx)[3*(*verIdx)[low]+2];
			
		if((*ver)[index0].y>(*ver)[index1].y) key=(*ver)[index1].y;
		else key=(*ver)[index0].y;
		if(key>(*ver)[index2].y) key=(*ver)[index2].y;
		//key

		while(low<high){
			index0=(*idx)[3*(*verIdx)[high]];
			index1=(*idx)[3*(*verIdx)[high]+1];
			index2=(*idx)[3*(*verIdx)[high]+2];
			if((*ver)[index0].y>(*ver)[index1].y) min=(*ver)[index1].y;
			else min=(*ver)[index0].y;
			if(min>(*ver)[index2].y) min=(*ver)[index2].y;
			//min

			while(low<high&&min>=key){
				high--;
				index0=(*idx)[3*(*verIdx)[high]];
				index1=(*idx)[3*(*verIdx)[high]+1];
				index2=(*idx)[3*(*verIdx)[high]+2];
				if((*ver)[index0].y>(*ver)[index1].y) min=(*ver)[index1].y;
				else min=(*ver)[index0].y;
				if(min>(*ver)[index2].y) min=(*ver)[index2].y;
				//min
			}
			(*verIdx)[low]=(*verIdx)[high];

			index0=(*idx)[3*(*verIdx)[low]];
			index1=(*idx)[3*(*verIdx)[low]+1];
			index2=(*idx)[3*(*verIdx)[low]+2];
			if((*ver)[index0].y>(*ver)[index1].y) min=(*ver)[index1].y;
			else min=(*ver)[index0].y;
			if(min>(*ver)[index2].y) min=(*ver)[index2].y;
			//min
			while(low<high&&min<=key){
				low++;
				index0=(*idx)[3*(*verIdx)[low]];
				index1=(*idx)[3*(*verIdx)[low]+1];
				index2=(*idx)[3*(*verIdx)[low]+2];
				if((*ver)[index0].y>(*ver)[index1].y) min=(*ver)[index1].y;
				else min=(*ver)[index0].y;
				if(min>(*ver)[index2].y) min=(*ver)[index2].y;
				//min
				(*verIdx)[high]=(*verIdx)[low];
			}
			(*verIdx)[low]=ini;
		}
	}
	if(depth%3==2){
		ini=(*verIdx)[low];
		float min;
		int index0,index1,index2;
		index0=(*idx)[3*(*verIdx)[low]];
		index1=(*idx)[3*(*verIdx)[low]+1];
		index2=(*idx)[3*(*verIdx)[low]+2];
			
		if((*ver)[index0].z>(*ver)[index1].z) key=(*ver)[index1].z;
		else key=(*ver)[index0].z;
		if(key>(*ver)[index2].z) key=(*ver)[index2].z;
		//key

		while(low<high){
			index0=(*idx)[3*(*verIdx)[high]];
			index1=(*idx)[3*(*verIdx)[high]+1];
			index2=(*idx)[3*(*verIdx)[high]+2];
			if((*ver)[index0].z>(*ver)[index1].z) min=(*ver)[index1].z;
			else min=(*ver)[index0].z;
			if(min>(*ver)[index2].z) min=(*ver)[index2].z;
			//min

			while(low<high&&min>=key){
				high--;
				index0=(*idx)[3*(*verIdx)[high]];
				index1=(*idx)[3*(*verIdx)[high]+1];
				index2=(*idx)[3*(*verIdx)[high]+2];
				if((*ver)[index0].z>(*ver)[index1].z) min=(*ver)[index1].z;
				else min=(*ver)[index0].z;
				if(min>(*ver)[index2].z) min=(*ver)[index2].z;
				//min
			}
			(*verIdx)[low]=(*verIdx)[high];

			index0=(*idx)[3*(*verIdx)[low]];
			index1=(*idx)[3*(*verIdx)[low]+1];
			index2=(*idx)[3*(*verIdx)[low]+2];
			if((*ver)[index0].z>(*ver)[index1].z) min=(*ver)[index1].z;
			else min=(*ver)[index0].z;
			if(min>(*ver)[index2].z) min=(*ver)[index2].z;
			//min
			while(low<high&&min<=key){
				low++;
				index0=(*idx)[3*(*verIdx)[low]];
				index1=(*idx)[3*(*verIdx)[low]+1];
				index2=(*idx)[3*(*verIdx)[low]+2];
				if((*ver)[index0].z>(*ver)[index1].z) min=(*ver)[index1].z;
				else min=(*ver)[index0].z;
				if(min>(*ver)[index2].z) min=(*ver)[index2].z;
				//min
				(*verIdx)[high]=(*verIdx)[low];
			}
			(*verIdx)[low]=ini;
		}
	}
	return low;
}

void kdtree::sort(vector<glm::vec3> *ver,vector<unsigned int> *idx,int depth){
	int half=(verIdx->size()+1)/2;
	/*if(depth%3==0){//x turn
		for(int i=0;i<half;i++){
			for(int j=verIdx->size()-1;j>i;j--){
				float min1,min2;
				int index0,index1,index2;
				index0=(*idx)[3*(*verIdx)[j]];
				index1=(*idx)[3*(*verIdx)[j]+1];
				index2=(*idx)[3*(*verIdx)[j]+2];
			
				if((*ver)[index0].x>(*ver)[index1].x) min1=(*ver)[index1].x;
				else min1=(*ver)[index0].x;
				if(min1>(*ver)[index2].x) min1=(*ver)[index2].x;
				//min1
				index0=(*idx)[3*(*verIdx)[j-1]];
				index1=(*idx)[3*(*verIdx)[j-1]+1];
				index2=(*idx)[3*(*verIdx)[j-1]+2];
				if((*ver)[index0].x>(*ver)[index1].x) min2=(*ver)[index1].x;
				else min2=(*ver)[index0].x;
				if(min2>(*ver)[index2].x) min2=(*ver)[index2].x;
				//min2
				if(min1<min2){//switch
					unsigned int temp1=(*verIdx)[j],temp2=(*verIdx)[j-1];
					(*verIdx)[j]=temp2;
					(*verIdx)[j-1]=temp1;
				}
			}
		}
	}
	else if(depth%3==1){//y turn
		for(int i=0;i<half;i++){
			for(int j=verIdx->size()-1;j>i;j--){
				float min1,min2;
				int index0,index1,index2;
				index0=(*idx)[3*(*verIdx)[j]];
				index1=(*idx)[3*(*verIdx)[j]+1];
				index2=(*idx)[3*(*verIdx)[j]+2];
				if((*ver)[index0].y>(*ver)[index1].y) min1=(*ver)[index1].y;
				else min1=(*ver)[index0].y;
				if(min1>(*ver)[index2].y) min1=(*ver)[index2].y;
				//min1
				index0=(*idx)[3*(*verIdx)[j-1]];
				index1=(*idx)[3*(*verIdx)[j-1]+1];
				index2=(*idx)[3*(*verIdx)[j-1]+2];
				if((*ver)[index0].y>(*ver)[index1].y) min2=(*ver)[index1].y;
				else min2=(*ver)[index0].y;
				if(min2>(*ver)[index2].y) min2=(*ver)[index2].y;
				//min2
				if(min1<min2){//switch
					unsigned int temp=(*verIdx)[j];
					(*verIdx)[j]=(*verIdx)[j-1];
					(*verIdx)[j-1]=temp;
				}
			}
		}
	}
	else{//z turn
		for(int i=0;i<half;i++){
			for(int j=verIdx->size()-1;j>i;j--){
				float min1,min2;
				int index0,index1,index2;
				index0=(*idx)[3*(*verIdx)[j]];
				index1=(*idx)[3*(*verIdx)[j]+1];
				index2=(*idx)[3*(*verIdx)[j]+2];
				if((*ver)[index0].z>(*ver)[index1].z) min1=(*ver)[index1].z;
				else min1=(*ver)[index0].z;
				if(min1>(*ver)[index2].z) min1=(*ver)[index2].z;
				//min1
				index0=(*idx)[3*(*verIdx)[j-1]];
				index1=(*idx)[3*(*verIdx)[j-1]+1];
				index2=(*idx)[3*(*verIdx)[j-1]+2];
				if((*ver)[index0].z>(*ver)[index1].z) min2=(*ver)[index1].z;
				else min2=(*ver)[index0].z;
				if(min2>(*ver)[index2].z) min2=(*ver)[index2].z;
				//min2
				if(min1<min2){//switch
					unsigned int temp=(*verIdx)[j];
					(*verIdx)[j]=(*verIdx)[j-1];
					(*verIdx)[j-1]=temp;
				}
			}
		}
	}*/
	quickSort(0,verIdx->size()-1,half,ver,idx,depth);
	//cout<<"@"<<endl;
	lcVerIdx=new vector<unsigned int>;
	rcVerIdx=new vector<unsigned int>;
	for(int i=0;i<half;i++) lcVerIdx->push_back((*verIdx)[i]);
	for(int i=half;i<verIdx->size();i++) rcVerIdx->push_back((*verIdx)[i]);
}

void kdtree::findBoundary(vector<glm::vec3> *ver,vector<unsigned int> *idx,int depth){
	int half=(verIdx->size()+1)/2,index;
	float xmax,xmin,ymax,ymin,zmax,zmin;
	xmax=ymax=zmax=-1e10;xmin=ymin=zmin=1e10;
	for(int i=0;i<half;i++){
		int index0,index1,index2;
		index0=(*idx)[3*(*verIdx)[i]];
		index1=(*idx)[3*(*verIdx)[i]+1];
		index2=(*idx)[3*(*verIdx)[i]+2];
		if((*ver)[index0].x>xmax) xmax=(*ver)[index0].x;
		if((*ver)[index1].x>xmax) xmax=(*ver)[index1].x;
		if((*ver)[index2].x>xmax) xmax=(*ver)[index2].x;
		if((*ver)[index0].x<xmin) xmin=(*ver)[index0].x;
		if((*ver)[index1].x<xmin) xmin=(*ver)[index1].x;
		if((*ver)[index2].x<xmin) xmin=(*ver)[index2].x;
		//x
		if((*ver)[index0].y>ymax) ymax=(*ver)[index0].y;
		if((*ver)[index1].y>ymax) ymax=(*ver)[index1].y;
		if((*ver)[index2].y>ymax) ymax=(*ver)[index2].y;
		if((*ver)[index0].y<ymin) ymin=(*ver)[index0].y;
		if((*ver)[index1].y<ymin) ymin=(*ver)[index1].y;
		if((*ver)[index2].y<ymin) ymin=(*ver)[index2].y;
		//y
		if((*ver)[index0].z>zmax) zmax=(*ver)[index0].z;
		if((*ver)[index1].z>zmax) zmax=(*ver)[index1].z;
		if((*ver)[index2].z>zmax) zmax=(*ver)[index2].z;
		if((*ver)[index0].z<zmin) zmin=(*ver)[index0].z;
		if((*ver)[index1].z<zmin) zmin=(*ver)[index1].z;
		if((*ver)[index2].z<zmin) zmin=(*ver)[index2].z;
		//z
	}
	lxmax=xmax;lxmin=xmin;lymax=ymax;lymin=ymin;lzmax=zmax;lzmin=zmin;
	//lchild
	for(int i=half;i<verIdx->size();i++){
		int index0,index1,index2;
		index0=(*idx)[3*(*verIdx)[i]];
		index1=(*idx)[3*(*verIdx)[i]+1];
		index2=(*idx)[3*(*verIdx)[i]+2];
		if((*ver)[index0].x>xmax) xmax=(*ver)[index0].x;
		if((*ver)[index1].x>xmax) xmax=(*ver)[index1].x;
		if((*ver)[index2].x>xmax) xmax=(*ver)[index2].x;
		if((*ver)[index0].x<xmin) xmin=(*ver)[index0].x;
		if((*ver)[index1].x<xmin) xmin=(*ver)[index1].x;
		if((*ver)[index2].x<xmin) xmin=(*ver)[index2].x;
		//x
		if((*ver)[index0].y>ymax) ymax=(*ver)[index0].y;
		if((*ver)[index1].y>ymax) ymax=(*ver)[index1].y;
		if((*ver)[index2].y>ymax) ymax=(*ver)[index2].y;
		if((*ver)[index0].y<ymin) ymin=(*ver)[index0].y;
		if((*ver)[index1].y<ymin) ymin=(*ver)[index1].y;
		if((*ver)[index2].y<ymin) ymin=(*ver)[index2].y;
		//y
		if((*ver)[index0].z>zmax) zmax=(*ver)[index0].z;
		if((*ver)[index1].z>zmax) zmax=(*ver)[index1].z;
		if((*ver)[index2].z>zmax) zmax=(*ver)[index2].z;
		if((*ver)[index0].z<zmin) zmin=(*ver)[index0].z;
		if((*ver)[index1].z<zmin) zmin=(*ver)[index1].z;
		if((*ver)[index2].z<zmin) zmin=(*ver)[index2].z;
		//z
	}
	rxmax=xmax;rxmin=xmin;rymax=ymax;rymin=ymin;rzmax=zmax;rzmin=zmin;
	//rchild
}