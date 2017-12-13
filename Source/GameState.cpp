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
	build_order = BuildOrder::Default;
	build_tanks = false;
	worker_defense = false;
	expanding = false;
	target_expansion = nullptr;
	secondary_scouting = false;
	last_time_expanded = 0;
	cyber_core = false;
	robotics_facility = 0;
	observatory = 0;
	supply_built = 4;
	pylon = 0;
	factory = 0;
	bunker = 0;
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
		if (base_list_iterator->getArea() == BWEM::Map::Instance().GetArea(unit->getTilePosition()))
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
	if (BWEM::Map::Instance().GetArea(tile_position) != nullptr)
	{
		auto base_list_iterator = base_list.begin();
		while (base_list_iterator != base_list.end())
		{
			if (base_list_iterator->getArea() == BWEM::Map::Instance().GetArea(tile_position))
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
	else
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

BWAPI::TilePosition GameState::getGasBuildTileLocation(const BWEM::Area* area)
{
	auto gas_location_iterator = gas_locations.begin();
	while (gas_location_iterator != gas_locations.end())
	{
		if (getContainingBase(gas_location_iterator->second) != nullptr)
		{
			if (gas_location_iterator->first == false &&
				getContainingBase(gas_location_iterator->second)->getArea() == area)
			{
				return gas_location_iterator->second;
			}
			else
			{
				gas_location_iterator++;
			}
		}
		else gas_location_iterator++;
	}
	return BWAPI::TilePositions::Invalid;
}
bool GameState::checkValidGasBuildTileLocation(int base_class)
{
	auto gas_location_iterator = gas_locations.begin();
	while (gas_location_iterator != gas_locations.end())
	{
		if (getContainingBase(gas_location_iterator->second) != nullptr)
		{
			if (gas_location_iterator->first == false &&
				getContainingBase(gas_location_iterator->second)->getBaseClass() == base_class)
			{
				return true;
			}
			else
			{
				gas_location_iterator++;
			}
		}
		else
			gas_location_iterator++;
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
	auto base_list_iterator = base_list.begin();
	auto main_base = base_list_iterator;
	auto last_enemy_base_found = base_list_iterator;
	bool found_main = false;
	int closest_distance = 0;
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
		closest_distance = getGroundDistance(BWAPI::Position(main_base->getArea()->Top()), BWAPI::Position((*closest_base)->getArea()->Top()));
		enemy_base_iterator++;
		int distance_to_check = 0;
		while (enemy_base_iterator != enemy_base_list.end())
		{
			distance_to_check = getGroundDistance(BWAPI::Position(main_base->getArea()->Top()), BWAPI::Position((*enemy_base_iterator)->getArea()->Top()));
			if (distance_to_check < closest_distance &&
				distance_to_check > 0)
			{
				closest_distance = distance_to_check;
				closest_base = enemy_base_iterator;
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
				if (unit.getUnit()->exists())
				{
					if (BWEM::Map::Instance().GetArea(unit.getUnit()->getTilePosition()) != nullptr)
					{
						if (base_list_iterator->getArea()->Id() == BWEM::Map::Instance().GetArea(unit.getUnit()->getTilePosition())->Id())
						{
							base_list_iterator->setBaseClass(4);
							break;
						}
					}
				}
			}
			if (base_list_iterator->getBaseClass() == 1)
			{
				for (auto unit : BWAPI::Broodwar->allies().getUnits())
				{
					if (unit->exists())
					{
						if (unit->getType().isBuilding())
						{
							if (BWEM::Map::Instance().GetArea(unit->getTilePosition()) != nullptr)
							{
								if (base_list_iterator->getArea()->Id() == BWEM::Map::Instance().GetArea(unit->getTilePosition())->Id())
								{
									base_list_iterator->setBaseClass(5);
									break;
								}
							}
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
						if (unit.second.getDiscoveredPosition() != BWAPI::TilePositions::Invalid ||
							unit.second.getDiscoveredPosition() != BWAPI::TilePositions::None ||
							unit.second.getDiscoveredPosition() != BWAPI::TilePositions::Origin ||
							unit.second.getDiscoveredPosition() != BWAPI::TilePositions::Unknown)
						{
							if (BWEM::Map::Instance().GetArea(unit.second.getDiscoveredPosition()) != nullptr)
							{
								if (base_list_iterator->getArea()->Id() == BWEM::Map::Instance().GetArea(unit.second.getDiscoveredPosition())->Id())
								{
									base_list_iterator->setBaseClass(2);
									break;
								}
							}
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
		if (military_iterator->second.getUnit()->getType() == type_to_check)
			count_of_type++;
		military_iterator++;
	}
	return count_of_type;
}

void GameState::addUnit(Object new_unit)
{
	military.insert(std::make_pair(new_unit.getUnit()->getID(), new_unit));
	objective_list.begin()->addUnit(new_unit);
}

std::map<int, Object>* GameState::getMilitary()
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
				BWAPI::Unitset units_in_range_of_target = BWAPI::Broodwar->getUnitsInRadius(detector_iterator->getUnit()->getTargetPosition(), detector_iterator->getUnit()->getType().sightRange(), (BWAPI::Filter::IsEnemy && BWAPI::Filter::IsCloaked) || (BWAPI::Filter::IsEnemy && BWAPI::Filter::IsBurrowed));
				if (units_in_range_of_target.size() > 0)
					detector_free = false;
			}
			if (detector_free == true &&
				detector_iterator->getUnit()->getPosition() != BWAPI::Positions::Invalid)
			{
				BWAPI::Unitset units_in_range_of_detector = BWAPI::Broodwar->getUnitsInRadius(detector_iterator->getUnit()->getPosition(), detector_iterator->getUnit()->getType().sightRange(), (BWAPI::Filter::IsEnemy && BWAPI::Filter::IsCloaked) || (BWAPI::Filter::IsEnemy && BWAPI::Filter::IsBurrowed));
				if (units_in_range_of_detector.size() == 0)
					return &(*detector_iterator);
				else
						detector_free = false;
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
	BWAPI::Position random_position;
	while (true)
	{
		random_position.x = rand() % BWAPI::Broodwar->mapWidth() * 32;
		random_position.y = rand() % BWAPI::Broodwar->mapHeight() * 32;
		auto base_list_iterator = base_list.begin();
		const BWEM::Area* area_to_check = BWEM::Map::Instance().GetArea((BWAPI::TilePosition)random_position);
		if (area_to_check != nullptr)
		{
			while (base_list_iterator != base_list.end())
			{
				if (BWEM::Map::Instance().GetArea((BWAPI::TilePosition)random_position)->Id() == base_list_iterator->getArea()->Id() &&
					base_list_iterator->getBaseClass() != 3 &&
					base_list_iterator->getBaseClass() != 4 &&
					base_list_iterator->getBaseClass() != 5)
					return random_position;
				else
					base_list_iterator++;
			}
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

void GameState::setBuildOrder(BuildOrder new_build_order)
{
	build_order = new_build_order;
}

BuildOrder GameState::getBuildOrder()
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
	checkBaseOwnership();
	if (minerals_committed < 0)
		minerals_committed = 0;
	if (objective_list.size() >= 1)
	{
		if (expanding)
		{
			bool objective_exists = false;
			auto current_objective = objective_list.begin();
			while (current_objective != objective_list.end())
			{
				if (current_objective->getObjective() == ObjectiveTypes::DefendExpansion)
					objective_exists = true;
				current_objective++;
			}
			if (!objective_exists)
			{
				Objective new_objective;
				new_objective.setObjective(ObjectiveTypes::DefendExpansion);
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
		if (build_order == BuildOrder::P2Gate1 &&
			objective_list.begin()->getUnits()->size() > 0)
		{
			bool objective_exists = false;
			auto current_objective = objective_list.begin();
			while (current_objective != objective_list.end())
			{
				if (current_objective->getObjective() == ObjectiveTypes::P2GateAttack)
					objective_exists = true;
				current_objective++;
			}
			if (!objective_exists)
			{
				Objective new_objective;
				new_objective.setObjective(ObjectiveTypes::P2GateAttack);
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
		else if (build_order == BuildOrder::P4GateGoonOpening &&
			getUnitTypeCount(BWAPI::UnitTypes::Protoss_Dragoon) >= 6)
		{
			Objective new_objective;
			new_objective.setObjective(ObjectiveTypes::P4GateGoonAttack);
			auto current_unit = objective_list.begin()->getUnits()->begin();
			while (current_unit != objective_list.begin()->getUnits()->end())
			{
				new_objective.addUnit(*current_unit);
				current_unit++;
			}
			objective_list.begin()->getUnits()->clear();
			objective_list.push_back(new_objective);
			build_order = BuildOrder::P4GateGoonMid;
		}
		if (build_order == BuildOrder::P4GateGoonMid &&
			getBuildingTypeCount(BWAPI::UnitTypes::Protoss_Nexus) >= 3)
		{
			build_order = BuildOrder::P4GateGoonLate;
		}
		if (objective_list.size() > 1 &&
			BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran &&
			build_order == BuildOrder::BGHMech &&
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
			if (build_order == BuildOrder::Default &&
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
			else if (build_order == BuildOrder::BGHMech &&
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
			if ((build_order == BuildOrder::Default ||
				build_order == BuildOrder::P4GateGoonMid ||
				build_order == BuildOrder::P4GateGoonLate) &&
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
			if (build_order == BuildOrder::Default &&
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

void GameState::initializeBuildMap()
{
	for (int y = 0; y < BWAPI::Broodwar->mapHeight(); y++)
	{
		for (int x = 0; x < BWAPI::Broodwar->mapWidth(); x++)
		{
			BWAPI::TilePosition position_to_check;
			position_to_check.x = x;
			position_to_check.y = y;
			if (BWAPI::Broodwar->isBuildable(position_to_check))
			{
				std::pair<TileFlags, int> new_coordinate;
				new_coordinate.first.unobstructed = true;
				new_coordinate.first.pylon_power_2x2 = false;
				new_coordinate.first.pylon_power_3x2 = false;
				new_coordinate.first.pylon_power_4x3 = false;
				if (BWEM::Map::Instance().GetArea(position_to_check) != nullptr)
					new_coordinate.second = BWEM::Map::Instance().GetArea(position_to_check)->Id();
				else
					new_coordinate.second = -1;
				build_position_map.push_back(new_coordinate);
			}
			else
			{
				std::pair<TileFlags, int> new_coordinate;
				new_coordinate.first.unobstructed = false;
				new_coordinate.first.pylon_power_2x2 = false;
				new_coordinate.first.pylon_power_3x2 = false;
				new_coordinate.first.pylon_power_4x3 = false;
				if (BWEM::Map::Instance().GetArea(position_to_check) != nullptr)
					new_coordinate.second = BWEM::Map::Instance().GetArea(position_to_check)->Id();
				else
					new_coordinate.second = -1;
				build_position_map.push_back(new_coordinate);
			}
		}
	}
}

void GameState::updateBuildMap(int x, int y, BWAPI::UnitType building_type, bool build_or_remove)
{
	//2x1
	if (building_type.isMineralField())
	{
		if (build_or_remove)
		{
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth())].first.unobstructed = false;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = false;
		}
		else
		{
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth())].first.unobstructed = true;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = true;
		}
	}
	//2x2
	if (building_type == BWAPI::UnitTypes::Terran_Missile_Turret ||
		building_type == BWAPI::UnitTypes::Protoss_Photon_Cannon ||
		building_type == BWAPI::UnitTypes::Protoss_Pylon)
	{
		if (x + 2 > BWAPI::Broodwar->mapWidth() ||
			y + 2 > BWAPI::Broodwar->mapHeight())
			return;
		if (build_or_remove)
		{
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth())].first.unobstructed = false;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth())].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = false;
		}
		else
		{
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth())].first.unobstructed = true;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth())].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = true;
		}
	}
	//3x2
	if (building_type == BWAPI::UnitTypes::Terran_Academy ||
		building_type == BWAPI::UnitTypes::Terran_Armory ||
		building_type == BWAPI::UnitTypes::Terran_Bunker ||
		building_type == BWAPI::UnitTypes::Terran_Supply_Depot ||
		building_type == BWAPI::UnitTypes::Protoss_Arbiter_Tribunal ||
		building_type == BWAPI::UnitTypes::Protoss_Citadel_of_Adun ||
		building_type == BWAPI::UnitTypes::Protoss_Cybernetics_Core ||
		building_type == BWAPI::UnitTypes::Protoss_Fleet_Beacon ||
		building_type == BWAPI::UnitTypes::Protoss_Forge ||
		building_type == BWAPI::UnitTypes::Protoss_Observatory ||
		building_type == BWAPI::UnitTypes::Protoss_Robotics_Facility ||
		building_type == BWAPI::UnitTypes::Protoss_Robotics_Support_Bay ||
		building_type == BWAPI::UnitTypes::Protoss_Shield_Battery ||
		building_type == BWAPI::UnitTypes::Protoss_Templar_Archives)
	{
		if (x + 3 > BWAPI::Broodwar->mapWidth() ||
			y + 2 > BWAPI::Broodwar->mapHeight())
			return;
		if (build_or_remove)
		{
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth())].first.unobstructed = false;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = false;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth())].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = false;
		}
		else
		{
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth())].first.unobstructed = true;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = true;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth())].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = true;
		}
	}
	//4x2
	else if (building_type == BWAPI::UnitTypes::Resource_Vespene_Geyser)
	{
		if (x + 4 > BWAPI::Broodwar->mapWidth() ||
			y + 2 > BWAPI::Broodwar->mapHeight())
			return;
		if (build_or_remove)
		{
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth())].first.unobstructed = false;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = false;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = false;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth())].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = false;
		}
		else
		{
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth())].first.unobstructed = true;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = true;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = true;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth())].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = true;
		}
	}
	//4x3
	else if (building_type == BWAPI::UnitTypes::Terran_Barracks ||
		building_type == BWAPI::UnitTypes::Terran_Engineering_Bay ||
		building_type == BWAPI::UnitTypes::Protoss_Gateway ||
		building_type == BWAPI::UnitTypes::Protoss_Nexus ||
		building_type == BWAPI::UnitTypes::Protoss_Stargate)
	{
		if (x + 4 > BWAPI::Broodwar->mapWidth() ||
			y + 3 > BWAPI::Broodwar->mapHeight())
			return;
		if (build_or_remove)
		{
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth())].first.unobstructed = false;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = false;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = false;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth())].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = false;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth())].first.unobstructed = false;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = false;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = false;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = false;
		}
		else
		{
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth())].first.unobstructed = true;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = true;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = true;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth())].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = true;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth())].first.unobstructed = true;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = true;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = true;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = true;
		}
	}
	//4x3 with addon.
	else if (building_type == BWAPI::UnitTypes::Terran_Command_Center ||
		building_type == BWAPI::UnitTypes::Terran_Factory ||
		building_type == BWAPI::UnitTypes::Terran_Science_Facility ||
		building_type == BWAPI::UnitTypes::Terran_Starport)
	{
		if (x + 4 > BWAPI::Broodwar->mapWidth() ||
			y + 3 > BWAPI::Broodwar->mapHeight())
			return;
		if (build_or_remove)
		{
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth())].first.unobstructed = false;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = false;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = false;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth())].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 4].first.unobstructed = false;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 5].first.unobstructed = false;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth())].first.unobstructed = false;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = false;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = false;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = false;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 4].first.unobstructed = false;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 5].first.unobstructed = false;
		}
		else
		{
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth())].first.unobstructed = true;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = true;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = true;
			build_position_map[x + (y * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth())].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 4].first.unobstructed = true;
			build_position_map[x + ((y + 1) * BWAPI::Broodwar->mapWidth()) + 5].first.unobstructed = true;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth())].first.unobstructed = true;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 1].first.unobstructed = true;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 2].first.unobstructed = true;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 3].first.unobstructed = true;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 4].first.unobstructed = true;
			build_position_map[x + ((y + 2) * BWAPI::Broodwar->mapWidth()) + 5].first.unobstructed = true;
		}
	}
	
}

std::vector<std::pair<TileFlags, int>>* GameState::getBuildPositionMap()
{
	return &build_position_map;
}

void GameState::setWorkerDefense(bool new_worker_defense)
{
	worker_defense = true;
}

bool GameState::getWorkerDefense()
{
	return worker_defense;
}

AIBase* GameState::getClosestEmptyBase()
{
	auto base_list_iterator = base_list.begin();
	auto main_base = base_list_iterator;
	auto last_empty_base_found = base_list_iterator;
	bool found_main = false;
	int closest_distance = 0;
	std::vector<AIBase*> empty_base_list;
	while (base_list_iterator != base_list.end())
	{
		if (base_list_iterator->getBaseClass() == 3)
		{
			main_base = base_list_iterator;
			found_main = true;
			base_list_iterator++;
		}
		else if (base_list_iterator->getBaseClass() == 1 &&
				base_list_iterator->getArea()->Bases().size() > 0)
		{
			empty_base_list.push_back(&(*base_list_iterator));
			base_list_iterator++;
		}
		else
		{
			base_list_iterator++;
		}
	}
	if (empty_base_list.size() == 0)
	{
		return nullptr;
	}
	else if (empty_base_list.size() == 1)
	{
		return (*empty_base_list.begin());
	}
	else if (found_main == true)
	{
		auto empty_base_iterator = empty_base_list.begin();
		auto closest_base = empty_base_iterator;
		closest_distance = getGroundDistance(BWAPI::Position((*closest_base)->getArea()->Top()), BWAPI::Position(main_base->getArea()->Top()));
		int distance_to_check = 0;
		empty_base_iterator++;
		while (empty_base_iterator != empty_base_list.end())
		{
			distance_to_check = getGroundDistance(BWAPI::Position((*empty_base_iterator)->getArea()->Top()), BWAPI::Position(main_base->getArea()->Top()));
			if (distance_to_check < closest_distance &&
				distance_to_check > 0)
			{
				closest_distance = distance_to_check;
				closest_base = empty_base_iterator;
			}
			empty_base_iterator++;
		}
		return *closest_base;
	}
	else
	{
		return nullptr;
	}
}

void GameState::toggleExpanding()
{
	if (expanding)
		expanding = false;
	else
		expanding = true;
}

bool GameState::getExpanding()
{
	return expanding;
}

void GameState::setTargetExpansion(AIBase* new_target_expansion)
{
	target_expansion = new_target_expansion;
}

AIBase* GameState::getTargetExpansion()
{
	return target_expansion;
}


void GameState::toggleSecondaryScouting()
{
	if (secondary_scouting)
		secondary_scouting = false;
	else
		secondary_scouting = true;
}

bool GameState::getSecondaryScouting()
{
	return secondary_scouting;
}

AIBase* GameState::getClosestEmptyBaseNotSecondaryScouted()
{
	auto base_list_iterator = base_list.begin();
	auto main_base = base_list_iterator;
	auto last_empty_base_found = base_list_iterator;
	bool found_main = false;
	int closest_distance = 0;
	std::vector<AIBase*> empty_base_list;
	while (base_list_iterator != base_list.end())
	{
		if (base_list_iterator->getBaseClass() == 3)
		{
			main_base = base_list_iterator;
			found_main = true;
			base_list_iterator++;
		}
		else if (base_list_iterator->getBaseClass() == 1 &&
			base_list_iterator->getArea()->Bases().size() > 0 &&
			!base_list_iterator->getSecondaryScouted())
		{
			empty_base_list.push_back(&(*base_list_iterator));
			base_list_iterator++;
		}
		else
		{
			base_list_iterator++;
		}
	}
	if (empty_base_list.size() == 0)
	{
		return nullptr;
	}
	else if (empty_base_list.size() == 1)
	{
		return (*empty_base_list.begin());
	}
	else if (found_main == true)
	{
		auto empty_base_iterator = empty_base_list.begin();
		auto closest_base = empty_base_iterator;
		closest_distance = getGroundDistance(BWAPI::Position(main_base->getArea()->Top()), BWAPI::Position((*closest_base)->getArea()->Top()));
		int distance_to_check = 0;
		empty_base_iterator++;
		while (empty_base_iterator != empty_base_list.end())
		{
			distance_to_check = getGroundDistance(BWAPI::Position(main_base->getArea()->Top()), BWAPI::Position((*empty_base_iterator)->getArea()->Top()));
			if (distance_to_check < closest_distance &&
				distance_to_check > 0)
			{
				closest_distance = distance_to_check;
				closest_base = empty_base_iterator;
			}
			empty_base_iterator++;
		}
		return *closest_base;
	}
	else
	{
		return nullptr;
	}
}

void GameState::resetSecondaryScouting()
{
	auto base_list_iterator = base_list.begin();
	while (base_list_iterator != base_list.end())
	{
		if (base_list_iterator->getSecondaryScouted())
			base_list_iterator->toggleSecondaryScouted();
		base_list_iterator++;
	}
}

AIBase* GameState::getClosestEmptyStartLocationNotSecondaryScouted()
{
	auto base_list_iterator = base_list.begin();
	auto main_base = base_list_iterator;
	auto last_empty_base_found = base_list_iterator;
	bool found_main = false;
	int closest_distance = 0;
	std::vector<AIBase*> empty_base_list;
	while (base_list_iterator != base_list.end())
	{
		if (base_list_iterator->getBaseClass() == 3)
		{
			main_base = base_list_iterator;
			found_main = true;
			base_list_iterator++;
		}
		else if (base_list_iterator->getBaseClass() == 1 &&
			base_list_iterator->getArea()->Bases().size() > 0 &&
			base_list_iterator->getArea()->Bases().begin()->Starting() &&
			!base_list_iterator->getSecondaryScouted())
		{
			empty_base_list.push_back(&(*base_list_iterator));
			base_list_iterator++;
		}
		else
		{
			base_list_iterator++;
		}
	}
	if (empty_base_list.size() == 0)
	{
		return nullptr;
	}
	else if (empty_base_list.size() == 1)
	{
		return (*empty_base_list.begin());
	}
	else if (found_main == true)
	{
		auto empty_base_iterator = empty_base_list.begin();
		auto closest_base = empty_base_iterator;
		closest_distance = getGroundDistance(BWAPI::Position((*closest_base)->getArea()->Top()), BWAPI::Position(main_base->getArea()->Top()));
		int distance_to_check = 0;
		empty_base_iterator++;
		while (empty_base_iterator != empty_base_list.end())
		{
			distance_to_check = getGroundDistance(BWAPI::Position((*empty_base_iterator)->getArea()->Top()), BWAPI::Position(main_base->getArea()->Top()));
			if (distance_to_check < closest_distance &&
				distance_to_check > 0)
			{
				closest_distance = distance_to_check;
				closest_base = empty_base_iterator;
			}
			empty_base_iterator++;
		}
		return *closest_base;
	}
	else
	{
		return nullptr;
	}
}

void GameState::setLastTimeExpanded()
{
	last_time_expanded = BWAPI::Broodwar->elapsedTime();
}

int GameState::getLastTimeExpanded()
{
	return last_time_expanded;
}

int GameState::getGroundDistance(BWAPI::Position point_a, BWAPI::Position point_b)
{
	int distance = 0;
	
	auto path = BWEM::Map::Instance().GetPath(point_a, point_b);

	if (path.size() == 0 &&
		BWEM::Map::Instance().GetNearestArea((BWAPI::TilePosition)point_a) != BWEM::Map::Instance().GetNearestArea((BWAPI::TilePosition)point_b))
		return -1;

	for (auto cpp : path)
	{
		auto center = BWAPI::Position{ cpp->Center() };
		distance += point_a.getDistance(center);
		point_a = center;
	}

	return distance + point_a.getDistance(point_b);
}

void GameState::toggleCyberCore()
{
	if (cyber_core)
		cyber_core = false;
	else
		cyber_core = true;
}

bool GameState::checkCyberCore()
{
	return cyber_core;
}

double GameState::getEnemyTotalStrength()
{
	double army_strength = 0;
	for (auto current_enemy_unit : enemy_units)
	{
		if (current_enemy_unit.second.getUnit()->getType().groundWeapon() != BWAPI::WeaponTypes::None &&
			current_enemy_unit.second.getUnit()->getType().groundWeapon() != BWAPI::WeaponTypes::Unknown &&
			current_enemy_unit.second.getUnit()->getType() != BWAPI::UnitTypes::Terran_SCV &&
			current_enemy_unit.second.getUnit()->getType() != BWAPI::UnitTypes::Protoss_Probe &&
			current_enemy_unit.second.getUnit()->getType() != BWAPI::UnitTypes::Zerg_Drone)
		{
			if (current_enemy_unit.second.getUnit()->getType() == BWAPI::UnitTypes::Protoss_Reaver)
			{
				army_strength += (((double)(BWAPI::UnitTypes::Protoss_Scarab.groundWeapon().damageAmount() * BWAPI::UnitTypes::Protoss_Scarab.maxGroundHits() * BWAPI::UnitTypes::Protoss_Scarab.groundWeapon().damageFactor()) / (double)60) * pow(current_enemy_unit.second.getUnit()->getType().groundWeapon().maxRange(), 1 / 3)) / 4;
			}
			else if (current_enemy_unit.second.getUnit()->getType() == BWAPI::UnitTypes::Protoss_Scarab)
			{

			}
			else
				army_strength += ((double)(current_enemy_unit.second.getUnit()->getType().groundWeapon().damageAmount() * current_enemy_unit.second.getUnit()->getType().maxGroundHits() * current_enemy_unit.second.getUnit()->getType().groundWeapon().damageFactor()) / (double)current_enemy_unit.second.getUnit()->getType().groundWeapon().damageCooldown()) * pow(current_enemy_unit.second.getUnit()->getType().groundWeapon().maxRange(), 1 / 3);
		}
	}
	return army_strength;
}

double GameState::getObjectiveStrength(Objective my_objective)
{
	double army_strength = 0;
	for (auto current_unit : *my_objective.getUnits())
	{
		std::string my_type = current_unit.getUnit()->getType().toString();
		double first = current_unit.getUnit()->getType().groundWeapon().damageAmount();
		double second = current_unit.getUnit()->getType().maxGroundHits();
		double third = current_unit.getUnit()->getType().groundWeapon().damageFactor();
		double fourth = current_unit.getUnit()->getType().groundWeapon().damageCooldown();
		double fifth = pow(current_unit.getUnit()->getType().groundWeapon().maxRange(), 1 / 3);
		army_strength += ((double)(current_unit.getUnit()->getType().groundWeapon().damageAmount() * current_unit.getUnit()->getType().maxGroundHits() * current_unit.getUnit()->getType().groundWeapon().damageFactor()) / (double)current_unit.getUnit()->getType().groundWeapon().damageCooldown()) * pow(current_unit.getUnit()->getType().groundWeapon().maxRange(), 1 / 3);
	}
	return army_strength;
}

AIBase* GameState::getMainBase()
{
	auto base_list_iterator = base_list.begin();
	auto main_base = base_list_iterator;
	while (base_list_iterator != base_list.end())
	{
		if (base_list_iterator->getBaseClass() == 3 &&
			base_list_iterator->getArea()->Bases().size() > 0)
		{
			return &(*base_list_iterator);
		}
		base_list_iterator++;
	}
	return nullptr;
}

double GameState::getLocalStrength(Object my_unit)
{
	double my_strength = 0;
	double enemy_strength = 0;
	if (BWAPI::Broodwar->getUnitsInRadius(my_unit.getUnit()->getPosition(), my_unit.getUnit()->getType().sightRange(), BWAPI::Filter::IsEnemy).size() > 0)
	{
		1;
	}
	for (auto current_unit : BWAPI::Broodwar->getUnitsInRadius(my_unit.getUnit()->getPosition(), my_unit.getUnit()->getType().sightRange()))
	{
		if (current_unit->getPlayer() == BWAPI::Broodwar->self() ||
			current_unit->getPlayer()->isAlly(BWAPI::Broodwar->self()))
		{
			if (current_unit->getType().groundWeapon() != BWAPI::WeaponTypes::None &&
				current_unit->getType().groundWeapon() != BWAPI::WeaponTypes::Unknown &&
				current_unit->getType() != BWAPI::UnitTypes::Terran_SCV &&
				current_unit->getType() != BWAPI::UnitTypes::Protoss_Probe &&
				current_unit->getType() != BWAPI::UnitTypes::Zerg_Drone)
			{
				if (current_unit->getType() == BWAPI::UnitTypes::Protoss_Reaver)
				{
					my_strength += (((double)(BWAPI::UnitTypes::Protoss_Scarab.groundWeapon().damageAmount() * BWAPI::UnitTypes::Protoss_Scarab.maxGroundHits() * BWAPI::UnitTypes::Protoss_Scarab.groundWeapon().damageFactor()) / (double)60) * pow(current_unit->getType().groundWeapon().maxRange(), 1 / 3)) / 4;
				}
				else if (current_unit->getType() == BWAPI::UnitTypes::Protoss_Scarab)
				{

				}
				else
					my_strength += ((double)(current_unit->getType().groundWeapon().damageAmount() * current_unit->getType().maxGroundHits() * current_unit->getType().groundWeapon().damageFactor()) / (double)current_unit->getType().groundWeapon().damageCooldown()) * pow(current_unit->getType().groundWeapon().maxRange(), 1 / 3);
			}
		}
		else
		{
			if (current_unit->getType().groundWeapon() != BWAPI::WeaponTypes::None &&
				current_unit->getType().groundWeapon() != BWAPI::WeaponTypes::Unknown &&
				current_unit->getType() != BWAPI::UnitTypes::Terran_SCV &&
				current_unit->getType() != BWAPI::UnitTypes::Protoss_Probe &&
				current_unit->getType() != BWAPI::UnitTypes::Zerg_Drone)
			{
				if (current_unit->getType() == BWAPI::UnitTypes::Protoss_Reaver)
				{
					enemy_strength += (((double)(BWAPI::UnitTypes::Protoss_Scarab.groundWeapon().damageAmount() * BWAPI::UnitTypes::Protoss_Scarab.maxGroundHits() * BWAPI::UnitTypes::Protoss_Scarab.groundWeapon().damageFactor()) / (double)60) * pow(current_unit->getType().groundWeapon().maxRange(), 1 / 3)) / 4;
				}
				else if (current_unit->getType() == BWAPI::UnitTypes::Protoss_Scarab)
				{

				}
				else
					enemy_strength += ((double)(current_unit->getType().groundWeapon().damageAmount() * current_unit->getType().maxGroundHits() * current_unit->getType().groundWeapon().damageFactor()) / (double)current_unit->getType().groundWeapon().damageCooldown()) * pow(current_unit->getType().groundWeapon().maxRange(), 1 / 3);
			}
		}
	}
	return my_strength - enemy_strength;
}

double GameState::getMyTotalStrength()
{
	double army_strength = 0;
	auto current_objective = objective_list.begin();
	while (current_objective != objective_list.end())
	{
		auto unit_iterator = current_objective->getUnits()->begin();
		while (unit_iterator != current_objective->getUnits()->end())
		{
			if (unit_iterator->getUnit()->getType().groundWeapon() != BWAPI::WeaponTypes::None &&
				unit_iterator->getUnit()->getType().groundWeapon() != BWAPI::WeaponTypes::Unknown &&
				unit_iterator->getUnit()->getType() != BWAPI::UnitTypes::Terran_SCV &&
				unit_iterator->getUnit()->getType() != BWAPI::UnitTypes::Protoss_Probe &&
				unit_iterator->getUnit()->getType() != BWAPI::UnitTypes::Zerg_Drone)
			{
				if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Reaver)
				{
					army_strength += (((double)(BWAPI::UnitTypes::Protoss_Scarab.groundWeapon().damageAmount() * BWAPI::UnitTypes::Protoss_Scarab.maxGroundHits() * BWAPI::UnitTypes::Protoss_Scarab.groundWeapon().damageFactor()) / (double)60) * pow(unit_iterator->getUnit()->getType().groundWeapon().maxRange(), 1 / 3)) / 4;
				}
				else if (unit_iterator->getUnit()->getType() == BWAPI::UnitTypes::Protoss_Scarab)
				{

				}
				else
					army_strength += ((double)(unit_iterator->getUnit()->getType().groundWeapon().damageAmount() * unit_iterator->getUnit()->getType().maxGroundHits() * unit_iterator->getUnit()->getType().groundWeapon().damageFactor()) / (double)unit_iterator->getUnit()->getType().groundWeapon().damageCooldown()) * pow(unit_iterator->getUnit()->getType().groundWeapon().maxRange(), 1 / 3);
			}
			unit_iterator++;
		}
		current_objective++;
	}
	return army_strength;
}

AIBase* GameState::getMyClosestBase(Object my_unit)
{
	auto base_list_iterator = base_list.begin();
	int farthest_distance = 0;
	int distance_to_test = 0;
	AIBase* return_base = nullptr;
	while (base_list_iterator != base_list.end())
	{
		if (base_list_iterator->getBaseClass() == 3 ||
			base_list_iterator->getBaseClass() == 4)
		{
			if (farthest_distance == 0)
			{
				farthest_distance = getGroundDistance(my_unit.getUnit()->getPosition(), (BWAPI::Position)base_list_iterator->getArea()->Top());
				return_base = &(*base_list_iterator);
			}
			else
			{
				distance_to_test = getGroundDistance(my_unit.getUnit()->getPosition(), (BWAPI::Position)base_list_iterator->getArea()->Top());
				if (distance_to_test < farthest_distance)
				{
					farthest_distance = distance_to_test;
					return_base = &(*base_list_iterator);
				}
			}
		}
		base_list_iterator++;
	}
	return return_base;
}

void GameState::addRoboticsFacility(int additional_robotics_facility)
{
	robotics_facility += additional_robotics_facility;
}

void GameState::addObservatory(int additional_observatory)
{
	observatory += additional_observatory;
}

int GameState::getRoboticsFacility()
{
	return robotics_facility;
}

int GameState::getObservatory()
{
	return observatory;
}

void GameState::addForge(int additional_forge)
{
	forge += additional_forge;
}

int GameState::getForge()
{
	return forge;
}

void GameState::addCitadelofAdun(int additional_citadel_of_adun)
{
	citadel_of_adun += additional_citadel_of_adun;
}

int GameState::getCitadelofAdun()
{
	return citadel_of_adun;
}

void GameState::addSupplyBuilt(int new_supply)
{
	supply_built += new_supply;
}

int GameState::getSupplyBuilt()
{
	return supply_built;
}

void GameState::addPylon(int additional_pylon)
{
	pylon += additional_pylon;
}

int GameState::getPylon()
{
	return pylon;
}

AIBase* GameState::getFarthestEmptyBaseNotSecondaryScouted()
{
	auto base_list_iterator = base_list.begin();
	auto main_base = base_list_iterator;
	auto last_empty_base_found = base_list_iterator;
	bool found_main = false;
	int farthest_distance = 0;
	std::vector<AIBase*> empty_base_list;
	while (base_list_iterator != base_list.end())
	{
		if (base_list_iterator->getBaseClass() == 3)
		{
			main_base = base_list_iterator;
			found_main = true;
			base_list_iterator++;
		}
		else if (base_list_iterator->getBaseClass() == 1 &&
			base_list_iterator->getArea()->Bases().size() > 0 &&
			!base_list_iterator->getSecondaryScouted())
		{
			empty_base_list.push_back(&(*base_list_iterator));
			base_list_iterator++;
		}
		else
		{
			base_list_iterator++;
		}
	}
	if (empty_base_list.size() == 0)
	{
		return nullptr;
	}
	else if (empty_base_list.size() == 1)
	{
		return (*empty_base_list.begin());
	}
	else if (found_main == true)
	{
		auto empty_base_iterator = empty_base_list.begin();
		auto farthest_base = empty_base_iterator;
		farthest_distance = getGroundDistance(BWAPI::Position(main_base->getArea()->Top()), BWAPI::Position((*farthest_base)->getArea()->Top()));
		int distance_to_check = 0;
		empty_base_iterator++;
		while (empty_base_iterator != empty_base_list.end())
		{
			distance_to_check = getGroundDistance(BWAPI::Position(main_base->getArea()->Top()), BWAPI::Position((*empty_base_iterator)->getArea()->Top()));
			if (distance_to_check > farthest_distance)
			{
				farthest_distance = distance_to_check;
				farthest_base = empty_base_iterator;
			}
			empty_base_iterator++;
		}
		return *farthest_base;
	}
	else
	{
		return nullptr;
	}
}

void GameState::addFactory(int additional_factory)
{
	factory += additional_factory;
}

int GameState::getFactory()
{
	return factory;
}

int GameState::getCompletedMacroBuildings()
{
	int count = 0;
	for (auto building_iterator : building_list)
	{
		if (building_iterator.getUnit()->getType().canProduce() &&
			!building_iterator.getUnit()->isBeingConstructed())
			count++;
	}
	return count;
}

int GameState::getUnderConstructionMacroBuildings()
{
	int count = 0;
	for (auto building_iterator : building_list)
	{
		if (building_iterator.getUnit()->getType().canProduce() &&
			building_iterator.getUnit()->isBeingConstructed())
			count++;
	}
	return count;
}

void GameState::addBunker(int additional_bunker)
{
	bunker += additional_bunker;
}

int GameState::getBunker()
{
	return bunker;
}

void GameState::loadBunker(Object* bunker_to_load)
{
	int number_loaded = bunker_to_load->getUnit()->getLoadedUnits().size();
	if (number_loaded < 4)
	{
		for (auto unit_iterator : *objective_list.begin()->getUnits())
		{
			if (unit_iterator.getUnit()->exists())
			{
				if (unit_iterator.getUnit()->getType() == BWAPI::UnitTypes::Terran_Marine &&
					!unit_iterator.getUnit()->isLoaded())
				{
					unit_iterator.getUnit()->rightClick(bunker_to_load->getUnit());
					number_loaded++;
				}
			}
			if (number_loaded == 4)
				break;
		}
	}
}

double GameState::getEnemyLocalStrength(Object my_unit)
{
	double enemy_strength = 0;
	for (auto current_unit : BWAPI::Broodwar->getUnitsInRadius(my_unit.getUnit()->getPosition(), my_unit.getUnit()->getType().sightRange(), BWAPI::Filter::IsEnemy))
	{
		if (current_unit->getType().groundWeapon() != BWAPI::WeaponTypes::None &&
			current_unit->getType().groundWeapon() != BWAPI::WeaponTypes::Unknown &&
			current_unit->getType() != BWAPI::UnitTypes::Terran_SCV &&
			current_unit->getType() != BWAPI::UnitTypes::Protoss_Probe &&
			current_unit->getType() != BWAPI::UnitTypes::Zerg_Drone)
		{
			if (current_unit->getType() == BWAPI::UnitTypes::Protoss_Reaver)
			{
				enemy_strength += (((double)(BWAPI::UnitTypes::Protoss_Scarab.groundWeapon().damageAmount() * BWAPI::UnitTypes::Protoss_Scarab.maxGroundHits() * BWAPI::UnitTypes::Protoss_Scarab.groundWeapon().damageFactor()) / (double)60) * pow(current_unit->getType().groundWeapon().maxRange(), 1 / 3)) / 4;
			}
			else if (current_unit->getType() == BWAPI::UnitTypes::Protoss_Scarab)
			{

			}
			else
				enemy_strength += ((double)(current_unit->getType().groundWeapon().damageAmount() * current_unit->getType().maxGroundHits() * current_unit->getType().groundWeapon().damageFactor()) / (double)current_unit->getType().groundWeapon().damageCooldown()) * pow(current_unit->getType().groundWeapon().maxRange(), 1 / 3);
		}
	}
	return enemy_strength;
}

std::vector<Object>* GameState::getRepairWorkers()
{
	return &repair_workers;
}

int GameState::getAssignedRepairWorkers(Object repair_target)
{
	int assigned_workers = 0;
	auto repair_worker_iterator = repair_workers.begin();
	while (repair_worker_iterator != repair_workers.end())
	{
		if (repair_worker_iterator->getRepairTarget() == repair_target.getUnit())
			assigned_workers++;
		repair_worker_iterator++;
	}
	return assigned_workers;
}

void GameState::assignRepairWorkers(Object* repair_target, int number_of_workers)
{
	AIBase* target_base = getNearestContainingBase(repair_target->getUnit());
	int workers_in_base = 0;
	auto mineral_worker_iterator = mineral_workers.begin();
	while (mineral_worker_iterator != mineral_workers.end())
	{
		if (mineral_worker_iterator->getBase() == target_base)
			workers_in_base++;
		mineral_worker_iterator++;
	}
	int assigned_workers = 0;
	mineral_worker_iterator = mineral_workers.begin();
	while (assigned_workers != number_of_workers &&
		workers_in_base - assigned_workers > 4 &&
		mineral_worker_iterator != mineral_workers.end())
	{
		if (mineral_worker_iterator->getBase() == target_base)
		{
			Object new_repair_worker(*mineral_worker_iterator);
			new_repair_worker.setRepairTarget(repair_target->getUnit());
			repair_workers.push_back(new_repair_worker);
			auto erase_iterator = mineral_worker_iterator;
			mineral_worker_iterator = mineral_workers.erase(erase_iterator);
		}
		else
			mineral_worker_iterator++;
	}
}

AIBase* GameState::getNearestContainingBase(BWAPI::Unit unit)
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

void GameState::removeRepairWorkers(Object* repair_target, int number_of_workers)
{
	int workers_removed = 0;
	auto repair_worker_iterator = repair_workers.begin();
	while (workers_removed != number_of_workers &&
		repair_worker_iterator != repair_workers.end())
	{
		if (repair_worker_iterator->getRepairTarget() == repair_target->getUnit())
		{
			Object new_mineral_worker(*repair_worker_iterator);
			mineral_workers.push_back(new_mineral_worker);
			auto erase_iterator = repair_worker_iterator;
			repair_worker_iterator = repair_workers.erase(erase_iterator);
			workers_removed++;
		}
		else
			repair_worker_iterator++;
	}
}