#include "MilitaryManager.h"

MilitaryManager::MilitaryManager(BWAPI::Game * game) :
  fap(game)
{
	scout_target = BWAPI::Positions::Invalid;
	global_strategy = 0;
  Broodwar = game;
}

void MilitaryManager::checkMilitary(std::unique_ptr<WorkerManager> &worker_manager, std::unique_ptr<GameState> &game_state)
{
	AIBase* target_base = game_state->getClosestEnemyBase();
	bool defense_target_aquired = false;
	BWAPI::Position defense_target_position;
	/*if (global_strategy == 0 &&
		game_state->getMilitary()->size() > 50 &&
		Broodwar->self()->getRace() == BWAPI::Races::Terran &&
		game_state->getBuildOrder() == "default")
	{
		global_strategy = 1;
	}
	else if (global_strategy == 1 &&
		game_state->getMilitary()->size() <= 50 &&
		Broodwar->self()->getRace() == BWAPI::Races::Terran &&
		game_state->getBuildOrder() == "default")
	{
		global_strategy = 0;
	}
	else if (global_strategy == 0 &&
		game_state->getMilitary()->size() > 24 &&
		Broodwar->self()->getRace() == BWAPI::Races::Terran &&
		game_state->getBuildOrder() == BuildOrder::BGHMech)
	{
		global_strategy = 1;
		auto building_list_iterator = game_state->getBuildingList()->begin();
		while (building_list_iterator != game_state->getBuildingList()->end())
		{
			if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks &&
				!building_list_iterator->getUnit()->isFlying())
			{
				building_list_iterator->getUnit()->lift();
				building_list_iterator = game_state->getBuildingList()->end();
			}
			else
				building_list_iterator++;
		}
	}
	else if (global_strategy == 1 &&
		game_state->getMilitary()->size() <= 12 &&
		Broodwar->self()->getRace() == BWAPI::Races::Terran &&
		game_state->getBuildOrder() == BuildOrder::BGHMech)
	{
		global_strategy = 0;
	}
	else if (global_strategy == 0 &&
		game_state->getMilitary()->size() > 30 &&
		Broodwar->self()->getRace() == BWAPI::Races::Protoss)
	{
		global_strategy = 1;
	}
	else if (global_strategy == 1 &&
		game_state->getMilitary()->size() <= 30 &&
		Broodwar->self()->getRace() == BWAPI::Races::Protoss)
	{
		global_strategy = 0;
	}
	else if (global_strategy == 0 &&
		game_state->getSupplyUsed() > 180 &&
		Broodwar->self()->getRace() == BWAPI::Races::Zerg)
	{
		global_strategy = 1;
	}
	else if (global_strategy == 1 &&
		game_state->getSupplyUsed() <= 60 &&
		Broodwar->self()->getRace() == BWAPI::Races::Zerg)
	{
		global_strategy = 0;
	}
	if (global_strategy == 1)
	{
		
	}
	
	auto military_iterator = game_state->getMilitary()->begin();
	while (military_iterator != game_state->getMilitary()->end())
	{
		if (military_iterator->getUnit() == nullptr)
		{
			auto erase_iterator = military_iterator;
			military_iterator = game_state->getMilitary()->erase(erase_iterator);
		}
		else if (!military_iterator->getUnit()->exists())
		{
			auto erase_iterator = military_iterator;
			military_iterator = game_state->getMilitary()->erase(erase_iterator);
		}
		else
		{
			if (Broodwar->self()->getRace() == BWAPI::Races::Terran)
			{
				if (Broodwar->self()->hasResearched(BWAPI::TechTypes::Stim_Packs) &&
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
	for (auto unit : Broodwar->enemies().getUnits())
	{
		if (game_state->getEnemyUnits()->find(static_cast<int>(unit->getID())) == game_state->getEnemyUnits()->end() &&
			unit->exists() &&
			unit->getType() != BWAPI::UnitTypes::Resource_Vespene_Geyser)
		{
			Object new_enemy(unit);
			new_enemy.setDiscoveredPosition(unit->getTilePosition());
			if (unit->getType().isBuilding())
				new_enemy.setIsBuilding();
			game_state->getEnemyUnits()->insert(std::pair<int, Object>(unit->getID(), new_enemy));
		}
		if ((unit->isBurrowed() ||
			unit->isCloaked()) &&
			!unit->isDetected() &&
			game_state->checkComsatStation() &&
			Broodwar->elapsedTime() - game_state->getLastScan() > 5)
		{
			for (auto unit_to_check : Broodwar->getUnitsInRadius(unit->getPosition(), BWAPI::UnitTypes::Zerg_Lurker.groundWeapon().maxRange()))
			{
				if (unit_to_check->getPlayer() == Broodwar->self())
				{
					auto building_list_iterator = game_state->getBuildingList()->begin();
					while (building_list_iterator != game_state->getBuildingList()->end())
					{
						if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Comsat_Station &&
							building_list_iterator->getUnit()->getEnergy() >= 50)
						{
							building_list_iterator->getUnit()->useTech(BWAPI::TechTypes::Scanner_Sweep, unit->getPosition());
							building_list_iterator = game_state->getBuildingList()->end();
							game_state->setLastScan(Broodwar->elapsedTime());
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
			Object* detector = game_state->getAvailableDetector();
			if (detector != nullptr)
			{
				detector->getUnit()->move(unit->getPosition());
			}
		}

	}
	auto current_objective = game_state->getObjectiveList()->begin();
	while (current_objective != game_state->getObjectiveList()->end())
	{
		BWAPI::TilePosition idlePosition(0, 0);
		if (current_objective->getUnits()->size() == 0 &&
			current_objective->getObjective() != ObjectiveTypes::Defend)
		{
			auto erase_iterator = current_objective;
			current_objective = game_state->getObjectiveList()->erase(erase_iterator);
		}
		else
		{
			if (current_objective->getObjective() == ObjectiveTypes::Attack &&
				game_state->getObjectiveList()->begin()->getUnits()->size() >= 3 * game_state->getBarracks())
			{
				auto current_unit = game_state->getObjectiveList()->begin()->getUnits()->begin();
				while (current_unit != game_state->getObjectiveList()->begin()->getUnits()->end())
				{
					current_objective->addUnit(*current_unit);
					current_unit++;
				}
				game_state->getObjectiveList()->begin()->getUnits()->clear();
			}
			else if (current_objective->getObjective() == ObjectiveTypes::P2GateAttack &&
				game_state->getObjectiveList()->begin()->getUnits()->size() > 0)
			{
				auto current_unit = game_state->getObjectiveList()->begin()->getUnits()->begin();
				while (current_unit != game_state->getObjectiveList()->begin()->getUnits()->end())
				{
					current_objective->addUnit(*current_unit);
					current_unit++;
				}
				game_state->getObjectiveList()->begin()->getUnits()->clear();
			}
			else if (current_objective->getObjective() == ObjectiveTypes::P4GateGoonAttack &&
				game_state->getEnemyTotalStrength() > game_state->getObjectiveStrength(*current_objective))
			{
				AIBase* main_base = game_state->getMainBase();
				auto current_unit = current_objective->getUnits()->begin();
				while (current_unit != current_objective->getUnits()->end())
				{
					game_state->getObjectiveList()->begin()->addUnit(*current_unit);
					if (main_base != nullptr)
						current_unit->getUnit()->move((BWAPI::Position)main_base->getArea()->Top());
					current_unit++;
				}
				current_objective->getUnits()->clear();
			}
			else if (current_objective->getObjective() == ObjectiveTypes::P4GateGoonAttack &&
				game_state->getObjectiveList()->begin()->getUnits()->size() > 0)
			{
				auto current_unit = game_state->getObjectiveList()->begin()->getUnits()->begin();
				while (current_unit != game_state->getObjectiveList()->begin()->getUnits()->end())
				{
					current_objective->addUnit(*current_unit);
					current_unit++;
				}
				game_state->getObjectiveList()->begin()->getUnits()->clear();
			}
			else if (current_objective->getObjective() == ObjectiveTypes::AttackWithRegroupPrioritizeWorkers &&
				game_state->getObjectiveList()->begin()->getUnits()->size() > 0)
			{
				auto current_unit = game_state->getObjectiveList()->begin()->getUnits()->begin();
				while (current_unit != game_state->getObjectiveList()->begin()->getUnits()->end())
				{
					current_objective->addUnit(*current_unit);
					current_unit++;
				}
				game_state->getObjectiveList()->begin()->getUnits()->clear();
			}
			if (current_objective->getObjective() == ObjectiveTypes::AttackWithRegroupPrioritizeWorkers &&
				game_state->getTimesRetreated() >= 2)
			{
				auto current_unit = current_objective->getUnits()->begin();
				while (current_unit != current_objective->getUnits()->end())
				{
					game_state->getObjectiveList()->begin()->addUnit(*current_unit);
					current_unit++;
				}
				current_objective->getUnits()->clear();
			}
			if (current_objective->getObjective() == ObjectiveTypes::P4GateGoonAttack ||
				current_objective->getObjective() == ObjectiveTypes::AttackWithRegroupPrioritizeWorkers)
			{
				if (Broodwar->getFrameCount() - current_objective->getLastFapFrame() > 480 ||
					!current_objective->getRegroup())
				{
					bool has_bunker = false;
					bool bunker_dead = true;
					fap.clearState();
					for (auto &unit : *current_objective->getUnits())
					{
						fap.addIfCombatUnitPlayer1(unit);
					}
					for (auto &unit : *game_state->getEnemyUnits())
					{
						if (unit.second.getPlayer()->isEnemy(Broodwar->self()) &&
							unit.second.getCurrentPosition() != BWAPI::Positions::Unknown)
						{
							fap.addIfCombatUnitPlayer2(unit.second);
							if (unit.second.getType() == BWAPI::UnitTypes::Terran_Bunker &&
								!has_bunker)
								has_bunker = true;
						}
						else
							fap.addIfCombatUnitPlayer1(unit.second);
					}
					std::pair<int, int> presim_scores = fap.playerScores();
					int presim_my_unit_count = fap.getState().first->size();
					int presim_enemy_unit_count = fap.getState().second->size();

					fap.simulate(480);

					int postsim_my_unit_count = fap.getState().first->size();
					int postsim_enemy_unit_count = fap.getState().second->size();
					std::pair<int, int> postsim_scores = fap.playerScores();
					int my_losses = presim_my_unit_count - postsim_my_unit_count;
					int enemy_losses = presim_enemy_unit_count - postsim_enemy_unit_count;
					int my_score_diff = presim_scores.first - postsim_scores.first;
					int enemy_score_diff = presim_scores.second - postsim_scores.second;
					if (my_losses > enemy_losses &&
						my_score_diff > 2 * enemy_score_diff)
					{
						if (has_bunker)
						{
							for (auto &unit : *fap.getState().second)
							{
								if (unit.unitType == BWAPI::UnitTypes::Terran_Bunker)
								{
									bunker_dead = false;
									break;
								}
							}
							if (bunker_dead)
							{
								current_objective->setRegroup(false);
								current_objective->setRegroupTarget(BWAPI::Positions::None);
							}
							else
							{
								current_objective->setRegroup(true);
								game_state->addTimesRetreated(1);
								AIBase* regroup_base = game_state->getSafeEmptyBaseClosestToEnemy();
								if (regroup_base != nullptr)
									current_objective->setRegroupTarget((BWAPI::Position)regroup_base->getArea()->Top());
								else
									current_objective->setRegroupTarget(current_objective->getCentroid());
							}
						}
						else
						{
							current_objective->setRegroup(true);
							game_state->addTimesRetreated(1);
							AIBase* regroup_base = game_state->getSafeEmptyBaseClosestToEnemy();
							if (regroup_base != nullptr)
								current_objective->setRegroupTarget((BWAPI::Position)regroup_base->getArea()->Top());
							else
								current_objective->setRegroupTarget(current_objective->getCentroid());
						}
					}
					else
					{
						current_objective->setRegroup(false);
						current_objective->setRegroupTarget(BWAPI::Positions::None);
					}
					current_objective->setLastFapFrame(Broodwar->getFrameCount());
				}
				else
				{
					AIBase * regroup_base = game_state->getSafeEmptyBaseClosestToEnemy();
					if (regroup_base != nullptr)
					{
						if (regroup_base->getArea() != BWEM::Map::Instance().GetArea((BWAPI::TilePosition)current_objective->getRegroupTarget()))
						{
							current_objective->setRegroupTarget((BWAPI::Position)regroup_base->getArea()->Top());
						}
					}
				}
			}
			if (current_objective->getObjective() == ObjectiveTypes::Defend)
			{
				int highest_score = 0;
				for (int x = 0; x < Broodwar->mapWidth(); x++)
				{
					for (int y = 0; y < Broodwar->mapHeight(); y++)
					{
						if (game_state->getDefenseGroundScore(BWAPI::TilePosition(x, y)) > highest_score)
						{
							highest_score = game_state->getDefenseGroundScore(BWAPI::TilePosition(x, y));
							idlePosition = BWAPI::TilePosition(x, y);
						}
					}
				}
				auto enemy_iterator = game_state->getEnemyUnits()->begin();
				while (enemy_iterator != game_state->getEnemyUnits()->end())
				{
					if (enemy_iterator->second.getUnit()->exists())
					{
						AIBase* base_to_check = game_state->getContainingBase(enemy_iterator->second.getUnit());
						int base_class = -1;
						if (base_to_check != nullptr)
							base_class = base_to_check->getBaseClass();
						if (base_class == 3 ||
							base_class == 4 ||
							base_class == 5)
						{
							defense_target_position = enemy_iterator->second.getUnit()->getPosition();
							defense_target_aquired = true;
							enemy_iterator = game_state->getEnemyUnits()->end();
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



			auto unit_iterator = current_objective->getUnits()->begin();
			while (unit_iterator != current_objective->getUnits()->end())
			{
				if (!unit_iterator->getUnit()->exists())
				{
					auto military_iterator = game_state->getMilitary()->begin();
					while (military_iterator != game_state->getMilitary()->end())
					{
						if (military_iterator->second.getUnit()->getID() == unit_iterator->getUnit()->getID())
						{
							auto erase_iterator = military_iterator;
							game_state->getMilitary()->erase(erase_iterator);
							military_iterator = game_state->getMilitary()->end();
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
						Broodwar->self()->hasResearched(BWAPI::TechTypes::Stim_Packs) &&
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
							if (checked_unit->getPlayer()->isEnemy(Broodwar->self()) &&
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
						Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode))
					{
						int max_range = BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode.groundWeapon().maxRange();
						if (game_state->getGroundDistance(unit_iterator->getUnit()->getPosition(), (BWAPI::Position)idlePosition) <= max_range / 2 &&
							!defense_target_aquired)
						{
							unit_iterator->getUnit()->useTech(BWAPI::TechTypes::Tank_Siege_Mode);
						}
						else
						{
							auto scanned_units = unit_iterator->getUnit()->getUnitsInRadius(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode.groundWeapon().maxRange());
							for (auto check_unit : scanned_units)
							{
								if (check_unit->getPlayer()->isEnemy(Broodwar->self()) &&
									!check_unit->isFlying())
								{
									unit_iterator->getUnit()->useTech(BWAPI::TechTypes::Tank_Siege_Mode);
									break;
								}
							}
						}
					}
					else if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode)
					{
						int max_range = BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode.groundWeapon().maxRange();
						if (defense_target_aquired ||
							game_state->getGroundDistance(unit_iterator->getUnit()->getPosition(), (BWAPI::Position)idlePosition) > max_range / 2)
						{
							bool nearby_enemy = false;
							auto scanned_units = unit_iterator->getUnit()->getUnitsInRadius(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode.groundWeapon().maxRange());
							for (auto check_unit : scanned_units)
							{
								if (check_unit->getPlayer()->isEnemy(Broodwar->self()) &&
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
					}
					AIBase* my_current_area = game_state->getContainingBase(unit_iterator->getUnit());
					if (my_current_area != nullptr)
					{
						if (my_current_area->getBaseClass() != 3 &&
							my_current_area->getBaseClass() != 4 &&
							my_current_area->getBaseClass() != 5 &&
							game_state->getLocalStrength(*unit_iterator) < 0)
						{
							AIBase* retreat_base = game_state->getMyClosestBase(*unit_iterator);
							if (retreat_base != nullptr)
							{
								unit_iterator->getUnit()->move((BWAPI::Position)retreat_base->getArea()->Top());
							}
						}
					}
					if (unit_iterator->getUnit()->isIdle() ||
						unit_iterator->getUnit()->getOrder() == BWAPI::Orders::Move)
					{
						if (defense_target_aquired)
						{
							if (unit_iterator->getType() != BWAPI::UnitTypes::Protoss_High_Templar)
								unit_iterator->getUnit()->attack(defense_target_position);
							else
								unit_iterator->getUnit()->move(defense_target_position);
						}
						int max_ground_range = unit_iterator->getUnit()->getType().groundWeapon().maxRange();
						int ground_distance_from_idle_position = game_state->getGroundDistance(unit_iterator->getUnit()->getPosition(), (BWAPI::Position)idlePosition);
						if (!defense_target_aquired &&
							max_ground_range < 33)
						{
							if (ground_distance_from_idle_position > 100)
								unit_iterator->getUnit()->move((BWAPI::Position)idlePosition);
							else if (unit_iterator->getUnit()->getOrder() == BWAPI::Orders::Move &&
								ground_distance_from_idle_position <= 100)
								unit_iterator->getUnit()->stop();
						}
						else
						{
							if (!defense_target_aquired &&
								ground_distance_from_idle_position > max_ground_range)
								unit_iterator->getUnit()->move((BWAPI::Position)idlePosition);
							else if (!defense_target_aquired &&
								unit_iterator->getUnit()->getOrder() == BWAPI::Orders::Move &&
								ground_distance_from_idle_position <= max_ground_range)
							{
								if (unit_iterator->getType() != BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode)
									unit_iterator->getUnit()->stop();
							}	
						}
					}
					
					unit_iterator++;
				}
				else if (current_objective->getObjective() == ObjectiveTypes::Attack)
				{
					if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Marine &&
						Broodwar->self()->hasResearched(BWAPI::TechTypes::Stim_Packs) &&
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
							if (checked_unit->getPlayer()->isEnemy(Broodwar->self()) &&
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
						Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode))
					{
						auto scanned_units = unit_iterator->getUnit()->getUnitsInRadius(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode.groundWeapon().maxRange());
						for (auto check_unit : scanned_units)
						{
							if (check_unit->getPlayer()->isEnemy(Broodwar->self()) &&
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
							if (check_unit->getPlayer()->isEnemy(Broodwar->self()) &&
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
							game_state->getSecondaryScouting())
						{
							auto enemy_unit_iterator = game_state->getEnemyUnits()->begin();
							while (enemy_unit_iterator != game_state->getEnemyUnits()->end())
							{
								if (BWEM::Map::Instance().GetArea(enemy_unit_iterator->second.getDiscoveredPosition()) != nullptr)
								{
									if (enemy_unit_iterator->second.isBuilding() &&
										target_base->getArea()->Id() == BWEM::Map::Instance().GetArea(enemy_unit_iterator->second.getDiscoveredPosition())->Id())
									{
										if (Broodwar->isVisible(enemy_unit_iterator->second.getDiscoveredPosition()))
										{
											BWAPI::Unitset scanned_units = Broodwar->getUnitsInRadius((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition(), 200);
											auto scanned_iterator = scanned_units.begin();
											if (scanned_units.size() == 0)
											{
												auto erase_iterator = enemy_unit_iterator;
												enemy_unit_iterator = game_state->getEnemyUnits()->erase(erase_iterator);
											}
											else
											{
												bool enemy_found = false;
												while (scanned_iterator != scanned_units.end())
												{
													if ((*scanned_iterator)->getPlayer()->isEnemy(Broodwar->self()))
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
													enemy_unit_iterator = game_state->getEnemyUnits()->erase(erase_iterator);
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
											enemy_unit_iterator = game_state->getEnemyUnits()->end();
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
						if (!game_state->getSecondaryScouting())
							game_state->toggleSecondaryScouting();
						if (unit_iterator->getUnit()->isIdle())
						{
							if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition()) != nullptr &&
								game_state->getClosestEmptyBaseNotSecondaryScouted() != nullptr)
							{
								if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition())->Id() == game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Id())
								{
									game_state->getClosestEmptyBaseNotSecondaryScouted()->toggleSecondaryScouted();
									if (game_state->getClosestEmptyBaseNotSecondaryScouted() != nullptr)
										unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
									else
										game_state->resetSecondaryScouting();
								}
								else
									unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
							}
							else
							{
								if (game_state->getClosestEmptyBaseNotSecondaryScouted() != nullptr)
									unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
								else
									game_state->resetSecondaryScouting();
							}
						}
					}
					unit_iterator++;
				}
				else if (current_objective->getObjective() == ObjectiveTypes::DefendExpansion)
				{
					BWAPI::Position farthest_chokepoint;
					if (game_state->getTargetExpansion() == nullptr ||
						!game_state->getExpanding())
					{
						game_state->getObjectiveList()->begin()->addUnit(*unit_iterator);
						auto erase_iterator = unit_iterator;
						unit_iterator = current_objective->getUnits()->erase(erase_iterator);
					}
					else
					{
						farthest_chokepoint = (BWAPI::Position)(*game_state->getTargetExpansion()->getArea()->ChokePoints().begin())->Center();
						if (game_state->getTargetExpansion()->getArea()->ChokePoints().size() > 1)
						{
							auto choke_point_iterator = game_state->getTargetExpansion()->getArea()->ChokePoints().begin();
							int farthest_distance = 0;
							choke_point_iterator++;
							AIBase* closest_enemy_base = game_state->getClosestEnemyBase();
							if (closest_enemy_base != nullptr)
							{
								farthest_distance = game_state->getGroundDistance(farthest_chokepoint, (BWAPI::Position)closest_enemy_base->getArea()->Top());
								while (choke_point_iterator != game_state->getTargetExpansion()->getArea()->ChokePoints().end())
								{
									int distance = game_state->getGroundDistance((BWAPI::Position)(*choke_point_iterator)->Center(), (BWAPI::Position)closest_enemy_base->getArea()->Top());
									if (farthest_distance > distance)
									{
										farthest_chokepoint = (BWAPI::Position)(*choke_point_iterator)->Center();
										farthest_distance = distance;
									}
									choke_point_iterator++;
								}
							}
						}
						if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode &&
							Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode))
						{
							auto scanned_units = unit_iterator->getUnit()->getUnitsInRadius(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode.groundWeapon().maxRange());
							for (auto check_unit : scanned_units)
							{
								if (check_unit->getPlayer()->isEnemy(Broodwar->self()) &&
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
								if (check_unit->getPlayer()->isEnemy(Broodwar->self()) &&
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
						if (unit_iterator->getUnit()->getDistance((BWAPI::Position)game_state->getTargetExpansion()->getArea()->Top()) > game_state->getGroundDistance((BWAPI::Position)game_state->getTargetExpansion()->getArea()->Top(), farthest_chokepoint) &&
							unit_iterator->getUnit()->isIdle())
						{
							unit_iterator->getUnit()->attack((BWAPI::Position)game_state->getTargetExpansion()->getArea()->Top());
							unit_iterator++;
						}
						else if (unit_iterator->getUnit()->getDistance((BWAPI::Position)game_state->getTargetExpansion()->getArea()->Top()) < game_state->getGroundDistance((BWAPI::Position)game_state->getTargetExpansion()->getArea()->Top(), farthest_chokepoint) &&
							unit_iterator->getUnit()->isIdle())
						{
							unit_iterator->getUnit()->attack(farthest_chokepoint);
							unit_iterator++;
						}
						else
							unit_iterator++;
					}
				}
				else if (current_objective->getObjective() == ObjectiveTypes::P2GateAttack)
				{
					if (target_base != nullptr)
					{
						if (unit_iterator->getUnit()->isIdle() ||
							game_state->getSecondaryScouting())
						{
							auto enemy_unit_iterator = game_state->getEnemyUnits()->begin();
							while (enemy_unit_iterator != game_state->getEnemyUnits()->end())
							{
								if (BWEM::Map::Instance().GetArea(enemy_unit_iterator->second.getDiscoveredPosition()) != nullptr)
								{
									if (enemy_unit_iterator->second.isBuilding() &&
										target_base->getArea()->Id() == BWEM::Map::Instance().GetArea(enemy_unit_iterator->second.getDiscoveredPosition())->Id())
									{
										if (Broodwar->isVisible(enemy_unit_iterator->second.getDiscoveredPosition()))
										{
											BWAPI::Unitset scanned_units = Broodwar->getUnitsInRadius((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition(), 200);
											auto scanned_iterator = scanned_units.begin();
											if (scanned_units.size() == 0)
											{
												auto erase_iterator = enemy_unit_iterator;
												enemy_unit_iterator = game_state->getEnemyUnits()->erase(erase_iterator);
											}
											else
											{
												bool enemy_found = false;
												while (scanned_iterator != scanned_units.end())
												{
													if ((*scanned_iterator)->getPlayer()->isEnemy(Broodwar->self()))
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
													enemy_unit_iterator = game_state->getEnemyUnits()->erase(erase_iterator);
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
											enemy_unit_iterator = game_state->getEnemyUnits()->end();
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
						if (!game_state->getSecondaryScouting())
							game_state->toggleSecondaryScouting();
						if (unit_iterator->getUnit()->isIdle())
						{
							if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition()) != nullptr &&
								game_state->getClosestEmptyStartLocationNotSecondaryScouted() != nullptr)
							{
								if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition())->Id() == game_state->getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Id())
								{
									game_state->getClosestEmptyStartLocationNotSecondaryScouted()->toggleSecondaryScouted();
									if (game_state->getClosestEmptyStartLocationNotSecondaryScouted() != nullptr)
										unit_iterator->getUnit()->attack(game_state->getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Bases().begin()->Center());
									else
									{
										if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition()) != nullptr)
										{
											if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition())->Id() == game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Id())
											{
												game_state->getClosestEmptyBaseNotSecondaryScouted()->toggleSecondaryScouted();
												if (game_state->getClosestEmptyBaseNotSecondaryScouted() != nullptr)
													unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
												else
													game_state->resetSecondaryScouting();
											}
											else
												unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
										}
										else
										{
											if (game_state->getClosestEmptyBaseNotSecondaryScouted() != nullptr)
												unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
											else
												game_state->resetSecondaryScouting();
										}
									}
								}
								else
									unit_iterator->getUnit()->attack(game_state->getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Bases().begin()->Center());
							}
							else
							{
								if (game_state->getClosestEmptyStartLocationNotSecondaryScouted() != nullptr)
									unit_iterator->getUnit()->attack(game_state->getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Bases().begin()->Center());
								else
								{
									if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition()) != nullptr)
									{
										if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition())->Id() == game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Id())
										{
											game_state->getClosestEmptyBaseNotSecondaryScouted()->toggleSecondaryScouted();
											if (game_state->getClosestEmptyBaseNotSecondaryScouted() != nullptr)
												unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
											else
												game_state->resetSecondaryScouting();
										}
										else
											unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
									}
									else
									{
										if (game_state->getClosestEmptyBaseNotSecondaryScouted() != nullptr)
											unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
										else
											game_state->resetSecondaryScouting();
									}
								}
							}
						}
					}
					unit_iterator++;
				}
				else if (current_objective->getObjective() == ObjectiveTypes::P4GateGoonAttack)
				{
					if (current_objective->getRegroup())
					{
						/*BWAPI::Position centroid = current_objective->getCentroid();
						if (centroid != BWAPI::Positions::Invalid)
						{
							unit_iterator->getUnit()->move(centroid);
						}*/
						unit_iterator->getUnit()->move(current_objective->getRegroupTarget());
						unit_iterator++;
					}
					
					/*if (game_state->getLocalStrength(*unit_iterator) < 0)
					{
						BWAPI::Position centroid = current_objective->getCentroid();
						if (centroid != BWAPI::Positions::Invalid)
						{
							unit_iterator->getUnit()->move(centroid);
						}
						unit_iterator++;
					}*/
					else
					{
						if (target_base != nullptr)
						{
							if (unit_iterator->getUnit()->isIdle() ||
								game_state->getSecondaryScouting() ||
								unit_iterator->getUnit()->getOrder() == BWAPI::Orders::Move)
							{
								auto enemy_unit_iterator = game_state->getEnemyUnits()->begin();
								while (enemy_unit_iterator != game_state->getEnemyUnits()->end())
								{
									if (BWEM::Map::Instance().GetArea(enemy_unit_iterator->second.getDiscoveredPosition()) != nullptr)
									{
										if (enemy_unit_iterator->second.isBuilding() &&
											target_base->getArea()->Id() == BWEM::Map::Instance().GetArea(enemy_unit_iterator->second.getDiscoveredPosition())->Id())
										{
											if (Broodwar->isVisible(enemy_unit_iterator->second.getDiscoveredPosition()))
											{
												BWAPI::Unitset scanned_units = Broodwar->getUnitsInRadius((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition(), 200);
												auto scanned_iterator = scanned_units.begin();
												if (scanned_units.size() == 0)
												{
													auto erase_iterator = enemy_unit_iterator;
													enemy_unit_iterator = game_state->getEnemyUnits()->erase(erase_iterator);
												}
												else
												{
													bool enemy_found = false;
													while (scanned_iterator != scanned_units.end())
													{
														if ((*scanned_iterator)->getPlayer()->isEnemy(Broodwar->self()))
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
														enemy_unit_iterator = game_state->getEnemyUnits()->erase(erase_iterator);
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
												enemy_unit_iterator = game_state->getEnemyUnits()->end();
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
							if (!game_state->getSecondaryScouting())
								game_state->toggleSecondaryScouting();
							if (unit_iterator->getUnit()->isIdle())
							{
								if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition()) != nullptr &&
									game_state->getClosestEmptyStartLocationNotSecondaryScouted() != nullptr)
								{
									if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition())->Id() == game_state->getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Id())
									{
										game_state->getClosestEmptyStartLocationNotSecondaryScouted()->toggleSecondaryScouted();
										if (game_state->getClosestEmptyStartLocationNotSecondaryScouted() != nullptr)
											unit_iterator->getUnit()->attack(game_state->getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Bases().begin()->Center());
										else
										{
											if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition()) != nullptr)
											{
												if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition())->Id() == game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Id())
												{
													game_state->getClosestEmptyBaseNotSecondaryScouted()->toggleSecondaryScouted();
													if (game_state->getClosestEmptyBaseNotSecondaryScouted() != nullptr)
														unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
													else
														game_state->resetSecondaryScouting();
												}
												else
													unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
											}
											else
											{
												if (game_state->getClosestEmptyBaseNotSecondaryScouted() != nullptr)
													unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
												else
													game_state->resetSecondaryScouting();
											}
										}
									}
									else
										unit_iterator->getUnit()->attack(game_state->getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Bases().begin()->Center());
								}
								else
								{
									if (game_state->getClosestEmptyStartLocationNotSecondaryScouted() != nullptr)
										unit_iterator->getUnit()->attack(game_state->getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Bases().begin()->Center());
									else
									{
										if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition()) != nullptr)
										{
											if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition())->Id() == game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Id())
											{
												game_state->getClosestEmptyBaseNotSecondaryScouted()->toggleSecondaryScouted();
												if (game_state->getClosestEmptyBaseNotSecondaryScouted() != nullptr)
													unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
												else
													game_state->resetSecondaryScouting();
											}
											else
												unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
										}
										else
										{
											if (game_state->getClosestEmptyBaseNotSecondaryScouted() != nullptr)
												unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
											else
												game_state->resetSecondaryScouting();
										}
									}
								}
							}
						}
						unit_iterator++;
					}
				}
				else if (current_objective->getObjective() == ObjectiveTypes::AttackWithRegroupPrioritizeWorkers)
				{
					if (current_objective->getRegroup())
					{
						/*BWAPI::Position centroid = current_objective->getCentroid();
						unit_iterator->getUnit()->move(centroid);*/
						unit_iterator->getUnit()->move(current_objective->getRegroupTarget());
						unit_iterator++;
					}
					/*if (game_state->getLocalStrength(*unit_iterator) < 0)
					{
						BWAPI::Position centroid = current_objective->getCentroid();
						if (centroid != BWAPI::Positions::Invalid)
						{
							unit_iterator->getUnit()->move(centroid);
						}
						unit_iterator++;
					}*/
					else
					{
						if (target_base != nullptr)
						{
							BWAPI::Unit target_unit = nullptr;
							auto enemy_unit_iterator = game_state->getEnemyUnits()->begin();
							while (enemy_unit_iterator != game_state->getEnemyUnits()->end())
							{
								if (enemy_unit_iterator->second.getUnit()->exists())
								{
									auto containing_area = BWEM::Map::Instance().GetArea(enemy_unit_iterator->second.getUnit()->getTilePosition());
									if (containing_area != nullptr)
									{
										for (auto &chokepoint : containing_area->ChokePoints())
										{
											if (enemy_unit_iterator->second.getUnit()->getDistance((BWAPI::Position)chokepoint->Center()) < 100 &&
												game_state->getGroundDistance(enemy_unit_iterator->second.getCurrentPosition(), unit_iterator->getCurrentPosition()) < 100 &&
												unit_iterator->getUnit()->getTarget() != enemy_unit_iterator->second.getUnit())
											{
												
												if (!target_unit)
													target_unit = enemy_unit_iterator->second.getUnit();
												else if (unit_iterator->getUnit()->getDistance(target_unit) > unit_iterator->getUnit()->getDistance(enemy_unit_iterator->second.getUnit()))
													target_unit = enemy_unit_iterator->second.getUnit();
												break;
											}
										}
									}
									if (!target_unit)
									{
										if (enemy_unit_iterator->second.getType() == BWAPI::UnitTypes::Terran_Bunker &&
											game_state->getGroundDistance(enemy_unit_iterator->second.getCurrentPosition(), unit_iterator->getCurrentPosition()) < unit_iterator->getType().sightRange() &&
											unit_iterator->getUnit()->getTarget() != enemy_unit_iterator->second.getUnit())
										{
											unit_iterator->getUnit()->attack(enemy_unit_iterator->second.getUnit());
											target_unit = enemy_unit_iterator->second.getUnit();
										}
									}
									if (!target_unit)
									{
										if (enemy_unit_iterator->second.getType().isWorker() &&
											game_state->getGroundDistance(enemy_unit_iterator->second.getCurrentPosition(), unit_iterator->getCurrentPosition()) < unit_iterator->getType().sightRange() &&
											unit_iterator->getUnit()->getTarget() != enemy_unit_iterator->second.getUnit())
										{
											target_unit = enemy_unit_iterator->second.getUnit();
										}
									}
								}
								enemy_unit_iterator++;
							}
							if (target_unit &&
								target_unit != unit_iterator->getUnit()->getTarget())
							{
								unit_iterator->getUnit()->attack(target_unit);
							}
							if ((unit_iterator->getUnit()->isIdle() ||
								game_state->getSecondaryScouting() ||
								unit_iterator->getUnit()->getOrder() == BWAPI::Orders::Move))
							{
								auto enemy_unit_iterator = game_state->getEnemyUnits()->begin();
								while (enemy_unit_iterator != game_state->getEnemyUnits()->end())
								{
									if (BWEM::Map::Instance().GetArea(enemy_unit_iterator->second.getDiscoveredPosition()) != nullptr)
									{
										if (enemy_unit_iterator->second.isBuilding() &&
											target_base->getArea()->Id() == BWEM::Map::Instance().GetArea(enemy_unit_iterator->second.getDiscoveredPosition())->Id())
										{
											if (Broodwar->isVisible(enemy_unit_iterator->second.getDiscoveredPosition()))
											{
												BWAPI::Unitset scanned_units = Broodwar->getUnitsInRadius((BWAPI::Position)enemy_unit_iterator->second.getDiscoveredPosition(), 200);
												auto scanned_iterator = scanned_units.begin();
												if (scanned_units.size() == 0)
												{
													auto erase_iterator = enemy_unit_iterator;
													enemy_unit_iterator = game_state->getEnemyUnits()->erase(erase_iterator);
												}
												else
												{
													bool enemy_found = false;
													while (scanned_iterator != scanned_units.end())
													{
														if ((*scanned_iterator)->getPlayer()->isEnemy(Broodwar->self()))
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
														enemy_unit_iterator = game_state->getEnemyUnits()->erase(erase_iterator);
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
												enemy_unit_iterator = game_state->getEnemyUnits()->end();
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
							if (!game_state->getSecondaryScouting())
								game_state->toggleSecondaryScouting();
							if (unit_iterator->getUnit()->isIdle())
							{
								if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition()) != nullptr &&
									game_state->getClosestEmptyStartLocationNotSecondaryScouted() != nullptr)
								{
									if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition())->Id() == game_state->getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Id())
									{
										game_state->getClosestEmptyStartLocationNotSecondaryScouted()->toggleSecondaryScouted();
										if (game_state->getClosestEmptyStartLocationNotSecondaryScouted() != nullptr)
											unit_iterator->getUnit()->attack(game_state->getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Bases().begin()->Center());
										else
										{
											if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition()) != nullptr)
											{
												if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition())->Id() == game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Id())
												{
													game_state->getClosestEmptyBaseNotSecondaryScouted()->toggleSecondaryScouted();
													if (game_state->getClosestEmptyBaseNotSecondaryScouted() != nullptr)
														unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
													else
														game_state->resetSecondaryScouting();
												}
												else
													unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
											}
											else
											{
												if (game_state->getClosestEmptyBaseNotSecondaryScouted() != nullptr)
													unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
												else
													game_state->resetSecondaryScouting();
											}
										}
									}
									else
										unit_iterator->getUnit()->attack(game_state->getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Bases().begin()->Center());
								}
								else
								{
									if (game_state->getClosestEmptyStartLocationNotSecondaryScouted() != nullptr)
										unit_iterator->getUnit()->attack(game_state->getClosestEmptyStartLocationNotSecondaryScouted()->getArea()->Bases().begin()->Center());
									else
									{
										if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition()) != nullptr)
										{
											if (BWEM::Map::Instance().GetArea(unit_iterator->getUnit()->getTilePosition())->Id() == game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Id())
											{
												game_state->getClosestEmptyBaseNotSecondaryScouted()->toggleSecondaryScouted();
												if (game_state->getClosestEmptyBaseNotSecondaryScouted() != nullptr)
													unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
												else
													game_state->resetSecondaryScouting();
											}
											else
												unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
										}
										else
										{
											if (game_state->getClosestEmptyBaseNotSecondaryScouted() != nullptr)
												unit_iterator->getUnit()->attack(game_state->getClosestEmptyBaseNotSecondaryScouted()->getArea()->Bases().begin()->Center());
											else
												game_state->resetSecondaryScouting();
										}
									}
								}
							}
						}
						unit_iterator++;
					}
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
		if (scout_unit.getResourceTarget() != nullptr)
			game_state->unassignWorkerFromMineral(&scout_unit);
		if (scout_target != BWAPI::Positions::Invalid)
		{
			if (scout_unit.getUnit()->getDistance(scout_target) < 200)
			{
				auto base_list_iterator = game_state->getBaseList()->begin();
				while (base_list_iterator != game_state->getBaseList()->end())
				{
					if (base_list_iterator->getArea() == BWEM::Map::Instance().GetArea(scout_unit.getUnit()->getTilePosition()) &&
						base_list_iterator->getScouted() == false)
					{
						base_list_iterator->toggleScouted();
						base_list_iterator = game_state->getBaseList()->end();
					}
					else
					{
						base_list_iterator++;
					}
				}
				scout_target = BWAPI::Positions::Invalid;
				auto base_iterator = game_state->getBaseList()->begin();
				while (base_iterator != game_state->getBaseList()->end())
				{
					if (base_iterator->getArea()->Bases().size() > 0)
					{
						if ((*base_iterator->getArea()->Bases().begin()).Starting() &&
							base_iterator->getScouted() == false)
						{
							scout_target = (BWAPI::Position)(*base_iterator->getArea()->Bases().begin()).Location();
							scout_unit.getUnit()->move(scout_target);
							base_iterator = game_state->getBaseList()->end();
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
			auto base_iterator = game_state->getBaseList()->begin();
			while (base_iterator != game_state->getBaseList()->end())
			{
				if (base_iterator->getArea()->Bases().size() > 0)
				{
					if ((*base_iterator->getArea()->Bases().begin()).Starting() &&
						base_iterator->getBaseClass() == 1)
					{
						scout_target = (BWAPI::Position)(*base_iterator->getArea()->Bases().begin()).Location();
						scout_unit.getUnit()->move(scout_target);
						base_iterator = game_state->getBaseList()->end();
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
			scout_unit.setBaseClass(3);
			game_state->addMineralWorker(scout_unit);
			scout_unit.clearObject();
		}
	}

	if (target_base != nullptr &&
		game_state->getSecondaryScouting())
	{
		game_state->toggleSecondaryScouting();
	}
}

void MilitaryManager::scout(std::unique_ptr<WorkerManager> &worker_manager, std::unique_ptr<GameState> &game_state)
{
	if (scout_unit.getUnit() == nullptr)
	{
		Object new_scout = game_state->getScout();
		if (new_scout.getUnit() != nullptr)
		{
			scout_unit = new_scout;
		}
		auto base_iterator = game_state->getBaseList()->begin();
		while (base_iterator != game_state->getBaseList()->end())
		{
			if (base_iterator->getArea()->Bases().size() > 0)
			{
				if ((*base_iterator->getArea()->Bases().begin()).Starting() &&
					base_iterator->getBaseClass() == 1)
				{
					scout_target = (BWAPI::Position)(*base_iterator->getArea()->Bases().begin()).Location();
					scout_unit.getUnit()->move(scout_target);
					base_iterator = game_state->getBaseList()->end();
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

void MilitaryManager::scout(Object intended_scout, std::unique_ptr<GameState> &game_state)
{
	scout_unit = intended_scout;
	auto base_iterator = game_state->getBaseList()->begin();
	while (base_iterator != game_state->getBaseList()->end())
	{
		if (base_iterator->getArea()->Bases().size() > 0)
		{
			if ((*base_iterator->getArea()->Bases().begin()).Starting() &&
				base_iterator->getBaseClass() == 1)
			{
				scout_target = (BWAPI::Position)(*base_iterator->getArea()->Bases().begin()).Location();
				scout_unit.getUnit()->move(scout_target);
				base_iterator = game_state->getBaseList()->end();
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