#ifndef AIBASE
#define AIBASE

#include"../../BWEM-community/BWEM/include/bwem.h"
#include"Resource.h"

class AIBase
{
public:
	AIBase(BWAPI::Game * game);
	AIBase(const BWEM::Area *new_area, int new_base_class, BWAPI::Game * game);

	void setArea(const BWEM::Area *new_area);
	void setBaseClass(int new_base_class);
	void setDistance(int new_distance);
	void toggleScouted();
	void toggleSecondaryScouted();
	void addMineral(Resource new_mineral);
	void addTimesSearched(int new_times_searched);
	void setCanSearch(bool new_can_search);

	const BWEM::Area *getArea();
	int getBaseClass();
	int getDistance();
	bool getScouted();
	bool getSecondaryScouted();
	bool removeMineral(Resource mineral);
	std::vector<Resource> *getMinerals();
	int getTileCount();
	int getTimesSearched();
	bool getCanSearch();

private:
	const BWEM::Area *my_area;
	int base_class;
	int distance;
	bool scouted;
	bool secondary_scouted;
	std::vector<Resource> minerals;
	int number_tile_positions;
	int times_searched;
	bool can_search;
  BWAPI::Game * Broodwar;
};

#endif