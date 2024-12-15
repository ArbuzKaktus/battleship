#include "out_input_stream.h"
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


const uint64_t kMaxFieldSize = 8000 * 8000; // ~61 Mb

uint64_t Commands::GetNumValue1() { return num_event_value1_; }

uint64_t Commands::GetNumValue2() { return num_event_value2_; }

std::string Commands::GetStrValue() { return str_event_value_; }

Events Commands::GetEvent(std::string input) {
  std::istringstream stream(input);
  std::string command;
  stream >> command;
  if (command == "ping") {
    return Events::PING;
  } else if (command == "exit") {
    std::cout << "ok" << std::endl;
    exit(EXIT_SUCCESS);
  } else if (command == "create") {
    std::string mode;
    stream >> str_event_value_;
    return Events::CREATE;
  }
  if (command == "start") {
    return Events::START;
  }
  if (command == "stop") {
    return Events::STOP;
  }
  if (command == "set") {
    std::string subCommand;
    stream >> subCommand;
    if (subCommand == "width" || subCommand == "height") {
      str_event_value_ = subCommand;
      stream >> num_event_value1_;
      return Events::SET_WIDTH_HEIGHT;
    }
    if (subCommand == "count") {
      stream >> num_event_value1_ >> num_event_value2_;
      return Events::SET_COUNT;
    }
    if (subCommand == "strategy") {
      stream >> str_event_value_;
      return Events::SET_STRATEGY;
    }
    if (subCommand == "result") {
      stream >> str_event_value_;
      return Events::SET_RESULT;
    }
  }

  if (command == "get") {
    stream >> str_event_value_;
    if (str_event_value_ == "count") {
      stream >> num_event_value1_;
      return Events::GET_COUNT;
    }
    return Events::GET_WIDTH_HEIGHT;
  }
  if (command == "shot") {
    if (input.size() == 4) {
      return Events::SHOT_BOT;
    }
    stream >> num_event_value1_ >> num_event_value2_;
    return Events::SHOT_PLAYER;
  }
  if (command == "dump") {
    stream >> str_event_value_;
    return Events::DUMP;
  }
  if (command == "load") {
    stream >> str_event_value_;
    return Events::LOAD;
  }
  if (command == "lose") {
    return Events::LOSE;
  }
  if (command == "win") {
    return Events::WIN;
  }
  if (command == "finished") {
    return Events::FINISHED;
  }
  std::cout << "unknown command" << std::endl;
  return Events::UKNOWN;
}

CommandsAnalyzer::CommandsAnalyzer() : is_loaded_state_(false) {}

uint64_t GetShipsInFileCount(const std::string &path) {
  uint64_t count = 0;
  std::ifstream file(path);
  std::string input;
  while (std::getline(file, input)) {
    ++count;
  }
  return count;
}

bool CommandsAnalyzer::LoadState(const std::string &path) {
  std::vector<uint64_t> ship_count(4);
  uint64_t size_x, size_y;
  size_t ship_size;
  char rotate;
  uint64_t x, y;
  std::ifstream file(path);
  std::istringstream stream;
  std::string input;
  if (!file.is_open()) {
    std::cout << "Error while reading file. Data is not load" << std::endl;
    return false;
  }
  std::getline(file, input);
  stream.str(input);
  stream >> size_x >> size_y;
  player_.SetHeight(size_y);
  player_.SetWidht(size_x);
  programm_.SetHeight(size_y);
  programm_.SetWidht(size_y);
  if (size_x * size_y > kMaxFieldSize ||
      size_x * size_y < sizeof(Ship) * GetShipsInFileCount(path)) {
    player_.CreateField();
  }
  while (std::getline(file, input)) {
    stream.clear();
    stream.str(input);
    stream >> ship_size >> rotate >> x >> y;
    ++ship_count[ship_size - 1];
    player_.PlaceShip(x, y, ship_size, rotate);
  }
  for (size_t i = 0; i != 4; ++i) {
    programm_.SetShipsCount(ship_count[i], i + 1);
    player_.SetShipsCount(ship_count[i], i + 1);
  }
  is_loaded_state_ = true;
  return true;
}

bool CommandsAnalyzer::SaveState(const std::string &path) {
  std::ofstream file(path);
  player_.DumpState(file);
  return true;
}

void CommandsAnalyzer::AnalyzeEvent(std::string input) {
  Events event = commands_.GetEvent(input);
  if (event >= Events::CREATE  && event <= Events::GET_COUNT && programm_.IsGameOn()) {
    std::cout << "failed" << std::endl;
    return;
  }
  if (event >= Events::SHOT_BOT && event <= Events::FINISHED && !programm_.IsGameOn()) {
    std::cout << "failed" << std::endl;
    return;
  }
  switch (event) {
    case Events::PING: {
      std::cout << "pong" << std::endl;
      break;
    }

    case Events::CREATE: {
      programm_.SetRole(commands_.GetStrValue());
      if (commands_.GetStrValue() == "master") {
        player_.SetRole("slave");
        player_.StartMyTurn();
        programm_.SetRules();
      } else {
        player_.SetRole("master");
        programm_.StartMyTurn();
      }
      std::cout << "ok" << std::endl;
      break;
    }

    case Events::START: {
      if ((programm_.CheckForStart() && player_.CheckForStart())) {
        programm_.StartGame();
        player_.StartGame();
        if (player_.GetHeight() * player_.GetWidth() < kMaxFieldSize ||
            player_.GetHeight() * player_.GetWidth() <
                sizeof(Ship) * programm_.GetAllShipsCount()) {
          player_.CreateField();
        }
        if (!is_loaded_state_) {
          player_.PlaceAllShips(false);
        }
        std::cout << "ok" << std::endl;
      } else {
        std::cout << "failed" << std::endl;
      }
      break;
    }

    case Events::LOAD: {
      if (LoadState(commands_.GetStrValue())) {
        std::cout << "ok" << std::endl;
      } else {
        std::cout << "failed" << std::endl;
      }
      break;
    }

    case Events::SET_WIDTH_HEIGHT: {
      if (player_.IsSlave()) {
        std::cout << "failed" << std::endl;
        break;
      }
      if (commands_.GetNumValue1() == 0) {
        std::cout << "failed" << std::endl;
        break;
      }
      if (commands_.GetStrValue() == "width") {
        programm_.SetWidht(commands_.GetNumValue1());
        player_.SetWidht(commands_.GetNumValue1());
      } else {
        programm_.SetHeight(commands_.GetNumValue1());
        player_.SetHeight(commands_.GetNumValue1());
      }
      std::cout << "ok" << std::endl;
      break;
    }

    case Events::SET_COUNT: {
      if (player_.IsSlave()) {
        std::cout << "failed" << std::endl;
        break;
      }
      if (programm_.GetHeight() == 0 || programm_.GetWidth() == 0) {
        std::cout << "failed" << std::endl;
        break;
      }
      if (player_.SetShipsCount(commands_.GetNumValue2(), commands_.GetNumValue1())) {
        programm_.SetShipsCount(commands_.GetNumValue2(), commands_.GetNumValue1());
        std::cout << "ok" << std::endl;
      } else {
        std::cout << "failed" << std::endl;
      }
      break;
    }

    case Events::GET_WIDTH_HEIGHT: {
      if (programm_.IsSlave()) {
        std::cout << "failed" << std::endl;
        break;
      }
      if (commands_.GetStrValue() == "width") {
        player_.SetWidht(programm_.GetWidth());
        std::cout << programm_.GetWidth() << std::endl;
      } else {
        player_.SetHeight(programm_.GetHeight());
        std::cout << programm_.GetHeight() << std::endl;
      }
      break;
    }

    case Events::SET_STRATEGY: {
      programm_.SetStrategy(commands_.GetStrValue());
      std::cout << "ok" << std::endl;
      break;
    }

    case Events::GET_COUNT: {
      if (!player_.IsSlave()) {
        std::cout << "failed" << std::endl;
        break;
      }
      size_t ship_size = commands_.GetNumValue1();
      if (ship_size > 4) {
        std::cout << "failed" << std::endl;
        break;
      } else if (ship_size == 0) {
        std::cout << "failed" << std::endl;
        break;
      }
      player_.SetShipsCount(programm_.GetShipsCount(ship_size), ship_size);
      std::cout << programm_.GetShipsCount(ship_size) << std::endl;
      break;
    }

    case Events::SHOT_BOT: {
      if (player_.IsMyTurn()) {
        std::cout << "failed" << std::endl;
        break;
      }
      std::pair<uint64_t, uint64_t> shot = programm_.Shot();
      std::cout << shot.first << " " << shot.second << std::endl;
      std::getline(std::cin, input);
      if (commands_.GetEvent(input) != Events::SET_RESULT) {
        std::cout << "failed" << std::endl;
        break;
      }
      if (commands_.GetStrValue() == "miss") {
        player_.StartMyTurn();
        programm_.UpdateFieldState(ShotStates::MISS);
      } else if (commands_.GetStrValue() == "hit") {
        programm_.UpdateFieldState(ShotStates::HIT);
      } else if (commands_.GetStrValue() == "kill") {
        programm_.UpdateFieldState(ShotStates::KILL);
      } else {
        std::cout << "faild" << std::endl;
        break;
      }
      std::cout << "ok" << std::endl;
      break;
    }

    case Events::SHOT_PLAYER: {
      if (!player_.IsMyTurn()) {
        std::cout << "failed" << std::endl;
        break;
      }
      ShotStates state = player_.Shot(commands_.GetNumValue1(), commands_.GetNumValue2());
      switch (state) {
        case ShotStates::MISS: {
          std::cout << "miss";
          programm_.StartMyTurn();
          player_.StopMyTurn();
          break;
        }
        case ShotStates::HIT: {
          std::cout << "hit";
          break;
        }
        case ShotStates::KILL: {
          std::cout << "kill";
          break;
        }
      }
      std::cout << std::endl;
      break;
    }

    case Events::STOP: {
      programm_.DeleteStates();
      player_.DeleteStates();
      std::cout << "ok" << std::endl;
      break;
    }

    case Events::DUMP: {
      SaveState(commands_.GetStrValue());
      std::cout << "ok" << std::endl;
      break;
    }

    case Events::WIN: {
      if (programm_.IsWin()) {
        std::cout << "yes";
      } else {
        std::cout << "no";
      }
      std::cout << std::endl;
      break;
    }

    case Events::LOSE: {
      if (player_.IsWin()) {
        std::cout << "yes";
      } else {
        std::cout << "no";
      }
      std::cout << std::endl;
      break;
    }

    case Events::FINISHED: {
      if (player_.IsFinished() || programm_.IsFinished()) {
        std::cout << "yes";
      } else {
        std::cout << "no";
      }
      std::cout << std::endl;
      break;
    }
    default: {
      std::cout << "failed" << std::endl;
      break;
    }
  }
}