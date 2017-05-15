#include"Object.h"

Object::Object(BWAPI::Unit new_unit, AIBase *new_base)
{
	my_unit = new_unit;
	my_base = new_base;
	build_type = BWAPI::UnitTypes::Unknown;
	base_class = 0;
	my_discovered_position = BWAPI::TilePositions::Invalid;
	is_building = false;
}

Object::Object(BWAPI::Unit new_unit)
{
	my_unit = new_unit;
	my_base = nullptr;
	build_type = BWAPI::UnitTypes::Unknown;
	base_class = 0;
	my_discovered_position = BWAPI::TilePositions::Invalid;
	is_building = false;
}

Object::Object()
{
	my_unit = nullptr;
	my_base = nullptr;
	build_type = BWAPI::UnitTypes::Unknown;
	base_class = 0;
	my_discovered_position = BWAPI::TilePositions::Invalid;
	is_building = false;
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

void Object::addGasWorker(int new_unit_id)
{
	gas_worker_unit_id_list.push_back(new_unit_id);
}

void Object::removeGasWorker(int remove_unit_id)
{
	auto gas_worker_iterator = gas_worker_unit_id_list.begin();
	while (gas_worker_iterator != gas_worker_unit_id_list.end())
	{
		if (*gas_worker_iterator == remove_unit_id)
		{
			gas_worker_unit_id_list.erase(gas_worker_iterator);
			return;
		}
		else
		{
			gas_worker_iterator++;
		}
	}
}

int Object::getNumberGasWorkers()
{
	return gas_worker_unit_id_list.size();
}

void Object::setDiscoveredPosition(BWAPI::TilePosition new_position)
{
	my_discovered_position = new_position;
}

BWAPI::TilePosition Object::getDiscoveredPosition()
{
	return my_discovered_position;
}

void Object::setIsBuilding()
{
	is_building = true;
}

bool Object::isBuilding()
{
	return is_building;
}