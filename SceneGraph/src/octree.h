#ifndef _OCTREE_
#define _OCTREE_
#include<iostream>
#include<vector>
#include <glm/glm.hpp>
using namespace std;

class octree{
public:
	//up(u),down(d),left(l),right(r),front(f),back(b)
	octree *ulf,*ulb,*urf,*urb,*dlf,*dlb,*drf,*drb;//eight child
	float xmax,xmin,ymax,ymin,zmax,zmin,xlength,ylength,zlength,xhalf,yhalf,zhalf;
	int depth;
	vector<unsigned int> mesh;

	octree();
	octree(int depth,float xmax,float xmin,float ymax,float ymin,float zmax,float zmin);
	~octree();

	void addTriangle(int index,glm::vec3 p1,glm::vec3 p2,glm::vec3 p3);
	void createTree(int depth,float xmax,float xmin,float ymax,float ymin,float zmax,float zmin);
	int countTriangle(octree *root);
};

#endif