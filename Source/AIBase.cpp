#include"AIBase.h"

AIBase::AIBase()
{
	my_area = nullptr;
	base_class = -1;
	distance = 0;
	scouted = false;
}

AIBase::AIBase(const BWEM::Area *new_area, int new_base_class)
{
	my_area = new_area;
	base_class = new_base_class;
	scouted = false;
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