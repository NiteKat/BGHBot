#include"MacroManager.h"

MacroManager::MacroManager(BWAPI::Game * game)
{
  Broodwar = game;
}

void MacroManager::checkMacro(std::unique_ptr<WorkerManager> &worker_manager, std::unique_ptr<GameState> &game_state)
{
	auto building_list_iterator = game_state->getBuildingList()->begin();
	while (building_list_iterator != game_state->getBuildingList()->end())
	{
		bool increment = true;
		if (game_state->getBuildOrder() == BuildOrder::TeamMeleeorFFA)
		{
			if (building_list_iterator->getUnit() == nullptr)
			{
				auto erase_iterator = building_list_iterator;
				building_list_iterator = game_state->getBuildingList()->erase(erase_iterator);
				increment = false;
			}
			else if (!building_list_iterator->getUnit()->exists())
			{
				auto used_positions = game_state->getUsedPositions();
				used_positions->erase(building_list_iterator->getUnit()->getTilePosition());
				auto erase_iterator = building_list_iterator;
				building_list_iterator = game_state->getBuildingList()->erase(erase_iterator);
				increment = false;
			}
			else if (!building_list_iterator->getUnit()->isCompleted() &&
				!building_list_iterator->getUnit()->isBeingConstructed() &&
				building_list_iterator->getUnit()->getType().getRace() == BWAPI::Races::Terran)
			{
				worker_manager->getNewBuildWorker(building_list_iterator->getUnit(), game_state);
			}
			else
			{
				if (building_list_iterator->getUnit()->isCompleted() &&
					!building_list_iterator->getUnit()->isFlying() &&
					building_list_iterator->getUnit()->isIdle())
				{
					BWAPI::Race race = building_list_iterator->getUnit()->getType().getRace();
					BWAPI::UnitType unit_to_build;
					if (race == BWAPI::Races::Terran)
						unit_to_build = BWAPI::UnitTypes::Terran_SCV;
					else
						unit_to_build = BWAPI::UnitTypes::Protoss_Probe;
					switch (building_list_iterator->getUnit()->getType())
					{
					case BWAPI::UnitTypes::Terran_Command_Center:
						if (building_list_iterator->getUnit()->canBuildAddon() &&
							Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Terran_Comsat_Station.mineralPrice() &&
							Broodwar->self()->gas() - game_state->getGasCommitted() >= BWAPI::UnitTypes::Terran_Comsat_Station.gasPrice())
						{
							building_list_iterator->getUnit()->buildAddon(BWAPI::UnitTypes::Terran_Comsat_Station);
						}
					case BWAPI::UnitTypes::Protoss_Nexus:
						if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
							game_state->getSupplyUsed(race) < game_state->getSupplyTotal(race))
						{
							int worker_count = 0;
							auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
							while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
							{
								if (mineral_worker_iterator->getBase()->getArea()->Id() == building_list_iterator->getBase()->getArea()->Id())
									worker_count++;
								mineral_worker_iterator++;
							}
							if (worker_count < building_list_iterator->getBase()->getArea()->Minerals().size() * 2)
								building_list_iterator->getUnit()->train(unit_to_build);
						}
						break;
					case BWAPI::UnitTypes::Zerg_Hatchery:
						for (auto larva : building_list_iterator->getUnit()->getLarva())
						{
							if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
								((game_state->getSupplyUsed(BWAPI::Races::Zerg) >= game_state->getSupplyExpected(BWAPI::Races::Zerg) &&
								game_state->getSpawningPool() == 0) ||
								(game_state->getSupplyUsed(BWAPI::Races::Zerg) >= game_state->getSupplyExpected(BWAPI::Races::Zerg) - 4 &&
								game_state->getSpawningPool() >= 1)))
							{
								larva->morph(BWAPI::UnitTypes::Zerg_Overlord);
								continue;
							}
							int worker_count = 0;
							auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
							while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
							{
								if (mineral_worker_iterator->getBase()->getArea()->Id() == building_list_iterator->getBase()->getArea()->Id())
									worker_count++;
								mineral_worker_iterator++;
							}
							if (worker_count < building_list_iterator->getBase()->getArea()->Minerals().size() * 2)
							{
								if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
									game_state->getSupplyUsed(race) < game_state->getSupplyTotal(race))
								{
									if (larva->morph(BWAPI::UnitTypes::Zerg_Drone))
									{
										game_state->addSupplyUsed(1, race);
									}
								}
							}
							else
							{
								double hydralisk = 0;
								double zergling = 0;
								for (auto& objective : *(game_state->getObjectiveList()))
								{
									for (auto& unit : *(objective.getUnits()))
									{
										switch (unit.getType())
										{
										case BWAPI::UnitTypes::Zerg_Zergling:
											zergling++;
											break;
										case BWAPI::UnitTypes::Zerg_Hydralisk:
											hydralisk++;
										}
									}
								}
								if (zergling < 3 &&
									game_state->getSupplyUsed(race) < game_state->getSupplyTotal(race) &&
									Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
									game_state->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Zergling))
								{
									if (larva->morph(BWAPI::UnitTypes::Zerg_Zergling))
										game_state->addSupplyUsed(1, race);
								}
								else if (hydralisk == 0 &&
									game_state->getSupplyUsed(race) < game_state->getSupplyTotal(race) - 2 &&
									Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Zerg_Hydralisk.mineralPrice() &&
									Broodwar->self()->gas() - game_state->getGasCommitted() >= BWAPI::UnitTypes::Zerg_Hydralisk.gasPrice() &&
									game_state->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Hydralisk))
								{
									if (larva->morph(BWAPI::UnitTypes::Zerg_Hydralisk))
										game_state->addSupplyUsed(2, race);
								}
								else if (hydralisk != 0)
								{
									if (zergling / hydralisk > 3 &&
										game_state->getSupplyUsed(race) < game_state->getSupplyTotal(race) - 2 &&
										Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Zerg_Hydralisk.mineralPrice() &&
										Broodwar->self()->gas() - game_state->getGasCommitted() >= BWAPI::UnitTypes::Zerg_Hydralisk.gasPrice() &&
										game_state->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Hydralisk))
									{
										if (larva->morph(BWAPI::UnitTypes::Zerg_Hydralisk))
											game_state->addSupplyUsed(2, race);
									}
									else if (game_state->getSupplyUsed(race) < game_state->getSupplyTotal(race) &&
										Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
										game_state->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Zergling))
									{
										if (larva->morph(BWAPI::UnitTypes::Zerg_Zergling))
											game_state->addSupplyUsed(1, race);
									}
								}
								else
								{
									if (game_state->getSupplyUsed(race) < game_state->getSupplyTotal(race) &&
										Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
										game_state->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Zergling))
									{
										if (larva->morph(BWAPI::UnitTypes::Zerg_Zergling))
											game_state->addSupplyUsed(1, race);
									}
								}
							}	
						}
						break;
					case BWAPI::UnitTypes::Terran_Barracks:
						if (!game_state->hasUnitTypeRequirement(BWAPI::UnitTypes::Terran_Medic))
						{
							if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
								game_state->getSupplyUsed(race) < game_state->getSupplyTotal(race) &&
								!game_state->checkAcademy())
							{
								building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine);
							}
						}
						else
						{
							if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50)
							{
								double marines = game_state->getUnitTypeCount(BWAPI::UnitTypes::Terran_Marine);
								double medics = game_state->getUnitTypeCount(BWAPI::UnitTypes::Terran_Medic);
								if (medics > 0)
								{
									double ratio = marines / medics;
									if (Broodwar->self()->gas() - game_state->getGasCommitted() >= 25 &&
										ratio > 5)
									{
										if (!building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Medic))
											building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine);
									}
									else
										building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine);
								}
								else if (marines >= 5 &&
										Broodwar->self()->gas() - game_state->getGasCommitted() >= 25)
									building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Medic);
								else
									building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine);
							}
						}
						break;
					case BWAPI::UnitTypes::Protoss_Gateway:
						if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
							game_state->getSupplyUsed(race) < game_state->getSupplyTotal(race))
						{
							building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Zealot);
						}
						break;
					case BWAPI::UnitTypes::Terran_Refinery:
					case BWAPI::UnitTypes::Protoss_Assimilator:
					case BWAPI::UnitTypes::Zerg_Extractor:
						if (building_list_iterator->getUnit()->isCompleted() &&
							building_list_iterator->getNumberGasWorkers() < 3)
						{
							auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
							while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
							{
								if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
								{
									building_list_iterator->addGasWorker(static_cast<int>(mineral_worker_iterator->getUnit()->getID()));
									if (mineral_worker_iterator->getResourceTarget() != nullptr)
										game_state->unassignWorkerFromMineral(&(*mineral_worker_iterator));
									mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
									game_state->getMineralWorkers()->erase(mineral_worker_iterator);
									mineral_worker_iterator = game_state->getMineralWorkers()->end();
								}
								else
								{
									mineral_worker_iterator++;
								}
							}
						}
						break;
					case BWAPI::UnitTypes::Terran_Academy:
						if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::TechTypes::Stim_Packs.mineralPrice() &&
							Broodwar->self()->gas() - game_state->getGasCommitted() >= BWAPI::TechTypes::Stim_Packs.gasPrice() &&
							!Broodwar->self()->hasResearched(BWAPI::TechTypes::Stim_Packs))
						{
							building_list_iterator->getUnit()->research(BWAPI::TechTypes::Stim_Packs);
						}
						else if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UpgradeTypes::U_238_Shells.mineralPrice() &&
							Broodwar->self()->gas() - game_state->getGasCommitted() >= BWAPI::UpgradeTypes::U_238_Shells.gasPrice() &&
							Broodwar->self()->hasResearched(BWAPI::TechTypes::Stim_Packs) &&
							Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::U_238_Shells) < 1)
						{
							building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::U_238_Shells);
						}
						break;
					case BWAPI::UnitTypes::Zerg_Spawning_Pool:
						if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UpgradeTypes::Metabolic_Boost.mineralPrice() &&
							Broodwar->self()->gas() - game_state->getGasCommitted() >= BWAPI::UpgradeTypes::Metabolic_Boost.gasPrice() &&
							Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Metabolic_Boost) == 0)
						{
							building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Metabolic_Boost);
						}
						break;
					case BWAPI::UnitTypes::Zerg_Hydralisk_Den:
						if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UpgradeTypes::Muscular_Augments.mineralPrice() &&
							Broodwar->self()->gas() - game_state->getGasCommitted() >= BWAPI::UpgradeTypes::Muscular_Augments.gasPrice() &&
							Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Muscular_Augments) == 0)
						{
							building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Muscular_Augments);
						}
						else if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UpgradeTypes::Grooved_Spines.mineralPrice() &&
							Broodwar->self()->gas() - game_state->getGasCommitted() >= BWAPI::UpgradeTypes::Grooved_Spines.gasPrice() &&
							Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Grooved_Spines) == 0)
						{
							building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Grooved_Spines);
						}
					}
				}
			}
			if (increment)
				building_list_iterator++;
		}
		else if (Broodwar->self()->getRace() == BWAPI::Races::Terran)
		{
			if (building_list_iterator->getUnit() == nullptr)
			{
				auto erase_iterator = building_list_iterator;
				building_list_iterator = game_state->getBuildingList()->erase(erase_iterator);
			}
			else if (!building_list_iterator->getUnit()->exists())
			{
				auto used_positions = game_state->getUsedPositions();
				used_positions->erase(building_list_iterator->getUnit()->getTilePosition());
				auto erase_iterator = building_list_iterator;
				building_list_iterator = game_state->getBuildingList()->erase(erase_iterator);
			}
			else if (!building_list_iterator->getUnit()->isCompleted() &&
				!building_list_iterator->getUnit()->isBeingConstructed())
			{
				worker_manager->getNewBuildWorker(building_list_iterator->getUnit(), game_state);
				building_list_iterator++;
			}
			else  if (game_state->getBuildOrder() == BuildOrder::BGHMech)
			{ 
				if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Command_Center &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getUnit()->canBuildAddon(BWAPI::UnitTypes::Terran_Comsat_Station) &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 50)
				{
					building_list_iterator->getUnit()->buildAddon(BWAPI::UnitTypes::Terran_Comsat_Station);
					game_state->addMineralsCommitted(50);
					game_state->addGasCommitted(50);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Command_Center &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getUnit()->canBuildAddon(BWAPI::UnitTypes::Terran_Comsat_Station) &&
					!building_list_iterator->getUnit()->isTraining() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 50)
				{
					building_list_iterator->getUnit()->buildAddon(BWAPI::UnitTypes::Terran_Comsat_Station);
					game_state->addMineralsCommitted(50);
					game_state->addGasCommitted(50);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Command_Center &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					game_state->getMineralWorkerCount() < 25 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal())
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_SCV);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks &&
					!building_list_iterator->getUnit()->isFlying() &&
					building_list_iterator->getUnit()->isCompleted() &&
					Broodwar->elapsedTime() < 300)
				{
					auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
					{
						if (game_state->getContainingBase(mineral_worker_iterator->getUnit())->getBaseClass() != 3 &&
							game_state->getContainingBase(mineral_worker_iterator->getUnit())->getBaseClass() != 4)
						{
							building_list_iterator->getUnit()->lift();
							mineral_worker_iterator = game_state->getMineralWorkers()->end();
						}
						else
							mineral_worker_iterator++;
					}
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks &&
					building_list_iterator->getUnit()->isFlying() &&
					Broodwar->elapsedTime() < 300)
				{
					bool workers_in_base = true;
					auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
					{
						if (game_state->getContainingBase(mineral_worker_iterator->getUnit())->getBaseClass() != 3)
						{
							workers_in_base = false;
							mineral_worker_iterator = game_state->getMineralWorkers()->end();
						}
						else
							mineral_worker_iterator++;
					}
					if (workers_in_base)
					{
						BWAPI::TilePosition position_to_land;
						if (Broodwar->self()->getStartLocation().x == 114 &&
							Broodwar->self()->getStartLocation().y == 116)
						{
							position_to_land.x = 90;
							position_to_land.y = 97;
						}
						else if (Broodwar->self()->getStartLocation().x == 114 &&
							Broodwar->self()->getStartLocation().y == 80)
						{
							position_to_land.x = 101;
							position_to_land.y = 63;
						}
						else if (Broodwar->self()->getStartLocation().x == 113 &&
							Broodwar->self()->getStartLocation().y == 8)
						{
							position_to_land.x = 96;
							position_to_land.y = 24;
						}
						else if (Broodwar->self()->getStartLocation().x == 72 &&
							Broodwar->self()->getStartLocation().y == 8)
						{
							position_to_land.x = 54;
							position_to_land.y = 25;
						}
						else if (Broodwar->self()->getStartLocation().x == 10 &&
							Broodwar->self()->getStartLocation().y == 6)
						{
							position_to_land.x = 30;
							position_to_land.y = 23;
						}
						else if (Broodwar->self()->getStartLocation().x == 8 &&
							Broodwar->self()->getStartLocation().y == 47)
						{
							position_to_land.x = 22;
							position_to_land.y = 57;
						}
						else if (Broodwar->self()->getStartLocation().x == 10 &&
							Broodwar->self()->getStartLocation().y == 114)
						{
							position_to_land.x = 17;
							position_to_land.y = 95;
						}
						else if (Broodwar->self()->getStartLocation().x == 63 &&
							Broodwar->self()->getStartLocation().y == 117)
						{
							position_to_land.x = 52;
							position_to_land.y = 96;
						}
						building_list_iterator->getUnit()->land(position_to_land);
						building_list_iterator++;
					}
					else
						building_list_iterator++;
				}
				/*else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					game_state->getUnitTypeCount(BWAPI::UnitTypes::Terran_Marine) < 4 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal())
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine);
					building_list_iterator++;
				}*/
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Factory &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					building_list_iterator->getUnit()->canBuildAddon() &&
					game_state->getBuildingTypeCount(BWAPI::UnitTypes::Terran_Machine_Shop) < 1 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 50)
				{
					building_list_iterator->getUnit()->buildAddon(BWAPI::UnitTypes::Terran_Machine_Shop);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Factory &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode, 0) &&
					building_list_iterator->getUnit()->getAddon() != nullptr &&
					game_state->getBuildTanks() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 100 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal() - 2)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Factory &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Terran_Goliath, 0) &&
					building_list_iterator->getUnit()->getAddon() == nullptr &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 50 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal() - 2)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Goliath);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Machine_Shop &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Charon_Boosters) < 1 &&
					Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Terran_Goliath) &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
				{
					building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Charon_Boosters);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Machine_Shop &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					!Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode) &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 150)
				{
					building_list_iterator->getUnit()->research(BWAPI::TechTypes::Tank_Siege_Mode);
					game_state->toggleBuildTanks();
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Refinery &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getNumberGasWorkers() < 3)
				{
					auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
					{
						if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
						{
							building_list_iterator->addGasWorker(static_cast<int>(mineral_worker_iterator->getUnit()->getID()));
							game_state->unassignWorkerFromMineral(&(*mineral_worker_iterator));
							mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
							game_state->getMineralWorkers()->erase(mineral_worker_iterator);
							mineral_worker_iterator = game_state->getMineralWorkers()->end();
						}
						else
						{
							mineral_worker_iterator++;
						}
					}
					building_list_iterator++;
				}
				else
				{
					building_list_iterator++;
				}
			}
			else if (game_state->getBuildOrder() == BuildOrder::TAyumiBuildOpening)
			{
				if (building_list_iterator->getUnit()->isCompleted() &&
					!building_list_iterator->getUnit()->isFlying() &&
					building_list_iterator->getUnit()->isIdle())
				{
					switch (building_list_iterator->getType())
					{
					case BWAPI::UnitTypes::Enum::Terran_Command_Center:
						if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
							game_state->getSupplyUsed() < game_state->getSupplyTotal())
						{
							if (game_state->getSupplyBuilt() < 9)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_SCV))
									game_state->addSupplyBuilt(1);
							}
							else if (game_state->getSupplyBuilt() < 10 &&
								game_state->getSupplyExpected() > 10)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_SCV))
									game_state->addSupplyBuilt(1);
							}
							else if (game_state->getSupplyBuilt() < 14 &&
								game_state->getSupplyExpected() == 18)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_SCV))
									game_state->addSupplyBuilt(1);
							}
							else if (game_state->getSupplyBuilt() < 20 &&
								game_state->getSupplyExpected() == 26)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_SCV))
									game_state->addSupplyBuilt(1);
							}
							else if (game_state->getSupplyBuilt() < 23 &&
								game_state->getSupplyExpected() == 36)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_SCV))
									game_state->addSupplyBuilt(1);
							}
							else if (game_state->getSupplyBuilt() < 29 &&
								game_state->getSupplyExpected() == 44)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_SCV))
									game_state->addSupplyBuilt(1);
							}
							else if (game_state->getSupplyBuilt() < 35 &&
								game_state->getBarracks() == 4)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_SCV))
									game_state->addSupplyBuilt(1);
							}
							else if (game_state->getSupplyBuilt() < 37 &&
								game_state->getGas() == 1)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_SCV))
									game_state->addSupplyBuilt(1);
							}
						}
						else if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
							Broodwar->self()->gas() - game_state->getGasCommitted() >= 50 &&
							game_state->getSupplyBuilt() >= 56 &&
							building_list_iterator->getUnit()->canBuildAddon())
							building_list_iterator->getUnit()->buildAddon(BWAPI::UnitTypes::Terran_Comsat_Station);
						break;
					case BWAPI::UnitTypes::Enum::Terran_Barracks:
						if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
							game_state->getSupplyUsed() < game_state->getSupplyTotal())
						{
							if (game_state->getSupplyBuilt() < 20 &&
								game_state->getSupplyExpected() == 26)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine))
									game_state->addSupplyBuilt(1);
							}
							else if (game_state->getSupplyBuilt() < 23 &&
								game_state->getSupplyExpected() == 36)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine))
									game_state->addSupplyBuilt(1);
							}
							else if (game_state->getSupplyBuilt() < 29 &&
								game_state->getSupplyExpected() == 44)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine))
									game_state->addSupplyBuilt(1);
							}
							else if (game_state->getSupplyBuilt() < 35 &&
								game_state->getBarracks() == 4)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine))
									game_state->addSupplyBuilt(1);
							}
							else if (game_state->getSupplyBuilt() < 35 &&
								game_state->getBarracks() == 4)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine))
									game_state->addSupplyBuilt(1);
							}
							else if (game_state->getSupplyBuilt() < 37 &&
								game_state->getGas() == 1)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine))
									game_state->addSupplyBuilt(1);
							}
							else if (game_state->getSupplyBuilt() >= 37 &&
								game_state->getSupplyExpected() >= 52 &&
								game_state->getSupplyBuilt() < 52)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine))
									game_state->addSupplyBuilt(1);
							}
							else if (game_state->getSupplyBuilt() >= 52 &&
								game_state->getSupplyBuilt() < 56)
							{
								if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Medic))
									game_state->addSupplyBuilt(1);
							}
						}
						break;
					case BWAPI::UnitTypes::Enum::Terran_Academy:
						if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
							!Broodwar->self()->hasResearched(BWAPI::TechTypes::Stim_Packs) &&
							game_state->getSupplyBuilt() >= 52)
							building_list_iterator->getUnit()->research(BWAPI::TechTypes::Stim_Packs);
						break;
					case BWAPI::UnitTypes::Enum::Terran_Refinery:
						if (building_list_iterator->getUnit()->isCompleted() &&
							building_list_iterator->getNumberGasWorkers() < 3)
						{
							auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
							while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
							{
								if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
								{
									building_list_iterator->addGasWorker(static_cast<int>(mineral_worker_iterator->getUnit()->getID()));
									if (mineral_worker_iterator->getResourceTarget() != nullptr)
										game_state->unassignWorkerFromMineral(&(*mineral_worker_iterator));
									mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
									game_state->getMineralWorkers()->erase(mineral_worker_iterator);
									mineral_worker_iterator = game_state->getMineralWorkers()->end();
								}
								else
								{
									mineral_worker_iterator++;
								}
							}
						}
						break;
					}
				}
				building_list_iterator++;
			}
			else
			{
				
				if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Command_Center &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getUnit()->canBuildAddon(BWAPI::UnitTypes::Terran_Comsat_Station) &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 50)
				{
					building_list_iterator->getUnit()->buildAddon(BWAPI::UnitTypes::Terran_Comsat_Station);
					game_state->addMineralsCommitted(50);
					game_state->addGasCommitted(50);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Command_Center &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal())
				{
					int worker_count = 0;
					auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
					{
						if (mineral_worker_iterator->getBase()->getArea()->Id() == building_list_iterator->getBase()->getArea()->Id())
							worker_count++;
						mineral_worker_iterator++;
					}
					if (worker_count < building_list_iterator->getBase()->getArea()->Minerals().size() * 2)
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_SCV);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks &&
					(!Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Terran_Medic, 0) ||
					Broodwar->self()->gas() - game_state->getGasCommitted() < 25) &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal())
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks &&
					Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Terran_Medic, 0) &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 25 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal())
				{
					int marines = game_state->getUnitTypeCount(BWAPI::UnitTypes::Terran_Marine);
					int medics = game_state->getUnitTypeCount(BWAPI::UnitTypes::Terran_Medic);
					if (medics == 0)
					{
						if (marines >= 5)
						{
							if (!building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Medic))
								building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine);
						}
						else
						{
							building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine);
						}
					}
					else if (marines / medics >= 5)
					{
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Medic);
					}
					else
					{
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine);
					}
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Academy &&
					Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::U_238_Shells) == 0 &&
					!building_list_iterator->getUnit()->isUpgrading() &&
					!building_list_iterator->getUnit()->isResearching() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 150)
				{
					building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::U_238_Shells);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Academy &&
					!building_list_iterator->getUnit()->isUpgrading() &&
					!building_list_iterator->getUnit()->isResearching() &&
					!Broodwar->self()->hasResearched(BWAPI::TechTypes::Stim_Packs) &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
				{
					building_list_iterator->getUnit()->research(BWAPI::TechTypes::Stim_Packs);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Refinery &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getNumberGasWorkers() < 3)
				{
					auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
					{
						if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
						{
							building_list_iterator->addGasWorker(static_cast<int>(mineral_worker_iterator->getUnit()->getID()));
							if (mineral_worker_iterator->getResourceTarget() != nullptr)
								game_state->unassignWorkerFromMineral(&(*mineral_worker_iterator));
							mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
							game_state->getMineralWorkers()->erase(mineral_worker_iterator);
							mineral_worker_iterator = game_state->getMineralWorkers()->end();
						}
						else
						{
							mineral_worker_iterator++;
						}
					}
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Comsat_Station &&
					building_list_iterator->getUnit()->getEnergy() > 190 &&
					Broodwar->elapsedTime() - game_state->getLastScan() > 30)
				{
					AIBase* base_to_scan = game_state->getFarthestEmptyBaseNotSecondaryScouted();
					if (base_to_scan != nullptr)
					{
						building_list_iterator->getUnit()->useTech(BWAPI::TechTypes::Scanner_Sweep, (BWAPI::Position)base_to_scan->getArea()->Top());
						base_to_scan->toggleSecondaryScouted();
						game_state->setLastScan(Broodwar->elapsedTime());
					}
					else
					{
						game_state->resetSecondaryScouting();
					}
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Factory &&
					building_list_iterator->getUnit()->getAddon() == nullptr &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Terran_Machine_Shop.mineralPrice() &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= BWAPI::UnitTypes::Terran_Machine_Shop.gasPrice())
				{
					building_list_iterator->getUnit()->buildAddon(BWAPI::UnitTypes::Terran_Machine_Shop);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Machine_Shop &&
					!building_list_iterator->getUnit()->isResearching() &&
					!Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode) &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Terran_Machine_Shop.mineralPrice() &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= BWAPI::UnitTypes::Terran_Machine_Shop.gasPrice())
				{
					if (building_list_iterator->getUnit()->research(BWAPI::TechTypes::Tank_Siege_Mode))
						game_state->toggleBuildTanks();
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Factory &&
					!building_list_iterator->getUnit()->isTraining() &&
					building_list_iterator->getUnit()->isIdle() &&
					game_state->getUnitTypeCount(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) + game_state->getUnitTypeCount(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode) < 21 &&
					game_state->getBuildTanks() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode.mineralPrice() &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode.gasPrice())
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Bunker)
				{
					if (building_list_iterator->getUnit()->getLoadedUnits().size() != 4)
						game_state->loadBunker(&(*building_list_iterator));
					double enemy_strength = game_state->getEnemyLocalStrength((*building_list_iterator));
					if (enemy_strength > 0)
					{
						int scvs_needed = (int)ceil(enemy_strength / 0.7);
						int scvs_assigned = game_state->getAssignedRepairWorkers((*building_list_iterator));
						if (scvs_assigned < scvs_needed)
							game_state->assignRepairWorkers(&(*building_list_iterator), scvs_needed - scvs_assigned);
						else if (scvs_assigned > scvs_needed)
							game_state->removeRepairWorkers(&(*building_list_iterator), scvs_assigned - scvs_needed);
					}
					else
					{
						if (building_list_iterator->getUnit()->getHitPoints() == building_list_iterator->getUnit()->getType().maxHitPoints())
							game_state->removeRepairWorkers(&(*building_list_iterator), game_state->getAssignedRepairWorkers(*building_list_iterator));
					}
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Engineering_Bay)
				{
					int upgrade_level_weapon = Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Terran_Infantry_Weapons);
					int upgrade_level_armor = Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Terran_Infantry_Armor);
					int minerals_available = Broodwar->self()->minerals() - game_state->getMineralsCommitted();
					int gas_available = Broodwar->self()->gas() - game_state->getGasCommitted();
					if (upgrade_level_weapon == 0 &&
						!building_list_iterator->getUnit()->isUpgrading() &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Terran_Infantry_Weapons) &&
						minerals_available >= BWAPI::UpgradeTypes::Terran_Infantry_Weapons.mineralPrice() + BWAPI::UpgradeTypes::Terran_Infantry_Weapons.mineralPriceFactor() * upgrade_level_weapon &&
						gas_available >= BWAPI::UpgradeTypes::Terran_Infantry_Weapons.gasPrice() + BWAPI::UpgradeTypes::Terran_Infantry_Weapons.gasPriceFactor() * upgrade_level_weapon)
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Terran_Infantry_Weapons);
					else if (upgrade_level_armor == 0 &&
						!building_list_iterator->getUnit()->isUpgrading() &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Terran_Infantry_Armor) &&
						minerals_available >= BWAPI::UpgradeTypes::Terran_Infantry_Armor.mineralPrice() + BWAPI::UpgradeTypes::Terran_Infantry_Armor.mineralPriceFactor() * upgrade_level_armor &&
						gas_available >= BWAPI::UpgradeTypes::Terran_Infantry_Armor.gasPrice() + BWAPI::UpgradeTypes::Terran_Infantry_Armor.gasPriceFactor() * upgrade_level_armor)
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Terran_Infantry_Armor);
					else if (upgrade_level_weapon <= upgrade_level_armor &&
						!building_list_iterator->getUnit()->isUpgrading() &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Terran_Infantry_Weapons) && 
						game_state->getScienceFacility() >= 1 &&
						minerals_available >= BWAPI::UpgradeTypes::Terran_Infantry_Armor.mineralPrice() + BWAPI::UpgradeTypes::Terran_Infantry_Armor.mineralPriceFactor() * upgrade_level_armor &&
						gas_available >= BWAPI::UpgradeTypes::Terran_Infantry_Armor.gasPrice() + BWAPI::UpgradeTypes::Terran_Infantry_Armor.gasPriceFactor() * upgrade_level_armor)
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Terran_Infantry_Weapons);
					else if (upgrade_level_armor == 0 &&
						!building_list_iterator->getUnit()->isUpgrading() &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Terran_Infantry_Armor) &&
						game_state->getScienceFacility() >= 1 &&
						minerals_available >= BWAPI::UpgradeTypes::Terran_Infantry_Armor.mineralPrice() + BWAPI::UpgradeTypes::Terran_Infantry_Armor.mineralPriceFactor() * upgrade_level_armor &&
						gas_available >= BWAPI::UpgradeTypes::Terran_Infantry_Armor.gasPrice() + BWAPI::UpgradeTypes::Terran_Infantry_Armor.gasPriceFactor() * upgrade_level_armor)
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Terran_Infantry_Armor);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Armory)
				{
					int upgrade_level_weapon = Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Terran_Vehicle_Weapons);
					int upgrade_level_armor = Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Terran_Vehicle_Plating);
					int minerals_available = Broodwar->self()->minerals() - game_state->getMineralsCommitted();
					int gas_available = Broodwar->self()->gas() - game_state->getGasCommitted();
					if (upgrade_level_weapon == 0 &&
						!building_list_iterator->getUnit()->isUpgrading() &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Terran_Vehicle_Weapons) &&
						minerals_available >= BWAPI::UpgradeTypes::Terran_Vehicle_Weapons.mineralPrice() + BWAPI::UpgradeTypes::Terran_Vehicle_Weapons.mineralPriceFactor() * upgrade_level_weapon &&
						gas_available >= BWAPI::UpgradeTypes::Terran_Vehicle_Weapons.gasPrice() + BWAPI::UpgradeTypes::Terran_Vehicle_Weapons.gasPriceFactor() * upgrade_level_weapon)
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Terran_Vehicle_Weapons);
					else if (upgrade_level_armor == 0 &&
						!building_list_iterator->getUnit()->isUpgrading() &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Terran_Vehicle_Plating) &&
						minerals_available >= BWAPI::UpgradeTypes::Terran_Vehicle_Plating.mineralPrice() + BWAPI::UpgradeTypes::Terran_Vehicle_Plating.mineralPriceFactor() * upgrade_level_armor &&
						gas_available >= BWAPI::UpgradeTypes::Terran_Vehicle_Plating.gasPrice() + BWAPI::UpgradeTypes::Terran_Vehicle_Plating.gasPriceFactor() * upgrade_level_armor)
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Terran_Vehicle_Plating);
					else if (upgrade_level_weapon <= upgrade_level_armor &&
						!building_list_iterator->getUnit()->isUpgrading() &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Terran_Vehicle_Weapons) &&
						game_state->getScienceFacility() >= 1 &&
						minerals_available >= BWAPI::UpgradeTypes::Terran_Vehicle_Plating.mineralPrice() + BWAPI::UpgradeTypes::Terran_Vehicle_Plating.mineralPriceFactor() * upgrade_level_armor &&
						gas_available >= BWAPI::UpgradeTypes::Terran_Vehicle_Plating.gasPrice() + BWAPI::UpgradeTypes::Terran_Vehicle_Plating.gasPriceFactor() * upgrade_level_armor)
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Terran_Vehicle_Weapons);
					else if (upgrade_level_armor == 0 &&
						!building_list_iterator->getUnit()->isUpgrading() &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Terran_Vehicle_Plating) &&
						game_state->getScienceFacility() >= 1 &&
						minerals_available >= BWAPI::UpgradeTypes::Terran_Vehicle_Plating.mineralPrice() + BWAPI::UpgradeTypes::Terran_Vehicle_Plating.mineralPriceFactor() * upgrade_level_armor &&
						gas_available >= BWAPI::UpgradeTypes::Terran_Vehicle_Plating.gasPrice() + BWAPI::UpgradeTypes::Terran_Vehicle_Plating.gasPriceFactor() * upgrade_level_armor)
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Terran_Vehicle_Plating);
					building_list_iterator++;
				}
				else
				{
					building_list_iterator++;
				}
			}
			
		}
		else if (Broodwar->self()->getRace() == BWAPI::Races::Protoss)
		{
			if (game_state->getBuildOrder() == BuildOrder::P2Gate1)
			{
				if (building_list_iterator->getUnit() == nullptr)
				{
					auto erase_iterator = building_list_iterator;
					building_list_iterator = game_state->getBuildingList()->erase(erase_iterator);
				}
				else if (!building_list_iterator->getUnit()->exists())
				{
					auto erase_iterator = building_list_iterator;
					building_list_iterator = game_state->getBuildingList()->erase(erase_iterator);
				}
				else if ((building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Nexus &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal() &&
					building_list_iterator->getUnit()->isIdle()) &&
					(game_state->getSupplyUsed() < 8 ||
					(game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Pylon) == 1 &&
					game_state->getSupplyUsed() < 9) ||
					(game_state->getBarracks() == 1 &&
					game_state->getSupplyUsed() < 10) ||
					(game_state->getBarracks() == 2 &&
					game_state->getMineralWorkerCount() < 15)))
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Probe);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Gateway &&
building_list_iterator->getUnit()->isIdle() &&
Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
game_state->getSupplyUsed() < game_state->getSupplyTotal() - 2)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Zealot);
					building_list_iterator++;
				}
				else
					building_list_iterator++;
			}
			else if (game_state->getBuildOrder() == BuildOrder::P4GateGoonOpening)
			{
				if ((building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Nexus &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal() &&
					building_list_iterator->getUnit()->isIdle()) &&
					(game_state->getSupplyBuilt() < 8 ||
					(game_state->getPylon() == 1 &&
					game_state->getSupplyBuilt() < 10) ||
					(game_state->getBarracks() == 1 &&
					game_state->getSupplyBuilt() < 12) ||
					(game_state->getPylon() == 2 &&
					game_state->getSupplyBuilt() < 16) ||
					(game_state->getGas() == 1 &&
					game_state->getSupplyBuilt() < 17) ||
					(game_state->checkCyberCore() &&
					game_state->getSupplyBuilt() < 27)))
				{
					if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Probe))
						game_state->addSupplyBuilt(1);
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Gateway &&
					building_list_iterator->getUnit()->isIdle())
				{
					if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Dragoon.mineralPrice() &&
						Broodwar->self()->gas() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Dragoon.gasPrice() &&
						game_state->getSupplyUsed() <= game_state->getSupplyTotal() - 2 &&
						(game_state->getSupplyBuilt() < 31 ||
						game_state->getBarracks() == 4))
					{
						if(building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Dragoon))
							game_state->addSupplyBuilt(2);
					}
					else if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Zealot.mineralPrice() &&
						(game_state->getUnitTypeCount(BWAPI::UnitTypes::Protoss_Zealot) < 1 ||
						(game_state->getUnitTypeCount(BWAPI::UnitTypes::Protoss_Zealot) < 2 &&
						game_state->checkCyberCore())) &&
						game_state->getSupplyUsed() <= game_state->getSupplyTotal() - 2)
					{
						if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Zealot))
							game_state->addSupplyBuilt(2);
					}
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core &&
					building_list_iterator->getUnit()->isIdle())
				{
					if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UpgradeTypes::Singularity_Charge.mineralPrice() &&
						Broodwar->self()->gas() - game_state->getMineralsCommitted() >= BWAPI::UpgradeTypes::Singularity_Charge.gasPrice() &&
						game_state->getSupplyBuilt() >= 26)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Singularity_Charge);
					}
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Assimilator &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getNumberGasWorkers() < 3)
				{
					auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
					{
						if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
						{
							building_list_iterator->addGasWorker(static_cast<int>(mineral_worker_iterator->getUnit()->getID()));
							game_state->unassignWorkerFromMineral(&(*mineral_worker_iterator));
							mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
							game_state->getMineralWorkers()->erase(mineral_worker_iterator);
							mineral_worker_iterator = game_state->getMineralWorkers()->end();
						}
						else
						{
							mineral_worker_iterator++;
						}
					}
				}
				building_list_iterator++;
			}
			else if (game_state->getBuildOrder() == BuildOrder::P4GateGoonMid)
			{
				if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Gateway &&
					building_list_iterator->getUnit()->isIdle())
				{
					if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Dragoon.mineralPrice() &&
						Broodwar->self()->gas() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Dragoon.gasPrice() &&
						game_state->getSupplyUsed() <= game_state->getSupplyTotal() - 2 &&
						game_state->getSupplyUsed() < 40)
					{
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Dragoon);
					}
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Assimilator &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getNumberGasWorkers() < 3)
				{
					auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
					{
						if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
						{
							building_list_iterator->addGasWorker(static_cast<int>(mineral_worker_iterator->getUnit()->getID()));
							mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
							game_state->getMineralWorkers()->erase(mineral_worker_iterator);
							mineral_worker_iterator = game_state->getMineralWorkers()->end();
						}
						else
						{
							mineral_worker_iterator++;
						}
					}
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Forge &&
					building_list_iterator->getUnit()->isIdle())
				{
					if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) == 0 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Weapons);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Armor) == 0 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Armor) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Armor);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) == 0 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 200 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 200)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Plasma_Shields);
					}
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Nexus &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal() &&
					game_state->getMineralWorkerCount() < 80)
				{
					int worker_count = 0;
					auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
					{
						if (mineral_worker_iterator->getBase()->getArea()->Id() == building_list_iterator->getBase()->getArea()->Id())
							worker_count++;
						mineral_worker_iterator++;
					}
					if (worker_count < building_list_iterator->getBase()->getArea()->Minerals().size() * 2)
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Probe);
				}
				building_list_iterator++;
			}
			else if (game_state->getBuildOrder() == BuildOrder::P4GateGoonLate)
			{
				if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Gateway &&
					building_list_iterator->getUnit()->isIdle())
				{
					if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Dragoon.mineralPrice() &&
						Broodwar->self()->gas() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Dragoon.gasPrice() &&
						game_state->getUnitTypeCount(BWAPI::UnitTypes::Protoss_Dragoon) < 3 * game_state->getUnitTypeCount(BWAPI::UnitTypes::Protoss_Zealot) &&
						game_state->getSupplyUsed() <= game_state->getSupplyTotal() - 2)
					{
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Dragoon);
					}
					else if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Zealot.mineralPrice() &&
						game_state->getSupplyUsed() <= game_state->getSupplyTotal() - 2)
					{
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Zealot);
					}
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Forge &&
					building_list_iterator->getUnit()->isIdle())
				{
					if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) == 0 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Weapons);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Armor) == 0 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Armor) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Armor);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) == 0 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 200 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 200)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Plasma_Shields);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) == 1 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) &&
						game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 150)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Weapons);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Armor) == 1 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Armor) &&
						game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 175 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 175)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Armor);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) == 1 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 300 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 300)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Plasma_Shields);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) == 2 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) &&
						game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 200 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 200)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Weapons);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Armor) == 2 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Armor) &&
						game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 250 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 250)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Armor);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) == 2 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 400 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 400)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Plasma_Shields);
					}
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun &&
					building_list_iterator->getUnit()->isIdle() &&
					Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Leg_Enhancements) == 0 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 150)
				{
					building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Leg_Enhancements);
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Robotics_Facility &&
					Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Observer, 0) &&
					!building_list_iterator->getUnit()->isTraining() &&
					game_state->getDetectorCount() < 5 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 25 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 75)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Observer);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Assimilator &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getNumberGasWorkers() < 3)
				{
					auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
					{
						if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
						{
							building_list_iterator->addGasWorker(static_cast<int>(mineral_worker_iterator->getUnit()->getID()));
							mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
							game_state->getMineralWorkers()->erase(mineral_worker_iterator);
							mineral_worker_iterator = game_state->getMineralWorkers()->end();
						}
						else
						{
							mineral_worker_iterator++;
						}
					}
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Nexus &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal() &&
					game_state->getMineralWorkerCount() < 80)
				{
					int worker_count = 0;
					auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
					{
						if (mineral_worker_iterator->getBase()->getArea()->Id() == building_list_iterator->getBase()->getArea()->Id())
							worker_count++;
						mineral_worker_iterator++;
					}
					if (worker_count < building_list_iterator->getBase()->getArea()->Minerals().size() * 2)
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Probe);
				}
				building_list_iterator++;
			}
			else if (game_state->getBuildOrder() == BuildOrder::PForgeFastExpand9poolOpening)
			{
				if ((building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Nexus &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50) &&
					(game_state->getSupplyBuilt() < 8 ||
					(game_state->getSupplyBuilt() < 10 &&
					game_state->getForge() == 0) ||
					(game_state->getSupplyBuilt() < 12 &&
					game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) == 1 &&
					game_state->getForge() == 1)))
				{
					if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Probe))
						game_state->addSupplyBuilt(1);

				}
				building_list_iterator++;
			}
			else
			{
				if (building_list_iterator->getUnit() == nullptr)
				{
					auto erase_iterator = building_list_iterator;
					building_list_iterator = game_state->getBuildingList()->erase(erase_iterator);
				}
				else if (!building_list_iterator->getUnit()->exists())
				{
					auto erase_iterator = building_list_iterator;
					building_list_iterator = game_state->getBuildingList()->erase(erase_iterator);
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Nexus &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					game_state->getMineralWorkerCount() < 70 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal())
				{
					int worker_count = 0;
					auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
					{
						if (mineral_worker_iterator->getBase()->getArea()->Id() == building_list_iterator->getBase()->getArea()->Id())
							worker_count++;
						mineral_worker_iterator++;
					}
					if (worker_count < building_list_iterator->getBase()->getArea()->Minerals().size() * 2)
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Probe);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Gateway &&
					Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_High_Templar, 0) &&
					Broodwar->self()->hasResearched(BWAPI::TechTypes::Psionic_Storm) &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					(double)game_state->getUnitTypeCount(BWAPI::UnitTypes::Protoss_High_Templar) / (double)game_state->getMilitary()->size() < 0.1 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 150 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal() - 2)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_High_Templar);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Gateway &&
					Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Dragoon, 0) &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 125 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 50 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal() - 2)
				{
					if (game_state->getUnitTypeCount(BWAPI::UnitTypes::Protoss_Dragoon) == 0)
					{
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Dragoon);
					}
					else if (game_state->getUnitTypeCount(BWAPI::UnitTypes::Protoss_Zealot) / game_state->getUnitTypeCount(BWAPI::UnitTypes::Protoss_Dragoon) < 3)
					{
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Zealot);
					}
					else
					{
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Dragoon);
					}
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Gateway &&
					!Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Dragoon, 0) &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal() - 2)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Zealot);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Gateway &&
					Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Dragoon, 0) &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() < 50 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal() - 2)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Zealot);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Templar_Archives &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isResearching() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 200 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 200)
				{
					building_list_iterator->getUnit()->research(BWAPI::TechTypes::Psionic_Storm);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Forge &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isUpgrading())
				{
					if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) == 0 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Weapons);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Armor) == 0 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Armor) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Armor);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) == 0 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 200 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 200)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Plasma_Shields);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) == 1 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) &&
						game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 150)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Weapons);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Armor) == 1 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Armor) &&
						game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 175 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 175)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Armor);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) == 1 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 300 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 300)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Plasma_Shields);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) == 2 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) &&
						game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 200 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 200)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Weapons);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Armor) == 2 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Armor) &&
						game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 250 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 250)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Armor);
					}
					else if (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) == 2 &&
						!Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 400 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 400)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Plasma_Shields);
					}

					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core &&
					!building_list_iterator->getUnit()->isUpgrading() &&
					Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Singularity_Charge) == 0 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 150)
				{
					building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Singularity_Charge);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun &&
					!building_list_iterator->getUnit()->isUpgrading() &&
					Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Leg_Enhancements) == 0 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 150)
				{
					building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Leg_Enhancements);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Robotics_Facility &&
					Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Observer, 0) &&
					!building_list_iterator->getUnit()->isTraining() &&
					game_state->getDetectorCount() < 5 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 25 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 75)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Observer);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Assimilator &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getNumberGasWorkers() < 3)
				{
					auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
					{
						if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
						{
							building_list_iterator->addGasWorker(static_cast<int>(mineral_worker_iterator->getUnit()->getID()));
							mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
							game_state->getMineralWorkers()->erase(mineral_worker_iterator);
							mineral_worker_iterator = game_state->getMineralWorkers()->end();
						}
						else
						{
							mineral_worker_iterator++;
						}
					}
				}
				else
				{
					building_list_iterator++;
				}
			}
		}
		else if (Broodwar->self()->getRace() == BWAPI::Races::Zerg)
		{
			if (building_list_iterator->getUnit() == nullptr)
			{
				auto erase_iterator = building_list_iterator;
				building_list_iterator = game_state->getBuildingList()->erase(erase_iterator);
			}
			else if (!building_list_iterator->getUnit()->exists())
			{
				auto erase_iterator = building_list_iterator;
				building_list_iterator = game_state->getBuildingList()->erase(erase_iterator);
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Hatchery &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0 &&
				(game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Lair) == 0 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hive) == 0) &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
			{
				building_list_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Lair);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Lair &&
				Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Pneumatized_Carapace) == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 150)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Pneumatized_Carapace);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Metabolic_Boost) == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Metabolic_Boost);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Muscular_Augments) == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 150)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Muscular_Augments);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Grooved_Spines) == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 150)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Grooved_Spines);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Melee_Attacks) == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Zerg_Melee_Attacks);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Missile_Attacks) == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Zerg_Missile_Attacks);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Carapace) == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 150)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Zerg_Carapace);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Melee_Attacks) == 1 &&
				(game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Lair) >= 1 ||
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hive) >= 1) &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 150)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Zerg_Melee_Attacks);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Missile_Attacks) == 1 &&
				(game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Lair) >= 1 ||
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hive) >= 1) &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 150)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Zerg_Missile_Attacks);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber &&
				Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Carapace) == 1 &&
				(game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Lair) >= 1 ||
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hive) >= 1) &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 225 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 225)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Zerg_Carapace);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Creep_Colony &&
				Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Sunken_Colony, 0) &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50)
			{
				building_list_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Sunken_Colony);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Extractor &&
				building_list_iterator->getUnit()->isCompleted() &&
				building_list_iterator->getNumberGasWorkers() < 3)
			{
				auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
				while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
				{
					if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
					{
						building_list_iterator->addGasWorker(static_cast<int>(mineral_worker_iterator->getUnit()->getID()));
						game_state->unassignWorkerFromMineral(&(*mineral_worker_iterator));
						mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
						game_state->getMineralWorkers()->erase(mineral_worker_iterator);
						mineral_worker_iterator = game_state->getMineralWorkers()->end();
					}
					else
					{
						mineral_worker_iterator++;
					}
				}
				building_list_iterator++;
			}
			else
			{
				building_list_iterator++;
			}
		}
	}

	if (Broodwar->self()->getRace() == BWAPI::Races::Zerg &&
		game_state->getBuildOrder() != BuildOrder::TeamMeleeorFFA)
	{
		auto larva_iterator = game_state->getLarva()->begin();
		while (larva_iterator != game_state->getLarva()->end())
		{
			if (game_state->getBuildOrder() == BuildOrder::FivePool)
			{
				if (((game_state->getSupplyBuilt() < 5 &&
					game_state->getSpawningPool() < 1) ||
					(game_state->getSupplyBuilt() < 7 &&
					game_state->getSpawningPool() == 1)) &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50)
				{
					if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Drone))
					{
						auto erase_iterator = larva_iterator;
						larva_iterator = game_state->getLarva()->erase(erase_iterator);
						game_state->addSupplyUsed(1);
						game_state->addSupplyBuilt(1);
					}
					else
						larva_iterator++;
				}
				else if (game_state->getSpawningPool() >= 1 &&
					game_state->getSupplyUsed() < game_state->getSupplyTotal() &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50)
				{
					if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Zergling))
					{
						auto erase_iterator = larva_iterator;
						larva_iterator = game_state->getLarva()->erase(erase_iterator);
						game_state->addSupplyUsed(1);
					}
					else
						larva_iterator++;
				}
				else if (game_state->getSupplyUsed() > game_state->getSupplyExpected() - 2 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100)
				{
					if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Overlord))
					{
						auto erase_iterator = larva_iterator;
						larva_iterator = game_state->getLarva()->erase(erase_iterator);
						
					}
					else
						larva_iterator++;
				}
				else
					larva_iterator++;
			}
			else
			{
				if (((game_state->getSupplyExpected() < 20 &&
					game_state->getSupplyUsed() > game_state->getSupplyExpected() - 2) ||
					(game_state->getSupplyExpected() >= 20 &&
					game_state->getSupplyUsed() > game_state->getSupplyExpected() - 10)) &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100)
				{
					if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Overlord))
					{
						auto erase_iterator = larva_iterator;
						larva_iterator = game_state->getLarva()->erase(erase_iterator);
						game_state->addSupplyExpected(8);
					}
					else
					{
						larva_iterator++;
					}
				}
				else if (game_state->getSupplyUsed() < game_state->getSupplyTotal() &&
					game_state->getMineralWorkerCount() < 30 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
					!Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Hydralisk, 0) &&
					Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Zergling, 0))
				{
					int decider = rand() % 2 + 1;
					if (decider == 1)
					{
						if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Drone))
						{
							auto erase_iterator = larva_iterator;
							larva_iterator = game_state->getLarva()->erase(erase_iterator);
							game_state->addSupplyUsed(1);
						}
						else
						{
							larva_iterator++;
						}
					}
					else if (decider == 2)
					{
						if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Zergling))
						{
							auto erase_iterator = larva_iterator;
							larva_iterator = game_state->getLarva()->erase(erase_iterator);
							game_state->addSupplyUsed(1);
						}
						else
						{
							larva_iterator++;
						}
					}
				}
				else if (game_state->getSupplyUsed() < game_state->getSupplyTotal() &&
					game_state->getMineralWorkerCount() < 30 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 75 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 25 &&
					Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Hydralisk, 0))
				{
					int decider = rand() % 2 + 1;

					if (decider == 2 &&
						Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 75 &&
						Broodwar->self()->gas() - game_state->getGasCommitted() >= 25)
					{
						if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Hydralisk))
						{
							auto erase_iterator = larva_iterator;
							larva_iterator = game_state->getLarva()->erase(erase_iterator);
							game_state->addSupplyUsed(1);
						}
						else
						{
							larva_iterator++;
						}
					}
					else
					{
						if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Drone))
						{
							auto erase_iterator = larva_iterator;
							larva_iterator = game_state->getLarva()->erase(erase_iterator);
							game_state->addSupplyUsed(1);
						}
						else
						{
							larva_iterator++;
						}
					}
				}
				else if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 75 &&
					Broodwar->self()->gas() - game_state->getGasCommitted() >= 30 &&
					Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Hydralisk))
				{
					if (game_state->getUnitTypeCount(BWAPI::UnitTypes::Zerg_Zergling) / game_state->getUnitTypeCount(BWAPI::UnitTypes::Zerg_Hydralisk) > 2)
					{
						if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Hydralisk))
						{
							auto erase_iterator = larva_iterator;
							larva_iterator = game_state->getLarva()->erase(erase_iterator);
							game_state->addSupplyUsed(1);
						}
						else
						{
							larva_iterator++;
						}
					}
					else
					{
						if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Zergling))
						{
							auto erase_iterator = larva_iterator;
							larva_iterator = game_state->getLarva()->erase(erase_iterator);
							game_state->addSupplyUsed(1);
						}
						else
						{
							larva_iterator++;
						}
					}
				}
				else if (game_state->getSupplyUsed() < game_state->getSupplyTotal() &&
					game_state->getMineralWorkerCount() < 30 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50)
				{
					if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Drone))
					{
						auto erase_iterator = larva_iterator;
						larva_iterator = game_state->getLarva()->erase(erase_iterator);
						game_state->addSupplyUsed(1);
					}
					else
					{
						larva_iterator++;
					}
				}
				else
				{
					larva_iterator++;
				}
			}
		}
	}
	if (game_state->getBuildOrder() == BuildOrder::TeamMeleeorFFA)
	{
		auto self = Broodwar->self();
		if (self->minerals() - game_state->getMineralsCommitted() >= 250 &&
			game_state->getHatchery() == 0)
		{
			if (worker_manager->build(BWAPI::UnitTypes::Zerg_Hatchery, 1, game_state))
			{
				game_state->setLastTimeExpanded();
				game_state->addMineralsCommitted(300);
				game_state->addHatchery(1);
				game_state->toggleExpanding();
			}
		}
		if (self->minerals() - game_state->getMineralsCommitted() >= 80 &&
			game_state->getGas() < game_state->getMiningBaseCount() &&
			Broodwar->elapsedTime() > 300)
		{
			if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 3, game_state))
			{
				game_state->addGas(1);
				game_state->addMineralsCommitted(100);
			}
			else if (worker_manager->build(BWAPI::UnitTypes::Terran_Refinery, 3, game_state))
			{
				game_state->addGas(1);
				game_state->addMineralsCommitted(100);
			}
			else if (worker_manager->build(BWAPI::UnitTypes::Zerg_Extractor, 3, game_state))
			{
				game_state->addGas(1);
				game_state->addMineralsCommitted(100);
			}
			else if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 4, game_state))
			{
				game_state->addGas(1);
				game_state->addMineralsCommitted(100);
			}
			else if (worker_manager->build(BWAPI::UnitTypes::Terran_Refinery, 4, game_state))
			{
				game_state->addGas(1);
				game_state->addMineralsCommitted(100);
			}
			else if (worker_manager->build(BWAPI::UnitTypes::Zerg_Extractor, 4, game_state))
			{
				game_state->addGas(1);
				game_state->addMineralsCommitted(100);
			}
		}
		if (self->minerals() - game_state->getMineralsCommitted() >= 400 &&
			Broodwar->elapsedTime() - game_state->getLastTimeExpanded() > 330 &&
			!game_state->getExpanding() &&
			game_state->getBarracks() >= 2 &&
			game_state->getCommandCenter() >= 1)
		{
			if (worker_manager->build(BWAPI::UnitTypes::Terran_Command_Center, 1, game_state))
			{
				game_state->setLastTimeExpanded();
				game_state->addMineralsCommitted(400);
				game_state->addCommandCenter(1);
				game_state->toggleExpanding();
			}
		}
		if (self->minerals() - game_state->getMineralsCommitted() >= 350 &&
			game_state->getNexus() == 0)
		{
			if (worker_manager->build(BWAPI::UnitTypes::Protoss_Nexus, 3, game_state))
			{
				game_state->addMineralsCommitted(400);
				game_state->addNexus(1);
			}
		}
		if (self->minerals() - game_state->getMineralsCommitted() >= 350 &&
			game_state->getCommandCenter() == 0)
		{
			if (worker_manager->build(BWAPI::UnitTypes::Terran_Command_Center, 3, game_state))
			{
				game_state->addMineralsCommitted(400);
				game_state->addCommandCenter(1);
			}
		}
		if (self->minerals() - game_state->getMineralsCommitted() >= 100 &&
			game_state->getCommandCenter() >= 1 &&
			((game_state->getBarracks() == 0 &&
			game_state->getSupplyUsed(BWAPI::Races::Terran) >= game_state->getSupplyExpected(BWAPI::Races::Terran) - 2) ||
			(game_state->getBarracks() > 1 &&
			game_state->getSupplyUsed(BWAPI::Races::Terran) >= game_state->getSupplyExpected(BWAPI::Races::Terran) - (game_state->getCompletedMacroBuildings() * 2 + game_state->getUnderConstructionMacroBuildings()))))
		{
			if (worker_manager->build(BWAPI::UnitTypes::Terran_Supply_Depot, 3, game_state))
			{
				game_state->addMineralsCommitted(100);
				game_state->addSupplyExpected(8, BWAPI::Races::Terran);
			}
		}
		if (self->minerals() - game_state->getMineralsCommitted() >= 100 &&
			game_state->getNexus() >= 1 &&
			((game_state->getGateway() == 0 &&
			game_state->getSupplyUsed(BWAPI::Races::Protoss) >= game_state->getSupplyExpected(BWAPI::Races::Protoss) - 2) ||
			(game_state->getGateway() >= 1 &&
			game_state->getSupplyUsed(BWAPI::Races::Protoss) >= game_state->getSupplyExpected(BWAPI::Races::Protoss) - (game_state->getCompletedMacroBuildings() * 2 + game_state->getUnderConstructionMacroBuildings())) ||
			(!game_state->getPylonBuilding() &&
			game_state->getNoPoweredPositions())))
		{
			if (worker_manager->build(BWAPI::UnitTypes::Protoss_Pylon, 3, game_state))
			{
				game_state->setPylonBuilding(true);
				game_state->addMineralsCommitted(100);
				game_state->addSupplyExpected(8, BWAPI::Races::Protoss);
			}
		}
		if (self->minerals() - game_state->getMineralsCommitted() >= 200 &&
			game_state->getSpawningPool() == 0)
		{
			if (worker_manager->build(BWAPI::UnitTypes::Zerg_Spawning_Pool, 3, game_state))
			{
				game_state->addMineralsCommitted(200);
				game_state->addSpawningPool(1);
			}
		}
		if (self->minerals() - game_state->getMineralsCommitted() >= 150 &&
			game_state->getBarracks() < 2)
		{
			if (worker_manager->build(BWAPI::UnitTypes::Terran_Barracks, 3, game_state))
			{
				game_state->addMineralsCommitted(150);
				game_state->addBarracks(1);
			}
		}
		if (self->minerals() - game_state->getMineralsCommitted() >= 150 &&
			game_state->getGateway() < 2)
		{
			if (worker_manager->build(BWAPI::UnitTypes::Protoss_Gateway, 3, game_state))
			{
				game_state->addMineralsCommitted(150);
				game_state->addGateway(1);
			}
		}
		if (self->minerals() - game_state->getMineralsCommitted() >= 130 &&
			game_state->getGas() >= 1 &&
			game_state->getBarracks() >= 1 &&
			!game_state->checkAcademy())
		{
			if (worker_manager->build(BWAPI::UnitTypes::Terran_Academy, 3, game_state))
			{
				game_state->toggleAcademy();
				game_state->addMineralsCommitted(150);
			}
		}
		if (self->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.mineralPrice() - 30 &&
			self->gas() - game_state->getGasCommitted() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.gasPrice() - 30 &&
			game_state->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Hydralisk_Den) &&
			game_state->getHydraliskDen() == 0)
		{
			if (worker_manager->build(BWAPI::UnitTypes::Zerg_Hydralisk_Den, 3, game_state))
			{
				game_state->addMineralsCommitted(BWAPI::UnitTypes::Zerg_Hydralisk_Den.mineralPrice());
				game_state->addGasCommitted(BWAPI::UnitTypes::Zerg_Hydralisk_Den.gasPrice());
				game_state->addHydraliskDen(1);
			}
		}
	}
	else if (Broodwar->self()->getRace() == BWAPI::Races::Terran)
	{
		if (game_state->getBuildOrder() == BuildOrder::TAyumiBuildOpening)
		{
			if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
				((game_state->getSupplyBuilt() >= 9 &&
				game_state->getSupplyExpected() == 10) ||
				(game_state->getSupplyBuilt() >= 14 &&
				game_state->getSupplyExpected() == 18) ||
				(game_state->getSupplyBuilt() >= 23 &&
				game_state->getSupplyExpected() == 36) ||
				(game_state->getSupplyBuilt() >= 37 &&
				game_state->getSupplyExpected() == 44) ||
				(game_state->getSupplyBuilt() >= 44 &&
				game_state->getSupplyExpected() == 52) ||
				(game_state->getSupplyExpected() >= 52 &&
				game_state->getSupplyUsed() >= game_state->getSupplyExpected() - 4)))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Supply_Depot, 3, game_state))
				{
					game_state->addMineralsCommitted(100);
					game_state->addSupplyExpected(8);
				}
			}
			if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				((game_state->getSupplyBuilt() >= 10 &&
				game_state->getBarracks() == 0) ||
				(game_state->getSupplyBuilt() >= 29 &&
				game_state->getBarracks() < 4)))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Barracks, 3, game_state))
				{
					game_state->addMineralsCommitted(150);
					game_state->addBarracks(1);
				}
			}
			if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 400 &&
				game_state->getSupplyBuilt() >= 20 &&
				game_state->getSupplyExpected() == 26)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Command_Center, 1, game_state))
				{
					game_state->addMineralsCommitted(400);
					game_state->toggleExpanding();
					game_state->setLastTimeExpanded();
				}
			}
			if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
				game_state->getSupplyBuilt() >= 35 &&
				game_state->getGas() == 0)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Refinery, 3, game_state))
				{
					game_state->addMineralsCommitted(100);
					game_state->addGas(1);
				}
			}
			if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				game_state->getSupplyBuilt() >= 38 &&
				!game_state->checkAcademy())
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Academy, 3, game_state))
				{
					game_state->addMineralsCommitted(150);
					game_state->toggleAcademy();
				}
			}
		}
		else
		{
			if (((Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 400 &&
				!game_state->getExpanding() &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Terran_Command_Center) > 1) ||
				(Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 400 &&
				!game_state->getExpanding() &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Terran_Command_Center) == 1 &&
				game_state->getUnitTypeCount(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode) >= 1 &&
				Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode) &&
				Broodwar->enemies().size() == 1 &&
				Broodwar->enemy()->getRace() != BWAPI::Races::Zerg) ||
				(Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 400 &&
				!game_state->getExpanding() &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Terran_Command_Center) == 1 &&
				Broodwar->enemies().size() == 1 &&
				Broodwar->enemy()->getRace() == BWAPI::Races::Zerg) ||
				(Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 400 &&
				!game_state->getExpanding() &&
				Broodwar->enemies().size() > 1)) &&
				Broodwar->elapsedTime() - game_state->getLastTimeExpanded() >= 105)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Command_Center, 1, game_state))
				{
					game_state->addMineralsCommitted(400);
					game_state->toggleExpanding();
					game_state->setLastTimeExpanded();
				}
			}
			if (game_state->getBarracks() > 1 &&
				game_state->getGas() > 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				!game_state->checkAcademy())
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Academy, 3, game_state))
				{
					game_state->addMineralsCommitted(150);
					game_state->toggleAcademy();
				}
			}
			if (game_state->getSupplyUsed() >= game_state->getSupplyExpected() - (game_state->getCompletedMacroBuildings() * 2 + game_state->getUnderConstructionMacroBuildings()) &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Supply_Depot, 3, game_state))
				{
					game_state->addSupplyExpected(8);
					game_state->addMineralsCommitted(100);
				}
			}
			if ((Broodwar->enemies().size() == 1 &&
				Broodwar->enemy()->getRace() != BWAPI::Races::Terran) ||
				Broodwar->enemies().size() > 1)
			{
				if ((game_state->getBarracks() < 1 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150) ||
					(game_state->getBarracks() < 2 &&
					game_state->getBunker() == 1 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150) ||
					(game_state->getBarracks() >= 2 &&
					game_state->getFactory() >= 1 &&
					game_state->getBarracks() < 3 * game_state->getBuildingTypeCount(BWAPI::UnitTypes::Terran_Command_Center) - 1 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 + 50 * game_state->getBarracks()))
				{
					if (worker_manager->build(BWAPI::UnitTypes::Terran_Barracks, 3, game_state))
					{
						game_state->addMineralsCommitted(150);
						game_state->addBarracks(1);
					}
				}
				if (game_state->getBarracks() == 1 &&
					game_state->getBunker() == 0 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Terran_Bunker.mineralPrice())
				{

					if (Broodwar->enemies().size() == 1)
					{
						AIBase* main_base = game_state->getMainBase();
						if (main_base != nullptr)
						{
							if (Broodwar->enemy()->getRace() == BWAPI::Races::Zerg)
							{
								if (worker_manager->build(BWAPI::UnitTypes::Terran_Bunker, 3, game_state))
								{
									game_state->addMineralsCommitted(BWAPI::UnitTypes::Terran_Bunker.mineralPrice());
									game_state->addBunker(1);
								}
							}
							else
							{
								BWAPI::TilePosition center_search = (BWAPI::TilePosition)(*game_state->getNearestContainingBase((BWAPI::TilePosition)Broodwar->self()->getStartLocation())->getArea()->ChokePoints().begin())->Center();
								if (worker_manager->build(BWAPI::UnitTypes::Terran_Bunker, 3, game_state))//worker_manager->buildNearPositionInBase(BWAPI::UnitTypes::Terran_Bunker, center_search, 3, game_state))
								{
									game_state->addMineralsCommitted(BWAPI::UnitTypes::Terran_Bunker.mineralPrice());
									game_state->addBunker(1);
								}
							}
						}
					}
					else
					{
						AIBase* main_base = game_state->getMainBase();
						if (main_base != nullptr)
						{
							BWAPI::TilePosition center_search = (BWAPI::TilePosition)(*game_state->getNearestContainingBase((BWAPI::TilePosition)Broodwar->self()->getStartLocation())->getArea()->ChokePoints().begin())->Center();
							if (worker_manager->buildNearPositionInBase(BWAPI::UnitTypes::Terran_Bunker, center_search, 3, game_state))
							{
								game_state->addMineralsCommitted(BWAPI::UnitTypes::Terran_Bunker.mineralPrice());
								game_state->addBunker(1);
							}
						}
					}

				}
			}
			else
			{
				if ((game_state->getBarracks() < 2 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150) ||
					(game_state->getBarracks() >= 2 &&
					game_state->getFactory() >= 1 &&
					game_state->getBarracks() < 3 * game_state->getBuildingTypeCount(BWAPI::UnitTypes::Terran_Command_Center) - 1 &&
					Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 + 50 * game_state->getBarracks()))
				{
					if (worker_manager->build(BWAPI::UnitTypes::Terran_Barracks, 3, game_state))
					{
						game_state->addMineralsCommitted(150);
						game_state->addBarracks(1);
					}
				}
			}
			if (game_state->getBarracks() > 1 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
				game_state->getGas() < game_state->getBuildingTypeCount(BWAPI::UnitTypes::Terran_Command_Center))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Refinery, 3, game_state))
				{
					game_state->addMineralsCommitted(100);
					game_state->addGas(1);
				}
				else if (worker_manager->build(BWAPI::UnitTypes::Terran_Refinery, 4, game_state))
				{
					game_state->addMineralsCommitted(100);
					game_state->addGas(1);
				}
			}
			if (((game_state->getBuildingTypeCount(BWAPI::UnitTypes::Terran_Barracks) >= 2 &&
				game_state->getFactory() < 1) ||
				(game_state->getBuildingTypeCount(BWAPI::UnitTypes::Terran_Command_Center) >= 2 &&
				game_state->getFactory() < 2)) &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Terran_Factory.mineralPrice() &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= BWAPI::UnitTypes::Terran_Factory.gasPrice())
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Factory, 3, game_state))
				{
					game_state->addMineralsCommitted(BWAPI::UnitTypes::Terran_Factory.mineralPrice());
					game_state->addGasCommitted(BWAPI::UnitTypes::Terran_Factory.gasPrice());
					game_state->addFactory(1);
				}
			}
			if (game_state->getBuildingTypeCount(BWAPI::UnitTypes::Terran_Command_Center) >= 2 &&
				game_state->getEngineeringBay() < 1 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Terran_Engineering_Bay.mineralPrice())
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Engineering_Bay, 3, game_state))
				{
					game_state->addMineralsCommitted(BWAPI::UnitTypes::Terran_Engineering_Bay.mineralPrice());
					game_state->addEngineeringBay(1);
				}
			}
			if (game_state->getBuildingTypeCount(BWAPI::UnitTypes::Terran_Command_Center) >= 2 &&
				game_state->getFactory() >= 1 &&
				game_state->getArmory() < 1 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Terran_Armory.mineralPrice() &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= BWAPI::UnitTypes::Terran_Armory.gasPrice())
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Armory, 3, game_state))
				{
					game_state->addMineralsCommitted(BWAPI::UnitTypes::Terran_Armory.mineralPrice());
					game_state->addGasCommitted(BWAPI::UnitTypes::Terran_Armory.gasPrice());
					game_state->addArmory(1);
				}
			}
			if (game_state->getBuildingTypeCount(BWAPI::UnitTypes::Terran_Command_Center) >= 3 &&
				game_state->getStarport() < 1 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Terran_Starport.mineralPrice() &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= BWAPI::UnitTypes::Terran_Starport.gasPrice())
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Starport, 3, game_state))
				{
					game_state->addMineralsCommitted(BWAPI::UnitTypes::Terran_Starport.mineralPrice());
					game_state->addGasCommitted(BWAPI::UnitTypes::Terran_Starport.gasPrice());
					game_state->addStarport(1);
				}
			}
			if (game_state->getStarport() >= 1 &&
				game_state->getScienceFacility() < 1 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Terran_Science_Facility.mineralPrice() &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= BWAPI::UnitTypes::Terran_Science_Facility.gasPrice())
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Science_Facility, 3, game_state))
				{
					game_state->addMineralsCommitted(BWAPI::UnitTypes::Terran_Science_Facility.mineralPrice());
					game_state->addGasCommitted(BWAPI::UnitTypes::Terran_Science_Facility.gasPrice());
					game_state->addScienceFacility(1);
				}
			}
		}
	}
	else if (Broodwar->self()->getRace() == BWAPI::Races::Protoss)
	{
		if (game_state->getBuildOrder() == BuildOrder::P2Gate1)
		{
			if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
				(game_state->getSupplyUsed() == 8 ||
				(game_state->getBarracks() == 2 &&
				game_state->getSupplyUsed() >= game_state->getSupplyExpected() - 10)))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Pylon, 3, game_state))
				{
					game_state->addSupplyExpected(8);
					game_state->addMineralsCommitted(100);
				}
			}
			if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Gateway) < 2 &&
				(game_state->getSupplyUsed() == 9 ||
				game_state->getSupplyUsed() == 10))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Gateway, 3, game_state))
				{
					game_state->addMineralsCommitted(150);
					game_state->addBarracks(1);
				}
			}
		}
		else if (game_state->getBuildOrder() == BuildOrder::P4GateGoonOpening)
		{
			if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
				((game_state->getSupplyBuilt() == 8 &&
				game_state->getPylon() < 1) ||
				(game_state->getSupplyBuilt() == 12 &&
				game_state->getPylon() < 2) ||
				(game_state->getSupplyBuilt() == 22 &&
				game_state->getPylon() < 3) ||
				(game_state->getSupplyBuilt() == 33 &&
				game_state->getPylon() < 4) ||
				(game_state->getSupplyBuilt() == 41 &&
				game_state->getPylon() < 5)))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Pylon, 3, game_state))
				{
					game_state->addSupplyExpected(8);
					game_state->addMineralsCommitted(100);
					game_state->addPylon(1);
				}
			}
			if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				((game_state->getSupplyBuilt() == 10 &&
				game_state->getBarracks() == 0) ||
				(game_state->getSupplyBuilt() >= 31 &&
				game_state->getBarracks() < 4)))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Gateway, 3, game_state))
				{
					game_state->addBarracks(1);
					game_state->addMineralsCommitted(150);
				}
			}
			if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100 &&
				game_state->getGas() == 0 &&
				game_state->getSupplyBuilt() >= 16)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 3, game_state))
				{
					game_state->addGas(1);
					game_state->addMineralsCommitted(100);
				}
			}
			if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Cybernetics_Core.mineralPrice() &&
				!game_state->checkCyberCore() &&
				game_state->getSupplyBuilt() >= 17)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Cybernetics_Core, 3, game_state))
				{
					game_state->toggleCyberCore();
					game_state->addMineralsCommitted(BWAPI::UnitTypes::Protoss_Cybernetics_Core.mineralPrice());
				}
			}
		}
		else if (game_state->getBuildOrder() == BuildOrder::P4GateGoonMid)
		{
			if ((Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 400 &&
				!game_state->getExpanding() &&
				Broodwar->elapsedTime() - game_state->getLastTimeExpanded() >= 105 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) == 1) ||
				(Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 400 &&
				!game_state->getExpanding() &&
				Broodwar->elapsedTime() - game_state->getLastTimeExpanded() >= 105 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) == 2 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Observatory) >= 1))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Nexus, 1, game_state))
				{
					game_state->addMineralsCommitted(400);
					game_state->toggleExpanding();
					game_state->setLastTimeExpanded();
				}
			}
			if ((game_state->getBarracks() == 0 && game_state->getSupplyUsed() >= game_state->getSupplyExpected() - 2 ||
				game_state->getBarracks() >= 1 && game_state->getSupplyUsed() >= game_state->getSupplyExpected() - 10) &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Pylon, 3, game_state))
				{
					game_state->addSupplyExpected(8);
					game_state->addMineralsCommitted(100);
				}
			}
			if (game_state->getBarracks() >= 2 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core) == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Cybernetics_Core, 3, game_state))
				{
					game_state->addMineralsCommitted(200);
				}
			}
			if (game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				game_state->getForge() == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Forge, 3, game_state))
				{
					game_state->addMineralsCommitted(150);
					game_state->addForge(1);
				}
			}
			if (((game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core) > 0 &&
				game_state->getGas() == 0) ||
				(game_state->getGas() >= 1 &&
				game_state->getGas() < game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus))) &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 3, game_state))
				{
					game_state->addMineralsCommitted(100);
					game_state->addGas(1);
				}
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 4, game_state))
				{
					game_state->addMineralsCommitted(100);
					game_state->addGas(1);
				}
			}
			if (game_state->getRoboticsFacility() == 0 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) == 0 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 200 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Robotics_Facility, 3, game_state))
				{
					game_state->addMineralsCommitted(200);
					game_state->addGasCommitted(200);
					game_state->addRoboticsFacility(1);
				}
			}
			if (game_state->getObservatory() == 0 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Observatory) == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Observatory, 3, game_state))
				{
					game_state->addMineralsCommitted(50);
					game_state->addGasCommitted(100);
					game_state->addObservatory(1);
				}
			}
			if (game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				game_state->getForge() == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Forge, 3, game_state))
				{
					game_state->addMineralsCommitted(150);
					game_state->addForge(1);
				}
			}
		}
		else if (game_state->getBuildOrder() == BuildOrder::P4GateGoonLate)
		{
			if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 400 &&
				!game_state->getExpanding() &&
				Broodwar->elapsedTime() - game_state->getLastTimeExpanded() >= 105)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Nexus, 1, game_state))
				{
					game_state->addMineralsCommitted(400);
					game_state->toggleExpanding();
					game_state->setLastTimeExpanded();
				}
			}
			if (((game_state->getBarracks() == 0 && game_state->getSupplyUsed() >= game_state->getSupplyExpected() - 2 ) ||
				(game_state->getBarracks() >= 1 && game_state->getSupplyUsed() >= game_state->getSupplyExpected() - 10)) &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Pylon, 3, game_state))
				{
					game_state->addSupplyExpected(8);
					game_state->addMineralsCommitted(100);
				}
			}
			if (game_state->getBarracks() >= 2 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core) == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Cybernetics_Core, 3, game_state))
				{
					game_state->addMineralsCommitted(200);
				}
			}
			if (game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				game_state->getForge() == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Forge, 3, game_state))
				{
					game_state->addMineralsCommitted(150);
					game_state->addForge(1);
				}
			}
			if (((game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core) > 0 &&
				game_state->getGas() == 0) ||
				(game_state->getGas() >= 1 &&
				game_state->getGas() < game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus))) &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 3, game_state))
				{
					game_state->addMineralsCommitted(100);
					game_state->addGas(1);
				}
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 4, game_state))
				{
					game_state->addMineralsCommitted(100);
					game_state->addGas(1);
				}
			}
			if (game_state->getRoboticsFacility() == 0 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) == 0 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 200 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Robotics_Facility, 3, game_state))
				{
					game_state->addMineralsCommitted(200);
					game_state->addGasCommitted(200);
					game_state->addRoboticsFacility(1);
				}
			}
			if (game_state->getObservatory() == 0 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Observatory) == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Observatory, 3, game_state))
				{
					game_state->addMineralsCommitted(50);
					game_state->addGasCommitted(100);
					game_state->addObservatory(1);
				}
			}
			if (game_state->getObservatory() > 0 &&
				game_state->getCitadelofAdun() == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Citadel_of_Adun, 3, game_state))
				{
					game_state->addMineralsCommitted(150);
					game_state->addGasCommitted(100);
					game_state->addCitadelofAdun(1);
				}
			}
			if (game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				game_state->getForge() == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Forge, 3, game_state))
				{
					game_state->addMineralsCommitted(150);
					game_state->addForge(1);
				}
			}
		}
		else if (game_state->getBuildOrder() == BuildOrder::PForgeFastExpand9poolOpening)
		{
			
		}
		else
		{
			if (Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 400 &&
				!game_state->getExpanding() &&
				Broodwar->elapsedTime() - game_state->getLastTimeExpanded() >= 105)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Nexus, 1, game_state))
				{
					game_state->addMineralsCommitted(400);
					game_state->toggleExpanding();
					game_state->setLastTimeExpanded();
				}
			}
			if ((game_state->getBarracks() == 0 && game_state->getSupplyUsed() >= game_state->getSupplyExpected() - 2 ||
				game_state->getBarracks() >= 1 && game_state->getSupplyUsed() >= game_state->getSupplyExpected() - 10) &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Pylon, 3, game_state))
				{
					game_state->addSupplyExpected(8);
					game_state->addMineralsCommitted(100);
				}
			}
			if (game_state->getBarracks() >= 2 &&
				!game_state->checkCyberCore() &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Cybernetics_Core, 3, game_state))
				{
					game_state->addMineralsCommitted(200);
					game_state->toggleCyberCore();
				}
			}
			if (game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				game_state->getForge() == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Forge, 3, game_state))
				{
					game_state->addMineralsCommitted(150);
					game_state->addForge(1);
				}
			}
			if (((game_state->checkCyberCore() &&
				game_state->getGas() == 0) ||
				(game_state->getGas() >= 1 &&
				game_state->getGas() < game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus))) &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 3, game_state))
				{
					game_state->addMineralsCommitted(100);
					game_state->addGas(1);
				}
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 4, game_state))
				{
					game_state->addMineralsCommitted(100);
					game_state->addGas(1);
				}
			}
			if (game_state->getObservatory() > 0 &&
				game_state->getGas() == 1 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 4, game_state))
				{
					game_state->addMineralsCommitted(100);
					game_state->addGas(1);
				}
			}
			if (Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Robotics_Facility, 0) &&
				game_state->getRoboticsFacility() == 0 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 200 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Robotics_Facility, 3, game_state))
				{
					game_state->addMineralsCommitted(200);
					game_state->addGasCommitted(200);
					game_state->addRoboticsFacility(1);
				}
			}
			if (Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Observatory, 0) &&
				game_state->getObservatory() == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Observatory, 3, game_state))
				{
					game_state->addMineralsCommitted(50);
					game_state->addGasCommitted(100);
					game_state->addObservatory(1);
				}
			}
			if (game_state->getObservatory() > 0 &&
				game_state->getCitadelofAdun() == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Citadel_of_Adun, 3, game_state))
				{
					game_state->addMineralsCommitted(150);
					game_state->addGasCommitted(100);
					game_state->addCitadelofAdun(1);
				}
			}
			if (Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Templar_Archives, 0) &&
				game_state->getTemplarArchives() == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Templar_Archives, 3, game_state))
				{
					game_state->addMineralsCommitted(150);
					game_state->addGasCommitted(200);
					game_state->addTemplarArchives(1);
				}
			}
			if ((game_state->getBarracks() < 2 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150) ||
				(game_state->getBarracks() >= 2 &&
				game_state->getBarracks() <= 3 * game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 + 100 * game_state->getBarracks()))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Gateway, 3, game_state))
				{
					game_state->addMineralsCommitted(150);
					game_state->addBarracks(1);
				}
			}
		}
	}
	else if (Broodwar->self()->getRace() == BWAPI::Races::Zerg)
	{
		if (game_state->getBuildOrder() == BuildOrder::FivePool)
		{
			if (game_state->getSpawningPool() == 0 &&
				game_state->getSupplyBuilt() == 5 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Zerg_Spawning_Pool, 3, game_state))
				{
					game_state->addMineralsCommitted(200);
					game_state->addSpawningPool(1);
				}
			}
			if (game_state->getHatchery() == 1 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 300)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Zerg_Hatchery, 3, game_state))
				{
					game_state->addMineralsCommitted(300);
					game_state->addHatchery(1);
				}
			}
		}
		else
		{
			if (game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hydralisk_Den) < 1 &&
				Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Hydralisk_Den, 0) &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 150 &&
				Broodwar->self()->gas() - game_state->getGasCommitted() >= 50)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Zerg_Hydralisk_Den, 3, game_state))
				{
					game_state->addMineralsCommitted(150);
					game_state->addGasCommitted(50);
				}
			}
			if (game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Spawning_Pool) >= 1 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Creep_Colony) + game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Sunken_Colony) < 20 &&
				game_state->getMineralWorkerCount() >= 15 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 75)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Zerg_Creep_Colony, 3, game_state))
				{
					game_state->addMineralsCommitted(75);
				}
			}
			if ((game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hatchery) < 2 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 300) ||
				(Broodwar->self()->minerals() - game_state->getMineralsCommitted() - game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hatchery) * 50 >= 300 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hatchery) < 9))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Zerg_Hatchery, 3, game_state))
				{
					game_state->addSupplyExpected(1);
					game_state->addMineralsCommitted(300);
				}
			}
			if (game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hatchery) > 1 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Spawning_Pool) == 0 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Zerg_Spawning_Pool, 3, game_state))
				{
					game_state->addMineralsCommitted(200);
				}
			}
			if (game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hatchery) > 1 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 50 &&
				game_state->getGas() == 0 &&
				game_state->checkValidGasBuildTileLocation(3))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Zerg_Extractor, 3, game_state))
				{
					game_state->addMineralsCommitted(50);
					game_state->addGas(1);
				}
			}
			if (game_state->getGas() >= 1 &&
				game_state->getEvolutionChambers() < 1 &&
				game_state->getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Lair) >= 1 &&
				Broodwar->self()->minerals() - game_state->getMineralsCommitted() >= 75)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Zerg_Evolution_Chamber, 3, game_state))
				{
					game_state->addMineralsCommitted(75);
					game_state->addEvolutionChambers(1);
				}
			}
		}
	}
}

