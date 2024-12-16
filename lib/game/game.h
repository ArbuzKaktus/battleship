#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include "shot.h"
#include "strategy.h"
#include "ship_storage.h"

struct ShipPlaceState {
  ShipPlaceState(uint64_t size_x, uint64_t size_y, std::vector<uint64_t> ships_count);

  bool MaKeStep();
  bool AllShipsArePlaced();

  bool IsAbroad();

  std::vector<uint64_t> count_ship_;
  size_t window_, new_size_;
  uint64_t x_, y_, min_x_, min_y_, max_x_, max_y_;
  uint16_t degrees_rotate_;
};

class SettingDevice {
public:
  virtual ~SettingDevice() = 0;
  SettingDevice();

  virtual bool SetShipsCount(const uint64_t& ships_count, const size_t& ship_size) = 0;
  virtual void DeleteStates() = 0;
  virtual bool CheckForStart();

  bool SetWidth(const uint64_t& size_x);
  bool SetHeight(const uint64_t& size_y);
  bool SetRole(const std::string& role);

  virtual const uint64_t GetAllShipsCount();
  const uint64_t GetWidth();
  const uint64_t GetHeight();
  const uint64_t GetShipsCount(size_t ship_size);

  bool IsSlave();

protected:
  bool is_slave_;
  uint64_t size_x_;
  uint64_t size_y_;
  std::vector<uint64_t> ships_count_;
};

class Game: public SettingDevice {
public:
  Game();
  virtual ~Game() = 0;

  virtual bool IsAnyAlive() = 0;
  virtual bool LoadConfig(const std::string& path) = 0;
  virtual void StartGame();

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

class ShipsConfig {
public:
  ShipsConfig() = default;
  ShipsConfig(uint64_t size_x, uint64_t size_y);
  
  bool SetShipsCount(const uint64_t& ships_count, const size_t& ship_size, ShipStorageBase* ships);
  bool PlaceAllShips(ShipStorageBase* ships);
  void PlaceShip(uint64_t x, uint64_t y, const size_t& ship_size, char rotate, ShipStorageBase* ships);
  
  bool IsConfigSet();

private:
  void PlaceShip(uint64_t& x, uint64_t& y, const size_t& ship_size, const uint16_t& rotation, ShipStorageBase* ships);
  
  bool CirclePlace(ShipPlaceState& state, ShipStorageBase* ships);
  bool ColumnRowPlace(bool is_row, ShipPlaceState& state, ShipStorageBase* ships);
  bool MakeUpToRightRotate(ShipPlaceState& state, ShipStorageBase* ships);
  bool MakeRightToDownRotate(ShipPlaceState& state, ShipStorageBase* ships);
  bool MakeDownToLeftRotate(ShipPlaceState& state, ShipStorageBase* ships);
  bool MakeLeftToUpRotate(ShipPlaceState& state, ShipStorageBase* ships);

  uint64_t size_x_;
  uint64_t size_y_;
  std::vector<uint64_t> ships_count_;
  bool is_check_;
};

class PlayerGame : public Game {
public:
  PlayerGame();
  ~PlayerGame();
  PlayerGame(const PlayerGame& other) = delete;
  PlayerGame& operator=(const PlayerGame& other) = delete;

  void DeleteStates() override;
  bool CheckForStart() override;
  bool LoadConfig(const std::string& path) override;
  void StartGame() override;
  void PlaceAllShips();
  void DumpState(std::ofstream& file);
  ShotStates Shot(const uint64_t& x, const uint64_t& y);

  bool SetShipsCount(const uint64_t& ships_count, const size_t& ship_size) override;
  void SetField(ShipStorageBase* ships);
  void SetConfig();
  
  bool IsAnyAlive() override;

private:
  ShipsConfig* config_;
  ShipStorageBase* ships_; 
};

class ProgrammGame : public Game {
public:
  ProgrammGame();

  bool IsAnyAlive() override;
  void DeleteStates() override;
  bool LoadConfig(const std::string& path) override;

  void SetRules();
  bool SetShipsCount(const uint64_t& ships_count, const size_t& ship_size) override;
  void SetStrategy(const std::string& strategy);

  std::pair<uint64_t, uint64_t> Shot();
  void UpdateFieldState(const ShotStates& shot_state);
  
private:
  uint64_t live_ships_count_;
  ChessStrategy chess_strategy_;
  OrderedStrategy ordered_strategy_;
  bool is_ordered_strategy_;
};