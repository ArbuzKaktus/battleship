#pragma once

#include "shot.h"
#include <cstddef>
#include <cstdint>
#include <bitset>
#include <fstream>
#include <list>

class Ship {
  public:
    Ship() = default;
    void PlaceShip(uint64_t& x, uint64_t& y, const uint16_t& rotate_degrees, const size_t& ship_size);
    bool IsHit(const uint64_t& x, const uint64_t& y);
    bool GetStateShot(const uint64_t& x, const uint64_t& y);
    std::pair<uint64_t, uint64_t> GetCoordinate();
    size_t GetShipSize();
    size_t GetRotate();
    size_t GetHp();

  private:  
    uint64_t x_;
    uint64_t y_;
    std::bitset<8> states_;
};

class ShipsList {
  public:
    ShotStates Shot(const uint64_t& x,const uint64_t& y);
    void DeleteShips();
    void PlaceShip(uint64_t& x, uint64_t& y, const size_t& ship_size, size_t rotation, bool is_check);
    void DumpState(std::ofstream& file);
    bool IsAnyAlive();
  private:
    std::list<Ship> ships_;
};
