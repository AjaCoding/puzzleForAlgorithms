PYTHON_INC := $(shell python3 -m pybind11 --includes)
PYTHON_EXT := $(shell python3-config --extension-suffix)
CXX := g++
CXXFLAGS := -O3 -Wall -std=c++11 -fPIC

# Sources
SRC := pathfinder.cpp
EXEC := pathfinder_exec
PYMOD := pathfinder$(PYTHON_EXT)

# Default: build both
all: $(EXEC) $(PYMOD)

# ---- Python extension (.so) ----
$(PYMOD): $(SRC)
	$(CXX) $(CXXFLAGS) -shared $(PYTHON_INC) $< -o $@

# ---- Standalone executable ----
$(EXEC): $(SRC)
	$(CXX) $(CXXFLAGS) $< -o $@

# ---- Run commands ----
run: $(EXEC)
	./$(EXEC)

test: $(PYMOD)
	python3 -c "import pathfinder; print('Module loaded:', pathfinder)"

# ---- Cleanup ----
clean:
	rm -f $(EXEC) $(PYMOD)
