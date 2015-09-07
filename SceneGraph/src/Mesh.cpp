#include "Mesh.h"

mesh::mesh() : Geometry(MESH){
	radius=0;
	center=glm::vec3(0,0,0);
}

mesh::~mesh(){}

void mesh::saveVertex(glm::vec3 vertex){
	ver.push_back(vertex);
}

void mesh::addVertex(glm::vec3 vertex){
	vertices_.push_back(vertex);
}

void mesh::addColor(glm::vec3 color){
	colors_.push_back(color);
}

void mesh::saveNormal(glm::vec3 normal){
	nor.push_back(normal);
}

void mesh::addNormal(glm::vec3 normal){
	normals_.push_back(normal);
}

void mesh::addVertexIndex(unsigned int index){
	indices_.push_back(index);
}

void mesh::addNormalIndex(unsigned int index){
	normalIdx.push_back(index);
}

void mesh::addTextureIndex(unsigned int index){
	textureIdx.push_back(index);
}

void mesh::buildGeomtery(){

}

void mesh::computeNormal(bool usVn){
	glm::vec3 v1,v2;
	cout<<vertices_.size()<<endl;
	cout<<normals_.size()<<endl;
	cout<<indices_.size()<<endl;
	vertices_.clear();
	for(int i=0;i<indices_.size();i+=3){
		vertices_.push_back(ver[indices_[i]]);
		vertices_.push_back(ver[indices_[i+1]]);
		vertices_.push_back(ver[indices_[i+2]]);
		//cout<<"vertex"<<endl;
		//getchar();
		colors_.push_back(glm::vec3(0.6,0.6,0.6));
		colors_.push_back(glm::vec3(0.6,0.6,0.6));
		colors_.push_back(glm::vec3(0.6,0.6,0.6));
		
		if(!usVn){
			v1=vertices_[i+2]-vertices_[i+1];
			v2=vertices_[i]-vertices_[i+1];
			normals_.push_back(glm::cross(v1,v2));
			normals_.push_back(glm::cross(v1,v2));
			normals_.push_back(glm::cross(v1,v2));
		}
		else{
			normals_.push_back(nor[normalIdx[i]]);
			normals_.push_back(nor[normalIdx[i+1]]);
			normals_.push_back(nor[normalIdx[i+2]]);
		}
	}
	indices_.clear();
	for(int i=0;i<vertices_.size();i++)
		indices_.push_back(i);
	if(usVn) cout<<"not compute"<<endl;
	else cout<<"compute"<<endl;
}

void mesh::averageNormal(bool usVn){
	glm::vec3 v1,v2;
	float xmax,xmin,ymax,ymin,zmax,zmin;
	xmax=ymax=zmax=-1e10;xmin=ymin=zmin=1e10;
	for(int i=0;i<ver.size();i++){
		vertices_.push_back(ver[i]);
		normals_.push_back(glm::vec3(0,0,0));
		colors_.push_back(glm::vec3(0.6,0.6,0.6));
		if(xmax<ver[i].x) xmax=ver[i].x;
		if(xmin>ver[i].x) xmin=ver[i].x;
		if(ymax<ver[i].y) ymax=ver[i].y;
		if(ymin>ver[i].y) ymin=ver[i].y;
		if(zmax<ver[i].z) zmax=ver[i].z;
		if(zmin>ver[i].z) zmin=ver[i].z;
	}

	//otree=new octree(4,xmax,xmin,ymax,ymin,zmax,zmin);//create octree

	/*vector<unsigned int> *verIdx=new vector<unsigned int>;
	for(int i=0;i<indices_.size()/3;i++) verIdx->push_back(i);
	ktree=new kdtree(0,xmax,xmin,ymax,ymin,zmax,zmin,verIdx);
	vector<glm::vec3> *ver=&vertices_;
	vector<unsigned int> *idx=&indices_;
	ktree->createTree(ver,idx);*/
	
	for(int i=0;i<indices_.size();i+=3){
		v1=vertices_[indices_[i+2]]-vertices_[indices_[i+1]];
		v2=vertices_[indices_[i]]-vertices_[indices_[i+1]];
		normals_[indices_[i]]=normals_[indices_[i]]+glm::cross(v1,v2);
		normals_[indices_[i+1]]=normals_[indices_[i+1]]+glm::cross(v1,v2);
		normals_[indices_[i+2]]=normals_[indices_[i+2]]+glm::cross(v1,v2);
		//otree->addTriangle(i/3,ver[indices_[i]],ver[indices_[i+1]],ver[indices_[i+2]]);
	}
	for(int i=0;i<vertices_.size();i++){
		normals_[i]=glm::normalize(normals_[i]);
	}
}

void mesh::initTreeStructure(char type){
	volatile DWORD start;
	treeType=type;
	float xmax,xmin,ymax,ymin,zmax,zmin;
	xmax=ymax=zmax=-1e10;xmin=ymin=zmin=1e10;
	for(int i=0;i<ver.size();i++){
		if(xmax<ver[i].x) xmax=ver[i].x;
		if(xmin>ver[i].x) xmin=ver[i].x;
		if(ymax<ver[i].y) ymax=ver[i].y;
		if(ymin>ver[i].y) ymin=ver[i].y;
		if(zmax<ver[i].z) zmax=ver[i].z;
		if(zmin>ver[i].z) zmin=ver[i].z;
	}
	if(type=='o'){//octree
		start=GetTickCount();
		otree=new octree(7,xmax,xmin,ymax,ymin,zmax,zmin);//create octree
		for(int i=0;i<indices_.size();i+=3){
			otree->addTriangle(i/3,ver[indices_[i]],ver[indices_[i+1]],ver[indices_[i+2]]);
		}
		cout<<"creating time: "<<GetTickCount()-start<<" ms"<<endl;
	}
	else{//kdtree
		start=GetTickCount();
		vector<unsigned int> *verIdx=new vector<unsigned int>;
		for(int i=0;i<indices_.size()/3;i++) verIdx->push_back(i);
		ktree=new kdtree(0,xmax,xmin,ymax,ymin,zmax,zmin,verIdx);
		vector<glm::vec3> *ver=&vertices_;
		vector<unsigned int> *idx=&indices_;
		ktree->createTree(ver,idx);
		cout<<"creating time: "<<GetTickCount()-start<<" ms"<<endl;
	}
}

void mesh::computeBoundingSphere(){
	float num=ver.size(),max_length=-1,length;
	glm::vec3 dis;
	for(unsigned int i=0;i<ver.size();i++)
		center=center+ver[i];
	center=center/num;
	for(unsigned int i=0;i<ver.size();i++){
		dis=ver[i]-center;
		length=glm::length(dis);
		if(length>max_length){
			max_length=length;
		}
	}
	radius=max_length;
}

bool mesh::sphereImpl(const Ray &ray)const{
	Intersection in;
	glm::vec3 normal(0,0,0);
	glm::vec3 intPoint;
	float t=-1,t1,t2,temp;
	in.normal=normal;
	in.t=t;

	if(ray.dir.x==0&&ray.dir.y==0&&ray.dir.z==0){
		float r=ray.orig.x*ray.orig.x+ray.orig.y*ray.orig.y+ray.orig.z*ray.orig.z;
		if(fabs(r-radius*radius)>=0.0001) return false;
	}

	float a,b,c,d;
	a=ray.dir.x*ray.dir.x+ray.dir.y*ray.dir.y+ray.dir.z*ray.dir.z;
	b=2*(ray.dir.x*ray.orig.x+ray.dir.y*ray.orig.y+ray.dir.z*ray.orig.z);
	c=ray.orig.x*ray.orig.x+ray.orig.y*ray.orig.y+ray.orig.z*ray.orig.z-radius*radius;
	d=b*b-4*a*c;
	if(d<0) return false;
	d=sqrt(d);
	//xyz
	t1=(-b-d)/(2*a);
	t2=(-b+d)/(2*a);
	//if(t1>t2) return in;//no intersect
	if(t2<0) return false;//negative intersect
	return true;

	/*if(t1>0.001){
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
	return in;*/
}

void mesh::kdIntersect(const Ray &ray,kdtree *root,vector<unsigned int> &treeIdx){
	if(!cubeImpl(ray,root->xmax,root->xmin,root->ymax,root->ymin,root->zmax,root->zmin)) return;//not intersect
	//cout<<"1"<<endl;
	if(root->lc==nullptr) treeIdx.push_back(root->mesh);
	else{
		if(root->rc==nullptr&&root->lc!=nullptr) cout<<"###"<<endl;
		kdIntersect(ray,root->lc,treeIdx);
		kdIntersect(ray,root->rc,treeIdx);
	}
}

void mesh::octIntersect(const Ray &ray,octree *root,vector<unsigned int> &octreeIdx){
	float xhalf,yhalf,zhalf;
	xhalf=(root->xmax+root->xmin)/2;yhalf=(root->ymax+root->ymin)/2;zhalf=(root->zmax+root->zmin)/2;
	if(!cubeImpl(ray,root->xmax,root->xmin,root->ymax,root->ymin,root->zmax,root->zmin)) return;//not intersect
	for(int i=0;i<root->mesh.size();i++)
		octreeIdx.push_back(root->mesh[i]);
	if(root->dlb!=nullptr){
		octIntersect(ray,root->dlb,octreeIdx);
		octIntersect(ray,root->dlf,octreeIdx);
		octIntersect(ray,root->drb,octreeIdx);
		octIntersect(ray,root->drf,octreeIdx);
		octIntersect(ray,root->ulb,octreeIdx);
		octIntersect(ray,root->ulf,octreeIdx);
		octIntersect(ray,root->urb,octreeIdx);
		octIntersect(ray,root->urf,octreeIdx);
	}
}

bool mesh::cubeImpl(const Ray &ray,float xmax,float xmin,float ymax,float ymin,float zmax,float zmin)const{
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
		if(x>xmax||x<xmin) return false;
	}
	if(ray.dir.y==0){
		float y=ray.orig.y;
		ty1=-1000000;ty2=1000000;
		if(y>ymax||y<ymin) return false;
	}
	if(ray.dir.z==0){
		float z=ray.orig.z;
		tz1=-1000000;tz2=1000000;
		if(z>zmax||z<zmin) return false;
	}

	if(ray.dir.x>0){
		tx1=(xmin-ray.orig.x)/ray.dir.x;
		tx2=(xmax-ray.orig.x)/ray.dir.x;
	}
	else if(ray.dir.x<0){
		tx2=(xmin-ray.orig.x)/ray.dir.x;
		tx1=(xmax-ray.orig.x)/ray.dir.x;
	}
	//x
	if(ray.dir.y>0){
		ty1=(ymin-ray.orig.y)/ray.dir.y;
		ty2=(ymax-ray.orig.y)/ray.dir.y;
	}
	else if(ray.dir.y<0){
		ty2=(ymin-ray.orig.y)/ray.dir.y;
		ty1=(ymax-ray.orig.y)/ray.dir.y;
	}
	//y
	if(ray.dir.z>0){
		tz1=(zmin-ray.orig.z)/ray.dir.z;
		tz2=(zmax-ray.orig.z)/ray.dir.z;
	}
	else if(ray.dir.z<0){
		tz2=(zmin-ray.orig.z)/ray.dir.z;
		tz1=(zmax-ray.orig.z)/ray.dir.z;
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
	if(t1>t2) return false;//no intersect
	if(t2<0) return false;//negative intersect
	return true;
}

Intersection mesh::intersectImpl(const Ray &ray)
{
	Intersection in;
	glm::vec3 p1,p2,p3,n1,n2,n3;
	vector<unsigned int> treeIdx;
	float t=-1;
	in.t=-1;
	//if(!sphereImpl(ray)) return in;
	if(treeType=='o') octIntersect(ray,otree,treeIdx);
	else kdIntersect(ray,ktree,treeIdx);
	for(int i=0;i<treeIdx.size();i++){
	//for(int i=0;i<indices_.size();i+=3){
		Intersection temp;
		temp.t=-1;
		/*p1=vertices_[indices_[i]];
		p2=vertices_[indices_[i+1]];
		p3=vertices_[indices_[i+2]];
		n1=normals_[indices_[i]];
		n2=normals_[indices_[i+1]];
		n3=normals_[indices_[i+2]];*/
		p1=vertices_[indices_[3*treeIdx[i]]];
		p2=vertices_[indices_[3*treeIdx[i]+1]];
		p3=vertices_[indices_[3*treeIdx[i]+2]];
		n1=normals_[indices_[3*treeIdx[i]]];
		n2=normals_[indices_[3*treeIdx[i]+1]];
		n3=normals_[indices_[3*treeIdx[i]+2]];
		temp=intersectImplTri(ray,p1,p2,p3,n1,n2,n3);
		//temp=intersectImplTriTest(ray,p1,p2,p3);
		if(temp.t!=-1){
			/*cout<<temp.normal.x<<","<<temp.normal.y<<","<<temp.normal.z<<endl;
			cout<<temp.t<<endl;
			getchar();*/
		}
		if(temp.t>1e-3&&(in.t<0||temp.t<in.t)){
			in.t=temp.t;
			in.normal.x=temp.normal.x;
			in.normal.y=temp.normal.y;
			in.normal.z=temp.normal.z;
		}
	}
	//if(in.t!=-1){ cout<<in.normal.x<<","<<in.normal.y<<","<<in.normal.z<<"!"<<endl;getchar();}
	treeIdx.clear();
	return in;
}

Intersection mesh::intersectImplTri(const Ray &ray,glm::vec3 p1,glm::vec3 p2,glm::vec3 p3,
									glm::vec3 n1,glm::vec3 n2,glm::vec3 n3)const
{
	Intersection in;
	//cout<<ray.dir.x<<","<<ray.dir.y<<","<<ray.dir.z<<endl;
	//cout<<ray.orig.x<<","<<ray.orig.y<<","<<ray.orig.z<<endl;
	glm::vec3 normal(0,0,0);
	float t=-1;
	in.normal=normal;
	in.t=t;
	glm::vec3 v1,v2,v;
	v1=p2-p1;v2=p3-p2;
	v=glm::cross(v1,v2);
	if(v.length()<1e-15) return in;
	float d=-(ray.orig.x-p2.x)*v.x-(ray.orig.y-p2.y)*v.y-(ray.orig.z-p2.z)*v.z;
	float s=v.x*ray.dir.x+v.y*ray.dir.y+v.z*ray.dir.z;
	if(fabs(s)<1e-15) return in;//not intersect
	t=d/s;
	
	if(t<0) return in;//negative intersect
	//cout<<t<<endl;
	float r1,r2,r3;
	const float PI=3.1415926;
	glm::vec3 center=ray.orig+t*ray.dir;
	//cout<<"center"<<center.x<<","<<center.y<<","<<center.z<<endl;
	glm::vec3 c1,c2,c3;
	c1=p1-center;c2=p2-center;c3=p3-center;
	r1=(glm::dot(c1,c1)+glm::dot(c2,c2)-glm::dot(p2-p1,p2-p1))/(2*glm::length(c1)*glm::length(c2));
	r2=(glm::dot(c1,c1)+glm::dot(c3,c3)-glm::dot(p3-p1,p3-p1))/(2*glm::length(c1)*glm::length(c3));
	r3=(glm::dot(c2,c2)+glm::dot(c3,c3)-glm::dot(p2-p3,p2-p3))/(2*glm::length(c2)*glm::length(c3));
	/*if(r1>1) r1=1;
	if(r1<-1) r1=-1;
	if(r2>1) r2=1;
	if(r2<-1) r2=-1;
	if(r3>1) r3=1;
	if(r3<-1) r3=-1;
	r1=acos(r1);
	r2=acos(r2);
	r3=acos(r3);
	if(fabs(r1+r2+r3-2*PI)>0.001) return in;//not intersect;
	*/
	glm::vec3 edge1=p2-p1;
	glm::vec3 edge2=p3-p1;
	glm::vec3 p=ray.orig+t*ray.dir;
	float e11=glm::dot(edge1,edge1);
	float e12=glm::dot(edge1,edge2);
	float e22=glm::dot(edge2,edge2);
	glm::vec3 w=p-p1;
	float we1=glm::dot(w,edge1);
	float we2=glm::dot(w,edge2);

	float D=e12*e12-e11*e22;
	s=(e12*we2-e22*we1)/D;
	float t0=(e12*we1-e11*we2)/D;
	if(s<0||s>1||t0<0||(s+t0)>1){
		return in;
	}
	in.normal=glm::normalize(v);
	in.t=t;

	float t1,t2;
	glm::vec3 v0=-c1,p4;
	v0=glm::normalize(v0);
	s=((p3.x-p2.x)*v0.y+(p2.y-p3.y)*v0.x);
	d=(p1.x-p3.x)*(p1.y-p2.y)-(p1.x-p2.x)*(p1.y-p3.y);
	if(fabs(s)<1e-5){
		s=((p3.x-p2.x)*v0.z+(p2.z-p3.z)*v0.x);
		d=(p1.x-p3.x)*(p1.z-p2.z)-(p1.x-p2.x)*(p1.z-p3.z);
	}
	t1=d/s;
	p4=p1+t1*v0;

	s=p4.x-p2.x;
	d=p3.x-p4.x;
	if(fabs(s)<1e-5){
		s=p4.y-p2.y;
		d=p3.y-p4.y;
		if(fabs(s)<1e-5){
			s=p4.z-p2.z;
			d=p3.z-p4.z;
		}
	}
	t2=d/s;
	t2=t2/(1+t2);

	s=p4.x-center.x;
	d=center.x-p1.x;
	if(fabs(s)<1e-5){
		s=p4.y-center.y;
		d=center.y-p1.y;
		if(fabs(s)<1e-5){
			s=p4.z-center.z;
			d=center.z-p1.z;
		}
	}
	t1=d/s;
	t1=1/(1+t1);
	in.normal=n2*t2+(1-t2)*n3;
	in.normal=glm::normalize(in.normal);
	in.normal=n1*t1+(1-t1)*in.normal;
	in.t=t;
	return in;
}

Intersection mesh::intersectImplTriTest(const Ray &ray,glm::vec3 p1,glm::vec3 p2,glm::vec3 p3)const
{
	Intersection in;
	glm::vec3 normal(0,0,0);
	float t=-1;
	in.normal=normal;
	in.t=t;
	glm::vec3 v1,v2,v;
	v1=p2-p1;v2=p3-p2;
	v=glm::cross(v1,v2);
	if(v.length()<1e-15) return in;
	float d=-(ray.orig.x-p2.x)*v.x-(ray.orig.y-p2.y)*v.y-(ray.orig.z-p2.z)*v.z;
	float s=v.x*ray.dir.x+v.y*ray.dir.y+v.z*ray.dir.z;
	if(fabs(s)<1e-15) return in;//not intersect
	t=d/s;
	
	if(t<0) return in;//negative intersect
	/*float r1,r2,r3;
	const float PI=3.1415926;
	glm::vec3 center=ray.orig+t*ray.dir;
	glm::vec3 c1,c2,c3;
	c1=p1-center;c2=p2-center;c3=p3-center;
	r1=(glm::dot(c1,c1)+glm::dot(c2,c2)-glm::dot(p2-p1,p2-p1))/(2*glm::length(c1)*glm::length(c2));
	r2=(glm::dot(c1,c1)+glm::dot(c3,c3)-glm::dot(p3-p1,p3-p1))/(2*glm::length(c1)*glm::length(c3));
	r3=(glm::dot(c2,c2)+glm::dot(c3,c3)-glm::dot(p2-p3,p2-p3))/(2*glm::length(c2)*glm::length(c3));
	if(r1>1) r1=1;
	if(r1<-1) r1=-1;
	if(r2>1) r2=1;
	if(r2<-1) r2=-1;
	if(r3>1) r3=1;
	if(r3<-1) r3=-1;
	r1=acos(r1);
	r2=acos(r2);
	r3=acos(r3);
	if(fabs(r1+r2+r3-2*PI)>0.0001) return in;//not intersect;
	in.normal=glm::normalize(v);
	in.t=t;
	return in;*/
	glm::vec3 edge1=p2-p1;
	glm::vec3 edge2=p3-p1;
	glm::vec3 p=ray.orig+t*ray.dir;
	float e11=glm::dot(edge1,edge1);
	float e12=glm::dot(edge1,edge2);
	float e22=glm::dot(edge2,edge2);
	glm::vec3 w=p-p1;
	float we1=glm::dot(w,edge1);
	float we2=glm::dot(w,edge2);

	float D=e12*e12-e11*e22;
	s=(e12*we2-e22*we1)/D;
	float t0=(e12*we1-e11*we2)/D;
	if(s<0||s>1||t0<0||(s+t0)>1){
		return in;
	}
	in.normal=glm::normalize(v);
	in.t=t;
	return in;
}