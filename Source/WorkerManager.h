#ifndef WORKERMANAGER
#define WORKERMANAGER

#include<vector>
#include<BWAPI.h>
#include"Object.h"
#include"BWEM 1.3.1/src/bwem.h"
#include"GameState.h"
#include<ctime>


class WorkerManager
{
public:
	int manageWorkers(GameState &game_state);
	bool build(BWAPI::UnitType building_type, int base_class, GameState &game_state);
	BWAPI::TilePosition getBuildLocation(Object build_worker, BWAPI::UnitType building_type, GameState &game_state);
	BWAPI::TilePosition getBuildLocation(Object build_worker, BWAPI::UnitType building_type, BWAPI::TilePosition start_position, GameState &game_state);
	void getNewBuildWorker(BWAPI::Unit building, GameState &game_state);
	bool buildNearPositionInBase(BWAPI::UnitType building_type, BWAPI::TilePosition start_position, int base_class, GameState &game_state);
	bool validateFloodFill(BWAPI::TilePosition build_position, BWAPI::UnitType building_type, GameState &game_state, AIBase* target_base);
	void floodFill(BWAPI::TilePosition start_position, std::vector<std::pair<TileFlags, int>>* build_map, GameState &game_state, AIBase* target_base);
	bool buildAtPosition(BWAPI::UnitType building_type, GameState &game_state, BWAPI::TilePosition build_position);

private:


};

#endif