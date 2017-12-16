#include"AIBase.h"

AIBase::AIBase()
{
	my_area = nullptr;
	base_class = -1;
	distance = 0;
	scouted = false;
	secondary_scouted = false;
}

AIBase::AIBase(const BWEM::Area *new_area, int new_base_class)
{
	my_area = new_area;
	base_class = new_base_class;
	scouted = false;
	secondary_scouted = false;
}

void AIBase::setArea(const BWEM::Area *new_area)
{
	my_area = new_area;
}

void AIBase::setBaseClass(int new_base_class)
{
	base_class = new_base_class;
}

const BWEM::Area *AIBase::getArea()
{
	return my_area;
}

int AIBase::getBaseClass()
{
	return base_class;
}

void AIBase::setDistance(int new_distance)
{
	distance = new_distance;
}

int AIBase::getDistance()
{
	return distance;
}

void AIBase::toggleScouted()
{
	if (scouted == false)
		scouted = true;
	else
		scouted = false;
}

bool AIBase::getScouted()
{
	return scouted;
}

void AIBase::toggleSecondaryScouted()
{
	if (secondary_scouted)
		secondary_scouted = false;
	else
		secondary_scouted = true;
}

bool AIBase::getSecondaryScouted()
{
	return secondary_scouted;
}

void AIBase::addMineral(Resource new_mineral)
{
	minerals.push_back(new_mineral);
}

bool AIBase::removeMineral(Resource mineral)
{
	auto mineral_iterator = minerals.begin();
	while (mineral_iterator != minerals.end())
	{
		if (*mineral_iterator == mineral)
		{
			minerals.erase(mineral_iterator);
			return true;
		}
		else
			mineral_iterator++;
	}
	return false;
}

std::vector<Resource>* AIBase::getMinerals()
{
	return &minerals;
}