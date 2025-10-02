from tkinter import *
import tkinter as tk
from tkinter import ttk
import pandas as pd
import numpy as np
import pathfinder


# print(pathfinder.__doc__)

# PANDAS READING CVS FILE AND RETURNING name, n, sides, start, goal
def readCSV(puzzle_num):  #puzzle_num is puzzle1, puzzle2,...
    puzzle = 'puzzle' + str(puzzle_num)
    print("About to search for: ", puzzle)
    # Make a Dataframe using Pandas to read csv
    try: 
        p_df = pd.read_csv("puzzles.csv") # Load data from CSV
        try:
            name, n, sides, start, goal = p_df.loc[p_df["id"] == puzzle, ["name", "n", "sides", "start", "goal"]].iloc[0] # access the only row selected from this
            return(name, n, sides, start, goal)
        except: 
            print("ERROR ACCESSING ROW(S)")
            quit
            
    except FileNotFoundError:
        print("Issue accessing csv file. ")

def display_grid(root, frame, word, n, state_index, total_states):
    """Draw one grid state inside frame"""
    # Clear old grid
    for widget in frame.winfo_children():
        widget.destroy()

    # Draw the grid
    for row in range(n):
        for col in range(n):
            idx = row * n + col
            char = word[idx]
            lbl = tk.Label(
                frame,
                text=char,
                width=4,
                height=2,
                font=("Arial", 24),
                borderwidth=1,
                relief="solid"
            )
            lbl.grid(row=row, column=col, padx=2, pady=2)

    # Show counter label
    counter_label.config(text=f"State {total_states - state_index}/{total_states}")

def get_selected_alg():
    print(f"Selected algorithm: {alg_var.get()}")
    selection_label.config(text=f"Selected: {alg_var.get()}")


def show_state():
    global states, current_index
    if not states:
        print("No states available")
        return

    idx = len(states) - 1 - current_index["val"]   # Option 2 style
    display_grid(root, grid_frame, states[idx], n, idx, len(states))

if __name__ == "__main__":
    root = tk.Tk()
    root.title("State Grid")

    # Read CSV to access puzzles
    puzzleNum = 1
    try:
        name, n, sides, start, goal = readCSV(puzzleNum)
        print(f"Here is the returned val: {start}, {goal}, and that's it")
    except:
        print("Read not successful.")

    

    #MAKE SELECTION RADIO BUTTONS INSTEAD 9/30
    # Variable to hold the currently selected algorithm
    alg_var = tk.StringVar(value="BFS")  # default selection

    # Navigation state index
    current_index = {"val": 0}  # use dict to keep mutable reference
    states = []

    # Algorithms list
    search_algorithms = [
        "BFS",
        "DFS",
        "UCS",
        "Greedy",
        "A*"
    ]

    # Frame to hold the radio buttons
    radio_frame = tk.Frame(root)
    radio_frame.pack(side="left", padx=20, pady=20, anchor="n")

    # Add radio buttons for each algorithm
    for alg in search_algorithms:
        rb = tk.Radiobutton(
            radio_frame,
            text=alg,
            variable=alg_var,
            value=alg,
            command=get_selected_alg
        )
        rb.pack(anchor="w")

    # Label to show current selection
    selection_label = tk.Label(radio_frame, text=f"Selected: {alg_var.get()}")
    selection_label.pack(pady=10)


    # Puzzle name label
    name_label = tk.Label(root, text=f"Puzzle Display: {name}", font=("Arial", 18))
    name_label.pack(pady=10)

    # Frame to hold the grid
    grid_frame = tk.Frame(root)
    grid_frame.pack(pady=10)

    # Counter label
    counter_label = tk.Label(root, text="", font=("Arial", 14))
    counter_label.pack(pady=5)

    def run_selected():
        global states  # global placeholder
        alg = alg_var.get()   # "BFS", "DFS", etc.
        print(f"Running {alg}...")

        # Call C++ function
        try:
            states = pathfinder.find_path(start, goal, alg)
            print(f"States returned: {len(states)}")
            current_index["val"] = 0   # reset index
            show_state()
        except Exception as e:
            print(f"Error running {alg}: {e}")

    # Button to run algorithm
    runAlgorithm = tk.Button(root, text="Run Algorithm", command=run_selected)
    runAlgorithm.pack(pady=10)

    
    def next_state():
        if current_index["val"] < len(states)-1:
            current_index["val"] += 1
            show_state()

    def prev_state():
        if current_index["val"] > 0:
            current_index["val"] -= 1
            show_state()

    def goal_state():
        current_index["val"] = len(states) - 1
        show_state()
        
    def start_state():
        current_index["val"] = 0
        show_state()



    # Navigation buttons
    nav_frame = tk.Frame(root)
    nav_frame.pack(pady=10)

    prev_btn = tk.Button(nav_frame, text="Previous", command=prev_state)
    prev_btn.grid(row=0, column=0, padx=10)

    next_btn = tk.Button(nav_frame, text="Next", command=next_state)
    next_btn.grid(row=0, column=1, padx=10)

    start_btn = tk.Button(nav_frame, text="Jump to start", command=start_state)
    start_btn.grid(row=1, column=0, padx=10)

    end_btn = tk.Button(nav_frame, text="Jump to goal", command=goal_state)
    end_btn.grid(row=1, column=1, padx=10)

    # Show the first state
    show_state()

    root.mainloop()
