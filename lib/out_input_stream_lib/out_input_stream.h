#pragma once

#include "../game_lib/game.h"
#include <cstdint>
#include "../game_lib/shot.h"

enum class Events {
  UKNOWN = 0,
  PING = 1,
  EXIT = 2,
  CREATE = 3,
  START = 4,
  LOAD = 5,
  SET_WIDTH_HEIGHT = 6,
  SET_COUNT = 7,
  GET_WIDTH_HEIGHT = 8,
  SET_STRATEGY = 9,
  GET_COUNT = 10,
  SHOT_BOT = 11,
  SHOT_PLAYER = 12,
  SET_RESULT = 13,
  STOP = 14,
  DUMP = 15,
  LOSE = 16,
  WIN = 17,
  FINISHED = 18
};

class Commands {
  public:
    uint64_t GetNumValue1();
    uint64_t GetNumValue2();
    Events GetEvent(std::string input);
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
    bool is_loaded_state_;
    Commands commands_;
    PlayerGame player_;
    ProgrammGame programm_;
};