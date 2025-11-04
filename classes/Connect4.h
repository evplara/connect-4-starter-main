#pragma once
#include "Game.h"
#include "Grid.h"
#include <vector>
#include <string>
#include <climits>

class Connect4 : public Game {
public:
    Connect4();
    ~Connect4();

    // Game lifecycle
    void setUpBoard() override;
    void stopGame() override;

    // Input & rules (no dragging in Connect 4)
    bool canBitMoveFrom(Bit &bit, BitHolder &src) override { return false; }
    bool canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override { return false; }
    bool actionForEmptyHolder(BitHolder &holder) override;

    // AI
    bool gameHasAI() override { return true; }
    void updateAI() override;

    // Win/draw & state
    Player* checkForWinner() override;
    bool    checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;

    Grid* getGrid() override { return _grid; }

private:
    // Board & helpers
    Grid* _grid = nullptr;
    const int _cols = 7;
    const int _rows = 6;
    float _square = 80.0f;

    // Flat board 
    std::vector<int> _board;
    std::vector<int> _heights;

    inline int idx(int x, int y) const { return y * _cols + x; }
    int topOpenRowInColumn(int col) const;
    int tagAt(int x, int y) const;
    void setTagAt(int x, int y, int tag, bool animate);
    bool isWinningTag(int tag) const;
    bool anyMovesLeft() const;

    bool holderToXY(BitHolder& holder, int& outX, int& outY) const;

    // Bit construction/placement (mirrors TicTacToe)
    Bit* makePieceForTag(int tag);
    void placeBitOnSquare(Bit* piece, ChessSquare* sq, bool animate);

    // AI (minimax with a simple heuristic)
    int  findBestAIMove(int aiTag, int depth, int &outScore);
    int  minimax(int depth, int alpha, int beta, bool maximizing, int aiTag);
    int  scorePosition(int tag) const;
    void makeMove(int col, int tag);
    void undoMove(int col);
};
