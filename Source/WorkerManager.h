#ifndef WORKERMANAGER
#define WORKERMANAGER

#include<vector>
#include<BWAPI.h>
#include"Object.h"
#include"BWEM 1.3.1/src/bwem.h"
#include"GameState.h"

class WorkerManager
{
public:
	int manageWorkers(GameState &game_state);
	bool build(BWAPI::UnitType building_type, int base_class, GameState &game_state);
	BWAPI::TilePosition getBuildLocation(Object build_worker, BWAPI::UnitType building_type, GameState &game_state);

private:


};

#endif