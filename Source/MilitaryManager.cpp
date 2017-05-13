#include "MilitaryManager.h"

MilitaryManager::MilitaryManager()
{
	scout_target = BWAPI::Positions::Invalid;
	global_strategy = 0;
}

void MilitaryManager::addUnit(Object new_unit)
{
	military.push_back(new_unit);
}

void MilitaryManager::checkMilitary(std::vector<AIBase> &base_list, WorkerManager &worker_manager)
{
	if (global_strategy == 0 &&
		military.size() > 50)
	{
		global_strategy = 1;
	}
	else if (global_strategy == 1 &&
		military.size() <= 50)
	{
		global_strategy = 0;
	}
	for (auto unit : BWAPI::Broodwar->enemies().getUnits())
	{
		if (enemy_units.find(unit->getID()) == enemy_units.end() &&
			unit->exists())
		{
			Object new_enemy(unit);
			enemy_units.insert(std::pair<int, Object>(unit->getID(), new_enemy));
		}
		if (unit->getType().isBuilding())
		{
			auto base_list_iterator = base_list.begin();
			while (base_list_iterator != base_list.end())
			{
				if (base_list_iterator->getArea() == BWEM::Map::Instance().GetNearestArea(unit->getTilePosition()))
				{
					base_list_iterator->setBaseClass(2);
					base_list_iterator = base_list.end();
				}
				else
				{
					base_list_iterator++;
				}
			}
		}
	}
	auto military_iterator = military.begin();
	while (military_iterator != military.end())
	{
		if (military_iterator->getUnit() == nullptr)
		{
			auto erase_iterator = military_iterator;
			military_iterator = military.erase(erase_iterator);
		}
		else if (!military_iterator->getUnit()->exists())
		{
			auto erase_iterator = military_iterator;
			military_iterator = military.erase(erase_iterator);
		}
		else
		{
			if (global_strategy == 0 &&
				military_iterator->getUnit()->isIdle())
			{
				auto enemy_iterator = enemy_units.begin();
				while (enemy_iterator != enemy_units.end())
				{
					if (enemy_iterator->second.getUnit()->exists())
					{
						if (getContainingBase(enemy_iterator->second.getUnit(), &base_list)->getBaseClass() == 3)
						{
							military_iterator->getUnit()->attack(enemy_iterator->second.getUnit()->getPosition());
							enemy_iterator = enemy_units.end();
						}
						else
						{
							enemy_iterator++;
						}
					}
					else
					{
						enemy_iterator++;
					}
				}
			}
			else if (global_strategy == 1 &&
				military_iterator->getUnit()->isIdle())
			{
				auto base_list_iterator = base_list.begin();
				while (base_list_iterator != base_list.end())
				{
					if (base_list_iterator->getBaseClass() == 2)
					{
						military_iterator->getUnit()->attack((BWAPI::Position)base_list_iterator->getArea()->Top());
						base_list_iterator = base_list.end();
					}
					else
					{
						base_list_iterator++;
					}
				}
			}
			military_iterator++;
		}
	}
	if (scout_unit.getUnit() != nullptr)
	{
		if (scout_target != BWAPI::Positions::Invalid)
		{
			if (scout_unit.getUnit()->getDistance(scout_target) < 100)
			{
				
				/*for (auto unit : scout_unit.getUnit()->getUnitsInRadius(100))
				{
					if (unit->getPlayer() != BWAPI::Broodwar->self())
					{
						auto base_list_iterator = base_list.begin();
						while (base_list_iterator != base_list.end())
						{
							if (base_list_iterator->getArea() == BWEM::Map::Instance().GetNearestArea(scout_unit.getUnit()->getTilePosition()))
							{
								base_list_iterator->setBaseClass(2);
								base_list_iterator = base_list.end();
							}
							else
							{
								base_list_iterator++;
							}
						}
					}
				}*/
				auto base_list_iterator = base_list.begin();
				while (base_list_iterator != base_list.end())
				{
					if (base_list_iterator->getArea() == BWEM::Map::Instance().GetNearestArea(scout_unit.getUnit()->getTilePosition()) &&
						base_list_iterator->getBaseClass() == 1)
					{
						base_list_iterator->setBaseClass(0);
						base_list_iterator = base_list.end();
					}
					else
					{
						base_list_iterator++;
					}
				}
				scout_target = BWAPI::Positions::Invalid;
				auto base_iterator = base_list.begin();
				while (base_iterator != base_list.end())
				{
					if (base_iterator->getArea()->Bases().size() > 0)
					{
						if ((*base_iterator->getArea()->Bases().begin()).Starting() &&
							base_iterator->getBaseClass() == 1)
						{
							scout_target = (BWAPI::Position)(*base_iterator->getArea()->Bases().begin()).Location();
							scout_unit.getUnit()->move(scout_target);
							base_iterator = base_list.end();
						}
						else
						{
							base_iterator++;
						}
					}
					else
					{
						base_iterator++;
					}
				}
			}
		}
		if (scout_unit.getUnit()->isIdle())
		{
			scout_target = BWAPI::Positions::Invalid;
			auto base_iterator = base_list.begin();
			while (base_iterator != base_list.end())
			{
				if (base_iterator->getArea()->Bases().size() > 0)
				{
					if ((*base_iterator->getArea()->Bases().begin()).Starting() &&
						base_iterator->getBaseClass() == 1)
					{
						scout_target = (BWAPI::Position)(*base_iterator->getArea()->Bases().begin()).Location();
						scout_unit.getUnit()->move(scout_target);
						base_iterator = base_list.end();
					}
					else
					{
						base_iterator++;
					}
				}
				else
				{
					base_iterator++;
				}
			}
		}
		if (scout_target == BWAPI::Positions::Invalid)
		{
			worker_manager.addMineralWorker(scout_unit);
			scout_unit.clearObject();
		}
	}
}

void MilitaryManager::scout(WorkerManager &worker_manager, std::vector<AIBase> &base_list)
{
	if (scout_unit.getUnit() == nullptr)
	{
		Object new_scout = worker_manager.getScout();
		if (new_scout.getUnit() != nullptr)
		{
			scout_unit = new_scout;
		}
		auto base_iterator = base_list.begin();
		while (base_iterator != base_list.end())
		{
			if (base_iterator->getArea()->Bases().size() > 0)
			{
				if ((*base_iterator->getArea()->Bases().begin()).Starting())
				{
					BWAPI::Broodwar << "True";
				}
				if ((*base_iterator->getArea()->Bases().begin()).Starting() &&
					base_iterator->getBaseClass() == 1)
				{
					scout_target = (BWAPI::Position)(*base_iterator->getArea()->Bases().begin()).Location();
					scout_unit.getUnit()->move(scout_target);
					base_iterator = base_list.end();
				}
				else
				{
					base_iterator++;
				}
			}
			else
			{
				base_iterator++;
			}
		}
		scout_unit.getUnit()->move(scout_target);
	}
}

AIBase* MilitaryManager::getContainingBase(BWAPI::Unit unit, std::vector<AIBase> *base_list)
{
	auto base_list_iterator = base_list->begin();
	while (base_list_iterator != base_list->end())
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