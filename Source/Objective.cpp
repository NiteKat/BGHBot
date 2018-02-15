#include "Objective.h"

Objective::Objective()
{
	my_objective = ObjectiveTypes::None;
	last_fap_frame = 0;
}

void Objective::addUnit(Object new_unit)
{
	my_units.push_back(new_unit);
}

void Objective::setObjective(ObjectiveTypes new_objective)
{
	my_objective = new_objective;
}

std::vector<Object>* Objective::getUnits()
{
	return &my_units;
}
ObjectiveTypes Objective::getObjective()
{
	return my_objective;
}

BWAPI::Position Objective::getCentroid()
{
	int x = 0;
	int y = 0;
	BWAPI::Position centroid = BWAPI::Positions::Invalid;
	if (my_units.size() == 0)
	{
		return centroid;
	}
	auto unit_iterator = my_units.begin();
	while (unit_iterator != my_units.end())
	{
		x += unit_iterator->getUnit()->getPosition().x;
		y += unit_iterator->getUnit()->getPosition().y;
		unit_iterator++;
	}
	centroid.x = (int)x / my_units.size();
	centroid.y = (int)y / my_units.size();
	return centroid;
}

void Objective::setLastFapFrame(int new_last_fap_frame)
{
	last_fap_frame = new_last_fap_frame;
}

int Objective::getLastFapFrame()
{
	return last_fap_frame;
}

void Objective::setRegroup(bool new_regroup)
{
	regroup = new_regroup;
}

bool Objective::getRegroup()
{
	return regroup;
}

void Objective::setRegroupTarget(BWAPI::Position new_regroup_target)
{
	regroup_target = new_regroup_target;
}

BWAPI::Position Objective::getRegroupTarget()
{
	return regroup_target;
}