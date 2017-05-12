#include"Object.h"

Object::Object(BWAPI::Unit new_unit, AIBase *new_base)
{
	my_unit = new_unit;
	my_base = new_base;
	build_type = BWAPI::UnitTypes::Unknown;
	base_class = 0;
}

Object::Object(BWAPI::Unit new_unit)
{
	my_unit = new_unit;
	my_base = nullptr;
	build_type = BWAPI::UnitTypes::Unknown;
	base_class = 0;
}

Object::Object()
{
	my_unit = nullptr;
	my_base = nullptr;
	build_type = BWAPI::UnitTypes::Unknown;
	base_class = 0;
}

void Object::setBuildType(BWAPI::UnitType new_build_type)
{
	build_type = new_build_type;
}

void Object::setBaseClass(int new_base_class)
{
	base_class = new_base_class;
}

void Object::clearObject()
{
	my_unit = nullptr;

}

BWAPI::Unit Object::getUnit()
{
	return my_unit;
}

AIBase* Object::getBase()
{
	return my_base;
}

BWAPI::UnitType Object::getBuildType()
{
	return build_type;
}