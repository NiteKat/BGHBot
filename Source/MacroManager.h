#ifndef MACROMANAGER
#define MACROMANAGER

#include<vector>
#include<math.h>
#include"GameState.h"
#include"Object.h"
#include"AIBase.h"
#include"WorkerManager.h"


class MacroManager
{
public:
	MacroManager(BWAPI::Game * game);

	void checkMacro(std::unique_ptr<WorkerManager> &worker_manager, std::unique_ptr<GameState> &game_state);

private:
  BWAPI::Game * Broodwar;
};

#endif