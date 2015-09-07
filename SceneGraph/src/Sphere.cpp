#include "Sphere.h"

static const float PI = 3.141592653589f;

// Creates a unit sphere.
Sphere::Sphere() :
    Geometry(SPHERE),
    center_(glm::vec3(0.f, 0.f, 0.f)),
    radius_(1.0f)
{
    buildGeomtery();
}

Sphere::~Sphere() {}

void Sphere::buildGeomtery()
{
    vertices_.clear();
    colors_.clear();
    normals_.clear();
    indices_.clear();

    // Find vertex positions for the sphere.
    unsigned int subdiv_axis = 16;      // vertical slices
    unsigned int subdiv_height = 16;        // horizontal slices
    float dphi = PI / subdiv_height;
    float dtheta = 2.0f * PI / subdiv_axis;
    float epsilon = 0.0001f;
    glm::vec3 color (0.6f, 0.6f, 0.6f);

    // North pole
    glm::vec3 point (0.0f, 1.0f, 0.0f);
    normals_.push_back(point);
    // scale by radius_ and translate by center_
    vertices_.push_back(center_ + radius_ * point);

    for (float phi = dphi; phi < PI; phi += dphi) {
        for (float theta = dtheta; theta <= 2.0f * PI + epsilon; theta += dtheta) {
            float sin_phi = sin(phi);

            point[0] = sin_phi * sin(theta);
            point[1] = cos(phi);
            point[2] = sin_phi * cos(theta);

            normals_.push_back(point);
            vertices_.push_back(center_ + radius_ * point);
        }
    }
    // South pole
    point = glm::vec3(0.0f, -1.0f, 0.0f);
    normals_.push_back(point);
    vertices_.push_back(center_ + radius_ * point);

    // fill in index array.
    // top cap
    for (unsigned int i = 0; i < subdiv_axis - 1; ++i) {
        indices_.push_back(0);
        indices_.push_back(i + 1);
        indices_.push_back(i + 2);
    }
    indices_.push_back(0);
    indices_.push_back(subdiv_axis);
    indices_.push_back(1);

    // middle subdivs
    unsigned int index = 1;
    for (unsigned int i = 0; i < subdiv_height - 2; i++) {
        for (unsigned int j = 0; j < subdiv_axis - 1; j++) {
            // first triangle
            indices_.push_back(index);
            indices_.push_back(index + subdiv_axis);
            indices_.push_back(index + subdiv_axis + 1);

            // second triangle
            indices_.push_back(index);
            indices_.push_back(index + subdiv_axis + 1);
            indices_.push_back(index + 1);

            index++;
        }
        // reuse vertices from start and end point of subdiv_axis slice
        indices_.push_back(index);
        indices_.push_back(index + subdiv_axis);
        indices_.push_back(index + 1);

        indices_.push_back(index);
        indices_.push_back(index + 1);
        indices_.push_back(index + 1 - subdiv_axis);

        index++;
    }

    // end cap
    unsigned int bottom = (subdiv_height - 1) * subdiv_axis + 1;
    unsigned int offset = bottom - subdiv_axis;
    for (unsigned int i = 0; i < subdiv_axis - 1 ; ++i) {
        indices_.push_back(bottom);
        indices_.push_back(i + offset);
        indices_.push_back(i + offset + 1);
    }
    indices_.push_back(bottom);
    indices_.push_back(bottom - 1);
    indices_.push_back(offset);

    // colors
    for (unsigned int i = 0; i < vertices_.size(); ++i) {
        colors_.push_back(glm::vec3(0.6, 0.6, 0.6));
    }
}

Intersection Sphere::intersectImpl(const Ray &ray)
{
	Intersection in;
	//cout<<ray.dir.x<<","<<ray.dir.y<<","<<ray.dir.z<<endl;
	//cout<<ray.orig.x<<","<<ray.orig.y<<","<<ray.orig.z<<endl;
	glm::vec3 normal(0,0,0);
	glm::vec3 intPoint;
	float t=-1,t1,t2,temp;
	in.normal=normal;
	in.t=t;

	if(ray.dir.x==0&&ray.dir.y==0&&ray.dir.z==0){
		float r=ray.orig.x*ray.orig.x+ray.orig.y*ray.orig.y+ray.orig.z*ray.orig.z;
		if(fabs(r-radius_*radius_)>=0.0001) return in;
	}

	float a,b,c,d;
	a=ray.dir.x*ray.dir.x+ray.dir.y*ray.dir.y+ray.dir.z*ray.dir.z;
	b=2*(ray.dir.x*ray.orig.x+ray.dir.y*ray.orig.y+ray.dir.z*ray.orig.z);
	c=ray.orig.x*ray.orig.x+ray.orig.y*ray.orig.y+ray.orig.z*ray.orig.z-radius_*radius_;
	d=b*b-4*a*c;
	if(d<0) return in;
	d=sqrt(d);
	//xyz
	t1=(-b-d)/(2*a);
	t2=(-b+d)/(2*a);
	//if(t1>t2) return in;//no intersect
	if(t2<0) return in;//negative intersect

	if(t1>0.001){
		intPoint=ray.orig+t1*ray.dir;//normal condition
		t=t1;
	}
	else{
		intPoint=ray.orig+t2*ray.dir;//source inside the sphere
		t=t2;
	}
	normal=intPoint;

	in.normal=normal;
	in.t=t;
	return in;
}