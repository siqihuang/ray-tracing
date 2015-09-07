#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <glm/glm.hpp>
#include <string>

class material{
private:
	std::string name;
	glm::vec3 DIFF,REFL;
	float EXPO,IOR,ABSO,EMITTANCE;
	bool MIRR,TRAN,EMIT;
public:
	material();
	material(std::string n);
	~material();
	std::string getName();
	glm::vec3 getDIFF();
	glm::vec3 getREFL();
	float getEXPO();
	float getIOR();
	float getABSO();
	float getEMITTANCE();
	bool isMIRR();
	bool isTRAN();
	bool isEMIT();
	void setDIFF(glm::vec3 diff);
	void setREFL(glm::vec3 refl);
	void setEXPO(float expo);
	void setIOR(float ior);
	void setMIRR(bool mirr);
	void setTRAN(bool tran);
	void setEMIT(bool emited);
	void setABSO(float abso);
	void setEMITTANCE(float emittance);
};

#endif