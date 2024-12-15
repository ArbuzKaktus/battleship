#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>
#include "shot.h"

class Field {
  public:
    void CreateField(uint64_t size_x, uint64_t size_y);
    void PlaceShip(uint64_t& x, uint64_t& y, const size_t& ship_size, size_t rotate, bool is_check);
    void DeleteField();
    ShotStates Shot(const uint64_t& x, const uint64_t& y);
    bool IsDeadShip(const uint64_t& x, const uint64_t& y);
    char GetShipRotate(const uint64_t& x, const uint64_t& y);
    size_t GetShipSize(uint64_t x, uint64_t y, char rotate);
    void DumpState(std::ofstream& file);
    bool IsAnyAlive();
  private:
    std::vector<std::vector<uint8_t>> field_matrix_;
};