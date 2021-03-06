#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <iostream>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Intersection.h"
#include "Ray.h"

using namespace std;

// An abstract base class for geometry in the scene graph.
class Geometry
{
public:
    // Enums for the types of geometry that your scene graph is required to contain.
    // Feel free to add more.
    enum geometryType {CUBE, SPHERE, CYLINDER, MESH};
    explicit Geometry(geometryType);
    virtual ~Geometry();

    // Function for building vertex data, i.e. vertices, colors, normals, indices.
    // Implemented in Sphere and Cylinder.
    virtual void buildGeomtery() = 0;

    // Getters
    const vector<glm::vec3>& getVertices() const
    {
        return vertices_;
    };
    const vector<glm::vec3>& getNormals() const
    {
        return normals_;
    };
    const vector<glm::vec3>& getColors() const
    {
        return colors_;
    };
    const vector<unsigned int>& getIndices() const
    {
        return indices_;
    };

    unsigned int getVertexCount() const
    {
        return vertices_.size();
    };
    unsigned int getIndexCount() const
    {
        return indices_.size();
    };

    const geometryType getGeometryType() const
    {
        return type_;
    };

	glm::vec3 *getVertices_(){
		int size=vertices_.size();
		glm::vec3 *vertices=new glm::vec3[size];
		for(int i=0;i<size;i++)
			vertices[i]=vertices_[i];
		return vertices;
	}

	glm::vec3 *getNormals_(){
		int size=normals_.size();
		glm::vec3 *normals=new glm::vec3[size];
		for(int i=0;i<size;i++)
			normals[i]=normals_[i];
		cout<<size<<endl;
		return normals;
	}

	glm::vec3 *getColor_(){
		int size=colors_.size();
		glm::vec3 *colors=new glm::vec3[size];
		for(int i=0;i<size;i++)
			colors[i]=colors_[i];
		cout<<size<<endl;
		return colors;
	}

	unsigned int *getIndices_(){
		int size=indices_.size();
		unsigned int *indices=new unsigned int[size];
		for(int i=0;i<size;i++)
			indices[i]=indices_[i];
		cout<<size<<endl;
		return indices;
	}

	void setColor(glm::vec3 color){
		int size=colors_.size();
		colors_.clear();
		for(int i=0;i<size;i++){
			colors_.push_back(color);
		}
	}

	Intersection intersect(const glm::mat4 &T, Ray ray_world);

protected:
    geometryType type_;

    vector<glm::vec3> vertices_;        // vertex buffer
    vector<glm::vec3> normals_;         // normal buffer
    vector<glm::vec3> colors_;          // color buffer
    vector<unsigned int> indices_;      // index buffer

	 virtual Intersection intersectImpl(const Ray &ray) = 0;
};

#endif
