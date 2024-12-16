#pragma once

#include <cstddef>
#include <cstdint>
#include <bitset>
#include <fstream>
#include <list>

#include "ship_storage.h"
#include "shot.h"

class Ship {
public:
  Ship() = default;

  void PlaceShip(uint64_t& x, uint64_t& y, const uint16_t& rotate_degrees, const size_t& ship_size);

  bool GetStateShot(const uint64_t& x, const uint64_t& y);
  std::pair<uint64_t, uint64_t> GetCoordinate();
  size_t GetShipSize();
  size_t GetRotate();
  size_t GetHp();

  bool IsHit(const uint64_t& x, const uint64_t& y);

private:  
  uint64_t x_;
  uint64_t y_;
  std::bitset<8> states_;
};

class ShipsList: public ShipStorageBase {
  public:
    ~ShipsList() override = default;

    ShotStates Shot(const uint64_t& x,const uint64_t& y) override;
    void PlaceShip(uint64_t& x, uint64_t& y, const size_t& ship_size, size_t rotation, bool is_check) override;
    void DumpState(std::ofstream& file) override;
    
    bool IsAnyAlive() override;

  private:
    std::list<Ship> ships_;
};