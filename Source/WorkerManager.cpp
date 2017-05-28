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
			build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::Move &&
			((build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::ConstructingBuilding &&
			BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran) || 
			(build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::PlayerGuard &&
			BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss) ||
			(build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::IncompleteBuilding &&
			BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)))
		{
			if (build_worker_iterator->getBuildType() != BWAPI::UnitTypes::Terran_Refinery)
			{
				BWAPI::TilePosition build_position = getBuildLocation(*build_worker_iterator, build_worker_iterator->getBuildType(), game_state);
				if (BWAPI::Broodwar->canBuildHere(build_position, build_worker_iterator->getBuildType(), build_worker_iterator->getUnit()))
				{
					build_worker_iterator->getUnit()->build(build_worker_iterator->getBuildType(), build_position);
					build_worker_iterator++;
				}
				else
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
						game_state.setGeyserUsed(new_building.getUnit()->getTilePosition());
					}
				}
				game_state.addMineralsCommitted(-100);
				
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
			else if (new_mineral_worker.getBuildType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun)
			{
				game_state.addMineralsCommitted(-150);
				game_state.addGasCommitted(-100);
			}
			else if (new_mineral_worker.getBuildType() == BWAPI::UnitTypes::Protoss_Templar_Archives)
			{
				game_state.addMineralsCommitted(-150);
				game_state.addGasCommitted(-200);
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
			if (building_type != BWAPI::UnitTypes::Terran_Refinery &&
				building_type != BWAPI::UnitTypes::Protoss_Assimilator &&
				building_type != BWAPI::UnitTypes::Zerg_Extractor)
			{
				BWAPI::TilePosition build_position = this->getBuildLocation(*mineral_worker_iterator, building_type, game_state);
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

BWAPI::TilePosition WorkerManager::getBuildLocation(Object build_worker, BWAPI::UnitType building_type, GameState &game_state)
{
	if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
	{
		if (game_state.getBuildOrder() == "build2")
		{
			if (building_type == BWAPI::UnitTypes::Terran_Supply_Depot &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Terran_Supply_Depot) == 0)
			{
				BWAPI::TilePosition position_to_build;
				if (BWAPI::Broodwar->self()->getStartLocation().x == 114 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 116)
				{
					position_to_build.x = 92;
					position_to_build.y = 95;
				}
				else if (BWAPI::Broodwar->self()->getStartLocation().x == 114 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 80)
				{
					position_to_build.x = 101;
					position_to_build.y = 61;
				}
				else if (BWAPI::Broodwar->self()->getStartLocation().x == 113 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 8)
				{
					position_to_build.x = 95;
					position_to_build.y = 22;
				}
				else if (BWAPI::Broodwar->self()->getStartLocation().x == 72 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 8)
				{
					position_to_build.x = 52;
					position_to_build.y = 23;
				}
				else if (BWAPI::Broodwar->self()->getStartLocation().x == 10 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 6)
				{
					position_to_build.x = 32;
					position_to_build.y = 21;
				}
				else if (BWAPI::Broodwar->self()->getStartLocation().x == 8 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 47)
				{
					position_to_build.x = 24;
					position_to_build.y = 55;
				}
				else if (BWAPI::Broodwar->self()->getStartLocation().x == 10 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 114)
				{
					position_to_build.x = 16;
					position_to_build.y = 93;
				}
				else if (BWAPI::Broodwar->self()->getStartLocation().x == 63 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 117)
				{
					position_to_build.x = 55;
					position_to_build.y = 94;
				}
				return position_to_build;
			}
			else if (building_type == BWAPI::UnitTypes::Terran_Barracks &&
				game_state.getBuildingTypeCount(BWAPI::UnitTypes::Terran_Barracks) == 0)
			{
				BWAPI::TilePosition position_to_build;
				if (BWAPI::Broodwar->self()->getStartLocation().x == 114 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 116)
				{
					position_to_build.x = 90;
					position_to_build.y = 97;
				}
				else if (BWAPI::Broodwar->self()->getStartLocation().x == 114 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 80)
				{
					position_to_build.x = 101;
					position_to_build.y = 63;
				}
				else if (BWAPI::Broodwar->self()->getStartLocation().x == 113 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 8)
				{
					position_to_build.x = 96;
					position_to_build.y = 24;
				}
				else if (BWAPI::Broodwar->self()->getStartLocation().x == 72 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 8)
				{
					position_to_build.x = 54;
					position_to_build.y = 25;
				}
				else if (BWAPI::Broodwar->self()->getStartLocation().x == 10 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 6)
				{
					position_to_build.x = 30;
					position_to_build.y = 23;
				}
				else if (BWAPI::Broodwar->self()->getStartLocation().x == 8 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 47)
				{
					position_to_build.x = 22;
					position_to_build.y = 57;
				}
				else if (BWAPI::Broodwar->self()->getStartLocation().x == 10 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 114)
				{
					position_to_build.x = 17;
					position_to_build.y = 95;
				}
				else if (BWAPI::Broodwar->self()->getStartLocation().x == 63 &&
					BWAPI::Broodwar->self()->getStartLocation().y == 117)
				{
					position_to_build.x = 52;
					position_to_build.y = 96;
				}
				return position_to_build;
			}
		}
		BWAPI::TilePosition position_to_try = build_worker.getBase()->getArea()->Bases().begin()->Location();
		bool too_close = false;
		while (true)
		{
			if (building_type == BWAPI::UnitTypes::Terran_Supply_Depot)
			{
				BWAPI::Unitset too_close_units = BWAPI::Broodwar->getUnitsInRadius((BWAPI::Position)position_to_try, 32);
				if (too_close_units.size() > 0)
				{
					position_to_try.x += rand() % 4;
					position_to_try.y += rand() % 4;
					position_to_try.makeValid();
				}
				else
				{
					BWAPI::Position position_to_check;
					position_to_check.x = ((BWAPI::Position)position_to_try).x + 33;
					position_to_check.y = ((BWAPI::Position)position_to_try).y;
					too_close_units = BWAPI::Broodwar->getUnitsInRadius(position_to_check, 32);
					if (too_close_units.size() > 0)
					{
						position_to_try.x -= rand() % 4;
						position_to_try.y += rand() % 4;
						position_to_try.makeValid();
					}
					else
					{
						position_to_check.x = ((BWAPI::Position)position_to_try).x + 33;
						position_to_check.y = ((BWAPI::Position)position_to_try).y + 33;
						too_close_units = BWAPI::Broodwar->getUnitsInRadius(position_to_check, 32);
						if (too_close_units.size() > 0)
						{
							position_to_try.x -= rand() % 3;
							position_to_try.y -= rand() % 2;
							position_to_try.makeValid();
						}
						else
						{
							position_to_check.x = ((BWAPI::Position)position_to_try).x;
							position_to_check.y = ((BWAPI::Position)position_to_try).y + 33;
							too_close_units = BWAPI::Broodwar->getUnitsInRadius(position_to_check, 32);
							if (too_close_units.size() > 0)
							{
								position_to_try.x += rand() % 3;
								position_to_try.y -= rand() % 2;
								position_to_try.makeValid();
							}
							else return position_to_try;
						}
					}
				}
			}
			/*else if (building_type == BWAPI::UnitTypes::Terran_Factory)
			{
				BWAPI::Unitset too_close_units = BWAPI::Broodwar->getUnitsInRadius((BWAPI::Position)position_to_try, 32);
				if (too_close_units.size() > 0)
				{
					position_to_try.x += rand() % 4;
					position_to_try.y += rand() % 4;
					position_to_try.makeValid();
				}
				else
				{
					BWAPI::Position position_to_check;
					position_to_check.x = ((BWAPI::Position)position_to_try).x + 113;
					position_to_check.y = ((BWAPI::Position)position_to_try).y + 81;
					too_close_units = BWAPI::Broodwar->getUnitsInRadius(position_to_check, 32);
					if (too_close_units.size() > 0)
					{
						position_to_try.x -= rand() % 2;
						position_to_try.y -= rand() % 2;
						position_to_try.makeValid();
					}
					else
					{
						position_to_check.x = ((BWAPI::Position)position_to_try).x;
						position_to_check.y = ((BWAPI::Position)position_to_try).y + 33;
						too_close_units = BWAPI::Broodwar->getUnitsInRadius(position_to_check, 32);
						if (too_close_units.size() > 0)
						{
							position_to_try.x += rand() % 3;
							position_to_try.y -= rand() % 2;
							position_to_try.makeValid();
						}
						else return position_to_try;
					}
				}
				
			}*/
			else
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
	else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
	{
		BWAPI::TilePosition position_to_try = BWAPI::Broodwar->getBuildLocation(building_type, build_worker.getUnit()->getTilePosition());
		while (true)
		{
			if (building_type == BWAPI::UnitTypes::Protoss_Pylon)
			{
				BWAPI::Unitset too_close_units = BWAPI::Broodwar->getUnitsInRadius((BWAPI::Position)position_to_try, 32);
				if (too_close_units.size() > 0)
				{
					position_to_try.x += rand() % 4;
					position_to_try.y += rand() % 4;
					position_to_try.makeValid();
				}
				else
				{
					BWAPI::Position position_to_check;
					position_to_check.x = ((BWAPI::Position)position_to_try).x + 33;
					position_to_check.y = ((BWAPI::Position)position_to_try).y;
					too_close_units = BWAPI::Broodwar->getUnitsInRadius(position_to_check, 32);
					if (too_close_units.size() > 0)
					{
						position_to_try.x -= rand() % 4;
						position_to_try.y += rand() % 4;
						position_to_try.makeValid();
					}
					else
					{
						position_to_check.x = ((BWAPI::Position)position_to_try).x + 33;
						position_to_check.y = ((BWAPI::Position)position_to_try).y + 33;
						too_close_units = BWAPI::Broodwar->getUnitsInRadius(position_to_check, 32);
						if (too_close_units.size() > 0)
						{
							position_to_try.x -= rand() % 4;
							position_to_try.y -= rand() % 4;
							position_to_try.makeValid();
						}
						else
						{
							position_to_check.x = ((BWAPI::Position)position_to_try).x;
							position_to_check.y = ((BWAPI::Position)position_to_try).y + 33;
							too_close_units = BWAPI::Broodwar->getUnitsInRadius(position_to_check, 32);
							if (too_close_units.size() > 0)
							{
								position_to_try.x += rand() % 4;
								position_to_try.y -= rand() % 4;
								position_to_try.makeValid();
							}
							else return position_to_try;
						}
					}
				}
			}
			else
			{
				return position_to_try;
			}
		}
	}
	else
	{
		return BWAPI::Broodwar->getBuildLocation(building_type, build_worker.getUnit()->getTilePosition());
	}
}