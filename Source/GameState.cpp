#include"GameState.h"

GameState::GameState()
{
	supply_total = 0;
	supply_used = 0;
	supply_expected = 10;
	minerals_committed = 0;
	barracks = 0;
	academy = false;
	gas = 0;
	gas_committed = 0;
	comsat_station = false;
	last_scan = 0;
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

AIBase* GameState::getContainingBase(BWAPI::TilePosition tile_position)
{
	auto base_list_iterator = base_list.begin();
	while (base_list_iterator != base_list.end())
	{
		if (base_list_iterator->getArea() == BWEM::Map::Instance().GetNearestArea(tile_position))
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

void GameState::addGas(int new_gas)
{
	gas += new_gas;
}

int GameState::getGas()
{
	return gas;
}

void GameState::initializeGasLocations()
{
	BWAPI::Unitset geyser_list = BWAPI::Broodwar->getStaticGeysers();
	auto geyser_list_iterator = geyser_list.begin();
	while (geyser_list_iterator != geyser_list.end())
	{
		std::pair<bool, BWAPI::TilePosition> new_geyser;
		new_geyser.first = false;
		new_geyser.second = (*geyser_list_iterator)->getTilePosition();
		gas_locations.push_back(new_geyser);
		geyser_list_iterator++;
	}
}

BWAPI::TilePosition GameState::getGasBuildTileLocation()
{
	auto gas_location_iterator = gas_locations.begin();
	while (gas_location_iterator != gas_locations.end())
	{
		if (gas_location_iterator->first == false &&
			getContainingBase(gas_location_iterator->second)->getBaseClass() == 3)
		{
			return gas_location_iterator->second;
		}
		else
		{
			gas_location_iterator++;
		}
	}
	return BWAPI::TilePositions::Invalid;
}
bool GameState::checkValidGasBuildTileLocation()
{
	auto gas_location_iterator = gas_locations.begin();
	while (gas_location_iterator != gas_locations.end())
	{
		if (gas_location_iterator->first == false &&
			getContainingBase(gas_location_iterator->second)->getBaseClass() == 3)
		{
			return true;
		}
		else
		{
			gas_location_iterator++;
		}
	}
	return false;
}

void GameState::setGeyserUsed(BWAPI::TilePosition geyser_position)
{
	auto gas_location_iterator = gas_locations.begin();
	while (gas_location_iterator != gas_locations.end())
	{
		if (gas_location_iterator->first == false &&
			gas_location_iterator->second == geyser_position)
		{
			gas_location_iterator->first = true;
			return;
		}
		else
		{
			gas_location_iterator++;
		}
	}
}

void GameState::setGeyserOpen(BWAPI::TilePosition geyser_position)
{
	auto gas_location_iterator = gas_locations.begin();
	while (gas_location_iterator != gas_locations.end())
	{
		if (gas_location_iterator->first == true &&
			gas_location_iterator->second == geyser_position)
		{
			gas_location_iterator->first = false;
			return;
		}
		else
		{
			gas_location_iterator++;
		}
	}
}

int GameState::getGasCommitted()
{
	return gas_committed;
}

void GameState::addGasCommitted(int new_gas)
{
	gas_committed += new_gas;
	if (gas_committed < 0)
		gas_committed = 0;
}

bool GameState::checkAcademy()
{
	return academy;
}

void GameState::toggleComsatStation()
{
	if (comsat_station == false)
	{
		comsat_station = true;
	}
	else
		comsat_station = false;
}

bool GameState::checkComsatStation()
{
	return comsat_station;
}

void GameState::setLastScan(int new_scan)
{
	last_scan = new_scan;
}

int GameState::getLastScan()
{
	return last_scan;
}