CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic -Icpp/include
LDFLAGS  ?=

SRCS := cpp/src/body.cpp cpp/src/force.cpp cpp/src/integrator.cpp cpp/src/json_io.cpp cpp/src/main.cpp
OBJS := $(SRCS:.cpp=.o)
BIN  := nbody-orbit

.PHONY: all clean run test demo

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

cpp/src/%.o: cpp/src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: $(BIN)
	mkdir -p data
	./$(BIN) --out data/trajectory.json

demo: $(BIN)
	mkdir -p data
	./$(BIN) --scenario scenarios/figure8.json --out data/figure8.json
	./$(BIN) --scenario scenarios/solar_lite.json --out data/solar_lite.json --steps 50000 --frame-every 50

test: $(BIN)
	mkdir -p data
	./$(BIN) --scenario scenarios/figure8.json --out data/test_traj.json --steps 5000 --frame-every 50
	python3 -m pytest -q python/tests

clean:
	rm -f $(OBJS) $(BIN)
