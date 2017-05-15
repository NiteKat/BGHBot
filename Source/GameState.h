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
	void addGas(int new_gas);
	void initializeGasLocations();
	void setGeyserUsed(BWAPI::TilePosition geyser_position);
	void setGeyserOpen(BWAPI::TilePosition geyser_position);
	void addGasCommitted(int new_gas);
	void toggleComsatStation();
	void setLastScan(int new_scan);
	void checkBaseOwnership();
	

	AIBase *getContainingBase(BWAPI::Unit);
	AIBase *getContainingBase(BWAPI::TilePosition tile_position);
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
	int getGas();
	BWAPI::TilePosition getGasBuildTileLocation();
	bool checkValidGasBuildTileLocation();
	int getGasCommitted();
	bool checkAcademy();
	bool checkComsatStation();
	int getLastScan();
	AIBase* getClosestEnemyBase();
	
private:
	std::vector<Object> building_list;
	std::vector<AIBase> base_list;
	std::map<int, Object> enemy_units;
	std::vector<Object> mineral_workers;
	std::vector<Object> build_workers;
	std::vector<std::pair<bool, BWAPI::TilePosition>> gas_locations;
	
	int supply_used;
	int supply_total;
	int supply_expected;
	int minerals_committed;
	int barracks;
	bool academy;
	int gas;
	int gas_committed;
	bool comsat_station;
	int last_scan;
};

#endif