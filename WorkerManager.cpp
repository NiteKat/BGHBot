#include"WorkerManager.h"

int WorkerManager::manageWorkers()
{
	int minerals_committed_adjust = 0;
	auto mineral_worker_iterator = mineral_workers.begin();
	while (mineral_worker_iterator != mineral_workers.end())
	{
		if (mineral_worker_iterator->getUnit() == nullptr)
		{
			auto erase_iterator = mineral_worker_iterator;
			mineral_worker_iterator = mineral_workers.erase(erase_iterator);
		}
		else if (!mineral_worker_iterator->getUnit()->exists())
		{
			auto erase_iterator = mineral_worker_iterator;
			mineral_worker_iterator = mineral_workers.erase(erase_iterator);
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

	auto build_worker_iterator = build_workers.begin();
	while (build_worker_iterator != build_workers.end())
	{
		if (build_worker_iterator->getUnit() == nullptr)
		{
			auto erase_iterator = build_worker_iterator;
			build_worker_iterator = build_workers.erase(erase_iterator);
		}
		else if (!build_worker_iterator->getUnit()->exists())
		{
			auto erase_iterator = build_worker_iterator;
			build_worker_iterator = build_workers.erase(erase_iterator);
		}
		else if (build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::PlaceBuilding &&
			build_worker_iterator->getUnit()->getOrder() != BWAPI::Orders::ConstructingBuilding)
		{
			BWAPI::Broodwar << "DEBUG: Order = " << build_worker_iterator->getUnit()->getOrder().toString() << std::endl;
			BWAPI::TilePosition build_position = getBuildLocation(*build_worker_iterator, build_worker_iterator->getBuildType());//BWAPI::Broodwar->getBuildLocation(build_worker_iterator->getBuildType(), (BWAPI::TilePosition)(*build_worker_iterator->getBase()->getArea()->Bases().begin()).Center());
			build_worker_iterator->getUnit()->build(build_worker_iterator->getBuildType(), build_position);
			build_worker_iterator++;
		}
		else if (build_worker_iterator->getUnit()->getOrder() == BWAPI::Orders::ConstructingBuilding)
		{
			Object new_mineral_worker(*build_worker_iterator);
			new_mineral_worker.setBuildType(BWAPI::UnitTypes::Unknown);
			new_mineral_worker.setBaseClass(0);
			mineral_workers.push_back(new_mineral_worker);
			auto erase_iterator = build_worker_iterator;
			build_worker_iterator = build_workers.erase(erase_iterator);
		}
		else
		{
			build_worker_iterator++;
		}
	}

	return minerals_committed_adjust;
}

void WorkerManager::addMineralWorker(Object new_worker)
{
	mineral_workers.push_back(new_worker);
}

bool WorkerManager::build(BWAPI::UnitType building_type, int base_class)
{
	auto mineral_worker_iterator = mineral_workers.begin();
	while (mineral_worker_iterator != mineral_workers.end())
	{
		if (mineral_worker_iterator->getBase()->getBaseClass() == base_class &&
			!mineral_worker_iterator->getUnit()->isCarryingMinerals() &&
			BWAPI::Broodwar->canMake(building_type, mineral_worker_iterator->getUnit()))
		{
			Object new_build_worker(*mineral_worker_iterator);
			BWAPI::TilePosition build_position = this->getBuildLocation(*mineral_worker_iterator, building_type);
			//BWAPI::TilePosition build_position = BWAPI::Broodwar->getBuildLocation(building_type, (BWAPI::TilePosition)(*new_build_worker.getBase()->getArea()->Bases().begin()).Center());
			if (build_position != BWAPI::TilePositions::Invalid)
			{
				if (new_build_worker.getUnit()->build(building_type, build_position))
				{
					new_build_worker.setBuildType(building_type);
					new_build_worker.setBaseClass(base_class);
					build_workers.push_back(new_build_worker);
					mineral_workers.erase(mineral_worker_iterator);
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
			mineral_worker_iterator++;
		}
	}
	return false;
}

BWAPI::TilePosition WorkerManager::getBuildLocation(Object build_worker, BWAPI::UnitType building_type)
{
	BWAPI::TilePosition position_to_try = build_worker.getBase()->getArea()->Bases().begin()->Location();
	bool too_close = false;
	while (true)
	{
		position_to_try.x += rand() % 6 - 3;
		if (position_to_try.x < 0)
		{
			position_to_try.x = 0;
		}
		if (position_to_try.x > BWAPI::Broodwar->mapWidth())
		{
			position_to_try.x = BWAPI::Broodwar->mapWidth();
		}
		position_to_try.y += rand() % 6 - 3;
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
					unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field_Type_3) &&
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

Object WorkerManager::getScout()
{
	auto mineral_worker_iterator = mineral_workers.begin();
	while (mineral_worker_iterator != mineral_workers.end())
	{
		if (mineral_worker_iterator->getBase()->getBaseClass() == 3 &&
			!mineral_worker_iterator->getUnit()->isCarryingMinerals())
		{
			Object new_scout(*mineral_worker_iterator);
			mineral_workers.erase(mineral_worker_iterator);
			return new_scout;
		}
		else
		{
			mineral_worker_iterator++;
		}
	}
	Object new_scout;
	return new_scout;
}

int WorkerManager::getMineralWorkerCount()
{
	return mineral_workers.size();
}