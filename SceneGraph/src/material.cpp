#include "material.h"

material::material(){}

material::material(std::string n){
	name=n;
}

material::~material(){}

std::string material::getName(){
	return name;
}

glm::vec3 material::getDIFF(){
	return DIFF;
}

glm::vec3 material::getREFL(){
	return REFL;
}

float material::getEXPO(){
	return EXPO;
}

float material::getIOR(){
	return IOR;
}

float material::getABSO(){
	return ABSO;
}

float material::getEMITTANCE(){
	return EMITTANCE;
}

bool material::isMIRR(){
	return MIRR;
}

bool material::isTRAN(){
	return TRAN;
}

bool material::isEMIT(){
	return EMIT;
}

void material::setDIFF(glm::vec3 diff){
	DIFF=diff;
}

void material::setREFL(glm::vec3 refl){
	REFL=refl;
}

void material::setEXPO(float expo){
	EXPO=expo;
}

void material::setIOR(float ior){
	IOR=ior;
}

void material::setMIRR(bool mirr){
	MIRR=mirr;
}

void material::setTRAN(bool tran){
	TRAN=tran;
}

void material::setEMIT(bool emited){
	EMIT=emited;
}

void material::setABSO(float abso){
	ABSO=abso;
}

void material::setEMITTANCE(float emittance){
	EMITTANCE=emittance;
}