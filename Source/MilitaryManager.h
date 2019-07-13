#ifndef MILITARYMANAGER
#define MILITARYMANAGER

#include<BWAPI.h>
#include<vector>
#include"Object.h"
#include"WorkerManager.h"
#include"AIBase.h"
#include<map>
#include"GameState.h"
#include"FAP.h"

class MilitaryManager
{
public:
	MilitaryManager(BWAPI::Game * game);

	void checkMilitary(std::unique_ptr<WorkerManager> &worker_manager, std::unique_ptr<GameState> &game_state);
	void scout(std::unique_ptr<WorkerManager> &worker_manager, std::unique_ptr<GameState> &game_state);
	void scout(Object intended_scout, std::unique_ptr<GameState> &game_state);

private:
	Object scout_unit;
	BWAPI::Position scout_target;
	int global_strategy;
  BWAPI::Game * Broodwar;
  Neolib::FastAPproximation fap;
};

#endif