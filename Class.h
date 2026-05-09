#pragma once
#include <iostream>
#include <string>
using namespace std;

// ─────────────────────────────────────────────
//               Enumerations
// ─────────────────────────────────────────────
enum class Color { WHITE, BLACK, NONE };
enum class PieceType { PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, EMPTY };

// ─────────────────────────────────────────────
//             Position helper struct
// ─────────────────────────────────────────────
struct Position {
    int row, col;
    Position(int r = -1, int c = -1) : row(r), col(c) {}
    bool isValid()                    const { return row >= 0 && row < 8 && col >= 0 && col < 8; }
    bool operator==(const Position& other) const { return row == other.row && col == other.col; }
};

// ─────────────────────────────────────────────
//           Custom dynamic array 
// ─────────────────────────────────────────────
class MoveList {
private:
    Position* moves_;
    int       count_;
    int       capacity_;
public:
    MoveList();
    MoveList(const MoveList& other);
    MoveList& operator=(const MoveList& other);
    ~MoveList();

    void            add(const Position& pos);
    int             size()      const;
    void            clear();
    Position& operator[](int idx);
    const Position& operator[](int idx) const;
};

// ─────────────────────────────────────────────
//             Forward declaration
// ─────────────────────────────────────────────
class Board;

// ─────────────────────────────────────────────
//  Abstract base class – Piece
// ─────────────────────────────────────────────
class Piece {
protected:
    Color     sideColor_;
    PieceType pieceType_;
    bool      everMoved_;
public:
    Piece(Color side, PieceType type);
    virtual ~Piece() {}

    Color     getColor()      const;
    PieceType getType()       const;
    bool      hasMoved()      const;
    void      markMoved(bool moved);

    virtual MoveList getCandidateMoves(const Position& square, const Board& board) const = 0;
    virtual char     getSymbol()                                                   const = 0;
};

// ─────────────────────────────────────────────
//            Concrete piece classes
// ─────────────────────────────────────────────
class Pawn : public Piece {
public:
    explicit Pawn(Color side);
    MoveList getCandidateMoves(const Position& square, const Board& board) const override;
    char     getSymbol() const override;
};

class Rook : public Piece {
public:
    explicit Rook(Color side);
    MoveList getCandidateMoves(const Position& square, const Board& board) const override;
    char     getSymbol() const override;
};

class Knight : public Piece {
public:
    explicit Knight(Color side);
    MoveList getCandidateMoves(const Position& square, const Board& board) const override;
    char     getSymbol() const override;
};

class Bishop : public Piece {
public:
    explicit Bishop(Color side);
    MoveList getCandidateMoves(const Position& square, const Board& board) const override;
    char     getSymbol() const override;
};

class Queen : public Piece {
public:
    explicit Queen(Color side);
    MoveList getCandidateMoves(const Position& square, const Board& board) const override;
    char     getSymbol() const override;
};

class King : public Piece {
public:
    explicit King(Color side);
    MoveList getCandidateMoves(const Position& square, const Board& board) const override;
    char     getSymbol() const override;
};

// ─────────────────────────────────────────────
//  Board  (Composition: owns all Piece objects)
// ─────────────────────────────────────────────
class Board {
private:
    Piece* squares_[8][8];
    void   clearBoard();
    void   setupPieces();
public:
    Board();
    ~Board();
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

    Piece* getPieceAt(const Position& pos)              const;
    Piece* getPieceAt(int row, int col)                 const;
    bool   isSquareEmpty(const Position& pos)           const;
    bool   isSquareEnemy(const Position& pos, Color side) const;

    void movePiece(const Position& from, const Position& to);
    void placePiece(const Position& pos, Piece* newPiece);

    // Template: simulate a move, run callable, then restore
    template <typename Func>
    void tryMove(const Position& from, const Position& to, Func callback) {
        Piece* moving = squares_[from.row][from.col];
        Piece* captured = squares_[to.row][to.col];
        squares_[to.row][to.col] = moving;
        squares_[from.row][from.col] = nullptr;
        callback();
        squares_[from.row][from.col] = moving;
        squares_[to.row][to.col] = captured;
    }

    MoveList getLegalMoves(const Position& square, Color side) const;
    bool     isKingInCheck(Color side)                         const;
    bool     playerHasLegalMove(Color side)                    const;
    Position findKingSquare(Color side)                        const;

    void reset();
    void draw(const string& whiteName, const string& blackName) const;
};

// ─────────────────────────────────────────────
//  Game – controls the full game loop
// ─────────────────────────────────────────────
class Game {
private:
    Board  board_;
    string whiteName_;
    string blackName_;
    Color  activeColor_;
    bool   isOver_;

    Color    getOpponent(Color side)                    const;
    Position parseInput(const string& input, bool& ok)  const;
    string   squareToString(const Position& pos)        const;
    void     handlePromotion(const Position& pos);
    void     passTurn();
public:
    Game();
    void setPlayers(const string& white, const string& black);
    void run();
};

// ─────────────────────────────────────────────
//               UI helpers
// ─────────────────────────────────────────────
void clearScreen();
void showMainMenu();
bool confirmExit();
void collectPlayerNames(string& white, string& black);
