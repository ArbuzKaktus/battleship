#include "field.h"
#include "shot.h"
#include <cstddef>
#include <fstream>

const size_t kRotateCount = 4;
const size_t kOneRotate = 90;

void Field::CreateField(uint64_t size_x, uint64_t size_y) {
  field_matrix_.resize(size_x);
  for (uint64_t i = 0; i != size_x; ++i) {
    field_matrix_[i].resize(size_y, 0);
  }
}

void Field::PlaceShip(uint64_t& x, uint64_t& y, const size_t& ship_size, size_t rotation, bool is_check) {
  int16_t x_k[kRotateCount] = {0, 1, 0, -1};
  int16_t y_k[kRotateCount] = {1, 0, -1, 0};
  if (!is_check) {
    field_matrix_[x][y] = 2;
  }
  for (size_t i = 0; i < ship_size - 1; ++i) {
    x += x_k[(rotation / kOneRotate) % kRotateCount];
    y += y_k[(rotation / kOneRotate) % kRotateCount];
    if (!is_check) {
      field_matrix_[x][y] = 2;
    }
  }
}

void Field::DeleteField() {
  field_matrix_.clear();
}

bool Field::IsDeadShip(const uint64_t& x, const uint64_t& y) {
  uint64_t size_x = field_matrix_.size();
  uint64_t size_y = field_matrix_[0].size();

  auto isShipCell = [&](uint64_t x, uint64_t y) {
    return x < size_x && y < size_y && (field_matrix_[x][y] == 1 || field_matrix_[x][y] == 2);
  };

  uint64_t left = y, right = y;
  while (left > 0 && isShipCell(x, left - 1)) {
    left--;
  }
  while (right + 1 < size_y && isShipCell(x, right + 1)) {
    right++;
  }
  for (uint64_t c = left; c <= right; c++) {
    if (field_matrix_[x][c] == 2) {
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
    if (field_matrix_[r][y] == 2) {
      return false;
    }
  }
  return true;
}

ShotStates Field::Shot(const uint64_t& x, const uint64_t& y) {
  if (x >= field_matrix_.size()|| y >= field_matrix_[0].size()) {
    return ShotStates::MISS;
  }
  if (field_matrix_[x][y] != 0) {
    field_matrix_[x][y] = 1;
    if (IsDeadShip(x,y)) {
      return ShotStates::KILL;
    }
    return ShotStates::HIT;
  }
  return ShotStates::MISS;
}

char Field::GetShipRotate(const uint64_t& x, const uint64_t& y) {
  if (y >= 1 && field_matrix_[x][y - 1] != 0 || x >= 1 && field_matrix_[x - 1][y] != 0) {
    return 'n';
  }
  if (x + 1 < field_matrix_.size() && field_matrix_[x + 1][y] != 0) {
    return 'h';
  }
  return 'v';
}

size_t Field::GetShipSize(uint64_t x, uint64_t y, char rotate) {
  size_t ship_size = 0;
  if (rotate == 'v') {
    while (y < field_matrix_[0].size() && field_matrix_[x][y] != 0) {
      ++ship_size;
      ++y;
    }
  } else {
    while (x < field_matrix_.size() && field_matrix_[x][y] != 0) {
      ++ship_size;
      ++x;
    }
  }
  return ship_size;
}

void Field::DumpState(std::ofstream& file) {
  char rotate;
  size_t size;
  uint64_t x;
  uint64_t y;
  for (uint64_t y = 0; y != field_matrix_[0].size(); ++y) {
    for (uint64_t x = 0; x != field_matrix_.size(); ++x) {
      if (field_matrix_[x][y] != 0) {
        rotate = GetShipRotate(x, y);
        if (rotate == 'n') {
          continue;
        }
        size = GetShipSize(x, y, rotate);
        file << size << " " << rotate << " ";
        file << x << " " << y << "\n";
      }
    }
  }
}

bool Field::IsAnyAlive() {
  for (uint64_t y = 0; y != field_matrix_[0].size(); ++y) {
    for (uint64_t x = 0; x != field_matrix_.size(); ++x) {
      if (field_matrix_[x][y] == 2) {
        return true;
      }
    }
  }
  return false;
}