#include "Objective.h"

Objective::Objective()
{
	my_objective = ObjectiveTypes::None;
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