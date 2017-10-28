#ifndef GAMESTATE
#define GAMESTATE
#include<vector>
#include"Object.h"
#include"AIBase.h"
#include"Objective.h"

struct TileFlags
{
	bool unobstructed;
	bool pylon_power_2x2;
	bool pylon_power_3x2;
	bool pylon_power_4x3;
};

enum class BuildOrder
{
	Default,
	BGHMech
};

class GameState
{
public:
	GameState();

	void addAIBase(AIBase new_base);
	void addBuilding(Object new_building);
	void addSupplyUsed(double new_supply);
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
	void addUnit(Object new_unit);
	void addLarva(Object new_larva);
	void addDetector(Object new_detector);
	void removeEnemyUnitsAtTilePosition(BWAPI::TilePosition target_position);
	void addEvolutionChambers(int new_evolution_chambers);
	void setBuildOrder(BuildOrder new_build_order);
	void toggleBuildTanks();
	void addObjective(Objective new_objective);
	void assessGame();
	void initializeBuildMap();
	void updateBuildMap(int x, int y, BWAPI::UnitType building_type, bool build_or_remove);
	void setWorkerDefense(bool new_worker_defense);
	void toggleExpanding();
	void setTargetExpansion(AIBase* new_target_expansion);


	AIBase *getContainingBase(BWAPI::Unit);
	AIBase *getContainingBase(BWAPI::TilePosition tile_position);
	std::vector<AIBase> *getBaseList();
	double getSupplyUsed();
	Object getScout();
	int getMineralWorkerCount();
	std::vector<Object> *getBuildingList();
	int getMineralsCommitted();
	double getSupplyTotal();
	int getBarracks();
	double getSupplyExpected();
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
	int getUnitTypeCount(BWAPI::UnitType type_to_check);
	std::vector<Object> *getMilitary();
	std::vector<Object> *getLarva();
	int getBuildingTypeCount(BWAPI::UnitType type_to_check);
	Object* getAvailableDetector();
	BWAPI::Position getRandomUncontrolledPosition();
	int getDetectorCount();
	int getEvolutionChambers();
	BuildOrder getBuildOrder();
	bool getBuildTanks();
	std::vector<Objective> *getObjectiveList();
	std::vector<std::pair<TileFlags, int>> *getBuildPositionMap();
	bool getWorkerDefense();
	AIBase* getClosestEmptyBase();
	bool getExpanding();
	AIBase* getTargetExpansion();
	
	
private:
	std::vector<Object> building_list;
	std::vector<AIBase> base_list;
	std::map<int, Object> enemy_units;
	std::vector<Object> mineral_workers;
	std::vector<Object> build_workers;
	std::vector<std::pair<bool, BWAPI::TilePosition>> gas_locations;
	std::vector<Object> military;
	std::vector<Object> larva;
	std::vector<Object> detectors;
	std::vector<Objective> objective_list;
	std::vector<std::pair<TileFlags, int>> build_position_map;
	
	double supply_used;
	double supply_total;
	double supply_expected;
	int minerals_committed;
	int barracks;
	bool academy;
	int gas;
	int gas_committed;
	bool comsat_station;
	int last_scan;
	int evolution_chambers;
	BuildOrder build_order;
	bool build_tanks;
	bool worker_defense;
	bool expanding;
	AIBase* target_expansion;

};

#endif