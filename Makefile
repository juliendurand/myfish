BIN_DIR = bin
BUILD_DIR = build
SRC_DIR = src

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS := $(OBJS:.o=.d)
EXEC = $(BIN_DIR)/myfish

CXXFLAGS += -Wall -std=c++11 -O3
DEPENDFLAGS += -std=c++11
LDFLAGS +=

build: $(OBJS)
	@$(MKDIR_P) $(BIN_DIR)
	$(CXX) -o $(EXEC) $(OBJS) $(LDFLAGS)
	@echo "Build Completed"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR_P) $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.depend:
	@$(CXX) $(DEPENDFLAGS) -MM $(DEPS) > $@ 2> /dev/null

.PHONY: clean

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR) .depend

-include $(DEPS)

MKDIR_P ?= mkdir -p
