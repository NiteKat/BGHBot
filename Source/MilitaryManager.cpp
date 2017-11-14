#include "MilitaryManager.h"

MilitaryManager::MilitaryManager()
{
	scout_target = BWAPI::Positions::Invalid;
	global_strategy = 0;
}

void MilitaryManager::checkMilitary(WorkerManager &worker_manager, GameState &game_state)
{
	AIBase* target_base = game_state.getClosestEnemyBase();
	/*if (global_strategy == 0 &&
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
		game_state.getBuildOrder() == BuildOrder::BGHMech)
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
		game_state.getBuildOrder() == BuildOrder::BGHMech)
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
				(military_iterator->getUnit()->isIdle() ||
				military_iterator->getUnit()->getOrder() == BWAPI::Orders::Stop))
			{
				
			}
			else if (global_strategy == 1 &&
				(military_iterator->getUnit()->isIdle() ||
				military_iterator->getUnit()->isPatrolling() ||
				military_iterator->getUnit()->getOrder() == BWAPI::Orders::Stop))
			{
				
			military_iterator++;
		}
	}
	*/
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
	auto current_objective = game_state.getObjectiveList()->begin();
	while (current_objective != game_state.getObjectiveList()->end())
	{
		if (current_objective->getUnits()->size() == 0 &&
			current_objective->getObjective() != ObjectiveTypes::Defend)
		{
			auto erase_iterator = current_objective;
			current_objective = game_state.getObjectiveList()->erase(erase_iterator);
		}
		else
		{
			if (current_objective->getObjective() == ObjectiveTypes::Attack &&
				game_state.getObjectiveList()->begin()->getUnits()->size() >= 3 * game_state.getBarracks())
			{
				auto current_unit = game_state.getObjectiveList()->begin()->getUnits()->begin();
				while (current_unit != game_state.getObjectiveList()->begin()->getUnits()->end())
				{
					current_objective->addUnit(*current_unit);
					current_unit++;
				}
				game_state.getObjectiveList()->begin()->getUnits()->clear();
			}
			if (current_objective->getObjective() == ObjectiveTypes::P2GateAttack &&
				game_state.getObjectiveList()->begin()->getUnits()->size() > 0)
			{
				auto current_unit = game_state.getObjectiveList()->begin()->getUnits()->begin();
				while (current_unit != game_state.getObjectiveList()->begin()->getUnits()->end())
				{
					current_objective->addUnit(*current_unit);
					current_unit++;
				}
				game_state.getObjectiveList()->begin()->getUnits()->clear();
			}
			auto unit_iterator = current_objective->getUnits()->begin();
			while (unit_iterator != current_objective->getUnits()->end())
			{
				if (!unit_iterator->getUnit()->exists())
				{
					auto military_iterator = game_state.getMilitary()->begin();
					while (military_iterator != game_state.getMilitary()->end())
					{
						if (military_iterator->getUnit()->getID() == unit_iterator->getUnit()->getID())
						{
							auto erase_iterator = military_iterator;
							game_state.getMilitary()->erase(erase_iterator);
							military_iterator = game_state.getMilitary()->end();
						}
						else
							military_iterator++;
					}
					auto erase_iterator2 = unit_iterator;
					unit_iterator = current_objective->getUnits()->erase(erase_iterator2);
				}
				else if (current_objective->getObjective() == ObjectiveTypes::Defend)
				{
					if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Marine &&
						BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Stim_Packs) &&
						unit_iterator->getUnit()->isAttacking() &&
						!unit_iterator->getUnit()->isStimmed() &&
						unit_iterator->getUnit()->getHitPoints() > 20)
					{
						unit_iterator->getUnit()->useTech(BWAPI::TechTypes::Stim_Packs);
					}
					if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode)
					{
						auto building_list_iterator = game_state.getBuildingList()->begin();
						while (building_list_iterator != game_state.getBuildingList()->end())
						{
							if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks)
							{
								if (unit_iterator->getUnit()->getDistance(building_list_iterator->getUnit()->getPosition()) < 150 &&
									BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode))
								{
									unit_iterator->getUnit()->useTech(BWAPI::TechTypes::Tank_Siege_Mode);
								}
								else if (unit_iterator->getUnit()->getDistance(building_list_iterator->getUnit()->getPosition()) < 150 &&
									!BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode))
								{
									unit_iterator->getUnit()->holdPosition();
								}
								else
								{
									unit_iterator->getUnit()->move(building_list_iterator->getUnit()->getPosition());
								}
								building_list_iterator = game_state.getBuildingList()->end();
							}
							else
								building_list_iterator++;
						}
					}
					if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_High_Templar &&
						unit_iterator->getUnit()->getEnergy() >= 75)
					{
						BWAPI::Unitset scanned_units = unit_iterator->getUnit()->getUnitsInRadius(288);
						for (auto checked_unit : scanned_units)
						{
							if (checked_unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()) &&
								!checked_unit->getType().isBuilding())
							{
								BWAPI::Unitset scanned_units_2 = checked_unit->getUnitsInRadius(48, BWAPI::Filter::IsEnemy);
								if (scanned_units_2.size() >= 3)
								{
									unit_iterator->getUnit()->useTech(BWAPI::TechTypes::Psionic_Storm, checked_unit->getPosition());
									break;
								}
							}
						}
					}
					if (unit_iterator->getUnit()->isIdle())
					{
						if (game_state.getBuildOrder() == BuildOrder::BGHMech)
						{
							if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Marine)
							{
								auto base_list_iterator = game_state.getBaseList()->begin();
								while (base_list_iterator != game_state.getBaseList()->end())
								{
									if (base_list_iterator->getBaseClass() == 3)
									{
										if (base_list_iterator->getArea()->Id() == game_state.getContainingBase(unit_iterator->getUnit())->getArea()->Id())
										{
											unit_iterator->getUnit()->holdPosition();
										}
										else
										{
											BWEM::CPPath path_to_check = BWEM::Map::Instance().GetPath((BWAPI::Position)game_state.getContainingBase(unit_iterator->getUnit())->getArea()->Top(), (BWAPI::Position)base_list_iterator->getArea()->Top());
											if (path_to_check.size() <= 0)
											{
												unit_iterator->getUnit()->move((BWAPI::Position)base_list_iterator->getArea()->Top());
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
						while (enemy_iterator != game_state.getEnemyUnits()->end())
						{
							if (enemy_iterator->second.getUnit()->exists())
							{
								AIBase* base_to_check = game_state.getContainingBase(enemy_iterator->second.getUnit());
								int base_class = -1;
								if (base_to_check != nullptr)
									base_class = base_to_check->getBaseClass();
								if (base_class == 3 ||
									base_class == 4 ||
									base_class == 5)
								{
									if (unit_iterator->getUnit()->getType() != BWAPI::UnitTypes::Protoss_High_Templar)
									{
										if (enemy_iterator->second.getUnit()->getType().isFlyer() &&
											unit_iterator->getUnit()->getType().airWeapon() != BWAPI::WeaponTypes::None)
										{
											unit_iterator->getUnit()->attack(enemy_iterator->second.getUnit()->getPosition());
											enemy_iterator = game_state.getEnemyUnits()->end();
										}
										else if (!enemy_iterator->second.getUnit()->getType().isFlyer())
										{
											unit_iterator->getUnit()->attack(enemy_iterator->second.getUnit()->getPosition());
											enemy_iterator = game_state.getEnemyUnits()->end();
										}
										else
											enemy_iterator++;
									}
									else
									{
										unit_iterator->getUnit()->move(enemy_iterator->second.getUnit()->getPosition());
										enemy_iterator = game_state.getEnemyUnits()->end();
									}
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
					unit_iterator++;
				}
				else if (current_objective->getObjective() == ObjectiveTypes::Attack)
				{
					if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Marine &&
						BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Stim_Packs) &&
						unit_iterator->getUnit()->isAttacking() &&
						!unit_iterator->getUnit()->isStimmed() &&
						unit_iterator->getUnit()->getHitPoints() > 20)
					{
						unit_iterator->getUnit()->useTech(BWAPI::TechTypes::Stim_Packs);
					}
					if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_High_Templar &&
						unit_iterator->getUnit()->getEnergy() >= 75)
					{
						BWAPI::Unitset scanned_units = unit_iterator->getUnit()->getUnitsInRadius(288);
						for (auto checked_unit : scanned_units)
						{
							if (checked_unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()) &&
								!checked_unit->getType().isBuilding())
							{
								BWAPI::Unitset scanned_units_2 = checked_unit->getUnitsInRadius(48, BWAPI::Filter::IsEnemy);
								if (scanned_units_2.size() >= 3)
								{
									unit_iterator->getUnit()->useTech(BWAPI::TechTypes::Psionic_Storm, checked_unit->getPosition());
									break;
								}
							}
						}
					}
					if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode &&
						BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode))
					{
						auto scanned_units = unit_iterator->getUnit()->getUnitsInRadius(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode.groundWeapon().maxRange());
						for (auto check_unit : scanned_units)
						{
							if (check_unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()) &&
								!check_unit->isFlying())
							{
								unit_iterator->getUnit()->useTech(BWAPI::TechTypes::Tank_Siege_Mode);
								break;
							}
						}
					}
					else if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode)
					{
						bool nearby_enemy = false;
						auto scanned_units = unit_iterator->getUnit()->getUnitsInRadius(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode.groundWeapon().maxRange());
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
							unit_iterator->getUnit()->useTech(BWAPI::TechTypes::Tank_Siege_Mode);
						}
					}
					if (target_base != nullptr)
					{
						if (unit_iterator->getUnit()->isIdle() ||
							game_state.getSecondaryScouting())
						{
							auto enemy_unit_iterator = game_state.getEnemyUnits()->begin();
							while (enemy_unit_iterator != game_state.getEnemyUnits()->end())
							{
								if (BWEM::Map::Instance().GetArea(enemy_unit_iterator->second.getDiscoveredPosition()) != nullptr)
								{
									if (enemy_unit_iterator->second.isBuilding() &&
										target_base->getArea()->Id() == BWEM::Map::Instance().GetArea(enemy_unit_iterator->second.getDiscoveredPosition())->Id())
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
													if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_High_Templar)
														unit_iterator->getUnit()->move((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition());
													else
														unit_iterator->getUnit()->attack((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition());
													enemy_unit_iterator++;
												}
											}
										}
										else
										{
											if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_High_Templar)
												unit_iterator->getUnit()->move((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition());
											else
												unit_iterator->getUnit()->attack((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition());
											enemy_unit_iterator = game_state.getEnemyUnits()->end();
										}
									}
									else
									{
										enemy_unit_iterator++;
									}
								}
								else
									enemy_unit_iterator++;
							}
						}
					}
					else
					{
						if (!game_state.getSecondaryScouting())
							game_state.toggleSecondaryScouting();
						if (unit_iterator->getUnit()->isIdle())
						{
							if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition()) != nullptr)
							{
								if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition())->Id() == game_state.getClosestEmptyBaseNotSecondaryScouted()->getArea()->Id())
								{
									game_state.getClosestEmptyBaseNotSecondaryScouted()->toggleSecondaryScouted();
									if (game_state.getClosestEmptyBaseNotSecondaryScouted() != nullptr)
										unit_iterator->getUnit()->attack(game_state.getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
									else
										game_state.resetSecondaryScouting();
								}
								else
									unit_iterator->getUnit()->attack(game_state.getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
							}
							else
							{
								if (game_state.getClosestEmptyBaseNotSecondaryScouted() != nullptr)
									unit_iterator->getUnit()->attack(game_state.getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
								else
									game_state.resetSecondaryScouting();
							}
						}
					}
					unit_iterator++;
				}
				else if (current_objective->getObjective() == ObjectiveTypes::DefendExpansion)
				{
					if (game_state.getTargetExpansion() == nullptr ||
						!game_state.getExpanding())
					{
						game_state.getObjectiveList()->begin()->addUnit(*unit_iterator);
						auto erase_iterator = unit_iterator;
						unit_iterator = current_objective->getUnits()->erase(erase_iterator);
					}
					else if (unit_iterator->getUnit()->getDistance((BWAPI::Position)game_state.getTargetExpansion()->getArea()->Top()) > 500 &&
						unit_iterator->getUnit()->isIdle())
					{
						unit_iterator->getUnit()->attack((BWAPI::Position)game_state.getTargetExpansion()->getArea()->Top());
						unit_iterator++;
					}
					else if (unit_iterator->getUnit()->getDistance((BWAPI::Position)(*game_state.getTargetExpansion()->getArea()->Bases().begin()).Location()) < 300 &&
						unit_iterator->getUnit()->isIdle())
					{
						if (game_state.getTargetExpansion()->getArea()->ChokePoints().size() > 1)
						{
							auto choke_point_iterator = game_state.getTargetExpansion()->getArea()->ChokePoints().begin();
							BWAPI::Position farthest_chokepoint = (BWAPI::Position)(*choke_point_iterator)->Center();
							int farthest_distance = 0;
							choke_point_iterator++;

							AIBase* closest_enemy_base = game_state.getClosestEnemyBase();
							if (closest_enemy_base != nullptr)
							{
								farthest_distance = game_state.getGroundDistance(farthest_chokepoint, (BWAPI::Position)closest_enemy_base->getArea()->Top());
								while (choke_point_iterator != game_state.getTargetExpansion()->getArea()->ChokePoints().end())
								{
									int distance = game_state.getGroundDistance((BWAPI::Position)(*choke_point_iterator)->Center(), (BWAPI::Position)closest_enemy_base->getArea()->Top());
									if (farthest_distance > distance)
									{
										farthest_chokepoint = (BWAPI::Position)(*choke_point_iterator)->Center();
										farthest_distance = distance;
									}
									choke_point_iterator++;
								}
							}
							unit_iterator->getUnit()->attack(farthest_chokepoint);
							unit_iterator++;
						}
						else
						{
							unit_iterator->getUnit()->attack((BWAPI::Position)(*game_state.getTargetExpansion()->getArea()->ChokePoints().begin())->Center());
						}
					}
					else
						unit_iterator++;
				}
				else if (current_objective->getObjective() == ObjectiveTypes::P2GateAttack)
				{
					if (target_base != nullptr)
					{
						if (unit_iterator->getUnit()->isIdle() ||
							game_state.getSecondaryScouting())
						{
							auto enemy_unit_iterator = game_state.getEnemyUnits()->begin();
							while (enemy_unit_iterator != game_state.getEnemyUnits()->end())
							{
								if (BWEM::Map::Instance().GetArea(enemy_unit_iterator->second.getDiscoveredPosition()) != nullptr)
								{
									if (enemy_unit_iterator->second.isBuilding() &&
										target_base->getArea()->Id() == BWEM::Map::Instance().GetArea(enemy_unit_iterator->second.getDiscoveredPosition())->Id())
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
													if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_High_Templar)
														unit_iterator->getUnit()->move((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition());
													else
														unit_iterator->getUnit()->attack((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition());
													enemy_unit_iterator++;
												}
											}
										}
										else
										{
											if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_High_Templar)
												unit_iterator->getUnit()->move((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition());
											else
												unit_iterator->getUnit()->attack((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition());
											enemy_unit_iterator = game_state.getEnemyUnits()->end();
										}
									}
									else
									{
										enemy_unit_iterator++;
									}
								}
								else
									enemy_unit_iterator++;
							}
						}
					}
					else
					{
						if (!game_state.getSecondaryScouting())
							game_state.toggleSecondaryScouting();
						if (unit_iterator->getUnit()->isIdle())
						{
							if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition()) != nullptr &&
								game_state.getClosestEmptyStartLocationNotSecondaryScouted() != nullptr)
							{
								if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition())->Id() == game_state.getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Id())
								{
									game_state.getClosestEmptyStartLocationNotSecondaryScouted()->toggleSecondaryScouted();
									if (game_state.getClosestEmptyStartLocationNotSecondaryScouted() != nullptr)
										unit_iterator->getUnit()->attack(game_state.getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Bases().begin()->Center());
									else
									{
										if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition()) != nullptr)
										{
											if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition())->Id() == game_state.getClosestEmptyBaseNotSecondaryScouted()->getArea()->Id())
											{
												game_state.getClosestEmptyBaseNotSecondaryScouted()->toggleSecondaryScouted();
												if (game_state.getClosestEmptyBaseNotSecondaryScouted() != nullptr)
													unit_iterator->getUnit()->attack(game_state.getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
												else
													game_state.resetSecondaryScouting();
											}
											else
												unit_iterator->getUnit()->attack(game_state.getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
										}
										else
										{
											if (game_state.getClosestEmptyBaseNotSecondaryScouted() != nullptr)
												unit_iterator->getUnit()->attack(game_state.getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
											else
												game_state.resetSecondaryScouting();
										}
									}
								}
								else
									unit_iterator->getUnit()->attack(game_state.getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Bases().begin()->Center());
							}
							else
							{
								if (game_state.getClosestEmptyStartLocationNotSecondaryScouted() != nullptr)
									unit_iterator->getUnit()->attack(game_state.getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Bases().begin()->Center());
								else
								{
									if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition()) != nullptr)
									{
										if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition())->Id() == game_state.getClosestEmptyBaseNotSecondaryScouted()->getArea()->Id())
										{
											game_state.getClosestEmptyBaseNotSecondaryScouted()->toggleSecondaryScouted();
											if (game_state.getClosestEmptyBaseNotSecondaryScouted() != nullptr)
												unit_iterator->getUnit()->attack(game_state.getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
											else
												game_state.resetSecondaryScouting();
										}
										else
											unit_iterator->getUnit()->attack(game_state.getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
									}
									else
									{
										if (game_state.getClosestEmptyBaseNotSecondaryScouted() != nullptr)
											unit_iterator->getUnit()->attack(game_state.getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
										else
											game_state.resetSecondaryScouting();
									}
								}
							}
						}
					}
					unit_iterator++;
				}
				else
				{
					unit_iterator++;
				}
			}
			current_objective++;
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
					if (base_list_iterator->getArea() == BWEM::Map::Instance().GetArea(scout_unit.getUnit()->getTilePosition()) &&
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

	if (target_base != nullptr &&
		game_state.getSecondaryScouting())
	{
		game_state.toggleSecondaryScouting();
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