#include "game.h"
#include "shot.h"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {
  const uint16_t kCircle = 360;
  const uint16_t kOneRotate = 90;
  const size_t kRotateCount = 4;
  const size_t KUpDegree = 0;
  const size_t kRightDegree = 90;
  const size_t kDownDegree = 180;
  const size_t kLeftDegree = 270;
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

bool AllShipsArePlaced(const std::vector<uint64_t>& count_ship) {
  for (const uint64_t& elem : count_ship) {
    if (elem != 0) {
      return false;
    }
  }
  return true;
}

SettingDevice::SettingDevice() : size_x_(0), size_y_(0) {
  ships_count_.resize(4, 0);
}

SettingDevice::~SettingDevice() {};

void SettingDevice::DeleteStates() {}

bool SettingDevice::SetShipsCount(const uint64_t& ships_count,
                                  const size_t& ship_size) {
  return true;
}

bool SettingDevice::IsSlave() { return is_slave_; }

bool Game::IsGameOn() { return is_game_on_; }

void SettingDevice::SetRole(const std::string& role) {
  if (role == "slave") {
    is_slave_ = true;
  } else {
    is_slave_ = false;
  }
}

void SettingDevice::SetWidht(const uint64_t& size_x) { size_x_ = size_x; }

void SettingDevice::SetHeight(const uint64_t& size_y) { size_y_ = size_y; }

uint64_t SettingDevice::GetWidth() { return size_x_; }

uint64_t SettingDevice::GetHeight() { return size_y_; }

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

uint64_t SettingDevice::GetShipsCount(size_t ship_size) {
  return ships_count_[ship_size - 1];
}

Game::Game() : is_game_on_(false){}

Game::~Game() {}

bool Game::IsLose() { return !IsAnyAlive(); }

bool Game::IsWin() { return !IsLose(); }

bool Game::IsFinished() { return IsLose(); }

void Game::StartMyTurn() { is_my_turn_ = true; }

void Game::StopMyTurn() { is_my_turn_ = false; }

bool Game::IsMyTurn() { return is_my_turn_; }

void Game::StartGame() { is_game_on_ = true; }

uint64_t Game::GetAllShipsCount() {
  uint64_t count = 0;
  for (uint64_t elem : ships_count_) {
    count += elem;
  }
  return count;
}

PlayerGame::PlayerGame() : is_field_(false) {
  is_slave_ = false;
  is_my_turn_ = false;
}

bool PlayerGame::IsAnyAlive() { 
  if (is_field_) {
    return field_.IsAnyAlive();
  }
  return ships_.IsAnyAlive();
}

void PlayerGame::CreateField() {
  is_field_ = true;
  field_.CreateField(size_x_, size_y_);
}

bool PlayerGame::IsPosibleToPlace(const uint64_t& ship_count_to_place,
                                  const size_t& ship_size_to_place) {
  ships_count_[ship_size_to_place - 1] = ship_count_to_place;
  if (PlaceAllShips(true)) {
    return true;
  }
  ships_count_[ship_size_to_place - 1] = 0;
  return false;
}

bool PlayerGame::SetShipsCount(const uint64_t& ships_count,
                               const size_t& ship_size) {
  if (!IsPosibleToPlace(ships_count, ship_size)) {
    return false;
  }
  return true;
}

ShotStates PlayerGame::Shot(const uint64_t& x, const uint64_t& y) {
  if (!is_field_) {
    return ships_.Shot(x, y);
  }
  return field_.Shot(x, y);
}

void PlayerGame::DeleteStates() {
  is_slave_ = false;
  is_my_turn_ = false;
  ships_count_.clear();
  ships_count_.resize(4);
  is_game_on_ = false;
  size_x_ = 0;
  size_y_ = 0;
  field_.DeleteField();
  ships_.DeleteShips();
}

void PlayerGame::PlaceShip(uint64_t& x, uint64_t& y, const size_t& ship_size,
                           const uint16_t& rotation, bool is_check) {
  if (!is_field_) {
    ships_.PlaceShip(x, y, ship_size, rotation, is_check);
    return;
  }
  field_.PlaceShip(x, y, ship_size, rotation, is_check);
}

bool PlayerGame::MakeUpToRightRotate(uint64_t& x, uint64_t& y, uint64_t& max_y,
                                     uint64_t& min_x, const uint64_t max_x,
                                     uint16_t& degrees_rotate,
                                     std::vector<uint64_t>& count_ship,
                                     bool is_check) {

  size_t window = max_y - y + 1;
  size_t new_size = FindShip(count_ship, window);
  if (new_size == 0) {
    y = max_y;
    degrees_rotate += kOneRotate;
    min_x += 2;
    return true;
  }
  PlaceShip(x, y, new_size, degrees_rotate, is_check);
  y += 2;
  if (y > max_y) {
    y = max_y;
    x += 2;
    if (x > max_x) {
      return false;
      ;
    }
    min_x += 2;
    degrees_rotate += kOneRotate;
  }
  return true;
}

bool PlayerGame::MakeRightToDownRotate(uint64_t& x, uint64_t& y,
                                       uint64_t& max_y, uint64_t& max_x,
                                       const uint64_t min_y, uint16_t& rotate,
                                       std::vector<uint64_t>& count_ship,
                                       bool is_check) {

  size_t window = max_x - x + 1;
  size_t new_size = FindShip(count_ship, window);
  if (new_size == 0) {
    x = max_x;
    rotate += kOneRotate;
    max_y -= 2;
    return true;
    ;
  }
  PlaceShip(x, y, new_size, rotate, is_check);
  x += 2;
  if (x > max_x) {
    x = max_x;
    y -= 2;
    if (y < min_y) {
      return false;
    }
    max_y -= 2;
    rotate += kOneRotate;
  }
  return true;
}

bool PlayerGame::MakeDownToLeftRotate(uint64_t& x, uint64_t& y, uint64_t& min_y,
                                      uint64_t& max_x, const uint64_t min_x,
                                      uint16_t& rotate,
                                      std::vector<uint64_t>& count_ship,
                                      bool is_check) {

  size_t window = y - min_y + 1;
  size_t new_size = FindShip(count_ship, window);
  if (new_size == 0) {
    y = min_y;
    rotate += kOneRotate;
    max_x -= 2;
    return true;
  }
  PlaceShip(x, y, new_size, rotate, is_check);
  if (y < min_y + 2) {
    y = min_y;
    if (x < min_x + 2) {
      return false;
    }
    x -= 2;
    max_x -= 2;
    rotate += kOneRotate;
  }
  return true;
}

bool PlayerGame::MakeLeftToUpRotate(uint64_t& x, uint64_t& y, uint64_t& min_y,
                                    uint64_t& min_x, const uint64_t max_y,
                                    uint16_t& rotate,
                                    std::vector<uint64_t>& count_ship,
                                    bool is_check) {

  size_t window = x - min_x + 1;
  size_t new_size = FindShip(count_ship, window);

  if (new_size == 0) {
    x = min_x;
    rotate += kOneRotate;
    min_y += 2;
    return 2;
    ;
  }
  PlaceShip(x, y, new_size, rotate, is_check);
  if (x < min_x + 2) {
    x = min_x;
    if (y + 2 > max_y) {
      return 1;
    }
    y += 2;
    min_y += 2;
    rotate += kOneRotate;
  }
  return 0;
}
bool PlayerGame::CirclePlace(bool is_check) {
  size_t window, new_size = 0;
  std::vector<uint64_t> count_ship = ships_count_;
  uint64_t x = 0, y = 0, min_x = 0, min_y = 0;
  uint64_t max_x = size_x_ - 1, max_y = size_y_ - 1;
  uint16_t degrees_rotate = 0;

  for (size_t ship_size = 4; ship_size != 0; --ship_size) {
    while (count_ship[ship_size - 1] != 0) {
      if (y + ship_size - 1 > max_y && degrees_rotate % kCircle == 0) {
        if (!MakeUpToRightRotate(x, y, max_y, min_x, max_x, degrees_rotate,
                                 count_ship, is_check)) {
          return false;
        }

      } else if (x + ship_size - 1 > max_x && degrees_rotate % kCircle == kRightDegree) {
        if (!MakeRightToDownRotate(x, y, max_y, max_x, min_y, degrees_rotate,
                                   count_ship, is_check)) {
          return false;
        }

      } else if (y + 1 < min_y + ship_size && degrees_rotate % kCircle == kDownDegree) {
        if (!MakeDownToLeftRotate(x, y, min_y, max_x, min_x, degrees_rotate,
                                  count_ship, is_check)) {
          return false;
        }

      } else if (x + 1 < min_x + ship_size && degrees_rotate % kCircle == kLeftDegree) {
        if (!MakeLeftToUpRotate(x, y, min_y, min_x, max_y, degrees_rotate,
                                count_ship, is_check)) {
          return false;
        }

      } else {
        if (x < min_x || x > max_x || y < min_y || y > max_y) {
          return false;
        }
        PlaceShip(x, y, ship_size, degrees_rotate, is_check);
        --count_ship[ship_size - 1];
        if (AllShipsArePlaced(count_ship)) {
          return true;
        }
        switch (degrees_rotate % kCircle) {
          case KUpDegree:
            y += 2;
            if (y > max_y) {
              y = max_y;
              x += 2;
              if (x > max_x) {
                return false;
              }
              min_x = x;
            }
            break;
          case kRightDegree:
            x += 2;
            if (x > max_x) {
              x = max_x;
              if (y < min_y + 2) {
                return false;
              }
              y -= 2;
              max_y = y;
            }
            break;
          case kDownDegree:
            if (y < min_y + 2) {
              y = min_y;
              if (x < min_x + 2) {
                return false;
              }
              x -= 2;
              max_x = x;
            } else {
              y -= 2;
            }
            break;
          case kLeftDegree:
            if (x < min_x + 2) {
              x = min_x;
              y += 2;
              if (y > max_y) {
                return false;
              }
              min_y = y;
            } else {
              x -= 2;
            }
            break;
        }
        degrees_rotate += kOneRotate;
      }
    }
  }
  return true;
}

bool PlayerGame::ColumnRowPlace(bool is_check, bool is_row) {
  size_t window, size_x = size_x_, size_y = size_y_, new_size = 0, rotate = 0;
  if (is_row) {
    size_x = size_y_;
    size_y = size_x_;
    rotate = kRightDegree;
  }
  std::vector<uint64_t> count_ship = ships_count_;
  for (size_t x = 0; x < size_x; x += 2) {
    for (size_t y = 0 ; y < size_y; ++y) {
      window = size_y_ - y;
      new_size = FindShip(count_ship, window);
      if (new_size == 0) {
        break;
      }
      if (is_row) {
        PlaceShip(y, x, new_size, rotate, is_check);
      }
      else {
        PlaceShip(x, y, new_size, rotate, is_check);
      }
      y += 2;
    }
  }
  return AllShipsArePlaced(count_ship);
}

bool PlayerGame::PlaceAllShips(bool is_check) {
  if (CirclePlace(true)) {
    if (is_check) {
      return true;
    }
    return CirclePlace(false);
  }
  if (ColumnRowPlace(true, true)) {
    if (is_check) {
      return true;
    }
    return ColumnRowPlace(false,true);
  }
  if (ColumnRowPlace(true, false)) {
    if (is_check) {
      return true;
    }
    return ColumnRowPlace(false,false);
  }
  return false;
}

void PlayerGame::DumpState(std::ofstream& file) {
  char rotate;
  size_t size;
  uint64_t x;
  uint64_t y;
  file << size_x_ << " " << size_y_ << '\n';
  if (is_field_) {
    field_.DumpState(file);
    return;
  }
  ships_.DumpState(file);
}

void PlayerGame::PlaceShip(uint64_t x, uint64_t y, const size_t& ship_size,
                           char rotate) {
  uint64_t degrees_rotate;
  rotate == 'v' ? degrees_rotate = 0 : degrees_rotate = kRightDegree;
  if (is_field_) {
    field_.PlaceShip(x, y, ship_size,  degrees_rotate, false);
  }
  ships_.PlaceShip(x, y, ship_size,  degrees_rotate, false);
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
  SetHeight(6);
  SetWidht(6);
  SetShipsCount(1, 1);
  SetShipsCount(2, 2);
  SetShipsCount(1, 3);
  SetShipsCount(2, 4);
}

void ProgrammGame::SetStrategy(const std::string& strategy) {
  strategy == "ordered" ? is_ordered_strategy_ = true
                        : is_ordered_strategy_ = false;
}

void ProgrammGame::UpdateFieldState(const ShotStates& shot_state) {
  if (shot_state == ShotStates::KILL) {
    --live_ships_count_;
    chess_strategy_.MakeOreal(size_x_, size_y_);
  }
  else if (shot_state == ShotStates::MISS) {
    StopMyTurn();
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