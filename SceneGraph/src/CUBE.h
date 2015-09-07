#ifndef CUBE_H
#define CUBE_H

#include "Geometry.h"

class Cube : public Geometry
{
public:
    Cube();
    virtual ~Cube();

    virtual void buildGeomtery();
protected:
	virtual Intersection intersectImpl(const Ray &ray);

private:
    glm::vec3 center_;
	float length_;
};

#endif