#ifndef WORKERMANAGER
#define WORKERMANAGER

#include<vector>
#include<BWAPI.h>
#include"Object.h"
#include"BWEM 1.3.1/src/bwem.h"

class WorkerManager
{
public:

	int manageWorkers();
	
	void addMineralWorker(Object new_worker);

	Object getScout();
	int getMineralWorkerCount();

	bool build(BWAPI::UnitType building_type, int base_class);
	BWAPI::TilePosition getBuildLocation(Object build_worker, BWAPI::UnitType building_type);

private:
	std::vector<Object> mineral_workers;
	std::vector<Object> build_workers;

};

#endif