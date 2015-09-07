#include "node.h"
#include <string>
#include<iostream>
#define M_PI 3.1415926

Node::Node(){
	firstChild=nullptr;
	nextBrother=nullptr;
	p=nullptr;
	traversed=0;
	rgba=glm::vec3(1,1,1);
}

Node::~Node(){}

void Node::setName(string n){
	name=n;
}

void Node::setParent(string p){
	parent=p;
}

void Node::setShape(string s){
	shape=s;
}

void Node::setTranslation(float x,float y,float z){
	translation.x=x;
	translation.y=y;
	translation.z=z;
}

void Node::setRotation(float x,float y,float z){
	rotation.x=x;
	rotation.y=y;
	rotation.z=z;
}

void Node::setScale(float x,float y,float z){
	scale.x=x;
	scale.y=y;
	scale.z=z;
}

void Node::setCenter(float x,float y,float z){
	center.x=x;
	center.y=y;
	center.z=z;
}

void Node::setRGBA(float r,float g,float b){
	rgba.r=r;
	rgba.g=g;
	rgba.b=b;
	showRGBA.r=r;
	showRGBA.g=g;
	showRGBA.b=b;
}

void Node::setVboCount(unsigned int count){
	vboCount=count;
}

void Node::resetRGBA(){
	showRGBA=rgba;
}

void Node::setShowRGBA(float r,float g,float b){
	showRGBA.r=r;
	showRGBA.g=g;
	showRGBA.b=b;
}

void Node::setModel(glm::mat4 m,char c){
	if(c=='r'){
		rmodel=rmodel*m;
		glm::vec4 d4=model*glm::vec4(0,0,0,1);
		glm::vec3 d3(d4);
		model=glm::translate(glm::mat4(),d3)*rmodel*smodel;
	}
	else if(c=='s'){
		smodel=smodel*m;
		glm::vec4 d4=model*glm::vec4(0,0,0,1);
		glm::vec3 d3(d4);
		model=glm::translate(glm::mat4(),d3)*rmodel*smodel;
		/*d4=model*glm::vec4(1,0,0,0);
		cout<<d4.x<<endl;
		d4=model*glm::vec4(0,1,0,0);
		cout<<d4.y<<endl;
		d4=model*glm::vec4(0,0,1,0);
		cout<<d4.z<<endl;*/
	}
	else if(c=='t'){
		model=model*m;
	}
}

void Node::setMatNum(int num){
	matNum=num;
}

string Node::getName(){
	return name;
}

string Node::getShape(){
	return shape;
}

int Node::getMatNum(){
	return matNum;
}

unsigned int Node::getVboCount(){
	return vboCount;
}

glm::vec3 Node::getTranslation(){
	return translation;
}

glm::vec3 Node::getRotation(){
	return rotation;
}

glm::vec3 Node::getScale(){
	return scale;
}

glm::vec3 Node::getRGBA(){
	return showRGBA;
}

glm::mat4 Node::getModel(char c){
	if(c=='s') return smodel;
	if(c=='r') return rmodel;
	if(c=='t') return tmodel;
	return model;
}

void Node::initTree(vector<Node> &n){
	unsigned int num=n.size();
	int j,k;
	Node *q;
	for(int i=0;i<num;i++){
		string s=n[i].parent;
		if(s=="null") continue;
		for(j=0;j<num;j++)
			if(n[j].name==s) break;
		if(n[j].name==s){//if find the parent node
			n[i].p=&n[j];
			q=&n[j];q=q->firstChild;
			if(q==nullptr) n[j].firstChild=&n[i];
			else{
				while(q->nextBrother!=nullptr) q=q->nextBrother;
				q->nextBrother=&n[i];
			}
		}
		else{//print error info
			cout<<"please check node names are right"<<endl;
		}
	}
}

glm::vec3 Node::getRandomPointOnSphere(Node *node) {
	// generate u, v, in the range (0, 1)
	float u = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float v = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	float theta = 2.0f * M_PI * u;
	float phi = acos(2.0f * v - 1.0f);

	// find x, y, z coordinates assuming unit sphere in object space
	glm::vec3 point;
	point[0] = sin(phi) * cos(theta);
	point[1] = sin(phi) * sin(theta);
	point[2] = cos(phi);

	// TODO: transform point to world space
	point=glm::vec3(node->getModel('m')*glm::vec4(point,1));

	return point;
}

// Returns a random point on a cube. Adapted from CIS 565
glm::vec3 Node::getRandomPointOnCube(Node *node) {
	// TODO: get the dimensions of the transformed cube in world space
	//glm::vec3 dim (0, 0, 0);
	glm::vec3 dim=glm::vec3(node->getModel('s')[0][0],node->getModel('s')[1][1],node->getModel('s')[2][2]);


	// Get surface area of the cube
	float side1 = dim[0] * dim[1];		// x-y
	float side2 = dim[1] * dim[2];		// y-z
	float side3 = dim[0] * dim[2];		// x-z
	float totalArea = 2.0f * (side1 + side2 + side3);	

	// pick random face weighted by surface area
	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	// pick 2 random components for the point in the range (-0.5, 0.5)
	float c1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.5f;
	float c2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.5f;

	glm::vec3 point;
	if (r < side1 / totalArea) {				
		// x-y front
		point = glm::vec3(c1, c2, 0.5f);
	} else if (r < (side1 * 2) / totalArea) {
		// x-y back
		point = glm::vec3(c1, c2, -0.5f);
	} else if (r < (side1 * 2 + side2) / totalArea) {
		// y-z front
		point = glm::vec3(0.5f, c1, c2);
	} else if (r < (side1 * 2 + side2 * 2) / totalArea) {
		// y-z back
		point = glm::vec3(-0.5f, c1, c2);
	} else if (r < (side1 * 2 + side2 * 2 + side3) / totalArea) {
		// x-z front 
		point = glm::vec3(c1, 0.5f, c2);
	} else {
		// x-z back
		point = glm::vec3(c1, -0.5f, c2);
	}

	//cout<<point.x<<","<<point.y<<","<<point.z<<endl;
	// TODO: transform point to world space
	point=glm::vec3(node->getModel('m')*glm::vec4(point,1));
	//cout<<point.x<<","<<point.y<<","<<point.z<<","<<"in"<<endl;
	//getchar();
	return point;
}

glm::vec3 Node::getCosineWeightedDirection(const glm::vec3& normal) {

	// Pick 2 random numbers in the range (0, 1)
	float xi1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float xi2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	//if(xi1<0) cout<<-1<<endl;

    float up = sqrt(xi1); 			// cos(theta)
	//if(1-up*up<0) cout<<-1<<endl;
    float over = sqrt(1 - up * up); // sin(theta)
    float around = xi2 * 2.0f * M_PI;
    
    // Find a direction that is not the normal based off of whether or not the normal's components 
    // are all equal to sqrt(1/3) or whether or not at least one component is less than sqrt(1/3).
    const float SQRT_OF_ONE_THIRD = sqrt(1.0f/3.0f);
    glm::vec3 directionNotNormal;
    if (abs(normal.x) < SQRT_OF_ONE_THIRD) {
      directionNotNormal = glm::vec3(1.f, 0.f, 0.f);
    } else if (abs(normal.y) < SQRT_OF_ONE_THIRD) {
      directionNotNormal = glm::vec3(0.f, 1.f, 0.f);
    } else {
      directionNotNormal = glm::vec3(0.f, 0.f, 1.f);
    }
    
    //Use not-normal direction to generate two perpendicular directions
    glm::vec3 perpendicularDirection1 = glm::normalize(glm::cross(normal, directionNotNormal));
    glm::vec3 perpendicularDirection2 = glm::normalize(glm::cross(normal, perpendicularDirection1));
    
    return (up * normal) + (cos(around) * over * perpendicularDirection1) + (sin(around) * over * perpendicularDirection2);
}
