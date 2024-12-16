#pragma once

#include <cstdint>

#include "../game/game.h"
#include "../game/shot.h"

enum Events {
  UKNOWN,
  PING,
  EXIT,
  CREATE,
  START,
  LOAD,
  SET_WIDTH_HEIGHT,
  SET_COUNT,
  GET_WIDTH_HEIGHT,
  SET_STRATEGY,
  GET_COUNT,
  SHOT_BOT,
  SHOT_PLAYER,
  SET_RESULT,
  STOP,
  DUMP,
  LOSE,
  WIN,
  FINISHED
};

class Commands {
public:
  Events GetEvent(std::string input);
  uint64_t GetNumValue1();
  uint64_t GetNumValue2();
  std::string GetStrValue();

private:
  uint64_t num_event_value1_;
  uint64_t num_event_value2_;
  std::string str_event_value_;
};

class CommandsAnalyzer {
public:
  CommandsAnalyzer();
  
  void AnalyzeEvent(std::string input);
  bool CheckEvent();
  bool LoadState(const std::string &path);
  bool SaveState(const std::string &path);

private:
  bool is_waiting_for_ans_;
  bool is_loaded_state_;
  Commands commands_;
  PlayerGame player_;
  ProgrammGame programm_;
};