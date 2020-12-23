#include "TimingDiagramDynamic.hpp"
#include <gtest/gtest.h>
#include <sstream>

TEST(TimeDiagramTest, InputsFromStream)
{
  std::stringstream ss;
  ss << R"(3
X 4
0 3
1 6
)";
  Diagram a;
  ss >> a;
  ASSERT_EQ(a.size(), 3);
  ASSERT_EQ(a[0].state, 'X');
  ASSERT_EQ(a[1].state, '0');
  ASSERT_EQ(a[2].state, '1');
  ASSERT_EQ(a[0].time, 4);
  ASSERT_EQ(a[1].time, 3);
  ASSERT_EQ(a[2].time, 6);
}

TEST(TimeDiagramTest, OutputsToStream)
{
  std::stringstream ss;
  Diagram a{"1111100XX1"};
  ss << a;
  ASSERT_EQ(a.size(), 4);
  ASSERT_STREQ(ss.str().c_str(), "1111100XX1");
}
TEST(TimeDiagramTest, ConcatsTwoDiagrams)
{
  std::stringstream ss;
  Diagram a{"1111100XX1"}, b{"111XXX00X"}, c = a + b;
  ss << c;
  ASSERT_EQ(c.size(), 7); // merge check
  ASSERT_STREQ(ss.str().c_str(), "1111100XX1111XXX00X");
}

TEST(TimeDiagramTest, ReplaceTimestampWithOtherDiagram)
{
  std::stringstream ss;
  Diagram a{"1111100XX1"}, b{"111XXX00X"};
  a(2, b);
  ss << a;
  ASSERT_EQ(a.size(), 8); // merge check
  ASSERT_STREQ(ss.str().c_str(), "1111XXX00X11100XX1");
}

TEST(TimeDiagramTest, ExceptionsChecks)
{
  std::stringstream ss;
  Diagram a{"1111100XX1"}, b{"111XXX00X"};
  ASSERT_THROW(a(11, b), std::out_of_range);
  ASSERT_THROW(Diagram("1110XXa"), std::runtime_error);
  ASSERT_THROW(a.at(5), std::out_of_range);
}

TEST(TimeDiagramTest, RepeatesChart)
{
  std::stringstream ss;
  Diagram a{"1111100XX1"};
  a *= 3;
  ss << a;
  ASSERT_EQ(a.size(), 10); // merge check
  ASSERT_STREQ(ss.str().c_str(), "1111100XX11111100XX11111100XX1");
  // ASSERT_THROW(a *= 3, std::runtime_error); // overflow check works only for static release
}

TEST(TimeDiagramTest, ShiftsChartToTheLeft)
{
  std::stringstream ss;
  Diagram a{"1111100XX1"};
  a <<= 2;
  ss << a;
  ASSERT_EQ(a.size(), 4); // merge check
  ASSERT_STREQ(ss.str().c_str(), "11100XX111");
  ss.str("");
  a <<= 3;
  ss << a;
  ASSERT_EQ(a.size(), 3); // merge check
  ASSERT_STREQ(ss.str().c_str(), "00XX111111");
  ss.str("");
  a <<= 6;
  ss << a;
  ASSERT_EQ(a.size(), 4); // merge check
  ASSERT_STREQ(ss.str().c_str(), "111100XX11");
  ss.str("");
}

TEST(TimeDiagramTest, ShiftsChartToTheRight)
{
  std::stringstream ss;
  Diagram a{"1111100XX1"};
  a >>= 2;
  ss << a;
  ASSERT_EQ(a.size(), 4); // merge check
  ASSERT_STREQ(ss.str().c_str(), "X11111100X");
  ss.str("");
  a >>= 3;
  ss << a;
  ASSERT_EQ(a.size(), 3); // merge check
  ASSERT_STREQ(ss.str().c_str(), "00XX111111");
  ss.str("");
  a >>= 4;
  ss << a;
  ASSERT_EQ(a.size(), 4); // merge check
  ASSERT_STREQ(ss.str().c_str(), "111100XX11");
  ss.str("");
}