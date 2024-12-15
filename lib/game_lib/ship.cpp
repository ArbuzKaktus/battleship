#include "ship.h"
#include <cstddef>
#include <cstdint>

namespace {
  const uint16_t kCircle = 360;
  const uint16_t kOneRotate = 90;
  const size_t kRotateCount = 4;
}

size_t Ship::GetShipSize() {
  std::bitset<8> mask("00001110");
  return ((states_ & mask) >> 1).to_ulong();
}

size_t Ship::GetRotate() {
  return (states_[0] * 90);
}

bool Ship::GetStateShot(const uint64_t& x, const uint64_t& y) { 
  std::bitset<8> mask("11110000");
  size_t num = (x - x_) + (y - y_);
  return ((states_ & mask) >> 4)[num];
}

std::pair<uint64_t, uint64_t> Ship::GetCoordinate() {
  return std::make_pair(x_, y_);
}

void Ship::PlaceShip(uint64_t& x, uint64_t& y, const uint16_t& rotate_degrees, const size_t& ship_size) {
  int16_t x_k[kRotateCount] = {0, 1, 0, -1};
  int16_t y_k[kRotateCount] = {1, 0, -1, 0};
  std::bitset<8> state("00000000");
  for (size_t i = 0; i < ship_size; ++i) {
    state[4 + i] = 1;
  }
  std::bitset<8> size(ship_size << 1);
  state = state | size;
  if (rotate_degrees % 180 == 0) {
    state[0] = 0;
  }
  else {
    state[0] = 1;
  }
  if (x_k[(rotate_degrees / kOneRotate) % kRotateCount] > 0 ||
      y_k[(rotate_degrees / kOneRotate) % kRotateCount] > 0) {
    x_ = x;
    y_ = y;
  }
  for (size_t i = 0; i < ship_size - 1; ++i) {
    x += x_k[(rotate_degrees / kOneRotate) % kRotateCount];
    y += y_k[(rotate_degrees / kOneRotate) % kRotateCount];
  }
  if (x_k[(rotate_degrees / kOneRotate) % kRotateCount] < 0 ||
      y_k[(rotate_degrees / kOneRotate) % kRotateCount] < 0) {
    x_ = x;
    y_ = y;
  }
  states_ = state;
}

size_t Ship::GetHp() {
  size_t hp = 0;
  std::bitset<8> states = states_ >> 4;
  for (size_t i = 0; i != 4; ++i) {
    if (states[i]) {
      ++hp;
    }
  }
  return hp;
}

bool Ship::IsHit(const uint64_t& x, const uint64_t& y) {
  if (x == x_ && y == y_) {
    states_[4] = 0;
    return true;
  }
  if (GetRotate() == 0) {
    if (x == x_ && y_ <= y && y < y_ + GetShipSize()) {
      states_[4 + y - y_] = 0;
      return true;
    }
  } else {
    if (y == y_ && x_ <= x && x < x_ + GetShipSize()) {
      states_[4 + x - x_] = 0;
      return true;
    }
  }
  return false;
}

void ShipsList::DeleteShips() {
  ships_.clear();
}

void ShipsList::PlaceShip(uint64_t& x, uint64_t& y, const size_t& ship_size, size_t rotation, bool is_check) {
  Ship ship_to_place;
  ship_to_place.PlaceShip(x, y, rotation, ship_size);
  if (!is_check) {
    ships_.push_back(ship_to_place);
  }
}

void ShipsList::DumpState(std::ofstream& file) {
  char rotate;
  size_t size;
  uint64_t x;
  uint64_t y;
  auto it = ships_.begin();
  std::pair<uint64_t, uint64_t> coordinate;
  for (; it != ships_.end(); ++it) {
    size = it->GetShipSize();
    rotate = it->GetRotate();
    coordinate = it->GetCoordinate();
    x = coordinate.first;
    y = coordinate.second;
    file << size << " " << rotate << " ";
    file << x << " " << y << "\n";
  }
}

ShotStates ShipsList::Shot(const uint64_t& x, const uint64_t& y) {
  auto it = ships_.begin();
  for (; it != ships_.end(); ++it) {
    if (it->IsHit(x, y)) {
      if (it->GetHp() == 0) {
        return ShotStates::KILL;
      }
      return ShotStates::HIT;
    }
  }
  return ShotStates::MISS;
}

bool ShipsList::IsAnyAlive() {
  auto it = ships_.begin();
  for (; it != ships_.end(); ++it) {
    if (it->GetHp() != 0) {
      return true;
    }
  }
  return false;
}