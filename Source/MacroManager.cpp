#include"MacroManager.h"

MacroManager::MacroManager()
{
	supply_total = 0;
	supply_used = 0;
	supply_expected = 10;
	minerals_committed = 0;
	barracks = 0;
	academy = false;
}

void MacroManager::checkMacro(WorkerManager* worker_manager)
{
	auto building_list_iterator = building_list.begin();
	while (building_list_iterator != building_list.end())
	{
		if (building_list_iterator->getUnit() == nullptr)
		{
			auto erase_iterator = building_list_iterator;
			building_list_iterator = building_list.erase(erase_iterator);
		}
		else if (!building_list_iterator->getUnit()->exists())
		{
			auto erase_iterator = building_list_iterator;
			building_list_iterator = building_list.erase(erase_iterator);
		}
		else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Command_Center &&
			building_list_iterator->getUnit()->isIdle() &&
			!building_list_iterator->getUnit()->isTraining() &&
			BWAPI::Broodwar->self()->minerals() - minerals_committed >= 50 &&
			worker_manager->getMineralWorkerCount() < 25 &&
			supply_used < supply_total)
		{
			building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_SCV);
			building_list_iterator++;
		}
		else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks &&
			building_list_iterator->getUnit()->isIdle() &&
			!building_list_iterator->getUnit()->isTraining() &&
			BWAPI::Broodwar->self()->minerals() - minerals_committed >= 50 &&
			supply_used < supply_total)
		{
			building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine);
			building_list_iterator++;
		}
		else
		{
			building_list_iterator++;
		}
	}
	if ((barracks == 0 && supply_used >= supply_expected - 2 ||
		barracks >= 1 && supply_used >= supply_expected - 10) &&
		BWAPI::Broodwar->self()->minerals() - minerals_committed >= 100)
	{
		if (worker_manager->build(BWAPI::UnitTypes::Terran_Supply_Depot, 3))
		{
			supply_expected += 8;
			minerals_committed += 100;
		}
	}
	if ((barracks < 2 &&
		BWAPI::Broodwar->self()->minerals() - minerals_committed >= 150) ||
		(barracks >= 2 &&
		barracks < 8 &&
		BWAPI::Broodwar->self()->minerals() - minerals_committed >= 150 + 50 * barracks))
	{
		if (worker_manager->build(BWAPI::UnitTypes::Terran_Barracks, 3))
		{
			minerals_committed += 150;
			barracks++;
		}
	}
}

void MacroManager::addAIBase(AIBase new_base)
{
	base_list.push_back(new_base);
}

void MacroManager::addBuilding(Object new_building)
{
	building_list.push_back(new_building);
}

void MacroManager::addSupplyUsed(int new_supply)
{
	supply_used += new_supply;
}

void MacroManager::addSupplyTotal(int new_supply)
{
	supply_total += new_supply;
}

void MacroManager::addMineralsCommitted(int new_minerals)
{
	minerals_committed += new_minerals;
}

void MacroManager::addSupplyExpected(int new_supply)
{
	supply_expected += new_supply;
}

void MacroManager::toggleAcademy()
{
	if (academy == true)
		academy = false;
	else
		academy = true;
}

AIBase* MacroManager::getContainingBase(BWAPI::Unit unit)
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

std::vector<AIBase>* MacroManager::getBaseList()
{
	return &base_list;
}

int MacroManager::getSupplyUsed()
{
	return supply_used;
}