#include"GameState.h"

GameState::GameState()
{
	supply_total = 0;
	supply_used = 0;
	if (BWAPI::Broodwar->self()->getRace() != BWAPI::Races::Zerg)
		supply_expected = 10;
	else
		supply_expected = 0;
	minerals_committed = 0;
	barracks = 0;
	academy = false;
	gas = 0;
	gas_committed = 0;
	comsat_station = false;
	last_scan = 0;
	evolution_chambers = 0;
	build_order = "default";
	build_tanks = false;
}

void GameState::addAIBase(AIBase new_base)
{
	base_list.push_back(new_base);
}

void GameState::addBuilding(Object new_building)
{
	building_list.push_back(new_building);
}

void GameState::addSupplyUsed(double new_supply)
{
	supply_used += new_supply;
}

void GameState::addSupplyTotal(int new_supply)
{
	supply_total += new_supply;
}

void GameState::addMineralsCommitted(int new_minerals)
{
	minerals_committed += new_minerals;
}

void GameState::addSupplyExpected(int new_supply)
{
	supply_expected += new_supply;
}

void GameState::toggleAcademy()
{
	if (academy == true)
		academy = false;
	else
		academy = true;
}

AIBase* GameState::getContainingBase(BWAPI::Unit unit)
{
	auto base_list_iterator = base_list.begin();
	while (base_list_iterator != base_list.end())
	{
		if (base_list_iterator->getArea() == BWEM::Map::Instance().GetNearestArea(unit->getTilePosition()))
		{
			return &(*base_list_iterator);
		}
		else
		{
			base_list_iterator++;
		}
	}
	return nullptr;
}

AIBase* GameState::getContainingBase(BWAPI::TilePosition tile_position)
{
	auto base_list_iterator = base_list.begin();
	while (base_list_iterator != base_list.end())
	{
		if (base_list_iterator->getArea() == BWEM::Map::Instance().GetNearestArea(tile_position))
		{
			return &(*base_list_iterator);
		}
		else
		{
			base_list_iterator++;
		}
	}
	return nullptr;
}

std::vector<AIBase>* GameState::getBaseList()
{
	return &base_list;
}

double GameState::getSupplyUsed()
{
	return supply_used;
}

void GameState::addMineralWorker(Object new_worker)
{
	mineral_workers.push_back(new_worker);
}

Object GameState::getScout()
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

int GameState::getMineralWorkerCount()
{
	return mineral_workers.size();
}

std::vector<Object>* GameState::getBuildingList()
{
	return &building_list;
}

int GameState::getMineralsCommitted()
{
	return minerals_committed;
}

double GameState::getSupplyTotal()
{
	return supply_total;
}

int GameState::getBarracks()
{
	return barracks;
}

double GameState::getSupplyExpected()
{
	return supply_expected;
}

void GameState::addBarracks(int new_barracks)
{
	barracks += new_barracks;
	if (barracks < 0)
		barracks = 0;
}

std::map<int, Object>* GameState::getEnemyUnits()
{
	return &enemy_units;
}

std::vector<Object>* GameState::getMineralWorkers()
{
	return &mineral_workers;
}

std::vector<Object>* GameState::getBuildWorkers()
{
	return &build_workers;
}

void GameState::addGas(int new_gas)
{
	gas += new_gas;
}

int GameState::getGas()
{
	return gas;
}

void GameState::initializeGasLocations()
{
	BWAPI::Unitset geyser_list = BWAPI::Broodwar->getStaticGeysers();
	auto geyser_list_iterator = geyser_list.begin();
	while (geyser_list_iterator != geyser_list.end())
	{
		std::pair<bool, BWAPI::TilePosition> new_geyser;
		new_geyser.first = false;
		new_geyser.second = (*geyser_list_iterator)->getTilePosition();
		gas_locations.push_back(new_geyser);
		geyser_list_iterator++;
	}
}

BWAPI::TilePosition GameState::getGasBuildTileLocation()
{
	auto gas_location_iterator = gas_locations.begin();
	while (gas_location_iterator != gas_locations.end())
	{
		if (gas_location_iterator->first == false &&
			getContainingBase(gas_location_iterator->second)->getBaseClass() == 3)
		{
			return gas_location_iterator->second;
		}
		else
		{
			gas_location_iterator++;
		}
	}
	return BWAPI::TilePositions::Invalid;
}
bool GameState::checkValidGasBuildTileLocation()
{
	auto gas_location_iterator = gas_locations.begin();
	while (gas_location_iterator != gas_locations.end())
	{
		if (gas_location_iterator->first == false &&
			getContainingBase(gas_location_iterator->second)->getBaseClass() == 3)
		{
			return true;
		}
		else
		{
			gas_location_iterator++;
		}
	}
	return false;
}

void GameState::setGeyserUsed(BWAPI::TilePosition geyser_position)
{
	auto gas_location_iterator = gas_locations.begin();
	while (gas_location_iterator != gas_locations.end())
	{
		if (gas_location_iterator->first == false &&
			gas_location_iterator->second == geyser_position)
		{
			gas_location_iterator->first = true;
			return;
		}
		else
		{
			gas_location_iterator++;
		}
	}
}

void GameState::setGeyserOpen(BWAPI::TilePosition geyser_position)
{
	auto gas_location_iterator = gas_locations.begin();
	while (gas_location_iterator != gas_locations.end())
	{
		if (gas_location_iterator->first == true &&
			gas_location_iterator->second == geyser_position)
		{
			gas_location_iterator->first = false;
			return;
		}
		else
		{
			gas_location_iterator++;
		}
	}
}

int GameState::getGasCommitted()
{
	return gas_committed;
}

void GameState::addGasCommitted(int new_gas)
{
	gas_committed += new_gas;
	if (gas_committed < 0)
		gas_committed = 0;
}

bool GameState::checkAcademy()
{
	return academy;
}

void GameState::toggleComsatStation()
{
	if (comsat_station == false)
	{
		comsat_station = true;
	}
	else
		comsat_station = false;
}

bool GameState::checkComsatStation()
{
	return comsat_station;
}

void GameState::setLastScan(int new_scan)
{
	last_scan = new_scan;
}

int GameState::getLastScan()
{
	return last_scan;
}

AIBase* GameState::getClosestEnemyBase()
{
	checkBaseOwnership();
	auto base_list_iterator = base_list.begin();
	auto main_base = base_list_iterator;
	auto last_enemy_base_found = base_list_iterator;
	bool found_main = false;
	std::vector<AIBase*> enemy_base_list;
	while (base_list_iterator != base_list.end())
	{
		if (base_list_iterator->getBaseClass() == 3)
		{
			main_base = base_list_iterator;
			found_main = true;
			base_list_iterator++;
		}
		else if (base_list_iterator->getBaseClass() == 2)
		{
			enemy_base_list.push_back(&(*base_list_iterator));
			base_list_iterator++;
		}
		else
		{
			base_list_iterator++;
		}
	}
	if (enemy_base_list.size() == 0)
	{
		return nullptr;
	}
	else if (enemy_base_list.size() == 1)
	{
		return (*enemy_base_list.begin());
	}
	else if (found_main == true)
	{
		auto enemy_base_iterator = enemy_base_list.begin();
		auto closest_base = enemy_base_iterator;
		BWEM::CPPath closest_path = BWEM::Map::Instance().GetPath(BWAPI::Position((*closest_base)->getArea()->Top()), BWAPI::Position(main_base->getArea()->Top()));
		BWEM::CPPath path_to_check;
		while (enemy_base_iterator != enemy_base_list.end())
		{
			path_to_check = BWEM::Map::Instance().GetPath(BWAPI::Position((*enemy_base_iterator)->getArea()->Top()), BWAPI::Position(main_base->getArea()->Top()));
			if (path_to_check.size() < closest_path.size())
			{
				closest_path = path_to_check;
				closest_base = enemy_base_iterator;
			}
			else if (path_to_check.size() == closest_path.size())
			{
				if ((*enemy_base_iterator)->getArea()->Top().getApproxDistance(main_base->getArea()->Top()) < (*closest_base)->getArea()->Top().getApproxDistance(main_base->getArea()->Top()))
				{
					closest_path = path_to_check;
					closest_base = enemy_base_iterator;
				}
			}
			enemy_base_iterator++;
		}
		return *closest_base;
	}
	else
	{
		return nullptr;
	}
}

void GameState::checkBaseOwnership()
{

	auto base_list_iterator = base_list.begin();
	while (base_list_iterator != base_list.end())
	{
		if (base_list_iterator->getBaseClass() != 3)
		{
			base_list_iterator->setBaseClass(1);
			for (auto unit : building_list)
			{
				if (base_list_iterator->getArea()->Id() == BWEM::Map::Instance().GetNearestArea(unit.getUnit()->getTilePosition())->Id())
				{
					base_list_iterator->setBaseClass(4);
					break;
				}
			}
			if (base_list_iterator->getBaseClass() == 1)
			{
				for (auto unit : BWAPI::Broodwar->allies().getUnits())
				{
					if (unit->getType().isBuilding())
					{
						if (base_list_iterator->getArea()->Id() == BWEM::Map::Instance().GetNearestArea(unit->getTilePosition())->Id())
						{
							base_list_iterator->setBaseClass(5);
							break;
						}
					}
				}
			}
			if (base_list_iterator->getBaseClass() == 1)
			{
				for (auto unit : enemy_units)
				{
					if (unit.second.isBuilding())
					{
						if (base_list_iterator->getArea()->Id() == BWEM::Map::Instance().GetNearestArea(unit.second.getDiscoveredPosition())->Id())
						{
							base_list_iterator->setBaseClass(2);
							break;
						}
					}
				}
			}
		}
		base_list_iterator++;
	}
}

int GameState::getUnitTypeCount(BWAPI::UnitType type_to_check)
{
	int count_of_type = 0;
	auto military_iterator = military.begin();
	while (military_iterator != military.end())
	{
		if (military_iterator->getUnit()->getType() == type_to_check)
			count_of_type++;
		military_iterator++;
	}
	return count_of_type;
}

void GameState::addUnit(Object new_unit)
{
	military.push_back(new_unit);
	objective_list.begin()->addUnit(new_unit);
}

std::vector<Object>* GameState::getMilitary()
{
	return &military;
}

void GameState::addLarva(Object new_larva)
{
	larva.push_back(new_larva);
}

std::vector<Object>* GameState::getLarva()
{
	return &larva;
}

int GameState::getBuildingTypeCount(BWAPI::UnitType type_to_check)
{
	int count_of_type = 0;
	auto building_list_iterator = building_list.begin();
	while (building_list_iterator != building_list.end())
	{
		if (building_list_iterator->getUnit()->getType() == type_to_check)
			count_of_type++;
		building_list_iterator++;
	}
	return count_of_type;
}

void GameState::addDetector(Object new_detector)
{
	detectors.push_back(new_detector);
}

Object* GameState::getAvailableDetector()
{
	if (detectors.size() == 0)
		return nullptr;
	auto detector_iterator = detectors.begin();
	while (detector_iterator != detectors.end())
	{
		bool detector_free = true;
		if (!detector_iterator->getUnit()->exists())
		{
			auto erase_iterator = detector_iterator;
			detector_iterator = detectors.erase(erase_iterator);
		}
		else if (!detector_iterator->getUnit()->isCompleted())
			detector_iterator++;
		else
		{
			if (!detector_iterator->getUnit()->isIdle())
			{
				BWAPI::Unitset units_in_range_of_target = BWAPI::Broodwar->getUnitsInRadius(detector_iterator->getUnit()->getTargetPosition(), detector_iterator->getUnit()->getType().sightRange());
				auto unit_iterator = units_in_range_of_target.begin();
				while (unit_iterator != units_in_range_of_target.end())
				{
					if ((*unit_iterator)->getPlayer()->isEnemy(BWAPI::Broodwar->self()) &&
						(*unit_iterator)->isCloaked())
					{
						unit_iterator = units_in_range_of_target.end();
						detector_free = false;
					}
					else
					{
						unit_iterator++;
					}
				}
			}
			if (detector_free == true &&
				detector_iterator->getUnit()->getPosition() != BWAPI::Positions::Invalid)
			{
				BWAPI::Unitset units_in_range_of_detector = BWAPI::Broodwar->getUnitsInRadius(detector_iterator->getUnit()->getPosition(), detector_iterator->getUnit()->getType().sightRange());
				if (units_in_range_of_detector.size() == 0)
					return &(*detector_iterator);
				auto unit_iterator = units_in_range_of_detector.begin();
				while (unit_iterator != units_in_range_of_detector.end())
				{
					if ((*unit_iterator)->getPlayer()->isEnemy(BWAPI::Broodwar->self()) &&
						(*unit_iterator)->isCloaked())
					{
						unit_iterator = units_in_range_of_detector.end();
						detector_free = false;
					}
					else
					{
						unit_iterator++;
					}
				}
			}
			if (detector_free)
				return &(*detector_iterator);
			else
				detector_iterator++;
		}
	}
	return nullptr;
}

BWAPI::Position GameState::getRandomUncontrolledPosition()
{
	checkBaseOwnership();
	BWAPI::Position random_position;
	while (true)
	{
		random_position.x = rand() % BWAPI::Broodwar->mapWidth() * 32;
		random_position.y = rand() % BWAPI::Broodwar->mapHeight() * 32;
		auto base_list_iterator = base_list.begin();
		while (base_list_iterator != base_list.end())
		{
			if (BWEM::Map::Instance().GetNearestArea((BWAPI::TilePosition)random_position)->Id() == base_list_iterator->getArea()->Id() &&
				base_list_iterator->getBaseClass() != 3 &&
				base_list_iterator->getBaseClass() != 4 &&
				base_list_iterator->getBaseClass() != 5)
				return random_position;
			else
				base_list_iterator++;
		}
	}
}

void GameState::removeEnemyUnitsAtTilePosition(BWAPI::TilePosition target_position)
{
	auto enemy_iterator = enemy_units.begin();
	while (enemy_iterator != enemy_units.end())
	{
		if (enemy_iterator->second.getDiscoveredPosition() == target_position)
		{
			enemy_units.erase(enemy_iterator);
			enemy_iterator = enemy_units.end();
		}
		else
		{
			enemy_iterator++;
		}
	}
}

int GameState::getDetectorCount()
{
	return detectors.size();
}

void GameState::addEvolutionChambers(int new_evolution_chambers)
{
	evolution_chambers += new_evolution_chambers;
}

int GameState::getEvolutionChambers()
{
	return evolution_chambers;
}

void GameState::setBuildOrder(std::string new_build_order)
{
	build_order = new_build_order;
}

std::string GameState::getBuildOrder()
{
	return build_order;
}

bool GameState::getBuildTanks()
{
	return build_tanks;
}

void GameState::toggleBuildTanks()
{
	if (build_tanks == true)
		build_tanks = false;
	else
		build_tanks = true;
}

std::vector<Objective>* GameState::getObjectiveList()
{
	return &objective_list;
}

void GameState::addObjective(Objective new_objective)
{
	objective_list.push_back(new_objective);
}

void GameState::assessGame()
{
	if (objective_list.size() >= 1)
	{
		if (objective_list.size() > 1 &&
			BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran &&
			build_order == "build2" &&
			barracks >= 1)
		{
			bool lift_barracks = false;
			auto current_objective = objective_list.begin();
			current_objective++;
			while (current_objective != objective_list.end())
			{
				auto current_unit = current_objective->getUnits()->begin();
				while (current_unit != current_objective->getUnits()->end())
				{
					if (current_unit->getUnit()->exists())
					{
						if (getContainingBase(current_unit->getUnit()->getTilePosition())->getBaseClass() == 3)
						{
							current_unit = current_objective->getUnits()->end();
							lift_barracks = true;
						}
						else
						{
							current_unit++;
						}
					}
					else
						current_unit++;
				}
				if (lift_barracks)
					current_objective = objective_list.end();
				else
					current_objective++;
			}
			auto current_building = building_list.begin();
			while (current_building != building_list.end())
			{
				if (current_building->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks &&
					!current_building->getUnit()->isFlying() &&
					lift_barracks == true)
				{
					current_building->getUnit()->lift();
					current_building = building_list.end();
				}
				else if (current_building->getUnit()->getType() == BWAPI::UnitTypes::Terran_Barracks &&
					current_building->getUnit()->isFlying() &&
					lift_barracks == false)
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

					current_building->getUnit()->land(position_to_land);
					current_building = building_list.end();
				}
				else
					current_building++;
			}
		}
		if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
		{
			if (build_order == "default" &&
				objective_list.begin()->getUnits()->size() > 50)
			{
				Objective new_objective;
				new_objective.setObjective(ObjectiveTypes::Attack);
				auto current_unit = objective_list.begin()->getUnits()->begin();
				while (current_unit != objective_list.begin()->getUnits()->end())
				{
					new_objective.addUnit(*current_unit);
					current_unit++;
				}
				objective_list.begin()->getUnits()->clear();
				objective_list.push_back(new_objective);
			}
			else if (build_order == "build2" &&
				objective_list.begin()->getUnits()->size() > 24)
			{
				Objective new_objective;
				new_objective.setObjective(ObjectiveTypes::Attack);
				auto current_unit = objective_list.begin()->getUnits()->begin();
				while (current_unit != objective_list.begin()->getUnits()->end())
				{
					new_objective.addUnit(*current_unit);
					current_unit++;
				}
				objective_list.begin()->getUnits()->clear();
				objective_list.push_back(new_objective);
			}
		}
		else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
		{
			if (build_order == "default" &&
				objective_list.begin()->getUnits()->size() > 30)
			{
				Objective new_objective;
				new_objective.setObjective(ObjectiveTypes::Attack);
				auto current_unit = objective_list.begin()->getUnits()->begin();
				while (current_unit != objective_list.begin()->getUnits()->end())
				{
					new_objective.addUnit(*current_unit);
					current_unit++;
				}
				objective_list.begin()->getUnits()->clear();
				objective_list.push_back(new_objective);
			}
		}
		else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
		{
			if (build_order == "default" &&
				objective_list.begin()->getUnits()->size() > 100)
			{
				Objective new_objective;
				new_objective.setObjective(ObjectiveTypes::Attack);
				auto current_unit = objective_list.begin()->getUnits()->begin();
				while (current_unit != objective_list.begin()->getUnits()->end())
				{
					new_objective.addUnit(*current_unit);
					current_unit++;
				}
				objective_list.begin()->getUnits()->clear();
				objective_list.push_back(new_objective);
			}
		}
	}
}