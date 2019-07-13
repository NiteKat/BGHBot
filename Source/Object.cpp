#include"Object.h"
#include"FAP.h"

Object::Object(BWAPI::Unit new_unit, AIBase *new_base) :
  my_unit(new_unit),
  my_resource(nullptr),
  my_player(new_unit->getPlayer()),
  my_repair_target(nullptr)
{
	my_base = new_base;
	build_type = BWAPI::UnitTypes::Unknown;
	base_class = 0;
	my_discovered_position = BWAPI::TilePositions::Invalid;
	is_building = false;
	elapsed_time_order_given = 0;
	target_base = nullptr;
	my_type = new_unit->getType();
	current_hit_points = new_unit->getHitPoints();
	max_hit_points = my_type.maxHitPoints();
	current_position = new_unit->getPosition();
	current_shields = new_unit->getShields();
	max_shields = my_type.maxShields();
	remaining_attack_cooldown = new_unit->getGroundWeaponCooldown();
}

Object::Object(BWAPI::Unit new_unit) :
  my_unit(new_unit),
  my_resource(nullptr),
  my_player(new_unit->getPlayer()),
  my_repair_target(nullptr)
{
	my_base = nullptr;
	build_type = BWAPI::UnitTypes::Unknown;
	base_class = 0;
	my_discovered_position = BWAPI::TilePositions::Invalid;
	is_building = false;
	elapsed_time_order_given = 0;
	target_base = nullptr;
	my_type = new_unit->getType();
	current_hit_points = new_unit->getHitPoints();
	max_hit_points = my_type.maxHitPoints();
	current_position = new_unit->getPosition();
	current_shields = new_unit->getShields();
	max_shields = my_type.maxShields();
	remaining_attack_cooldown = new_unit->getGroundWeaponCooldown();
}

Object::Object(const Neolib::FAPUnit &fu) :
  my_unit(nullptr),
  my_resource(nullptr),
  my_player(fu.player),
  my_repair_target(nullptr)
{
	my_base = nullptr;
	build_type = BWAPI::UnitTypes::Unknown;
	base_class = 0;
	my_discovered_position = BWAPI::TilePositions::Invalid;
	is_building = false;
	elapsed_time_order_given = 0;
	target_base = nullptr;
	my_type = fu.unitType;
	current_hit_points = fu.health;
	max_hit_points = fu.maxHealth;
	current_position = {fu.x, fu.y};
	current_shields = fu.shields;
	max_shields = fu.maxShields;
	remaining_attack_cooldown = fu.groundCooldown;
}

Object::Object() :
  my_unit(nullptr),
  my_resource(nullptr),
  my_player(nullptr),
  my_repair_target(nullptr)
{
	my_base = nullptr;
	build_type = BWAPI::UnitTypes::Unknown;
	base_class = 0;
	my_discovered_position = BWAPI::TilePositions::Invalid;
	is_building = false;
	elapsed_time_order_given = 0;
	target_base = nullptr;
	my_type = BWAPI::UnitTypes::None;
	current_hit_points = 0;
	max_hit_points = 0;
	current_position = BWAPI::Position(0, 0);
	current_shields = 0;
	max_shields = 0;
	remaining_attack_cooldown = 0;
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

int Object::getCurrentHitPoints()
{
	return current_hit_points;
}

int Object::getMaxHitPoints()
{
	return max_hit_points;
}

BWAPI::Position Object::getCurrentPosition()
{
	return current_position;
}

int Object::getCurrentShields()
{
	return current_shields;
}

int Object::getMaxShields()
{
	return max_shields;
}

BWAPI::UnitType Object::getType()
{
	return my_type;
}

int Object::getRemainingAttackCooldown()
{
	return remaining_attack_cooldown;
}

bool Object::expired()
{
	if (my_unit.getGame()->getFrameCount() - last_frame_seen >= 1920)
		return true;
	else
		return false;
}

void Object::updateObject()
{
	if (my_unit->exists())
	{
		current_hit_points = my_unit->getHitPoints();
		max_hit_points = my_type.maxHitPoints();
		current_position = my_unit->getPosition();
		current_shields = my_unit->getShields();
		max_shields = my_type.maxShields();
		remaining_attack_cooldown = my_unit->getGroundWeaponCooldown();
		my_player = my_unit->getPlayer();
		last_frame_seen = my_unit.getGame()->getFrameCount();
	}
	else
	{
		if (my_unit.getGame()->isVisible((BWAPI::TilePosition)current_position))
		{
			current_position = BWAPI::Positions::Unknown;
		}
	}
}

BWAPI::Player Object::getPlayer()
{
	return my_player;
}