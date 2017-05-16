#ifndef MILITARYMANAGER
#define MILITARYMANAGER

#include<BWAPI.h>
#include<vector>
#include"Object.h"
#include"WorkerManager.h"
#include"AIBase.h"
#include<map>
#include"GameState.h"

class MilitaryManager
{
public:
	MilitaryManager();

	void checkMilitary(WorkerManager &worker_manager, GameState &game_state);
	void scout(WorkerManager &worker_manager, GameState &game_state);

private:
	Object scout_unit;
	BWAPI::Position scout_target;
	int global_strategy;
};

#endif