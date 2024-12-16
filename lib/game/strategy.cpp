#include "strategy.h"

#include <cstddef>

void Strategy::ResetShot() {
  x_shot_ = last_x_shot_;
  y_shot_ = last_y_shot_;
}

ChessStrategy::ChessStrategy() {
  x_shot_ = 0;
  y_shot_ = 0;
  last_x_shot_ = 0;
  last_y_shot_ = 0;
}

void ChessStrategy::AddOreal(const uint64_t& x, const uint64_t& y) {
  std::pair<uint64_t, uint64_t> cords(x, y);
  for (auto it = temp_oreal_.begin() ; it != temp_oreal_.begin(); ++it) {
    if (*it == cords) {
      return;
    }
  }
  temp_oreal_.push_back(cords);
}

void ChessStrategy::MakeOreal(const uint64_t& size_x, const uint64_t& size_y) {
  uint64_t x = last_x_shot_;
  uint64_t y = last_y_shot_;
  if (x + 1 < size_x && y + 1 < size_y) {
    AddOreal(x + 1, y + 1);
  }
  if (x + 1 < size_x && y > 1) {
    AddOreal(x + 1, y - 1);
  }
  if (!IsSecondCircle()) {
    if (x + 1 < size_x) {
      AddOreal(x + 1, y);
    }
    if (y + 1 < size_y) {
      AddOreal(x, y + 1);
    }
    if (y >= 1) {
      AddOreal(x, y - 1);
    }
    if (x >= 1) {
      AddOreal(x - 1, y);
    }
  }
}

Strategy::~Strategy() {}

Strategy::Strategy() {
  x_shot_ = 0;
  y_shot_ = 0;
  last_x_shot_ = 0;
  last_y_shot_ = 0;
}

bool ChessStrategy::IsSecondCircle() {
  return last_y_shot_ % 2 != 0;
}

bool ChessStrategy::IsOreal() {
  for (std::pair<uint64_t, uint64_t>& elem : temp_oreal_) {
    if (elem.first == x_shot_ && elem.second == y_shot_) {
      return true;
    }
  }
  return false;
}

void ChessStrategy::DeleteOreal(const uint64_t& x, const uint64_t& y) {
  std::pair<uint64_t, uint64_t> cords(x, y);
  for (auto it = temp_oreal_.begin(); it != temp_oreal_.end(); ++it) {
    if (*it == cords) {
      temp_oreal_.erase(it);
      return;
    }
  }
}

std::pair<uint64_t, uint64_t> ChessStrategy::Shot(const uint64_t& size_x, const uint64_t& size_y) {
  const size_t kChessOffset = 2;
  if (!IsOreal()) {
    last_x_shot_ = x_shot_;
    last_y_shot_ = y_shot_;
    if (y_shot_ + kChessOffset >= size_y) {
      ++x_shot_;
      if (x_shot_ >= size_x) {
        x_shot_ = 0;
      }
      !IsSecondCircle() ? y_shot_ = (x_shot_ + 1) % 2 : x_shot_ % 2;
    } else {
      y_shot_ += kChessOffset;
    }
  } else {
    while (IsOreal()) {
      if (y_shot_ + kChessOffset >= size_y) {
        ++x_shot_;
        if (x_shot_ >= size_x) {
          x_shot_ = 0;
        }
        !IsSecondCircle() ? y_shot_ = (x_shot_ + 1) % 2 : x_shot_ % 2;
      } else {
        y_shot_ += kChessOffset;
      }
    }
    last_x_shot_ = x_shot_;
    last_y_shot_ = y_shot_;
  }
  return {last_x_shot_, last_y_shot_};
}

OrderedStrategy::OrderedStrategy() {
  x_shot_ = 0;
  y_shot_ = 0;
  last_x_shot_ = 0;
  last_y_shot_ = 0;
}

std::pair<uint64_t, uint64_t> OrderedStrategy::Shot(const uint64_t& size_x, const uint64_t& size_y) {
  last_x_shot_ = x_shot_;
  last_y_shot_ = y_shot_;
    if (x_shot_ + 1 >= size_x) {
    x_shot_ = 0;
    ++y_shot_;
    if (y_shot_ >= size_y) {
      y_shot_ = 0;
      x_shot_ = 0;
    }
  } else {
    ++x_shot_;
  }
  return {last_x_shot_, last_y_shot_};
}