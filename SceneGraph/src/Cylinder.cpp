#include "Cylinder.h"

static const float PI = 3.141592653589f;

// Creates a unit cylinder centered at (0, 0, 0)
Cylinder::Cylinder() :
    Geometry(CYLINDER),
    center_(glm::vec3(0.f, 0.f, 0.f)),
    radius_(0.5f),
    height_(1.0f)
{
    buildGeomtery();
}

Cylinder::~Cylinder() {}

void Cylinder::buildGeomtery()
{
    vertices_.clear();
    colors_.clear();
    normals_.clear();
    indices_.clear();

    unsigned short subdiv = 20;
    float dtheta = 2 * PI / subdiv;

    glm::vec4 point_top(0.0f, 0.5f * height_, radius_, 1.0f),
        point_bottom (0.0f, -0.5f * height_, radius_, 1.0f);
    vector<glm::vec3> cap_top, cap_bottom;

    // top and bottom cap vertices
    for (int i = 0; i < subdiv + 1; ++i) {
        glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), i * dtheta, glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), center_);

        cap_top.push_back(glm::vec3(translate * rotate * point_top));
        cap_bottom.push_back(glm::vec3(translate * rotate * point_bottom));
    }

    //Create top cap.
    for ( int i = 0; i < subdiv - 2; i++) {
        vertices_.push_back(cap_top[0]);
        vertices_.push_back(cap_top[i + 1]);
        vertices_.push_back(cap_top[i + 2]);
    }
    //Create bottom cap.
    for (int i = 0; i < subdiv - 2; i++) {
        vertices_.push_back(cap_bottom[0]);
        vertices_.push_back(cap_bottom[i + 1]);
        vertices_.push_back(cap_bottom[i + 2]);
    }
    //Create barrel
    for (int i = 0; i < subdiv; i++) {
        //Right-side up triangle
        vertices_.push_back(cap_top[i]);
        vertices_.push_back(cap_bottom[i + 1]);
        vertices_.push_back(cap_bottom[i]);
        //Upside-down triangle
        vertices_.push_back(cap_top[i]);
        vertices_.push_back(cap_top[i + 1]);
        vertices_.push_back(cap_bottom[i + 1]);
    }

    // create normals
    glm::vec3 top_centerpoint(0.0f , 0.5f * height_ , 0.0f),
        bottom_centerpoint(0.0f, -0.5f * height_, 0.0f);
    glm::vec3 normal(0, 1, 0);

    // Create top cap.
    for (int i = 0; i < subdiv - 2; i++) {
        normals_.push_back(normal);
        normals_.push_back(normal);
        normals_.push_back(normal);
    }
    // Create bottom cap.
    for (int i = 0; i < subdiv - 2; i++) {
        normals_.push_back(-normal);
        normals_.push_back(-normal);
        normals_.push_back(-normal);
    }

    // Create barrel
    for (int i = 0; i < subdiv; i++) {
        //Right-side up triangle
        normals_.push_back(glm::normalize(cap_top[i] - top_centerpoint));
        normals_.push_back(glm::normalize(cap_bottom[i + 1] - bottom_centerpoint));
        normals_.push_back(glm::normalize(cap_bottom[i] - bottom_centerpoint));
        //Upside-down triangle
        normals_.push_back(glm::normalize(cap_top[i] - top_centerpoint));
        normals_.push_back(glm::normalize(cap_top[i + 1] - top_centerpoint));
        normals_.push_back(glm::normalize(cap_bottom[i + 1] - bottom_centerpoint));
    }

    // indices and colors
    glm::vec3 color (0.6f, 0.6f, 0.6f);
    for (unsigned int i = 0; i < vertices_.size(); ++i) {
        colors_.push_back(color);
    }

    for (unsigned int i = 0; i < vertices_.size(); ++i) {
        indices_.push_back(i);
    }
}

Intersection Cylinder::intersectImpl(const Ray &ray)
{
	Intersection in;
	glm::vec3 normal(0,0,0);
	glm::vec3 intPoint;
	float t=-1,txz1,txz2,ty1,ty2,t1,t2,temp;
	int lab,lab1,lab2;
	in.normal=normal;
	in.t=t;

	if(ray.dir.x==0&&ray.dir.z==0){
		float xz_2=ray.orig.x*ray.orig.x+ray.orig.z*ray.orig.z;
		txz1=-1000000;txz2=1000000;
		if(xz_2>radius_*radius_) return in;
	}
	if(ray.dir.y==0){
		float y=ray.orig.y;
		ty1=-1000000;ty2=1000000;
		if(y>height_/2||y<-height_/2) return in;
	}


	float a,b,c,d;
	a=ray.dir.x*ray.dir.x+ray.dir.z*ray.dir.z;
	b=2*(ray.dir.x*ray.orig.x+ray.dir.z*ray.orig.z);
	c=ray.orig.x*ray.orig.x+ray.orig.z*ray.orig.z-radius_*radius_;
	d=b*b-4*a*c;
	if(d<0) return in;//dt<0
	d=sqrt(d);
	txz1=(-b-d)/(2*a);
	txz2=(-b+d)/(2*a);
	//xz
	if(ray.dir.y>0){
		ty1=(-height_/2-ray.orig.y)/ray.dir.y;
		ty2=(height_/2-ray.orig.y)/ray.dir.y;
	}
	else if(ray.dir.y<0){
		ty2=(-height_/2-ray.orig.y)/ray.dir.y;
		ty1=(height_/2-ray.orig.y)/ray.dir.y;
	}
	//y
	if(txz1>ty1){
		t1=txz1;
		lab1=1;
	}
	else{
		t1=ty1;
		lab1=2;
	}
	//t1
	if(txz2<ty2){
		t2=txz2;
		lab2=1;
	}
	else{
		t2=ty2;
		lab2=2;
	}
	//t2;
	if(t1>t2) return in;//no intersect
	if(t2<0) return in;//negative intersect

	if(t1>0.01){
		intPoint=ray.orig+t1*ray.dir;//normal condition
		t=t1;
		lab=lab1;
	}
	else{
		intPoint=ray.orig+t2*ray.dir;//source inside the cylinder
		t=t2;
		lab=lab2;
	}

	if(lab==2&&intPoint.y>=0) normal.y=1;
	else if(lab==2&&intPoint.y<0) normal.y=-1;
	else if(lab==1){
		normal.x=intPoint.x;
		normal.z=intPoint.z;
		normal=glm::normalize(normal);
	}

	in.normal=normal;
	in.t=t;
	return in;
}