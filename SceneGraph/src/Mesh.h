#ifndef MESH_H
#define MESH_H

#include "Geometry.h"
#include "octree.h"
#include "kdtree.h"
#include <iostream>
#include <time.h>
#include <Windows.h>
using namespace std;

class mesh :public Geometry
{
private:
	vector<glm::vec3> ver,nor;
	vector<unsigned int> normalIdx,textureIdx;
	float radius;
	glm::vec3 center;
	octree *otree;
	kdtree *ktree;
	char treeType;

protected:
	virtual Intersection intersectImpl(const Ray &ray);
	bool sphereImpl(const Ray &ray)const;
	bool cubeImpl(const Ray &ray,float xmax,float xmin,float ymax,float ymin,float zmax,float zmin)const;
	void octIntersect(const Ray &ray,octree *root,vector<unsigned int> &treeIdx);
	void kdIntersect(const Ray &ray,kdtree *root,vector<unsigned int> &treeIdx);

public:
	mesh();
	~mesh();
	void initTreeStructure(char type);
	void computeBoundingSphere();
	void saveVertex(glm::vec3 vertex);
	void addVertex(glm::vec3 vertex);
	void addColor(glm::vec3 color);
	void saveNormal(glm::vec3 normal);
	void addNormal(glm::vec3 normal);
	void addVertexIndex(unsigned int index);
	void addNormalIndex(unsigned int index);
	void addTextureIndex(unsigned int index);
	void buildGeomtery();
	void computeNormal(bool usVn);
	void averageNormal(bool usVn);
	Intersection intersectImplTri(const Ray &ray,glm::vec3 p1,glm::vec3 p2,glm::vec3 p3,
		glm::vec3 n1,glm::vec3 n2,glm::vec3 n3)const;
	Intersection intersectImplTriTest(const Ray &ray,glm::vec3 p1,glm::vec3 p2,glm::vec3 p3)const;
};

#endif