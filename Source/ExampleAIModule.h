#pragma once
#include <BWAPI.h>
#include"../../BWEM-community/BWEM/include/bwem.h"
#include"WorkerManager.h"
#include<vector>
#include"AIBase.h"
#include"MacroManager.h"
#include"MilitaryManager.h"
#include"GameState.h"
#include"Objective.h"
#include"Resource.h"

#include "..\Frontend\BWAPIFrontendClient\ProtoClient.h"


// Remember not to use "Broodwar" in any global class constructor!

class NiteKat
{
public:
  NiteKat();
  void init();

  // functions for callbacks, leave these as they are.
  void onStart();
  void onEnd(bool isWinner);
  void onFrame();
  void onSendText(std::string text);
  void onReceiveText(BWAPI::Player player, std::string text);
  void onPlayerLeft(BWAPI::Player player);
  void onNukeDetect(BWAPI::Position target);
  void onUnitDiscover(BWAPI::Unit unit);
  void onUnitEvade(BWAPI::Unit unit);
  void onUnitShow(BWAPI::Unit unit);
  void onUnitHide(BWAPI::Unit unit);
  void onUnitCreate(BWAPI::Unit unit);
  void onUnitDestroy(BWAPI::Unit unit);
  void onUnitMorph(BWAPI::Unit unit);
  void onUnitRenegade(BWAPI::Unit unit);
  void onSaveGame(std::string gameName);
  void onUnitComplete(BWAPI::Unit unit);
  // Everything below this line is safe to modify.

  BWAPI::ProtoClient BWAPIClient;
  BWAPI::Game Broodwar;
  std::unique_ptr<GameState> game_state;
  std::unique_ptr <WorkerManager> worker_manager;
  std::unique_ptr <MacroManager> macro_manager;
  std::unique_ptr <MilitaryManager> military_manager;
  double assess_game_time;
  double manage_workers_time;
  double check_macro_time;
  double check_military_time;
  Neolib::FastAPproximation fap;
};


