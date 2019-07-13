#ifndef WORKERMANAGER
#define WORKERMANAGER

#include<vector>
#include<BWAPI.h>
#include"Object.h"
#include"../../BWEM-community/BWEM/include/bwem.h"
#include"GameState.h"
#include<ctime>


class WorkerManager
{
public:
  WorkerManager(BWAPI::Game * game);

	int manageWorkers(std::unique_ptr<GameState> &game_state);
	bool build(BWAPI::UnitType building_type, int base_class, std::unique_ptr<GameState> &game_state);
	BWAPI::TilePosition getBuildLocation(Object build_worker, BWAPI::UnitType building_type, std::unique_ptr<GameState> &game_state);
	BWAPI::TilePosition getBuildLocation(Object build_worker, BWAPI::UnitType building_type, BWAPI::TilePosition start_position, std::unique_ptr<GameState> &game_state);
	void getNewBuildWorker(BWAPI::Unit building, std::unique_ptr<GameState> &game_state);
	bool buildNearPositionInBase(BWAPI::UnitType building_type, BWAPI::TilePosition start_position, int base_class, std::unique_ptr<GameState> &game_state);
	bool validateFloodFill(BWAPI::TilePosition build_position, BWAPI::UnitType building_type, std::unique_ptr<GameState> &game_state, AIBase* target_base);
	void floodFill(BWAPI::TilePosition start_position, std::vector<std::pair<TileFlags, int>>* build_map, std::unique_ptr<GameState> &game_state, AIBase* target_base);
	bool buildAtPosition(BWAPI::UnitType building_type, std::unique_ptr<GameState> &game_state, BWAPI::TilePosition build_position);

private:
  BWAPI::Game * Broodwar;

};

#endif