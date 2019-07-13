#include"AIBase.h"

AIBase::AIBase(BWAPI::Game * game)
{
	my_area = nullptr;
	base_class = -1;
	distance = 0;
	scouted = false;
	secondary_scouted = false;
	number_tile_positions = 0;
	times_searched = 0;
	can_search = true;
  Broodwar = game;
}

AIBase::AIBase(const BWEM::Area *new_area, int new_base_class, BWAPI::Game * game)
{
  Broodwar = game;
	my_area = new_area;
	base_class = new_base_class;
	scouted = false;
	secondary_scouted = false;
	BWAPI::TilePosition current_position;
	number_tile_positions = 0;
	for (int x = 0; x < Broodwar->mapWidth(); x++)
	{
		current_position.x = x;
		for (int y = 0; y < Broodwar->mapHeight(); y++)
		{
			current_position.y = y;
			const BWEM::Area* area_to_check = BWEM::Map::Instance().GetArea(current_position);
			if (area_to_check != nullptr)
			{
				if (area_to_check = new_area)
					number_tile_positions++;
			}
		}
	}
	times_searched = 0;
	can_search = true;
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

int AIBase::getTileCount()
{
	return number_tile_positions;
}

void AIBase::addTimesSearched(int new_times_searched)
{
	times_searched += new_times_searched;
}

void AIBase::setCanSearch(bool new_can_search)
{
	can_search = new_can_search;
}

int AIBase::getTimesSearched()
{
	return times_searched;
}

bool AIBase::getCanSearch()
{
	return can_search;
}