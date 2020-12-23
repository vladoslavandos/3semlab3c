project(TimingDiagram LANGUAGES CXX)
cmake_minimum_required(VERSION 3.12)

find_package(GTest REQUIRED)

# add_executable(Static main.cpp TimingDiagram.cpp)
# add_executable(Operators mainOperators.cpp TimingDiagramOperators.cpp)
add_executable(Dymanic mainDynamic.cpp TimingDiagramDynamic.cpp)

enable_testing()

add_executable(Tests test.cpp TimingDiagramDynamic.cpp)
target_link_libraries(Tests GTest::GTest GTest::Main)

add_test(AllTests Tests)
# add_executable(StaticEdition main1.cpp LogicGate.cpp)

# add_executable(DynamicEdition main.cpp LogicGateDynamic.cpp)
# # without menu
# add_executable(OperatorsEdition LogicGateOperators.cpp)