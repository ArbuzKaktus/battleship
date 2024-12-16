#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

#include "shot.h"
#include "ship_storage.h"

class Field: public ShipStorageBase {
public:
  ~Field() override = default;
  Field(uint64_t size_x, uint64_t size_y);

  void PlaceShip(uint64_t& x, uint64_t& y, const size_t& ship_size, size_t rotate, bool is_check) override;
  ShotStates Shot(const uint64_t& x, const uint64_t& y) override;
  void DumpState(std::ofstream& file) override;
  bool IsAnyAlive() override;
  
private:
  char GetShipRotate(const uint64_t& x, const uint64_t& y);
  size_t GetShipSize(uint64_t x, uint64_t y, char rotate);

  bool IsDeadShip(const uint64_t& x, const uint64_t& y);

  std::vector<std::vector<uint8_t>> field_matrix_;
};