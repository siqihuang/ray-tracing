#include "octree.h"

octree::octree(){}

octree::octree(int depth,float xmax,float xmin,float ymax,float ymin,float zmax,float zmin){
	this->depth=depth;
	this->xmax=xmax;
	this->xmin=xmin;
	this->ymax=ymax;
	this->ymin=ymin;
	this->zmax=zmax;
	this->zmin=zmin;
	xlength=xmax-xmin;
	ylength=ymax-ymin;
	zlength=zmax-zmin;
	xhalf=(xmax+xmin)/2;
	yhalf=(ymax+ymin)/2;
	zhalf=(zmax+zmin)/2;
	createTree(depth,xmax,xmin,ymax,ymin,zmax,zmin);
}

octree::~octree(){}

void octree::createTree(int depth,float xmax,float xmin,float ymax,float ymin,float zmax,float zmin){
	if(depth>1){
		ulf=new octree(depth-1,xhalf,xmin,ymax,yhalf,zmax,zhalf);
		ulb=new octree(depth-1,xhalf,xmin,ymax,yhalf,zhalf,zmin);
		urf=new octree(depth-1,xmax,xhalf,ymax,yhalf,zmax,zhalf);
		urb=new octree(depth-1,xmax,xhalf,ymax,yhalf,zhalf,zmin);
		dlf=new octree(depth-1,xhalf,xmin,yhalf,ymin,zmax,zhalf);
		dlb=new octree(depth-1,xhalf,xmin,yhalf,ymin,zhalf,zmin);
		drf=new octree(depth-1,xmax,xhalf,yhalf,ymin,zmax,zhalf);
		drb=new octree(depth-1,xmax,xhalf,yhalf,ymin,zhalf,zmin);
	}
	else{
		ulf=nullptr;
		ulb=nullptr;
		urf=nullptr;
		urb=nullptr;
		dlf=nullptr;
		dlb=nullptr;
		drf=nullptr;
		drb=nullptr;
	}
}

void octree::addTriangle(int index,glm::vec3 p1,glm::vec3 p2,glm::vec3 p3){
	float xmax,xmin,ymax,ymin,zmax,zmin;
	if(urf==nullptr){//leaf
		mesh.push_back(index);
		return;
	}
	
	if(p1.x>p2.x){
		xmax=p1.x;
		xmin=p2.x;
	}
	else{
		xmax=p2.x;
		xmin=p1.x;
	}
	if(xmax<p3.x) xmax=p3.x;
	if(xmin>p3.x) xmin=p3.x;
	//x
	if(p1.y>p2.y){
		ymax=p1.y;
		ymin=p2.y;
	}
	else{
		ymax=p2.y;
		ymin=p1.y;
	}
	if(ymax<p3.y) ymax=p3.y;
	if(ymin>p3.y) ymin=p3.y;
	//y
	if(p1.z>p2.z){
		zmax=p1.z;
		zmin=p2.z;
	}
	else{
		zmax=p2.z;
		zmin=p1.z;
	}
	if(zmax<p3.z) zmax=p3.z;
	if(zmin>p3.z) zmin=p3.z;
	//z
	
	if(xmax>=xhalf&&xmin<xhalf){//cannot split
		mesh.push_back(index);
		return;
	}
	if(ymax>=yhalf&&ymin<yhalf){//cannot split
		mesh.push_back(index);
		return;
	}
	if(zmax>=zhalf&&zmin<zhalf){//cannot split
		mesh.push_back(index);
		return;
	}
	//can split below
	if(xmin>=xhalf&&ymin>=yhalf&&zmin>=zhalf)
		urf->addTriangle(index,p1,p2,p3);
	else if(xmin>=xhalf&&ymin>=yhalf&&zmax<zhalf)
		urb->addTriangle(index,p1,p2,p3);
	else if(xmin>=xhalf&&ymax<yhalf&&zmin>=zhalf)
		drf->addTriangle(index,p1,p2,p3);
	else if(xmin>=xhalf&&ymax<yhalf&&zmax<zhalf)
		drb->addTriangle(index,p1,p2,p3);
	else if(xmax<xhalf&&ymin>=yhalf&&zmin>=zhalf)
		ulf->addTriangle(index,p1,p2,p3);
	else if(xmax<xhalf&&ymin>=yhalf&&zmax<zhalf)
		ulb->addTriangle(index,p1,p2,p3);
	else if(xmax<xhalf&&ymax<yhalf&&zmin>=zhalf)
		dlf->addTriangle(index,p1,p2,p3);
	else if(xmax<xhalf&&ymax<yhalf&&zmax<zhalf)
		dlb->addTriangle(index,p1,p2,p3);
}

int octree::countTriangle(octree *root){
	int num=0;
	num+=root->mesh.size();
	if(root->dlb!=nullptr){
		num+=countTriangle(root->dlb);
		num+=countTriangle(root->dlf);
		num+=countTriangle(root->drb);
		num+=countTriangle(root->drf);
		num+=countTriangle(root->ulb);
		num+=countTriangle(root->ulf);
		num+=countTriangle(root->urb);
		num+=countTriangle(root->urf);
	}
	return num;
}