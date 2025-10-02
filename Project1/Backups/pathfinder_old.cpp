// 8-Puzzle
// Jordan Cobb and Aja Brunet
// 9/25/25

#include <iostream>
#include <vector>
#include <fstream>
#include <tuple>
#include <string>
#include <cmath>
#include <queue>
#include <ctime>
#include <unordered_set>

/*
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
*/

using namespace std;
// namespace py = pybind11;

struct Tile {
    char symbol;
    int index;

    vector<Tile*> sides = {};

    Tile(char symbol, int index) : symbol(symbol), index(index) {}
};



struct State {
    string nodeString;
    int blankIndex = 0; // Maintain the index of the '_' to easiily swap values
    int manDist = 0;    // Sum of the tiles' distances to the goal

    vector<Tile*> tiles;
    State* parent = nullptr;
    int level = 0;

    // Sole state constructor
    State(string stateString, State* parent, int level) : nodeString(stateString), parent(parent) , level(level) {
        // Initialize the tiles in the state
        int index = 0;
        for (char symbol : stateString) {
            if (symbol == '_')
                blankIndex = index;
            tiles.push_back(new Tile(symbol, index));
            index++;
        }

        // Properly connect tiles to neighbors
        // Creates 2n(n-1) connections for n x n board
        int stateSize = stateString.size(); // 9
        int n = sqrt(stateSize);            // n = 3
        for (int i = 0; i < stateSize; i++) {            
            if (((i + 1) % n) != 0)     // Tile has a right neighbor
                tiles[i]->sides.push_back(tiles[i + 1]);        
            if (i < (stateSize - n))    // Tile has a bottom neighbor
                tiles[i]->sides.push_back(tiles[i + n]); 
            if ((i % n) != 0)           // Tile has a left neighbor
                tiles[i]->sides.push_back(tiles[i - 1]);
            if (i >= n)                 // Tile has an upper neighbor
                tiles[i]->sides.push_back(tiles[i - n]);
        }
    }

    void swapSymbols(int otherIndex) {
        char temp = tiles[otherIndex]->symbol;
        tiles[otherIndex]->symbol = tiles[blankIndex]->symbol;
        tiles[blankIndex]->symbol = temp;
        blankIndex = otherIndex;
    }

    // Print selected state using helper function
    void printState() {
        string stateString = getState();
        int n = sqrt(stateString.length());
        int count = 0;
        for (char c : stateString) {
            count++;
            cout << c;
            if (count % n == 0)
                cout << endl;
        }
        cout << endl;
    }

    // Returns a string of all the current symbol orientation
    string getState() {
        string output;
        for (Tile* tile : tiles)
            output += tile->symbol;
        return output;
    }

    ~State() {
        for (Tile* tile : tiles)
            delete tile;
    }
};

struct BFS {
    bool operator()(const State* a, const State* b) const {
        return a; // UPDATE AS BFS is created
    }
}

struct AStar {
    bool operator()(const State* a, const State* b) const {
        return a->manDist + a->level > b->manDist + b->level; // a > b means a has lower priority
    }
};

struct Greedy {
    bool operator()(const State* a, const State* b) const {
        return a->manDist > b->manDist; // a > b means a has lower priority
    }
};

vector<string> storePath(State *endState)
{
    vector<string> path;
    // Save the found path to the output vector
    State *current = endState;
    int count = 0;
    if (!current)
    {
        cout << "No solution found!" << endl;
        return {};
    }
    while (current)
    {
        path.push_back(current->getState());
        current = current->parent;
        count++;
    }
    cout << "Path length: " << count << endl;

    return path;
}

struct Puzzle {
    int n;
    int sides;
    string start;
    string goal;

    State* endState;
    vector<State*> states;
    
    Puzzle(string start, string goal) : n(sqrt(start.size())), start(start), goal(goal) {}

    // Calculate the Manhattan Distance between tiles and store in state
    void findDistances(State* state) {
        for (Tile* tile : state->tiles) {
            if (tile->symbol != '_') {
                int currentIndex = tile->index;
                int goalIndex = goal.find(tile->symbol);
                int currentRow = currentIndex / n;
                int currentCol = currentIndex % n;
                int goalRow = goalIndex / n;
                int goalCol = goalIndex % n;
                state->manDist += abs(currentRow - goalRow) + abs(currentCol - goalCol);
            }
        }
    }



    template <typename Comparator>
    vector<string> calcAStarGreedy() {

        // Declare data structures and initialize states
        priority_queue<State*, vector<State*>, Comparator> searchNext;
        unordered_set<string> visited;    // Use unordered set to achieve 0(1) lookup for visited states using its string
        State* startState = new State(start, nullptr, 0);
        searchNext.push(startState);

        int count = 0;
        while (!searchNext.empty()) {
            State* currState = searchNext.top();
            searchNext.pop();
            string currString = currState->getState();
            visited.insert(currString);

            states.push_back(currState);

            if (currString == goal) {
                cout << "Goal found!" << endl;
                endState = currState;
                break;
            }

            Tile* blankTile = currState->tiles[currState->blankIndex];

            // Iterate through the neighbors of the blanktile to "consider options" for possible moves
            for (Tile* side : blankTile->sides) {
                State* newState = new State(currString, currState, currState->level + 1);
                newState->swapSymbols(side->index);

                string newString = newState->getState();
                if (visited.find(newString) == visited.end()) {  // State has not been visited
                    // Check if state already exists in visited set
                    findDistances(newState);
                    visited.insert(newString);
                    searchNext.push(newState);
                }
                else
                    delete newState;
            }
            count++;
        }

        cout << "Iterations: " << count << endl;

        return storePath(endState);
    }


    // ************ BREADTH FIRST SEARCH IMPLEMENTATION HERE ***********
    template <typename Comparator>
    vector<string> calcBFS() {

        // Declare data structures and initialize states
        //priority_queue<State*, vector<State*>, Comparator> searchNext;
        queue<State*, vector<State*>, Comparator> searchNext;
        unordered_set<string> visited;    // Use unordered set to achieve 0(1) lookup for visited states using its string
        State* startState = new State(start, nullptr, 0);
        searchNext.push(startState);

        int count = 0;
        while (!searchNext.empty()) {
            State* currState = searchNext.front(); //
            searchNext.pop();
            string currString = currState->getState();
            visited.insert(currString);

            states.push_back(currState);

            if (currString == goal) {
                cout << "Goal found!" << endl;
                endState = currState;
                break;
            }

            Tile* blankTile = currState->tiles[currState->blankIndex];

            // Iterate through the neighbors of the blanktile to "consider options" for possible moves
            for (Tile* side : blankTile->sides) {
                State* newState = new State(currString, currState, currState->level + 1);
                newState->swapSymbols(side->index);

                string newString = newState->getState();
                if (visited.find(newString) == visited.end()) {  // State has not been visited
                    // Check if state already exists in visited set
                    findDistances(newState);
                    visited.insert(newString);
                    searchNext.push(newState);
                }
                else
                    delete newState;
            }
            count++;
        }

        cout << "Iterations: " << count << endl;

        return storePath(endState);
    }





    // Destructor to dynamically deallocate state vectors
    ~Puzzle() {
        int count = 0;
        for (State* state : states) {
            delete state;
            count++;
        }
        // cout << to_string(count) << " state(s) deleted" << endl;
    }
};



vector<string> run(string start, string goal, string algorithm) {
    vector<string> path;

    // Initialize 
    Puzzle* puzzle = new Puzzle(start, goal);

    // Start timer
    clock_t begin = clock();

    // Calculate path (start state -> end state) using specified algorithm
    if (algorithm == "BFS")
        path = puzzle->calcAStarGreedy<Greedy>();
    else if (algorithm == "DFS")
        path = puzzle->calcAStarGreedy<Greedy>();
    else if (algorithm == "UCS")
        path = puzzle->calcAStarGreedy<Greedy>();
    else if (algorithm == "Greedy")
        path = puzzle->calcAStarGreedy<Greedy>();
    else if (algorithm == "A*")
        path = puzzle->calcAStarGreedy<AStar>();
    else {
        cout << "Invalid algorithm type" << endl; 
        return path;
    }

    // End timer and print
    clock_t end = clock();
    double elapsed_time = double(end - begin) / CLOCKS_PER_SEC;
    cout << "Runtime: " << elapsed_time << " seconds" << endl;

    // Deallocate memory
    delete puzzle;

    return path;
}

int main() {
    // Initialize variables
    string start = "BUMHLODT_";
    string goal = "HUMBOLDT_";

    vector<string> path = run(start, goal, "Greedy");

    return 0;
}

/*
// Pybind module
PYBIND11_MODULE(pathfinder, m) {
	m.doc() = "8-Puzzle Module";

	m.def("find_path", &run,
		  "Find the shortest path from the start state to a goal state with a selected algorithm",
		  py::arg("start"), py::arg("goal"), py::arg("algorithm"));
}
*/