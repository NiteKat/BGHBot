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
	MacroManager();

	void checkMacro(WorkerManager* worker_manager, GameState &game_state);

private:
};

#endif