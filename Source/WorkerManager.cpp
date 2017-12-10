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
				for (auto unit : mineral_worker_iterator->getUnit()->getUnitsInRadius(100))
				{
					if ((unit->getType() == BWAPI::UnitTypes::Zerg_Zergling ||
						unit->getType() == BWAPI::UnitTypes::Terran_SCV ||
						unit->getType() == BWAPI::UnitTypes::Protoss_Probe ||
						unit->getType() == BWAPI::UnitTypes::Zerg_Drone) &&
						unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
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
					if (mineral_worker_iterator->getBase()->getArea()->Minerals().size() > 0)
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
				if (game_state.getExpanding())
				{
					game_state.toggleExpanding();
				}
				game_state.addMineralsCommitted(-400);
				game_state.setTargetExpansion(nullptr);
			}
			game_state.addMineralsCommitted(-1 * build_worker_iterator->getBuildType().mineralPrice());
			game_state.addGasCommitted(-1 * build_worker_iterator->getBuildType().gasPrice());
			if (build_worker_iterator->getBuildType().getRace() == BWAPI::Races::Protoss)
				game_state.updateBuildMap(build_worker_iterator->getTargetBuildLocation().x, build_worker_iterator->getTargetBuildLocation().y, build_worker_iterator->getBuildType(), false);
			auto erase_iterator = build_worker_iterator;
			build_worker_iterator = game_state.getBuildWorkers()->erase(erase_iterator);
		}
		else if (build_worker_iterator->getTargetBase() != nullptr)
		{
			if (BWEM::Map::Instance().GetArea(build_worker_iterator->getUnit()->getTilePosition()) != nullptr)
			{
				if (build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::PlaceBuilding)
				{
					if (((build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::ConstructingBuilding &&
						BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran) ||
						(build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::PlayerGuard &&
						BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)) &&
						build_worker_iterator->getTargetBase()->getArea()->Id() == BWEM::Map::Instance().GetArea(build_worker_iterator->getUnit()->getTilePosition())->Id())
					{
						bool building_found = false;
						for (auto unit : BWAPI::Broodwar->getUnitsInRadius(build_worker_iterator->getUnit()->getPosition(), 2 * 32))
						{
							if (unit->getPlayer() == BWAPI::Broodwar->self() &&
								(unit->getType() == BWAPI::UnitTypes::Terran_Command_Center ||
								unit->getType() == BWAPI::UnitTypes::Protoss_Nexus))
							{
								building_found = true;
							}
						}
						if (building_found == true)
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
						else if (build_worker_iterator->getUnit()->getDistance((BWAPI::Position)build_worker_iterator->getTargetBase()->getArea()->Bases().begin()->Location()) <= 5 * 32)
						{
							if(!build_worker_iterator->getUnit()->build(build_worker_iterator->getBuildType(), build_worker_iterator->getTargetBase()->getArea()->Bases().begin()->Location()))
								build_worker_iterator->getUnit()->move((*build_worker_iterator->getTargetBase()->getArea()->Bases().begin()).Center());
							build_worker_iterator++;
						}
						else
						{
							build_worker_iterator->getUnit()->move((*build_worker_iterator->getTargetBase()->getArea()->Bases().begin()).Center());
							build_worker_iterator++;
						}
					}
					else if (BWEM::Map::Instance().GetArea((BWAPI::TilePosition)build_worker_iterator->getUnit()->getOrderTargetPosition()) != nullptr)
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
							else if (build_worker_iterator->getTargetBase()->getArea()->Minerals().size() == 0 &&
								build_worker_iterator->getUnit()->getDistance((BWAPI::Position)build_worker_iterator->getTargetBase()->getArea()->Bases().begin()->Location()) <= 5 * 32)
							{
								build_worker_iterator->getUnit()->build(build_worker_iterator->getBuildType(), build_worker_iterator->getTargetBase()->getArea()->Bases().begin()->Location());
								build_worker_iterator++;
							}
							else if (build_worker_iterator->getTargetBase()->getArea()->Id() != BWEM::Map::Instance().GetArea((BWAPI::TilePosition)build_worker_iterator->getUnit()->getOrderTargetPosition())->Id() ||
								build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::Move)
							{
								if (build_worker_iterator->getTargetBase()->getArea()->Minerals().size() > 0)
								{
									if (!build_worker_iterator->getUnit()->gather((*build_worker_iterator->getTargetBase()->getArea()->Minerals().begin())->Unit()))
									{
										build_worker_iterator->getUnit()->move((*build_worker_iterator->getTargetBase()->getArea()->Bases().begin()).Center());
									}
								}
								else
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
					else
					{
						if (build_worker_iterator->getTargetBase()->getArea()->Minerals().size() > 0)
						{
							if (!build_worker_iterator->getUnit()->gather((*build_worker_iterator->getTargetBase()->getArea()->Minerals().begin())->Unit()))
							{
								build_worker_iterator->getUnit()->move((*build_worker_iterator->getTargetBase()->getArea()->Bases().begin()).Center());
							}
						}
						else
						{
							build_worker_iterator->getUnit()->move((*build_worker_iterator->getTargetBase()->getArea()->Bases().begin()).Center());
						}
						build_worker_iterator++;
					}
				}
				else
					build_worker_iterator++;
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
			if (build_worker_iterator->getBuildType() != BWAPI::UnitTypes::Terran_Refinery &&
				build_worker_iterator->getBuildType() != BWAPI::UnitTypes::Protoss_Assimilator)
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
				BWAPI::TilePosition build_position = game_state.getGasBuildTileLocation(build_worker_iterator->getBase()->getArea());
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
			if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Assimilator)
			{				
				Object new_mineral_worker(*build_worker_iterator);
				new_mineral_worker.setBuildType(BWAPI::UnitTypes::Unknown);
				new_mineral_worker.setBaseClass(0);
				game_state.getMineralWorkers()->push_back(new_mineral_worker);
				game_state.addMineralsCommitted(-100);
				auto erase_iterator = build_worker_iterator;
				build_worker_iterator = game_state.getBuildWorkers()->erase(erase_iterator);
			}
			else
			{
				game_state.updateBuildMap(build_worker_iterator->getTargetBuildLocation().x, build_worker_iterator->getTargetBuildLocation().y, build_worker_iterator->getBuildType(), false);
				BWAPI::TilePosition build_position = getBuildLocation(*build_worker_iterator, build_worker_iterator->getBuildType(), game_state);
				if (BWAPI::Broodwar->canBuildHere(build_position, build_worker_iterator->getBuildType(), build_worker_iterator->getUnit()))
				{
					build_worker_iterator->getUnit()->build(build_worker_iterator->getBuildType(), build_position);
					game_state.updateBuildMap(build_position.x, build_position.y, build_worker_iterator->getBuildType(), false);
				}
				else
					build_worker_iterator->getUnit()->move((BWAPI::Position)build_position);
				build_worker_iterator->setTargetBuildLocation(build_position);
				build_worker_iterator++;
			}
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
				game_state.addForge(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core)
			{
				game_state.addMineralsCommitted(-200);
				if (game_state.checkCyberCore())
					game_state.toggleCyberCore();
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Robotics_Facility)
			{
				game_state.addMineralsCommitted(-200);
				game_state.addGasCommitted(-200);
				game_state.addRoboticsFacility(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Observatory)
			{
				game_state.addMineralsCommitted(-50);
				game_state.addGasCommitted(-100);
				game_state.addObservatory(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun)
			{
				game_state.addMineralsCommitted(-150);
				game_state.addGasCommitted(-100);
				game_state.addCitadelofAdun(-1);
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
				game_state.addFactory(-1);
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
			if (build_worker_iterator->getBuildType().getRace() == BWAPI::Races::Protoss)
				game_state.updateBuildMap(build_worker_iterator->getTargetBuildLocation().x, build_worker_iterator->getTargetBuildLocation().y, build_worker_iterator->getBuildType(), false);
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
		if (game_state.getContainingBase(mineral_worker_iterator->getUnit()->getTilePosition()) != nullptr)
		{
			bool temp_change_base_class = false;
			if (base_class == 1)
			{
				temp_change_base_class = true;
				base_class = 3;
			}
			if (mineral_worker_iterator->getBase()->getBaseClass() == base_class &&
				!mineral_worker_iterator->getUnit()->isCarryingMinerals() &&
				BWAPI::Broodwar->canMake(building_type, mineral_worker_iterator->getUnit()) &&
				game_state.getContainingBase(mineral_worker_iterator->getUnit()->getTilePosition())->getBaseClass() == base_class)
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
							if (building_type.getRace() == BWAPI::Races::Protoss)
								game_state.updateBuildMap(build_position.x, build_position.y, building_type, true);
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
							if (building_type.getRace() == BWAPI::Races::Protoss)
								game_state.updateBuildMap(build_position.x, build_position.y, building_type, true);
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
					if (game_state.checkValidGasBuildTileLocation(base_class) &&
						BWEM::Map::Instance().GetArea(new_build_worker.getUnit()->getTilePosition()) != nullptr)
					{
						BWAPI::TilePosition build_position = game_state.getGasBuildTileLocation(BWEM::Map::Instance().GetArea(new_build_worker.getUnit()->getTilePosition()));
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
							mineral_worker_iterator++;
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
		else
		{
			mineral_worker_iterator++;
		}
	}
	return false;
}

BWAPI::TilePosition WorkerManager::getBuildLocation(Object build_worker, BWAPI::UnitType building_type, GameState &game_state)
{
	std::time_t build_location_start = std::clock();
	bool has_addon = building_type.canBuildAddon();
	if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
	{
		BWAPI::TilePosition position_to_build;
		if (BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition()) != nullptr)
			position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
		else
			position_to_build = build_worker.getUnit()->getTilePosition();
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
			if (!try_new_position &&
				has_addon)
			{
				if (position_to_build.x != 0 && position_to_build.y != 0)
				{
					for (int x = position_to_build.x + building_size.x - 1; x < position_to_build.x + building_size.x + 3; x++)
					{
						if (x == BWAPI::Broodwar->mapWidth())
						{
							try_new_position = true;
							x = position_to_build.x + building_size.x + 3;
						}
						else
						{
							for (int y = position_to_build.y + building_size.y - 2; y < position_to_build.y + building_size.y; y++)
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
										x = position_to_build.x + building_size.x + 3;
										y = position_to_build.y + building_size.y;
									}
								}
							}
						}
					}
				}
				else
				{
					for (int x = position_to_build.x + building_size.x; x < position_to_build.x + building_size.x + 2; x++)
					{
						if (x == BWAPI::Broodwar->mapWidth())
						{
							try_new_position = true;
							x = position_to_build.x + building_size.x;
						}
						else
						{
							for (int y = position_to_build.y + building_size.y - 2; y < position_to_build.y + building_size.y; y++)
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
										x = position_to_build.x + building_size.x + 2;
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
					const BWEM::Area* area_to_check = BWEM::Map::Instance().GetArea(position_to_build);
					if (area_to_check != nullptr)
					{
						if (BWEM::Map::Instance().GetArea(position_to_build) == nullptr)
						{
							in_base = false;
							if (BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition()) != nullptr)
								position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
							else
								position_to_build = build_worker.getUnit()->getTilePosition();
						}
						else if (BWEM::Map::Instance().GetArea(position_to_build)->Id() == build_worker.getBase()->getArea()->Id())
							in_base = true;
						else if (game_state.getContainingBase(position_to_build) != nullptr)
						{
							if (build_worker.getBase()->getBaseClass() == 3 &&
								game_state.getContainingBase(position_to_build)->getBaseClass() == 3)
							{
								in_base = true;
							}
							else
							{
								if (BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition()) != nullptr)
									position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
								else
									position_to_build = build_worker.getUnit()->getTilePosition();
							}
						}
						else
						{
							if (BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition()) != nullptr)
								position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
							else
								position_to_build = build_worker.getUnit()->getTilePosition();
						}
					}
					else
					{
						if (BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition()) != nullptr)
							position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
						else
							position_to_build = build_worker.getUnit()->getTilePosition();
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
			BWAPI::TilePosition position_to_build;
			if (BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition()) != nullptr)
				position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
			else
				position_to_build = build_worker.getUnit()->getTilePosition();
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
							else if (game_state.getContainingBase(position_to_build) != nullptr)
							{
								if (build_worker.getBase()->getBaseClass() == 3 &&
									game_state.getContainingBase(position_to_build)->getBaseClass() == 3)
								{
									in_base = true;
								}
								else
								{
									if (BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition()) != nullptr)
										position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
									else
										position_to_build = build_worker.getUnit()->getTilePosition();
								}
							}
							else
							{
								if (BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition()) != nullptr)
									position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
								else
									position_to_build = build_worker.getUnit()->getTilePosition();
							}
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
			if (BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition()) != nullptr)
			{
				BWAPI::TilePosition position_to_build;
				if (BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition()) != nullptr)
					position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
				else
					position_to_build = build_worker.getUnit()->getTilePosition();
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
								else if (game_state.getContainingBase(position_to_build) != nullptr)
								{
									if (build_worker.getBase()->getBaseClass() == 3 &&
										game_state.getContainingBase(position_to_build)->getBaseClass() == 3)
									{
										in_base = true;
									}
									else
									{
										if (BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition()) != nullptr)
											position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
										else
											position_to_build = build_worker.getUnit()->getTilePosition();
									}
								}
								else
								{
									if (BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition()) != nullptr)
										position_to_build = (BWAPI::TilePosition)BWEM::Map::Instance().GetArea(build_worker.getUnit()->getTilePosition())->Top();
									else
										position_to_build = build_worker.getUnit()->getTilePosition();
								}
							}
							if ((std::clock() - build_location_start) * 1000 > 350)
							{
								return BWAPI::TilePositions::Invalid;
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
				return BWAPI::Broodwar->getBuildLocation(building_type, build_worker.getUnit()->getTilePosition());
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
			if (building->getType() == BWAPI::UnitTypes::Terran_Command_Center)
			{
				mineral_worker_iterator->setBase(game_state.getContainingBase(building->getTilePosition()));
				mineral_worker_iterator->setBaseClass(4);
			}
			mineral_worker_iterator = game_state.getMineralWorkers()->end();
		}
		else
			mineral_worker_iterator++;
	}
}