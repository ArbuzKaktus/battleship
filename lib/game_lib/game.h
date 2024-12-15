#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include "field.h"
#include "ship.h"
#include "shot.h"
#include "strategy.h"

struct ShipPlaceState {
  ShipPlaceState(uint64_t size_x, uint64_t size_y, std::vector<uint64_t> ships_count);
  bool IsAbroad();
  bool MaKeStep();
  bool AllShipsArePlaced();

  std::vector<uint64_t> count_ship;
  size_t window, new_size;
  uint64_t x, y, min_x, min_y, max_x, max_y;
  uint16_t degrees_rotate;
};

class SettingDevice {
  public:
    SettingDevice();
    virtual ~SettingDevice() = 0;
    virtual bool SetShipsCount(const uint64_t& ships_count, const size_t& ship_size);
    virtual void DeleteStates();
    uint64_t GetWidth();
    uint64_t GetHeight();
    uint64_t GetShipsCount(size_t ship_size);
    void SetWidht(const uint64_t& size_x);
    void SetHeight(const uint64_t& size_y);
    void SetRole(const std::string& role);
    bool CheckForStart();
    bool IsSlave();

  protected:
    bool is_slave_;
    uint64_t size_x_;
    uint64_t size_y_;
    std::vector<uint64_t> ships_count_;
};

class Game: public SettingDevice {
  public:
    virtual ~Game() = 0;
    virtual bool IsAnyAlive() = 0;
    virtual uint64_t GetAllShipsCount();
    Game();
    void StartGame();
    bool IsWin();
    bool IsMyTurn();
    bool IsGameOn();
    bool IsLose();
    bool IsFinished();
    void StartMyTurn();
    void StopMyTurn();

  protected:
    bool is_my_turn_;
    bool is_game_on_;
};

class PlayerGame : public Game {
  public:
    PlayerGame();
    bool SetShipsCount(const uint64_t& ships_count, const size_t& ship_size) override;
    void DeleteStates() override;
    bool IsAnyAlive() override;
    void DumpState(std::ofstream& file);
    bool PlaceAllShips(bool is_check);
    bool IsPosibleToPlace(const uint64_t& ships_count, const size_t& ship_size);
    void PlaceShip(uint64_t x, uint64_t y, const size_t& ship_size, char rotate);
    void CreateField();
    ShotStates Shot(const uint64_t& x, const uint64_t& y);

  private:
    bool CirclePlace(bool is_check, ShipPlaceState& state);
    bool ColumnRowPlace(bool is_check, bool is_row, ShipPlaceState& state);
    bool MakeUpToRightRotate(ShipPlaceState& state, bool is_check);
    bool MakeRightToDownRotate(ShipPlaceState& state, bool is_check);
    bool MakeDownToLeftRotate(ShipPlaceState& state, bool is_check);
    bool MakeLeftToUpRotate(ShipPlaceState& state, bool is_check);
    void PlaceShip(uint64_t& x, uint64_t& y, const size_t& ship_size, const uint16_t& rotation, bool is_check);
    bool is_field_;
    Field field_;
    ShipsList ships_;
};

class ProgrammGame : public Game {
  public:
    ProgrammGame();
    bool IsAnyAlive() override;
    bool SetShipsCount(const uint64_t& ships_count, const size_t& ship_size) override;
    void DeleteStates() override;
    void SetRules();
    void SetStrategy(const std::string& strategy);
    std::pair<uint64_t, uint64_t> Shot();
    void UpdateFieldState(const ShotStates& shot_state);

  private:
    uint64_t live_ships_count_;
    ChessStrategy chess_strategy_;
    OrderedStrategy ordered_strategy_;
    bool is_ordered_strategy_;
};