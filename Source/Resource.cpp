#include "Resource.h"

Resource::Resource(BWAPI::Unit unit)
{
	my_unit = unit;
}

void Resource::addWorker(BWAPI::Unit unit)
{
	bool exists = false;
	for (auto worker : assigned_workers)
	{
		if (worker == unit)
			exists = true;
	}
	if (!exists)
		assigned_workers.push_back(unit);
}
void Resource::removeWorker(BWAPI::Unit unit)
{
	auto iterator = assigned_workers.begin();
	while (iterator != assigned_workers.end())
	{
		if (*iterator == unit)
		{
			assigned_workers.erase(iterator);
			return;
		}
		else
			iterator++;
	}
}

bool Resource::exists()
{
	return my_unit->exists();
}

int Resource::getNumberAssigned()
{
	return assigned_workers.size();
}

bool Resource::operator==(const Resource compare) const
{
	if (my_unit == compare.my_unit)
		return true;
	else
		return false;
}

BWAPI::Unit Resource::getUnit()
{
	return my_unit;
}

const std::vector<BWAPI::Unit> Resource::getAssignedWorkers()
{
	return assigned_workers;
}