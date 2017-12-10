#include"MacroManager.h"

MacroManager::MacroManager()
{
	
}

void MacroManager::checkMacro(WorkerManager* worker_manager, GameState &game_state)
{
	auto building_list_iterator = game_state.getBuildingList()->begin();
	while (building_list_iterator != game_state.getBuildingList()->end())
	{
		if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
		{
			if (building_list_iterator->getUnit() == nullptr)
			{
				auto erase_iterator = building_list_iterator;
				building_list_iterator = game_state.getBuildingList()->erase(erase_iterator);
			}
			else if (!building_list_iterator->getUnit()->exists())
			{
				auto erase_iterator = building_list_iterator;
				building_list_iterator = game_state.getBuildingList()->erase(erase_iterator);
			}
			else if (!building_list_iterator->getUnit()->isCompleted() &&
				!building_list_iterator->getUnit()->isBeingConstructed())
			{
				worker_manager->getNewBuildWorker(building_list_iterator->getUnit(), game_state);
				building_list_iterator++;
			}
			else  if (game_state.getBuildOrder() == BuildOrder::BGHMech)
			{ 
				if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Command_Center &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getUnit()->canBuildAddon(BWAPI::UnitTypes::Terran_Comsat_Station) &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 50)
				{
					building_list_iterator->getUnit()->buildAddon(BWAPI::UnitTypes::Terran_Comsat_Station);
					game_state.addMineralsCommitted(50);
					game_state.addGasCommitted(50);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Command_Center &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getUnit()->canBuildAddon(BWAPI::UnitTypes::Terran_Comsat_Station) &&
					!building_list_iterator->getUnit()->isTraining() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 50)
				{
					building_list_iterator->getUnit()->buildAddon(BWAPI::UnitTypes::Terran_Comsat_Station);
					game_state.addMineralsCommitted(50);
					game_state.addGasCommitted(50);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Command_Center &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
					game_state.getMineralWorkerCount() < 25 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal())
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_SCV);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks &&
					!building_list_iterator->getUnit()->isFlying() &&
					building_list_iterator->getUnit()->isCompleted() &&
					BWAPI::Broodwar->elapsedTime() < 300)
				{
					auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
					{
						if (game_state.getContainingBase(mineral_worker_iterator->getUnit())->getBaseClass() != 3 &&
							game_state.getContainingBase(mineral_worker_iterator->getUnit())->getBaseClass() != 4)
						{
							building_list_iterator->getUnit()->lift();
							mineral_worker_iterator = game_state.getMineralWorkers()->end();
						}
						else
							mineral_worker_iterator++;
					}
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks &&
					building_list_iterator->getUnit()->isFlying() &&
					BWAPI::Broodwar->elapsedTime() < 300)
				{
					bool workers_in_base = true;
					auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
					{
						if (game_state.getContainingBase(mineral_worker_iterator->getUnit())->getBaseClass() != 3)
						{
							workers_in_base = false;
							mineral_worker_iterator = game_state.getMineralWorkers()->end();
						}
						else
							mineral_worker_iterator++;
					}
					if (workers_in_base)
					{
						BWAPI::TilePosition position_to_land;
						if (BWAPI::Broodwar->self()->getStartLocation().x == 114 &&
							BWAPI::Broodwar->self()->getStartLocation().y == 116)
						{
							position_to_land.x = 90;
							position_to_land.y = 97;
						}
						else if (BWAPI::Broodwar->self()->getStartLocation().x == 114 &&
							BWAPI::Broodwar->self()->getStartLocation().y == 80)
						{
							position_to_land.x = 101;
							position_to_land.y = 63;
						}
						else if (BWAPI::Broodwar->self()->getStartLocation().x == 113 &&
							BWAPI::Broodwar->self()->getStartLocation().y == 8)
						{
							position_to_land.x = 96;
							position_to_land.y = 24;
						}
						else if (BWAPI::Broodwar->self()->getStartLocation().x == 72 &&
							BWAPI::Broodwar->self()->getStartLocation().y == 8)
						{
							position_to_land.x = 54;
							position_to_land.y = 25;
						}
						else if (BWAPI::Broodwar->self()->getStartLocation().x == 10 &&
							BWAPI::Broodwar->self()->getStartLocation().y == 6)
						{
							position_to_land.x = 30;
							position_to_land.y = 23;
						}
						else if (BWAPI::Broodwar->self()->getStartLocation().x == 8 &&
							BWAPI::Broodwar->self()->getStartLocation().y == 47)
						{
							position_to_land.x = 22;
							position_to_land.y = 57;
						}
						else if (BWAPI::Broodwar->self()->getStartLocation().x == 10 &&
							BWAPI::Broodwar->self()->getStartLocation().y == 114)
						{
							position_to_land.x = 17;
							position_to_land.y = 95;
						}
						else if (BWAPI::Broodwar->self()->getStartLocation().x == 63 &&
							BWAPI::Broodwar->self()->getStartLocation().y == 117)
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
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
					game_state.getUnitTypeCount(BWAPI::UnitTypes::Terran_Marine) < 4 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal())
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine);
					building_list_iterator++;
				}*/
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Factory &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					building_list_iterator->getUnit()->canBuildAddon() &&
					game_state.getBuildingTypeCount(BWAPI::UnitTypes::Terran_Machine_Shop) < 1 &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 50)
				{
					building_list_iterator->getUnit()->buildAddon(BWAPI::UnitTypes::Terran_Machine_Shop);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Factory &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode, 0) &&
					building_list_iterator->getUnit()->getAddon() != nullptr &&
					game_state.getBuildTanks() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal() - 2)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Factory &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Terran_Goliath, 0) &&
					building_list_iterator->getUnit()->getAddon() == nullptr &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 50 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal() - 2)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Goliath);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Machine_Shop &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Charon_Boosters) < 1 &&
					BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Terran_Goliath) &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
				{
					building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Charon_Boosters);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Machine_Shop &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					!BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode) &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 150)
				{
					building_list_iterator->getUnit()->research(BWAPI::TechTypes::Tank_Siege_Mode);
					game_state.toggleBuildTanks();
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Refinery &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getNumberGasWorkers() < 3)
				{
					auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
					{
						if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
						{
							building_list_iterator->addGasWorker(mineral_worker_iterator->getUnit()->getID());
							mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
							game_state.getMineralWorkers()->erase(mineral_worker_iterator);
							mineral_worker_iterator = game_state.getMineralWorkers()->end();
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
			else
			{
				
				if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Command_Center &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getUnit()->canBuildAddon(BWAPI::UnitTypes::Terran_Comsat_Station) &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 50)
				{
					building_list_iterator->getUnit()->buildAddon(BWAPI::UnitTypes::Terran_Comsat_Station);
					game_state.addMineralsCommitted(50);
					game_state.addGasCommitted(50);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Command_Center &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal())
				{
					int worker_count = 0;
					auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
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
					(!BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Terran_Medic, 0) ||
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() < 25) &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal())
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks &&
					BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Terran_Medic, 0) &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 25 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal())
				{
					int marines = game_state.getUnitTypeCount(BWAPI::UnitTypes::Terran_Marine);
					int medics = game_state.getUnitTypeCount(BWAPI::UnitTypes::Terran_Medic);
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
					BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::U_238_Shells) == 0 &&
					!building_list_iterator->getUnit()->isUpgrading() &&
					!building_list_iterator->getUnit()->isResearching() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 150)
				{
					building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::U_238_Shells);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Academy &&
					!building_list_iterator->getUnit()->isUpgrading() &&
					!building_list_iterator->getUnit()->isResearching() &&
					!BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Stim_Packs) &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
				{
					building_list_iterator->getUnit()->research(BWAPI::TechTypes::Stim_Packs);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Refinery &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getNumberGasWorkers() < 3)
				{
					auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
					{
						if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
						{
							building_list_iterator->addGasWorker(mineral_worker_iterator->getUnit()->getID());
							mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
							game_state.getMineralWorkers()->erase(mineral_worker_iterator);
							mineral_worker_iterator = game_state.getMineralWorkers()->end();
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
					BWAPI::Broodwar->elapsedTime() - game_state.getLastScan() > 30)
				{
					AIBase* base_to_scan = game_state.getFarthestEmptyBaseNotSecondaryScouted();
					if (base_to_scan != nullptr)
					{
						building_list_iterator->getUnit()->useTech(BWAPI::TechTypes::Scanner_Sweep, (BWAPI::Position)base_to_scan->getArea()->Top());
						base_to_scan->toggleSecondaryScouted();
						game_state.setLastScan(BWAPI::Broodwar->elapsedTime());
					}
					else
					{
						game_state.resetSecondaryScouting();
					}
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Factory &&
					building_list_iterator->getUnit()->getAddon() == nullptr &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= BWAPI::UnitTypes::Terran_Machine_Shop.mineralPrice() &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= BWAPI::UnitTypes::Terran_Machine_Shop.gasPrice())
				{
					building_list_iterator->getUnit()->buildAddon(BWAPI::UnitTypes::Terran_Machine_Shop);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Machine_Shop &&
					!building_list_iterator->getUnit()->isResearching() &&
					!BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode) &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= BWAPI::UnitTypes::Terran_Machine_Shop.mineralPrice() &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= BWAPI::UnitTypes::Terran_Machine_Shop.gasPrice())
				{
					if (building_list_iterator->getUnit()->research(BWAPI::TechTypes::Tank_Siege_Mode))
						game_state.toggleBuildTanks();
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Factory &&
					!building_list_iterator->getUnit()->isTraining() &&
					building_list_iterator->getUnit()->isIdle() &&
					game_state.getUnitTypeCount(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) + game_state.getUnitTypeCount(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode) < 10 &&
					game_state.getBuildTanks() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode.mineralPrice() &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode.gasPrice())
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
					building_list_iterator++;
				}
				else
				{
					building_list_iterator++;
				}
			}
		}
		else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
		{
			if (game_state.getBuildOrder() == BuildOrder::P2Gate1)
			{
				if (building_list_iterator->getUnit() == nullptr)
				{
					auto erase_iterator = building_list_iterator;
					building_list_iterator = game_state.getBuildingList()->erase(erase_iterator);
				}
				else if (!building_list_iterator->getUnit()->exists())
				{
					auto erase_iterator = building_list_iterator;
					building_list_iterator = game_state.getBuildingList()->erase(erase_iterator);
				}
				else if ((building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Nexus &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal() &&
					building_list_iterator->getUnit()->isIdle()) &&
					(game_state.getSupplyUsed() < 8 ||
					(game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Pylon) == 1 &&
					game_state.getSupplyUsed() < 9) ||
					(game_state.getBarracks() == 1 &&
					game_state.getSupplyUsed() < 10) ||
					(game_state.getBarracks() == 2 &&
					game_state.getMineralWorkerCount() < 15)))
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Probe);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Gateway &&
building_list_iterator->getUnit()->isIdle() &&
BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
game_state.getSupplyUsed() < game_state.getSupplyTotal() - 2)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Zealot);
					building_list_iterator++;
				}
				else
					building_list_iterator++;
			}
			else if (game_state.getBuildOrder() == BuildOrder::P4GateGoonOpening)
			{
				if ((building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Nexus &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal() &&
					building_list_iterator->getUnit()->isIdle()) &&
					(game_state.getSupplyBuilt() < 8 ||
					(game_state.getPylon() == 1 &&
					game_state.getSupplyBuilt() < 10) ||
					(game_state.getBarracks() == 1 &&
					game_state.getSupplyBuilt() < 12) ||
					(game_state.getPylon() == 2 &&
					game_state.getSupplyBuilt() < 16) ||
					(game_state.getGas() == 1 &&
					game_state.getSupplyBuilt() < 17) ||
					(game_state.checkCyberCore() &&
					game_state.getSupplyBuilt() < 27)))
				{
					if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Probe))
						game_state.addSupplyBuilt(1);
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Gateway &&
					building_list_iterator->getUnit()->isIdle())
				{
					if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Dragoon.mineralPrice() &&
						BWAPI::Broodwar->self()->gas() - game_state.getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Dragoon.gasPrice() &&
						game_state.getSupplyUsed() <= game_state.getSupplyTotal() - 2 &&
						(game_state.getSupplyBuilt() < 31 ||
						game_state.getBarracks() == 4))
					{
						if(building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Dragoon))
							game_state.addSupplyBuilt(2);
					}
					else if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Zealot.mineralPrice() &&
						(game_state.getUnitTypeCount(BWAPI::UnitTypes::Protoss_Zealot) < 1 ||
						(game_state.getUnitTypeCount(BWAPI::UnitTypes::Protoss_Zealot) < 2 &&
						game_state.checkCyberCore())) &&
						game_state.getSupplyUsed() <= game_state.getSupplyTotal() - 2)
					{
						if (building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Zealot))
							game_state.addSupplyBuilt(2);
					}
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core &&
					building_list_iterator->getUnit()->isIdle())
				{
					if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= BWAPI::UpgradeTypes::Singularity_Charge.mineralPrice() &&
						BWAPI::Broodwar->self()->gas() - game_state.getMineralsCommitted() >= BWAPI::UpgradeTypes::Singularity_Charge.gasPrice() &&
						game_state.getSupplyBuilt() >= 26)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Singularity_Charge);
					}
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Assimilator &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getNumberGasWorkers() < 3)
				{
					auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
					{
						if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
						{
							building_list_iterator->addGasWorker(mineral_worker_iterator->getUnit()->getID());
							mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
							game_state.getMineralWorkers()->erase(mineral_worker_iterator);
							mineral_worker_iterator = game_state.getMineralWorkers()->end();
						}
						else
						{
							mineral_worker_iterator++;
						}
					}
				}
				building_list_iterator++;
			}
			else if (game_state.getBuildOrder() == BuildOrder::P4GateGoonMid)
			{
				if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Gateway &&
					building_list_iterator->getUnit()->isIdle())
				{
					if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Dragoon.mineralPrice() &&
						BWAPI::Broodwar->self()->gas() - game_state.getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Dragoon.gasPrice() &&
						game_state.getSupplyUsed() <= game_state.getSupplyTotal() - 2 &&
						game_state.getSupplyUsed() < 40)
					{
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Dragoon);
					}
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Assimilator &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getNumberGasWorkers() < 3)
				{
					auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
					{
						if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
						{
							building_list_iterator->addGasWorker(mineral_worker_iterator->getUnit()->getID());
							mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
							game_state.getMineralWorkers()->erase(mineral_worker_iterator);
							mineral_worker_iterator = game_state.getMineralWorkers()->end();
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
					if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) == 0 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Weapons);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Armor) == 0 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Armor) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Armor);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) == 0 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 200 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 200)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Plasma_Shields);
					}
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Nexus &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal() &&
					game_state.getMineralWorkerCount() < 80)
				{
					int worker_count = 0;
					auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
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
			else if (game_state.getBuildOrder() == BuildOrder::P4GateGoonLate)
			{
				if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Gateway &&
					building_list_iterator->getUnit()->isIdle())
				{
					if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Dragoon.mineralPrice() &&
						BWAPI::Broodwar->self()->gas() - game_state.getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Dragoon.gasPrice() &&
						game_state.getUnitTypeCount(BWAPI::UnitTypes::Protoss_Dragoon) < 3 * game_state.getUnitTypeCount(BWAPI::UnitTypes::Protoss_Zealot) &&
						game_state.getSupplyUsed() <= game_state.getSupplyTotal() - 2)
					{
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Dragoon);
					}
					else if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Zealot.mineralPrice() &&
						game_state.getSupplyUsed() <= game_state.getSupplyTotal() - 2)
					{
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Zealot);
					}
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Forge &&
					building_list_iterator->getUnit()->isIdle())
				{
					if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) == 0 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Weapons);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Armor) == 0 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Armor) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Armor);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) == 0 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 200 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 200)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Plasma_Shields);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) == 1 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) &&
						game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 150)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Weapons);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Armor) == 1 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Armor) &&
						game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 175 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 175)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Armor);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) == 1 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 300 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 300)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Plasma_Shields);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) == 2 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) &&
						game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 200 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 200)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Weapons);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Armor) == 2 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Armor) &&
						game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 250 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 250)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Armor);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) == 2 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 400 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 400)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Plasma_Shields);
					}
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun &&
					building_list_iterator->getUnit()->isIdle() &&
					BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Leg_Enhancements) == 0 &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 150)
				{
					building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Leg_Enhancements);
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Robotics_Facility &&
					BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Observer, 0) &&
					!building_list_iterator->getUnit()->isTraining() &&
					game_state.getDetectorCount() < 5 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 25 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 75)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Observer);
					game_state.addSupplyUsed(1);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Assimilator &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getNumberGasWorkers() < 3)
				{
					auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
					{
						if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
						{
							building_list_iterator->addGasWorker(mineral_worker_iterator->getUnit()->getID());
							mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
							game_state.getMineralWorkers()->erase(mineral_worker_iterator);
							mineral_worker_iterator = game_state.getMineralWorkers()->end();
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
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal() &&
					game_state.getMineralWorkerCount() < 80)
				{
					int worker_count = 0;
					auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
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
			else
			{
				if (building_list_iterator->getUnit() == nullptr)
				{
					auto erase_iterator = building_list_iterator;
					building_list_iterator = game_state.getBuildingList()->erase(erase_iterator);
				}
				else if (!building_list_iterator->getUnit()->exists())
				{
					auto erase_iterator = building_list_iterator;
					building_list_iterator = game_state.getBuildingList()->erase(erase_iterator);
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Nexus &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal())
				{
					int worker_count = 0;
					auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
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
					BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_High_Templar, 0) &&
					BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Psionic_Storm) &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					(double)game_state.getUnitTypeCount(BWAPI::UnitTypes::Protoss_High_Templar) / (double)game_state.getMilitary()->size() < 0.1 &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 150 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal() - 2)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_High_Templar);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Gateway &&
					BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Dragoon, 0) &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 125 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 50 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal() - 2)
				{
					if (game_state.getUnitTypeCount(BWAPI::UnitTypes::Protoss_Dragoon) == 0)
					{
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Dragoon);
					}
					else if (game_state.getUnitTypeCount(BWAPI::UnitTypes::Protoss_Zealot) / game_state.getUnitTypeCount(BWAPI::UnitTypes::Protoss_Dragoon) < 3)
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
					!BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Dragoon, 0) &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal() - 2)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Zealot);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Gateway &&
					BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Dragoon, 0) &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isTraining() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() < 50 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal() - 2)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Zealot);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Templar_Archives &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isResearching() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 200 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 200)
				{
					building_list_iterator->getUnit()->research(BWAPI::TechTypes::Psionic_Storm);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Forge &&
					building_list_iterator->getUnit()->isIdle() &&
					!building_list_iterator->getUnit()->isUpgrading())
				{
					if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) == 0 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Weapons);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Armor) == 0 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Armor) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Armor);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) == 0 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 200 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 200)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Plasma_Shields);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) == 1 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) &&
						game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 150)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Weapons);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Armor) == 1 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Armor) &&
						game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 175 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 175)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Armor);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) == 1 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 300 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 300)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Plasma_Shields);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) == 2 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Weapons) &&
						game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 200 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 200)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Weapons);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Armor) == 2 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Ground_Armor) &&
						game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 250 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 250)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Ground_Armor);
					}
					else if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) == 2 &&
						!BWAPI::Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Protoss_Plasma_Shields) &&
						BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 400 &&
						BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 400)
					{
						building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Protoss_Plasma_Shields);
					}

					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core &&
					!building_list_iterator->getUnit()->isUpgrading() &&
					BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Singularity_Charge) == 0 &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 150)
				{
					building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Singularity_Charge);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun &&
					!building_list_iterator->getUnit()->isUpgrading() &&
					BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Leg_Enhancements) == 0 &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 150)
				{
					building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Leg_Enhancements);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Robotics_Facility &&
					BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Observer, 0) &&
					!building_list_iterator->getUnit()->isTraining() &&
					game_state.getDetectorCount() < 5 &&
					game_state.getSupplyUsed() < game_state.getSupplyTotal() &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 25 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 75)
				{
					building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Observer);
					game_state.addSupplyUsed(1);
					building_list_iterator++;
				}
				else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Assimilator &&
					building_list_iterator->getUnit()->isCompleted() &&
					building_list_iterator->getNumberGasWorkers() < 3)
				{
					auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
					while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
					{
						if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
						{
							building_list_iterator->addGasWorker(mineral_worker_iterator->getUnit()->getID());
							mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
							game_state.getMineralWorkers()->erase(mineral_worker_iterator);
							mineral_worker_iterator = game_state.getMineralWorkers()->end();
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
		else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
		{
			if (building_list_iterator->getUnit() == nullptr)
			{
				auto erase_iterator = building_list_iterator;
				building_list_iterator = game_state.getBuildingList()->erase(erase_iterator);
			}
			else if (!building_list_iterator->getUnit()->exists())
			{
				auto erase_iterator = building_list_iterator;
				building_list_iterator = game_state.getBuildingList()->erase(erase_iterator);
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Hatchery &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0 &&
				(game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Lair) == 0 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hive) == 0) &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
			{
				building_list_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Lair);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Lair &&
				BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Pneumatized_Carapace) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 150)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Pneumatized_Carapace);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Metabolic_Boost) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Metabolic_Boost);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Muscular_Augments) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 150)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Muscular_Augments);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Grooved_Spines) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 150)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Grooved_Spines);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Melee_Attacks) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Zerg_Melee_Attacks);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Missile_Attacks) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Zerg_Missile_Attacks);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Carapace) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 150)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Zerg_Carapace);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Melee_Attacks) == 1 &&
				(game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Lair) >= 1 ||
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hive) >= 1) &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 150)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Zerg_Melee_Attacks);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber &&
				!building_list_iterator->getUnit()->isUpgrading() &&
				BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Missile_Attacks) == 1 &&
				(game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Lair) >= 1 ||
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hive) >= 1) &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 150)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Zerg_Missile_Attacks);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber &&
				BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Zerg_Carapace) == 1 &&
				(game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Lair) >= 1 ||
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hive) >= 1) &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 225 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 225)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Zerg_Carapace);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Creep_Colony &&
				BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Sunken_Colony, 0) &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50)
			{
				building_list_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Sunken_Colony);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Extractor &&
				building_list_iterator->getUnit()->isCompleted() &&
				building_list_iterator->getNumberGasWorkers() < 3)
			{
				auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
				while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
				{
					if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
					{
						building_list_iterator->addGasWorker(mineral_worker_iterator->getUnit()->getID());
						mineral_worker_iterator->getUnit()->gather(building_list_iterator->getUnit());
						game_state.getMineralWorkers()->erase(mineral_worker_iterator);
						mineral_worker_iterator = game_state.getMineralWorkers()->end();
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

	if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
	{
		auto larva_iterator = game_state.getLarva()->begin();
		while (larva_iterator != game_state.getLarva()->end())
		{
			if (((game_state.getSupplyExpected() < 20 && 
				game_state.getSupplyUsed() > game_state.getSupplyExpected() - 2) ||
				(game_state.getSupplyExpected() >= 20 &&
				game_state.getSupplyUsed() > game_state.getSupplyExpected() - 10)) &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100)
			{
				if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Overlord))
				{
					auto erase_iterator = larva_iterator;
					larva_iterator = game_state.getLarva()->erase(erase_iterator);
					game_state.addSupplyExpected(8);
				}
				else
				{
					larva_iterator++;
				}
			}
			else if (game_state.getSupplyUsed() < game_state.getSupplyTotal() &&
				game_state.getMineralWorkerCount() < 30 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
				!BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Hydralisk, 0) &&
				BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Zergling, 0))
			{
				int decider = rand() % 2 + 1;
				if (decider == 1)
				{
					if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Drone))
					{
						auto erase_iterator = larva_iterator;
						larva_iterator = game_state.getLarva()->erase(erase_iterator);
						game_state.addSupplyUsed(1);
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
						larva_iterator = game_state.getLarva()->erase(erase_iterator);
						game_state.addSupplyUsed(1);
					}
					else
					{
						larva_iterator++;
					}
				}
			}
			else if (game_state.getSupplyUsed() < game_state.getSupplyTotal() &&
				game_state.getMineralWorkerCount() < 30 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 75 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 25 &&
				BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Hydralisk, 0))
			{
				int decider = rand() % 2 + 1;
				
				if (decider == 2 &&
					BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 75 &&
					BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 25)
				{
					if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Hydralisk))
					{
						auto erase_iterator = larva_iterator;
						larva_iterator = game_state.getLarva()->erase(erase_iterator);
						game_state.addSupplyUsed(1);
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
						larva_iterator = game_state.getLarva()->erase(erase_iterator);
						game_state.addSupplyUsed(1);
					}
					else
					{
						larva_iterator++;
					}
				}
			}
			else if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 75 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 30 &&
				BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Hydralisk))
			{
				if (game_state.getUnitTypeCount(BWAPI::UnitTypes::Zerg_Zergling) / game_state.getUnitTypeCount(BWAPI::UnitTypes::Zerg_Hydralisk) > 2)
				{
					if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Hydralisk))
					{
						auto erase_iterator = larva_iterator;
						larva_iterator = game_state.getLarva()->erase(erase_iterator);
						game_state.addSupplyUsed(1);
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
						larva_iterator = game_state.getLarva()->erase(erase_iterator);
						game_state.addSupplyUsed(1);
					}
					else
					{
						larva_iterator++;
					}
				}
			}
			else if (game_state.getSupplyUsed() < game_state.getSupplyTotal() &&
				game_state.getMineralWorkerCount() < 30 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50)
			{
				if (larva_iterator->getUnit()->morph(BWAPI::UnitTypes::Zerg_Drone))
				{
					auto erase_iterator = larva_iterator;
					larva_iterator = game_state.getLarva()->erase(erase_iterator);
					game_state.addSupplyUsed(1);
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

	if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
	{
			if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 400 &&
				!game_state.getExpanding() &&
				BWAPI::Broodwar->elapsedTime() - game_state.getLastTimeExpanded() >= 105)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Command_Center, 1, game_state))
				{
					game_state.addMineralsCommitted(400);
					game_state.toggleExpanding();
					game_state.setLastTimeExpanded();
				}
			}
			if (game_state.getBarracks() > 1 &&
				game_state.getGas() > 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
				!game_state.checkAcademy())
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Academy, 3, game_state))
				{
					game_state.addMineralsCommitted(150);
					game_state.toggleAcademy();
				}
			}
			if ((game_state.getBarracks() == 0 && game_state.getSupplyUsed() >= game_state.getSupplyExpected() - 2 ||
				game_state.getBarracks() >= 1 && game_state.getSupplyUsed() >= game_state.getSupplyExpected() - 10) &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Supply_Depot, 3, game_state))
				{
					game_state.addSupplyExpected(8);
					game_state.addMineralsCommitted(100);
				}
			}
			if ((game_state.getBarracks() < 2 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150) ||
				(game_state.getBarracks() >= 2 &&
				game_state.getFactory() >= 1 &&
				game_state.getBarracks() < 3 * game_state.getBuildingTypeCount(BWAPI::UnitTypes::Terran_Command_Center) - 1&&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 + 50 * game_state.getBarracks()))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Barracks, 3, game_state))
				{
					game_state.addMineralsCommitted(150);
					game_state.addBarracks(1);
				}
			}
			if (game_state.getBarracks() > 1 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
				game_state.getGas() < game_state.getBuildingTypeCount(BWAPI::UnitTypes::Terran_Command_Center))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Refinery, 3, game_state))
				{
					game_state.addMineralsCommitted(100);
					game_state.addGas(1);
				}
				else if (worker_manager->build(BWAPI::UnitTypes::Terran_Refinery, 4, game_state))
				{
					game_state.addMineralsCommitted(100);
					game_state.addGas(1);
				}
			}
			if (game_state.getBuildingTypeCount(BWAPI::UnitTypes::Terran_Barracks) >= 2 &&
				game_state.getFactory() < 1 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= BWAPI::UnitTypes::Terran_Factory.mineralPrice() &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= BWAPI::UnitTypes::Terran_Factory.gasPrice())
			{
				if (worker_manager->build(BWAPI::UnitTypes::Terran_Factory, 3, game_state))
				{
					game_state.addMineralsCommitted(BWAPI::UnitTypes::Terran_Factory.mineralPrice());
					game_state.addGasCommitted(BWAPI::UnitTypes::Terran_Factory.gasPrice());
					game_state.addFactory(1);
				}
			}
	}
	else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
	{
		if (game_state.getBuildOrder() == BuildOrder::P2Gate1)
		{
			if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
				(game_state.getSupplyUsed() == 8 ||
				(game_state.getBarracks() == 2 &&
				game_state.getSupplyUsed() >= game_state.getSupplyExpected() - 10)))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Pylon, 3, game_state))
				{
					game_state.addSupplyExpected(8);
					game_state.addMineralsCommitted(100);
				}
			}
			if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Gateway) < 2 &&
				(game_state.getSupplyUsed() == 9 ||
				game_state.getSupplyUsed() == 10))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Gateway, 3, game_state))
				{
					game_state.addMineralsCommitted(150);
					game_state.addBarracks(1);
				}
			}
		}
		else if (game_state.getBuildOrder() == BuildOrder::P4GateGoonOpening)
		{
			if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
				((game_state.getSupplyBuilt() == 8 &&
				game_state.getPylon() < 1) ||
				(game_state.getSupplyBuilt() == 12 &&
				game_state.getPylon() < 2) ||
				(game_state.getSupplyBuilt() == 22 &&
				game_state.getPylon() < 3) ||
				(game_state.getSupplyBuilt() == 33 &&
				game_state.getPylon() < 4) ||
				(game_state.getSupplyBuilt() == 41 &&
				game_state.getPylon() < 5)))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Pylon, 3, game_state))
				{
					game_state.addSupplyExpected(8);
					game_state.addMineralsCommitted(100);
					game_state.addPylon(1);
				}
			}
			if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
				((game_state.getSupplyBuilt() == 10 &&
				game_state.getBarracks() == 0) ||
				(game_state.getSupplyBuilt() >= 31 &&
				game_state.getBarracks() < 4)))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Gateway, 3, game_state))
				{
					game_state.addBarracks(1);
					game_state.addMineralsCommitted(150);
				}
			}
			if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
				game_state.getGas() == 0 &&
				game_state.getSupplyBuilt() >= 16)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 3, game_state))
				{
					game_state.addGas(1);
					game_state.addMineralsCommitted(100);
				}
			}
			if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= BWAPI::UnitTypes::Protoss_Cybernetics_Core.mineralPrice() &&
				!game_state.checkCyberCore() &&
				game_state.getSupplyBuilt() >= 17)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Cybernetics_Core, 3, game_state))
				{
					game_state.toggleCyberCore();
					game_state.addMineralsCommitted(BWAPI::UnitTypes::Protoss_Cybernetics_Core.mineralPrice());
				}
			}
		}
		else if (game_state.getBuildOrder() == BuildOrder::P4GateGoonMid)
		{
			if ((BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 400 &&
				!game_state.getExpanding() &&
				BWAPI::Broodwar->elapsedTime() - game_state.getLastTimeExpanded() >= 105 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) == 1) ||
				(BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 400 &&
				!game_state.getExpanding() &&
				BWAPI::Broodwar->elapsedTime() - game_state.getLastTimeExpanded() >= 105 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) == 2 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Observatory) >= 1))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Nexus, 1, game_state))
				{
					game_state.addMineralsCommitted(400);
					game_state.toggleExpanding();
					game_state.setLastTimeExpanded();
				}
			}
			if ((game_state.getBarracks() == 0 && game_state.getSupplyUsed() >= game_state.getSupplyExpected() - 2 ||
				game_state.getBarracks() >= 1 && game_state.getSupplyUsed() >= game_state.getSupplyExpected() - 10) &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Pylon, 3, game_state))
				{
					game_state.addSupplyExpected(8);
					game_state.addMineralsCommitted(100);
				}
			}
			if (game_state.getBarracks() >= 2 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Cybernetics_Core, 3, game_state))
				{
					game_state.addMineralsCommitted(200);
				}
			}
			if (game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				game_state.getForge() == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Forge, 3, game_state))
				{
					game_state.addMineralsCommitted(150);
					game_state.addForge(1);
				}
			}
			if (((game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core) > 0 &&
				game_state.getGas() == 0) ||
				(game_state.getGas() >= 1 &&
				game_state.getGas() < game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus))) &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 3, game_state))
				{
					game_state.addMineralsCommitted(100);
					game_state.addGas(1);
				}
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 4, game_state))
				{
					game_state.addMineralsCommitted(100);
					game_state.addGas(1);
				}
			}
			if (game_state.getRoboticsFacility() == 0 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) == 0 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 200 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Robotics_Facility, 3, game_state))
				{
					game_state.addMineralsCommitted(200);
					game_state.addGasCommitted(200);
					game_state.addRoboticsFacility(1);
				}
			}
			if (game_state.getObservatory() == 0 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Observatory) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Observatory, 3, game_state))
				{
					game_state.addMineralsCommitted(50);
					game_state.addGasCommitted(100);
					game_state.addObservatory(1);
				}
			}
			if (game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				game_state.getForge() == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Forge, 3, game_state))
				{
					game_state.addMineralsCommitted(150);
					game_state.addForge(1);
				}
			}
		}
		else if (game_state.getBuildOrder() == BuildOrder::P4GateGoonLate)
		{
			if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 400 &&
				!game_state.getExpanding() &&
				BWAPI::Broodwar->elapsedTime() - game_state.getLastTimeExpanded() >= 105)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Nexus, 1, game_state))
				{
					game_state.addMineralsCommitted(400);
					game_state.toggleExpanding();
					game_state.setLastTimeExpanded();
				}
			}
			if ((game_state.getBarracks() == 0 && game_state.getSupplyUsed() >= game_state.getSupplyExpected() - 2 ||
				game_state.getBarracks() >= 1 && game_state.getSupplyUsed() >= game_state.getSupplyExpected() - 10) &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Pylon, 3, game_state))
				{
					game_state.addSupplyExpected(8);
					game_state.addMineralsCommitted(100);
				}
			}
			if (game_state.getBarracks() >= 2 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Cybernetics_Core, 3, game_state))
				{
					game_state.addMineralsCommitted(200);
				}
			}
			if (game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				game_state.getForge() == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Forge, 3, game_state))
				{
					game_state.addMineralsCommitted(150);
					game_state.addForge(1);
				}
			}
			if (((game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core) > 0 &&
				game_state.getGas() == 0) ||
				(game_state.getGas() >= 1 &&
				game_state.getGas() < game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus))) &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 3, game_state))
				{
					game_state.addMineralsCommitted(100);
					game_state.addGas(1);
				}
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 4, game_state))
				{
					game_state.addMineralsCommitted(100);
					game_state.addGas(1);
				}
			}
			if (game_state.getRoboticsFacility() == 0 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) == 0 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 200 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Robotics_Facility, 3, game_state))
				{
					game_state.addMineralsCommitted(200);
					game_state.addGasCommitted(200);
					game_state.addRoboticsFacility(1);
				}
			}
			if (game_state.getObservatory() == 0 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Observatory) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Observatory, 3, game_state))
				{
					game_state.addMineralsCommitted(50);
					game_state.addGasCommitted(100);
					game_state.addObservatory(1);
				}
			}
			if (game_state.getObservatory() > 0 &&
				game_state.getCitadelofAdun() == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Citadel_of_Adun, 3, game_state))
				{
					game_state.addMineralsCommitted(150);
					game_state.addGasCommitted(100);
					game_state.addCitadelofAdun(1);
				}
			}
			if (game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				game_state.getForge() == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Forge, 3, game_state))
				{
					game_state.addMineralsCommitted(150);
					game_state.addForge(1);
				}
			}
		}
		else if (game_state.getBuildOrder() == BuildOrder::PForgeFastExpand9poolOpening)
		{
			if (game_state.getSupplyBuilt() == 8)
			{

			}
		}
		else
		{
			if (BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 400 &&
				!game_state.getExpanding() &&
				BWAPI::Broodwar->elapsedTime() - game_state.getLastTimeExpanded() >= 105)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Nexus, 1, game_state))
				{
					game_state.addMineralsCommitted(400);
					game_state.toggleExpanding();
					game_state.setLastTimeExpanded();
				}
			}
			if ((game_state.getBarracks() == 0 && game_state.getSupplyUsed() >= game_state.getSupplyExpected() - 2 ||
				game_state.getBarracks() >= 1 && game_state.getSupplyUsed() >= game_state.getSupplyExpected() - 10) &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Pylon, 3, game_state))
				{
					game_state.addSupplyExpected(8);
					game_state.addMineralsCommitted(100);
				}
			}
			if (game_state.getBarracks() >= 2 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Cybernetics_Core, 3, game_state))
				{
					game_state.addMineralsCommitted(200);
				}
			}
			if (game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Forge) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Forge, 3, game_state))
				{
					game_state.addMineralsCommitted(150);
				}
			}
			if (((game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core) > 0 &&
				game_state.getGas() == 0) ||
				(game_state.getGas() >= 1 &&
				game_state.getGas() < game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus))) &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 3, game_state))
				{
					game_state.addMineralsCommitted(100);
					game_state.addGas(1);
				}
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 4, game_state))
				{
					game_state.addMineralsCommitted(100);
					game_state.addGas(1);
				}
			}
			if (game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Observatory) > 0 &&
				game_state.getGas() == 1 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Assimilator, 4, game_state))
				{
					game_state.addMineralsCommitted(100);
					game_state.addGas(1);
				}
			}
			if (BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Robotics_Facility, 0) &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) == 0 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 200 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Robotics_Facility, 3, game_state))
				{
					game_state.addMineralsCommitted(200);
					game_state.addGasCommitted(200);
				}
			}
			if (BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Observatory, 0) &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Observatory) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Observatory, 3, game_state))
				{
					game_state.addMineralsCommitted(50);
					game_state.addGasCommitted(100);
				}
			}
			if (game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Observatory) > 0 &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Citadel_of_Adun) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 100)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Citadel_of_Adun, 3, game_state))
				{
					game_state.addMineralsCommitted(150);
					game_state.addGasCommitted(100);
				}
			}
			if (BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Protoss_Templar_Archives, 0) &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Templar_Archives) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 200)
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Templar_Archives, 3, game_state))
				{
					game_state.addMineralsCommitted(150);
					game_state.addGasCommitted(200);
				}
			}
			if ((game_state.getBarracks() < 2 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150) ||
				(game_state.getBarracks() >= 2 &&
				game_state.getBarracks() <= 3 * game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) > 1 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 + 100 * game_state.getBarracks()))
			{
				if (worker_manager->build(BWAPI::UnitTypes::Protoss_Gateway, 3, game_state))
				{
					game_state.addMineralsCommitted(150);
					game_state.addBarracks(1);
				}
			}
		}
	}
	else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
	{
		if (game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hydralisk_Den) < 1 &&
			BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Zerg_Hydralisk_Den, 0) &&
			BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
			BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 50)
		{
			if (worker_manager->build(BWAPI::UnitTypes::Zerg_Hydralisk_Den, 3, game_state))
			{
				game_state.addMineralsCommitted(150);
				game_state.addGasCommitted(50);
			}
		}
		if (game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Spawning_Pool) >= 1 &&
			game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Creep_Colony) + game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Sunken_Colony) < 20 &&
			game_state.getMineralWorkerCount() >= 15 &&
			BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 75)
		{
			if (worker_manager->build(BWAPI::UnitTypes::Zerg_Creep_Colony, 3, game_state))
			{
				game_state.addMineralsCommitted(75);
			}
		}
		if ((game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hatchery) < 2 &&
			BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 300) ||
			(BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() - game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hatchery) * 50 >= 300 &&
			game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hatchery) < 9))
		{
			if (worker_manager->build(BWAPI::UnitTypes::Zerg_Hatchery, 3, game_state))
			{
				game_state.addSupplyExpected(1);
				game_state.addMineralsCommitted(300);
			}
		}
		if (game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hatchery) > 1 &&
			game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Spawning_Pool) == 0 &&
			BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 200)
		{
			if (worker_manager->build(BWAPI::UnitTypes::Zerg_Spawning_Pool, 3, game_state))
			{
				game_state.addMineralsCommitted(200);
			}
		}
		if (game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Hatchery) > 1 &&
			BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
			game_state.getGas() == 0 &&
			game_state.checkValidGasBuildTileLocation(3))
		{
			if (worker_manager->build(BWAPI::UnitTypes::Zerg_Extractor, 3, game_state))
			{
				game_state.addMineralsCommitted(50);
				game_state.addGas(1);
			}
		}
		if (game_state.getGas() >= 1 &&
			game_state.getEvolutionChambers() < 1 &&
			game_state.getBuildingTypeCount(BWAPI::UnitTypes::Zerg_Lair) >= 1 &&
			BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 75)
		{
			if (worker_manager->build(BWAPI::UnitTypes::Zerg_Evolution_Chamber, 3, game_state))
			{
				game_state.addMineralsCommitted(75);
				game_state.addEvolutionChambers(1);
			}
		}
	}
}

