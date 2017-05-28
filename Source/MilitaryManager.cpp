#include "MilitaryManager.h"

MilitaryManager::MilitaryManager()
{
	scout_target = BWAPI::Positions::Invalid;
	global_strategy = 0;
}

void MilitaryManager::checkMilitary(WorkerManager &worker_manager, GameState &game_state)
{
	AIBase* target_base;
	if (global_strategy == 0 &&
		game_state.getMilitary()->size() > 50 &&
		BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran &&
		game_state.getBuildOrder() == "default")
	{
		global_strategy = 1;
	}
	else if (global_strategy == 1 &&
		game_state.getMilitary()->size() <= 50 &&
		BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran &&
		game_state.getBuildOrder() == "default")
	{
		global_strategy = 0;
	}
	else if (global_strategy == 0 &&
		game_state.getMilitary()->size() > 24 &&
		BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran &&
		game_state.getBuildOrder() == "build2")
	{
		global_strategy = 1;
		auto building_list_iterator = game_state.getBuildingList()->begin();
		while (building_list_iterator != game_state.getBuildingList()->end())
		{
			if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks &&
				!building_list_iterator->getUnit()->isFlying())
			{
				building_list_iterator->getUnit()->lift();
				building_list_iterator = game_state.getBuildingList()->end();
			}
			else
				building_list_iterator++;
		}
	}
	else if (global_strategy == 1 &&
		game_state.getMilitary()->size() <= 12 &&
		BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran &&
		game_state.getBuildOrder() == "build2")
	{
		global_strategy = 0;
	}
	else if (global_strategy == 0 &&
		game_state.getMilitary()->size() > 30 &&
		BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
	{
		global_strategy = 1;
	}
	else if (global_strategy == 1 &&
		game_state.getMilitary()->size() <= 30 &&
		BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
	{
		global_strategy = 0;
	}
	else if (global_strategy == 0 &&
		game_state.getSupplyUsed() > 180 &&
		BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
	{
		global_strategy = 1;
	}
	else if (global_strategy == 1 &&
		game_state.getSupplyUsed() <= 60 &&
		BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
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
			unit->exists() &&
			unit->getType() != BWAPI::UnitTypes::Resource_Vespene_Geyser)
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
		else if ((unit->isBurrowed() ||
			unit->isCloaked()) &&
			!unit->isDetected())
		{
			Object* detector = game_state.getAvailableDetector();
			if (detector != nullptr)
			{
				detector->getUnit()->move(unit->getPosition());
			}
		}
		
	}
	auto military_iterator = game_state.getMilitary()->begin();
	while (military_iterator != game_state.getMilitary()->end())
	{
		if (military_iterator->getUnit() == nullptr)
		{
			auto erase_iterator = military_iterator;
			military_iterator = game_state.getMilitary()->erase(erase_iterator);
		}
		else if (!military_iterator->getUnit()->exists())
		{
			auto erase_iterator = military_iterator;
			military_iterator = game_state.getMilitary()->erase(erase_iterator);
		}
		else
		{
			if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
			{
				if (BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Stim_Packs) &&
					military_iterator->getUnit()->isAttacking() &&
					!military_iterator->getUnit()->isStimmed() &&
					military_iterator->getUnit()->getHitPoints() > 20)
				{
					military_iterator->getUnit()->useTech(BWAPI::TechTypes::Stim_Packs);
				}
			}
			if (global_strategy == 0 &&
				military_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode)
			{
				auto building_list_iterator = game_state.getBuildingList()->begin();
				while (building_list_iterator != game_state.getBuildingList()->end())
				{
					if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks)
					{
						if (military_iterator->getUnit()->getDistance(building_list_iterator->getUnit()->getPosition()) < 150 &&
							BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode))
						{
							military_iterator->getUnit()->useTech(BWAPI::TechTypes::Tank_Siege_Mode);
						}
						else if (military_iterator->getUnit()->getDistance(building_list_iterator->getUnit()->getPosition()) < 150 &&
							!BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode))
						{
							military_iterator->getUnit()->holdPosition();
						}
						else
						{
							military_iterator->getUnit()->move(building_list_iterator->getUnit()->getPosition());
						}
						building_list_iterator = game_state.getBuildingList()->end();
					}
					else
						building_list_iterator++;
				}
			}
			else if (global_strategy == 1 &&
				military_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode &&
				BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode))
			{
				auto scanned_units = military_iterator->getUnit()->getUnitsInRadius(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode.groundWeapon().maxRange());
				for (auto check_unit : scanned_units)
				{
					if (check_unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()) &&
						!check_unit->isFlying())
					{
						military_iterator->getUnit()->useTech(BWAPI::TechTypes::Tank_Siege_Mode);
						break;
					}
				}
			}
			else if (global_strategy == 1 &&
				military_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode)
			{
				bool nearby_enemy = false;
				auto scanned_units = military_iterator->getUnit()->getUnitsInRadius(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode.groundWeapon().maxRange());
				for (auto check_unit : scanned_units)
				{
					if (check_unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()) &&
						!check_unit->isFlying())
					{
						nearby_enemy = true;
						break;
					}
				}
				if (nearby_enemy == false)
				{
					military_iterator->getUnit()->useTech(BWAPI::TechTypes::Tank_Siege_Mode);
				}
			}
			if (global_strategy == 0 &&
				military_iterator->getUnit()->isIdle())
			{
				if (game_state.getBuildOrder() == "build2")
				{
					if (military_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Marine)
					{
						auto base_list_iterator = game_state.getBaseList()->begin();
						while (base_list_iterator != game_state.getBaseList()->end())
						{
							if (base_list_iterator->getBaseClass() == 3)
							{
								if (base_list_iterator->getArea()->Id() == game_state.getContainingBase(military_iterator->getUnit())->getArea()->Id())
								{
									military_iterator->getUnit()->holdPosition();
								}
								else
								{
									BWEM::CPPath path_to_check = BWEM::Map::Instance().GetPath((BWAPI::Position)game_state.getContainingBase(military_iterator->getUnit())->getArea()->Top(), (BWAPI::Position)base_list_iterator->getArea()->Top());
									if (path_to_check.size() <= 0)
									{
										military_iterator->getUnit()->move((BWAPI::Position)base_list_iterator->getArea()->Top());
									}
								}
								base_list_iterator = game_state.getBaseList()->end();
							}
							else
								base_list_iterator++;
						} 
					}
				}
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
							if (enemy_iterator->second.getUnit()->getType().isFlyer() &&
								military_iterator->getUnit()->getType().airWeapon() != BWAPI::WeaponTypes::None)
							{
								military_iterator->getUnit()->attack(enemy_iterator->second.getUnit()->getPosition());
								enemy_iterator = game_state.getEnemyUnits()->end();
							}
							else if (!enemy_iterator->second.getUnit()->getType().isFlyer())
							{
								military_iterator->getUnit()->attack(enemy_iterator->second.getUnit()->getPosition());
								enemy_iterator = game_state.getEnemyUnits()->end();
							}
							else
								enemy_iterator++;
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
				(military_iterator->getUnit()->isIdle() ||
				military_iterator->getUnit()->isPatrolling()))
			{
				if (target_base != nullptr)
				{
					auto enemy_unit_iterator = game_state.getEnemyUnits()->begin();
					while (enemy_unit_iterator != game_state.getEnemyUnits()->end())
					{
						if (enemy_unit_iterator->second.isBuilding() &&
							target_base->getArea()->Id() == BWEM::Map::Instance().GetNearestArea(enemy_unit_iterator->second.getDiscoveredPosition())->Id())
						{
							if (BWAPI::Broodwar->isVisible(enemy_unit_iterator->second.getDiscoveredPosition()))
							{
								BWAPI::Unitset scanned_units = BWAPI::Broodwar->getUnitsInRadius((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition(), 200);
								auto scanned_iterator = scanned_units.begin();
								if (scanned_units.size() == 0)
								{
									auto erase_iterator = enemy_unit_iterator;
									enemy_unit_iterator = game_state.getEnemyUnits()->erase(erase_iterator);
								}
								else
								{
									bool enemy_found = false;
									while (scanned_iterator != scanned_units.end())
									{
										if ((*scanned_iterator)->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
										{
											enemy_found = true;
											scanned_iterator = scanned_units.end();
										}
										else
										{
											scanned_iterator++;
										}
									}
									if (enemy_found == false)
									{
										auto erase_iterator = enemy_unit_iterator;
										enemy_unit_iterator = game_state.getEnemyUnits()->erase(erase_iterator);
									}
									else
									{
										military_iterator->getUnit()->attack((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition());
										enemy_unit_iterator++;
									}
								}
							}
							else
							{
								military_iterator->getUnit()->attack((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition());
								enemy_unit_iterator = game_state.getEnemyUnits()->end();
							}
						}
						else
						{
							enemy_unit_iterator++;
						}
					}
				}
				else if (!military_iterator->getUnit()->isPatrolling())
				{
					military_iterator->getUnit()->patrol(game_state.getRandomUncontrolledPosition());
				}
			}
			military_iterator++;
		}
	}
	if (scout_unit.getUnit() != nullptr)
	{
		if (scout_target != BWAPI::Positions::Invalid)
		{
			if (scout_unit.getUnit()->getDistance(scout_target) < 200)
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