#include "Connect4.h"
#include "../Application.h"
#include <algorithm>

Connect4::Connect4() {
    setNumberOfPlayers(2);
    _board.assign(_cols * _rows, 0);
    _heights.assign(_cols, 0);

    // setAIPlayer(1);
}

Connect4::~Connect4() { stopGame(); }

// setup/teardown
void Connect4::setUpBoard() {
    if (_grid) { delete _grid; _grid = nullptr; }
    _grid = new Grid(_cols, _rows);

    _grid->initializeSquares(_square, "square.png");

    std::fill(_board.begin(), _board.end(), 0);
    std::fill(_heights.begin(), _heights.end(), 0);

    startGame();  
}

void Connect4::stopGame() {
    if (!_grid) return;
    _grid->forEachSquare([](ChessSquare* sq, int, int){
        sq->destroyBit();
    });
    delete _grid; _grid = nullptr;
}

bool Connect4::holderToXY(BitHolder& holder, int& outX, int& outY) const {
    ChessSquare* target = dynamic_cast<ChessSquare*>(&holder);
    if (!target) return false;
    for (int y=0; y<_rows; ++y) {
        for (int x=0; x<_cols; ++x) {
            if (_grid->getSquare(x,y) == target) { outX = x; outY = y; return true; }
        }
    }
    return false;
}

int Connect4::topOpenRowInColumn(int col) const {
    if (col < 0 || col >= _cols) return -1;
    int filled = _heights[col];        // # of pieces in this column (from bottom)
    if (filled >= _rows) return -1;
    // grid has y=0 at top lowest open visual row is (_rows-1 - filled)
    return (_rows - 1) - filled;
}

int Connect4::tagAt(int x, int y) const {
    if (x<0 || x>=_cols || y<0 || y>=_rows) return 0;
    return _board[idx(x,y)];
}

Bit* Connect4::makePieceForTag(int tag) {
    Bit* bit = new Bit();
    bit->LoadTextureFromFile(tag == 1 ? "yellow.png" : "red.png");
    bit->setOwner(getPlayerAt(tag == 1 ? 0 : 1)); 
    return bit;
}

void Connect4::placeBitOnSquare(Bit* piece, ChessSquare* sq, bool animate) {
    if (!piece || !sq) return;

    piece->setPosition(sq->getPosition());          
    sq->setBit(piece);                             

    
}

void Connect4::setTagAt(int x, int y, int tag, bool animate) {
    _board[idx(x,y)] = tag;
    // update column fill from bottom
    int used = _heights[x];
    _heights[x] = std::max(_heights[x], used+1);

    ChessSquare* sq = _grid->getSquare(x,y);
    if (!sq) return;

    Bit* p = makePieceForTag(tag);
    placeBitOnSquare(p, sq, animate);
}

bool Connect4::actionForEmptyHolder(BitHolder &holder) {
	if (checkForWinner() || checkForDraw())
        return false;

    int hx=-1, hy=-1;
    if (!holderToXY(holder, hx, hy)) return false;

    int col = hx;
    int row = topOpenRowInColumn(col);
    if (row < 0) return false;                

    int tag = (getCurrentPlayer()->playerNumber() == 0) ? 1 : 2;
    setTagAt(col, row, tag, /*animate=*/true);
    endTurn();                                 
    return true;
}

// win/draw
bool Connect4::isWinningTag(int tag) const {
    //horizontal
    for (int y=0; y<_rows; ++y)
        for (int x=0; x<_cols-3; ++x)
            if (tagAt(x,y)==tag && tagAt(x+1,y)==tag && tagAt(x+2,y)==tag && tagAt(x+3,y)==tag) return true;
    //vertical
    for (int x=0; x<_cols; ++x)
        for (int y=0; y<_rows-3; ++y)
            if (tagAt(x,y)==tag && tagAt(x,y+1)==tag && tagAt(x,y+2)==tag && tagAt(x,y+3)==tag) return true;
    //diagonal '/'
    for (int y=0; y<_rows-3; ++y)
        for (int x=0; x<_cols-3; ++x)
            if (tagAt(x,y)==tag && tagAt(x+1,y+1)==tag && tagAt(x+2,y+2)==tag && tagAt(x+3,y+3)==tag) return true;
    //diagonal '\'
    for (int y=0; y<_rows-3; ++y)
        for (int x=3; x<_cols; ++x)
            if (tagAt(x,y)==tag && tagAt(x-1,y+1)==tag && tagAt(x-2,y+2)==tag && tagAt(x-3,y+3)==tag) return true;
    return false;
}

Player* Connect4::checkForWinner() {
    for (int tag = 1; tag <= 2; ++tag) {
        if (isWinningTag(tag)) {
            _winner = getPlayerAt(tag == 1 ? 0 : 1);
            return _winner;
        }
    }
    return nullptr;
}

bool Connect4::anyMovesLeft() const {
    for (int c=0; c<_cols; ++c) if (_heights[c] < _rows) return true;
    return false;
}

bool Connect4::checkForDraw() {
    if (checkForWinner()) return false;
    return !anyMovesLeft();
}

std::string Connect4::initialStateString() {
    return std::string(_cols * _rows, '0');
}

std::string Connect4::stateString() {
    std::string s; s.reserve(_cols * _rows);
    for (int y=0; y<_rows; ++y)
        for (int x=0; x<_cols; ++x)
            s.push_back(char('0' + _board[idx(x,y)]));
    return s;
}

void Connect4::setStateString(const std::string &s) {
    if ((int)s.size() != _cols*_rows) return;

    // Clear board visuals
    _grid->forEachSquare([](ChessSquare* sq, int, int){ sq->destroyBit(); });
    std::fill(_board.begin(), _board.end(), 0);
    std::fill(_heights.begin(), _heights.end(), 0);

    for (int y=0; y<_rows; ++y) {
        for (int x=0; x<_cols; ++x) {
            int t = s[y*_cols + x] - '0';
            _board[idx(x,y)] = t;
            if (t != 0) {
                // increase column height
                _heights[x] = std::max(_heights[x], (_rows - y));
                ChessSquare* sq = _grid->getSquare(x,y);
                if (sq) placeBitOnSquare(makePieceForTag(t), sq, /*animate=*/false);
            }
        }
    }
}

// AI
void Connect4::updateAI() {
	if (checkForWinner() || checkForDraw())
        return;
		
    int currentTag = (getCurrentPlayer()->playerNumber() == 0) ? 1 : 2;
    int bestScore = 0;
    int bestCol = findBestAIMove(currentTag, /*depth=*/5, bestScore);
    if (bestCol >= 0) {
        int row = topOpenRowInColumn(bestCol);
        if (row >= 0) {
            setTagAt(bestCol, row, currentTag, /*animate=*/true);
            endTurn();
        }
    }
}

int Connect4::findBestAIMove(int aiTag, int depth, int &outScore) {
    int bestCol = -1, best = INT_MIN;
    for (int c=0; c<_cols; ++c) {
        if (_heights[c] >= _rows) continue;
        makeMove(c, aiTag);
        int sc = minimax(depth-1, INT_MIN, INT_MAX, /*max*/false, aiTag);
        undoMove(c);
        if (sc > best) { best = sc; bestCol = c; }
    }
    outScore = best;
    return bestCol;
}

int Connect4::minimax(int depth, int alpha, int beta, bool maximizing, int aiTag) {
    int opp = (aiTag==1?2:1);
    if (isWinningTag(aiTag)) return 100000 + depth;
    if (isWinningTag(opp))   return -100000 - depth;
    if (depth==0 || !anyMovesLeft()) return scorePosition(aiTag);

    if (maximizing) {
        int best = INT_MIN;
        for (int c=0; c<_cols; ++c) {
            if (_heights[c] >= _rows) continue;
            makeMove(c, aiTag);
            best = std::max(best, minimax(depth-1, alpha, beta, false, aiTag));
            undoMove(c);
            alpha = std::max(alpha, best);
            if (beta <= alpha) break;
        }
        return best;
    } else {
        int best = INT_MAX;
        for (int c=0; c<_cols; ++c) {
            if (_heights[c] >= _rows) continue;
            makeMove(c, opp);
            best = std::min(best, minimax(depth-1, alpha, beta, true, aiTag));
            undoMove(c);
            beta = std::min(beta, best);
            if (beta <= alpha) break;
        }
        return best;
    }
}

int Connect4::scorePosition(int tag) const {
    int opp = (tag==1?2:1);
    auto scoreFor = [&](int t)->int{
        int sc = 0;
        auto w4 = [&](int a,int b,int c,int d){
            int cnt = (a==t)+(b==t)+(c==t)+(d==t);
            int empty = (a==0)+(b==0)+(c==0)+(d==0);
            if (cnt==4) sc += 10000;
            else if (cnt==3 && empty==1) sc += 100;
            else if (cnt==2 && empty==2) sc += 10;
        };
        // horiz
        for (int y=0;y<_rows;++y) for (int x=0;x<_cols-3;++x) w4(tagAt(x,y),tagAt(x+1,y),tagAt(x+2,y),tagAt(x+3,y));
        // vert
        for (int x=0;x<_cols;++x) for (int y=0;y<_rows-3;++y) w4(tagAt(x,y),tagAt(x,y+1),tagAt(x,y+2),tagAt(x,y+3));
        // diag /
        for (int y=0;y<_rows-3;++y) for (int x=0;x<_cols-3;++x) w4(tagAt(x,y),tagAt(x+1,y+1),tagAt(x+2,y+2),tagAt(x+3,y+3));
        // diag '\'
        for (int y=0;y<_rows-3;++y) for (int x=3;x<_cols;++x)   w4(tagAt(x,y),tagAt(x-1,y+1),tagAt(x-2,y+2),tagAt(x-3,y+3));
        // center bias
        for (int y=0;y<_rows;++y) if (tagAt(_cols/2,y)==t) sc += 3;
        return sc;
    };
    return scoreFor(tag) - scoreFor(opp);
}

void Connect4::makeMove(int col, int tag) {
    int h = _heights[col];
    int y = (_rows - 1) - h;
    _board[idx(col,y)] = tag;
    _heights[col] = h + 1;
}

void Connect4::undoMove(int col) {
    int h = _heights[col];
    if (h<=0) return;
    int y = (_rows - 1) - (h-1);
    _board[idx(col,y)] = 0;
    _heights[col] = h - 1;
}
