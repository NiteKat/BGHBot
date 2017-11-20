#ifndef OBJECTIVE
#define OBJECTIVE

#include<vector>
#include"Object.h"


enum class ObjectiveTypes
{
	None,
	Attack,
	Defend,
	DefendExpansion,
	P2GateAttack,
	P4GateGoonAttack
};

class Objective
{
public:
	Objective();
	void addUnit(Object new_unit);
	void setObjective(ObjectiveTypes new_objective);

	std::vector<Object>* getUnits();
	ObjectiveTypes getObjective();
	BWAPI::Position getCentroid();

private:
	std::vector<Object> my_units;
	ObjectiveTypes my_objective;
};

#endif