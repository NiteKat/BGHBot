#include"Object.h"

Object::Object(BWAPI::Unit new_unit, AIBase *new_base)
{
	my_unit = new_unit;
	my_base = new_base;
	build_type = BWAPI::UnitTypes::Unknown;
	base_class = 0;
	my_discovered_position = BWAPI::TilePositions::Invalid;
	is_building = false;
	elapsed_time_order_given = 0;
	target_base = nullptr;
	my_resource = nullptr;
}

Object::Object(BWAPI::Unit new_unit)
{
	my_unit = new_unit;
	my_base = nullptr;
	build_type = BWAPI::UnitTypes::Unknown;
	base_class = 0;
	my_discovered_position = BWAPI::TilePositions::Invalid;
	is_building = false;
	elapsed_time_order_given = 0;
	target_base = nullptr;
	my_resource = nullptr;
}

Object::Object()
{
	my_unit = nullptr;
	my_base = nullptr;
	build_type = BWAPI::UnitTypes::Unknown;
	base_class = 0;
	my_discovered_position = BWAPI::TilePositions::Invalid;
	is_building = false;
	elapsed_time_order_given = 0;
	target_base = nullptr;
	my_resource = nullptr;
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

void Object::setElapsedTimeOrderGiven(int new_elapsed_time)
{
	elapsed_time_order_given = new_elapsed_time;
}

int Object::getElapsedTimeOrderGiven()
{
	return elapsed_time_order_given;
}

void Object::setTargetBase(AIBase *new_target_base)
{
	target_base = new_target_base;
}

AIBase* Object::getTargetBase()
{
	return target_base;
}

void Object::setBase(AIBase *new_base)
{
	my_base = new_base;
}

void Object::setTargetBuildLocation(BWAPI::TilePosition new_target_build_location)
{
	target_build_location = new_target_build_location;
}

BWAPI::TilePosition Object::getTargetBuildLocation()
{
	return target_build_location;
}

void Object::setRepairTarget(BWAPI::Unit new_repair_target)
{
	my_repair_target = new_repair_target;
}

BWAPI::Unit Object::getRepairTarget()
{
	return my_repair_target;
}

BWAPI::Unit Object::getResourceTarget()
{
	if (my_resource != nullptr)
	{
		if (my_resource->exists())
			return my_resource;
		else
		{
			my_resource = nullptr;
			return nullptr;
		}
	}
	else
		return nullptr;
}

void Object::setResourceTarget(BWAPI::Unit resource)
{
	my_resource = resource;
}