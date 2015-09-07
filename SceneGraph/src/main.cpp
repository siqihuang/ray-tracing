// NOTE: This definition forces GLM to use radians (not degrees) for ALL of its
// angle arguments. The documentation may not always reflect this fact.
// YOU SHOULD USE THIS IN ALL FILES YOU CREATE WHICH INCLUDE GLM
#define GLM_FORCE_RADIANS
#define MAX_ITER 5
#define SAMPLE_NUM 1.0
#define LIGHT_SAMPLE 1.0
#define ABSO_PROB 0.2
#define MC_ITER 2000
//open mp
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glew/glew.h"
#include <GL/glut.h>

#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

#include "Geometry.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "CUBE.h"
#include "input.h"
#include "tests.h"
#include "Ray.h"
#include "EasyBMP.h"
#include "material.h"

static const float PI = 3.141592653589f;


// Vertex arrays needed for drawing
unsigned int *vboPos;
unsigned int *vboCol;
unsigned int *vboNor;
unsigned int *vboIdx;

// Attributes
unsigned int locationPos;
unsigned int locationCol;
unsigned int locationNor;

// Uniforms
unsigned int unifModel;
unsigned int unifModelInvTr;
unsigned int unifViewProj;
unsigned int unifLightPos;
unsigned int unifLightColor;
unsigned int unifCameraPos;

// Needed to compile and link and use the shaders
unsigned int shaderProgram;

// Window dimensions, change if you want a bigger or smaller window
unsigned int windowWidth = 640;
unsigned int windowHeight = 480;

// Animation/transformation stuff
clock_t old_time;
float rotation = 0.0f;
bool blur=false;

input in;
Node *root,*p,*q,*n,*light;
glm::vec3 lightColor;
unsigned int nodeNum;
BMP SampleOutput;

// TESTING GEOMETRY
Geometry* geometry;

glm::vec3 **background,**group;
glm::vec3 A,B,H,V,M;

void sampleUploadSphere(Node *p);
void sampleUploadSquare(Node *p);
void sampleUploadCylinder(Node *p);
void sampleUploadCube(Node *p);
void sampleUploadMesh(Node *p);
void sampleDrawObject(glm::mat4 models, int faces);

// Helper function to read shader source and put it in a char array
// thanks to Swiftless
std::string textFileRead(const char*);

// Some other helper functions from CIS 565 and CIS 277
void printLinkInfoLog(int);
void printShaderInfoLog(int);
void printGLErrorLog();

// Standard glut-based program functions
void init(void);
void resize(int, int);
void display(void);
void keypress(unsigned char, int, int);
void mousepress(int button, int state, int x, int y);
void cleanup(void);

void initShader();
void cleanupShader();

// modified from CIS 277
Node *preorderTraverse(Node *p);
void uploadObject(Node *p);
void createGeometry();
void drawObject(glm::mat4 modelmat);

void initRay();
void outputImage();
Intersection intersectObj(glm::vec3 EYEP,glm::vec3 dir);
glm::vec3 rayTrace(glm::vec3 ori,glm::vec3 dir,glm::vec3 trans,int depth,bool inside);
bool insidePoint(glm::vec3 ori,glm::vec3 des,float t);
void output(int n);

volatile DWORD dwStart;
volatile int global = 0;

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
	//srand((unsigned)time(NULL));
    // Use RGBA double buffered window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	//RunTests();

	string filename;
	//cout<<"please input the file name"<<endl;
	//cin>>filename;
	//in.readFile(filename);
	in.readFile("txt/hw3c_cornell.txt");
	char flag;
	cout<<"please indicate if using motion blur, y/n"<<endl;
	cin>>flag;
	if(flag=='y') blur=true;

	initRay();
	root=in.getNode(0);
	p=root;
	q=root;
	n=root;
	light=in.getNode(in.lightNodeIdx);
	lightColor=in.getLCOL(0);

	nodeNum=in.getNodeNum();

    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Scene Graph");

    glewInit();

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keypress);
    glutMouseFunc(mousepress);
    //glutIdleFunc(display);

    glutMainLoop();
    return 0;
}

void initRay(){
	A=glm::cross(in.getVDIR(),in.getUVEC());
	B=glm::cross(A,in.getVDIR());
	V=glm::normalize(B);
	V=V*tan(in.getFOVY());
	V=V*glm::length(in.getVDIR());
	H=glm::normalize(A);
	H=H*tan(in.getFOVY());
	H=H*(float)(1.0*in.getWidth()/in.getHeight());
	H=H*glm::length(in.getVDIR());
	M=in.getEYEP()+in.getVDIR();

	group=new glm::vec3*[in.getHeight()];
	for(int i=0;i<in.getHeight();i++)
		group[i]=new glm::vec3[in.getWidth()];

	background=new glm::vec3*[in.getHeight()];
	for(int i=0;i<in.getHeight();i++)
		background[i]=new glm::vec3[in.getWidth()];
	for(int i=0;i<in.getHeight();i++)
		for(int j=0;j<in.getWidth();j++)
			background[i][j]=glm::vec3(0,0,0);
}

void outputImage(){
	SampleOutput.SetSize(in.getWidth(),in.getHeight());
	dwStart=GetTickCount();
	float sample=SAMPLE_NUM;
	for(int m=0;m<MC_ITER;m++){
		srand((unsigned)time(NULL)^omp_get_thread_num()^m);
		#pragma omp parallel for
		for(int i=0;i<in.getHeight();i++){
			for(int j=0;j<in.getWidth();j++){
				glm::vec3 finalColor=glm::vec3(0,0,0);
				for(int k=0;k<sample;k++){
					float sx,sy,dt=1.0/sample;
					glm::vec3 v;
					sx=(1.0*j-0.5+dt*k)/(in.getWidth()-1);
					sy=(1.0*i-0.5+dt*k)/(in.getHeight()-1);
					group[in.getHeight()-i-1][j]=M+H*(2*sx-1)+V*(2*sy-1);

					v=group[in.getHeight()-i-1][j]-in.getEYEP();
					v=glm::normalize(v);
					finalColor=finalColor+rayTrace(in.getEYEP(),v,glm::vec3(1,1,1),0,false);
				}
				//cout<<finalColor.x<<","<<finalColor.y<<","<<finalColor.z<<endl;
				//getchar();
				background[in.getHeight()-1-i][j]=background[in.getHeight()-1-i][j]+finalColor/sample;
			}
			global++;
			if(global%800==0) cout<<100.0*global/in.getHeight()<<"% complete"<<endl;
		}
		if(m%10==0) output(m+1);
		if(blur){
			glm::mat4 model=glm::translate(glm::mat4(1),glm::vec3(-0.05,0,0));
			n->setModel(model,'t');
		}
	}
	cout<<"run time: "<<GetTickCount()-dwStart<<" ms"<<endl;
	output(MC_ITER);
}

void output(int n){
	char buffer[10];
	itoa(n/10+1,buffer,10);
	string name=string("seq/SAMPLE")+buffer+".bmp";
	for(int i=0;i<in.getHeight();i++){
		for(int j=0;j<in.getWidth();j++){
			float r,g,b;
			r=background[i][j].r/n;
			g=background[i][j].g/n;
			b=background[i][j].b/n;
			if(r>1) r=1;
			if(g>1) g=1;
			if(b>1) b=1;
			//SampleOutput(j,i)->Red=(int)abs(background[i][j].r*255);
			//SampleOutput(j,i)->Green=(int)abs(background[i][j].g*255);
			//SampleOutput(j,i)->Blue=(int)abs(background[i][j].b*255);
			SampleOutput(j,i)->Red=(int)abs(r*255);
			SampleOutput(j,i)->Green=(int)abs(g*255);
			SampleOutput(j,i)->Blue=(int)abs(b*255);
		}
	}
	cout<<"complete"<<endl;
	//SampleOutput.WriteToFile(name.c_str());
	SampleOutput.WriteToFile("SAMPLE.bmp");
}

glm::vec3 rayTrace(glm::vec3 ori,glm::vec3 dir,glm::vec3 trans,int depth,bool inside){
	material mat;
	glm::vec3 ambi,diff,spec,refr,interPoint,dir_ref,dir_light,color;
	spec=diff=glm::vec3(0,0,0);
	Intersection inter,int_light;
	if(depth>MAX_ITER){
		return glm::vec3(0,0,0);//to be determined
	}
	//max depth
	inter=intersectObj(ori,dir);
	if(inter.t==-1){
		return glm::vec3(0,0,0);
	}
	//if not intersect
	mat=in.getMat()[inter.mat];
	interPoint=ori+dir*inter.t;
	//get material and get interPoint
	if(mat.isMIRR()) trans=trans*mat.getREFL();
	else if(!mat.isTRAN()) trans=trans*mat.getDIFF();
	if(1-mat.getABSO()>1e-3) trans=trans/(1-mat.getABSO());

	if(inter.t==-2){
		diff=lightColor*mat.getEMITTANCE()*trans;
		spec=diff;
		return diff;
	}


	ambi=mat.getDIFF();
	//ambient color
	if(mat.isMIRR()&&mat.isTRAN()){
		glm::vec3 mirr,refr;
		float wMirr=0.85;
		dir_ref=dir-inter.normal*(glm::dot(dir,inter.normal)*2);
		dir_ref=glm::normalize(dir_ref);
		mirr=rayTrace(interPoint,dir_ref,trans,depth+1,false);

		float ref_rate=mat.getIOR(),dt;
		if(!inside) ref_rate=1.0/ref_rate;
		dt=1-ref_rate*ref_rate*(1-pow(glm::dot(inter.normal,dir),2));
		if(dt<0){
			dir_ref=dir-inter.normal*(glm::dot(dir,inter.normal)*2);
			dir_ref=glm::normalize(dir_ref);
			refr=rayTrace(interPoint,dir_ref,trans,depth+1,inside);
		}
		else{
			glm::vec3 dir_refr;
			dir_refr=(-ref_rate*glm::dot(inter.normal,dir)-sqrt(dt))*inter.normal+dir*ref_rate;
			dir_refr=glm::normalize(dir_refr);
			//refr=rayTrace(interPoint,dir_refr,depth+1,!inside);
			refr=rayTrace(interPoint,dir_refr,trans,depth+1,!inside);
		}
		diff=wMirr*refr+(1-wMirr)*mirr;
		spec=diff;
		ambi=spec;
	}
	else if(mat.isMIRR()){
		dir_ref=dir-inter.normal*(glm::dot(dir,inter.normal)*2);
		dir_ref=glm::normalize(dir_ref);
		diff=rayTrace(interPoint,dir_ref,trans,depth+1,false);
		//if(insidePoint(interPoint,in.getLPOS(),int_light.t)) spec=diff;
		spec=diff*mat.getREFL();
		diff=spec;
		ambi=spec;
	}
	else if(mat.isTRAN()){
		float ref_rate=mat.getIOR(),dt;
		if(!inside) ref_rate=1.0/ref_rate;
		dt=1-ref_rate*ref_rate*(1-pow(glm::dot(inter.normal,dir),2));
		if(dt<0){
			cout<<"!"<<endl;
			dir_ref=dir-inter.normal*(glm::dot(dir,inter.normal)*2);
			dir_ref=glm::normalize(dir_ref);
			diff=rayTrace(interPoint,dir_ref,trans,depth+1,inside);
			
			if(!mat.isMIRR()){
				spec=diff;
				ambi=diff;
			}
		}
		else{
			//cout<<"@"<<endl;
			glm::vec3 dir_refr;
			dir_refr=(-ref_rate*glm::dot(inter.normal,dir)-sqrt(dt))*inter.normal+dir*ref_rate;
			dir_refr=glm::normalize(dir_refr);
			//refr=rayTrace(interPoint,dir_refr,depth+1,!inside);
			diff=rayTrace(interPoint,dir_refr,trans,depth+1,!inside);
			if(!mat.isMIRR()){
				spec=diff;
				ambi=diff;
			}
		}
	}
	else{//not hit the light
		float count=LIGHT_SAMPLE;
		float ran=static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		if(ran<mat.getABSO()*(1+depth*0.5)) return glm::vec3(0,0,0);//absorbed
		for(unsigned int i=0;i<LIGHT_SAMPLE;i++){
			glm::vec3 tspec,tdiff,lightPos;
			if(light->getShape()=="sphere"){//sphere light
				lightPos=light->getRandomPointOnSphere(light);
			}
			else{//cube light
				lightPos=light->getRandomPointOnCube(light);
				//cout<<lightPos.x<<","<<lightPos.y<<","<<lightPos.z<<","<<"out"<<endl;
				//getchar();
			}
			dir_light=lightPos-interPoint;
			dir_light=glm::normalize(dir_light);
			//light vector
			//int_light=intersectObj(interPoint,dir_light);
			//if(glm::dot(inter.normal,dir_light)>0&&(int_light.t==-1||insidePoint(interPoint,lightPos,int_light.t))){
				float cd;
				glm::vec3 randRef=light->getCosineWeightedDirection(inter.normal);
				tspec=rayTrace(interPoint,randRef,trans,depth+1,false);
				tdiff=tspec;
				//diffuse color
			//}
			//else{
				//tspec=glm::vec3(0,0,0);
				//tdiff=glm::vec3(0,0,0);
			//}
			//if(tspec.r!=0||tspec.g!=0||tspec.b!=0){
				spec=spec+tspec;
			//}
			//if(tdiff.r!=0||tdiff.g!=0||tdiff.b!=0){
				diff=diff+tdiff;
			//}
		}
		//if(count!=0){
			diff=diff/count;
			spec=spec/count;
		//}
	}
	//specular color

	float r,g,b;
	r=0.4*diff.r+0.6*spec.r;
	g=0.4*diff.g+0.6*spec.g;
	b=0.4*diff.b+0.6*spec.b;
	color=glm::vec3(r,g,b);
	
	return color;
}

bool insidePoint(glm::vec3 ori,glm::vec3 des,float t){
	float length;
	length=glm::length(ori-des);
	//if(length<0.01) cout<<"!"<<endl;
	if(length>t&&t>0.0001) return false;
	else{
		return true;
	}
}

Intersection intersectObj(glm::vec3 EYEP,glm::vec3 dir){
	Node *point=root;
	string name;
	Intersection inter,temp;
	Geometry *geometry;
	inter.t=-1;
	int mat;
	geometry=point->obj;
	if(geometry!=nullptr){
		inter=geometry->intersect(point->getModel('m'),Ray(EYEP,dir));
		mat=point->getMatNum();
		name=point->getName();
		if(inter.t<1e-3) inter.t=-1;
	}
	point=preorderTraverse(point);
	while(point!=root){
		geometry=point->obj;
		temp=geometry->intersect(point->getModel('m'),Ray(EYEP,dir));
		if(temp.t>1e-3&&(temp.t<inter.t||inter.t<0)){
			inter=temp;
			mat=point->getMatNum();
			name=point->getName();
		}
		point=preorderTraverse(point);
	}
	if(inter.t==-1) return inter;//no intersect
	if(name==in.getNode(in.lightNodeIdx)->getName()) inter.t=-2;//intersect with light
	inter.mat=mat;
	inter.normal=glm::normalize(inter.normal);
	return inter;
}

Node *preorderTraverse(Node *p){
	p->traversed=1;//already traversed
	if(p->firstChild!=nullptr) p=p->firstChild;
	else if(p->nextBrother!=nullptr) p=p->nextBrother;
	else{
		while(p->p!=nullptr&&p->nextBrother==nullptr)
			p=p->p;
		if(p->p==nullptr) in.resetTraversed();
		else p=p->nextBrother;
	}
	return p;
}

void init()
{
    // Create the VBOs and vboIdx we'll be using to render images in OpenGL
	int count=in.getCount();
	vboPos=new unsigned int[count];
	vboCol=new unsigned int[count];
	vboNor=new unsigned int[count];
	vboIdx=new unsigned int[count];
	for(int i=0;i<count;i++){
		glGenBuffers(1,&vboPos[i]);
		glGenBuffers(1,&vboCol[i]);
		glGenBuffers(1,&vboNor[i]);
		glGenBuffers(1,&vboIdx[i]);
	}
	
    // Set the color which clears the screen between frames
	glClearColor(0, 0, 0, 1);
    // Enable and clear the depth buffer
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1);
    glDepthFunc(GL_LEQUAL);

    // Set up our shaders here
    initShader();
	glm::mat4 modelmat;
	createGeometry();
	geometry=p->obj;
	if(geometry!=nullptr){
		geometry->setColor(p->getRGBA());

		modelmat=glm::mat4();
		modelmat=glm::translate(modelmat,p->getTranslation());
		p->setModel(modelmat,'t');

		modelmat=glm::mat4();
		modelmat=glm::rotate(modelmat,(float)p->getRotation().x/180*PI,glm::vec3(1,0,0));
		modelmat=glm::rotate(modelmat,(float)p->getRotation().y/180*PI,glm::vec3(0,1,0));
		modelmat=glm::rotate(modelmat,(float)p->getRotation().z/180*PI,glm::vec3(0,0,1));
		//modelmat=glm::rotate(glm::mat4(1),45.0f/180*PI,glm::vec3(0,0,1));
		p->setModel(modelmat,'r');

		modelmat=glm::mat4();
		modelmat=glm::scale(modelmat,p->getScale());
		p->setModel(modelmat,'s');
		uploadObject(p);
	}
	p=preorderTraverse(p);

	while(p!=root){
		createGeometry();
		geometry=p->obj;
		geometry->setColor(p->getRGBA());

		modelmat=(p->p->getModel('m'));
		modelmat=glm::translate(modelmat,p->getTranslation());
		p->setModel(modelmat,'t');

		modelmat=(p->p->getModel('r'));
		modelmat=glm::rotate(modelmat,p->getRotation().x,glm::vec3(1,0,0));
		modelmat=glm::rotate(modelmat,p->getRotation().y,glm::vec3(0,1,0));
		modelmat=glm::rotate(modelmat,p->getRotation().z,glm::vec3(0,0,1));
		p->setModel(modelmat,'r');

		modelmat=(p->p->getModel('s'));
		modelmat=glm::scale(modelmat,p->getScale());
		p->setModel(modelmat,'s');

		uploadObject(p);
		p=preorderTraverse(p);
	}
    //resize(windowWidth, windowHeight);
	resize(in.getWidth(),in.getHeight());
	old_time = clock();
}

void createGeometry(){
	if(p->getShape()=="sphere")
		p->obj=new Sphere();
	else if(p->getShape()=="cylinder")
		p->obj=new Cylinder();
	else if(p->getShape()=="cube")
		p->obj=new Cube();
	else if(p->getShape()=="mesh"){
		p->obj=p->m;
	}
}

void uploadObject(Node *p){
	if(p->getShape()=="sphere")
		sampleUploadSphere(p);
	else if(p->getShape()=="cylinder")
		sampleUploadCylinder(p);
	else if(p->getShape()=="cube")
		sampleUploadCube(p);
	else if(p->getShape()=="mesh")
		sampleUploadMesh(p);
}

void initShader()
{
    // Read in the shader program source files
    std::string vertSourceS = textFileRead("shaders/diff.vert.glsl");
    const char *vertSource = vertSourceS.c_str();
    std::string fragSourceS = textFileRead("shaders/diff.frag.glsl");
    const char *fragSource = fragSourceS.c_str();
	
    // Tell the GPU to create new shaders and a shader program
    GLuint shadVert = glCreateShader(GL_VERTEX_SHADER);
    GLuint shadFrag = glCreateShader(GL_FRAGMENT_SHADER);
    shaderProgram = glCreateProgram();

    // Load and compiler each shader program
    // Then check to make sure the shaders complied correctly
    // - Vertex shader
    glShaderSource    (shadVert, 1, &vertSource, NULL);
    glCompileShader   (shadVert);
    printShaderInfoLog(shadVert);
    // - Diffuse fragment shader
    glShaderSource    (shadFrag, 1, &fragSource, NULL);
    glCompileShader   (shadFrag);
    printShaderInfoLog(shadFrag);

    // Link the shader programs together from compiled bits
    glAttachShader  (shaderProgram, shadVert);
    glAttachShader  (shaderProgram, shadFrag);
    glLinkProgram   (shaderProgram);
    printLinkInfoLog(shaderProgram);

    // Clean up the shaders now that they are linked
    glDetachShader(shaderProgram, shadVert);
    glDetachShader(shaderProgram, shadFrag);
    glDeleteShader(shadVert);
    glDeleteShader(shadFrag);

    // Find out what the GLSL locations are, since we can't pre-define these
    locationPos    = glGetAttribLocation (shaderProgram, "vs_Position");
    locationNor    = glGetAttribLocation (shaderProgram, "vs_Normal");
    locationCol    = glGetAttribLocation (shaderProgram, "vs_Color");
    unifViewProj   = glGetUniformLocation(shaderProgram, "u_ViewProj");
    unifModel      = glGetUniformLocation(shaderProgram, "u_Model");
    unifModelInvTr = glGetUniformLocation(shaderProgram, "u_ModelInvTr");
	unifLightPos   = glGetUniformLocation(shaderProgram, "u_LightPos");
	unifLightColor = glGetUniformLocation(shaderProgram, "u_LightColor");
	unifCameraPos  = glGetUniformLocation(shaderProgram, "u_CameraPos");

    printGLErrorLog();
}

void cleanup()
{
    /*glDeleteBuffers(1, &vboPos);
    glDeleteBuffers(1, &vboCol);
    glDeleteBuffers(1, &vboNor);
    glDeleteBuffers(1, &vboIdx);
	*/
	for(int i=0;i<in.getCount();i++){
		glDeleteBuffers(1,&vboPos[i]);
		glDeleteBuffers(1,&vboCol[i]);
		glDeleteBuffers(1,&vboNor[i]);
		glDeleteBuffers(1,&vboIdx[i]);
	}
    glDeleteProgram(shaderProgram);

    delete geometry;
}

void keypress(unsigned char key, int x, int y)
{
    switch (key) {
    case 'q':
        cleanup();
        exit(0);
        break;
	case 'n':
		n->resetRGBA();
		q->setShowRGBA(1,1,0);
		geometry=q->obj;
		if(geometry!=nullptr){
			geometry->setColor(q->getRGBA());
			uploadObject(q);
		}
		geometry=n->obj;
		if(geometry!=nullptr){
			geometry->setColor(n->getRGBA());
			uploadObject(n);
			n=q;
		}
		else n=q;
		cout<<n->getName()<<endl;
		q=preorderTraverse(q);
		break;
	case 'p':
		outputImage();
		cout<<"!"<<endl;
		break;
	case 'a':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::translate(glm::mat4(1),glm::vec3(-0.5,0,0));
			temp->setModel(model,'t');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				model=glm::translate(glm::mat4(1),glm::vec3(-0.5,0,0));
				temp->setModel(model,'t');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'd':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::translate(glm::mat4(1),glm::vec3(0.5,0,0));
			temp->setModel(model,'t');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				model=glm::translate(glm::mat4(1),glm::vec3(0.5,0,0));
				temp->setModel(model,'t');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'w':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::translate(glm::mat4(1),glm::vec3(0,0.5,0));
			temp->setModel(model,'t');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				model=glm::translate(glm::mat4(1),glm::vec3(0,0.5,0));
				temp->setModel(model,'t');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 's':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::translate(glm::mat4(1),glm::vec3(0,-0.5,0));
			temp->setModel(model,'t');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				model=glm::translate(glm::mat4(1),glm::vec3(0,-0.5,0));
				temp->setModel(model,'t');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'e':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::translate(glm::mat4(1),glm::vec3(0,0,0.5));
			temp->setModel(model,'t');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				model=glm::translate(glm::mat4(1),glm::vec3(0,0,0.5));
				temp->setModel(model,'t');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'r':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::translate(glm::mat4(1),glm::vec3(0,0,-0.5));
			temp->setModel(model,'t');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				model=glm::translate(glm::mat4(1),glm::vec3(0,0,-0.5));
				temp->setModel(model,'t');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'x':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::scale(glm::mat4(1),glm::vec3(0.5,1,1));
			temp->setModel(model,'s');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				model=glm::scale(glm::mat4(1),glm::vec3(0.5,1,1));
				temp->setModel(model,'s');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'X':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::scale(glm::mat4(1),glm::vec3(2,1,1));
			temp->setModel(model,'s');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				model=glm::scale(glm::mat4(1),glm::vec3(2,1,1));
				temp->setModel(model,'s');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'y':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::scale(glm::mat4(1),glm::vec3(1,0.5,1));
			temp->setModel(model,'s');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				model=glm::scale(glm::mat4(1),glm::vec3(1,0.5,1));
				temp->setModel(model,'s');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'Y':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::scale(glm::mat4(1),glm::vec3(1,2,1));
			temp->setModel(model,'s');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				model=glm::scale(glm::mat4(1),glm::vec3(1,2,1));
				temp->setModel(model,'s');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'z':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::scale(glm::mat4(1),glm::vec3(1,1,0.5));
			temp->setModel(model,'s');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				model=glm::scale(glm::mat4(1),glm::vec3(1,1,0.5));
				temp->setModel(model,'s');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'Z':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::scale(glm::mat4(1),glm::vec3(1,1,2));
			temp->setModel(model,'s');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				model=glm::scale(glm::mat4(1),glm::vec3(1,1,2));
				temp->setModel(model,'s');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'j':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::rotate(glm::mat4(1),10.0f/180*PI,glm::vec3(1,0,0));
			temp->setModel(model,'r');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				glm::vec4 vf=n->getModel('m')*glm::vec4(0,0,0,1);
				glm::vec4 vc=glm::inverse(temp->getModel('m'))*vf;
				glm::vec3 v(vc);
				model=glm::translate(glm::mat4(),v);
				temp->setModel(model,'t');
				model=glm::rotate(glm::mat4(),10.0f/180*PI,glm::vec3(1,0,0));
				temp->setModel(model,'r');
				model=glm::translate(glm::mat4(),-v);
				temp->setModel(model,'t');

				//model=glm::rotate(glm::mat4(1),10.0f/180*PI,glm::vec3(1,0,0));
				//temp->setModel(model,'r');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'J':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::rotate(glm::mat4(1),-10.0f/180*PI,glm::vec3(1,0,0));
			temp->setModel(model,'r');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				glm::vec4 vf=n->getModel('m')*glm::vec4(0,0,0,1);
				glm::vec4 vc=glm::inverse(temp->getModel('m'))*vf;
				glm::vec3 v(vc);
				model=glm::translate(glm::mat4(),v);
				temp->setModel(model,'t');
				model=glm::rotate(glm::mat4(),-10.0f/180*PI,glm::vec3(1,0,0));
				temp->setModel(model,'r');
				model=glm::translate(glm::mat4(),-v);
				temp->setModel(model,'t');

				//model=glm::rotate(glm::mat4(1),-10.0f/180*PI,glm::vec3(1,0,0));
				//temp->setModel(model,'r');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'k':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::rotate(glm::mat4(1),10.0f/180*PI,glm::vec3(0,1,0));
			temp->setModel(model,'r');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				/*glm::vec4 vc=temp->getModel('m')*glm::vec4(0,0,0,1);
				glm::vec4 vf=glm::inverse(n->getModel('m'))*vc;
				glm::vec4 v4(vf.x,0,vf.z,1);

				vf=n->getModel('m')*v4;
				vc=glm::inverse(temp->getModel('m'))*vf;
				v4=vc;
				glm::vec3 v3(v4);
				v3=glm::normalize(v3);

				/*vf=n->getModel('m')*v;
				vc=glm::inverse(temp->getModel('m'))*vf;
				glm::vec3 v1(vc);

				model=glm::translate(glm::mat4(),v1);
				temp->setModel(model,'t');
				model=glm::rotate(glm::mat4(),10.0f/180*PI,glm::vec3(0,1,0));
				temp->setModel(model,'r');
				model=glm::translate(glm::mat4(),-v1);
				temp->setModel(model,'t');
				

				glm::mat4 r=glm::inverse(n->getModel('m'))*temp->getModel('m');

				model=glm::rotate(glm::mat4(1),10.0f/180*PI,glm::vec3(0,1,0));
				glm::mat4 v4=n->getModel('m')*model*r;
				//model=glm::translate(glm::mat4(),glm::vec3(v4[3]));
				//temp->setModel(model,'t');
				//model=v4;
				//model[3]=glm::vec4(0,0,0,1);
				temp->setModel(v4,'r');
				temp=preorderTraverse(temp);
				*/

				/*vf=n->getModel('m')*glm::vec4(0,0,0,1);
				vc=glm::inverse(temp->getModel('m'))*vf;
				glm::vec3 v(vc);
				//model=glm::translate(glm::mat4(),v);
				//temp->setModel(model,'t');
				model=glm::rotate(glm::mat4(),10.0f/180*PI,v3);
				temp->setModel(model,'r');
				//model=glm::translate(glm::mat4(),-v);
				//temp->setModel(model,'t');

				//model=glm::rotate(glm::mat4(1),-10.0f/180*PI,glm::vec3(0,1,0));
				//temp->setModel(model,'r');
				temp=preorderTraverse(temp);
				*/

				glm::vec4 vf=n->getModel('m')*glm::vec4(0,0,0,1);
				glm::vec4 vc=glm::inverse(temp->getModel('m'))*vf;
				glm::vec3 v(vc);
				model=glm::translate(glm::mat4(),v);
				temp->setModel(model,'t');
				model=glm::rotate(glm::mat4(),10.0f/180*PI,glm::vec3(0,1,0));
				temp->setModel(model,'r');
				model=glm::translate(glm::mat4(),-v);
				temp->setModel(model,'t');

				//model=glm::rotate(glm::mat4(1),-10.0f/180*PI,glm::vec3(0,1,0));
				//temp->setModel(model,'r');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'K':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::rotate(glm::mat4(1),-10.0f/180*PI,glm::vec3(0,1,0));
			temp->setModel(model,'r');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				glm::vec4 vf=n->getModel('m')*glm::vec4(0,0,0,1);
				glm::vec4 vc=glm::inverse(temp->getModel('m'))*vf;
				glm::vec3 v(vc);
				model=glm::translate(glm::mat4(),v);
				temp->setModel(model,'t');
				model=glm::rotate(glm::mat4(),-10.0f/180*PI,glm::vec3(0,1,0));
				temp->setModel(model,'r');
				model=glm::translate(glm::mat4(),-v);
				temp->setModel(model,'t');

				//model=glm::rotate(glm::mat4(1),-10.0f/180*PI,glm::vec3(0,1,0));
				//temp->setModel(model,'r');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'l':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::rotate(glm::mat4(1),10.0f/180*PI,glm::vec3(0,0,1));
			temp->setModel(model,'r');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				glm::vec4 vf=n->getModel('m')*glm::vec4(0,0,0,1);
				glm::vec4 vc=glm::inverse(temp->getModel('m'))*vf;
				glm::vec3 v(vc);
				model=glm::translate(glm::mat4(),v);
				temp->setModel(model,'t');
				model=glm::rotate(glm::mat4(),10.0f/180*PI,glm::vec3(0,0,1));
				temp->setModel(model,'r');
				model=glm::translate(glm::mat4(),-v);
				temp->setModel(model,'t');

				//model=glm::rotate(glm::mat4(1),10.0f/180*PI,glm::vec3(0,0,1));
				//temp->setModel(model,'r');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'L':{
			Node *temp=n,*next=n;
			glm::mat4 model=glm::rotate(glm::mat4(1),-10.0f/180*PI,glm::vec3(0,0,1));
			temp->setModel(model,'r');
			temp=preorderTraverse(temp);
			while(next->p!=nullptr&&next->nextBrother==nullptr) next=next->p;
			if(next->p!=nullptr) next=next->nextBrother;
			while(temp!=next){
				glm::vec4 vf=n->getModel('m')*glm::vec4(0,0,0,1);
				glm::vec4 vc=glm::inverse(temp->getModel('m'))*vf;
				glm::vec3 v(vc);
				model=glm::translate(glm::mat4(),v);
				temp->setModel(model,'t');
				model=glm::rotate(glm::mat4(),-10.0f/180*PI,glm::vec3(0,0,1));
				temp->setModel(model,'r');
				model=glm::translate(glm::mat4(),-v);
				temp->setModel(model,'t');

				//model=glm::rotate(glm::mat4(1),-10.0f/180*PI,glm::vec3(0,0,1));
				//temp->setModel(model,'r');
				temp=preorderTraverse(temp);
			}
		}
		break;
	case 'u':{
			if(n!=root){
				q=n->p;
				if(q->firstChild==n) {
					q->firstChild=n->nextBrother;
				}
				else{
					q=q->firstChild;
					while(q->nextBrother!=n) q=q->nextBrother;
					q->nextBrother=n->nextBrother;
				}
				n=root;
				q=root;
			}
		}
		break;
	case 'f':
		in.moveLight(0.5,'x');
		break;
	case 'F':
		in.moveLight(-0.5,'x');
		break;
	case 'g':
		in.moveLight(0.5,'y');
		break;
	case 'G':
		in.moveLight(-0.5,'y');
		break;
	case 'h':
		in.moveLight(0.5,'z');
		break;
	case 'H':
		in.moveLight(-0.5,'z');
		break;
    }
    glutPostRedisplay();
}

void mousepress(int button, int state, int x, int y)
{
    // Put any mouse events here
}

void display()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    clock_t newTime = clock();
    //rotation += 2.5f * (static_cast<float>(newTime - old_time) / static_cast<float>(CLOCKS_PER_SEC));
    old_time = newTime;

    // Create a matrix to pass to the model matrix uniform variable in the
    // vertex shader, which is used to transform the vertices in our draw call.
    // The default provided value is an identity matrix; you'll change this.

    // Make sure you're using the right program for rendering
    glUseProgram(shaderProgram);

    // TODO
    // Draw the two components of our scene separately, for your scenegraphs it
    // will help your sanity to do separate draws for each type of primitive
    // geometry, otherwise your VBOs will get very, very complicated fast
	/*glm::mat4 modelmat = glm::mat4(1);
	geometry=p->obj;
	geometry->setColor(p->getRGBA());
	modelmat=glm::rotate(modelmat,p->getRotation().x,glm::vec3(1,0,0));
	modelmat=glm::rotate(modelmat,p->getRotation().y,glm::vec3(0,1,0));
	modelmat=glm::rotate(modelmat,p->getRotation().z,glm::vec3(0,0,1));
	modelmat=glm::translate(modelmat,p->getTranslation());
	modelmat=glm::scale(modelmat,p->getScale());
	p->setModel(modelmat);*/
	//uploadObject();
	drawObject(p->getModel('m'));
	p=preorderTraverse(p);
	while(p!=root){
		/*geometry=p->obj;
		geometry->setColor(p->getRGBA());
		modelmat=p->p->getModel();
		modelmat=glm::rotate(modelmat,p->getRotation().x,glm::vec3(1,0,0));
		modelmat=glm::rotate(modelmat,p->getRotation().y,glm::vec3(0,1,0));
		modelmat=glm::rotate(modelmat,p->getRotation().z,glm::vec3(0,0,1));
		modelmat=glm::translate(modelmat,p->getTranslation());
		modelmat=glm::scale(modelmat,p->getScale());
		p->setModel(modelmat);
		//uploadObject();
		drawObject(modelmat);
		p=preorderTraverse(p);*/
		drawObject(p->getModel('m'));
		p=preorderTraverse(p);
	}
	
    // Move the rendering we just made onto the screen
    glutSwapBuffers();

    // Check for any GL errors that have happened recently
    printGLErrorLog();
}

void drawObject(glm::mat4 modelmat){
	if(p->getShape()=="sphere")
		sampleDrawObject(modelmat,480*2);
	else if(p->getShape()=="cylinder")
		sampleDrawObject(modelmat,114*2);
	else if(p->getShape()=="cube")
		sampleDrawObject(modelmat,12*2);
	else if(p->getShape()=="mesh")
		sampleDrawObject(modelmat,p->obj->getIndices().size()/3);
}

void sampleUploadSquare(Node *p)
{
    // Take a close look at how vertex, normal, color, and index informations are created and
    // uploaded to the GPU for drawing. You will need to do something similar to get your
    // scene graph to draw.

    // =========================== Create some data to draw ====================================
    // These four points define where the quad would be BEFORE transformations
    // this is referred to as object-space and it's best to center geometry at the origin for easier transformations.
    // Each vertex is {x,y,z,w} where w is the homogeneous coordinate

    // Number of vertices
    const int VERTICES = 4;
    // Number of triangles
    const int TRIANGLES = 2;

    // Sizes of the various array elements below.
    static const GLsizei SIZE_POS = sizeof(glm::vec3);
    static const GLsizei SIZE_NOR = sizeof(glm::vec3);
    static const GLsizei SIZE_COL = sizeof(glm::vec3);
    static const GLsizei SIZE_TRI = 3 * sizeof(GLuint);

    // Initialize an array of floats to hold our cube's position data.
    // Each vertex is {x,y,z,w} where w is the homogeneous coordinate
    glm::vec3 positions[VERTICES] = {
        glm::vec3(-1, +1, -1),
        glm::vec3(-1, -1, -1),
        glm::vec3(+1, -1, -1),
        glm::vec3(+1, +1, -1),
    };

    // Same as above for the cube's normal data.
    glm::vec3 normals[VERTICES] = {
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, 1),
    };

    // Initialize an array of floats to hold our square's color data
    // Color elements are in the range [0, 1], {r, g, b}
    glm::vec3 colors[VERTICES] = {
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 0, 1),
        glm::vec3(1, 1, 0),
    };

    // Initialize an array of six unsigned ints to hold our square's index data
    GLuint indices[TRIANGLES][3] = {
        0, 1, 2,
        0, 2, 3,
    };

    // ================UPLOADING CODE (GENERALLY, ONCE PER CHANGE IN DATA)==============
    // Now we put the data into the Vertex Buffer Object for the graphics system to use
	glBindBuffer(GL_ARRAY_BUFFER, vboPos[p->getVboCount()]);
    // Use STATIC_DRAW since the square's vertices don't need to change while the program runs.
    // Take a look at STREAM_DRAW and DYNAMIC_DRAW to see when they should be used.
    // Always make sure you are telling OpenGL the right size to make the buffer. Here we need 16 floats.
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, &positions, GL_STATIC_DRAW);

    // Bind+upload the color data
    glBindBuffer(GL_ARRAY_BUFFER, vboCol[p->getVboCount()]);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, &colors, GL_STATIC_DRAW);

    // Bind+upload the normals
    glBindBuffer(GL_ARRAY_BUFFER, vboNor[p->getVboCount()]);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, &normals, GL_STATIC_DRAW);

    // Bind+upload the indices to the GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIdx[p->getVboCount()]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, TRIANGLES * SIZE_TRI, &indices, GL_STATIC_DRAW);

    // Once data is loaded onto the GPU, we are done with the float arrays.
    // For your scene graph implementation, you shouldn't create and delete the vertex information
    // every frame. You would probably want to store and reuse them.
}

void sampleUploadMesh(Node *p)
{
    // Number of vertices
	const int VERTICES = geometry->getVertices().size();
    // Number of triangles
	const int TRIANGLES =geometry->getIndices().size()/3;
    // Sizes of the various array elements below.
    static const GLsizei SIZE_POS = sizeof(glm::vec3);
    static const GLsizei SIZE_NOR = sizeof(glm::vec3);
    static const GLsizei SIZE_COL = sizeof(glm::vec3);
    static const GLsizei SIZE_TRI = 3 * sizeof(GLuint);

    // Initialize an array of floats to hold our cube's position data.
    // Each vertex is {x,y,z,w} where w is the homogeneous coordinate
	vector<glm::vec3> position=geometry->getVertices();
	glm::vec3 *positions=new glm::vec3[VERTICES];
	//glm::vec3 positions[8];
	for(int i=0;i<VERTICES;i++){
		*(positions+i)=position[i];
	}

    // Same as above for the cube's normal data.
	vector<glm::vec3> normal=geometry->getNormals();
	glm::vec3 *normals=new glm::vec3[VERTICES];
	//glm::vec3 normals[36];
	for(int i=0;i<VERTICES;i++)
		normals[i]=normal[i];

    // Initialize an array of floats to hold our square's color data
	vector<glm::vec3> color=geometry->getColors();
	glm::vec3 *colors=new glm::vec3[VERTICES];
	//glm::vec3 colors[8];
	for(int i=0;i<VERTICES;i++)
		colors[i]=color[i];

    // Initialize an array of six unsigned ints to hold our square's index data
	vector<unsigned int> ind=geometry->getIndices();
	unsigned int *indices=new unsigned int[TRIANGLES*3];
	//unsigned int indices[36];
	for(int i=0;i<3*TRIANGLES;i++)
		indices[i]=ind[i];

	/*cout<<"VERTICES"<<endl;
	for(int i=0;i<VERTICES;i++) cout<<positions[i].x<<","<<position[i].y<<","<<position[i].z<<endl;
	cout<<"NORMALS"<<endl;
	for(int i=0;i<3*TRIANGLES;i++) cout<<normals[i].x<<","<<normals[i].y<<","<<normals[i].z<<endl;
	cout<<"COLORS"<<endl;
	for(int i=0;i<VERTICES;i++) cout<<colors[i].x<<","<<colors[i].y<<","<<colors[i].z<<endl;
	cout<<"INDEXS"<<endl;
	for(int i=0;i<3*TRIANGLES;i++) cout<<indices[i]<<endl;*/

    // ================UPLOADING CODE (GENERALLY, ONCE PER CHANGE IN DATA)==============
    // Now we put the data into the Vertex Buffer Object for the graphics system to use
    glBindBuffer(GL_ARRAY_BUFFER, vboPos[p->getVboCount()]);
    // Use STATIC_DRAW since the square's vertices don't need to change while the program runs.
    // Take a look at STREAM_DRAW and DYNAMIC_DRAW to see when they should be used.
    // Always make sure you are telling OpenGL the right size to make the buffer. Here we need 16 floats.
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, positions, GL_DYNAMIC_DRAW);

    // Bind+upload the color data
    glBindBuffer(GL_ARRAY_BUFFER, vboCol[p->getVboCount()]);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, colors, GL_DYNAMIC_DRAW);

    // Bind+upload the normals
    glBindBuffer(GL_ARRAY_BUFFER, vboNor[p->getVboCount()]);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, normals, GL_DYNAMIC_DRAW);

    // Bind+upload the indices to the GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIdx[p->getVboCount()]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, TRIANGLES * SIZE_TRI, indices, GL_DYNAMIC_DRAW);

    // Once data is loaded onto the GPU, we are done with the float arrays.
    // For your scene graph implementation, you shouldn't create and delete the vertex information
    // every frame. You would probably want to store and reuse them.
}

void sampleUploadCube(Node *p)
{
    // Number of vertices
    const int VERTICES = 8;
    // Number of triangles
    const int TRIANGLES =12;
	const int NUM=geometry->getVertexCount();

    // Sizes of the various array elements below.
    static const GLsizei SIZE_POS = sizeof(glm::vec3);
    static const GLsizei SIZE_NOR = sizeof(glm::vec3);
    static const GLsizei SIZE_COL = sizeof(glm::vec3);
    static const GLsizei SIZE_TRI = 3 * sizeof(GLuint);

    // Initialize an array of floats to hold our cube's position data.
    // Each vertex is {x,y,z,w} where w is the homogeneous coordinate
	vector<glm::vec3> position=geometry->getVertices();
	glm::vec3 positions[8];
	for(int i=0;i<8;i++){
		*(positions+i)=position[i];
	}

    // Same as above for the cube's normal data.
	vector<glm::vec3> normal=geometry->getNormals();
	glm::vec3 normals[8];
	for(int i=0;i<8;i++)
		normals[i]=normal[i];

    // Initialize an array of floats to hold our square's color data
	vector<glm::vec3> color=geometry->getColors();
	glm::vec3 colors[8];
	for(int i=0;i<8;i++)
		colors[i]=color[i];

    // Initialize an array of six unsigned ints to hold our square's index data
	vector<unsigned int> ind=geometry->getIndices();
	unsigned int indices[36];
	for(int i=0;i<36;i++)
		indices[i]=ind[i];

    // ================UPLOADING CODE (GENERALLY, ONCE PER CHANGE IN DATA)==============
    // Now we put the data into the Vertex Buffer Object for the graphics system to use
    glBindBuffer(GL_ARRAY_BUFFER, vboPos[p->getVboCount()]);
    // Use STATIC_DRAW since the square's vertices don't need to change while the program runs.
    // Take a look at STREAM_DRAW and DYNAMIC_DRAW to see when they should be used.
    // Always make sure you are telling OpenGL the right size to make the buffer. Here we need 16 floats.
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, &positions, GL_STATIC_DRAW);

    // Bind+upload the color data
    glBindBuffer(GL_ARRAY_BUFFER, vboCol[p->getVboCount()]);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, &colors, GL_STATIC_DRAW);

    // Bind+upload the normals
    glBindBuffer(GL_ARRAY_BUFFER, vboNor[p->getVboCount()]);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, &normals, GL_STATIC_DRAW);

    // Bind+upload the indices to the GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIdx[p->getVboCount()]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, TRIANGLES * SIZE_TRI, &indices, GL_STATIC_DRAW);

    // Once data is loaded onto the GPU, we are done with the float arrays.
    // For your scene graph implementation, you shouldn't create and delete the vertex information
    // every frame. You would probably want to store and reuse them.
}

void sampleUploadCylinder(Node *p)
{
    // Number of vertices
    const int VERTICES = 228;
    // Number of triangles
    const int TRIANGLES =76;
	const int NUM=geometry->getVertexCount();

    // Sizes of the various array elements below.
    static const GLsizei SIZE_POS = sizeof(glm::vec3);
    static const GLsizei SIZE_NOR = sizeof(glm::vec3);
    static const GLsizei SIZE_COL = sizeof(glm::vec3);
    static const GLsizei SIZE_TRI = 3 * sizeof(GLuint);

    // Initialize an array of floats to hold our cube's position data.
    // Each vertex is {x,y,z,w} where w is the homogeneous coordinate
	vector<glm::vec3> position=geometry->getVertices();
	glm::vec3 positions[228];
	for(int i=0;i<228;i++){
		*(positions+i)=position[i];
	}

    // Same as above for the cube's normal data.
	vector<glm::vec3> normal=geometry->getNormals();
	glm::vec3 normals[228];
	for(int i=0;i<228;i++)
		normals[i]=normal[i];

    // Initialize an array of floats to hold our square's color data
	vector<glm::vec3> color=geometry->getColors();
	glm::vec3 colors[228];
	for(int i=0;i<228;i++)
		colors[i]=color[i];

    // Initialize an array of six unsigned ints to hold our square's index data
	vector<unsigned int> ind=geometry->getIndices();
	unsigned int indices[228];
	for(int i=0;i<228;i++)
		indices[i]=ind[i];

    // ================UPLOADING CODE (GENERALLY, ONCE PER CHANGE IN DATA)==============
    // Now we put the data into the Vertex Buffer Object for the graphics system to use
    glBindBuffer(GL_ARRAY_BUFFER, vboPos[p->getVboCount()]);
    // Use STATIC_DRAW since the square's vertices don't need to change while the program runs.
    // Take a look at STREAM_DRAW and DYNAMIC_DRAW to see when they should be used.
    // Always make sure you are telling OpenGL the right size to make the buffer. Here we need 16 floats.
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, &positions, GL_STATIC_DRAW);

    // Bind+upload the color data
    glBindBuffer(GL_ARRAY_BUFFER, vboCol[p->getVboCount()]);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, &colors, GL_STATIC_DRAW);

    // Bind+upload the normals
    glBindBuffer(GL_ARRAY_BUFFER, vboNor[p->getVboCount()]);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, &normals, GL_STATIC_DRAW);

    // Bind+upload the indices to the GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIdx[p->getVboCount()]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, TRIANGLES * SIZE_TRI, &indices, GL_STATIC_DRAW);

    // Once data is loaded onto the GPU, we are done with the float arrays.
    // For your scene graph implementation, you shouldn't create and delete the vertex information
    // every frame. You would probably want to store and reuse them.
}

void sampleUploadSphere(Node *p)
{
    // Number of vertices
    const int VERTICES = 242;
    // Number of triangles
    const int TRIANGLES =480;
	const int NUM=geometry->getVertexCount();

    // Sizes of the various array elements below.
    static const GLsizei SIZE_POS = sizeof(glm::vec3);
    static const GLsizei SIZE_NOR = sizeof(glm::vec3);
    static const GLsizei SIZE_COL = sizeof(glm::vec3);
    static const GLsizei SIZE_TRI = 3 * sizeof(GLuint);

    // Initialize an array of floats to hold our cube's position data.
    // Each vertex is {x,y,z,w} where w is the homogeneous coordinate
	vector<glm::vec3> position=geometry->getVertices();
	glm::vec3 positions[242];
	for(int i=0;i<242;i++){
		*(positions+i)=position[i];
	}

    // Same as above for the cube's normal data.
	vector<glm::vec3> normal=geometry->getNormals();
	glm::vec3 normals[242];
	for(int i=0;i<242;i++)
		normals[i]=normal[i];

    // Initialize an array of floats to hold our square's color data
	vector<glm::vec3> color=geometry->getColors();
	glm::vec3 colors[242];
	for(int i=0;i<242;i++)
		colors[i]=color[i];

    // Initialize an array of six unsigned ints to hold our square's index data
	vector<unsigned int> ind=geometry->getIndices();
	unsigned int indices[1440];
	for(int i=0;i<1440;i++)
		indices[i]=ind[i];

    // ================UPLOADING CODE (GENERALLY, ONCE PER CHANGE IN DATA)==============
    // Now we put the data into the Vertex Buffer Object for the graphics system to use
    glBindBuffer(GL_ARRAY_BUFFER, vboPos[p->getVboCount()]);
    // Use STATIC_DRAW since the square's vertices don't need to change while the program runs.
    // Take a look at STREAM_DRAW and DYNAMIC_DRAW to see when they should be used.
    // Always make sure you are telling OpenGL the right size to make the buffer. Here we need 16 floats.
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, &positions, GL_STATIC_DRAW);

    // Bind+upload the color data
    glBindBuffer(GL_ARRAY_BUFFER, vboCol[p->getVboCount()]);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, &colors, GL_STATIC_DRAW);

    // Bind+upload the normals
    glBindBuffer(GL_ARRAY_BUFFER, vboNor[p->getVboCount()]);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, &normals, GL_STATIC_DRAW);

    // Bind+upload the indices to the GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIdx[p->getVboCount()]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, TRIANGLES * SIZE_TRI, &indices, GL_STATIC_DRAW);

    // Once data is loaded onto the GPU, we are done with the float arrays.
    // For your scene graph implementation, you shouldn't create and delete the vertex information
    // every frame. You would probably want to store and reuse them.
}

void sampleDrawObject(glm::mat4 model,int faces)
{
    // Tell the GPU which shader program to use to draw things
	glUseProgram(shaderProgram);

	glUniform4f(unifLightPos,in.getLPOS(0).x,in.getLPOS(0).y,in.getLPOS(0).z,1);
	glUniform4f(unifLightColor,in.getLCOL(0).x,in.getLCOL(0).y,in.getLCOL(0).z,1);
	glUniform4f(unifCameraPos,in.getEYEP().x,in.getEYEP().y,in.getEYEP().z,1);

    // Take a close look at how vertex, normal, color, and index informations
    // are created and uploaded to the GPU for drawing. You will need to do
    // something similar to get your scene graph to draw.

    //model = glm::rotate(model, rotation, glm::vec3(1, 1, 1));

    // Number of faces (1 on a square)
    //const int FACES = faces;
    // Number of triangles (2 per face)
    const int TRIANGLES = faces;
    // =============================== Draw the data that we sent =================================
    // Activate our three kinds of vertex information
    glEnableVertexAttribArray(locationPos);
    glEnableVertexAttribArray(locationCol);
    glEnableVertexAttribArray(locationNor);

    // Set the 4x4 model transformation matrices
    // Pointer to the first element of the array
    glUniformMatrix4fv(unifModel, 1, GL_FALSE, &model[0][0]);
    // Also upload the inverse transpose for normal transformation
    const glm::mat4 modelInvTranspose = glm::inverse(glm::transpose(model));
    glUniformMatrix4fv(unifModelInvTr, 1, GL_FALSE, &modelInvTranspose[0][0]);

    // Tell the GPU where the positions are: in the position buffer (4 components each)
    glBindBuffer(GL_ARRAY_BUFFER, vboPos[p->getVboCount()]);
    glVertexAttribPointer(locationPos, 3, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the colors are: in the color buffer (4 components each)
    glBindBuffer(GL_ARRAY_BUFFER, vboCol[p->getVboCount()]);
    glVertexAttribPointer(locationCol, 3, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the normals are: in the normal buffer (4 components each)
    glBindBuffer(GL_ARRAY_BUFFER, vboNor[p->getVboCount()]);
    glVertexAttribPointer(locationNor, 3, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the indices are: in the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIdx[p->getVboCount()]);

    // Draw the elements. Here we are only drawing 2 triangles * 3 vertices per triangle, for a
    // total of 6 elements.
    glDrawElements(GL_TRIANGLES, TRIANGLES * 3, GL_UNSIGNED_INT, 0);

    // Shut off the information since we're done drawing.
    glDisableVertexAttribArray(locationPos);
    glDisableVertexAttribArray(locationCol);
    glDisableVertexAttribArray(locationNor);

    // Check for OpenGL errors
    printGLErrorLog();
}

void resize(int width, int height)
{
    // Set viewport
    glViewport(0, 0, width, height);

    // Get camera information
    // Add code here if you want to play with camera settings/ make camera interactive.
    //glm::mat4 projection = glm::perspective(PI / 4, width / (float) height, 0.1f, 100.0f);
    //glm::mat4 camera = glm::lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 projection = glm::perspective(in.getFOVY(), width / (float) height, 0.1f, 100.0f);
	glm::mat4 camera = glm::lookAt(in.getEYEP(), in.getEYEP()+in.getVDIR(), in.getUVEC());
    projection = projection * camera;

    // Upload the projection matrix, which changes only when the screen or
    // camera changes
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(unifViewProj, 1, GL_FALSE, &projection[0][0]);

    glutPostRedisplay();
}


std::string textFileRead(const char *filename)
{
    // http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
    std::ifstream in(filename, std::ios::in);
    if (!in) {
        std::cerr << "Error reading file" << std::endl;
        throw (errno);
    }
	std::istreambuf_iterator<char>();
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

void printGLErrorLog()
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error " << error << ": ";
        const char *e =
            error == GL_INVALID_OPERATION             ? "GL_INVALID_OPERATION" :
            error == GL_INVALID_ENUM                  ? "GL_INVALID_ENUM" :
            error == GL_INVALID_VALUE                 ? "GL_INVALID_VALUE" :
            error == GL_INVALID_INDEX                 ? "GL_INVALID_INDEX" :
            "unknown";
        std::cerr << e << std::endl;

        // Throwing here allows us to use the debugger stack trace to track
        // down the error.
#ifndef __APPLE__
        // But don't do this on OS X. It might cause a premature crash.
        // http://lists.apple.com/archives/mac-opengl/2012/Jul/msg00038.html
        throw;
#endif
    }
}

void printLinkInfoLog(int prog)
{
    GLint linked;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (linked == GL_TRUE) {
        return;
    }
    std::cerr << "GLSL LINK ERROR" << std::endl;

    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }
    // Throwing here allows us to use the debugger to track down the error.
    throw;
}

void printShaderInfoLog(int shader)
{
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_TRUE) {
        return;
    }
    std::cerr << "GLSL COMPILE ERROR" << std::endl;

    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }
    // Throwing here allows us to use the debugger to track down the error.
    throw;
}
