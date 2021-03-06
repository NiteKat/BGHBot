#include"WorkerManager.h"

WorkerManager::WorkerManager(BWAPI::Game * game)
{
  Broodwar = game;
}

int WorkerManager::manageWorkers(std::unique_ptr<GameState> &game_state)
{
	int minerals_committed_adjust = 0;
	auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
	while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
	{
		if (mineral_worker_iterator->getUnit() == nullptr)
		{
			auto erase_iterator = mineral_worker_iterator;
			mineral_worker_iterator = game_state->getMineralWorkers()->erase(erase_iterator);
		}
		else if (!mineral_worker_iterator->getUnit()->exists())
		{
			auto erase_iterator = mineral_worker_iterator;
			if (mineral_worker_iterator->getResourceTarget() != nullptr)
				game_state->unassignWorkerFromMineral(&(*mineral_worker_iterator));
			mineral_worker_iterator = game_state->getMineralWorkers()->erase(erase_iterator);
		}
		else
		{
      if (mineral_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::ConstructingBuilding)
        int w = 9;
			BWAPI::Unit target = mineral_worker_iterator->getResourceTarget();
			if ((target == nullptr ||
				!target->exists()) &&
				mineral_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::ConstructingBuilding)
			{
				game_state->assignWorkerToMineral(&(*mineral_worker_iterator));
			}
			else if ((mineral_worker_iterator->getUnit()->getOrderTarget() == nullptr ||
        !mineral_worker_iterator->getUnit()->isCarryingMinerals() &&
        target != nullptr &&
				mineral_worker_iterator->getUnit()->getOrderTarget() != target ) &&
				mineral_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::AttackUnit)
			{
				mineral_worker_iterator->getUnit()->gather(target);
			}
			else if (mineral_worker_iterator->getUnit()->isCarryingMinerals() &&
				mineral_worker_iterator->getUnit()->isIdle())
			{
				mineral_worker_iterator->getUnit()->returnCargo();
			}
			if (mineral_worker_iterator->getUnit()->isUnderAttack() &&
				mineral_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::AttackUnit)
			{
				for (auto unit : mineral_worker_iterator->getUnit()->getUnitsInRadius(100))
				{
					if ((unit->getType() == BWAPI::UnitTypes::Zerg_Zergling ||
						unit->getType() == BWAPI::UnitTypes::Terran_SCV ||
						unit->getType() == BWAPI::UnitTypes::Protoss_Probe ||
						unit->getType() == BWAPI::UnitTypes::Zerg_Drone) &&
						unit->getPlayer()->isEnemy(Broodwar->self()))
					{
						mineral_worker_iterator->getUnit()->attack(unit);
					}
				}
			}
			mineral_worker_iterator++;
		}
	}

	auto build_worker_iterator = game_state->getBuildWorkers()->begin();
	while (build_worker_iterator != game_state->getBuildWorkers()->end())
	{
		if (build_worker_iterator->getUnit() == nullptr)
		{
			auto erase_iterator = build_worker_iterator;
			build_worker_iterator = game_state->getBuildWorkers()->erase(erase_iterator);
		}
		else if (!build_worker_iterator->getUnit()->exists())
		{
			if (build_worker_iterator->getTargetBase() != nullptr)
			{
				if (game_state->getExpanding())
				{
					game_state->toggleExpanding();
				}
				game_state->addMineralsCommitted(-400);
				game_state->setTargetExpansion(nullptr);
			}
			if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Supply_Depot)
			{
				game_state->addSupplyExpected(-8, BWAPI::Races::Terran);
				game_state->addMineralsCommitted(-100);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Pylon)
			{
				game_state->setPylonBuilding(false);
				game_state->addSupplyExpected(-8, BWAPI::Races::Protoss);
				game_state->addMineralsCommitted(-100);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Barracks)
			{
				game_state->addMineralsCommitted(-150);
				game_state->addBarracks(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Gateway &&
				game_state->getBuildOrder() != BuildOrder::P2Gate1)
			{
				game_state->addMineralsCommitted(-150);
				game_state->addBarracks(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Refinery)
			{
				game_state->addGas(-1);
				game_state->addMineralsCommitted(-100);

			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Academy)
			{
				if (game_state->getBuildingTypeCount(BWAPI::UnitTypes::Terran_Academy) == 0)
					game_state->toggleAcademy();
				game_state->addMineralsCommitted(-150);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Zerg_Hatchery)
			{
				game_state->addSupplyExpected(-1, BWAPI::Races::Zerg);
				game_state->addMineralsCommitted(-300);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Forge)
			{
				game_state->addMineralsCommitted(-150);
				game_state->addForge(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core)
			{
				game_state->addMineralsCommitted(-200);
				if (game_state->checkCyberCore())
					game_state->toggleCyberCore();
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Robotics_Facility)
			{
				game_state->addMineralsCommitted(-200);
				game_state->addGasCommitted(-200);
				game_state->addRoboticsFacility(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Observatory)
			{
				game_state->addMineralsCommitted(-50);
				game_state->addGasCommitted(-100);
				game_state->addObservatory(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun)
			{
				game_state->addMineralsCommitted(-150);
				game_state->addGasCommitted(-100);
				game_state->addCitadelofAdun(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Templar_Archives)
			{
				game_state->addMineralsCommitted(-150);
				game_state->addGasCommitted(-200);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Factory)
			{
				game_state->addMineralsCommitted(-200);
				game_state->addGasCommitted(-150);
				game_state->addFactory(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Armory)
			{
				game_state->addMineralsCommitted(-100);
				game_state->addGasCommitted(-50);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Command_Center)
			{
				game_state->addMineralsCommitted(-400);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Bunker)
			{
				game_state->addMineralsCommitted(-100);
				game_state->addBunker(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Engineering_Bay)
			{
				game_state->addMineralsCommitted(-125);
				game_state->addEngineeringBay(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Armory)
			{
				game_state->addMineralsCommitted(-1 * build_worker_iterator->getBuildType().mineralPrice());
				game_state->addGasCommitted(-1 * build_worker_iterator->getBuildType().gasPrice());
				game_state->addArmory(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Starport)
			{
				game_state->addMineralsCommitted(-1 * build_worker_iterator->getBuildType().mineralPrice());
				game_state->addGasCommitted(-1 * build_worker_iterator->getBuildType().gasPrice());
				game_state->addStarport(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Science_Facility)
			{
				game_state->addMineralsCommitted(-1 * build_worker_iterator->getBuildType().mineralPrice());
				game_state->addGasCommitted(-1 * build_worker_iterator->getBuildType().gasPrice());
				game_state->addScienceFacility(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Zerg_Spawning_Pool)
			{
				game_state->addMineralsCommitted(-1 * build_worker_iterator->getBuildType().mineralPrice());
				game_state->addSpawningPool(-1);
			}
			else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den)
			{
				game_state->addMineralsCommitted(-1 * build_worker_iterator->getBuildType().mineralPrice());
				game_state->addGasCommitted(-1 * build_worker_iterator->getBuildType().gasPrice());
				game_state->addHydraliskDen(-1);
			}
			game_state->addPositionToQueue(build_worker_iterator->getBuildType(), build_worker_iterator->getTargetBuildLocation());
			auto erase_iterator = build_worker_iterator;
			build_worker_iterator = game_state->getBuildWorkers()->erase(erase_iterator);
		}
		else
		{
			if (build_worker_iterator->getResourceTarget() != nullptr)
			{
				game_state->unassignWorkerFromMineral(&(*build_worker_iterator));
			}
			if (build_worker_iterator->getTargetBase() != nullptr)
			{
				if (BWEM::Map::Instance().GetArea(build_worker_iterator->getUnit()->getTilePosition()) != nullptr)
				{
					if (build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::PlaceBuilding)
					{
						if (((build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::ConstructingBuilding &&
							build_worker_iterator->getUnit()->getType().getRace() == BWAPI::Races::Terran) ||
							(build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::PlayerGuard &&
							build_worker_iterator->getUnit()->getType().getRace() == BWAPI::Races::Protoss)) &&
							build_worker_iterator->getTargetBase()->getArea()->Id() == BWEM::Map::Instance().GetArea(build_worker_iterator->getUnit()->getTilePosition())->Id())
						{
							bool building_found = false;
							for (auto unit : Broodwar->getUnitsInRadius(build_worker_iterator->getUnit()->getPosition(), 2 * 32))
							{
								if (unit->getPlayer() == Broodwar->self() &&
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
									game_state->getMineralWorkers()->push_back(new_mineral_worker);
									auto erase_iterator = build_worker_iterator;
									build_worker_iterator = game_state->getBuildWorkers()->erase(erase_iterator);
							}
							else if (build_worker_iterator->getUnit()->getDistance((BWAPI::Position)build_worker_iterator->getTargetBase()->getArea()->Bases().begin()->Location()) <= 5 * 32)
							{
								if (!build_worker_iterator->getUnit()->build(build_worker_iterator->getBuildType(), build_worker_iterator->getTargetBase()->getArea()->Bases().begin()->Location()))
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
								{
									//build_worker_iterator->getUnit()->move((*build_worker_iterator->getTargetBase()->getArea()->Bases().begin()).Center());
									build_worker_iterator++;
								}
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
								//auto target_base = game_state->getNearestContainingBase((BWAPI::TilePosition)build_worker_iterator->getUnit()->getOrderTargetPosition());
								//if (target_base->getBaseClass() != 3 &&
								//	target_base->getBaseClass() != 4 &&
								//	build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::PlaceBuilding)
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
				build_worker_iterator->getUnit()->getType().getRace() == BWAPI::Races::Terran) ||
				(build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::PlayerGuard &&
				build_worker_iterator->getUnit()->getType().getRace() == BWAPI::Races::Protoss) ||
				(build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::IncompleteBuilding &&
				build_worker_iterator->getUnit()->getType().getRace() == BWAPI::Races::Zerg)))
			{
				if (build_worker_iterator->getBuildType() != BWAPI::UnitTypes::Terran_Refinery &&
					build_worker_iterator->getBuildType() != BWAPI::UnitTypes::Protoss_Assimilator)
				{
					BWAPI::Position top_left = (BWAPI::Position)build_worker_iterator->getTargetBuildLocation();
					BWAPI::Position bottom_right = (BWAPI::Position)(build_worker_iterator->getTargetBuildLocation() + build_worker_iterator->getBuildType().tileSize());
					BWAPI::Unitset units_in_area = Broodwar->getUnitsInRectangle(top_left, bottom_right);
					for (auto unit : units_in_area)
					{
						if (unit->isIdle() &&
							unit != build_worker_iterator->getUnit())
						{
							BWAPI::Position choke = (BWAPI::Position)(*game_state->getNearestContainingBase(unit)->getArea()->ChokePoints().begin())->Center();
							unit->move(choke);
						}
					}
					build_worker_iterator->getUnit()->build(build_worker_iterator->getBuildType(), build_worker_iterator->getTargetBuildLocation());
					build_worker_iterator++;
				}
				else
				{
					BWAPI::TilePosition build_position = game_state->getGasBuildTileLocation(build_worker_iterator->getBase()->getArea());
					build_worker_iterator->getUnit()->build(build_worker_iterator->getBuildType(), build_position);
					build_worker_iterator++;
				}
			}
			else if (build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::ConstructingBuilding &&
				build_worker_iterator->getUnit()->getType().getRace() == BWAPI::Races::Terran)
			{
				if (build_worker_iterator->getBuildType() != BWAPI::UnitTypes::Terran_Refinery)
				{
					Object new_mineral_worker(*build_worker_iterator);
					new_mineral_worker.setBuildType(BWAPI::UnitTypes::Unknown);
					new_mineral_worker.setBaseClass(0);
					game_state->getMineralWorkers()->push_back(new_mineral_worker);
					auto erase_iterator = build_worker_iterator;
					build_worker_iterator = game_state->getBuildWorkers()->erase(erase_iterator);
				}
				else
				{
					Object new_building(build_worker_iterator->getUnit()->getBuildUnit(), game_state->getContainingBase(build_worker_iterator->getUnit()->getBuildUnit()));
					new_building.addGasWorker(static_cast<int>(build_worker_iterator->getUnit()->getID()));
					game_state->addBuilding(new_building);
					game_state->addMineralsCommitted(-100);
					game_state->setGeyserUsed(build_worker_iterator->getUnit()->getBuildUnit()->getTilePosition());
					auto erase_iterator = build_worker_iterator;
					build_worker_iterator = game_state->getBuildWorkers()->erase(erase_iterator);
				}
			}
			else if (build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::PlayerGuard &&
				build_worker_iterator->getUnit()->getType().getRace() == BWAPI::Races::Protoss)
			{
				if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Assimilator)
				{
					Object new_mineral_worker(*build_worker_iterator);
					new_mineral_worker.setBuildType(BWAPI::UnitTypes::Unknown);
					new_mineral_worker.setBaseClass(0);
					game_state->getMineralWorkers()->push_back(new_mineral_worker);
					game_state->addMineralsCommitted(-100);
					auto erase_iterator = build_worker_iterator;
					build_worker_iterator = game_state->getBuildWorkers()->erase(erase_iterator);
				}
				else
				{
					BWAPI::Position top_left = (BWAPI::Position)build_worker_iterator->getTargetBuildLocation();
					BWAPI::Position bottom_right = (BWAPI::Position)(build_worker_iterator->getTargetBuildLocation() + build_worker_iterator->getBuildType().tileSize());
					BWAPI::Unitset units_in_area = Broodwar->getUnitsInRectangle(top_left, bottom_right);
					for (auto unit : units_in_area)
					{
						if (unit->isIdle() &&
							unit != build_worker_iterator->getUnit())
						{
							BWAPI::Position choke = (BWAPI::Position)(*game_state->getNearestContainingBase(unit)->getArea()->ChokePoints().begin())->Center();
							unit->move(choke);
						}
					}
					build_worker_iterator->getUnit()->build(build_worker_iterator->getBuildType(), build_worker_iterator->getTargetBuildLocation());
					build_worker_iterator++;
				}
			}
			else if (build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::IncompleteBuilding &&
				build_worker_iterator->getUnit()->getType().getRace() == BWAPI::Races::Zerg)
			{
				auto erase_iterator = build_worker_iterator;
				build_worker_iterator = game_state->getBuildWorkers()->erase(erase_iterator);
			}
			else if (build_worker_iterator->getElapsedTimeOrderGiven() + 30 < Broodwar->elapsedTime() &&
				!game_state->getExpanding())
			{

				if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Supply_Depot)
				{
					game_state->addSupplyExpected(-8, BWAPI::Races::Terran);
					game_state->addMineralsCommitted(-100);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Pylon)
				{
					game_state->addSupplyExpected(-8, BWAPI::Races::Protoss);
					game_state->addMineralsCommitted(-100);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Barracks)
				{
					game_state->addMineralsCommitted(-150);
					game_state->addBarracks(-1);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Gateway &&
					game_state->getBuildOrder() != BuildOrder::P2Gate1)
				{
					game_state->addMineralsCommitted(-150);
					game_state->addBarracks(-1);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Refinery)
				{
					game_state->addGas(-1);
					game_state->addMineralsCommitted(-100);

				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Academy)
				{
					if (game_state->getBuildingTypeCount(BWAPI::UnitTypes::Terran_Academy) == 0)
						game_state->toggleAcademy();
					game_state->addMineralsCommitted(-150);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Zerg_Hatchery)
				{
					game_state->addSupplyExpected(-1, BWAPI::Races::Zerg);
					game_state->addMineralsCommitted(-300);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Forge)
				{
					game_state->addMineralsCommitted(-150);
					game_state->addForge(-1);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core)
				{
					game_state->addMineralsCommitted(-200);
					if (game_state->checkCyberCore())
						game_state->toggleCyberCore();
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Robotics_Facility)
				{
					game_state->addMineralsCommitted(-200);
					game_state->addGasCommitted(-200);
					game_state->addRoboticsFacility(-1);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Observatory)
				{
					game_state->addMineralsCommitted(-50);
					game_state->addGasCommitted(-100);
					game_state->addObservatory(-1);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun)
				{
					game_state->addMineralsCommitted(-150);
					game_state->addGasCommitted(-100);
					game_state->addCitadelofAdun(-1);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Protoss_Templar_Archives)
				{
					game_state->addMineralsCommitted(-150);
					game_state->addGasCommitted(-200);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Factory)
				{
					game_state->addMineralsCommitted(-200);
					game_state->addGasCommitted(-150);
					game_state->addFactory(-1);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Armory)
				{
					game_state->addMineralsCommitted(-100);
					game_state->addGasCommitted(-50);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Command_Center)
				{
					game_state->addMineralsCommitted(-400);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Bunker)
				{
					game_state->addMineralsCommitted(-100);
					game_state->addBunker(-1);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Engineering_Bay)
				{
					game_state->addMineralsCommitted(-125);
					game_state->addEngineeringBay(-1);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Armory)
				{
					game_state->addMineralsCommitted(-1 * build_worker_iterator->getBuildType().mineralPrice());
					game_state->addGasCommitted(-1 * build_worker_iterator->getBuildType().gasPrice());
					game_state->addArmory(-1);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Starport)
				{
					game_state->addMineralsCommitted(-1 * build_worker_iterator->getBuildType().mineralPrice());
					game_state->addGasCommitted(-1 * build_worker_iterator->getBuildType().gasPrice());
					game_state->addStarport(-1);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Terran_Science_Facility)
				{
					game_state->addMineralsCommitted(-1 * build_worker_iterator->getBuildType().mineralPrice());
					game_state->addGasCommitted(-1 * build_worker_iterator->getBuildType().gasPrice());
					game_state->addScienceFacility(-1);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Zerg_Spawning_Pool)
				{
					game_state->addMineralsCommitted(-1 * build_worker_iterator->getBuildType().mineralPrice());
					game_state->addSpawningPool(-1);
				}
				else if (build_worker_iterator->getBuildType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den)
				{
					game_state->addMineralsCommitted(-1 * build_worker_iterator->getBuildType().mineralPrice());
					game_state->addGasCommitted(-1 * build_worker_iterator->getBuildType().gasPrice());
					game_state->addHydraliskDen(-1);
				}
				build_worker_iterator->getUnit()->stop();
				game_state->addPositionToQueue(build_worker_iterator->getBuildType(), build_worker_iterator->getTargetBuildLocation());
				Object new_mineral_worker(*build_worker_iterator);
				new_mineral_worker.setBuildType(BWAPI::UnitTypes::Unknown);
				new_mineral_worker.setBaseClass(0);
				game_state->getMineralWorkers()->push_back(new_mineral_worker);
				auto erase_iterator = build_worker_iterator;
				build_worker_iterator = game_state->getBuildWorkers()->erase(erase_iterator);
			}
			else
			{
				build_worker_iterator++;
			}
		}
	}

	auto repair_worker_iterator = game_state->getRepairWorkers()->begin();
	while (repair_worker_iterator != game_state->getRepairWorkers()->end())
	{
		if (!repair_worker_iterator->getUnit()->exists())
		{
			auto erase_iterator = repair_worker_iterator;
			repair_worker_iterator = game_state->getRepairWorkers()->erase(erase_iterator);
		}
		else if (repair_worker_iterator->getRepairTarget() == nullptr ||
			!repair_worker_iterator->getRepairTarget()->exists())
		{
			Object new_mineral_worker(*repair_worker_iterator);
			new_mineral_worker.setRepairTarget(nullptr);
			game_state->getMineralWorkers()->push_back(new_mineral_worker);
			auto erase_iterator = repair_worker_iterator;
			repair_worker_iterator = game_state->getRepairWorkers()->erase(erase_iterator);
		}
		else if (repair_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::Repair)
		{
			if (repair_worker_iterator->getRepairTarget()->getHitPoints() < repair_worker_iterator->getRepairTarget()->getType().maxHitPoints())
			{
				repair_worker_iterator->getUnit()->repair(repair_worker_iterator->getRepairTarget());
			}
			repair_worker_iterator++;
		}
		else
		{
			repair_worker_iterator++;
		}
	}


	return minerals_committed_adjust;
}

bool WorkerManager::build(BWAPI::UnitType building_type, int base_class, std::unique_ptr<GameState> &game_state)
{
	auto start_time = std::chrono::high_resolution_clock::now();
	std::vector<const BWEM::Area*> bad_areas;
	auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
	while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
	{
		if (game_state->getContainingBase(mineral_worker_iterator->getUnit()->getTilePosition()) != nullptr)
		{
			bool temp_change_base_class = false;
			if (base_class == 1)
			{
				temp_change_base_class = true;
				base_class = 3;
			}
			if (mineral_worker_iterator->getBase()->getBaseClass() == base_class &&
				!mineral_worker_iterator->getUnit()->isCarryingMinerals() &&
				!mineral_worker_iterator->getUnit()->isConstructing() &&
				Broodwar->canMake(building_type, mineral_worker_iterator->getUnit()) &&
				game_state->getContainingBase(mineral_worker_iterator->getUnit()->getTilePosition())->getBaseClass() == base_class)
			{
				bool proceed = true;
				for (auto &bad_area : bad_areas)
				{
					if (bad_area == game_state->getContainingBase(mineral_worker_iterator->getUnit()->getTilePosition())->getArea())
					{
						proceed = false;
					}
				}
				if (proceed)
				{
					Object new_build_worker(*mineral_worker_iterator);
					if (temp_change_base_class == true)
						base_class = 1;
					if ((building_type == BWAPI::UnitTypes::Terran_Command_Center ||
						building_type == BWAPI::UnitTypes::Protoss_Nexus ||
						building_type == BWAPI::UnitTypes::Zerg_Hatchery) &&
						base_class == 1)
					{
						//AIBase* closest_empty_base = game_state->getClosestEmptyBase();
						AIBase* closest_empty_base = game_state->getBaseForExpansion();
						if (closest_empty_base != nullptr)
						{
							new_build_worker.setBuildType(building_type);
							new_build_worker.setBaseClass(base_class);
							new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
							new_build_worker.setTargetBase(closest_empty_base);
							game_state->getBuildWorkers()->push_back(new_build_worker);
							game_state->getMineralWorkers()->erase(mineral_worker_iterator);
							game_state->setTargetExpansion(closest_empty_base);
							return true;
						}
						else
							return false;
					}
					if (building_type != BWAPI::UnitTypes::Terran_Refinery &&
						building_type != BWAPI::UnitTypes::Protoss_Assimilator &&
						building_type != BWAPI::UnitTypes::Zerg_Extractor)
					{
						BWAPI::TilePosition build_position;
						if (building_type.requiresCreep() ||
							building_type == BWAPI::UnitTypes::Zerg_Hatchery)
							build_position = Broodwar->getBuildLocation(building_type, (BWAPI::TilePosition)new_build_worker.getCurrentPosition());
						else
							build_position = game_state->getBuildLocation(building_type, *mineral_worker_iterator);
						//build_position = this->getBuildLocation(*mineral_worker_iterator, building_type, game_state);
						//BWAPI::TilePosition build_position = Broodwar->getBuildLocation(building_type, (BWAPI::TilePosition)(*new_build_worker.getBase()->getArea()->Bases().begin()).Center());
						if (build_position != BWAPI::TilePositions::Invalid)
						{
							if (new_build_worker.getUnit()->build(building_type, build_position))
							{
								new_build_worker.setBuildType(building_type);
								new_build_worker.setBaseClass(base_class);
								new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
								new_build_worker.setTargetBuildLocation(build_position);
								game_state->getBuildWorkers()->push_back(new_build_worker);
								game_state->getMineralWorkers()->erase(mineral_worker_iterator);
								if (building_type.getRace() == BWAPI::Races::Protoss)
									game_state->updateBuildMap(build_position.x, build_position.y, building_type, true);
								return true;
							}
							else
							{
								new_build_worker.getUnit()->move((BWAPI::Position)build_position);
								new_build_worker.setBuildType(building_type);
								new_build_worker.setBaseClass(base_class);
								new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
								new_build_worker.setTargetBuildLocation(build_position);
								game_state->getBuildWorkers()->push_back(new_build_worker);
								game_state->getMineralWorkers()->erase(mineral_worker_iterator);
								if (building_type.getRace() == BWAPI::Races::Protoss)
									game_state->updateBuildMap(build_position.x, build_position.y, building_type, true);
								return true;
							}
						}
						else
						{
							bad_areas.push_back(game_state->getContainingBase(mineral_worker_iterator->getUnit()->getTilePosition())->getArea());
							auto end_time = std::chrono::high_resolution_clock::now();
							if (std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() > 20)
							{
								return false;
							}
							mineral_worker_iterator++;
						}
					}
					else
					{
						if (game_state->checkValidGasBuildTileLocation(base_class) &&
							BWEM::Map::Instance().GetArea(new_build_worker.getUnit()->getTilePosition()) != nullptr)
						{
							BWAPI::TilePosition build_position = game_state->getGasBuildTileLocation(BWEM::Map::Instance().GetArea(new_build_worker.getUnit()->getTilePosition()));
							if (build_position != BWAPI::TilePositions::Invalid)
							{
								if (new_build_worker.getUnit()->build(building_type, build_position))
								{
									new_build_worker.setBuildType(building_type);
									new_build_worker.setBaseClass(base_class);
									new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
									game_state->getBuildWorkers()->push_back(new_build_worker);
									game_state->getMineralWorkers()->erase(mineral_worker_iterator);
									return true;
								}
								else
								{
									new_build_worker.getUnit()->move((BWAPI::Position)build_position);
									new_build_worker.setBuildType(building_type);
									new_build_worker.setBaseClass(base_class);
									new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
									game_state->getBuildWorkers()->push_back(new_build_worker);
									game_state->getMineralWorkers()->erase(mineral_worker_iterator);
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
					mineral_worker_iterator++;
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
	mineral_worker_iterator = game_state->getMineralWorkers()->begin();
	while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
	{
		if (game_state->getContainingBase(mineral_worker_iterator->getUnit()->getTilePosition()) != nullptr)
		{
			if (!mineral_worker_iterator->getUnit()->isCarryingMinerals() &&
				Broodwar->canMake(building_type, mineral_worker_iterator->getUnit()))
			{
				bool proceed = true;
				for (auto &bad_area : bad_areas)
				{
					if (bad_area == game_state->getContainingBase(mineral_worker_iterator->getUnit()->getTilePosition())->getArea())
					{
						proceed = false;
					}
				}
				if (proceed)
				{
					Object new_build_worker(*mineral_worker_iterator);
					if ((building_type == BWAPI::UnitTypes::Terran_Command_Center ||
						building_type == BWAPI::UnitTypes::Protoss_Nexus ||
						building_type == BWAPI::UnitTypes::Zerg_Hatchery))
					{
						AIBase* closest_empty_base = game_state->getClosestEmptyBase();
						if (closest_empty_base != nullptr)
						{
							new_build_worker.setBuildType(building_type);
							new_build_worker.setBaseClass(base_class);
							new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
							new_build_worker.setTargetBase(closest_empty_base);
							game_state->getBuildWorkers()->push_back(new_build_worker);
							game_state->getMineralWorkers()->erase(mineral_worker_iterator);
							game_state->setTargetExpansion(closest_empty_base);
							return true;
						}
						else
							return false;
					}
					if (building_type != BWAPI::UnitTypes::Terran_Refinery &&
						building_type != BWAPI::UnitTypes::Protoss_Assimilator &&
						building_type != BWAPI::UnitTypes::Zerg_Extractor)
					{
						BWAPI::TilePosition build_position = game_state->getBuildLocation(building_type, *mineral_worker_iterator);
						//BWAPI::TilePosition build_position = this->getBuildLocation(*mineral_worker_iterator, building_type, game_state);
						//BWAPI::TilePosition build_position = Broodwar->getBuildLocation(building_type, (BWAPI::TilePosition)(*new_build_worker.getBase()->getArea()->Bases().begin()).Center());
						if (build_position != BWAPI::TilePositions::Invalid)
						{
							if (new_build_worker.getUnit()->build(building_type, build_position))
							{
								new_build_worker.setBuildType(building_type);
								new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
								new_build_worker.setTargetBuildLocation(build_position);
								game_state->getBuildWorkers()->push_back(new_build_worker);
								game_state->getMineralWorkers()->erase(mineral_worker_iterator);
								if (building_type.getRace() == BWAPI::Races::Protoss)
									game_state->updateBuildMap(build_position.x, build_position.y, building_type, true);
								return true;
							}
							else
							{
								new_build_worker.getUnit()->move((BWAPI::Position)build_position);
								new_build_worker.setBuildType(building_type);
								new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
								new_build_worker.setTargetBuildLocation(build_position);
								game_state->getBuildWorkers()->push_back(new_build_worker);
								game_state->getMineralWorkers()->erase(mineral_worker_iterator);
								if (building_type.getRace() == BWAPI::Races::Protoss)
									game_state->updateBuildMap(build_position.x, build_position.y, building_type, true);
								return true;
							}
						}
						else
						{
							auto end_time = std::chrono::high_resolution_clock::now();
							if (std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() > 20)
							{
								return false;
							}
							bad_areas.push_back(game_state->getContainingBase(mineral_worker_iterator->getUnit()->getTilePosition())->getArea());
							mineral_worker_iterator++;
						}
					}
					else
					{
						if (game_state->checkValidGasBuildTileLocation(base_class) &&
							BWEM::Map::Instance().GetArea(new_build_worker.getUnit()->getTilePosition()) != nullptr)
						{
							BWAPI::TilePosition build_position = game_state->getGasBuildTileLocation(BWEM::Map::Instance().GetArea(new_build_worker.getUnit()->getTilePosition()));
							if (build_position != BWAPI::TilePositions::Invalid)
							{
								if (new_build_worker.getUnit()->build(building_type, build_position))
								{
									new_build_worker.setBuildType(building_type);
									new_build_worker.setBaseClass(base_class);
									new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
									game_state->getBuildWorkers()->push_back(new_build_worker);
									game_state->getMineralWorkers()->erase(mineral_worker_iterator);
									return true;
								}
								else
								{
									new_build_worker.getUnit()->move((BWAPI::Position)build_position);
									new_build_worker.setBuildType(building_type);
									new_build_worker.setBaseClass(base_class);
									new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
									game_state->getBuildWorkers()->push_back(new_build_worker);
									game_state->getMineralWorkers()->erase(mineral_worker_iterator);
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
				else mineral_worker_iterator++;
			}
			else
			{
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

BWAPI::TilePosition WorkerManager::getBuildLocation(Object build_worker, BWAPI::UnitType building_type, std::unique_ptr<GameState> &game_state)
{
	std::time_t build_location_start = std::clock();
	bool has_addon = building_type.canBuildAddon();
	if (Broodwar->self()->getRace() == BWAPI::Races::Terran)
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
					if (x == Broodwar->mapWidth())
					{
						try_new_position = true;
						x = position_to_build.x + building_size.x + 1;
					}
					else
					{
						for (int y = position_to_build.y; y < position_to_build.y + building_size.y; y++)
						{
							if (y == Broodwar->mapHeight())
							{
								try_new_position = true;
								y = position_to_build.y + building_size.y;
								x = position_to_build.x + building_size.x;
							}
							else
							{
								if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
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
					if (x == Broodwar->mapWidth())
					{
						try_new_position = true;
						x = position_to_build.x + building_size.x;
					}
					else
					{
						for (int y = position_to_build.y; y < position_to_build.y + building_size.y; y++)
						{
							if (y == Broodwar->mapHeight())
							{
								try_new_position = true;
								y = position_to_build.y + building_size.y;
								x = position_to_build.x + building_size.x;
							}
							else
							{
								if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
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
						if (x == Broodwar->mapWidth())
						{
							try_new_position = true;
							x = position_to_build.x + building_size.x + 3;
						}
						else
						{
							for (int y = position_to_build.y + building_size.y - 2; y < position_to_build.y + building_size.y; y++)
							{
								if (y == Broodwar->mapHeight())
								{
									try_new_position = true;
									y = position_to_build.y + building_size.y;
									x = position_to_build.x + building_size.x;
								}
								else
								{
									if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
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
						if (x == Broodwar->mapWidth())
						{
							try_new_position = true;
							x = position_to_build.x + building_size.x + 2;
						}
						else
						{
							for (int y = position_to_build.y + building_size.y - 2; y < position_to_build.y + building_size.y; y++)
							{
								if (y == Broodwar->mapHeight())
								{
									try_new_position = true;
									y = position_to_build.y + building_size.y;
									x = position_to_build.x + building_size.x;
								}
								else
								{
									if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
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
					Broodwar->makeValid(position_to_build);
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
						else if (game_state->getContainingBase(position_to_build) != nullptr)
						{
							if (build_worker.getBase() == game_state->getContainingBase(position_to_build))
							{
								in_base = true;
							}
							else
							{
								if ((std::clock() - build_location_start) * 1000 > 350)
								{
									return BWAPI::TilePositions::Invalid;
								}
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
	/*else if (Broodwar->self()->getRace() == BWAPI::Races::Zerg)
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
	else if (Broodwar->self()->getRace() == BWAPI::Races::Protoss)
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
						if (x == Broodwar->mapWidth())
						{
							try_new_position = true;
							x = position_to_build.x + building_size.x + 1;
						}
						else
						{
							for (int y = position_to_build.y - 1; y < position_to_build.y + building_size.y + 1; y++)
							{
								if (y == Broodwar->mapHeight())
								{
									try_new_position = true;
									y = position_to_build.y + building_size.y + 1;
									x = position_to_build.x + building_size.y + 1;
								}
								else
								{
									if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
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
						if (x == Broodwar->mapWidth())
						{
							try_new_position = true;
							x = position_to_build.x + building_size.x;
						}
						else
						{
							for (int y = position_to_build.y; y < position_to_build.y + building_size.y; y++)
							{
								if (y == Broodwar->mapHeight())
								{
									try_new_position = true;
									x = position_to_build.x + building_size.x;
									y = position_to_build.y + building_size.y;
								}
								else
								{
									if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
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
            Broodwar->makeValid(position_to_build);
						if (!(BWEM::Map::Instance().GetArea(position_to_build) == nullptr))
						{
							if (BWEM::Map::Instance().GetArea(position_to_build)->Id() == build_worker.getBase()->getArea()->Id())
								in_base = true;
							else if (game_state->getContainingBase(position_to_build) != nullptr)
							{
								if (build_worker.getBase()->getBaseClass() == 3 &&
									game_state->getContainingBase(position_to_build)->getBaseClass() == 3)
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
					if (!Broodwar->hasPower(position_to_build, building_type))
						try_new_position = true;
					if (!try_new_position)
					{
						if (position_to_build.x != 0 && position_to_build.y != 0)
						{
							for (int x = position_to_build.x - 1; x < position_to_build.x + building_size.x + 1; x++)
							{
								if (x == Broodwar->mapWidth())
								{
									try_new_position = true;
									x = position_to_build.x + building_size.x + 1;
								}
								else
								{
									for (int y = position_to_build.y - 1; y < position_to_build.y + building_size.y + 1; y++)
									{
										if (y == Broodwar->mapHeight())
										{
											try_new_position = true;
											x = position_to_build.x + building_size.x + 1;
											y = position_to_build.y + building_size.y + 1;
										}
										else
										{
											if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
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
								if (x == Broodwar->mapWidth())
								{
									try_new_position = true;
									x = position_to_build.x + building_size.x;
								}
								else
								{
									for (int y = position_to_build.y; y < position_to_build.y + building_size.y; y++)
									{
										if (y == Broodwar->mapHeight())
										{
											try_new_position = true;
											x = position_to_build.x + building_size.x;
											y = position_to_build.y + building_size.y;
										}
										else
										{
											if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
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
              Broodwar->makeValid(position_to_build);

							if (!(BWEM::Map::Instance().GetArea(position_to_build) == nullptr))
							{
								if (BWEM::Map::Instance().GetArea(position_to_build)->Id() == build_worker.getBase()->getArea()->Id())
									in_base = true;
								else if (game_state->getContainingBase(position_to_build) != nullptr)
								{
									if (build_worker.getBase()->getBaseClass() == 3 &&
										game_state->getContainingBase(position_to_build)->getBaseClass() == 3)
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
				return Broodwar->getBuildLocation(building_type, build_worker.getUnit()->getTilePosition());
		}
	}
	else
	{
		return Broodwar->getBuildLocation(building_type, build_worker.getUnit()->getTilePosition());
	}
}

void WorkerManager::getNewBuildWorker(BWAPI::Unit building, std::unique_ptr<GameState> &game_state)
{
	auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
	while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
	{
		if (!mineral_worker_iterator->getUnit()->isCarryingMinerals())
		{
			mineral_worker_iterator->getUnit()->rightClick(building);
			if (building->getType() == BWAPI::UnitTypes::Terran_Command_Center)
			{
				mineral_worker_iterator->setBase(game_state->getContainingBase(building->getTilePosition()));
				mineral_worker_iterator->setBaseClass(4);
			}
			mineral_worker_iterator = game_state->getMineralWorkers()->end();
		}
		else
			mineral_worker_iterator++;
	}
}

bool WorkerManager::buildNearPositionInBase(BWAPI::UnitType building_type, BWAPI::TilePosition start_position, int base_class, std::unique_ptr<GameState> &game_state)
{
	auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
	while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
	{
		if (game_state->getContainingBase(mineral_worker_iterator->getUnit()->getTilePosition()) != nullptr)
		{
			bool temp_change_base_class = false;
			if (base_class == 1)
			{
				temp_change_base_class = true;
				base_class = 3;
			}
			if (mineral_worker_iterator->getBase()->getBaseClass() == base_class &&
				!mineral_worker_iterator->getUnit()->isCarryingMinerals() &&
				Broodwar->canMake(building_type, mineral_worker_iterator->getUnit()) &&
				game_state->getContainingBase(mineral_worker_iterator->getUnit()->getTilePosition())->getBaseClass() == base_class)
			{
				Object new_build_worker(*mineral_worker_iterator);
				if (temp_change_base_class == true)
					base_class = 1;
				if ((building_type == BWAPI::UnitTypes::Terran_Command_Center ||
					building_type == BWAPI::UnitTypes::Protoss_Nexus ||
					building_type == BWAPI::UnitTypes::Zerg_Hatchery) &&
					base_class == 1)
				{
					AIBase* closest_empty_base = game_state->getClosestEmptyBase();
					if (closest_empty_base != nullptr)
					{
						new_build_worker.setBuildType(building_type);
						new_build_worker.setBaseClass(base_class);
						new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
						new_build_worker.setTargetBase(closest_empty_base);
						game_state->getBuildWorkers()->push_back(new_build_worker);
						game_state->getMineralWorkers()->erase(mineral_worker_iterator);
						game_state->setTargetExpansion(closest_empty_base);
						return true;
					}
					else
						return false;
				}
				if (building_type != BWAPI::UnitTypes::Terran_Refinery &&
					building_type != BWAPI::UnitTypes::Protoss_Assimilator &&
					building_type != BWAPI::UnitTypes::Zerg_Extractor)
				{
					BWAPI::TilePosition build_position = this->getBuildLocation(*mineral_worker_iterator, building_type, start_position, game_state);
					//BWAPI::TilePosition build_position = Broodwar->getBuildLocation(building_type, (BWAPI::TilePosition)(*new_build_worker.getBase()->getArea()->Bases().begin()).Center());
					if (build_position != BWAPI::TilePositions::Invalid)
					{
						if (new_build_worker.getUnit()->build(building_type, build_position))
						{
							new_build_worker.setBuildType(building_type);
							new_build_worker.setBaseClass(base_class);
							new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
							new_build_worker.setTargetBuildLocation(build_position);
							game_state->getBuildWorkers()->push_back(new_build_worker);
							game_state->getMineralWorkers()->erase(mineral_worker_iterator);
							if (building_type.getRace() == BWAPI::Races::Protoss)
								game_state->updateBuildMap(build_position.x, build_position.y, building_type, true);
							return true;
						}
						else
						{
							new_build_worker.getUnit()->move((BWAPI::Position)build_position);
							new_build_worker.setBuildType(building_type);
							new_build_worker.setBaseClass(base_class);
							new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
							new_build_worker.setTargetBuildLocation(build_position);
							game_state->getBuildWorkers()->push_back(new_build_worker);
							game_state->getMineralWorkers()->erase(mineral_worker_iterator);
							if (building_type.getRace() == BWAPI::Races::Protoss)
								game_state->updateBuildMap(build_position.x, build_position.y, building_type, true);
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
					if (game_state->checkValidGasBuildTileLocation(base_class) &&
						BWEM::Map::Instance().GetArea(new_build_worker.getUnit()->getTilePosition()) != nullptr)
					{
						BWAPI::TilePosition build_position = game_state->getGasBuildTileLocation(BWEM::Map::Instance().GetArea(new_build_worker.getUnit()->getTilePosition()));
						if (build_position != BWAPI::TilePositions::Invalid)
						{
							if (new_build_worker.getUnit()->build(building_type, build_position))
							{
								new_build_worker.setBuildType(building_type);
								new_build_worker.setBaseClass(base_class);
								new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
								game_state->getBuildWorkers()->push_back(new_build_worker);
								game_state->getMineralWorkers()->erase(mineral_worker_iterator);
								return true;
							}
							else
							{
								new_build_worker.getUnit()->move((BWAPI::Position)build_position);
								new_build_worker.setBuildType(building_type);
								new_build_worker.setBaseClass(base_class);
								new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
								game_state->getBuildWorkers()->push_back(new_build_worker);
								game_state->getMineralWorkers()->erase(mineral_worker_iterator);
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

BWAPI::TilePosition WorkerManager::getBuildLocation(Object build_worker, BWAPI::UnitType building_type, BWAPI::TilePosition start_position, std::unique_ptr<GameState> &game_state)
{
	std::time_t build_location_start = std::clock();
	bool has_addon = building_type.canBuildAddon();
	if (Broodwar->self()->getRace() == BWAPI::Races::Terran)
	{
		BWAPI::TilePosition position_to_build;
		position_to_build = start_position;
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
					if (x == Broodwar->mapWidth())
					{
						try_new_position = true;
						x = position_to_build.x + building_size.x + 1;
					}
					else
					{
						for (int y = position_to_build.y; y < position_to_build.y + building_size.y; y++)
						{
							if (y == Broodwar->mapHeight())
							{
								try_new_position = true;
								y = position_to_build.y + building_size.y;
								x = position_to_build.x + building_size.x;
							}
							else
							{
								if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
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
				for (int x = position_to_build.x; x < position_to_build.x + building_size.x; x++)
				{
					if (x == Broodwar->mapWidth())
					{
						try_new_position = true;
						x = position_to_build.x + building_size.x;
					}
					else
					{
						for (int y = position_to_build.y; y < position_to_build.y + building_size.y; y++)
						{
							if (y == Broodwar->mapHeight())
							{
								try_new_position = true;
								y = position_to_build.y + building_size.y;
								x = position_to_build.x + building_size.x;
							}
							else
							{
								if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
								{
									try_new_position = true;
									y = position_to_build.y + building_size.y;
									x = position_to_build.x + building_size.x;
								}
							}
						}
					}
				}
				if (!try_new_position)
				{
					try_new_position = !validateFloodFill(position_to_build, building_type, game_state, game_state->getContainingBase(position_to_build));
				}
			}
			if (!try_new_position &&
				has_addon)
			{
				if (position_to_build.x != 0 && position_to_build.y != 0)
				{
					for (int x = position_to_build.x + building_size.x - 1; x < position_to_build.x + building_size.x + 3; x++)
					{
						if (x == Broodwar->mapWidth())
						{
							try_new_position = true;
							x = position_to_build.x + building_size.x + 3;
						}
						else
						{
							for (int y = position_to_build.y + building_size.y - 2; y < position_to_build.y + building_size.y; y++)
							{
								if (y == Broodwar->mapHeight())
								{
									try_new_position = true;
									y = position_to_build.y + building_size.y;
									x = position_to_build.x + building_size.x;
								}
								else
								{
									if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
									{
										try_new_position = true;
										x = position_to_build.x + building_size.x + 3;
										y = position_to_build.y + building_size.y;
									}
								}
							}
						}
					}
					if (!try_new_position)
					{
						try_new_position = !validateFloodFill(position_to_build, building_type, game_state, game_state->getContainingBase(position_to_build));
					}
				}
				else
				{
					for (int x = position_to_build.x + building_size.x; x < position_to_build.x + building_size.x + 2; x++)
					{
						if (x == Broodwar->mapWidth())
						{
							try_new_position = true;
							x = position_to_build.x + building_size.x;
						}
						else
						{
							for (int y = position_to_build.y + building_size.y - 2; y < position_to_build.y + building_size.y; y++)
							{
								if (y == Broodwar->mapHeight())
								{
									try_new_position = true;
									y = position_to_build.y + building_size.y;
									x = position_to_build.x + building_size.x;
								}
								else
								{
									if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
									{
										try_new_position = true;
										y = position_to_build.y + building_size.y;
										x = position_to_build.x + building_size.x + 2;
									}
								}
							}
						}
					}
					if (!try_new_position)
					{
						try_new_position = !validateFloodFill(position_to_build, building_type, game_state, game_state->getContainingBase(position_to_build));
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
          Broodwar->makeValid(position_to_build);
					const BWEM::Area* area_to_check = BWEM::Map::Instance().GetArea(position_to_build);
					if (area_to_check != nullptr)
					{
						if (BWEM::Map::Instance().GetArea(position_to_build) == nullptr)
						{
							in_base = false;
							position_to_build = start_position;
						}
						else if (BWEM::Map::Instance().GetArea(position_to_build)->Id() == build_worker.getBase()->getArea()->Id())
							in_base = true;
						else if (game_state->getContainingBase(position_to_build) != nullptr)
						{
							if (build_worker.getBase()->getBaseClass() == 3 &&
								game_state->getContainingBase(position_to_build)->getBaseClass() == 3)
							{
								in_base = true;
							}
							else
							{
								position_to_build = start_position;
							}
						}
						else
						{
							position_to_build = start_position;
						}
					}
					else
					{
						position_to_build = start_position;
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
	/*else if (Broodwar->self()->getRace() == BWAPI::Races::Zerg)
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
	else if (Broodwar->self()->getRace() == BWAPI::Races::Protoss)
	{
		if (building_type == BWAPI::UnitTypes::Protoss_Pylon)
		{
			BWAPI::TilePosition position_to_build;
			position_to_build = start_position;
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
						if (x == Broodwar->mapWidth())
						{
							try_new_position = true;
							x = position_to_build.x + building_size.x + 1;
						}
						else
						{
							for (int y = position_to_build.y - 1; y < position_to_build.y + building_size.y + 1; y++)
							{
								if (y == Broodwar->mapHeight())
								{
									try_new_position = true;
									y = position_to_build.y + building_size.y + 1;
									x = position_to_build.x + building_size.y + 1;
								}
								else
								{
									if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
									{
										try_new_position = true;
										y = position_to_build.y + building_size.y + 1;
										x = position_to_build.x + building_size.y + 1;
									}
								}
							}
						}
					}
					if (!try_new_position)
					{
						try_new_position = !validateFloodFill(position_to_build, building_type, game_state, game_state->getContainingBase(position_to_build));
					}
				}
				else
				{
					for (int x = position_to_build.x; x < position_to_build.x + building_size.x; x++)
					{
						if (x == Broodwar->mapWidth())
						{
							try_new_position = true;
							x = position_to_build.x + building_size.x;
						}
						else
						{
							for (int y = position_to_build.y; y < position_to_build.y + building_size.y; y++)
							{
								if (y == Broodwar->mapHeight())
								{
									try_new_position = true;
									x = position_to_build.x + building_size.x;
									y = position_to_build.y + building_size.y;
								}
								else
								{
									if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
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
					if (!try_new_position)
					{
						try_new_position = !validateFloodFill(position_to_build, building_type, game_state, game_state->getContainingBase(position_to_build));
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
            Broodwar->makeValid(position_to_build);
						if (!(BWEM::Map::Instance().GetArea(position_to_build) == nullptr))
						{
							if (BWEM::Map::Instance().GetArea(position_to_build)->Id() == build_worker.getBase()->getArea()->Id())
								in_base = true;
							else if (game_state->getContainingBase(position_to_build) != nullptr)
							{
								if (build_worker.getBase()->getBaseClass() == 3 &&
									game_state->getContainingBase(position_to_build)->getBaseClass() == 3)
								{
									in_base = true;
								}
								else
								{
									position_to_build = start_position;
								}
							}
							else
							{
								position_to_build = start_position;
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
				position_to_build = start_position;
				BWAPI::TilePosition building_size = building_type.tileSize();
				bool can_build = false;
				bool try_new_position = false;
				while (!can_build)
				{
					try_new_position = false;
					if (!Broodwar->hasPower(position_to_build, building_type))
						try_new_position = true;
					if (!try_new_position)
					{
						if (position_to_build.x != 0 && position_to_build.y != 0)
						{
							for (int x = position_to_build.x - 1; x < position_to_build.x + building_size.x + 1; x++)
							{
								if (x == Broodwar->mapWidth())
								{
									try_new_position = true;
									x = position_to_build.x + building_size.x + 1;
								}
								else
								{
									for (int y = position_to_build.y - 1; y < position_to_build.y + building_size.y + 1; y++)
									{
										if (y == Broodwar->mapHeight())
										{
											try_new_position = true;
											x = position_to_build.x + building_size.x + 1;
											y = position_to_build.y + building_size.y + 1;
										}
										else
										{
											if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
											{
												try_new_position = true;
												x = position_to_build.x + building_size.x + 1;
												y = position_to_build.y + building_size.y + 1;
											}
										}
									}
								}
							}
							if (!try_new_position)
							{
								try_new_position = !validateFloodFill(position_to_build, building_type, game_state, game_state->getContainingBase(position_to_build));
							}
						}
						else
						{
							for (int x = position_to_build.x; x < position_to_build.x + building_size.x; x++)
							{
								if (x == Broodwar->mapWidth())
								{
									try_new_position = true;
									x = position_to_build.x + building_size.x;
								}
								else
								{
									for (int y = position_to_build.y; y < position_to_build.y + building_size.y; y++)
									{
										if (y == Broodwar->mapHeight())
										{
											try_new_position = true;
											x = position_to_build.x + building_size.x;
											y = position_to_build.y + building_size.y;
										}
										else
										{
											if (!game_state->getBuildPositionMap()->at(x + (y * Broodwar->mapWidth())).first.unobstructed)
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
						if (!try_new_position)
						{
							try_new_position = !validateFloodFill(position_to_build, building_type, game_state, game_state->getContainingBase(position_to_build));
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
              Broodwar->makeValid(position_to_build);

							if (!(BWEM::Map::Instance().GetArea(position_to_build) == nullptr))
							{
								if (BWEM::Map::Instance().GetArea(position_to_build)->Id() == build_worker.getBase()->getArea()->Id())
									in_base = true;
								else if (game_state->getContainingBase(position_to_build) != nullptr)
								{
									if (build_worker.getBase()->getBaseClass() == 3 &&
										game_state->getContainingBase(position_to_build)->getBaseClass() == 3)
									{
										in_base = true;
									}
									else
									{
										position_to_build = start_position;
									}
								}
								else
								{
									position_to_build = start_position;
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
				return Broodwar->getBuildLocation(building_type, start_position);
		}
	}
	else
	{
		return Broodwar->getBuildLocation(building_type, start_position);
	}
}

bool WorkerManager::validateFloodFill(BWAPI::TilePosition build_position, BWAPI::UnitType building_type, std::unique_ptr<GameState> &game_state, AIBase* target_base)
{
	game_state->updateBuildMap(build_position.x, build_position.y, building_type, true);
	std::vector<std::pair<TileFlags, int>> copy_of_build_position_map = *game_state->getBuildPositionMap();
	const BWEM::Area* build_area = BWEM::Map::Instance().GetArea(build_position);
	BWAPI::TilePosition start_position = build_position;
	while (!copy_of_build_position_map.at(start_position.x + start_position.y * Broodwar->mapWidth()).first.unobstructed)
	{
		start_position.x += game_state->getRandomInteger(-1, 1);
		start_position.x += game_state->getRandomInteger(-1, 1);
    Broodwar->makeValid(start_position);
	}
	floodFill(start_position, &copy_of_build_position_map, game_state, target_base);
	BWAPI::TilePosition check_position;
	int obstructed_count = 0;
	for (int x = 0; x < Broodwar->mapWidth(); x++)
	{
		check_position.x = x;
		for (int y = 0; y < Broodwar->mapHeight(); y++)
		{
			check_position.y = 0;
			AIBase* current_base = game_state->getContainingBase(check_position);
			if (current_base == target_base)
			{
				if (!copy_of_build_position_map.at(x + y * Broodwar->mapWidth()).first.unobstructed)
					obstructed_count++;
			}
		}
	}
	game_state->updateBuildMap(build_position.x, build_position.y, building_type, false);
	return obstructed_count == target_base->getTileCount();
}

void WorkerManager::floodFill(BWAPI::TilePosition start_position, std::vector<std::pair<TileFlags, int>>* build_map, std::unique_ptr<GameState> &game_state, AIBase* target_base)
{
	if (Broodwar->isValid(start_position))
	{
		AIBase* position_base = game_state->getContainingBase(start_position);
		if (position_base != nullptr)
		{
			if (position_base == target_base)
			{
				TileFlags* current_flags = &build_map->at(start_position.x + start_position.y * Broodwar->mapWidth()).first;
				if (current_flags->unobstructed = true)
				{
					current_flags->unobstructed = false;
					BWAPI::TilePosition new_position = start_position;
					new_position.x--;
					floodFill(new_position, build_map, game_state, target_base);
					new_position.x++;
					new_position.y--;
					floodFill(new_position, build_map, game_state, target_base);
					new_position.x++;
					new_position.y++;
					floodFill(new_position, build_map, game_state, target_base);
					new_position.x--;
					new_position.y++;
					floodFill(new_position, build_map, game_state, target_base);
				}
			}
		}
	}
}

bool WorkerManager::buildAtPosition(BWAPI::UnitType building_type, std::unique_ptr<GameState> &game_state, BWAPI::TilePosition build_position)
{
	auto mineral_worker_iterator = game_state->getMineralWorkers()->begin();
	while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
	{
		if (game_state->getContainingBase(mineral_worker_iterator->getUnit()->getTilePosition()) != nullptr)
		{
			if (/*mineral_worker_iterator->getBase()->getBaseClass() == base_class &&*/
				!mineral_worker_iterator->getUnit()->isCarryingMinerals() &&
				Broodwar->canMake(building_type, mineral_worker_iterator->getUnit()) &&
				game_state->getContainingBase(mineral_worker_iterator->getUnit()->getTilePosition()) == game_state->getContainingBase(build_position))
			{
				Object new_build_worker(*mineral_worker_iterator);
				if (new_build_worker.getUnit()->build(building_type, build_position))
				{
					new_build_worker.setBuildType(building_type);
					new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
					new_build_worker.setTargetBuildLocation(build_position);
					game_state->getBuildWorkers()->push_back(new_build_worker);
					game_state->getMineralWorkers()->erase(mineral_worker_iterator);
					game_state->addUsedPosition(build_position);
					return true;
				}
				else
				{
					new_build_worker.getUnit()->move((BWAPI::Position)build_position);
					new_build_worker.setBuildType(building_type);
					new_build_worker.setTargetBuildLocation(build_position);
					new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
					game_state->getBuildWorkers()->push_back(new_build_worker);
					game_state->getMineralWorkers()->erase(mineral_worker_iterator);
					game_state->addUsedPosition(build_position);
					return true;
				}
			}
		}
		mineral_worker_iterator++;
	}
	mineral_worker_iterator = game_state->getMineralWorkers()->begin();
	while (mineral_worker_iterator != game_state->getMineralWorkers()->end())
	{
		if (game_state->getContainingBase(mineral_worker_iterator->getUnit()->getTilePosition()) != nullptr)
		{
			if (/*mineral_worker_iterator->getBase()->getBaseClass() == base_class &&*/
				!mineral_worker_iterator->getUnit()->isCarryingMinerals() &&
				Broodwar->canMake(building_type, mineral_worker_iterator->getUnit()))
			{
				Object new_build_worker(*mineral_worker_iterator);
				if (new_build_worker.getUnit()->build(building_type, build_position))
				{
					new_build_worker.setBuildType(building_type);
					new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
					new_build_worker.setTargetBuildLocation(build_position);
					game_state->getBuildWorkers()->push_back(new_build_worker);
					game_state->getMineralWorkers()->erase(mineral_worker_iterator);
					game_state->addUsedPosition(build_position);
					return true;
				}
				else
				{
					new_build_worker.getUnit()->move((BWAPI::Position)build_position);
					new_build_worker.setBuildType(building_type);
					new_build_worker.setTargetBuildLocation(build_position);
					new_build_worker.setElapsedTimeOrderGiven(Broodwar->elapsedTime());
					game_state->getBuildWorkers()->push_back(new_build_worker);
					game_state->getMineralWorkers()->erase(mineral_worker_iterator);
					game_state->addUsedPosition(build_position);
					return true;
				}
			}
		}
		mineral_worker_iterator++;
	}
	return false;
}