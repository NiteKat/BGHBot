#ifndef GAMESTATE
#define GAMESTATE
#include<vector>
#include"Object.h"
#include"AIBase.h"

class GameState
{
public:
	GameState();

	void addAIBase(AIBase new_base);
	void addBuilding(Object new_building);
	void addSupplyUsed(int new_supply);
	void addSupplyTotal(int new_supply);
	void addMineralsCommitted(int new_minerals);
	void addSupplyExpected(int new_supply);
	void toggleAcademy();
	void addMineralWorker(Object new_worker);
	void addBarracks(int new_barracks);

	AIBase *getContainingBase(BWAPI::Unit);
	std::vector<AIBase> *getBaseList();
	int getSupplyUsed();
	Object getScout();
	int getMineralWorkerCount();
	std::vector<Object> *getBuildingList();
	int getMineralsCommitted();
	int getSupplyTotal();
	int getBarracks();
	int getSupplyExpected();
	std::map<int, Object> *getEnemyUnits();
	std::vector<Object> *getMineralWorkers();
	std::vector<Object> *getBuildWorkers();
	
private:
	std::vector<Object> building_list;
	std::vector<AIBase> base_list;
	std::map<int, Object> enemy_units;
	std::vector<Object> mineral_workers;
	std::vector<Object> build_workers;
	
	int supply_used;
	int supply_total;
	int supply_expected;
	int minerals_committed;
	int barracks;
	bool academy;
};

#endif