Class layout
classes/
  Connect4.h / Connect4.cpp   <-- new game class
Game.h / Game.cpp             <-- base game framework
Grid.h / Grid.cpp             <-- grid & square helpers
Application.cpp               <-- menu, app loop, render/update glue
TicTacToe.h / TicTacToe.cpp   <-- reference for Bit creation/placement patterns

Inheritance: Connect4 : public Game
Uses the same lifecycle hooks and UI behavior as existing games.

Board model:
Internally stores the board as a flat vector _board of size 7 * 6 with values {0=empty, 1=P1, 2=P2} plus a _heights[7] array tracking how many pieces are in each column (from bottom). This makes “drop into a column” O(1) and enables very fast AI simulation (make/undo moves).

Rendering:
Uses Grid to create a 7×6 layout (initializeSquares), then spawns Bit objects for yellow/red pieces. Bit creation mirrors TicTacToe:
new Bit()
LoadTextureFromFile("yellow.png" / "red.png")
setOwner(getPlayerAt(0 or 1))
square->setBit(bit)

Input:
Override actionForEmptyHolder(BitHolder& holder). Clicking any square in a column translates to that column index. The piece is added to the lowest open slot in that column. Drag-moving is disabled (Connect 4 doesn’t move pieces once placed).

Turn flow/game over:
After each placement, call endTurn(). Winner/draw is checked via the usual framework path. Guard both human input and AI updates so no more moves can be made after game over—only Reset remains active.

Win/Draw detection:
Efficient scans over 4-cell windows for the current tag (1/2) across:
Horizontal windows x..x+3
Vertical windows y..y+3
Diagonal / and \ windows

AI (minimax + heuristic):
Depth-limited minimax (default depth 5) with α-β pruning. Heuristic favors:
Center column occupancy (slight bias)
“Windows” where a 4-cell segment has 2-in-a-row (open), 3-in-a-row (one open), or 4-in-a-row (win)
AI score minus opponent score for balance

State save/restore:
stateString() encodes the board as 42 characters ('0'..'2'), row-major from top. setStateString() rebuilds both memory and visuals (destroy bits, then re-spawn based on the chars). This lets you serialize/deserialize games cleanly.

Cleanup:
stopGame() destroys all bits on squares and deletes the grid. The destructor also releases the grid .
