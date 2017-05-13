#include"MacroManager.h"

MacroManager::MacroManager()
{
	
}

void MacroManager::checkMacro(WorkerManager* worker_manager, GameState &game_state)
{
	auto building_list_iterator = game_state.getBuildingList()->begin();
	while (building_list_iterator != game_state.getBuildingList()->end())
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
			building_list_iterator->getUnit()->isIdle() &&
			!building_list_iterator->getUnit()->isTraining() &&
			BWAPI::Broodwar->self()->minerals() - game_state.getMineralsCommitted() >= 50 &&
			game_state.getSupplyUsed() < game_state.getSupplyTotal())
		{
			building_list_iterator->getUnit()->train(BWAPI::UnitTypes::Terran_Marine);
			building_list_iterator++;
		}
		else
		{
			building_list_iterator++;
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
}

