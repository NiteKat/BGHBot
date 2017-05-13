#include"GameState.h"

GameState::GameState()
{
	supply_total = 0;
	supply_used = 0;
	supply_expected = 10;
	minerals_committed = 0;
	barracks = 0;
	academy = false;
}

void GameState::addAIBase(AIBase new_base)
{
	base_list.push_back(new_base);
}

void GameState::addBuilding(Object new_building)
{
	building_list.push_back(new_building);
}

void GameState::addSupplyUsed(int new_supply)
{
	supply_used += new_supply;
}

void GameState::addSupplyTotal(int new_supply)
{
	supply_total += new_supply;
}

void GameState::addMineralsCommitted(int new_minerals)
{
	minerals_committed += new_minerals;
}

void GameState::addSupplyExpected(int new_supply)
{
	supply_expected += new_supply;
}

void GameState::toggleAcademy()
{
	if (academy == true)
		academy = false;
	else
		academy = true;
}

AIBase* GameState::getContainingBase(BWAPI::Unit unit)
{
	auto base_list_iterator = base_list.begin();
	while (base_list_iterator != base_list.end())
	{
		if (base_list_iterator->getArea() == BWEM::Map::Instance().GetNearestArea(unit->getTilePosition()))
		{
			return &(*base_list_iterator);
		}
		else
		{
			base_list_iterator++;
		}
	}
	return nullptr;
}

std::vector<AIBase>* GameState::getBaseList()
{
	return &base_list;
}

int GameState::getSupplyUsed()
{
	return supply_used;
}

void GameState::addMineralWorker(Object new_worker)
{
	mineral_workers.push_back(new_worker);
}

Object GameState::getScout()
{
	auto mineral_worker_iterator = mineral_workers.begin();
	while (mineral_worker_iterator != mineral_workers.end())
	{
		if (mineral_worker_iterator->getBase()->getBaseClass() == 3 &&
			!mineral_worker_iterator->getUnit()->isCarryingMinerals())
		{
			Object new_scout(*mineral_worker_iterator);
			mineral_workers.erase(mineral_worker_iterator);
			return new_scout;
		}
		else
		{
			mineral_worker_iterator++;
		}
	}
	Object new_scout;
	return new_scout;
}

int GameState::getMineralWorkerCount()
{
	return mineral_workers.size();
}

std::vector<Object>* GameState::getBuildingList()
{
	return &building_list;
}

int GameState::getMineralsCommitted()
{
	return minerals_committed;
}

int GameState::getSupplyTotal()
{
	return supply_total;
}

int GameState::getBarracks()
{
	return barracks;
}

int GameState::getSupplyExpected()
{
	return supply_expected;
}

void GameState::addBarracks(int new_barracks)
{
	barracks += new_barracks;
	if (barracks < 0)
		barracks = 0;
}

std::map<int, Object>* GameState::getEnemyUnits()
{
	return &enemy_units;
}

std::vector<Object>* GameState::getMineralWorkers()
{
	return &mineral_workers;
}

std::vector<Object>* GameState::getBuildWorkers()
{
	return &build_workers;
}