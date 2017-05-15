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

void MilitaryManager::checkMilitary(WorkerManager &worker_manager, GameState &game_state)
{
	AIBase* target_base;
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
	if (global_strategy == 1)
	{
		target_base = game_state.getClosestEnemyBase();
	}
	for (auto unit : BWAPI::Broodwar->enemies().getUnits())
	{
		if (game_state.getEnemyUnits()->find(unit->getID()) == game_state.getEnemyUnits()->end() &&
			unit->exists())
		{
			Object new_enemy(unit);
			new_enemy.setDiscoveredPosition(unit->getTilePosition());
			if (unit->getType().isBuilding())
				new_enemy.setIsBuilding();
			game_state.getEnemyUnits()->insert(std::pair<int, Object>(unit->getID(), new_enemy));
		}
		if ((unit->isBurrowed() ||
			unit->isCloaked()) &&
			!unit->isDetected() &&
			game_state.checkComsatStation() &&
			BWAPI::Broodwar->elapsedTime() - game_state.getLastScan() > 5)
		{
			for (auto unit_to_check : BWAPI::Broodwar->getUnitsInRadius(unit->getPosition(), BWAPI::UnitTypes::Zerg_Lurker.groundWeapon().maxRange()))
			{
				if (unit_to_check->getPlayer() == BWAPI::Broodwar->self())
				{
					auto building_list_iterator = game_state.getBuildingList()->begin();
					while (building_list_iterator != game_state.getBuildingList()->end())
					{
						if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Comsat_Station &&
							building_list_iterator->getUnit()->getEnergy() >= 50)
						{
							building_list_iterator->getUnit()->useTech(BWAPI::TechTypes::Scanner_Sweep, unit->getPosition());
							building_list_iterator = game_state.getBuildingList()->end();
							game_state.setLastScan(BWAPI::Broodwar->elapsedTime());
						}
						else
						{
							building_list_iterator++;
						}
					}
					break;
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
				auto enemy_iterator = game_state.getEnemyUnits()->begin();
				game_state.checkBaseOwnership();
				while (enemy_iterator != game_state.getEnemyUnits()->end())
				{
					if (enemy_iterator->second.getUnit()->exists())
					{
						if (game_state.getContainingBase(enemy_iterator->second.getUnit())->getBaseClass() == 3 ||
							game_state.getContainingBase(enemy_iterator->second.getUnit())->getBaseClass() == 4 ||
							game_state.getContainingBase(enemy_iterator->second.getUnit())->getBaseClass() == 5)
						{
							military_iterator->getUnit()->attack(enemy_iterator->second.getUnit()->getPosition());
							enemy_iterator = game_state.getEnemyUnits()->end();
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
				if (target_base != nullptr)
				{
					auto enemy_unit_iterator = game_state.getEnemyUnits()->begin();
					while (enemy_unit_iterator != game_state.getEnemyUnits()->end())
					{
						if (enemy_unit_iterator->second.isBuilding() &&
							target_base->getArea()->Id() == BWEM::Map::Instance().GetNearestArea(enemy_unit_iterator->second.getDiscoveredPosition())->Id())
						{
							military_iterator->getUnit()->attack((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition());
							enemy_unit_iterator = game_state.getEnemyUnits()->end();
						}
						else
						{
							enemy_unit_iterator++;
						}
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
				auto base_list_iterator = game_state.getBaseList()->begin();
				while (base_list_iterator != game_state.getBaseList()->end())
				{
					if (base_list_iterator->getArea() == BWEM::Map::Instance().GetNearestArea(scout_unit.getUnit()->getTilePosition()) &&
						base_list_iterator->getScouted() == false)
					{
						base_list_iterator->toggleScouted();
						base_list_iterator = game_state.getBaseList()->end();
					}
					else
					{
						base_list_iterator++;
					}
				}
				scout_target = BWAPI::Positions::Invalid;
				auto base_iterator = game_state.getBaseList()->begin();
				while (base_iterator != game_state.getBaseList()->end())
				{
					if (base_iterator->getArea()->Bases().size() > 0)
					{
						if ((*base_iterator->getArea()->Bases().begin()).Starting() &&
							base_iterator->getScouted() == false)
						{
							scout_target = (BWAPI::Position)(*base_iterator->getArea()->Bases().begin()).Location();
							scout_unit.getUnit()->move(scout_target);
							base_iterator = game_state.getBaseList()->end();
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
			auto base_iterator = game_state.getBaseList()->begin();
			while (base_iterator != game_state.getBaseList()->end())
			{
				if (base_iterator->getArea()->Bases().size() > 0)
				{
					if ((*base_iterator->getArea()->Bases().begin()).Starting() &&
						base_iterator->getBaseClass() == 1)
					{
						scout_target = (BWAPI::Position)(*base_iterator->getArea()->Bases().begin()).Location();
						scout_unit.getUnit()->move(scout_target);
						base_iterator = game_state.getBaseList()->end();
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
			game_state.addMineralWorker(scout_unit);
			scout_unit.clearObject();
		}
	}
}

void MilitaryManager::scout(WorkerManager &worker_manager, GameState &game_state)
{
	if (scout_unit.getUnit() == nullptr)
	{
		Object new_scout = game_state.getScout();
		if (new_scout.getUnit() != nullptr)
		{
			scout_unit = new_scout;
		}
		auto base_iterator = game_state.getBaseList()->begin();
		while (base_iterator != game_state.getBaseList()->end())
		{
			if (base_iterator->getArea()->Bases().size() > 0)
			{
				if ((*base_iterator->getArea()->Bases().begin()).Starting())
				{
					BWAPI::Broodwar << "True" << std::endl;
				}
				if ((*base_iterator->getArea()->Bases().begin()).Starting() &&
					base_iterator->getBaseClass() == 1)
				{
					scout_target = (BWAPI::Position)(*base_iterator->getArea()->Bases().begin()).Location();
					scout_unit.getUnit()->move(scout_target);
					base_iterator = game_state.getBaseList()->end();
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