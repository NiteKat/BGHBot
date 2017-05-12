#ifndef MILITARYMANAGER
#define MILITARYMANAGER

#include<BWAPI.h>
#include<vector>
#include"Object.h"
#include"WorkerManager.h"
#include"AIBase.h"
#include<map>

class MilitaryManager
{
public:
	MilitaryManager();

	void addUnit(Object new_unit);

	void checkMilitary(std::vector<AIBase> &base_list, WorkerManager &worker_manager);
	void scout(WorkerManager &worker_manager, std::vector<AIBase> &base_list);

	AIBase* MilitaryManager::getContainingBase(BWAPI::Unit unit, std::vector<AIBase> *base_list);

private:
	std::vector<Object> military;
	Object scout_unit;
	BWAPI::Position scout_target;
	std::map<int, Object> enemy_units;
	int global_strategy;
};

#endif