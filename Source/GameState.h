#ifndef GAMESTATE
#define GAMESTATE
#include<vector>
#include"Object.h"
#include"AIBase.h"
#include"Objective.h"
#include<math.h>

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
	BGHMech,
	P2Gate1,
	P4GateGoonOpening,
	P4GateGoonMid,
	P4GateGoonLate,
	PForgeFastExpand9poolOpening
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
	void toggleSecondaryScouting();
	void resetSecondaryScouting();
	void setLastTimeExpanded();
	void toggleCyberCore();
	void addRoboticsFacility(int additional_robotics_facility);
	void addObservatory(int additional_observatory);
	void addForge(int additional_forge);
	void addCitadelofAdun(int additional_citadel_of_adun);
	void addSupplyBuilt(int new_supply);
	void addPylon(int additional_pylon);
	void addFactory(int additional_factory);
	void addBunker(int additional_bunker);
	void loadBunker(Object* bunker_to_load);
	void assignRepairWorkers(Object* repair_target, int number_of_workers);
	void removeRepairWorkers(Object* repair_target, int number_of_workers);
	void removeMineral(BWAPI::Unit mineral);
	void drawMineralLockLines();
	void transferWorkersToNewBase(AIBase* new_base);
	void addEngineeringBay(int new_engineering_bay);
	void addStarport(int additional_starport);
	void addScienceFacility(int additional_science_facility);
	void addArmory(int additional_armory);
	
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
	BWAPI::TilePosition getGasBuildTileLocation(const BWEM::Area* area);
	bool checkValidGasBuildTileLocation(int base_class);
	int getGasCommitted();
	bool checkAcademy();
	bool checkComsatStation();
	int getLastScan();
	AIBase* getClosestEnemyBase();
	int getUnitTypeCount(BWAPI::UnitType type_to_check);
	std::map<int, Object> *getMilitary();
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
	bool getSecondaryScouting();
	AIBase* getClosestEmptyBaseNotSecondaryScouted();
	AIBase* getClosestEmptyStartLocationNotSecondaryScouted();
	int getLastTimeExpanded();
	int getGroundDistance(BWAPI::Position point_a, BWAPI::Position point_b);
	bool checkCyberCore();
	double getEnemyTotalStrength();
	double getObjectiveStrength(Objective my_objective);
	AIBase* getMainBase();
	double getLocalStrength(Object my_unit);
	double getMyTotalStrength();
	AIBase* getMyClosestBase(Object my_unit);
	int getRoboticsFacility();
	int getObservatory();
	int getForge();
	int getCitadelofAdun();
	int getSupplyBuilt();
	int getPylon();
	AIBase* getFarthestEmptyBaseNotSecondaryScouted();
	int getFactory();
	int getCompletedMacroBuildings();
	int getUnderConstructionMacroBuildings();
	int getBunker();
	double getEnemyLocalStrength(Object my_unit);
	std::vector<Object> *getRepairWorkers();
	int getAssignedRepairWorkers(Object repair_target);
	AIBase* getNearestContainingBase(BWAPI::Unit unit);
	bool assignWorkerToMineral(Object* worker);
	bool unassignWorkerFromMineral(Object* worker);
	bool assignWorkerToMineralAtBase(Object* worker, AIBase* target_base);
	int getEngineeringBay();
	int getStarport();
	int getScienceFacility();
	int getArmory();
	
private:
	std::vector<Object> building_list;
	std::vector<AIBase> base_list;
	std::map<int, Object> enemy_units;
	std::vector<Object> mineral_workers;
	std::vector<Object> build_workers;
	std::vector<std::pair<bool, BWAPI::TilePosition>> gas_locations;
	std::map<int, Object> military;
	std::vector<Object> larva;
	std::vector<Object> detectors;
	std::vector<Objective> objective_list;
	std::vector<std::pair<TileFlags, int>> build_position_map;
	std::vector<Object> repair_workers;
	
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
	bool secondary_scouting;
	int last_time_expanded;
	bool cyber_core;
	int robotics_facility;
	int observatory;
	int forge;
	int citadel_of_adun;
	int supply_built;
	int pylon;
	int factory;
	int bunker;
	int engineering_bay;
	int starport;
	int science_facility;
	int armory;
};

#endif