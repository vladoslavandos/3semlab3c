#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

struct Signal
{
  char state;
  int time;
  Signal(char _state = 'X', int _time = 0);
  friend std::istream& operator>>(std::istream& st, Signal& sig);
  friend std::ostream& operator<<(std::ostream& st, Signal const& sig);
};

class Diagram
{
public:
  // static size_t constexpr __n = 20;

private:
  Signal* sections;
  size_t csize;
  Diagram(size_t blocksn);

public:
  inline size_t size() const { return csize; }
  Diagram();
  Diagram(char state);
  Diagram(char const* ascii_symbs);
  Diagram(std::vector<Signal> const& sigs);
  Diagram(Diagram const& Diagram);
  Diagram& operator=(Diagram const& Diagram);
  Diagram(Diagram&& Diagram) noexcept;
  Diagram& operator=(Diagram&& Diagram);
  ~Diagram();
  friend std::istream& operator>>(std::istream& st, Diagram& Diagram);
  friend std::ostream& operator<<(std::ostream& st, Diagram const& Diagram);
  Diagram operator+(Diagram const& second) const;
  int get_total_time() const;
  void insertSignalBlock(Signal const& sig);
  Diagram& operator()(int timestamp, Diagram const& second);
  Diagram& operator*=(size_t n);
  Diagram& operator>>=(int tshift);
  Diagram& operator<<=(int tshift);
  Diagram& mergeBlocks();

  // получить блок сигнала по индексу (с проверкой границ)
  Signal const& at(size_t n) const;
  // получить блок сигнала по индексу (без проверки границ)
  Signal const& operator[](size_t n) const;
};