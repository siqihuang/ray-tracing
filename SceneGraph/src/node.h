#ifndef NODE_H
#define NODE_H

#include<glm/glm.hpp>
#include"Geometry.h"
#include"Mesh.h"
#include<string>
#include<vector>
using namespace std;

class Node{
private:
	string name,parent,shape;
	unsigned int vboCount;
	glm::vec3 translation,rotation,scale,center,rgba,showRGBA;
	glm::mat4 model,tmodel,rmodel,smodel;
	int matNum;
public:
	Node();
	~Node();
	Node *firstChild,*nextBrother,*p;
	Geometry *obj;
	mesh *m;
	int traversed;
	enum GEOMTYPE{CUBE,CYLINDER,SPHERE,MESH};
	void setName(string n);
	void setParent(string p);
	void setShape(string s);
	void setTranslation(float x,float y,float z);
	void setRotation(float x,float y,float z);
	void setScale(float x,float y,float z);
	void setCenter(float x,float y,float z);
	void setRGBA(float r,float g,float b);
	void setShowRGBA(float r,float g,float b);
	void setVboCount(unsigned int count);
	void setMatNum(int num);
	void resetRGBA();
	void initTree(vector<Node> &n);
	void setModel(glm::mat4 m,char c);
	string getName();
	string getShape();
	int getMatNum();
	unsigned int getVboCount();
	glm::vec3 getTranslation();
	glm::vec3 getRotation();
	glm::vec3 getScale();
	glm::vec3 getRGBA();
	glm::mat4 getModel(char c);

	glm::vec3 getRandomPointOnSphere(Node *node);
	glm::vec3 getRandomPointOnCube(Node *node);
	glm::vec3 getCosineWeightedDirection(const glm::vec3& normal);
};

#endif