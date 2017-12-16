#ifndef OBJECT
#define OBJECT
#include<BWAPI.h>
#include"BWEM 1.3.1/src/bwem.h"
#include "AIBase.h"
#include<vector>

class Object
{
public:
	Object(BWAPI::Unit new_unit, AIBase *my_base);
	Object(BWAPI::Unit new_unit);
	Object();

	void setBuildType(BWAPI::UnitType new_build_type);
	void setBaseClass(int new_base_class);
	void clearObject();
	void addGasWorker(int new_unit_id);
	void removeGasWorker(int remove_unit_id);
	void setDiscoveredPosition(BWAPI::TilePosition new_position);
	void setIsBuilding();
	void setElapsedTimeOrderGiven(int new_elapsed_time);
	void setTargetBase(AIBase *new_target_base);
	void setBase(AIBase *new_base);
	void setTargetBuildLocation(BWAPI::TilePosition new_target_build_location);
	void setRepairTarget(BWAPI::Unit new_repair_target);
	void setResourceTarget(BWAPI::Unit resource);

	BWAPI::Unit getUnit();
	AIBase *getBase();
	BWAPI::UnitType getBuildType();
	int getNumberGasWorkers();
	BWAPI::TilePosition getDiscoveredPosition();
	bool isBuilding();
	int getElapsedTimeOrderGiven();
	AIBase *getTargetBase();
	BWAPI::TilePosition getTargetBuildLocation();
	BWAPI::Unit getRepairTarget();
	BWAPI::Unit getResourceTarget();

private:
	BWAPI::Unit my_unit;
	AIBase *my_base;
	BWAPI::UnitType build_type;
	int base_class;
	std::vector<int> gas_worker_unit_id_list;
	BWAPI::TilePosition my_discovered_position;
	bool is_building;
	int elapsed_time_order_given;
	AIBase *target_base;
	BWAPI::TilePosition target_build_location;
	BWAPI::Unit my_repair_target;
	BWAPI::Unit my_resource;
};

#endif