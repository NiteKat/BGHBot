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
	engineering_bay = 0;
	starport = 0;
	science_facility = 0;
	armory = 0;
	mt = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
	scouted = false;
	assign_scout = false;
	photon_cannon = 0;
	forge = 0;
	citadel_of_adun = 0;
	spawning_pool = 0;
	if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
		hatchery = 1;
	else
		hatchery = 0;
	times_retreated = 0;
	pylon_building = false;
	templar_archives = 0;
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
	if (unit == nullptr ||
		unit->getTilePosition() == BWAPI::TilePositions::Invalid ||
		unit->getTilePosition() == BWAPI::TilePositions::None ||
		unit->getTilePosition() == BWAPI::TilePositions::Unknown)
		return nullptr;
	if (BWEM::Map::Instance().GetArea(unit->getTilePosition()) != nullptr)
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
	else
		return nullptr;
}

AIBase* GameState::getContainingBase(BWAPI::TilePosition tile_position)
{
	if (tile_position == BWAPI::TilePositions::Invalid ||
		tile_position == BWAPI::TilePositions::None ||
		tile_position == BWAPI::TilePositions::Unknown)
		return nullptr;
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
	pylon_building = false;
	for (auto &building_iterator : building_list)
	{
		if (building_iterator.getType() == BWAPI::UnitTypes::Protoss_Pylon &&
			building_iterator.getUnit()->isBeingConstructed())
			pylon_building = true;
	}
	if (times_retreated == 2 &&
		build_order == BuildOrder::P2Gate1)
	{
		build_order = BuildOrder::Default;
	}
	checkBaseOwnership();
	if (BWAPI::Broodwar->getFrameCount() == 0)
	{
		if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
		{
			BWAPI::TilePosition search_center = BWAPI::Broodwar->self()->getStartLocation();
			if (BWAPI::Broodwar->enemies().size() == 1 &&
				BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Zerg)
			{
				
				BWAPI::TilePosition direction = search_center - (BWAPI::TilePosition)(*BWEM::Map::Instance().GetArea(search_center)->ChokePoints().begin())->Center();
				if (abs(direction.x) > abs(direction.y))
				{
					if (direction.x < 0)
					{
						search_center += BWAPI::TilePosition(6, 0);
					}
					else
						search_center += BWAPI::TilePosition(-8, 0);
				}
				else
				{
					if (direction.y < 0)
					{
						search_center += BWAPI::TilePosition(0, -6);
					}
					else
						search_center -= BWAPI::TilePosition(0, 4);
				}
				placeBlock(BWEM::Map::Instance().GetArea(BWAPI::Broodwar->self()->getStartLocation()), std::make_pair<int, int>(8, 6), BlockType::TTvZBunkerStart, search_center);
			}
		}
	}
	if ((BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran ||
		BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss ) &&
		!scouted &&
		!assign_scout &&
		supply_used == 10)
		assign_scout = true;
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
				if (current_objective->getObjective() == ObjectiveTypes::AttackWithRegroupPrioritizeWorkers)
					objective_exists = true;
				current_objective++;
			}
			if (!objective_exists)
			{
				Objective new_objective;
				new_objective.setObjective(ObjectiveTypes::AttackWithRegroupPrioritizeWorkers);
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
			else if (build_order == BuildOrder::FivePool &&
				objective_list.begin()->getUnits()->size() > 0)
			{
				if (objective_list.size() == 1)
				{
					Objective new_objective;
					new_objective.setObjective(ObjectiveTypes::AttackWithRegroupPrioritizeWorkers);
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

	initDefenseGrid();
	for (auto &current_base : base_list)
	{
		//I own this base, so evaluate positions.
		if (current_base.getBaseClass() == 3 ||
			current_base.getBaseClass() == 4)
		{
			for (auto &current_chokepoint : current_base.getArea()->ChokePoints())
			{
				if (!current_chokepoint->Blocked())
				{
					auto areas = current_chokepoint->GetAreas();
					if (areas.first != nullptr &&
						areas.second != nullptr)
					{
						AIBase* base_to_review_first = getBaseforArea(areas.first);
						AIBase* base_to_review_second = getBaseforArea(areas.second);
						int class_first = base_to_review_first->getBaseClass();
						int class_second = base_to_review_second->getBaseClass();
						if ((base_to_review_first->getBaseClass() != 3 &&
							base_to_review_first->getBaseClass() != 4 &&
							base_to_review_first->getBaseClass() != 5) ||
							(base_to_review_second->getBaseClass() != 3 &&
							base_to_review_second->getBaseClass() != 4 &&
							base_to_review_second->getBaseClass() != 5))
						{
							BWAPI::TilePosition position_to_add_score = (BWAPI::TilePosition)current_chokepoint->Center();
							defense_grid[position_to_add_score.y * BWAPI::Broodwar->mapWidth() + position_to_add_score.x].first += 9;
						}
					}
				}
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
	if (point_a == BWAPI::Positions::Invalid ||
		point_a == BWAPI::Positions::Unknown ||
		point_a == BWAPI::Positions::None ||
		point_b == BWAPI::Positions::Invalid ||
		point_b == BWAPI::Positions::Unknown ||
		point_b == BWAPI::Positions::None)
		return -1;
	auto path = BWEM::Map::Instance().GetPath(point_a, point_b);

	if (path.size() == 0 &&
		BWEM::Map::Instance().GetNearestArea((BWAPI::WalkPosition)point_a) != BWEM::Map::Instance().GetNearestArea((BWAPI::WalkPosition)point_b))
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
			if (new_repair_worker.getResourceTarget() != nullptr)
				unassignWorkerFromMineral(&new_repair_worker);
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

AIBase* GameState::getNearestContainingBase(BWAPI::TilePosition tile_position)
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
			assignWorkerToMineral(&new_mineral_worker);
			mineral_workers.push_back(new_mineral_worker);
			auto erase_iterator = repair_worker_iterator;
			repair_worker_iterator = repair_workers.erase(erase_iterator);
			workers_removed++;
		}
		else
			repair_worker_iterator++;
	}
}

void GameState::removeMineral(BWAPI::Unit mineral)
{
	auto base_list_iterator = base_list.begin();
	while (base_list_iterator != base_list.end())
	{
		if (base_list_iterator->removeMineral(mineral))
			return;
		else
			base_list_iterator++;
	}
}

bool GameState::assignWorkerToMineral(Object* worker)
{
	AIBase* containing_base = getNearestContainingBase(worker->getUnit());
	const std::vector<Resource> *minerals = containing_base->getMinerals();
	Resource* lowest_assigned = nullptr;
	if ((containing_base->getBaseClass() == 3 ||
		containing_base->getBaseClass() == 4) &&
		minerals->size() > 0)
	{
		for (auto &current_mineral : *containing_base->getMinerals())
		{
			if (lowest_assigned == nullptr)
				lowest_assigned = &current_mineral;
			else
			{
				int lowest_workers = lowest_assigned->getNumberAssigned();
				int possible_lowest = current_mineral.getNumberAssigned();
				if (possible_lowest < lowest_workers)
					lowest_assigned = &current_mineral;
			}
		}
			worker->setResourceTarget(lowest_assigned->getUnit());
			lowest_assigned->addWorker(worker->getUnit());
			return true;
	}
	else
	{
		for (auto &base : base_list)
		{
			minerals = base.getMinerals();
			if ((base.getBaseClass() == 3 ||
				base.getBaseClass() == 4 ) &&
				minerals->size() > 0)
			{
				for (auto &current_mineral : *base.getMinerals())
				{
					if (lowest_assigned == nullptr)
						lowest_assigned = &current_mineral;
					else
					{
						int lowest_workers = lowest_assigned->getNumberAssigned();
						int possible_lowest = current_mineral.getNumberAssigned();
						if (possible_lowest < lowest_workers)
							lowest_assigned = &current_mineral;
					}
				}
				worker->setResourceTarget(lowest_assigned->getUnit());
				lowest_assigned->addWorker(worker->getUnit());
				return true;
			}
		}
	}
	return false;
}

bool GameState::unassignWorkerFromMineral(Object* worker)
{
	AIBase* mineral_base = getContainingBase(worker->getResourceTarget());
	BWAPI::Unit worker_target = worker->getResourceTarget();
	if (mineral_base != nullptr &&
		worker_target != nullptr)
	{
		for (auto &current_mineral : *mineral_base->getMinerals())
		{
			if (current_mineral.getUnit() == worker_target)
			{
				current_mineral.removeWorker(worker->getUnit());
				worker->setResourceTarget(nullptr);
				return true;
			}
		}
		return false;
	}
	else
		return false;
}

void GameState::drawMineralLockLines()
{
	for (auto base : base_list)
	{
		for (auto mineral : *base.getMinerals())
		{
			for (auto worker : mineral.getAssignedWorkers())
			{
				BWAPI::Broodwar->drawLineMap(worker->getPosition(), mineral.getUnit()->getPosition(), BWAPI::Colors::Yellow);
			}
		}
	}
}

void GameState::transferWorkersToNewBase(AIBase* new_base)
{
	int mineral_count = new_base->getMinerals()->size();
	int max_workers = mineral_count * 2;
	AIBase* main_base = nullptr;
	std::vector<Object*> workers_to_reassign;
	for (auto &base : base_list)
	{
		if (base.getBaseClass() == 3 &&
			base.getArea()->Minerals().size() > 0)
		{
			main_base = &base;
			break;
		}
	}
	int total_workers = 0;
	for (auto &worker : mineral_workers)
	{
		AIBase* containing_base = getContainingBase(worker.getUnit());
		if (containing_base != nullptr)
		{
			if (containing_base == main_base)
			{
				total_workers++;
				workers_to_reassign.push_back(&worker);
			}
		}
	}
	int number_reassign = std::floor((double)total_workers / (double)2);
	for (int current_worker = 1; current_worker <= number_reassign; current_worker++)
	{
		assignWorkerToMineralAtBase(workers_to_reassign[current_worker - 1], new_base);
	}
}

bool GameState::assignWorkerToMineralAtBase(Object* worker, AIBase* target_base)
{
	const std::vector<Resource> *minerals = target_base->getMinerals();
	Resource* lowest_assigned = nullptr;
	if (minerals->size() > 0)
	{
		for (auto &current_mineral : *target_base->getMinerals())
		{
			if (lowest_assigned == nullptr)
				lowest_assigned = &current_mineral;
			else
			{
				int lowest_workers = lowest_assigned->getNumberAssigned();
				int possible_lowest = current_mineral.getNumberAssigned();
				if (possible_lowest < lowest_workers)
					lowest_assigned = &current_mineral;
			}
		}
		unassignWorkerFromMineral(worker);
		worker->setBase(target_base);
		worker->setResourceTarget(lowest_assigned->getUnit());
		lowest_assigned->addWorker(worker->getUnit());
		return true;
	}
	else
		return false;
}

void GameState::addEngineeringBay(int new_engineering_bay)
{
	engineering_bay += new_engineering_bay;
}

int GameState::getEngineeringBay()
{
	return engineering_bay;
}

void GameState::addStarport(int additional_starport)
{
	starport += additional_starport;
}
void GameState::addScienceFacility(int additional_science_facility)
{
	science_facility += additional_science_facility;
}

int GameState::getStarport()
{
	return starport;
}

int GameState::getScienceFacility()
{
	return science_facility;
}

void GameState::addArmory(int additional_armory)
{
	armory = additional_armory;
}

int GameState::getArmory()
{
	return armory;
}

int GameState::getRandomInteger(int min, int max)
{
	std::uniform_int_distribution<int> random_number(min, max);
	return random_number(mt);
}

void GameState::addUsedPosition(BWAPI::TilePosition new_position)
{
	used_positions.insert(new_position);
}

std::set<BWAPI::TilePosition>* GameState::getUsedPositions()
{
	return &used_positions;
}

void GameState::setScouted(bool new_value)
{
	scouted = new_value;
}

bool GameState::getScouted()
{
	return scouted;
}

void GameState::setAssignScout(bool new_value)
{
	assign_scout = new_value;
}

bool GameState::getAssignScout()
{
	return assign_scout;
}

void GameState::setAssignScout(bool new_value, Object new_intended_scout)
{
	assign_scout = new_value;
	intended_scout = new_intended_scout;
}

Object GameState::getIntendedScout()
{
	return intended_scout;
}

void GameState::setIntendedScout(Object new_intended_scout)
{
	intended_scout = new_intended_scout;
}

void GameState::addPhotonCannon(int new_photon_cannon)
{
	photon_cannon += new_photon_cannon;
}

int GameState::getPhotonCannon()
{
	return photon_cannon;
}

void GameState::addSpawningPool(int new_spawning_pool)
{
	spawning_pool += new_spawning_pool;
}

int GameState::getSpawningPool()
{
	return spawning_pool;
}

void GameState::updateUnits()
{
	for (auto &objective : objective_list)
	{
		for (auto &unit : *objective.getUnits())
		{
			unit.updateObject();
		}
	}
	for (auto &unit : enemy_units)
	{
		unit.second.updateObject();
	}
}

AIBase* GameState::getSafeEmptyBaseClosestToEnemy()
{
	std::vector<AIBase*> bases_with_enemy_units;
	for (auto &unit : enemy_units)
	{
		AIBase* containing_base = getContainingBase((BWAPI::TilePosition)unit.second.getCurrentPosition());
		if (containing_base == nullptr)
			continue;
		if (std::find(bases_with_enemy_units.begin(), bases_with_enemy_units.end(), containing_base) == bases_with_enemy_units.end())
		{
			bases_with_enemy_units.push_back(containing_base);
		}
	}
	if (bases_with_enemy_units.size() == 0)
		return nullptr;
	auto base_iterator = bases_with_enemy_units.begin();
	AIBase* main_base = getMainBase();
	AIBase* closest_base = *base_iterator;
	int closest_ground_distance = getGroundDistance((BWAPI::Position)closest_base->getArea()->Top(), (BWAPI::Position)main_base->getArea()->Top());
	base_iterator++;
	while (base_iterator != bases_with_enemy_units.end())
	{
		int distance_to_check = getGroundDistance((BWAPI::Position)(*base_iterator)->getArea()->Top(), (BWAPI::Position)main_base->getArea()->Top());
		if (distance_to_check < closest_ground_distance)
		{
			closest_base = *base_iterator;
			closest_ground_distance = distance_to_check;
		}
		base_iterator++;
	}
	
	BWEM::CPPath path_from_main = BWEM::Map::Instance().GetPath((BWAPI::Position)main_base->getArea()->Top(), (BWAPI::Position)closest_base->getArea()->Top());
	if (path_from_main.size() == 0)
		return nullptr;
	std::pair<const BWEM::Area*, const BWEM::Area*> possible_areas = (*std::prev(path_from_main.end()))->GetAreas();
	if (possible_areas.first == closest_base->getArea())
	{
		for (auto &base : base_list)
		{
			if (base.getArea() == possible_areas.second)
				return &base;
		}
		return nullptr;
	}
	else
	{
		for (auto &base : base_list)
		{
			if (base.getArea() == possible_areas.first)
				return &base;
		}
		return nullptr;
	}
}

void GameState::addHatchery(int new_hatchery)
{
	hatchery += new_hatchery;
}

int GameState::getHatchery()
{
	return hatchery;
}

void GameState::readConfigFile()
{
	std::ifstream config("bwapi-data/read/config.txt");
	if (!config)
	{
		//do stuff
	}
	else
	{
		//do stuff
	}
	config.close();
}

void GameState::initDefenseGrid()
{
	int map_cell_count = BWAPI::Broodwar->mapHeight() * BWAPI::Broodwar->mapWidth();
	std::pair<int, int> data = std::make_pair(0, 0);
	bool empty = true;
	if (defense_grid.size() > 0)
		empty = false;
	for (int index = 0; index < map_cell_count; index++)
	{
		if (empty)
			defense_grid.push_back(data);
		else
		{
			defense_grid[index] = data;
		}
	}
}

AIBase* GameState::getBaseforArea(const BWEM::Area* area)
{
	for (auto &base : base_list)
	{
		if (base.getArea() == area)
			return &base;
	}
	return nullptr;
}

void GameState::printDefenseGrid()
{
	for (int x = 0; x < BWAPI::Broodwar->mapWidth(); x++)
	{
		for (int y = 0; y < BWAPI::Broodwar->mapHeight(); y++)
		{
			BWAPI::Broodwar->drawBoxMap(x * 32, y * 32, x * 32 + 32, y * 32 + 32, BWAPI::Colors::Blue);
			BWAPI::Broodwar->drawTextMap(x * 32 + 16, y * 32 + 16, "%d", defense_grid[y * BWAPI::Broodwar->mapWidth() + x].first);
		}
	}
}

int GameState::getDefenseGroundScore(BWAPI::TilePosition position_score)
{
	if (position_score.isValid())
		return defense_grid[position_score.y * BWAPI::Broodwar->mapWidth() + position_score.x].first;
	else
		return -9000;
}

BWAPI::TilePosition GameState::getBuildLocation(BWAPI::UnitType build_type, Object build_worker)
{
	const BWEM::Area* worker_area = BWEM::Map::Instance().GetArea((BWAPI::TilePosition)build_worker.getCurrentPosition());
	
	BWAPI::TilePosition return_position = BWAPI::TilePositions::Invalid;
	//Is the worker in a valid area?
	if (!worker_area)
	{
		//Worker is not in a valid area, return invalid position.
		return return_position;
	}
	
	//Get base for the area.
	AIBase* worker_base = getBaseforArea(worker_area);
	if (worker_base->getCanSearch() &&
		worker_base->getTimesSearched() > 3)
	{
		worker_base->setCanSearch(false);
	}

	//6x3 build location
	if (build_type.canBuildAddon())
	{
		if (six_by_three_positions.size() == 0)
		{
			//Check if this area is one we want to try to place blocks in.
			if (!worker_base->getCanSearch())
			{
				//Cannot search this area.
				return return_position;
			}
			//No positions stored, attempt to place a block.
			if (tryPlacingBlocks(worker_area, build_type))
			{
				//We were able to place a block, so there will be a position to return.
				return_position = getPositionFromVector(worker_area, &six_by_three_positions);
				return return_position;
			}
			else
			{
				worker_base->addTimesSearched(1);
				return return_position;
			}
		}
		else
		{
			//Attempt to fetch a position from the vector.
			return_position = getPositionFromVector(worker_area, &six_by_three_positions);
			if (return_position != BWAPI::TilePositions::Invalid)
			{
				//found a position, return it.
				return return_position;
			}
			else
			{
				//Check if this area is one we want to try to place blocks in.
				if (!worker_base->getCanSearch())
				{
					//Cannot search this area.
					return return_position;
				}
				//No position found, try placing a block and returning a position from the placed block.
				if (tryPlacingBlocks(worker_area, build_type))
				{
					//We were able to place a block, so there will be a position to return.
					return_position = getPositionFromVector(worker_area, &six_by_three_positions);
					return return_position;
				}
				else
				{
					worker_base->addTimesSearched(1);
					return return_position;
				}
			}
		}
	}
	//Defensive Position
	else if (build_type == BWAPI::UnitTypes::Terran_Bunker)
	{
		if (three_by_two_defense_positions.size() == 0)
		{
			//Check if this area is one we want to try to place blocks in.
			if (!worker_base->getCanSearch())
			{
				//Cannot search this area.
				return return_position;
			}
			//No positions stored, attempt to place a block.
			if (tryPlacingBlocks(worker_area, build_type))
			{
				//We were able to place a block, so there will be a position to return.
				return_position = getPositionFromVector(worker_area, &three_by_two_defense_positions);
				return return_position;
			}
			else
			{
				worker_base->addTimesSearched(1);
				return return_position;
			}
		}
		else
		{
			return_position = getPositionFromVector(worker_area, &three_by_two_defense_positions);
			if (return_position != BWAPI::TilePositions::Invalid)
			{
				//found a position, return it.
				return return_position;
			}
			else
			{
				//Check if this area is one we want to try to place blocks in.
				if (!worker_base->getCanSearch())
				{
					//Cannot search this area.
					return return_position;
				}
				//No position found, try placing a block and returning a position from the placed block.
				if (tryPlacingBlocks(worker_area, build_type))
				{
					//We were able to place a block, so there will be a position to return.
					return_position = getPositionFromVector(worker_area, &three_by_two_defense_positions);
					return return_position;
				}
				else
				{
					worker_base->addTimesSearched(1);
					return return_position;
				}
			}
		}
	}
	//4x3 build location
	else if (build_type.tileWidth() == 4)
	{
		if (four_by_three_positions.size() == 0)
		{
			//Check if this area is one we want to try to place blocks in.
			if (!worker_base->getCanSearch())
			{
				//Cannot search this area.
				return return_position;
			}
			//No positions stored, attempt to place a block.
			if (tryPlacingBlocks(worker_area, build_type))
			{
				//We were able to place a block, so there will be a position to return.
				//If the building requires power, we need to use the power checking version,
				//as there may not be a position with power available.
				if (build_type.requiresPsi())
					return_position = getPositionFromVectorWithPower(worker_area, &four_by_three_positions, build_type);
				else
					return_position = getPositionFromVector(worker_area, &four_by_three_positions);
				return return_position;
			}
			else
			{
				worker_base->addTimesSearched(1);
				return return_position;
			}
		}
		else
		{
			//Attempt to fetch a position from the vector.
			//If the building requires power, we need to use the power checking version,
			//as there may not be a position with power available.
			if (build_type.requiresPsi())
				return_position = getPositionFromVectorWithPower(worker_area, &four_by_three_positions, build_type);
			else
				return_position = getPositionFromVector(worker_area, &four_by_three_positions);
			if (return_position != BWAPI::TilePositions::Invalid)
			{
				//found a position, return it.
				return return_position;
			}
			else
			{
				//Check if this area is one we want to try to place blocks in.
				if (!worker_base->getCanSearch())
				{
					//Cannot search this area.
					return return_position;
				}
				//No position found, try placing a block if building does not require Psi and returning a position from the placed block.
				if (!build_type.requiresPsi())
				{
					//Check if this area is one we want to try to place blocks in. If it is,
					//increment the search counter. If search counter is more than 4, mark
					//it for no further searching.
					if (!worker_base->getCanSearch())
					{
						//Cannot search this area.
						return return_position;
					}
					if (tryPlacingBlocks(worker_area, build_type))
					{
						//We were able to place a block, so there will be a position to return.
						//If the building requires power, we need to use the power checking version,
						//as there may not be a position with power available.
						if (build_type.requiresPsi())
							return_position = getPositionFromVectorWithPower(worker_area, &four_by_three_positions, build_type);
						else
							return_position = getPositionFromVector(worker_area, &four_by_three_positions);
						return return_position;
					}
					else
					{
						worker_base->addTimesSearched(1);
						return return_position;
					}
				}
				else
					return return_position;
			}
		}
	}
	//3x2 build location
	else if (build_type.tileWidth() == 3)
	{
		if (three_by_two_positions.size() == 0)
		{
			//Check if this area is one we want to try to place blocks in.
			if (!worker_base->getCanSearch())
			{
				//Cannot search this area.
				return return_position;
			}
			//No positions stored, attempt to place a block.
			if (tryPlacingBlocks(worker_area, build_type))
			{
				//We were able to place a block, so there will be a position to return.
				//If the building requires power, we need to use the power checking version,
				//as there may not be a position with power available.
				if (build_type.requiresPsi())
					return_position = getPositionFromVectorWithPower(worker_area, &three_by_two_positions, build_type);
				else
					return_position = getPositionFromVector(worker_area, &three_by_two_positions);
				return return_position;
			}
			else
			{
				worker_base->addTimesSearched(1);
				return return_position;
			}
		}
		else
		{
			//Attempt to fetch a position from the vector.
			//If the building requires power, we need to use the power checking version,
			//as there may not be a position with power available.
			if (build_type.requiresPsi())
				return_position = getPositionFromVectorWithPower(worker_area, &three_by_two_positions, build_type);
			else
				return_position = getPositionFromVector(worker_area, &three_by_two_positions);
			if (return_position != BWAPI::TilePositions::Invalid)
			{
				//found a position, return it.
				return return_position;
			}
			else
			{
				//No position found, try placing a block if building does not require Psi and returning a position from the placed block.
				if (!build_type.requiresPsi())
				{
					//Check if this area is one we want to try to place blocks in.
					if (!worker_base->getCanSearch())
					{
						//Cannot search this area.
						return return_position;
					}
					if (tryPlacingBlocks(worker_area, build_type))
					{
						//We were able to place a block, so there will be a position to return.
						//If the building requires power, we need to use the power checking version,
						//as there may not be a position with power available.
						if (build_type.requiresPsi())
							return_position = getPositionFromVectorWithPower(worker_area, &three_by_two_positions, build_type);
						else
							return_position = getPositionFromVector(worker_area, &three_by_two_positions);
						return return_position;
					}
					else
					{
						worker_base->addTimesSearched(1);
						return return_position;
					}
				}
				else
					return return_position;
			}
		}
	}
	//2x2 build location
	else
	{
		if (two_by_two_positions.size() == 0)
		{
			//Check if this area is one we want to try to place blocks in.
			if (!worker_base->getCanSearch())
			{
				//Cannot search this area.
				return return_position;
			}
			//No positions stored, attempt to place a block.
			if (tryPlacingBlocks(worker_area, build_type))
			{
				//We were able to place a block, so there will be a position to return.
				//If the building requires power, we need to use the power checking version,
				//as there may not be a position with power available.
				if (build_type.requiresPsi())
					return_position = getPositionFromVectorWithPower(worker_area, &two_by_two_positions, build_type);
				else
					return_position = getPositionFromVector(worker_area, &two_by_two_positions);
				return return_position;
			}
			else
			{
				worker_base->addTimesSearched(1);
				return return_position;
			}
		}
		else
		{
			//Attempt to fetch a position from the vector.
			//If the building requires power, we need to use the power checking version,
			//as there may not be a position with power available.
			if (build_type.requiresPsi())
				return_position = getPositionFromVectorWithPower(worker_area, &two_by_two_positions, build_type);
			else
				return_position = getPositionFromVector(worker_area, &two_by_two_positions);
			if (return_position != BWAPI::TilePositions::Invalid)
			{
				//found a position, return it.
				return return_position;
			}
			else
			{
				//No position found, try placing a block if building does not require Psi and returning a position from the placed block.
				if (!build_type.requiresPsi())
				{
					//Check if this area is one we want to try to place blocks in.
					if (!worker_base->getCanSearch())
					{
						//Cannot search this area.
						return return_position;
					}
					if (tryPlacingBlocks(worker_area, build_type))
					{
						//We were able to place a block, so there will be a position to return.
						//If the building requires power, we need to use the power checking version,
						//as there may not be a position with power available.
						if (build_type.requiresPsi())
							return_position = getPositionFromVectorWithPower(worker_area, &two_by_two_positions, build_type);
						else
							return_position = getPositionFromVector(worker_area, &two_by_two_positions);
						return return_position;
					}
					else
					{
						worker_base->addTimesSearched(1);
						return return_position;
					}
				}
				else
					return return_position;
			}
		}
	}
}

bool GameState::checkRegionBuildable(BWAPI::TilePosition top_left, std::pair<int, int> size)
{
	//check that the region defined in the parameters is clear in the buildmap.
	for (int x = top_left.x - 1; x < top_left.x + size.first + 1; x++)
	{
		if (x > BWAPI::Broodwar->mapWidth())
			return false;
		for (int y = top_left.y - 1; y < top_left.y + size.second + 1; y++)
		{
			if (y > BWAPI::Broodwar->mapHeight())
				return false;
			if (y * BWAPI::Broodwar->mapWidth() + x < build_position_map.size())
			{
				if (!build_position_map.at(y * BWAPI::Broodwar->mapWidth() + x).first.unobstructed)
				{
					return false;
				}
			}
			else
				return false;
		}
	}
	return true;
}

bool GameState::placeBlock(const BWEM::Area* area, std::pair<int, int> block_size, BlockType block_type, BWAPI::TilePosition search_center = BWAPI::TilePositions::Invalid)
{
	//Place block that contains a spot for the unittype passed. If placed, return true, if not, return false.
	bool block_placed = false;
	bool out_of_area = false;
	BWAPI::TilePosition position_to_check;
	if (search_center == BWAPI::TilePositions::Invalid)
	{
		if (block_type == BlockType::TBunkerTurret)
		{
			if (area->Bases().size() == 0)
				search_center = (BWAPI::TilePosition)area->Top();
			else
			{
				search_center = area->Bases().begin()->Location();
				BWAPI::TilePosition forward_choke = BWAPI::TilePositions::Invalid;
				for (auto choke_point : area->ChokePoints())
				{
					if (!choke_point->Blocked())
					{
						int base_class_1 = getBaseforArea(choke_point->GetAreas().first)->getBaseClass();
						int base_class_2 = getBaseforArea(choke_point->GetAreas().second)->getBaseClass();
						if ((base_class_1 != 3 &&
							base_class_1 != 4) ||
							(base_class_2 != 3 &&
							base_class_2 != 4))
						{
							forward_choke = (BWAPI::TilePosition)choke_point->Center();
						}
					}
				}
				if (forward_choke != BWAPI::TilePositions::Invalid)
				{
					BWAPI::TilePosition direction = forward_choke - search_center;
					search_center += BWAPI::TilePosition(direction.x / 1.2, direction.y / 1.2);
				}
			}
		}
		else
		{
			search_center = (BWAPI::TilePosition)area->Top();
		}
	}
	position_to_check = search_center;
	auto start_time = std::chrono::high_resolution_clock::now();
	const BWEM::Area* area_to_check;
	while (!block_placed)
	{
		if (checkRegionBuildable(position_to_check, block_size))
		{
			area_to_check = BWEM::Map::Instance().GetArea(position_to_check);
			if (area_to_check == area)
			{
				area_to_check = BWEM::Map::Instance().GetArea(position_to_check + BWAPI::TilePosition(block_size.first, block_size.second));
				if (area_to_check == area)
				{
					//This region is clear, and in the intended area, add build locations for this block and update buildmap.
					BWAPI::TilePosition position_to_add;
					position_to_add.x = position_to_check.x;
					position_to_add.y = position_to_check.y;
					switch (block_type)
					{
					case BlockType::TLargeAddonMacro:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Factory, true);
						six_by_three_positions.push_back(position_to_add);
						position_to_add.y += 3;
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Factory, true);
						six_by_three_positions.push_back(position_to_add);
						position_to_add.y += 3;
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Factory, true);
						six_by_three_positions.push_back(position_to_add);
						break;
					case BlockType::TMediumAddonMacro:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Factory, true);
						six_by_three_positions.push_back(position_to_add);
						position_to_add.y += 3;
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Factory, true);
						six_by_three_positions.push_back(position_to_add);
						break;
					case BlockType::TSmallAddonMacro:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Factory, true);
						six_by_three_positions.push_back(position_to_add);
						break;
					case BlockType::T2x1TurretBlock:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Missile_Turret, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add.x += 2;
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Missile_Turret, true);
						two_by_two_positions.push_back(position_to_add);
						break;
					case BlockType::T4x4SupplyBlock:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Supply_Depot, true);
						three_by_two_positions.push_back(position_to_add);
						position_to_add.x += 3;
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Supply_Depot, true);
						three_by_two_positions.push_back(position_to_add);
						position_to_add.y += 2;
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Supply_Depot, true);
						three_by_two_positions.push_back(position_to_add);
						position_to_add.x -= 3;
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Supply_Depot, true);
						three_by_two_positions.push_back(position_to_add);
						break;
					case BlockType::TMacro4Block:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Barracks, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add.x += 4;
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Barracks, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add.y += 3;
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Barracks, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add.x -= 4;
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Barracks, true);
						four_by_three_positions.push_back(position_to_add);
						break;
					case BlockType::TMediumMacroH:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Barracks, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add.x += 4;
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Barracks, true);
						four_by_three_positions.push_back(position_to_add);
						break;
					case BlockType::TMediumMacroV:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Barracks, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add.y += 3;
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Barracks, true);
						four_by_three_positions.push_back(position_to_add);
						break;
					case BlockType::TSmallMacro:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Barracks, true);
						four_by_three_positions.push_back(position_to_add);
						break;
					case BlockType::TTvZBunkerStart:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Barracks, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(1, 3);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Supply_Depot, true);
						three_by_two_defense_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(3, -2);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Supply_Depot, true);
						three_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(0, 2);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Barracks, true);
						four_by_three_positions.push_back(position_to_add);;
						break;
					case BlockType::TBunkerTurret:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Missile_Turret, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(2, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Terran_Missile_Turret, true);
						three_by_two_defense_positions.push_back(position_to_add);
						break;
					case BlockType::P2Tech:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(2, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(2, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(-1, 2);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Robotics_Facility, true);
						three_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(-3, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Robotics_Facility, true);
						three_by_two_positions.push_back(position_to_add);
						break;
					case BlockType::P4Macro:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Gateway, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(4, 1);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(2, -1);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Gateway, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(0, 3);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Gateway, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(-2, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(-4, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Gateway, true);
						four_by_three_positions.push_back(position_to_add);
						break;
					case BlockType::P4Tech:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Robotics_Facility, true);
						three_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(3, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Robotics_Facility, true);
						three_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(1, 2);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(-2, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(-2, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(0, 2);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Robotics_Facility, true);
						three_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(3, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Robotics_Facility, true);
						three_by_two_positions.push_back(position_to_add);
						break;
					case BlockType::PLargeMacroTech:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Gateway, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(4, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(2, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Robotics_Facility, true);
						three_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(0, 2);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Robotics_Facility, true);
						three_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(-2, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(-4, 1);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Gateway, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(4, 1);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(2, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Gateway, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(-2, 2);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(-4, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Gateway, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(4, 2);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(2, -1);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Gateway, true);
						four_by_three_positions.push_back(position_to_add);
						break;
					case BlockType::PSingleMacro:
						position_to_add += BWAPI::TilePosition(0, 1);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(2, -1);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Gateway, true);
						four_by_three_positions.push_back(position_to_add);
						break;
					case BlockType::PWideMacroTech:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Gateway, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(4, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Robotics_Facility, true);
						three_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(4, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Gateway, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(-2, 2);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(-2, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(-4, 1);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Gateway, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(4, 1);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(2, 0);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(2, -1);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Gateway, true);
						four_by_three_positions.push_back(position_to_add);
						position_to_add += BWAPI::TilePosition(-4, 3);
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Robotics_Facility, true);
						three_by_two_positions.push_back(position_to_add);
						break;
					case BlockType::PSinglePylon:
						updateBuildMap(position_to_add.x, position_to_add.y, BWAPI::UnitTypes::Protoss_Pylon, true);
						two_by_two_positions.push_back(position_to_add);
					}
					block_placed = true;
				}
				else
					out_of_area = true;
			}
			else
				out_of_area = true;
		}
		if (!block_placed)
		{
			auto end_time = std::chrono::high_resolution_clock::now();
			if (std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() > 1)
			{
				return false;
			}
			if (out_of_area)
			{
				//Out of the area we want to build in.
				position_to_check = search_center;
				out_of_area = false;
			}
			else
			{
				//This region is not clear, pick a new position and try again.
				position_to_check.x += getRandomInteger(-3, 3);
				position_to_check.y += getRandomInteger(-3, 3);
			}
		}
	}
	return true;
}

BWAPI::TilePosition GameState::getPositionFromVector(const BWEM::Area* area, std::vector<BWAPI::TilePosition>* position_vector)
{
	//Return the first found position in the designated area.
	auto position_iterator = position_vector->begin();
	while (position_iterator != position_vector->end())
	{
		const BWEM::Area* position_area = BWEM::Map::Instance().GetArea(*position_iterator);
		if (position_area)
		{
			if (position_area == area)
			{
				BWAPI::TilePosition return_position = *position_iterator;
				*position_iterator = position_vector->back();
				position_vector->pop_back();
				return return_position;
			}
		}
		position_iterator++;
	}
	return BWAPI::TilePositions::Invalid;
}

bool GameState::tryPlacingBlocks(const BWEM::Area* area, BWAPI::UnitType build_type)
{
	if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
	{
		if (build_type.canBuildAddon())
		{
			if (placeBlock(area, std::make_pair<int, int>(6, 9), BlockType::TLargeAddonMacro))
				return true;
			else
			{
				//No block placed. Try a different block type.
				if (placeBlock(area, std::make_pair<int, int>(6, 6), BlockType::TMediumAddonMacro))
					return true;
				else
				{
					//No block placed. Try a different block type.
					if (placeBlock(area, std::make_pair<int, int>(6, 3), BlockType::TSmallAddonMacro))
						return true;
					else
					{
						//No block placed, out of block types.
						return false;
					}
				}
			}
		}
		else if (build_type == BWAPI::UnitTypes::Terran_Bunker)
		{
			if (placeBlock(area, std::make_pair<int, int>(5, 2), BlockType::TBunkerTurret))
				return true;
			else
				//No block placed, out of block types.
				return false;
		}
		else if (build_type.tileWidth() == 4)
		{
			if (placeBlock(area, std::make_pair<int, int>(8, 6), BlockType::TMacro4Block))
				return true;
			else
			{
				//No block placed. Try a different block type.
				if (placeBlock(area, std::make_pair<int, int>(8, 3), BlockType::TMediumMacroH))
					return true;
				else
				{
					//No block placed. Try a different block type.
					if (placeBlock(area, std::make_pair<int, int>(4, 6), BlockType::TMediumMacroV))
						return true;
					else
					{
						//No block placed. Try a different block type.
						if (placeBlock(area, std::make_pair<int, int>(4, 3), BlockType::TSmallMacro))
							return true;
						else
						{
							//No block placed, out of block types.
							return false;
						}
					}
				}
			}
		}
		else if (build_type.tileWidth() == 3)
		{
			if (placeBlock(area, std::make_pair<int, int>(6, 4), BlockType::T4x4SupplyBlock))
				return true;
			else
			{
				//No block placed, out of block types.
				return false;
			}
		}
		else if (build_type.tileWidth() == 2)
		{
			if (placeBlock(area, std::make_pair<int, int>(4, 2), BlockType::T2x1TurretBlock))
				return true;
			else
			{
				//No block placed, out of block types.
				return false;
			}
		}
	}
	else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
	{
		if (build_type.tileWidth() == 4)
		{
			if (placeBlock(area, std::make_pair<int, int>(10, 10), BlockType::PLargeMacroTech))
				return true;
			else
			{
				//No block placed. Try a different block type.
				if (placeBlock(area, std::make_pair<int, int>(12, 8), BlockType::PWideMacroTech))
					return true;
				else
				{
					//No block placed. Try a different block type.
					if (placeBlock(area, std::make_pair<int, int>(10, 6), BlockType::P4Macro))
						return true;
					else
					{
						//No block placed. Try a different block type.
						if (placeBlock(area, std::make_pair<int, int>(6, 3), BlockType::PSingleMacro))
							return true;
						else
						{
							//No block placed, out of block types.
							return false;
						}
					}
				}
			}
		}
		else if (build_type.tileWidth() == 3)
		{
			if (placeBlock(area, std::make_pair<int, int>(6, 6), BlockType::P4Tech))
				return true;
			else
			{
				//No block placed. Try a different block type.
				if (placeBlock(area, std::make_pair<int, int>(6, 4), BlockType::P2Tech))
					return true;
				else
				{
					//No block placed, out of block types.
					return false;
				}
			}
		}
		else if (build_type.tileWidth() == 2)
		{
			if (placeBlock(area, std::make_pair<int, int>(10, 10), BlockType::PLargeMacroTech))
				return true;
			else
			{
				//No block placed. Try a different block type.
				if (placeBlock(area, std::make_pair<int, int>(12, 8), BlockType::PWideMacroTech))
					return true;
				else
				{
					//No block placed. Try a different block type.
					if (placeBlock(area, std::make_pair<int, int>(10, 6), BlockType::P4Macro))
						return true;
					else
					{
						//No block placed. Try a different block type.
						if (placeBlock(area, std::make_pair<int, int>(6, 3), BlockType::PSingleMacro))
							return true;
						else
						{
							//No block placed. Try a different block type.
							if (placeBlock(area, std::make_pair<int, int>(10, 6), BlockType::PSinglePylon))
								return true;
							else
							{
								//No block placed, out of block types.
								return false;
							}
						}
					}
				}
			}
		}
	}
}

void GameState::printReservedTilePositions()
{
	for (auto &current_position : six_by_three_positions)
	{
		BWAPI::Position top_left = (BWAPI::Position)current_position;
		BWAPI::Position bottom_right;
		bottom_right.x = top_left.x + 6 * 32;
		bottom_right.y = top_left.y + 3 * 32;
		BWAPI::Broodwar->drawBoxMap(top_left, bottom_right, BWAPI::Broodwar->self()->getColor());
	}
	for (auto &current_position : four_by_three_positions)
	{
		BWAPI::Position top_left = (BWAPI::Position)current_position;
		BWAPI::Position bottom_right;
		bottom_right.x = top_left.x + 4 * 32;
		bottom_right.y = top_left.y + 3 * 32;
		BWAPI::Broodwar->drawBoxMap(top_left, bottom_right, BWAPI::Broodwar->self()->getColor());
	}
	for (auto &current_position : three_by_two_positions)
	{
		BWAPI::Position top_left = (BWAPI::Position)current_position;
		BWAPI::Position bottom_right;
		bottom_right.x = top_left.x + 3 * 32;
		bottom_right.y = top_left.y + 2 * 32;
		BWAPI::Broodwar->drawBoxMap(top_left, bottom_right, BWAPI::Broodwar->self()->getColor());
	}
	for (auto &current_position : two_by_two_positions)
	{
		BWAPI::Position top_left = (BWAPI::Position)current_position;
		BWAPI::Position bottom_right;
		bottom_right.x = top_left.x + 2 * 32;
		bottom_right.y = top_left.y + 2 * 32;
		BWAPI::Broodwar->drawBoxMap(top_left, bottom_right, BWAPI::Broodwar->self()->getColor());
	}
	for (auto &current_position : three_by_two_defense_positions)
	{
		BWAPI::Position top_left = (BWAPI::Position)current_position;
		BWAPI::Position bottom_right;
		bottom_right.x = top_left.x + 3 * 32;
		bottom_right.y = top_left.y + 2 * 32;
		BWAPI::Broodwar->drawBoxMap(top_left, bottom_right, BWAPI::Broodwar->self()->getColor());
	}
}

void GameState::addPositionToQueue(BWAPI::Unit unit)
{
	switch (unit->getType().tileWidth())
	{
	case 4:
		if (unit->getType() == BWAPI::UnitTypes::Terran_Bunker)
			three_by_two_defense_positions.push_back(unit->getTilePosition());
		else
			four_by_three_positions.push_back(unit->getTilePosition());
		break;
	case 3:
		three_by_two_positions.push_back(unit->getTilePosition());
		break;
	case 2:
		two_by_two_positions.push_back(unit->getTilePosition());
		break;
	}
}

void GameState::addTimesRetreated(int new_times_retreated)
{
	times_retreated += new_times_retreated;
}

int GameState::getTimesRetreated()
{
	return times_retreated;
}

BWAPI::TilePosition GameState::getPositionFromVectorWithPower(const BWEM::Area* area, std::vector<BWAPI::TilePosition>* position_vector, BWAPI::UnitType build_type)
{
	//Return the first found position in the designated area.
	//If building requires power, check for power at the location.
	auto position_iterator = position_vector->begin();
	while (position_iterator != position_vector->end())
	{
		if (BWAPI::Broodwar->hasPower(*position_iterator, build_type))
		{
			const BWEM::Area* position_area = BWEM::Map::Instance().GetArea(*position_iterator);
			if (position_area)
			{
				if (position_area == area)
				{
					BWAPI::TilePosition return_position = *position_iterator;
					*position_iterator = position_vector->back();
					position_vector->pop_back();
					return return_position;
				}
			}
		}
		position_iterator++;
	}
	return BWAPI::TilePositions::Invalid;
}

void GameState::addTemplarArchives(int new_templar_archive)
{
	templar_archives += new_templar_archive;
}

int GameState::getTemplarArchives()
{
	return templar_archives;
}
