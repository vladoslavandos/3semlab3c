#include "TimingDiagramDynamic.hpp"

Signal::Signal(char _state, int _time) : state{_state}, time{_time} {};

std::istream& operator>>(std::istream& st, Signal& sig)
{
  sig.state = '\0';
  while (sig.state != '0' && sig.state != '1' && sig.state != 'X')
  {
    st >> sig.state;
  }
  st >> sig.time;
  return st;
}

std::ostream& operator<<(std::ostream& st, Signal const& sig)
{
  for (int i = 0; i < sig.time; i++)
    st << sig.state;
  return st;
}

Diagram::Diagram() : csize{0}, sections{nullptr} {}

Diagram::Diagram(char state) : csize{1} { sections = new Signal(state, INT32_MAX); }

Diagram::Diagram(char const* ascii_symbs) : csize{0}
{
  size_t len = std::strlen(ascii_symbs), blocksn = 0;
  int counter = 1;
  char last   = '\0';
  for (size_t i = 0; i < len; i++)
  {
    if (ascii_symbs[i] != '0' && ascii_symbs[i] != '1' && ascii_symbs[i] != 'X')
      throw std::runtime_error("Unexpected symbol.");
    if (last != ascii_symbs[i])
    {
      if (last != '\0')
        blocksn++;
      last = ascii_symbs[i];
    }
  }
  if (len != 0)
    blocksn++;
  sections = new Signal[blocksn];
  last     = '\0';
  for (size_t i = 0; i < len; i++)
  {
    if (last == ascii_symbs[i])
      counter++;
    else
    {
      if (last != '\0')
      {
        sections[csize++] = Signal(last, counter);
        counter           = 1;
      }
      last = ascii_symbs[i];
    }
  }
  if (len != 0)
    sections[csize++] = Signal(last, counter);
}

Diagram::Diagram(std::vector<Signal> const& sigs) : csize{0}, sections{new Signal[sigs.size()]}
{
  for (auto& item : sigs)
    sections[csize] = sigs[csize++];
}

Diagram::Diagram(size_t blocksn) : csize{blocksn}, sections{new Signal[blocksn]} {}

Diagram::Diagram(Diagram const& Diagram) : csize{0}
{
  sections = new Signal[Diagram.size()];
  for (size_t i = 0; i < Diagram.size(); i++)
    sections[csize] = Diagram.sections[csize++];
}

Diagram& Diagram::operator=(Diagram const& Diagram)
{
  delete[] sections;
  csize    = 0;
  sections = new Signal[Diagram.size()];
  for (size_t i = 0; i < Diagram.size(); i++)
    sections[csize] = Diagram.sections[csize++];
  return *this;
}

Diagram::Diagram(Diagram&& Diagram) noexcept : csize{Diagram.size()}, sections{Diagram.sections}
{
  Diagram.sections = nullptr;
  Diagram.csize    = 0;
}

Diagram& Diagram::operator=(Diagram&& Diagram)
{
  delete[] sections;
  csize          = Diagram.size();
  sections       = Diagram.sections;
  Diagram.sections = nullptr;
  Diagram.csize    = 0;
  return *this;
}

Diagram::~Diagram()
{
  delete[] sections;
  csize = 0;
}

std::istream& operator>>(std::istream& st, Diagram& Diagram)
{
  st >> Diagram.csize;
  Diagram.sections = (Signal*)std::realloc((void*)Diagram.sections, Diagram.size() * sizeof(Signal));
  for (size_t i = 0; i < Diagram.csize; i++)
    st >> Diagram.sections[i];
  return st;
}

std::ostream& operator<<(std::ostream& st, Diagram const& Diagram)
{
  for (size_t i = 0; i < Diagram.csize; i++)
    st << Diagram.sections[i];
  return st;
}

Diagram Diagram::operator+(Diagram const& second) const
{
  Diagram result{size() + second.size()};
  for (size_t i = 0; i < size(); i++)
    result.sections[i] = sections[i];

  for (size_t i = 0; i < second.size(); i++)
    result.sections[i + size()] = second.sections[i];

  result.mergeBlocks();

  return result;
}

Diagram& Diagram::operator()(int timestamp, Diagram const& second)
{
  Diagram result;
  int time = 0;
  size_t i = 0;
  for (; i < size(); i++)
  {
    time += sections[i].time;
    if (time >= timestamp)
      break;
    result.insertSignalBlock(Signal(sections[i].state, sections[i].time));
  }
  if (i == size())
    throw std::out_of_range("Timestamp arg was out of avaliable signal time range.");
  result.insertSignalBlock(Signal(sections[i].state, sections[i].time - time + timestamp - 1));

  for (size_t j = 0; j < second.size(); j++)
    result.insertSignalBlock(Signal(second.sections[j].state, second.sections[j].time));
  result.insertSignalBlock(Signal(sections[i].state, time - timestamp));
  i++;
  for (; i < size(); i++)
    result.insertSignalBlock(Signal(sections[i].state, sections[i].time));

  result.mergeBlocks();

  return *this = std::move(result);
}

Diagram& Diagram::operator*=(size_t n)
{
  sections = (Signal*)std::realloc((void*)sections, size() * n * sizeof(Signal));
  for (size_t i = 1; i < n; i++)
    for (size_t j = 0; j < size(); j++)
      sections[i * size() + j] = sections[j];
  csize = size() * n;

  mergeBlocks();

  return *this;
}

Diagram& Diagram::operator>>=(int tshift)
{
  if (get_total_time() == 0 || tshift == 0)
    return *this;
  if (tshift < 0)
    return *this <<= -tshift;
  Diagram result;
  int time = 0;
  tshift %= get_total_time();
  auto i = size() - 1;
  for (; i >= 0; i--)
  {
    time += sections[i].time;
    if (time >= tshift)
      break;
  }
  auto pos = i;
  if (sections[i].time - time + tshift)
    result.insertSignalBlock(Signal(sections[i].state, sections[i].time - time + tshift));
  i++;
  for (; i < size(); i++)
    result.insertSignalBlock(Signal(sections[i].state, sections[i].time));
  for (i = 0; i < pos; i++)
    result.insertSignalBlock(Signal(sections[i].state, sections[i].time));
  if (time - tshift)
    result.insertSignalBlock(Signal(sections[i].state, time - tshift));

  result.mergeBlocks();

  return *this = std::move(result);
}

Diagram& Diagram::operator<<=(int tshift)
{
  if (get_total_time() == 0 || tshift == 0)
    return *this;
  if (tshift < 0)
    return *this >>= -tshift;
  return *this >>= get_total_time() - tshift;
}

int Diagram::get_total_time() const
{
  int result = 0;
  for (size_t i = 0; i < size(); i++)
    result += sections[i].time;
  return result;
}

void Diagram::insertSignalBlock(Signal const& sig)
{
  sections          = (Signal*)std::realloc((void*)sections, (size() + 1) * sizeof(Signal));
  sections[csize++] = sig;
}

Diagram& Diagram::mergeBlocks()
{
  if (size() == 0)
    return *this;
  char last             = '\0';
  int block_time        = sections[0].time;
  size_t resulting_size = 0;
  for (size_t i = 0; i < size(); i++)
  {
    if (last == sections[i].state)
      block_time += sections[i].time;
    else
    {
      if (last != '\0')
      {
        sections[resulting_size++] = Signal(last, block_time);
        block_time                 = sections[i].time;
      }
      last = sections[i].state;
    }
  }
  sections[resulting_size++] = Signal(last, block_time);
  csize                      = resulting_size;
  sections                   = (Signal*)std::realloc((void*)sections, size() * sizeof(Signal));
  return *this;
}

Signal const& Diagram::at(size_t n) const
{
  if (n >= size())
    throw std::out_of_range("Index was out of range.");
  return sections[n];
}

Signal const& Diagram::operator[](size_t n) const { return sections[n]; }