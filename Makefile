BIN_DIR = bin
BUILD_DIR = build
SRC_DIR = src

SRCS := $(shell find $(SRC_DIR) -name *.cpp -or -name *.c)
OBJS := $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)
EXEC = $(BIN_DIR)/myfish

CXXFLAGS += -Wall -std=c++11 -O3
DEPENDFLAGS += -std=c++11
LDFLAGS +=

build: $(OBJS) .depend
	@$(MKDIR_P) $(BIN_DIR)
	$(CXX) -o $(EXEC) $(OBJS) $(LDFLAGS)
	@echo "Build Completed"
	@echo ""

.depend:
	-@$(CXX) $(DEPENDFLAGS) -MM $(DEPS) > $@ 2> /dev/null

.default:
	build

.PHONY: clean

clean:
	@rm -f $(EXE) $(OBJS) .depend

-include .depend

MKDIR_P ?= mkdir -p
