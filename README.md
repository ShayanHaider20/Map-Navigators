
# A* Pathfinding Algorithm with Multithreading

## Overview
This project implements the **A* (A-Star) pathfinding algorithm** in **C** with **multithreading** using `pthread`. The program searches for the shortest path between a source and destination point on a **XxY grid**, which contains randomly generated obstacles. The grid is processed by multiple threads to speed up the pathfinding process.

## Features
- Implements **A* Search Algorithm** for pathfinding.
- Uses **multithreading** to parallelize the search process.
- Generates a **random grid** with obstacles.
- Allows users to **input source and destination coordinates**.
- Visualizes the **shortest path** using `+` symbols.
- Supports **thread synchronization** using `pthread_mutex`.

## Grid Representation
| Symbol | Meaning |
|--------|---------|
| `X`    | Obstacle |
| `S`    | Start Position |
| `D`    | Destination |
| `.`    | Unexplored Path |
| `+`    | Shortest Path Found |

## Compilation and Execution
### **Requirements**
- GCC Compiler
- POSIX Threads (pthreads)
- Windows System (`windows.h` used)

### **Compilation Command**
```bash
gcc -o astar_multithreaded astar_multithreaded.c -lpthread
```

### **Running the Program**
```bash
./astar_multithreaded
```

### **User Input**
The program will prompt the user to enter:
- **Source Row & Column** (Starting position)
- **Destination Row & Column** (Target position)

Example Input:
```
Enter the source row: 5
Enter the source col: 3
Enter the destination row: 20
Enter the destination column: 50
```

## Implementation Details
### **1. Data Structures**
- **Grid (2D Array):** Represents the search space.
- **Node Struct:** Stores row, column, f-score, g-score, and parent pointer.
- **Open List (Array of Nodes):** Stores nodes to be explored next.
- **Visited List (2D Array):** Tracks explored nodes.

### **2. Algorithm Flow**
1. **Grid Initialization**: Random obstacles are placed.
2. **User Input**: Source and Destination are defined.
3. **Start Node Creation**: Initialized with `g=0`, `f=h(source, destination)`.
4. **Threads Execution**:
   - Each thread picks the best node from the open list.
   - Expands valid neighbors and updates scores.
   - Stops when the destination is reached.
5. **Path Reconstruction**: Traces back from destination to source.
6. **Final Grid Display**: Shows the shortest path.

## Example Output
```
Shortest path(Represented by +) is:

. . . X X X X . . .
. . . X S . . . X .
. . . . . + + + + D
```

## Author
Developed by Shayan Haider

