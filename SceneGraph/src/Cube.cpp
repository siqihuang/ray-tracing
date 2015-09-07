#include "CUBE.h"

static const float PI = 3.141592653589f;

// Creates a unit cylinder centered at (0, 0, 0)
Cube::Cube() : Geometry(CUBE),
    center_(glm::vec3(0.f, 0.f, 0.f)),
    length_(1.0f)
{
    buildGeomtery();
}

Cube::~Cube() {}

void Cube::buildGeomtery()
{
    vertices_.clear();
    colors_.clear();
    normals_.clear();
    indices_.clear();

	vertices_.push_back(glm::vec3(-length_/2,-length_/2,+length_/2));
	vertices_.push_back(glm::vec3(+length_/2,-length_/2,+length_/2));
	vertices_.push_back(glm::vec3(+length_/2,+length_/2,+length_/2));
	vertices_.push_back(glm::vec3(-length_/2,+length_/2,+length_/2));
	vertices_.push_back(glm::vec3(-length_/2,-length_/2,-length_/2));
	vertices_.push_back(glm::vec3(+length_/2,-length_/2,-length_/2));
	vertices_.push_back(glm::vec3(+length_/2,+length_/2,-length_/2));
	vertices_.push_back(glm::vec3(-length_/2,+length_/2,-length_/2));

	glm::vec3 centerP(0,0,0);
	for(int i=0;i<8;i++){
		normals_.push_back(vertices_[i]-centerP);
		colors_.push_back(glm::vec3(0.6,0.6,0.6));
	}

	indices_.push_back(0);indices_.push_back(1);indices_.push_back(2);
	indices_.push_back(0);indices_.push_back(2);indices_.push_back(3);

	indices_.push_back(0);indices_.push_back(3);indices_.push_back(7);
	indices_.push_back(0);indices_.push_back(7);indices_.push_back(4);

	indices_.push_back(4);indices_.push_back(5);indices_.push_back(6);
	indices_.push_back(4);indices_.push_back(6);indices_.push_back(7);

	indices_.push_back(1);indices_.push_back(2);indices_.push_back(6);
	indices_.push_back(1);indices_.push_back(6);indices_.push_back(5);

	indices_.push_back(0);indices_.push_back(4);indices_.push_back(5);
	indices_.push_back(0);indices_.push_back(5);indices_.push_back(1);

	indices_.push_back(3);indices_.push_back(2);indices_.push_back(6);
	indices_.push_back(3);indices_.push_back(6);indices_.push_back(7);
}

Intersection Cube::intersectImpl(const Ray &ray)
{
	Intersection in;
	glm::vec3 normal(0,0,0);
	glm::vec3 intPoint;
	float t=-1,tx1,tx2,ty1,ty2,tz1,tz2,t1,t2,temp;
	int lab1,lab2,lab;
	in.normal=normal;
	in.t=t;
	//cout<<ray.dir.y<<endl;
	if(ray.dir.x==0){
		float x=ray.orig.x;
		tx1=-1000000;tx2=1000000;
		if(x>length_/2||x<-length_/2) return in;
	}
	if(ray.dir.y==0){
		float y=ray.orig.y;
		ty1=-1000000;ty2=1000000;
		if(y>length_/2||y<-length_/2) return in;
	}
	if(ray.dir.z==0){
		float z=ray.orig.z;
		tz1=-1000000;tz2=1000000;
		if(z>length_/2||z<-length_/2) return in;
	}

	if(ray.dir.x>0){
		tx1=(-length_/2-ray.orig.x)/ray.dir.x;
		tx2=(length_/2-ray.orig.x)/ray.dir.x;
	}
	else if(ray.dir.x<0){
		tx2=(-length_/2-ray.orig.x)/ray.dir.x;
		tx1=(length_/2-ray.orig.x)/ray.dir.x;
	}
	//x
	if(ray.dir.y>0){
		ty1=(-length_/2-ray.orig.y)/ray.dir.y;
		ty2=(length_/2-ray.orig.y)/ray.dir.y;
	}
	else if(ray.dir.y<0){
		ty2=(-length_/2-ray.orig.y)/ray.dir.y;
		ty1=(length_/2-ray.orig.y)/ray.dir.y;
	}
	//y
	if(ray.dir.z>0){
		tz1=(-length_/2-ray.orig.z)/ray.dir.z;
		tz2=(length_/2-ray.orig.z)/ray.dir.z;
	}
	else if(ray.dir.z<0){
		tz2=(-length_/2-ray.orig.z)/ray.dir.z;
		tz1=(length_/2-ray.orig.z)/ray.dir.z;
	}
	//z
	if(tx1>ty1){
		t1=tx1;
		lab1=1;
	}
	else{
		t1=ty1;
		lab1=2;
	}
	if(t1<tz1){
		t1=tz1;
		lab1=3;
	}
	//t1
	if(tx2<ty2){
		t2=tx2;
		lab2=1;
	}
	else{
		t2=ty2;
		lab2=2;
	}
	if(t2>tz2){
		t2=tz2;
		lab2=3;
	}
	//t2;
	if(t1>t2) return in;//no intersect
	if(t2<0) return in;//negative intersect

	if(t1>0.001){
		intPoint=ray.orig+t1*ray.dir;//normal condition
		t=t1;
		lab=lab1;
	}
	else{
		intPoint=ray.orig+t2*ray.dir;//source inside the cube
		t=t2;
		lab=lab2;
	}

	/*if(intPoint.x==-length_/2) normal=glm::vec3(-1,0,0);
	else if(intPoint.x==length_/2) normal=glm::vec3(1,0,0);
	else if(intPoint.y==-length_/2) normal=glm::vec3(0,-1,0);
	else if(intPoint.y==length_/2) normal=glm::vec3(0,1,0);
	else if(intPoint.z==-length_/2) normal=glm::vec3(0,0,-1);
	else normal=glm::vec3(0,0,1);*/
	if(lab==1&&intPoint.x>=0) normal.x=1;
	else if(lab==1&&intPoint.x<0) normal.x=-1;
	else if(lab==2&&intPoint.y>=0) normal.y=1;
	else if(lab==2&&intPoint.y<0) normal.y=-1;
	else if(lab==3&&intPoint.z>=0) normal.z=1;
	else if(lab==3&&intPoint.z<0) normal.z=-1;
	
	//if(lab==1&&intPoint.x>=0) cout<<t<<endl;

	in.normal=normal;
	in.t=t;
	//if(in.normal.x==1) cout<<"!"<<endl;
	//cout<<normal.x<<","<<normal.y<<","<<normal.z<<endl;
	//getchar();
	return in;
}