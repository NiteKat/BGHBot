#include"WorkerManager.h"

int WorkerManager::manageWorkers(GameState &game_state)
{
	int minerals_committed_adjust = 0;
	auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
	while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
	{
		if (mineral_worker_iterator->getUnit() == nullptr)
		{
			auto erase_iterator = mineral_worker_iterator;
			mineral_worker_iterator = game_state.getMineralWorkers()->erase(erase_iterator);
		}
		else if (!mineral_worker_iterator->getUnit()->exists())
		{
			auto erase_iterator = mineral_worker_iterator;
			mineral_worker_iterator = game_state.getMineralWorkers()->erase(erase_iterator);
		}
		else
		{
			if (mineral_worker_iterator->getUnit()->isUnderAttack() &&
				mineral_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::AttackUnit)
			{
				for (auto unit : mineral_worker_iterator->getUnit()->getUnitsInRadius(10))
				{
					if (unit->getType() == BWAPI::UnitTypes::Zerg_Zergling)
					{
						mineral_worker_iterator->getUnit()->attack(unit);
					}
				}
				mineral_worker_iterator++;
			}
			else if (mineral_worker_iterator->getUnit()->isIdle())
			{
				if (mineral_worker_iterator->getBase()->getArea()->Minerals().size() > 0)
				{
					mineral_worker_iterator->getUnit()->gather((*mineral_worker_iterator->getBase()->getArea()->Minerals().begin())->Unit());
				}
				mineral_worker_iterator++;
			}
			else
			{
				mineral_worker_iterator++;
			}
		}
	}

	auto build_worker_iterator = game_state.getBuildWorkers()->begin();
	while (build_worker_iterator != game_state.getBuildWorkers()->end())
	{
		if (build_worker_iterator->getUnit() == nullptr)
		{
			auto erase_iterator = build_worker_iterator;
			build_worker_iterator = game_state.getBuildWorkers()->erase(erase_iterator);
		}
		else if (!build_worker_iterator->getUnit()->exists())
		{
			auto erase_iterator = build_worker_iterator;
			build_worker_iterator = game_state.getBuildWorkers()->erase(erase_iterator);
		}
		else if (build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::PlaceBuilding &&
			((build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::ConstructingBuilding &&
			BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran) || 
			(build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::PlayerGuard &&
			BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss) ||
			(build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::IncompleteBuilding &&
			BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)))
		{
			BWAPI::Broodwar << "DEBUG: Order = " << build_worker_iterator->getUnit()->getOrder().toString() << std::endl;
			if (build_worker_iterator->getBuildType() != BWAPI::UnitTypes::Terran_Refinery)
			{
				BWAPI::TilePosition build_position = getBuildLocation(*build_worker_iterator, build_worker_iterator->getBuildType());//BWAPI::Broodwar->getBuildLocation(build_worker_iterator->getBuildType(), (BWAPI::TilePosition)(*build_worker_iterator->getBase()->getArea()->Bases().begin()).Center());
				build_worker_iterator->getUnit()->build(build_worker_iterator->getBuildType(), build_position);
				build_worker_iterator++;
			}
			else
			{
				BWAPI::TilePosition build_position = game_state.getGasBuildTileLocation();
				build_worker_iterator->getUnit()->build(build_worker_iterator->getBuildType(), build_position);
				build_worker_iterator++;
			}
		}
		else if (build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::ConstructingBuilding &&
			BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
		{
			if (build_worker_iterator->getBuildType() != BWAPI::UnitTypes::Terran_Refinery)
			{
				Object new_mineral_worker(*build_worker_iterator);
				new_mineral_worker.setBuildType(BWAPI::UnitTypes::Unknown);
				new_mineral_worker.setBaseClass(0);
				game_state.getMineralWorkers()->push_back(new_mineral_worker);
				auto erase_iterator = build_worker_iterator;
				build_worker_iterator = game_state.getBuildWorkers()->erase(erase_iterator);
			}
			else
			{
				Object new_building(build_worker_iterator->getUnit()->getBuildUnit(), game_state.getContainingBase(build_worker_iterator->getUnit()->getBuildUnit()));
				new_building.addGasWorker(build_worker_iterator->getUnit()->getID());
				game_state.addBuilding(new_building);
				game_state.addMineralsCommitted(-100);
				game_state.setGeyserUsed(build_worker_iterator->getUnit()->getBuildUnit()->getTilePosition());
				auto erase_iterator = build_worker_iterator;
				build_worker_iterator = game_state.getBuildWorkers()->erase(erase_iterator);				
			}
		}
		else if (build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::PlayerGuard &&
			BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
		{
			Object new_mineral_worker(*build_worker_iterator);
			if (new_mineral_worker.getBuildType() == BWAPI::UnitTypes::Protoss_Pylon)
			{
				game_state.addMineralsCommitted(-100);
			}
			else if (new_mineral_worker.getBuildType() == BWAPI::UnitTypes::Protoss_Gateway)
			{
				game_state.addMineralsCommitted(-150);
			}
			else if (new_mineral_worker.getBuildType() == BWAPI::UnitTypes::Protoss_Forge)
			{
				game_state.addMineralsCommitted(-150);
			}
			else if (new_mineral_worker.getBuildType() == BWAPI::UnitTypes::Protoss_Assimilator)
			{
				for (auto unit : new_mineral_worker.getUnit()->getUnitsInRadius(100))
				{
					if (unit->getType() == BWAPI::UnitTypes::Protoss_Assimilator)
					{
						Object new_building(unit, game_state.getContainingBase(unit));
						game_state.addBuilding(new_building);
					}
				}
				
			}
			else if (new_mineral_worker.getBuildType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core)
			{
				game_state.addMineralsCommitted(-200);
			}
			else if (new_mineral_worker.getBuildType() == BWAPI::UnitTypes::Protoss_Robotics_Facility)
			{
				game_state.addMineralsCommitted(-200);
				game_state.addGasCommitted(-200);
			}
			else if (new_mineral_worker.getBuildType() == BWAPI::UnitTypes::Protoss_Observatory)
			{
				game_state.addMineralsCommitted(-50);
				game_state.addGasCommitted(-100);
			}
			new_mineral_worker.setBuildType(BWAPI::UnitTypes::Unknown);
			new_mineral_worker.setBaseClass(0);
			game_state.getMineralWorkers()->push_back(new_mineral_worker);
			auto erase_iterator = build_worker_iterator;
			build_worker_iterator = game_state.getBuildWorkers()->erase(erase_iterator);
		}
		else if (build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::IncompleteBuilding &&
			BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
		{
			auto erase_iterator = build_worker_iterator;
			build_worker_iterator = game_state.getBuildWorkers()->erase(erase_iterator);
		}
		else
		{
			build_worker_iterator++;
		}
	}

	return minerals_committed_adjust;
}

bool WorkerManager::build(BWAPI::UnitType building_type, int base_class, GameState &game_state)
{
	auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
	while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
	{
		if (mineral_worker_iterator->getBase()->getBaseClass() == base_class &&
			!mineral_worker_iterator->getUnit()->isCarryingMinerals() &&
			BWAPI::Broodwar->canMake(building_type, mineral_worker_iterator->getUnit()))
		{
			Object new_build_worker(*mineral_worker_iterator);
			if (building_type != BWAPI::UnitTypes::Terran_Refinery)
			{
				BWAPI::TilePosition build_position = this->getBuildLocation(*mineral_worker_iterator, building_type);
				//BWAPI::TilePosition build_position = BWAPI::Broodwar->getBuildLocation(building_type, (BWAPI::TilePosition)(*new_build_worker.getBase()->getArea()->Bases().begin()).Center());
				if (build_position != BWAPI::TilePositions::Invalid)
				{
					if (new_build_worker.getUnit()->build(building_type, build_position))
					{
						new_build_worker.setBuildType(building_type);
						new_build_worker.setBaseClass(base_class);
						game_state.getBuildWorkers()->push_back(new_build_worker);
						game_state.getMineralWorkers()->erase(mineral_worker_iterator);
						return true;
					}
					else
						mineral_worker_iterator++;
				}
				else
				{
					mineral_worker_iterator++;
				}
			}
			else
			{
				if (game_state.checkValidGasBuildTileLocation())
				{
					BWAPI::TilePosition build_position = game_state.getGasBuildTileLocation();
					if (build_position != BWAPI::TilePositions::Invalid)
					{
						if (new_build_worker.getUnit()->build(building_type, build_position))
						{
							new_build_worker.setBuildType(building_type);
							new_build_worker.setBaseClass(base_class);
							game_state.getBuildWorkers()->push_back(new_build_worker);
							game_state.getMineralWorkers()->erase(mineral_worker_iterator);
							return true;
						}
						else
							return false;
					}
					else
						return false;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			mineral_worker_iterator++;
		}
	}
	return false;
}

BWAPI::TilePosition WorkerManager::getBuildLocation(Object build_worker, BWAPI::UnitType building_type)
{
	if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
	{
		BWAPI::TilePosition position_to_try = build_worker.getBase()->getArea()->Bases().begin()->Location();
		bool too_close = false;
		while (true)
		{
			position_to_try.x += rand() % 7 - 3;
			if (position_to_try.x < 0)
			{
				position_to_try.x = 0;
			}
			if (position_to_try.x > BWAPI::Broodwar->mapWidth())
			{
				position_to_try.x = BWAPI::Broodwar->mapWidth();
			}
			position_to_try.y += rand() % 7 - 3;
			if (position_to_try.y < 0)
			{
				position_to_try.y = 0;
			}
			if (position_to_try.y > BWAPI::Broodwar->mapHeight())
			{
				position_to_try.y = BWAPI::Broodwar->mapHeight();
			}
			if (BWAPI::Broodwar->canBuildHere(position_to_try, building_type))
			{
				for (auto unit : BWAPI::Broodwar->getUnitsInRadius((BWAPI::Position)position_to_try, 128))
				{
					if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field ||
						unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field_Type_2 ||
						unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field_Type_3 ||
						unit->getType() == BWAPI::UnitTypes::Terran_Command_Center) &&
						unit->getTilePosition().getDistance(position_to_try) <= 128)
					{
						too_close = true;
					}
				}
				if (too_close == false)
					return position_to_try;
				else
					too_close = false;
			}
		}
	}
	/*else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
	{
		if (building_type == BWAPI::UnitTypes::Zerg_Creep_Colony &&
			)
		{
			BWAPI::TilePosition position_to_try;
			position_to_try.x = (*BWEM::Map::Instance().GetNearestArea(build_worker.getUnit()->getTilePosition())->ChokePoints().begin())->Center().x;
			position_to_try.y = (*BWEM::Map::Instance().GetNearestArea(build_worker.getUnit()->getTilePosition())->ChokePoints().begin())->Center().y;
			while (true)
			{

			}
		}
	}*/
	else
	{
		return BWAPI::Broodwar->getBuildLocation(building_type, build_worker.getUnit()->getTilePosition());
	}
}