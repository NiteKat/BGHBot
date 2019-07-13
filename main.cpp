#include <BWAPI.h>
#include <iostream>

#include "Source\ExampleAIModule.h"


void reconnect(BWAPI::Client &BWAPIClient)
{
  while (!BWAPIClient.connect())
  {
    std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 });
  }
}

int main(int argc, const char* argv[])
{
  NiteKat bot;
  std::cout << "Connecting..." << std::endl;
  reconnect(bot.BWAPIClient);
  while (true)
  {
    std::cout << "waiting to enter match" << std::endl;
    while (!bot.Broodwar->isInGame())
    {
      while (!bot.BWAPIClient.isConnected())
      {
        std::cout << "Reconnecting..." << std::endl;
        reconnect(bot.BWAPIClient);
      }
      bot.BWAPIClient.update(bot.Broodwar);
    }
    bot.onStart();
    while (bot.Broodwar->isInGame())
    {
      for (auto &e : bot.Broodwar->getEvents())
      {
        switch (e.getType())
        {
        case BWAPI::EventType::MatchEnd:
          bot.onEnd(e.isWinner());
          break;
        case BWAPI::EventType::SendText:
          bot.onSendText(e.getText());
          break;
        case BWAPI::EventType::ReceiveText:
          bot.onReceiveText(e.getPlayer(), e.getText());
          break;
        case BWAPI::EventType::PlayerLeft:
          bot.onPlayerLeft(e.getPlayer());
          break;
        case BWAPI::EventType::NukeDetect:
          bot.onNukeDetect(e.getPosition());
          break;
        case BWAPI::EventType::UnitDiscover:
          bot.onUnitDiscover(e.getUnit());
          break;
        case BWAPI::EventType::UnitEvade:
          bot.onUnitEvade(e.getUnit());
          break;
        case BWAPI::EventType::UnitShow:
          bot.onUnitShow(e.getUnit());
          break;
        case BWAPI::EventType::UnitHide:
          bot.onUnitHide(e.getUnit());
          break;
        case BWAPI::EventType::UnitCreate:
          bot.onUnitCreate(e.getUnit());
          break;
        case BWAPI::EventType::UnitDestroy:
          bot.onUnitDestroy(e.getUnit());
          break;
        case BWAPI::EventType::UnitMorph:
          bot.onUnitMorph(e.getUnit());
          break;
        case BWAPI::EventType::UnitRenegade:
          bot.onUnitRenegade(e.getUnit());
          break;
        case BWAPI::EventType::SaveGame:
          bot.onSaveGame(e.getText());
          break;
        case BWAPI::EventType::UnitComplete:
          bot.onUnitComplete(e.getUnit());
          break;
        }
      }
      bot.onFrame();
      bot.BWAPIClient.update(bot.Broodwar);
    }
  }
  return 0;
}