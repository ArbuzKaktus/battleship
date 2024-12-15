#pragma once

#include <cstdint>
#include <list>
#include <vector>

class Strategy {
  public:
    Strategy();
    virtual ~Strategy() = 0;
    virtual std::pair<uint64_t, uint64_t> Shot(const uint64_t& size_x, const uint64_t& size_y) = 0;
  protected:

    static uint64_t x_shot_;
    static uint64_t y_shot_;
};

class OrderedStrategy: public Strategy {
  public:
    std::pair<uint64_t, uint64_t> Shot(const uint64_t& size_x, const uint64_t& size_y) override;
};

class ChessStrategy: public Strategy {
  public:
    ChessStrategy();
    std::pair<uint64_t, uint64_t> Shot(const uint64_t& size_x, const uint64_t& size_y) override;
    void MakeOreal(const uint64_t& size_x, const uint64_t& size_y);
  private:
  
    void AddOreal(const uint64_t& x, const uint64_t& y);
    void DeleteOreal(const uint64_t& x, const uint64_t& y);
    bool IsOreal();
    bool second_circle_;
    uint64_t last_x_shot_;
    uint64_t last_y_shot_;
    std::list<std::pair<uint64_t, uint64_t>> temp_oreal_;
    std::vector<std::pair<uint64_t, uint64_t>> shots_memory;

};