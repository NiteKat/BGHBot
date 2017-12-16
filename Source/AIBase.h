#ifndef AIBASE
#define AIBASE

#include"BWEM 1.3.1/src/bwem.h"
#include"Resource.h"

class AIBase
{
public:
	AIBase();
	AIBase(const BWEM::Area *new_area, int new_base_class);

	void setArea(const BWEM::Area *new_area);
	void setBaseClass(int new_base_class);
	void setDistance(int new_distance);
	void toggleScouted();
	void toggleSecondaryScouted();
	void addMineral(Resource new_mineral);

	const BWEM::Area *getArea();
	int getBaseClass();
	int getDistance();
	bool getScouted();
	bool getSecondaryScouted();
	bool removeMineral(Resource mineral);
	std::vector<Resource> *getMinerals();

private:
	const BWEM::Area *my_area;
	int base_class;
	int distance;
	bool scouted;
	bool secondary_scouted;
	std::vector<Resource> minerals;
};

#endif