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
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Command_Center &&
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
				game_state.getMineralWorkerCount() < 25 &&
				game_state.getSupplyUsed() < game_state.getSupplyTotal())
			{
				building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_SCV);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks &&
				!BWAPI::Broodwar->self()->hasUnitTypeRequirement(BWAPI::UnitTypes::Terran_Medic, 0) &&
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
						building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Medic);
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
		else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
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
				game_state.getMineralWorkerCount() < 30 &&
				game_state.getSupplyUsed() < game_state.getSupplyTotal())
			{
				building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Probe);
				building_list_iterator++;
			}
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Gateway &&
				building_list_iterator->getUnit()->isIdle() &&
				!building_list_iterator->getUnit()->isTraining() &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
				game_state.getSupplyUsed() < game_state.getSupplyTotal() - 2)
			{
				building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Protoss_Zealot);
				building_list_iterator++;
			}
			else
			{
				building_list_iterator++;
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
			else if (building_list_iterator->getUnit()->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den &&
				BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Muscular_Augments) == 0 &&
				BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 &&
				BWAPI::Broodwar->self()->gas() - game_state.getGasCommitted() >= 150)
			{
				building_list_iterator->getUnit()->upgrade(BWAPI::UpgradeTypes::Muscular_Augments);
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
		if (game_state.getBarracks() > 0 &&
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
			game_state.getBarracks() < 8 &&
			BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 + 50 * game_state.getBarracks()))
		{
			if (worker_manager->build(BWAPI::UnitTypes::Terran_Barracks, 3, game_state))
			{
				game_state.addMineralsCommitted(150);
				game_state.addBarracks(1);
			}
		}
		if (game_state.getBarracks() > 0 &&
			BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 100 &&
			game_state.getGas() == 0 &&
			game_state.checkValidGasBuildTileLocation())
		{
			if (worker_manager->build(BWAPI::UnitTypes::Terran_Refinery, 3, game_state))
			{
				game_state.addMineralsCommitted(100);
				game_state.addGas(1);
			}
		}
	}
	else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
	{
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
		if ((game_state.getBarracks() < 2 &&
			BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150) ||
			(game_state.getBarracks() >= 2 &&
			game_state.getBarracks() < 8 &&
			BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 150 + 100 * game_state.getBarracks()))
		{
			if (worker_manager->build(BWAPI::UnitTypes::Protoss_Gateway, 3, game_state))
			{
				game_state.addMineralsCommitted(150);
				game_state.addBarracks(1);
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
			game_state.checkValidGasBuildTileLocation())
		{
			if (worker_manager->build(BWAPI::UnitTypes::Zerg_Extractor, 3, game_state))
			{
				game_state.addMineralsCommitted(50);
				game_state.addGas(1);
			}
		}
	}
}

