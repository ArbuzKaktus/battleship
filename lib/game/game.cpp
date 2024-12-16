#include "game.h"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "field.h"
#include "ship_storage.h"
#include "ship.h"
#include "shot.h"
#include "strategy.h"

namespace {
  const uint16_t kCircle = 360;
  const uint16_t kOneRotate = 90;
  const size_t kRotateCount = 4;
  const size_t KUpDegree = 0;
  const size_t kRightDegree = 90;
  const size_t kDownDegree = 180;
  const size_t kLeftDegree = 270;
  const uint64_t kMaxFieldSize = 8000 * 8000;
}

uint64_t GetShipsInFileCount(const std::string &path) {
  uint64_t count = 0;
  std::ifstream file(path);
  std::string input;
  while (std::getline(file, input)) {
    ++count;
  }
  return count;
}

ShipPlaceState::ShipPlaceState(uint64_t size_x, uint64_t size_y, std::vector<uint64_t> ships_count)
        : count_ship_(ships_count), new_size_(0), x_(0), y_(0), min_x_(0), min_y_(0),
          max_x_(size_x - 1), max_y_(size_y - 1), degrees_rotate_(0) 
        {}

bool ShipPlaceState::MaKeStep() {
  switch (degrees_rotate_ % kCircle) {
  case KUpDegree:
  y_ += 2;
  if (y_ > max_y_) {
    degrees_rotate_ += kOneRotate;
    y_ = max_y_;
    x_ += 2;
    if (x_ > max_x_) {
    return false;
    }
    min_x_ = x_;
  }
  break;
  case kRightDegree:
  x_ += 2;
  if (x_ > max_x_) {
    degrees_rotate_ += kOneRotate;
    x_ = max_x_;
    if (y_ < min_y_ + 2) {
    return false;
    }
    y_ -= 2;
    max_y_ = y_;
  }
  break;
  case kDownDegree:
  if (y_ < min_y_ + 2) {
    degrees_rotate_ += kOneRotate;
    y_ = min_y_;
    if (x_ < min_x_ + 2) {
    return false;
    }
    x_ -= 2;
    max_x_ = x_;
  } else {
    y_ -= 2;
  }
  break;
  case kLeftDegree:
  if (x_ < min_x_ + 2) {
    degrees_rotate_ += kOneRotate;
    x_ = min_x_;
    y_ += 2;
    if (y_ > max_y_) {
    return false;
    }
    min_y_ = y_;
  } else {
    x_ -= 2;
  }
  break;
  }
  return true;
}

bool ShipPlaceState::AllShipsArePlaced() {
  for (uint64_t elem: count_ship_) {
  if (elem != 0) {
    return false;
  }
  }
  return true;
}

bool ShipPlaceState::IsAbroad() {
  return x_ < min_x_ || x_ > max_x_ || y_ < min_y_ || y_ > max_y_;
}

size_t FindShip(std::vector<size_t>& ship_counts, const size_t& window) {
  for (int16_t i = 3; i != -1; --i) {
    if (i + 1 <= window && ship_counts[i] != 0) {
      --ship_counts[i];
      return i + 1;
    }
  }
  return 0;
}

bool IsDeadShip(std::vector<std::vector<uint8_t>>& field, uint64_t x, uint64_t y) {
  uint64_t size_x = field.size();
  uint64_t size_y = field[0].size();

  auto isShipCell = [&](uint64_t x, uint64_t y) {
    return x < size_x && y < size_y && (field[x][y] == 1 || field[x][y] == 2);
  };

  uint64_t left = y, right = y;
  while (left > 0 && isShipCell(x, left - 1)) {
    left--;
  }
  while (right + 1 < size_y && isShipCell(x, right + 1)) {
    right++;
  }
  for (uint64_t c = left; c <= right; c++) {
    if (field[x][c] == 2) {
      return false;
    }
  }
  uint64_t up = x, down = x;
  while (up > 0 && isShipCell(up - 1, y)) {
    up--;
  }
  while (down + 1 < size_x && isShipCell(down + 1, y)) {
    down++;
  }
  for (uint64_t r = up; r <= down; r++) {
    if (field[r][y] == 2) {
      return false;
    }
  }
  return true;
}

SettingDevice::SettingDevice() : size_x_(0), size_y_(0) {
  ships_count_.resize(4, 0);
}

SettingDevice::~SettingDevice() {};

bool SettingDevice::IsSlave() { return is_slave_; }

bool Game::IsGameOn() { return is_game_on_; }

bool SettingDevice::SetRole(const std::string& role) {
  if (role == "slave") {
    is_slave_ = true;
  } else if (role == "master") {
    is_slave_ = false;
  } else {
    return false;
  }
  return true;
}

bool SettingDevice::SetWidth(const uint64_t& size_x) { 
  if (size_x == 0) return false;
  size_x_ = size_x;
  return true;
}

bool SettingDevice::SetHeight(const uint64_t& size_y) {
  if (size_y == 0) return false;
  size_y_ = size_y; 
  return true;
}

const uint64_t SettingDevice::GetWidth() { return size_x_; }

const uint64_t SettingDevice::GetHeight() { return size_y_; }

bool SettingDevice::CheckForStart() {
  if (size_x_ == 0 || size_y_ == 0) {
    return false;
  }
  for (uint64_t elem : ships_count_) {
    if (elem != 0) {
      return true;
    }
  }
  return false;
}

const uint64_t SettingDevice::GetShipsCount(size_t ship_size) {
  return ships_count_[ship_size - 1];
}

Game::Game() : is_game_on_(false) {}

Game::~Game() {}

bool Game::IsLose() { return !IsAnyAlive(); }

bool Game::IsWin() { return !IsLose(); }

bool Game::IsFinished() { return IsLose(); }

void Game::StartMyTurn() { is_my_turn_ = true; }

void Game::StopMyTurn() { is_my_turn_ = false; }

bool Game::IsMyTurn() { return is_my_turn_; }

void Game::StartGame() {
  is_game_on_ = true; 
  if (is_slave_) {
    is_my_turn_ = true;
  } else {
    is_my_turn_ = false;
  }
}

const uint64_t SettingDevice::GetAllShipsCount() {
  uint64_t count = 0;
  for (uint64_t elem : ships_count_) {
    count += elem;
  }
  return count;
}


ShipsConfig::ShipsConfig(uint64_t size_x, uint64_t size_y)
                        : size_x_(size_x), 
                          size_y_(size_y), 
                          is_check_(false)
                        { ships_count_.resize(4, 0); }

bool ShipsConfig::SetShipsCount(const uint64_t& ships_count,
                               const size_t& ship_size, ShipStorageBase* ships) {
  is_check_ = true;
  ships_count_[ship_size - 1] = ships_count;
  if (PlaceAllShips(ships)) {
    is_check_ = false;
    return true;
  }
  ships_count_[ship_size - 1] = 0;
  return false;
}

bool ShipsConfig::IsConfigSet() {
  if (size_x_ == 0 || size_y_ == 0) {
    return false;
  }
  for (uint64_t elem : ships_count_) {
    if (elem != 0) {
      return true;
    }
  }
  return false;
}

PlayerGame::PlayerGame() {
  is_slave_ = false;
  is_my_turn_ = false;
  config_ = nullptr;
  ships_ = new ShipsList;
}

void PlayerGame::StartGame() {
  if (size_x_ * size_y_ <= kMaxFieldSize || sizeof(Ship) * GetAllShipsCount()) {
    SetField(new Field(size_x_, size_y_));
  }
  is_game_on_ = true;
  if (is_slave_) {
    is_my_turn_ = true;
  } else {
    is_my_turn_ = false;
  }
}

bool PlayerGame::LoadConfig(const std::string& path) {
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
  SetHeight(size_y);
  SetWidth(size_x);
  SetConfig();
  if (size_x * size_y > kMaxFieldSize ||
      size_x * size_y < sizeof(Ship) * GetShipsInFileCount(path)) {
    SetField(new Field(size_x, size_y));
  }
  while (std::getline(file, input)) {
    stream.clear();
    stream.str(input);
    stream >> ship_size >> rotate >> x >> y;
    ++ship_count[ship_size - 1];
    config_->PlaceShip(x, y, ship_size, rotate, ships_);
  }
  for (size_t i = 0; i != 4; ++i) {
    SetShipsCount(ship_count[i], i + 1);
  }
  return true;
}

bool PlayerGame::IsAnyAlive() { 
  return ships_->IsAnyAlive();
}

void PlayerGame::SetField(ShipStorageBase* ships) {
  if (ships_) {
    delete ships_;
  }
  ships_ = ships;
}

ShotStates PlayerGame::Shot(const uint64_t& x, const uint64_t& y) {
  return ships_->Shot(x, y);
}

bool PlayerGame::SetShipsCount(const uint64_t& ships_count,
                               const size_t& ship_size) {
  if (!config_) {
    SetConfig();
  }
  return config_->SetShipsCount(ships_count, ship_size, ships_);
}

bool PlayerGame::CheckForStart() {
  return config_->IsConfigSet();
}

void PlayerGame::DeleteStates() {
  is_slave_ = false;
  is_my_turn_ = false;
  ships_count_.clear();
  ships_count_.resize(4);
  is_game_on_ = false;
  size_x_ = 0;
  size_y_ = 0;
  delete ships_;
  delete config_;
  ships_ = new ShipsList;
  config_ = nullptr;
}

bool ShipsConfig::MakeUpToRightRotate(ShipPlaceState& state, ShipStorageBase* ships) {
  size_t window = state.max_y_ - state.y_ + 1;
  size_t new_size = FindShip(state.count_ship_, window);
  if (new_size == 0) {
    state.y_ = state.max_y_;
    state.degrees_rotate_ += kOneRotate;
    state.min_x_ += 2;
    return true;
  }
  PlaceShip(state.x_, state.y_, new_size, state.degrees_rotate_, ships);
  state.y_ += 2;
  if (state.y_ > state.max_y_) {
    state.y_ = state.max_y_;
    state.x_ += 2;
    if (state.x_ > state.max_x_) {
      return false;
    }
    state.min_x_ += 2;
    state.degrees_rotate_ += kOneRotate;
  }
  return true;
}

bool ShipsConfig::MakeRightToDownRotate(ShipPlaceState& state, ShipStorageBase* ships) {
  size_t window = state.max_x_ - state.x_ + 1;
  size_t new_size = FindShip(state.count_ship_, window);
  if (new_size == 0) {
    state.x_ = state.max_x_;
    state.degrees_rotate_ += kOneRotate;
    state.max_y_ -= 2;
    return true;
  }
  PlaceShip(state.x_, state.y_, new_size, state.degrees_rotate_, ships);
  state.x_ += 2;
  if (state.x_ > state.max_x_) {
    state.x_ = state.max_x_;
    state.y_ -= 2;
    if (state.y_ < state.min_y_) {
      return false;
    }
    state.max_y_ -= 2;
    state.degrees_rotate_ += kOneRotate;
  }
  return true;
}

bool ShipsConfig::MakeDownToLeftRotate(ShipPlaceState& state, ShipStorageBase* ships) {
  size_t window = state.y_ - state.min_y_ + 1;
  size_t new_size = FindShip(state.count_ship_, window);
  if (new_size == 0) {
    state.y_ = state.min_y_;
    state.degrees_rotate_ += kOneRotate;
    state.max_x_ -= 2;
    return true;
  }
  PlaceShip(state.x_, state.y_, new_size, state.degrees_rotate_, ships);
  if (state.y_ < state.min_y_ + 2) {
    state.y_ = state.min_y_;
    if (state.x_ < state.min_x_ + 2) {
      return false;
    }
    state.x_ -= 2;
    state.max_x_ -= 2;
    state.degrees_rotate_ += kOneRotate;
  }
  return true;
}

bool ShipsConfig::MakeLeftToUpRotate(ShipPlaceState& state, ShipStorageBase* ships) {
  size_t window = state.x_ - state.min_x_ + 1;
  size_t new_size = FindShip(state.count_ship_, window);

  if (new_size == 0) {
    state.x_ = state.min_x_;
    state.degrees_rotate_ += kOneRotate;
    state.min_y_ += 2;
    return true;
  }
  PlaceShip(state.x_, state.y_, new_size, state.degrees_rotate_, ships);
  if (state.x_ < state.min_x_ + 2) {
    state.x_ = state.min_x_;
    if (state.y_ + 2 > state.max_y_) {
      return false;
    }
    state.y_ += 2;
    state.min_y_ += 2;
    state.degrees_rotate_ += kOneRotate;
  }
  return true;
}

bool ShipsConfig::CirclePlace(ShipPlaceState& state, ShipStorageBase* ships) {
  for (size_t ship_size = 4; ship_size != 0; --ship_size) {
    while (state.count_ship_[ship_size - 1] != 0) {
      if (state.y_ + ship_size - 1 > state.max_y_ && state.degrees_rotate_ % kCircle == 0) {
        if (!MakeUpToRightRotate(state, ships)) {
          return false;
        } 

      } else if (state.x_ + ship_size - 1 > state.max_x_ && state.degrees_rotate_ % kCircle == kRightDegree) {
        if (!MakeRightToDownRotate(state, ships)) {
          return false;
        }

      } else if (state.y_ + 1 < state.min_y_ + ship_size && state.degrees_rotate_ % kCircle == kDownDegree) {
        if (!MakeDownToLeftRotate(state, ships)) {
          return false;
        }

      } else if (state.x_ + 1 < state.min_x_ + ship_size && state.degrees_rotate_ % kCircle == kLeftDegree) {
        if (!MakeLeftToUpRotate(state, ships)) {
          return false;
        }

      } else {
        PlaceShip(state.x_, state.y_, ship_size, state.degrees_rotate_, ships);
        --state.count_ship_[ship_size - 1];
        if (!state.MaKeStep()) {
          return state.AllShipsArePlaced();
        }
      }
    }
  }
  return state.AllShipsArePlaced();
}

bool ShipsConfig::ColumnRowPlace(bool is_row, ShipPlaceState& state, ShipStorageBase* ships) {
  uint64_t size_x = size_x_, size_y = size_y_;
  if (is_row) {
    size_x = size_y_;
    size_y = size_x_;
    state.degrees_rotate_ = kRightDegree;
  }
  for (size_t x = 0; x < size_x; x += 2) {
    for (size_t y = 0 ; y < size_y; y += 2) {
      state.window_ = size_y - y;
      state.new_size_ = FindShip(state.count_ship_, state.window_);
      if (state.new_size_ == 0) {
        break;
      }
      if (is_row) {
        PlaceShip(y, x, state.new_size_, state.degrees_rotate_, ships);
      }
      else {
        PlaceShip(x, y, state.new_size_, state.degrees_rotate_, ships);
      }
    }
  }
  return state.AllShipsArePlaced();
}

bool ShipsConfig::PlaceAllShips(ShipStorageBase* ships) {
  ShipPlaceState state(size_x_, size_y_, ships_count_);
  if (CirclePlace(state, ships)) {
    if (is_check_) {
      return true;
    }
    return CirclePlace(state, ships);
  }
  state = {size_x_, size_y_, ships_count_};
  if (ColumnRowPlace(true, state,ships)) {
    if (is_check_) {
      return true;
    }
    return ColumnRowPlace(true, state, ships);
  }
  state = {size_x_, size_y_, ships_count_};
  if (ColumnRowPlace(false, state, ships)) {
    if (is_check_) {
      return true;
    }
    return ColumnRowPlace(false, state, ships);
  }
  return false;
}

void ShipsConfig::PlaceShip(uint64_t x, uint64_t y, const size_t& ship_size,
                           char rotate, ShipStorageBase* ships) {
  uint64_t degrees_rotate;
  rotate == 'v' ? degrees_rotate = 0 : degrees_rotate = kRightDegree;
  ships->PlaceShip(x, y, ship_size, rotate, is_check_);
}

void ShipsConfig::PlaceShip(uint64_t& x, uint64_t& y, const size_t& ship_size, 
                            const uint16_t& rotate, ShipStorageBase* ships) {
  ships->PlaceShip(x, y, ship_size, rotate, is_check_);
}

void PlayerGame::DumpState(std::ofstream& file) {
  char rotate;
  size_t size;
  uint64_t x;
  uint64_t y;
  file << size_x_ << " " << size_y_ << '\n';
  ships_->DumpState(file);
}

PlayerGame::~PlayerGame() {
  delete config_;
  delete ships_;
}

void PlayerGame::SetConfig() {
  if (config_) {
    delete config_;
  }
  config_ = new ShipsConfig(size_x_, size_y_);
}

void PlayerGame::PlaceAllShips() {
  config_->PlaceAllShips(ships_);
}

ProgrammGame::ProgrammGame() : is_ordered_strategy_(false), live_ships_count_(0) {
  is_slave_ = true;
  is_my_turn_ = true;
}

bool ProgrammGame::SetShipsCount(const uint64_t& ships_count,
                                 const size_t& ship_size) {
  ships_count_[ship_size - 1] = ships_count;
  return true;
}

void ProgrammGame::SetRules() {
  SetHeight(9);
  SetWidth(11);
  SetShipsCount(2, 2);
  SetShipsCount(3, 3);
  SetShipsCount(8, 4);
}

void ProgrammGame::SetStrategy(const std::string& strategy) {

  if (strategy == "ordered") {
    Strategy* temp = new OrderedStrategy();

  }

}

bool ProgrammGame::LoadConfig(const std::string& path) {
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
  SetHeight(size_y);
  SetWidth(size_x);
  while (std::getline(file, input)) {
    stream.clear();
    stream.str(input);
    stream >> ship_size >> rotate >> x >> y;
    ++ship_count[ship_size - 1];
  }
  for (size_t i = 0; i != 4; ++i) {
    SetShipsCount(ship_count[i], i + 1);
  }
  return true;
}

void ProgrammGame::UpdateFieldState(const ShotStates& shot_state) {
  if (shot_state == ShotStates::KILL) {
    --live_ships_count_;
    chess_strategy_.MakeOreal(size_x_, size_y_);
  }
  else if (shot_state == ShotStates::MISS) {
    StopMyTurn();
  }
  else if (shot_state == ShotStates::UNKNOWN) {
    if (is_ordered_strategy_) {
      ordered_strategy_.ResetShot();
    } else {
      chess_strategy_.ResetShot();
    }
  }
}

void ProgrammGame::DeleteStates() {
  ordered_strategy_ = OrderedStrategy();
  chess_strategy_ = ChessStrategy();
  is_ordered_strategy_ = false;
  is_slave_ = true;
  is_my_turn_ = true;
  ships_count_.clear();
  ships_count_.resize(4);
  live_ships_count_ = 0;
  is_game_on_ = false;
  size_x_ = 0;
  size_y_ = 0;
}

std::pair<uint64_t, uint64_t> ProgrammGame::Shot() {
  std::pair<uint64_t, uint64_t> cord;
  if (is_ordered_strategy_) {
    return ordered_strategy_.Shot(size_x_, size_y_);
  }
  return chess_strategy_.Shot(size_x_, size_y_);
}

bool ProgrammGame::IsAnyAlive() { return live_ships_count_ != 0; }