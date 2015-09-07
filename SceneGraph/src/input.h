#ifndef INPUT_H
#define INPUT_H

#include<iostream>
#include<glm/glm.hpp>
#include<vector>
#include<string>
#include<fstream>
#include "node.h"
#include "Mesh.h"
#include "material.h"
using namespace std;

class input{
private:
	unsigned int width,height,count,lightNum,lightIdx;
	glm::vec3 EYEP,VDIR,UVEC;
	vector<glm::vec3> LPOS,LCOL;
	float FOVY;
	vector<Node> node;
	Node n;
	ifstream in,inObj;
	vector<material> mat;
public:
	int lightNodeIdx;
	input();
	~input();
	void readFile(string fileName);
	mesh *readObj(string fileName);
	Node *getNode(int i);
	unsigned int getNodeNum();
	unsigned int getWidth();
	unsigned int getHeight();
	unsigned int getCount();
	unsigned int getLightCount();
	glm::vec3 getEYEP();
	glm::vec3 getVDIR();
	glm::vec3 getUVEC();
	glm::vec3 getLPOS(int i);
	glm::vec3 getLCOL(int i);
	vector<material> getMat();
	float getFOVY();
	void resetTraversed();
	void moveLight(float distancem, char axis);
};

#endif