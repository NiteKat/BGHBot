#include "ExampleAIModule.h"
#include <iostream>
#include <ctime>



using namespace BWAPI;
using namespace Filter;
using namespace BWEM;
using namespace BWEM::BWAPI_ext;
using namespace BWEM::utils;

//namespace { auto & theMap = BWEM::Map::Instance(); }

void NiteKat::init() 
{
}

NiteKat::NiteKat() :
  Broodwar(BWAPIClient),
  fap(&Broodwar)
{
  BWEM::utils::MapDrawer::showSeas = false;
  BWEM::utils::MapDrawer::showLakes = false;
  BWEM::utils::MapDrawer::showUnbuildable = false;
  BWEM::utils::MapDrawer::showGroundHeight = false;
  BWEM::utils::MapDrawer::showMinerals = false;
  BWEM::utils::MapDrawer::showGeysers = false;
  BWEM::utils::MapDrawer::showStaticBuildings = false;
  BWEM::utils::MapDrawer::showBases = true;
  BWEM::utils::MapDrawer::showAssignedRessources = false;
  BWEM::utils::MapDrawer::showFrontier = false;
  BWEM::utils::MapDrawer::showCP = false;
}


void NiteKat::onStart()
{

	try
	{
    game_state = std::make_unique<GameState>(&Broodwar);
    worker_manager = std::make_unique<WorkerManager>(&Broodwar);
    macro_manager = std::make_unique<MacroManager>(&Broodwar);
    military_manager = std::make_unique<MilitaryManager>(&Broodwar);
		srand(time(NULL));
		// Print the map name.
		// BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
		Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;
		//game_state->setMapName(Broodwar->mapFileName());
		//game_state->readConfigFile();

		// Enable the UserInput flag, which allows us to control the bot and type messages.
		//Broodwar->enableFlag(Flag::UserInput);

		// Uncomment the following line and the bot will know about everything through the fog of war (cheat).
		//Broodwar->enableFlag(Flag::CompleteMapInformation);

		// Set the command optimization level so that common commands can be grouped
		// and reduce the bot's APM (Actions Per Minute).
		Broodwar->setCommandOptimizationLevel(2);

		// Check if this is a replay
		if (Broodwar->isReplay())
		{

			// Announce the players in the replay
			Broodwar << "The following players are in this replay:" << std::endl;

			// Iterate all the players in the game using a std:: iterator
			Playerset players = Broodwar->getPlayers();
			for (auto p : players)
			{
				// Only print the player if they are not an observer
				if (!p->isObserver())
					Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
			}

		}
		else // if this is not a replay
		{
			// Retrieve you and your enemy's races. enemy() will just return the first enemy.
			// If you wish to deal with multiple enemies then you must use enemies().
			if (Broodwar->enemy()) // First make sure there is an enemy
				Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;

			Broodwar << "Map initialization..." << std::endl;
      
      BWEM::Map::Instance().Initialize(&Broodwar);
			BWEM::Map::Instance().EnableAutomaticPathAnalysis();
			bool startingLocationsOK = BWEM::Map::Instance().FindBasesForStartingLocations();
			assert(startingLocationsOK);

			Broodwar << "gg" << std::endl;
			for (const auto &area : BWEM::Map::Instance().Areas())
			{
				int base_region = 0;
        auto test = BWEM::Map::Instance().GetArea(Broodwar->self()->getStartLocation());
				if (BWEM::Map::Instance().GetArea(Broodwar->self()->getStartLocation())->BottomRight() == area.BottomRight())
					base_region = 3;
				else if (area.Minerals().size() > 0)
					base_region = 1;
				AIBase new_base(&area, base_region, &Broodwar);
				if (new_base.getBaseClass() == 3)
					new_base.toggleScouted();
				game_state->addAIBase(new_base);
			}
			for (auto unit : Broodwar->getStaticNeutralUnits())
			{
				Resource new_mineral(unit);
				if (unit->getType().isMineralField() &&
					unit->getInitialResources() > 0 &&
					BWEM::Map::Instance().GetArea(unit->getTilePosition()) != nullptr)
					game_state->getContainingBase(unit)->addMineral(new_mineral);
			}
			auto base_list_iterator = game_state->getBaseList()->begin();
			while (base_list_iterator != game_state->getBaseList()->end())
			{
				if (base_list_iterator->getBaseClass() == 3)
				{
					auto base_list_iterator2 = game_state->getBaseList()->begin();
					while (base_list_iterator2 != game_state->getBaseList()->end())
					{
						if (base_list_iterator2->getBaseClass() == 0 &&
							BWEM::Map::Instance().GetPath((BWAPI::Position)base_list_iterator2->getArea()->Top(), (BWAPI::Position)base_list_iterator->getArea()->Top()).size() == 1)
							base_list_iterator2->setBaseClass(3);
						base_list_iterator2++;
					}
					base_list_iterator = game_state->getBaseList()->end();
				}
				else
					base_list_iterator++;
			}
		}
		Objective new_objective;
		new_objective.setObjective(ObjectiveTypes::Defend);
		game_state->addObjective(new_objective);
		game_state->initializeBuildMap();
		/*if (Broodwar->self()->getRace() == Races::Terran)
		{
			if (Broodwar->enemies().size() > 1 + Broodwar->allies().size())
				game_state->setBuildOrder(BuildOrder::BGHMech);
			else
			{
				int build_order_decider = rand() % 2 + 1;
				if (build_order_decider == 2)
				{
					game_state->setBuildOrder(BuildOrder::BGHMech);
				}
			}
		}*/
		if (Broodwar->self()->getRace() == Races::Protoss)
		{
			//game_state->setBuildOrder(BuildOrder::P2Gate1);
			if (Broodwar->enemies().size() == 1 &&
				Broodwar->enemy()->getRace() == BWAPI::Races::Terran)
				game_state->setBuildOrder(BuildOrder::P2Gate1);
			else if (Broodwar->enemies().size() == 1 &&
				Broodwar->enemy()->getRace() == BWAPI::Races::Protoss)
				game_state->setBuildOrder(BuildOrder::P4GateGoonOpening);
			/*else if (Broodwar->enemies().size() == 1 &&
				Broodwar->enemy()->getRace() == BWAPI::Races::Zerg)
				game_state->setBuildOrder(BuildOrder::PForgeFastExpand9poolOpening);*/
		}
		else if (Broodwar->self()->getRace() == Races::Zerg)
		{
			if (Broodwar->enemies().size() == 1)
				game_state->setBuildOrder(BuildOrder::FivePool);
		}
		/*else
		{
			game_state->setBuildOrder(BuildOrder::TAyumiBuildOpening);
		}*/
		game_state->initializeGasLocations();
		Broodwar << game_state->getEnemyUnits()->size() << std::endl;
		Broodwar << Broodwar->getGameType() << std::endl;
		assess_game_time = 0;
		manage_workers_time = 0;
		check_macro_time = 0;
		check_military_time = 0;
		game_state->initDefenseGrid();
		if (Broodwar->getGameType() == BWAPI::GameTypes::Team_Free_For_All ||
			Broodwar->getGameType() == BWAPI::GameTypes::Team_Melee)
		{
			game_state->setBuildOrder(BuildOrder::TeamMeleeorFFA);
			for (auto& unit : Broodwar->self()->getUnits())
			{
				if (unit->getType().getRace() == BWAPI::Races::Terran)
					game_state->setTerran(true);
				else if (unit->getType().getRace() == BWAPI::Races::Protoss)
					game_state->setProtoss(true);
				else if (unit->getType().getRace() == BWAPI::Races::Zerg)
					game_state->setZerg(true);
				switch (unit->getType())
				{
				case UnitTypes::Terran_Command_Center:
					game_state->addCommandCenter(1);
					break;
				case UnitTypes::Protoss_Nexus:
					game_state->addNexus(1);
					break;
				}
			}
		}
	}
	catch (const std::exception & e)
	{
		Broodwar << "EXCEPTION: " << e.what() << std::endl;
	}
}

void NiteKat::onEnd(bool isWinner)
{
  // Called when the game ends
  if ( isWinner )
  {
    // Log your win here!
  }
}

void NiteKat::onFrame()
{
	try
	{
		/*if (Broodwar->getSelectedUnits().size() == 1)
		{
			Broodwar->drawTextScreen(0, 20, "x=%i y=%i", (*Broodwar->getSelectedUnits().begin())->getTilePosition().x, (*Broodwar->getSelectedUnits().begin())->getTilePosition().y);
			Broodwar->drawTextScreen(0, 30, "Order: %s, current cooldown: %d", (*Broodwar->getSelectedUnits().begin())->getOrder().c_str(), (*Broodwar->getSelectedUnits().begin())->getGroundWeaponCooldown());
		}*/
		//game_state->printExpansionScores();
		std::clock_t start_clock;
		//game_state->printReservedTilePositions();
		//game_state->printDefenseGrid();
		//game_state->drawMineralLockLines();
		// Called once every game frame
		//Debug code for drawing the map and listing base classes.
    BWEM::Map::Instance().Draw(&Broodwar);
		//BWEM::utils::drawMap(BWEM::Map::Instance());
		/*if (game_state->getBaseList()->size() > 0)
		{
			auto base_list_iterator = game_state->getBaseList()->begin();
			while (base_list_iterator != game_state->getBaseList()->end())
			{
				Broodwar->drawTextMap((BWAPI::Position)base_list_iterator->getArea()->Top(), "%i", base_list_iterator->getBaseClass());
				base_list_iterator++;
			}
		}
		if (Broodwar->getSelectedUnits().size() == 1)
		{
			Broodwar->drawTextScreen(0, 20, "x=%i y=%i", (*Broodwar->getSelectedUnits().begin())->getTilePosition().x, (*Broodwar->getSelectedUnits().begin())->getTilePosition().y);
			Broodwar->drawTextScreen(0, 30, "Order: %s, current cooldown: %d", (*Broodwar->getSelectedUnits().begin())->getOrder().c_str(), (*Broodwar->getSelectedUnits().begin())->getGroundWeaponCooldown());
		}
		else
		{
			Broodwar->drawTextScreen(0, 20, "Nothing selected.");
		}
		for (int index = 0; index < game_state->getBuildPositionMap()->size(); index++)
		{
			if (game_state->getBuildPositionMap()->at(index).first.unobstructed)
				Broodwar->drawTextMap((index - Broodwar->mapWidth() * (index / Broodwar->mapWidth())) * 32, (index / Broodwar->mapWidth()) * 32, "T");
			else
				Broodwar->drawTextMap((index - Broodwar->mapWidth() * (index / Broodwar->mapWidth())) * 32, (index / Broodwar->mapWidth()) * 32, "F");
		}*/
		/*Broodwar->drawTextScreen(0, 20, "# of Tanks = %i", game_state->getUnitTypeCount(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) + game_state->getUnitTypeCount(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode));
		if (game_state->getBuildOrder() == BuildOrder::BGHMech)
			Broodwar->drawTextScreen(0, 30, "Build Order is BGH Mech");
		else if (game_state->getBuildOrder() == BuildOrder::Default)
			Broodwar->drawTextScreen(0, 30, "Build Order is Default");
		else if (game_state->getBuildOrder() == BuildOrder::P2Gate1)
			Broodwar->drawTextScreen(0, 30, "Build Order is P2Gate1");
		else if (game_state->getBuildOrder() == BuildOrder::P4GateGoonLate)
			Broodwar->drawTextScreen(0, 30, "Build Order is P2GateGoonLate");
		else if (game_state->getBuildOrder() == BuildOrder::P4GateGoonMid)
			Broodwar->drawTextScreen(0, 30, "Build Order is P4GateGoonMid");
		else if (game_state->getBuildOrder() == BuildOrder::P4GateGoonOpening)
			Broodwar->drawTextScreen(0, 30, "Build Order is P4GateGoonOpening");
		else if (game_state->getBuildOrder() == BuildOrder::PForgeFastExpand9poolOpening)
			Broodwar->drawTextScreen(0, 30, "Build Order is PForgeFastExpand9poolOpening");
		else if (game_state->getBuildOrder() == BuildOrder::FivePool)
			Broodwar->drawTextScreen(0, 30, "Build Order is FivePool");*/
		//Broodwar->drawTextScreen(0, 40, "Minerals Committed: %i", game_state->getMineralsCommitted());
		//Broodwar->drawTextScreen(0, 60, "Gas Committed: %i", game_state->getGasCommitted());
		/*Broodwar->drawTextScreen(0, 70, "Assess Game Time: %f", assess_game_time);
		Broodwar->drawTextScreen(0, 80, "Manage Worker Time: %f", manage_workers_time);
		Broodwar->drawTextScreen(0, 90, "Check Macro Time: %f", check_macro_time);
		Broodwar->drawTextScreen(0, 100, "Check Military Time: %f", check_military_time);
		Broodwar->drawTextScreen(0, 110, "My army strength: %f", game_state->getMyTotalStrength());
		Broodwar->drawTextScreen(0, 120, "Enemy army strength: %f", game_state->getEnemyTotalStrength());*/

		/*Broodwar->drawTextScreen(0, 40, "Terran Supply Used: %f", game_state->getSupplyUsed(Races::Terran));
		Broodwar->drawTextScreen(0, 50, "Protoss Supply Used: %f", game_state->getSupplyUsed(Races::Protoss));
		Broodwar->drawTextScreen(0, 60, "Zerg Supply Used: %f", game_state->getSupplyUsed(Races::Zerg));*/
		
		/*for (const auto &area : BWEM::Map::Instance().Areas())
		{
			Broodwar->drawTextMap((Position)area.BottomRight(), "%i", area.Id());
		}*/

		// Display the game frame rate as text in the upper left area of the screen
		
		Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
		Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());

		// Return if the game is a replay or is paused
		if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
			return;


		// Prevent spamming by only running our onFrame once every number of latency frames.
		// Latency frames are the number of frames before commands are processed.
		if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
			return;
		
		if (!game_state->getScouted() &&
			game_state->getAssignScout())
		{
			if (game_state->getIntendedScout().getUnit() == nullptr)
			{
				military_manager->scout(worker_manager, game_state);
				game_state->setScouted(true);
			}
			else
			{
				military_manager->scout(game_state->getIntendedScout(), game_state);
				game_state->setScouted(true);
			}
		}
		start_clock = std::clock();
		game_state->assessGame();
		game_state->updateUnits();
		assess_game_time = (double)((std::clock() - start_clock) / (double)CLOCKS_PER_SEC) * 1000;
		start_clock = std::clock();
		worker_manager->manageWorkers(game_state);
		manage_workers_time = (double)((std::clock() - start_clock) / (double)CLOCKS_PER_SEC) * 1000;
		start_clock = std::clock();
		macro_manager->checkMacro(worker_manager, game_state);
		check_macro_time = (double)((std::clock() - start_clock) / (double)CLOCKS_PER_SEC) * 1000;
		start_clock = std::clock();
		military_manager->checkMilitary(worker_manager, game_state);
		check_military_time = (double)((std::clock() - start_clock) / (double)CLOCKS_PER_SEC) * 1000;
		
	}
	catch (const std::exception & e)
	{
		Broodwar << "EXCEPTION: " << e.what() << std::endl;
	}
}

void NiteKat::onSendText(std::string text)
{

	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());


  // Make sure to use %s and pass the text as a parameter,
  // otherwise you may run into problems when you use the %(percent) character!

}

void NiteKat::onReceiveText(BWAPI::Player player, std::string text)
{
	BWEM::utils::MapDrawer::ProcessCommand(text);
  // Parse the received text
  Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void NiteKat::onPlayerLeft(BWAPI::Player player)
{
  // Interact verbally with the other players in the game by
  // announcing that the other player has left.
  Broodwar->sendText("Goodbye %s!", player->getName().data());
}

void NiteKat::onNukeDetect(BWAPI::Position target)
{

  // Check if the target is a valid position
  if ( Broodwar->isValid(target) )
  {
    // if so, print the location of the nuclear strike target
    Broodwar << "Nuclear Launch Detected at " << target << std::endl;
  }
  else 
  {
    // Otherwise, ask other players where the nuke is!
    Broodwar->sendText("Where's the nuke?");
  }

  // You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void NiteKat::onUnitDiscover(BWAPI::Unit unit)
{
}

void NiteKat::onUnitEvade(BWAPI::Unit unit)
{
}

void NiteKat::onUnitShow(BWAPI::Unit unit)
{
}

void NiteKat::onUnitHide(BWAPI::Unit unit)
{
}

void NiteKat::onUnitCreate(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().data(), unit->getType().c_str());
    }
  }
  else
  {
	  if (((unit->getType().isBuilding() &&
		  unit->getPlayer() == Broodwar->self()) ||
		   unit->getType().isResourceContainer()) &&
		   unit->getType().getRace() != Races::Protoss)
	  {
		  game_state->updateBuildMap(unit->getTilePosition().x, unit->getTilePosition().y, unit->getType(), true);
	  }
	  if (unit->getType().isBuilding() &&
		  unit->getType().getRace() == Races::Protoss &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  if (unit->getType() == BWAPI::UnitTypes::Protoss_Nexus)
			  game_state->updateBuildMap(unit->getTilePosition().x, unit->getTilePosition().y, unit->getType(), true);
		  auto build_worker_iterator = game_state->getBuildWorkers()->begin();
		  while (build_worker_iterator != game_state->getBuildWorkers()->end())
		  {

			  if (build_worker_iterator->getBuildType() == unit->getType() &&
				  build_worker_iterator->getTargetBuildLocation() == unit->getTilePosition())
			  {
				  if (game_state->getBuildOrder() == BuildOrder::PForgeFastExpand9poolOpening &&
					  !game_state->getScouted())
				  {
					  game_state->setAssignScout(true);
					  Object new_scout(*build_worker_iterator);
					  game_state->setIntendedScout(new_scout);
					  auto erase_iterator = build_worker_iterator;
					  build_worker_iterator = game_state->getBuildWorkers()->erase(erase_iterator);
					  build_worker_iterator = game_state->getBuildWorkers()->end();
				  }
				  else
				  {
					  Object new_mineral_worker(*build_worker_iterator);
					  new_mineral_worker.setBuildType(BWAPI::UnitTypes::Unknown);
					  new_mineral_worker.setBaseClass(0);
					  game_state->getMineralWorkers()->push_back(new_mineral_worker);
					  auto erase_iterator = build_worker_iterator;
					  build_worker_iterator = game_state->getBuildWorkers()->erase(erase_iterator);
					  build_worker_iterator = game_state->getBuildWorkers()->end();
				  }
			  }
			  else
				  build_worker_iterator++;
		  }
		  game_state->addMineralsCommitted(-1 * unit->getType().mineralPrice());
		  game_state->addGasCommitted(-1 * unit->getType().gasPrice());
	  }
	  if (unit->getType() == UnitTypes::Terran_SCV &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  game_state->addSupplyUsed(1, Races::Terran);
	  }
	  else if (unit->getType() == UnitTypes::Protoss_Probe &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  game_state->addSupplyUsed(1, Races::Protoss);
	  }
	  else if (unit->getType() == UnitTypes::Zerg_Drone &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  game_state->addSupplyUsed(1, Races::Zerg);
	  }
	  else if ((unit->getType() == UnitTypes::Terran_Marine ||
		  unit->getType() == UnitTypes::Terran_Medic )&&
		  unit->getPlayer() == Broodwar->self())
	  {
		  game_state->addSupplyUsed(1, Races::Terran);
	  }
	  else if ((unit->getType() == UnitTypes::Protoss_Zealot ||
		  unit->getType() == UnitTypes::Protoss_Dragoon ||
		  unit->getType() == UnitTypes::Protoss_High_Templar) &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  game_state->addSupplyUsed(2, Races::Protoss);
	  }
	  else if (unit->getType() == UnitTypes::Terran_Supply_Depot &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
		  game_state->addMineralsCommitted(-100);
	  }
	  else if (unit->getType() == UnitTypes::Protoss_Pylon &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
	  }
	  else if (unit->getType() == UnitTypes::Terran_Barracks &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
		  game_state->addMineralsCommitted(-150);
	  }
	  else if (unit->getType() == UnitTypes::Protoss_Gateway &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
	  }
	  else if (unit->getType() == UnitTypes::Terran_Refinery &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
		  game_state->addMineralsCommitted(-100);
		  game_state->setGeyserUsed(unit->getTilePosition());
	  }
	  else if (unit->getType() == UnitTypes::Terran_Comsat_Station &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
		  game_state->addMineralsCommitted(-50);
		  game_state->addGasCommitted(-50);
		  if (!game_state->checkComsatStation())
			game_state->toggleComsatStation();
	  }
	  else if (unit->getType() == UnitTypes::Terran_Academy &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
		  game_state->addMineralsCommitted(-150);
	  }
	  else if (unit->getType() == UnitTypes::Zerg_Larva &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_larva(unit, game_state->getContainingBase(unit));
		  game_state->addLarva(new_larva);
	  }
	  else if (unit->getType() == UnitTypes::Zerg_Overlord &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_detector(unit, game_state->getContainingBase(unit));
		  game_state->addDetector(new_detector);
		  game_state->addSupplyExpected(8, Races::Zerg);
	  }
	  else if (unit->getType() == UnitTypes::Zerg_Hatchery &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addSupplyExpected(1, Races::Zerg);
		  game_state->addBuilding(new_building);
	  }
	  else if ((unit->getType() == BWAPI::UnitTypes::Protoss_Forge ||
		  unit->getType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core ||
		  unit->getType() == BWAPI::UnitTypes::Protoss_Robotics_Facility ||
		  unit->getType() == BWAPI::UnitTypes::Protoss_Observatory ||
		  unit->getType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun || 
		  unit->getType() == BWAPI::UnitTypes::Protoss_Templar_Archives ||
		  unit->getType() == BWAPI::UnitTypes::Protoss_Photon_Cannon) &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Protoss_Observer &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_detector(unit, game_state->getContainingBase(unit));
		  game_state->addSupplyUsed(1, BWAPI::Races::Protoss);
		  game_state->addDetector(new_detector);
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode)
	  {
		  game_state->addSupplyUsed(2, Races::Terran);
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Terran_Goliath)
	  {
		  game_state->addSupplyUsed(2, Races::Terran);
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Terran_Factory)
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
		  game_state->addMineralsCommitted(-200);
		  game_state->addGasCommitted(-150);
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Terran_Machine_Shop)
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Terran_Armory)
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
		  game_state->addMineralsCommitted(-100);
		  game_state->addGasCommitted(-50);
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Terran_Command_Center &&
		  Broodwar->elapsedTime() > 5)
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
		  game_state->addSupplyExpected(10, Races::Terran);
		  game_state->addMineralsCommitted(-400);
		  if (game_state->getExpanding())
		  {
			  game_state->toggleExpanding();
			  game_state->setTargetExpansion(nullptr);
		  }
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Protoss_Nexus &&
		  Broodwar->elapsedTime() > 5)
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
		  game_state->addSupplyExpected(9, Races::Protoss);
 		  game_state->addMineralsCommitted(-400);
		  if (game_state->getExpanding())
		  {
			  game_state->toggleExpanding();
			  game_state->setTargetExpansion(nullptr);
		  }
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Terran_Bunker)
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
		  game_state->addMineralsCommitted(-100);
	  }
	  else if (unit->getType() == UnitTypes::Terran_Engineering_Bay &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
		  game_state->addMineralsCommitted(-125);
	  }
	  else if (unit->getType() == UnitTypes::Terran_Starport &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
		  game_state->addMineralsCommitted(-1 * unit->getType().mineralPrice());
		  game_state->addGasCommitted(-1 * unit->getType().gasPrice());
	  }
	  else if (unit->getType() == UnitTypes::Terran_Science_Facility &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state->getContainingBase(unit));
		  game_state->addBuilding(new_building);
		  game_state->addMineralsCommitted(-1 * unit->getType().mineralPrice());
		  game_state->addGasCommitted(-1 * unit->getType().gasPrice());
	  }
  }
}

void NiteKat::onUnitDestroy(BWAPI::Unit unit)
{
	try
	{
		if (unit->getType().isBuilding() &&
			unit->getPlayer() == Broodwar->self() &&
			!unit->getType().isAddon() &&
			!unit->getType().isResourceDepot() &&
			!unit->getType().isRefinery())
		{
			//game_state->updateBuildMap(unit->getTilePosition().x, unit->getTilePosition().y, unit->getType(), false);
			game_state->addPositionToQueue(unit);
		}
		if (unit->getPlayer()->isEnemy(Broodwar->self()))
		{
			auto dead_enemy = game_state->getEnemyUnits()->find(static_cast<int>(unit->getID()));
			if (dead_enemy != game_state->getEnemyUnits()->end())
				game_state->getEnemyUnits()->erase(dead_enemy);
		}
		if (unit->getType().isMineralField())
		{
			BWEM::Map::Instance().OnMineralDestroyed(unit);
			game_state->removeMineral(unit);
		}
		else if (unit->getType().isSpecialBuilding())
		{
			BWEM::Map::Instance().OnStaticBuildingDestroyed(unit);
		}
		if ((unit->getType() == UnitTypes::Terran_SCV ||
			unit->getType() == UnitTypes::Terran_Marine ||
			unit->getType() == UnitTypes::Terran_Medic ||
			unit->getType() == UnitTypes::Protoss_Probe ||
			unit->getType() == UnitTypes::Zerg_Drone ||
			unit->getType() == UnitTypes::Protoss_Observer) &&
			unit->getPlayer() == Broodwar->self())
		{
			switch (unit->getType().getRace())
			{
			case Races::Terran:
				game_state->addSupplyUsed(-1, Races::Terran);
				break;
			case Races::Protoss:
				game_state->addSupplyUsed(-1, Races::Protoss);
				break;
			default:
				game_state->addSupplyUsed(-1, Races::Zerg);
			}
		}
		else if ((unit->getType() == UnitTypes::Protoss_Zealot ||
			unit->getType() == UnitTypes::Protoss_Dragoon ||
			unit->getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode ||
			unit->getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode ||
			unit->getType() == UnitTypes::Terran_Goliath ||
			unit->getType() == UnitTypes::Protoss_High_Templar) &&
			unit->getPlayer() == Broodwar->self())
		{
			switch (unit->getType().getRace())
			{
			case Races::Terran:
				game_state->addSupplyUsed(-2, Races::Terran);
				break;
			default:
				game_state->addSupplyUsed(-2, Races::Protoss);
			}
		}
		else if ((unit->getType() == UnitTypes::Terran_Supply_Depot ||
			unit->getType() == UnitTypes::Protoss_Pylon ||
			unit->getType() == UnitTypes::Zerg_Overlord) &&
			unit->getPlayer() == Broodwar->self())
		{
			if (unit->isCompleted())
			{
				switch (unit->getType().getRace())
				{
				case Races::Terran:
					game_state->addSupplyTotal(-8, Races::Terran);
					break;
				case Races::Protoss:
					game_state->addSupplyTotal(-8, Races::Protoss);
					break;
				default:
					game_state->addSupplyTotal(-8, Races::Zerg);
				}
			}
			switch (unit->getType().getRace())
			{
			case Races::Terran:
				game_state->addSupplyExpected(-8, Races::Terran);
				break;
			case Races::Protoss:
				game_state->addSupplyExpected(-8, Races::Protoss);
				break;
			default:
				game_state->addSupplyExpected(-8, Races::Zerg);
			}
			
		}
		else if ((unit->getType() == UnitTypes::Terran_Barracks ||
			unit->getType() == UnitTypes::Protoss_Gateway) &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->addBarracks(-1);
		}
		else if ((unit->getType() == UnitTypes::Terran_Refinery ||
			unit->getType() == UnitTypes::Zerg_Extractor ||
			unit->getType() == UnitTypes::Protoss_Assimilator) &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->setGeyserOpen(unit->getTilePosition());
			game_state->addGas(-1);
		}
		else if (unit->getType() == UnitTypes::Terran_Academy &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->toggleAcademy();
		}
		else if (unit->getType() == UnitTypes::Zerg_Zergling &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->addSupplyUsed(-.5, Races::Zerg);
		}
		else if (unit->getType() == UnitTypes::Zerg_Hydralisk &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->addSupplyUsed(-1, Races::Zerg);
		}
		else if (unit->getType() == UnitTypes::Protoss_Observatory &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->addObservatory(-1);
		}
		else if (unit->getType() == UnitTypes::Protoss_Robotics_Facility &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->addRoboticsFacility(-1);
		}
		else if (unit->getType() == UnitTypes::Protoss_Citadel_of_Adun &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->addCitadelofAdun(-1);
		}
		else if (unit->getType() == UnitTypes::Protoss_Cybernetics_Core &&
			unit->getPlayer() == Broodwar->self() &&
			game_state->checkCyberCore())
		{
			game_state->toggleCyberCore();
		}
		else if (unit->getType() == UnitTypes::Terran_Factory &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->addFactory(-1);
		}
		else if (unit->getType() == UnitTypes::Terran_Bunker &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->addBunker(-1);
		}
		else if (unit->getType() == UnitTypes::Terran_Engineering_Bay &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->addEngineeringBay(-1);
		}
		else if (unit->getType() == UnitTypes::Terran_Armory &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->addArmory(-1);
		}
		else if (unit->getType() == UnitTypes::Terran_Starport &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->addStarport(-1);
		}
		else if (unit->getType() == UnitTypes::Terran_Science_Facility &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->addScienceFacility(-1);
		}
		else if (unit->getType() == UnitTypes::Protoss_Templar_Archives &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->addTemplarArchives(-1);
		}
		else if (unit->getType() == UnitTypes::Zerg_Hydralisk_Den &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->addHydraliskDen(-1);
		}
	}
	catch (const std::exception & e)
	{
		Broodwar << "EXCEPTION: " << e.what() << std::endl;
	}
}

void NiteKat::onUnitMorph(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		// if we are in a replay, then we will print out the build order of the structures
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		{
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().data(), unit->getType().c_str());
		}
	}
	else
	{
		if (unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser)
		{
			auto dead_enemy = game_state->getEnemyUnits()->find(static_cast<int>(unit->getID()));
			if (dead_enemy != game_state->getEnemyUnits()->end())
				game_state->getEnemyUnits()->erase(dead_enemy);
		}
		if (unit->getBuildType() == BWAPI::UnitTypes::Zerg_Overlord &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_detector(unit, game_state->getContainingBase(unit));
			game_state->addDetector(new_detector);
			game_state->addSupplyExpected(8, Races::Zerg);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state->getContainingBase(unit));
			game_state->addBuilding(new_building);
			game_state->addSupplyExpected(1, Races::Zerg);
			game_state->addMineralsCommitted(-300);
			game_state->addSupplyUsed(-1, Races::Zerg);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state->getContainingBase(unit));
			game_state->addBuilding(new_building);
			game_state->addMineralsCommitted(-200);
			game_state->addSupplyUsed(-1, Races::Zerg);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state->getContainingBase(unit));
			game_state->addBuilding(new_building);
			game_state->addMineralsCommitted(-75);
			game_state->addSupplyUsed(-1, Races::Zerg);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Zerg_Creep_Colony &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state->addMineralsCommitted(-75);
			Object new_building(unit, game_state->getContainingBase(unit));
			game_state->addBuilding(new_building);
			game_state->addSupplyUsed(-1, Races::Zerg);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Zerg_Extractor &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state->getContainingBase(unit));
			game_state->addBuilding(new_building);
			game_state->addMineralsCommitted(-50);
			game_state->setGeyserUsed(unit->getTilePosition());
			game_state->addSupplyUsed(-1, Races::Zerg);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state->getContainingBase(unit));
			game_state->addBuilding(new_building);
			game_state->addMineralsCommitted(-150);
			game_state->addGasCommitted(-50);
			game_state->addSupplyUsed(-1, Races::Zerg);
		}
	}
}

void NiteKat::onUnitRenegade(BWAPI::Unit unit)
{
	if (unit->getType() == UnitTypes::Protoss_Assimilator &&
		unit->getPlayer() == Broodwar->self())
	{
		Object new_building(unit, game_state->getContainingBase(unit));
		game_state->addBuilding(new_building);
		game_state->setGeyserUsed(unit->getTilePosition());
	}
}

void NiteKat::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void NiteKat::onUnitComplete(BWAPI::Unit unit)
{
	if (unit->getType() == UnitTypes::Terran_Command_Center &&
		unit->getPlayer() == Broodwar->self())
	{
		Object new_building(unit, game_state->getContainingBase(unit));
		game_state->addBuilding(new_building);
		game_state->addSupplyTotal(10, Races::Terran);
		if (Broodwar->elapsedTime() > 5)
		{
			game_state->transferWorkersToNewBase(game_state->getContainingBase(unit));
		}
	}
	else if (unit->getType() == UnitTypes::Protoss_Nexus &&
		unit->getPlayer() == Broodwar->self())
	{
		Object new_building(unit, game_state->getContainingBase(unit));
		game_state->addBuilding(new_building);
		game_state->addSupplyTotal(9, Races::Protoss);
		if (Broodwar->elapsedTime() > 5)
		{
			game_state->transferWorkersToNewBase(game_state->getContainingBase(unit));
		}
	}
	else if ((unit->getType() == UnitTypes::Terran_Supply_Depot ||
		unit->getType() == UnitTypes::Protoss_Pylon ||
		unit->getType() == UnitTypes::Zerg_Overlord) &&
		unit->getPlayer() == Broodwar->self())
	{
		if (unit->getType() == UnitTypes::Protoss_Pylon)
			game_state->setPylonBuilding(false);
		auto race = unit->getType().getRace();
		game_state->addSupplyTotal(8, race);
	}
	else if (unit->getType() == UnitTypes::Zerg_Hatchery &&
		unit->getPlayer() == Broodwar->self())
	{
		game_state->addSupplyTotal(1, Races::Zerg);
	}
	else if (unit->getPlayer() == Broodwar->self() &&
		(unit->getType() == UnitTypes::Terran_SCV ||
		unit->getType() == UnitTypes::Protoss_Probe ||
		unit->getType() == UnitTypes::Zerg_Drone))
	{
		Object new_worker(unit, game_state->getContainingBase(unit));
		game_state->addMineralWorker(new_worker);
	}
	else if (unit->getPlayer() == Broodwar->self() &&
		unit->getType() != UnitTypes::Protoss_Observer &&
		unit->getType() != UnitTypes::Zerg_Overlord &&
		!unit->getType().isBuilding() &&
		unit->getType() != UnitTypes::Zerg_Larva &&
		game_state->getMilitary()->find(static_cast<int>(unit->getID())) == game_state->getMilitary()->end())
	{
		Object new_unit(unit);
		game_state->addUnit(new_unit);
	}
}
