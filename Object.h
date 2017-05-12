#ifndef OBJECT
#define OBJECT
#include<BWAPI.h>
#include"BWEM 1.3.1/src/bwem.h"
#include "AIBase.h"

class Object
{
public:
	Object(BWAPI::Unit new_unit, AIBase *my_base);
	Object(BWAPI::Unit new_unit);
	Object();

	void setBuildType(BWAPI::UnitType new_build_type);
	void setBaseClass(int new_base_class);
	void clearObject();

	BWAPI::Unit getUnit();
	AIBase *getBase();
	BWAPI::UnitType getBuildType();

private:
	BWAPI::Unit my_unit;
	AIBase *my_base;
	BWAPI::UnitType build_type;
	int base_class;
};

#endif