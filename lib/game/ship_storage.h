#pragma once

#include <cstdint>
#include <fstream>

#include "shot.h"

class ShipStorageBase {
  public:
    virtual ~ShipStorageBase() = 0;

    virtual ShotStates Shot(const uint64_t& x,const uint64_t& y) = 0;
    virtual void PlaceShip(uint64_t& x, uint64_t& y, const size_t& ship_size, size_t rotation, bool is_check) = 0;
    virtual void DumpState(std::ofstream& file) = 0;
    
    virtual bool IsAnyAlive() = 0;
};