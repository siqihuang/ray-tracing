#include"input.h"
#include<iostream>

#define PI 3.1415926
#define RAD PI/180.0f

input::input(){
	count=0;
	lightNum=0;
}

input::~input(){}

void input::readFile(string fileName){
	string input;
	float x,y,z;
	in.open(fileName);
	while(!in.eof()){
		in>>input;
		if(input=="RESO"){
			in>>width;
			in>>height;
		}
		else if(input=="EYEP"){
			in>>x;in>>y;in>>z;
			EYEP.x=x;EYEP.y=y;EYEP.z=z;
		}
		else if(input=="VDIR"){
			in>>x;in>>y;in>>z;
			VDIR.x=x;VDIR.y=y;VDIR.z=z;
		}
		else if(input=="UVEC"){
			in>>x;in>>y;in>>z;
			UVEC.x=x;UVEC.y=y;UVEC.z=z;
		}
		else if(input=="FOVY"){
			in>>FOVY;
			FOVY=FOVY*RAD;
			FOVY/=2;
		}
		else if(input=="LPOS"){
			in>>x;in>>y;in>>z;
			glm::vec3 v;
			v.r=x;v.g=y;v.b=z;
			LPOS.push_back(v);
		}
		else if(input=="LCOL"){
			in>>x;in>>y;in>>z;
			glm::vec3 v;
			v.r=x;v.g=y;v.b=z;
			LCOL.push_back(v);
			lightNum++;
		}
		else if(input=="NODE"){
			in>>input;
			n.setName(input);
		}
		else if(input=="TRANSLATION"){
			in>>x;in>>y;in>>z;
			n.setTranslation(x,y,z);
		}
		else if(input=="ROTATION"){
			in>>x;in>>y;in>>z;
			x*=RAD;y*=RAD;z*=RAD;
			n.setRotation(x,y,z);
		}
		else if(input=="SCALE"){
			in>>x;in>>y;in>>z;
			n.setScale(x,y,z);
		}
		else if(input=="CENTER"){
			in>>x;in>>y;in>>z;
			n.setCenter(x,y,z);
		}
		else if(input=="PARENT"){
			in>>input;
			n.setParent(input);
		}
		else if(input=="SHAPE"){
			in>>input;
			n.setShape(input);
			if(input=="mesh"){
				in>>input;
				in>>input;
				n.m=readObj(input);
			}

			in>>input;
			if(input=="MAT"){
				in>>input;
				for(int i=0;i<mat.size();i++){
					if(input==mat[i].getName()){
						n.setMatNum(i);
						break;
					}
				}
				if(input==mat[lightIdx].getName()) lightNodeIdx=node.size();
				if(input=="null") n.setMatNum(-1);
			}
			n.setVboCount(count);
			count++;
			n.setRGBA(1,1,1);
			node.push_back(n);
		}
		else if(input=="MAT"){
			glm::vec3 v;
			float f,abso;
			bool b;
			in>>input;
			material m(input);

			in>>input;
			in>>x;in>>y;in>>z;
			v=glm::vec3(x,y,z);
			m.setDIFF(v);
			
			in>>input;
			in>>x;in>>y;in>>z;
			v=glm::vec3(x,y,z);
			m.setREFL(v);

			in>>input;
			in>>f;
			m.setEXPO(f);

			in>>input;
			in>>f;
			m.setIOR(f);

			in>>input;
			in>>b;
			m.setMIRR(b);

			if(m.isMIRR()){
				if(m.getREFL().r>m.getREFL().g) abso=m.getREFL().r;
				else abso=m.getREFL().g;
				if(abso<m.getREFL().b) abso=m.getREFL().b;
			}
			else{
				if(m.getDIFF().r>m.getDIFF().g) abso=m.getDIFF().r;
				else abso=m.getDIFF().g;
				if(abso<m.getDIFF().b) abso=m.getDIFF().b;
			}

			in>>input;
			in>>b;
			m.setTRAN(b);

			in>>input;
			in>>b;
			m.setEMIT(b);

			if(b){
				in>>input;
				in>>f;
				m.setEMITTANCE(f);
			}

			mat.push_back(m);
			if(b){
				lightIdx=mat.size()-1;
				LCOL.push_back(m.getDIFF());
			}
		}
	}
	n.initTree(node);
	LPOS.push_back(glm::vec3(0,0,0));
}

 mesh *input::readObj(string fileName){
	string input;
	bool usVn=false;
	mesh *m=new mesh();
	glm::vec3 vec;
	unsigned int index;
	ifstream inObj;
	inObj.open(fileName);
	while(!inObj.eof()){
		inObj>>input;
		if(input=="v"){
			inObj>>vec.x;
			inObj>>vec.y;
			inObj>>vec.z;
			m->saveVertex(vec);
			//m->addVertex(vec);
			//m->addColor(glm::vec3(0.6,0.6,0.6));
		}
		else if(input=="vn"){
			usVn=true;
			inObj>>vec.x;
			inObj>>vec.y;
			inObj>>vec.z;
			m->saveNormal(vec);
		}
		else if(input=="vt"){}
		else if(input=="f"){
			for(int j=0;j<3;j++){
				int i=0;
				inObj>>input;
				int count=0;
				index=0;
				while(input[count]<='9'&&input[count]>='0') count++;
				for(i=0;i<count;i++)
					index+=(input[i]-'0')*pow(10.0,count-1-i);
				m->addVertexIndex(index-1);
				i++;

				if(i<input.size()){//texture
					int count=i;
					index=0;
					while(input[count]<='9'&&input[count]>='0') count++;
					for(;i<count;i++)
						index+=(input[i]-'0')*pow(10.0,count-1-i);
					m->addTextureIndex(index-1);
					i++;
				}
				if(i<input.size()){//normal
					int count=i;
					index=0;
					while(input[count]<='9'&&input[count]>='0') count++;
					for(;i<count;i++)
						index+=(input[i]-'0')*pow(10.0,count-1-i);
					m->addNormalIndex(index-1);
					i++;
				}
			}
		}
	}
	inObj.close();
	//m->computeNormal(usVn);
	m->averageNormal(usVn);
	m->computeBoundingSphere();
	cout<<"please indicate tree type, 'o' for octree tree, 'k' for kdtree'"<<endl;
	char type;
	cin>>type;
	m->initTreeStructure(type);
	return m;
}

Node *input::getNode(int i){
	return &node[i];
}

unsigned int input::getNodeNum(){
	return node.size();
}

unsigned int input::getWidth(){
	return width;
}

unsigned int input::getHeight(){
	return height;
}

unsigned int input::getCount(){
	return count;
}

unsigned int input::getLightCount(){
	return lightNum;
}

vector<material> input::getMat(){
	return mat;
}

glm::vec3 input::getEYEP(){
	return EYEP;
}

glm::vec3 input::getVDIR(){
	return VDIR;
}

glm::vec3 input::getUVEC(){
	return UVEC;
}

glm::vec3 input::getLPOS(int i){
	return LPOS[i];
}

glm::vec3 input::getLCOL(int i){
	return LCOL[i];
}

float input::getFOVY(){
	return FOVY;
}

void input::moveLight(float distance, char axis){
	if(axis=='x') LPOS[0].x+=distance;
	else if(axis=='y') LPOS[0].y+=distance;
	else if(axis=='z') LPOS[0].z+=distance;
}

void input::resetTraversed(){
	for(int i=0;i<node.size();i++)
		node[i].traversed=0;
}