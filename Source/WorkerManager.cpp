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
			else if (BWEM::Map::Instance().GetArea((BWAPI::TilePosition)mineral_worker_iterator->getUnit()->getOrderTargetPosition()) != nullptr &&
				!mineral_worker_iterator->getUnit()->isCarryingMinerals())
			{
				if (BWEM::Map::Instance().GetArea((BWAPI::TilePosition)mineral_worker_iterator->getUnit()->getOrderTargetPosition())->Id() != mineral_worker_iterator->getBase()->getArea()->Id() ||
					(mineral_worker_iterator->getUnit()->getOrderTargetPosition().x == 0 &&
					mineral_worker_iterator->getUnit()->getOrderTargetPosition().y == 0))
				{
					mineral_worker_iterator->getUnit()->gather((*mineral_worker_iterator->getBase()->getArea()->Minerals().begin())->Unit());
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
			if (build_worker_iterator->getTargetBase() != nullptr)
			{
				game_state.toggleExpanding();
				game_state.addMineralsCommitted(-400);
				game_state.setTargetExpansion(nullptr);
			}
			auto erase_iterator = build_worker_iterator;
			build_worker_iterator = game_state.getBuildWorkers()->erase(erase_iterator);
		}
		else if (build_worker_iterator->getTargetBase() != nullptr)
		{
			if (build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::PlaceBuilding)
			{
				if (BWEM::Map::Instance().GetArea((BWAPI::TilePosition)build_worker_iterator->getUnit()->getOrderTargetPosition()) != nullptr)
				{
					if (build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::ConstructingBuilding &&
						BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
					{
						Object new_mineral_worker(*build_worker_iterator);
						new_mineral_worker.setBuildType(BWAPI::UnitTypes::Unknown);
						new_mineral_worker.setBaseClass(4);
						new_mineral_worker.setBase(new_mineral_worker.getTargetBase());
						new_mineral_worker.setTargetBase(nullptr);
						game_state.getMineralWorkers()->push_back(new_mineral_worker);
						auto erase_iterator = build_worker_iterator;
						build_worker_iterator = game_state.getBuildWorkers()->erase(erase_iterator);
					}
					else
					{
						if (BWEM::Map::Instance().GetArea(build_worker_iterator->getUnit()->getTilePosition()) != nullptr)
						{
							if ((build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::MiningMinerals ||
								build_worker_iterator->getUnit()->isCarryingMinerals()) &&
								BWEM::Map::Instance().GetArea(build_worker_iterator->getUnit()->getTilePosition())->Id() == build_worker_iterator->getTargetBase()->getArea()->Id())
							{
								build_worker_iterator->getUnit()->build(build_worker_iterator->getBuildType(), build_worker_iterator->getTargetBase()->getArea()->Bases().begin()->Location());
								build_worker_iterator++;
							}
							else if (build_worker_iterator->getTargetBase()->getArea()->Id() != BWEM::Map::Instance().GetArea((BWAPI::TilePosition)build_worker_iterator->getUnit()->getOrderTargetPosition())->Id() ||
								build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::Move)
							{
								if (!build_worker_iterator->getUnit()->gather((*build_worker_iterator->getTargetBase()->getArea()->Minerals().begin())->Unit()))
								{
									build_worker_iterator->getUnit()->move((*build_worker_iterator->getTargetBase()->getArea()->Bases().begin()).Center());
								}
								build_worker_iterator++;
							}
							else
								build_worker_iterator++;
						}
						else
							build_worker_iterator++;
					}
				}
				else
				{
					if (!build_worker_iterator->getUnit()->gather((*build_worker_iterator->getTargetBase()->getArea()->Minerals().begin())->Unit()))
					{
						build_worker_iterator->getUnit()->move((*build_worker_iterator->getTargetBase()->getArea()->Bases().begin()).Center());
					}
					build_worker_iterator++;
				}
			}
			else
				build_worker_iterator++;
		}
		else if (build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::Move)
		{
			if (build_worker_iterator->getUnit()->getDistance((BWAPI::Position)build_worker_iterator->getTargetBuildLocation()) == 5 * 32)
				build_worker_iterator->getUnit()->build(build_worker_iterator->getBuildType(), build_worker_iterator->getTargetBuildLocation());
			build_worker_iterator++;
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
		else if (build_worker_iterator->getElapsedTimeOrderGiven() + 30 < BWAPI::Broodwar->elapsedTime() &&
			!game_state.getExpanding())
		{

			if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Supply_Depot)
			{
				game_state.addSupplyExpected(-8);
				game_state.addMineralsCommitted(-100);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Pylon)
			{
				game_state.addSupplyExpected(-8);
				game_state.addMineralsCommitted(-100);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Barracks)
			{
				game_state.addMineralsCommitted(-150);
				game_state.addBarracks(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Gateway &&
				game_state.getBuildOrder() != BuildOrder::P2Gate1)
			{
				game_state.addMineralsCommitted(-150);
				game_state.addBarracks(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Refinery)
			{
				game_state.addGas(-1);
				game_state.addMineralsCommitted(-100);

			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Academy)
			{
				if (game_state.getBuildingTypeCount(BWAPI::UnitTypes::Terran_Academy) == 0)
					game_state.toggleAcademy();
				game_state.addMineralsCommitted(-150);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Zerg_Hatchery)
			{
				game_state.addSupplyExpected(-1);
				game_state.addMineralsCommitted(-300);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Forge)
			{
				game_state.addMineralsCommitted(-150);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core)
			{
				game_state.addMineralsCommitted(-200);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Robotics_Facility)
			{
				game_state.addMineralsCommitted(-200);
				game_state.addGasCommitted(-200);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Observatory)
			{
				game_state.addMineralsCommitted(-50);
				game_state.addGasCommitted(-100);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun)
			{
				game_state.addMineralsCommitted(-150);
				game_state.addGasCommitted(-100);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Templar_Archives)
			{
				game_state.addMineralsCommitted(-150);
				game_state.addGasCommitted(-200);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Factory)
			{
				game_state.addMineralsCommitted(-200);
				game_state.addGasCommitted(-150);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Armory)
			{
				game_state.addMineralsCommitted(-100);
				game_state.addGasCommitted(-50);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Command_Center)
			{
				game_state.addMineralsCommitted(-400);
			}
			build_worker_iterator->getUnit()->stop();
			Object new_mineral_worker(*build_worker_iterator);
			new_mineral_worker.setBuildType(BWAPI::UnitTypes::Unknown);
			new_mineral_worker.setBaseClass(0);
			game_state.getMineralWorkers()->push_back(new_mineral_worker);
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
		bool temp_change_base_class = false;
		if (base_class == 1)
		{
			temp_change_base_class = true;
			base_class = 3;
		}
		if (mineral_worker_iterator->getBase()->getBaseClass() == base_class &&
			!mineral_worker_iterator->getUnit()->isCarryingMinerals() &&
			BWAPI::Broodwar->canMake(building_type, mineral_worker_iterator->getUnit()))
		{
			Object new_build_worker(*mineral_worker_iterator);
			if (temp_change_base_class == true)
				base_class = 1;
			if ((building_type == BWAPI::UnitTypes::Terran_Command_Center ||
				building_type == BWAPI::UnitTypes::Protoss_Nexus ||
				building_type == BWAPI::UnitTypes::Zerg_Hatchery) &&
				base_class == 1)
			{
				AIBase* closest_empty_base = game_state.getClosestEmptyBase();
				if (closest_empty_base != nullptr)
				{
					new_build_worker.setBuildType(building_type);
					new_build_worker.setBaseClass(base_class);
					new_build_worker.setElapsedTimeOrderGiven(BWAPI::Broodwar->elapsedTime());
					new_build_worker.setTargetBase(closest_empty_base);
					game_state.getBuildWorkers()->push_back(new_build_worker);
					game_state.getMineralWorkers()->erase(mineral_worker_iterator);
					game_state.setTargetExpansion(closest_empty_base);
					return true;
				}
				else
					return false;
			}
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
						new_build_worker.setElapsedTimeOrderGiven(BWAPI::Broodwar->elapsedTime());
						new_build_worker.setTargetBuildLocation(build_position);
						game_state.getBuildWorkers()->push_back(new_build_worker);
						game_state.getMineralWorkers()->erase(mineral_worker_iterator);
						return true;
					}
					else
					{
						new_build_worker.getUnit()->move((BWAPI::Position)build_position);
						new_build_worker.setBuildType(building_type);
						new_build_worker.setBaseClass(base_class);
						new_build_worker.setElapsedTimeOrderGiven(BWAPI::Broodwar->elapsedTime());
						new_build_worker.setTargetBuildLocation(build_position);
						game_state.getBuildWorkers()->push_back(new_build_worker);
						game_state.getMineralWorkers()->erase(mineral_worker_iterator);
						return true;
					}
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
							new_build_worker.setElapsedTimeOrderGiven(BWAPI::Broodwar->elapsedTime());
							game_state.getBuildWorkers()->push_back(new_build_worker);
							game_state.getMineralWorkers()->erase(mineral_worker_iterator);
							return true;
						}
						else
						{
							new_build_worker.getUnit()->move((BWAPI::Position)build_position);
							new_build_worker.setBuildType(building_type);
							new_build_worker.setBaseClass(base_class);
							new_build_worker.setElapsedTimeOrderGiven(BWAPI::Broodwar->elapsedTime());
							game_state.getBuildWorkers()->push_back(new_build_worker);
							game_state.getMineralWorkers()->erase(mineral_worker_iterator);
							return true;
						}
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
			if (temp_change_base_class == true)
				base_class = 1;
			mineral_worker_iterator++;
		}
	}
	return false;
}

BWAPI::TilePosition WorkerManager::getBuildLocation(Object build_worker, BWAPI::UnitType building_type, GameState &game_state)
{
	std::time_t build_location_start = std::clock();
	if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
	{
		if (game_state.getBuildOrder() == BuildOrder::BGHMech)
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
		BWAPI::TilePosition position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
		BWAPI::TilePosition building_size = building_type.tileSize();
		bool can_build = false;
		bool try_new_position = false;
		while (!can_build)
		{
			try_new_position = false;
			if (position_to_build.x != 0 && position_to_build.y != 0)
			{
				for (int x = position_to_build.x - 1; x < position_to_build.x + building_size.x + 1; x++)
				{
					if (x == BWAPI::Broodwar->mapWidth())
					{
						try_new_position = true;
						x = position_to_build.x + building_size.x + 1;
					}
					else
					{
						for (int y = position_to_build.y; y < position_to_build.y + building_size.y; y++)
						{
							if (y == BWAPI::Broodwar->mapHeight())
							{
								try_new_position = true;
								y = position_to_build.y + building_size.y;
								x = position_to_build.x + building_size.x;
							}
							else
							{
								if (!game_state.getBuildPositionMap()->at(x + (y * BWAPI::Broodwar->mapWidth())).first.unobstructed)
								{
									try_new_position = true;
									x = position_to_build.x + building_size.x;
									y = position_to_build.y + building_size.y;
								}
							}
						}
					}
				}
			}
			else
			{
				for (int x = position_to_build.x ; x < position_to_build.x + building_size.x; x++)
				{
					if (x == BWAPI::Broodwar->mapWidth())
					{
						try_new_position = true;
						x = position_to_build.x + building_size.x;
					}
					else
					{
						for (int y = position_to_build.y; y < position_to_build.y + building_size.y; y++)
						{
							if (y == BWAPI::Broodwar->mapHeight())
							{
								try_new_position = true;
								y = position_to_build.y + building_size.y;
								x = position_to_build.x + building_size.x;
							}
							else
							{
								if (!game_state.getBuildPositionMap()->at(x + (y * BWAPI::Broodwar->mapWidth())).first.unobstructed)
								{
									try_new_position = true;
									y = position_to_build.y + building_size.y;
									x = position_to_build.x + building_size.x;
								}
							}
						}
					}
				}
			}
			if (!try_new_position)
				can_build = true;
			else
			{
				bool in_base = false;
				while (!in_base)
				{
					position_to_build.x += rand() % 5 - 2;
					position_to_build.y += rand() % 5 - 2;
					position_to_build.makeValid();
					const BWEM::Area* area_to_check = BWEM::Map::Instance().GetArea(position_to_build);
					if (area_to_check != nullptr)
					{
						if (BWEM::Map::Instance().GetArea(position_to_build) == nullptr)
						{
							in_base = false;
							position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
						}
						else if (BWEM::Map::Instance().GetArea(position_to_build)->Id() == build_worker.getBase()->getArea()->Id())
							in_base = true;
						else
							position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
					}
					else
						position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
				}
			}
			if ((std::clock() - build_location_start) * 1000 > 350)
			{
				return BWAPI::TilePositions::Invalid;
			}
		}
		return position_to_build;
	}
	/*else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
	{
		if (building_type == BWAPI::UnitTypes::Zerg_Creep_Colony &&
			)
		{
			BWAPI::TilePosition position_to_try;
			position_to_try.x = (*BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->ChokePoints().begin())->Center().x;
			position_to_try.y = (*BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->ChokePoints().begin())->Center().y;
			while (true)
			{

			}
		}
	}*/
	else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
	{
		if (building_type == BWAPI::UnitTypes::Protoss_Pylon)
		{
			BWAPI::TilePosition position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
			BWAPI::TilePosition building_size = building_type.tileSize();
			bool can_build = false;
			bool try_new_position = false;
			while (!can_build)
			{
				try_new_position = false;
				if (position_to_build.x != 0 && position_to_build.y != 0)
				{
					for (int x = position_to_build.x - 1; x < position_to_build.x + building_size.x + 1; x++)
					{
						if (x == BWAPI::Broodwar->mapWidth())
						{
							try_new_position = true;
							x = position_to_build.x + building_size.x + 1;
						}
						else
						{
							for (int y = position_to_build.y - 1; y < position_to_build.y + building_size.y + 1; y++)
							{
								if (y == BWAPI::Broodwar->mapHeight())
								{
									try_new_position = true;
									y = position_to_build.y + building_size.y + 1;
									x = position_to_build.x + building_size.y + 1;
								}
								else
								{
									if (!game_state.getBuildPositionMap()->at(x + (y * BWAPI::Broodwar->mapWidth())).first.unobstructed)
									{
										try_new_position = true;
										y = position_to_build.y + building_size.y + 1;
										x = position_to_build.x + building_size.y + 1;
									}
								}
							}
						}
					}
				}
				else
				{
					for (int x = position_to_build.x; x < position_to_build.x + building_size.x; x++)
					{
						if (x == BWAPI::Broodwar->mapWidth())
						{
							try_new_position = true;
							x = position_to_build.x + building_size.x;
						}
						else
						{
							for (int y = position_to_build.y; y < position_to_build.y + building_size.y; y++)
							{
								if (y == BWAPI::Broodwar->mapHeight())
								{
									try_new_position = true;
									x = position_to_build.x + building_size.x;
									y = position_to_build.y + building_size.y;
								}
								else
								{
									if (!game_state.getBuildPositionMap()->at(x + (y * BWAPI::Broodwar->mapWidth())).first.unobstructed)
									{
										try_new_position = true;
										try_new_position = true;
										x = position_to_build.x + building_size.x;
										y = position_to_build.y + building_size.y;
									}
								}
							}
						}
					}
				}
				if (!try_new_position)
					can_build = true;
				else
				{
					bool in_base = false;
					while (!in_base)
					{
						position_to_build.x += rand() % 5 - 2;
						position_to_build.y += rand() % 5 - 2;
						position_to_build.makeValid();
						if (!(BWEM::Map::Instance().GetArea(position_to_build) == nullptr))
						{
							if (BWEM::Map::Instance().GetArea(position_to_build)->Id() == build_worker.getBase()->getArea()->Id())
								in_base = true;
							else
								position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
						}
					}
				}
				if ((std::clock() - build_location_start) * 1000 > 350)
				{
					return BWAPI::TilePositions::Invalid;
				}
			}
			return position_to_build;
		}
		else
		{
			BWAPI::TilePosition position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
			BWAPI::TilePosition building_size = building_type.tileSize();
			bool can_build = false;
			bool try_new_position = false;
			while (!can_build)
			{
				try_new_position = false;
				if (!BWAPI::Broodwar->hasPower(position_to_build, building_type))
					try_new_position = true;
				if (!try_new_position)
				{
					if (position_to_build.x != 0 && position_to_build.y != 0)
					{
						for (int x = position_to_build.x - 1; x < position_to_build.x + building_size.x + 1; x++)
						{
							if (x == BWAPI::Broodwar->mapWidth())
							{
								try_new_position = true;
								x = position_to_build.x + building_size.x + 1;
							}
							else
							{
								for (int y = position_to_build.y - 1; y < position_to_build.y + building_size.y + 1; y++)
								{
									if (y == BWAPI::Broodwar->mapHeight())
									{
										try_new_position = true;
										x = position_to_build.x + building_size.x + 1;
										y = position_to_build.y + building_size.y + 1;
									}
									else
									{
										if (!game_state.getBuildPositionMap()->at(x + (y * BWAPI::Broodwar->mapWidth())).first.unobstructed)
										{
											try_new_position = true;
											x = position_to_build.x + building_size.x + 1;
											y = position_to_build.y + building_size.y + 1;
										}
									}
								}
							}
						}
					}
					else
					{
						for (int x = position_to_build.x; x < position_to_build.x + building_size.x; x++)
						{
							if (x == BWAPI::Broodwar->mapWidth())
							{
								try_new_position = true;
								x = position_to_build.x + building_size.x;
							}
							else
							{
								for (int y = position_to_build.y; y < position_to_build.y + building_size.y; y++)
								{
									if (y == BWAPI::Broodwar->mapHeight())
									{
										try_new_position = true;
										x = position_to_build.x + building_size.x;
										y = position_to_build.y + building_size.y;
									}
									else
									{
										if (!game_state.getBuildPositionMap()->at(x + (y * BWAPI::Broodwar->mapWidth())).first.unobstructed)
										{
											try_new_position = true;
											x = position_to_build.x + building_size.x;
											y = position_to_build.y + building_size.y;
										}
									}
								}
							}
						}
					}
				}
				if (!try_new_position)
					can_build = true;
				else
				{
					bool in_base = false;
					while (!in_base)
					{
						position_to_build.x += rand() % 5 - 2;
						position_to_build.y += rand() % 5 - 2;
						position_to_build.makeValid();

						if (!(BWEM::Map::Instance().GetArea(position_to_build) == nullptr))
						{
							if (BWEM::Map::Instance().GetArea(position_to_build)->Id() == build_worker.getBase()->getArea()->Id())
								in_base = true;
							else
								position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
						}
					}
				}
				if ((std::clock() - build_location_start) * 1000 > 350)
				{
					return BWAPI::TilePositions::Invalid;
				}
			}
			return position_to_build;
		}
	}
	else
	{
		return BWAPI::Broodwar->getBuildLocation(building_type, build_worker.getUnit()->getTilePosition());
	}
}

void WorkerManager::getNewBuildWorker(BWAPI::Unit building, GameState &game_state)
{
	auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
	while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
	{
		if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
		{
			mineral_worker_iterator->getUnit()->rightClick(building);
			mineral_worker_iterator = game_state.getMineralWorkers()->end();
		}
		else
			mineral_worker_iterator++;
	}
}