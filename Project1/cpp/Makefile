PYTHON_INC := $(shell python3 -m pybind11 --includes)	# Get Python include flags using pybind11
PYTHON_EXT := $(shell python3-config --extension-suffix) # Get Python extension suffix (e.g., .so, .pyd)
CXX := g++	# C++ compiler
CXXFLAGS := -O3 -Wall -std=c++11 -fPIC # Compiler flags, including c++11 and shared object flag

# Sources
SRC := pathfinder.cpp # Source file for both executable and Python module
EXEC := pathfinder_exec # Standalone executable name
PYMOD := pathfinder$(PYTHON_EXT) # Create Python module name with correct suffix (e.g., .so, .pyd)

# Default: build both
all: $(EXEC) $(PYMOD)	# Build both the executable and the Python module

# ---- Python extension (.so) ----
$(PYMOD): $(SRC)
	$(CXX) $(CXXFLAGS) -shared $(PYTHON_INC) $< -o $@

# ---- Standalone executable ----
$(EXEC): $(SRC)
	$(CXX) $(CXXFLAGS) $< -o $@

# ---- Run commands ----
run: $(EXEC) $(PYMOD)
	./$(EXEC)

# ---- Cleanup ----
clean:
	rm -f $(EXEC) $(PYMOD)
