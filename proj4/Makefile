CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -Iinclude -I/src -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lgtest -lgtest_main -lpthread -lexpat -L/usr/lib/ -lgtest -lgtest_main
SRC_DIR = src
TEST_DIR = testsrc
OBJ_DIR = obj
BIN_DIR = bin


SRC = $(wildcard $(SRC_DIR)/*.cpp)
TESTSRC = $(wildcard $(TEST_DIR)/*.cpp)
OBJS = $(OBJ_DIR)/StringUtils.o $(OBJ_DIR)/StringDataSource.o $(OBJ_DIR)/StringDataSink.o $(OBJ_DIR)/DSVReader.o $(OBJ_DIR)/DSVWriter.o $(OBJ_DIR)/XMLReader.o $(OBJ_DIR)/XMLWriter.o $(OBJ_DIR)/CSVBusSystem.o $(OBJ_DIR)/OpenStreetMap.o $(OBJ_DIR)/DijkstraPathRouter.o $(OBJ_DIR)/BusSystemIndex.o $(OBJ_DIR)/TransportationPlannerCommandLine.o $(OBJ_DIR)/DijkstraTransportationPlanner.o
TESTOBJS = $(OBJ_DIR)/StringUtilsTest.o $(OBJ_DIR)/StringDataSourceTest.o $(OBJ_DIR)/StringDataSinkTest.o $(OBJ_DIR)/DSVTest.o $(OBJ_DIR)/XMLTest.o $(OBJ_DIR)/CSVBusSystemTest.o $(OBJ_DIR)/OpenStreetMapTest.o $(OBJ_DIR)/DijkstraPathRouterTest.o $(OBJ_DIR)/CSVBusSystemIndexTest.o $(OBJ_DIR)/TPCommandLineTest.o $(OBJ_DIR)/CSVOSMTransportationPlannerTest.o
TARGET = $(BIN_DIR)/tests


test: all
	./$(TARGET)


all: $(TARGET)


# make directories
directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)
	@echo "directories made"


# compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | directories
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "src compiled"
   
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp | directories
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "testsrc compiled"


# link tests
$(TARGET): $(OBJS) $(TESTOBJS) | directories
	@$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "linked tests"


# clean build
clean:
	@rm -rf $(OBJ_DIR)
	@rm -rf $(BIN_DIR)
	@echo "cleaned"