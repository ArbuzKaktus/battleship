#pragma once

#include <cstdint>
#include <list>
#include <vector>

class Strategy {
public:
  Strategy();
  virtual ~Strategy() = 0;

  virtual std::pair<uint64_t, uint64_t> Shot(const uint64_t& size_x, const uint64_t& size_y) = 0;
  void ResetShot();

protected:
  uint64_t x_shot_;
  uint64_t y_shot_;
  uint64_t last_x_shot_;
  uint64_t last_y_shot_;
};

class OrderedStrategy: public Strategy {
public:
  ~OrderedStrategy() override = default;
  OrderedStrategy();

  std::pair<uint64_t, uint64_t> Shot(const uint64_t& size_x, const uint64_t& size_y) override;
};

class ChessStrategy: public Strategy {
public:
  ~ChessStrategy() override = default;
  ChessStrategy();
  std::pair<uint64_t, uint64_t> Shot(const uint64_t& size_x, const uint64_t& size_y) override;
  void MakeOreal(const uint64_t& size_x, const uint64_t& size_y);

private:
  void AddOreal(const uint64_t& x, const uint64_t& y);
  void DeleteOreal(const uint64_t& x, const uint64_t& y);

  bool IsSecondCircle();
  bool IsOreal();

  std::list<std::pair<uint64_t, uint64_t>> temp_oreal_;
  std::vector<std::pair<uint64_t, uint64_t>> shots_memory;
};