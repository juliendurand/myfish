OBJS = src/main.o
EXEC = ./bin/myfish

CXXFLAGS += -Wall -std=c++11 -O3
DEPENDFLAGS += -std=c++11
LDFLAGS +=

build: $(OBJS) .depend
	$(CXX) -o $(EXEC) $(OBJS) $(LDFLAGS)

clean:
	@rm -f $(EXE) $(OBJS) .depend


.depend:
	-@$(CXX) $(DEPENDFLAGS) -MM $(OBJS:.o=.cpp) > $@ 2> /dev/null

-include .depend
