#ifndef MACROMANAGER
#define MACROMANAGER

#include<vector>
#include"Object.h"
#include"AIBase.h"
#include"WorkerManager.h"

class MacroManager
{
public:
	MacroManager();

	void checkMacro(WorkerManager* worker_manager);

	void addAIBase(AIBase new_base);
	void addBuilding(Object new_building);
	void addSupplyUsed(int new_supply);
	void addSupplyTotal(int new_supply);
	void addMineralsCommitted(int new_minerals);
	void addSupplyExpected(int new_supply);
	void toggleAcademy();

	AIBase *getContainingBase(BWAPI::Unit);
	std::vector<AIBase> *getBaseList();
	int getSupplyUsed();

private:
	std::vector<Object> building_list;
	std::vector<AIBase> base_list;

	int supply_used;
	int supply_total;
	int supply_expected;
	int minerals_committed;
	int barracks;
	bool academy;
};

#endif