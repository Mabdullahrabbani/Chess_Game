#pragma once
#include <iostream>
#include <string>
using namespace std;

enum class Color { WHITE, BLACK, NONE };
enum class PieceType { PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, EMPTY };

struct Position {
    int row, col;
    Position(int r = -1, int c = -1) : row(r), col(c) {}
    bool isValid()                     const { return row >= 0 && row < 8 && col >= 0 && col < 8; }
    bool operator==(const Position& o) const { return row == o.row && col == o.col; }
};

class MoveList {
    Position moves_[64];
    int      count_ = 0;
public:
    void            add(const Position& p) { if (count_ < 64) moves_[count_++] = p; }
    int             size()                 const { return count_; }
    void            clear() { count_ = 0; }
    Position& operator[](int i) { return moves_[i]; }
    const Position& operator[](int i)     const { return moves_[i]; }
};

class Board;

class Piece {
protected:
    Color     sideColor_;
    PieceType pieceType_;
    bool      everMoved_ = false;
public:
    Piece(Color side, PieceType type) : sideColor_(side), pieceType_(type) {}
    virtual ~Piece() {}

    Color     getColor() const { return sideColor_; }
    PieceType getType()  const { return pieceType_; }
    bool      hasMoved() const { return everMoved_; }
    void      markMoved(bool v) { everMoved_ = v; }

    virtual MoveList getCandidateMoves(const Position& sq, const Board& board) const = 0;
    virtual char     getSymbol() const = 0;
};

class Pawn : public Piece { public: explicit Pawn(Color s);   MoveList getCandidateMoves(const Position&, const Board&) const override; char getSymbol() const override; };
class Rook : public Piece { public: explicit Rook(Color s);   MoveList getCandidateMoves(const Position&, const Board&) const override; char getSymbol() const override; };
class Knight : public Piece { public: explicit Knight(Color s); MoveList getCandidateMoves(const Position&, const Board&) const override; char getSymbol() const override; };
class Bishop : public Piece { public: explicit Bishop(Color s); MoveList getCandidateMoves(const Position&, const Board&) const override; char getSymbol() const override; };
class Queen : public Piece { public: explicit Queen(Color s);  MoveList getCandidateMoves(const Position&, const Board&) const override; char getSymbol() const override; };
class King : public Piece { public: explicit King(Color s);   MoveList getCandidateMoves(const Position&, const Board&) const override; char getSymbol() const override; };

class Board {
    Piece* squares_[8][8];
    void   clearBoard();
    void   setupPieces();
public:
    Board();
    ~Board();
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

    Piece* getPieceAt(const Position& pos)                const;
    Piece* getPieceAt(int row, int col)                   const;
    bool   isSquareEmpty(const Position& pos)             const;
    bool   isSquareEnemy(const Position& pos, Color side) const;
    void   movePiece(const Position& from, const Position& to);
    void   placePiece(const Position& pos, Piece* p);

    template<typename Func>
    void tryMove(const Position& from, const Position& to, Func fn) {
        Piece* mv = squares_[from.row][from.col];
        Piece* cap = squares_[to.row][to.col];
        squares_[to.row][to.col] = mv;
        squares_[from.row][from.col] = nullptr;
        fn();
        squares_[from.row][from.col] = mv;
        squares_[to.row][to.col] = cap;
    }

    MoveList getLegalMoves(const Position& sq, Color side) const;
    bool     isKingInCheck(Color side)                     const;
    bool     playerHasLegalMove(Color side)                const;
    Position findKingSquare(Color side)                    const;
    void     reset();
    void     draw(const string& wName, const string& bName) const;
};

class Game {
    Board  board_;
    string whiteName_, blackName_;
    Color  activeColor_ = Color::WHITE;
    bool   isOver_ = false;

    Color    getOpponent(Color side)                const;
    Position parseInput(const string& in, bool& ok) const;
    string   squareToString(const Position& pos)    const;
    void     handlePromotion(const Position& pos);
    void     passTurn();
public:
    Game() = default;
    void setPlayers(const string& white, const string& black);
    void run();
};

void clearScreen();
void showMainMenu();
bool confirmExit();
void collectPlayerNames(string& white, string& black);
void enableAnsiColors();
