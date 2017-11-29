#include "ExampleAIModule.h"
#include <iostream>
#include <ctime>



using namespace BWAPI;
using namespace Filter;
using namespace BWEM;
using namespace BWEM::BWAPI_ext;
using namespace BWEM::utils;

namespace { auto & theMap = BWEM::Map::Instance(); }


void ExampleAIModule::onStart()
{

	try
	{
		srand(time(NULL));
		// Print the map name.
		// BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
		Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

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
			theMap.Initialize();
			theMap.EnableAutomaticPathAnalysis();
			bool startingLocationsOK = theMap.FindBasesForStartingLocations();
			assert(startingLocationsOK);

			BWEM::utils::MapPrinter::Initialize(&theMap);
			BWEM::utils::printMap(theMap);
			BWEM::utils::pathExample(theMap);
			theMap.FindBasesForStartingLocations();

			Broodwar << "gg" << std::endl;
			for (const auto &area : theMap.Areas())
			{
				int base_region = 0;
				if (theMap.GetArea(Broodwar->self()->getStartLocation())->BottomRight() == area.BottomRight())
					base_region = 3;
				else if (area.Minerals().size() > 0)
					base_region = 1;
				AIBase new_base(&area, base_region);
				if (new_base.getBaseClass() == 3)
					new_base.toggleScouted();
				game_state.addAIBase(new_base);
			}
			auto base_list_iterator = game_state.getBaseList()->begin();
			while (base_list_iterator != game_state.getBaseList()->end())
			{
				if (base_list_iterator->getBaseClass() == 3)
				{
					auto base_list_iterator2 = game_state.getBaseList()->begin();
					while (base_list_iterator2 != game_state.getBaseList()->end())
					{
						if (base_list_iterator2->getBaseClass() == 0 &&
							theMap.GetPath((BWAPI::Position)base_list_iterator2->getArea()->Top(), (BWAPI::Position)base_list_iterator->getArea()->Top()).size() == 1)
							base_list_iterator2->setBaseClass(3);
						base_list_iterator2++;
					}
					base_list_iterator = game_state.getBaseList()->end();
				}
				else
					base_list_iterator++;
			}
		}
		Objective new_objective;
		new_objective.setObjective(ObjectiveTypes::Defend);
		game_state.addObjective(new_objective);
		game_state.initializeBuildMap();
		/*if (Broodwar->self()->getRace() == Races::Terran)
		{
			if (Broodwar->enemies().size() > 1 + Broodwar->allies().size())
				game_state.setBuildOrder(BuildOrder::BGHMech);
			else
			{
				int build_order_decider = rand() % 2 + 1;
				if (build_order_decider == 2)
				{
					game_state.setBuildOrder(BuildOrder::BGHMech);
				}
			}
		}*/
		if (Broodwar->self()->getRace() == Races::Protoss)
		{
			if (Broodwar->enemies().size() == 1 &&
				Broodwar->enemy()->getRace() == BWAPI::Races::Terran)
				game_state.setBuildOrder(BuildOrder::P2Gate1);
			else if (Broodwar->enemies().size() == 1 &&
				Broodwar->enemy()->getRace() == BWAPI::Races::Protoss)
				game_state.setBuildOrder(BuildOrder::P4GateGoonOpening);
		}
		scouted = false;
		game_state.initializeGasLocations();
		Broodwar << game_state.getEnemyUnits()->size() << std::endl;
		assess_game_time = 0;
		manage_workers_time = 0;
		check_macro_time = 0;
		check_military_time = 0;
	}
	catch (const std::exception & e)
	{
		Broodwar << "EXCEPTION: " << e.what() << std::endl;
	}
}

void ExampleAIModule::onEnd(bool isWinner)
{
  // Called when the game ends
  if ( isWinner )
  {
    // Log your win here!
  }
}

void ExampleAIModule::onFrame()
{
	try
	{
		std::clock_t start_clock;
		// Called once every game frame
		//Debug code for drawing the map and listing base classes.
		/*BWEM::utils::drawMap(theMap);
		if (game_state.getBaseList()->size() > 0)
		{
			auto base_list_iterator = game_state.getBaseList()->begin();
			while (base_list_iterator != game_state.getBaseList()->end())
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
		for (int index = 0; index < game_state.getBuildPositionMap()->size(); index++)
		{
			if (game_state.getBuildPositionMap()->at(index).first.unobstructed)
				Broodwar->drawTextMap((index - Broodwar->mapWidth() * (index / Broodwar->mapWidth())) * 32, (index / Broodwar->mapWidth()) * 32, "T");
			else
				Broodwar->drawTextMap((index - Broodwar->mapWidth() * (index / Broodwar->mapWidth())) * 32, (index / Broodwar->mapWidth()) * 32, "F");
		}*/
		if (game_state.getBuildOrder() == BuildOrder::BGHMech)
			Broodwar->drawTextScreen(0, 30, "Build Order is BGH Mech");
		else if (game_state.getBuildOrder() == BuildOrder::Default)
			Broodwar->drawTextScreen(0, 30, "Build Order is Default");
		else if (game_state.getBuildOrder() == BuildOrder::P2Gate1)
			Broodwar->drawTextScreen(0, 30, "Build Order is P2Gate1");
		else if (game_state.getBuildOrder() == BuildOrder::P4GateGoonLate)
			Broodwar->drawTextScreen(0, 30, "Build Order is P2GateGoonLate");
		else if (game_state.getBuildOrder() == BuildOrder::P4GateGoonMid)
			Broodwar->drawTextScreen(0, 30, "Build Order is P4GateGoonMid");
		else if (game_state.getBuildOrder() == BuildOrder::P4GateGoonOpening)
			Broodwar->drawTextScreen(0, 30, "Build Order is P4GateGoonOpening");
		Broodwar->drawTextScreen(0, 40, "Minerals Committed: %i", game_state.getMineralsCommitted());
		Broodwar->drawTextScreen(0, 60, "Gas Committed: %i", game_state.getGasCommitted());
		Broodwar->drawTextScreen(0, 70, "Assess Game Time: %f", assess_game_time);
		Broodwar->drawTextScreen(0, 80, "Manage Worker Time: %f", manage_workers_time);
		Broodwar->drawTextScreen(0, 90, "Check Macro Time: %f", check_macro_time);
		Broodwar->drawTextScreen(0, 100, "Check Military Time: %f", check_military_time);
		Broodwar->drawTextScreen(0, 110, "My army strength: %f", game_state.getMyTotalStrength());
		Broodwar->drawTextScreen(0, 120, "Enemy army strength: %f", game_state.getEnemyTotalStrength());



		/*for (const auto &area : theMap.Areas())
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
		
		if (scouted == false &&
			game_state.getSupplyUsed() >= 10)
		{
			military_manager.scout(worker_manager, game_state);
			scouted = true;
		}
		start_clock = std::clock();
		game_state.assessGame();
		assess_game_time = (double)((std::clock() - start_clock) / (double)CLOCKS_PER_SEC) * 1000;
		start_clock = std::clock();
		worker_manager.manageWorkers(game_state);
		manage_workers_time = (double)((std::clock() - start_clock) / (double)CLOCKS_PER_SEC) * 1000;
		start_clock = std::clock();
		macro_manager.checkMacro(&worker_manager, game_state);
		check_macro_time = (double)((std::clock() - start_clock) / (double)CLOCKS_PER_SEC) * 1000;
		start_clock = std::clock();
		military_manager.checkMilitary(worker_manager, game_state);
		check_military_time = (double)((std::clock() - start_clock) / (double)CLOCKS_PER_SEC) * 1000;
	}
	catch (const std::exception & e)
	{
		Broodwar << "EXCEPTION: " << e.what() << std::endl;
	}
}

void ExampleAIModule::onSendText(std::string text)
{

	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());


  // Make sure to use %s and pass the text as a parameter,
  // otherwise you may run into problems when you use the %(percent) character!

}

void ExampleAIModule::onReceiveText(BWAPI::Player player, std::string text)
{
	BWEM::utils::MapDrawer::ProcessCommand(text);
  // Parse the received text
  Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void ExampleAIModule::onPlayerLeft(BWAPI::Player player)
{
  // Interact verbally with the other players in the game by
  // announcing that the other player has left.
  Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void ExampleAIModule::onNukeDetect(BWAPI::Position target)
{

  // Check if the target is a valid position
  if ( target )
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

void ExampleAIModule::onUnitDiscover(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitEvade(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitShow(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitHide(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitCreate(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
  else
  {
	  if (((unit->getType().isBuilding() &&
		  unit->getPlayer() == Broodwar->self()) ||
		   unit->getType().isResourceContainer()) &&
		   unit->getType().getRace() != Races::Protoss)
	  {
		  game_state.updateBuildMap(unit->getTilePosition().x, unit->getTilePosition().y, unit->getType(), true);
	  }
	  if (unit->getType().isBuilding() &&
		  unit->getType().getRace() == Races::Protoss &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  if (unit->getType() == BWAPI::UnitTypes::Protoss_Nexus)
			  game_state.updateBuildMap(unit->getTilePosition().x, unit->getTilePosition().y, unit->getType(), true);
		  auto build_worker_iterator = game_state.getBuildWorkers()->begin();
		  while (build_worker_iterator != game_state.getBuildWorkers()->end())
		  {

			  if (build_worker_iterator->getBuildType() == unit->getType() &&
				  build_worker_iterator->getTargetBuildLocation() == unit->getTilePosition())
			  {
				  Object new_mineral_worker(*build_worker_iterator);
				  new_mineral_worker.setBuildType(BWAPI::UnitTypes::Unknown);
				  new_mineral_worker.setBaseClass(0);
				  game_state.getMineralWorkers()->push_back(new_mineral_worker);
				  auto erase_iterator = build_worker_iterator;
				  build_worker_iterator = game_state.getBuildWorkers()->erase(erase_iterator);
				  build_worker_iterator = game_state.getBuildWorkers()->end();
			  }
			  else
				  build_worker_iterator++;
		  }
		  game_state.addMineralsCommitted(-1 * unit->getType().mineralPrice());
		  game_state.addGasCommitted(-1 * unit->getType().gasPrice());
	  }
	  if (unit->getType() == UnitTypes::Terran_SCV &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  game_state.addSupplyUsed(1);
	  }
	  else if (unit->getType() == UnitTypes::Protoss_Probe &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  game_state.addSupplyUsed(1);
	  }
	  else if (unit->getType() == UnitTypes::Zerg_Drone &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  game_state.addSupplyUsed(1);
	  }
	  else if ((unit->getType() == UnitTypes::Terran_Marine ||
		  unit->getType() == UnitTypes::Terran_Medic )&&
		  unit->getPlayer() == Broodwar->self())
	  {
		  game_state.addSupplyUsed(1);
	  }
	  else if ((unit->getType() == UnitTypes::Protoss_Zealot ||
		  unit->getType() == UnitTypes::Protoss_Dragoon ||
		  unit->getType() == UnitTypes::Protoss_High_Templar) &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  game_state.addSupplyUsed(2);
	  }
	  else if (unit->getType() == UnitTypes::Terran_Supply_Depot &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state.getContainingBase(unit));
		  game_state.addBuilding(new_building);
		  game_state.addMineralsCommitted(-100);
	  }
	  else if (unit->getType() == UnitTypes::Protoss_Pylon &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state.getContainingBase(unit));
		  game_state.addBuilding(new_building);
	  }
	  else if (unit->getType() == UnitTypes::Terran_Barracks &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state.getContainingBase(unit));
		  game_state.addBuilding(new_building);
		  game_state.addMineralsCommitted(-150);
	  }
	  else if (unit->getType() == UnitTypes::Protoss_Gateway &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state.getContainingBase(unit));
		  game_state.addBuilding(new_building);
	  }
	  else if (unit->getType() == UnitTypes::Terran_Refinery &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state.getContainingBase(unit));
		  game_state.addBuilding(new_building);
		  game_state.addMineralsCommitted(-100);
		  game_state.setGeyserUsed(unit->getTilePosition());
	  }
	  else if (unit->getType() == UnitTypes::Terran_Comsat_Station &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state.getContainingBase(unit));
		  game_state.addBuilding(new_building);
		  game_state.addMineralsCommitted(-50);
		  game_state.addGasCommitted(-50);
		  if (!game_state.checkComsatStation())
			game_state.toggleComsatStation();
	  }
	  else if (unit->getType() == UnitTypes::Terran_Academy &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state.getContainingBase(unit));
		  game_state.addBuilding(new_building);
		  game_state.addMineralsCommitted(-150);
	  }
	  else if (unit->getType() == UnitTypes::Zerg_Larva &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_larva(unit, game_state.getContainingBase(unit));
		  game_state.addLarva(new_larva);
	  }
	  else if (unit->getType() == UnitTypes::Zerg_Overlord &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_detector(unit, game_state.getContainingBase(unit));
		  game_state.addDetector(new_detector);
		  game_state.addSupplyTotal(8);
		  game_state.addSupplyExpected(8);
	  }
	  else if (unit->getType() == UnitTypes::Zerg_Hatchery &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state.getContainingBase(unit));
		  game_state.addBuilding(new_building);
	  }
	  else if ((unit->getType() == BWAPI::UnitTypes::Protoss_Forge ||
		  unit->getType() == BWAPI::UnitTypes::Protoss_Cybernetics_Core ||
		  unit->getType() == BWAPI::UnitTypes::Protoss_Robotics_Facility ||
		  unit->getType() == BWAPI::UnitTypes::Protoss_Observatory ||
		  unit->getType() == BWAPI::UnitTypes::Protoss_Citadel_of_Adun || 
		  unit->getType() == BWAPI::UnitTypes::Protoss_Templar_Archives) &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_building(unit, game_state.getContainingBase(unit));
		  game_state.addBuilding(new_building);
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Protoss_Observer &&
		  unit->getPlayer() == Broodwar->self())
	  {
		  Object new_detector(unit, game_state.getContainingBase(unit));
		  game_state.addDetector(new_detector);
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode)
	  {
		  game_state.addSupplyUsed(2);
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Terran_Goliath)
	  {
		  game_state.addSupplyUsed(2);
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Terran_Factory)
	  {
		  Object new_building(unit, game_state.getContainingBase(unit));
		  game_state.addBuilding(new_building);
		  game_state.addMineralsCommitted(-200);
		  game_state.addGasCommitted(-150);
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Terran_Machine_Shop)
	  {
		  Object new_building(unit, game_state.getContainingBase(unit));
		  game_state.addBuilding(new_building);
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Terran_Armory)
	  {
		  Object new_building(unit, game_state.getContainingBase(unit));
		  game_state.addBuilding(new_building);
		  game_state.addMineralsCommitted(-100);
		  game_state.addGasCommitted(-50);
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Terran_Command_Center &&
		  Broodwar->elapsedTime() > 5)
	  {
		  Object new_building(unit, game_state.getContainingBase(unit));
		  game_state.addBuilding(new_building);
		  game_state.addSupplyExpected(10);
		  game_state.addMineralsCommitted(-400);
		  if (game_state.getExpanding())
		  {
			  game_state.toggleExpanding();
			  game_state.setTargetExpansion(nullptr);
		  }
	  }
	  else if (unit->getType() == BWAPI::UnitTypes::Protoss_Nexus &&
		  Broodwar->elapsedTime() > 5)
	  {
		  Object new_building(unit, game_state.getContainingBase(unit));
		  game_state.addBuilding(new_building);
		  game_state.addSupplyExpected(9);
 		  game_state.addMineralsCommitted(-400);
		  if (game_state.getExpanding())
		  {
			  game_state.toggleExpanding();
			  game_state.setTargetExpansion(nullptr);
		  }
	  }
  }
}

void ExampleAIModule::onUnitDestroy(BWAPI::Unit unit)
{
	try
	{
		if (unit->getType().isBuilding() &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state.updateBuildMap(unit->getTilePosition().x, unit->getTilePosition().y, unit->getType(), false);
		}
		if (unit->getPlayer()->isEnemy(Broodwar->self()))
		{
			auto dead_enemy = game_state.getEnemyUnits()->find(unit->getID());
			if (dead_enemy != game_state.getEnemyUnits()->end())
				game_state.getEnemyUnits()->erase(dead_enemy);
		}
		if (unit->getType().isMineralField())
		{
			theMap.OnMineralDestroyed(unit);
		}
		else if (unit->getType().isSpecialBuilding())
		{
			theMap.OnStaticBuildingDestroyed(unit);
		}

		if ((unit->getType() == UnitTypes::Terran_SCV ||
			unit->getType() == UnitTypes::Terran_Marine ||
			unit->getType() == UnitTypes::Terran_Medic ||
			unit->getType() == UnitTypes::Protoss_Probe ||
			unit->getType() == UnitTypes::Zerg_Drone ||
			unit->getType() == UnitTypes::Protoss_Observer) &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state.addSupplyUsed(-1);
		}
		else if ((unit->getType() == UnitTypes::Protoss_Zealot ||
			unit->getType() == UnitTypes::Protoss_Dragoon ||
			unit->getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode ||
			unit->getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode ||
			unit->getType() == UnitTypes::Terran_Goliath ||
			unit->getType() == UnitTypes::Protoss_High_Templar) &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state.addSupplyUsed(-2);
		}
		else if ((unit->getType() == UnitTypes::Terran_Supply_Depot ||
			unit->getType() == UnitTypes::Protoss_Pylon ||
			unit->getType() == UnitTypes::Zerg_Overlord) &&
			unit->getPlayer() == Broodwar->self())
		{
			if (unit->isCompleted())
				game_state.addSupplyTotal(-8);
			game_state.addSupplyExpected(-8);
		}
		else if ((unit->getType() == UnitTypes::Terran_Barracks ||
			unit->getType() == UnitTypes::Protoss_Gateway) &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state.addBarracks(-1);
		}
		else if ((unit->getType() == UnitTypes::Terran_Refinery ||
			unit->getType() == UnitTypes::Zerg_Extractor ||
			unit->getType() == UnitTypes::Protoss_Assimilator) &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state.setGeyserOpen(unit->getTilePosition());
			game_state.addGas(-1);
		}
		else if (unit->getType() == UnitTypes::Terran_Academy &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state.toggleAcademy();
		}
		else if (unit->getType() == UnitTypes::Zerg_Zergling &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state.addSupplyUsed(-.5);
		}
		else if (unit->getType() == UnitTypes::Zerg_Hydralisk &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state.addSupplyUsed(-1);
		}
		else if (unit->getType() == UnitTypes::Protoss_Observatory &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state.addObservatory(-1);
		}
		else if (unit->getType() == UnitTypes::Protoss_Robotics_Facility &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state.addRoboticsFacility(-1);
		}
		else if (unit->getType() == UnitTypes::Protoss_Citadel_of_Adun &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state.addCitadelofAdun(-1);
		}
		else if (unit->getType() == UnitTypes::Protoss_Cybernetics_Core &&
			unit->getPlayer() == Broodwar->self() &&
			game_state.checkCyberCore())
		{
			game_state.toggleCyberCore();
		}
	}
	catch (const std::exception & e)
	{
		Broodwar << "EXCEPTION: " << e.what() << std::endl;
	}
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		// if we are in a replay, then we will print out the build order of the structures
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		{
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
	}
	else
	{
		if (unit->getType() == BWAPI::UnitTypes::Zerg_Drone &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_worker(unit, game_state.getContainingBase(unit));
			game_state.addMineralWorker(new_worker);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_detector(unit, game_state.getContainingBase(unit));
			game_state.addDetector(new_detector);
			game_state.addSupplyTotal(8);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state.getContainingBase(unit));
			game_state.addBuilding(new_building);
			game_state.addSupplyTotal(1);
			game_state.addMineralsCommitted(-300);
			game_state.addSupplyUsed(-1);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state.getContainingBase(unit));
			game_state.addBuilding(new_building);
			game_state.addMineralsCommitted(-200);
			game_state.addSupplyUsed(-1);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state.getContainingBase(unit));
			game_state.addBuilding(new_building);
			game_state.addMineralsCommitted(-75);
			game_state.addSupplyUsed(-1);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Zerg_Zergling &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_unit(unit, game_state.getContainingBase(unit));
			game_state.addUnit(new_unit);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Zerg_Creep_Colony &&
			unit->getPlayer() == Broodwar->self())
		{
			game_state.addMineralsCommitted(-75);
			Object new_building(unit, game_state.getContainingBase(unit));
			game_state.addBuilding(new_building);
			game_state.addSupplyUsed(-1);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Zerg_Extractor &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state.getContainingBase(unit));
			game_state.addBuilding(new_building);
			game_state.addMineralsCommitted(-50);
			game_state.setGeyserUsed(unit->getTilePosition());
			game_state.addSupplyUsed(-1);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_building(unit, game_state.getContainingBase(unit));
			game_state.addBuilding(new_building);
			game_state.addMineralsCommitted(-150);
			game_state.addGasCommitted(-50);
			game_state.addSupplyUsed(-1);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk &&
			unit->getPlayer() == Broodwar->self())
		{
			Object new_unit(unit, game_state.getContainingBase(unit));
			game_state.addUnit(new_unit);
		}
	}
}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit unit)
{
	if (unit->getType() == UnitTypes::Protoss_Assimilator &&
		unit->getPlayer() == Broodwar->self())
	{
		Object new_building(unit, game_state.getContainingBase(unit));
		game_state.addBuilding(new_building);
		game_state.setGeyserUsed(unit->getTilePosition());
	}
}

void ExampleAIModule::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void ExampleAIModule::onUnitComplete(BWAPI::Unit unit)
{
	if (unit->getType() == UnitTypes::Terran_Command_Center &&
		unit->getPlayer() == Broodwar->self())
	{
		Object new_building(unit, game_state.getContainingBase(unit));
		game_state.addBuilding(new_building);
		game_state.addSupplyTotal(10);
		if (Broodwar->elapsedTime() > 5)
		{
			int number_workers_at_main = 0;
			auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
			while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
			{
				if (mineral_worker_iterator->getBase()->getBaseClass() == 3)
					number_workers_at_main++;
				mineral_worker_iterator++;
			}
			int workers_moved = 0;
			int number_workers_to_move = floor(number_workers_at_main / 2);
			mineral_worker_iterator = game_state.getMineralWorkers()->begin();
			while (mineral_worker_iterator != game_state.getMineralWorkers()->end() &&
				workers_moved <= number_workers_to_move)
			{
				if (mineral_worker_iterator->getBase()->getBaseClass() == 3)
				{
					mineral_worker_iterator->setBase(new_building.getBase());
					workers_moved++;
				}
				mineral_worker_iterator++;
			}
		}
	}
	else if (unit->getType() == UnitTypes::Protoss_Nexus &&
		unit->getPlayer() == Broodwar->self())
	{
		Object new_building(unit, game_state.getContainingBase(unit));
		game_state.addBuilding(new_building);
		game_state.addSupplyTotal(9);
		if (Broodwar->elapsedTime() > 5)
		{
			int number_workers_at_main = 0;
			auto mineral_worker_iterator = game_state.getMineralWorkers()->begin();
			while (mineral_worker_iterator != game_state.getMineralWorkers()->end())
			{
				if (mineral_worker_iterator->getBase()->getBaseClass() == 3)
					number_workers_at_main++;
				mineral_worker_iterator++;
			}
			int workers_moved = 0;
			int number_workers_to_move = floor(number_workers_at_main / 2);
			mineral_worker_iterator = game_state.getMineralWorkers()->begin();
			while (mineral_worker_iterator != game_state.getMineralWorkers()->end() &&
				workers_moved <= number_workers_to_move)
			{
				if (mineral_worker_iterator->getBase()->getBaseClass() == 3)
				{
					mineral_worker_iterator->setBase(new_building.getBase());
					workers_moved++;
				}
				mineral_worker_iterator++;
			}
		}
	}
	else if ((unit->getType() == UnitTypes::Terran_Supply_Depot ||
		unit->getType() == UnitTypes::Protoss_Pylon) &&
		unit->getPlayer() == Broodwar->self())
	{
		game_state.addSupplyTotal(8);
	}
	else if (unit->getType() == UnitTypes::Zerg_Hatchery &&
		unit->getPlayer() == Broodwar->self())
	{
		game_state.addSupplyTotal(1);
	}
	else if (unit->getPlayer() == BWAPI::Broodwar->self() &&
		(unit->getType() == UnitTypes::Terran_SCV ||
		unit->getType() == UnitTypes::Protoss_Probe ||
		unit->getType() == UnitTypes::Zerg_Drone))
	{
		Object new_worker(unit, game_state.getContainingBase(unit));
		game_state.addMineralWorker(new_worker);
	}
	else if (unit->getPlayer() == BWAPI::Broodwar->self() &&
		(unit->getType() != UnitTypes::Protoss_Observer ||
		unit->getType() != UnitTypes::Zerg_Overlord) &&
		!unit->getType().isBuilding())
	{
		Object new_unit(unit);
		game_state.addUnit(new_unit);
	}
}
