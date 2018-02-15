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
	P4GateGoonAttack,
	AttackWithRegroupPrioritizeWorkers
};

class Objective
{
public:
	Objective();
	void addUnit(Object new_unit);
	void setObjective(ObjectiveTypes new_objective);
	void setLastFapFrame(int new_last_fap_frame);
	void setRegroup(bool new_regroup);
	void setRegroupTarget(BWAPI::Position new_regroup_target);

	std::vector<Object>* getUnits();
	ObjectiveTypes getObjective();
	BWAPI::Position getCentroid();
	int getLastFapFrame();
	bool getRegroup();
	BWAPI::Position getRegroupTarget();

private:
	std::vector<Object> my_units;
	ObjectiveTypes my_objective;
	int last_fap_frame;
	bool regroup;
	BWAPI::Position regroup_target;
};

#endif