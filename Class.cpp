#include "Class.h"
#include <cstdlib>

// ═══════════════════════════════════════════════════════════════
//  MoveList  (It Is custom dynamic array)
// ═══════════════════════════════════════════════════════════════
MoveList::MoveList() : moves_(nullptr), count_(0), capacity_(0) {}

MoveList::MoveList(const MoveList& other) : moves_(nullptr), count_(0), capacity_(0) {
    for (int i = 0; i < other.count_; ++i) add(other.moves_[i]);
}

MoveList& MoveList::operator=(const MoveList& other) {
    if (this == &other) return *this;
    delete[] moves_;
    moves_ = nullptr; count_ = 0; capacity_ = 0;
    for (int i = 0; i < other.count_; ++i) add(other.moves_[i]);
    return *this;
}

MoveList::~MoveList() { delete[] moves_; }

void MoveList::add(const Position& pos) {
    if (count_ == capacity_) {
        int       newCap = (capacity_ == 0) ? 4 : capacity_ * 2;
        Position* temp = new Position[newCap];
        for (int i = 0; i < count_; ++i) 
            temp[i] = moves_[i];
        delete[] moves_;
        moves_ = temp;
        capacity_ = newCap;
    }
    moves_[count_++] = pos;
}

int             MoveList::size()          const { return count_; }
void            MoveList::clear() { count_ = 0; }
Position& MoveList::operator[](int idx) { return moves_[idx]; }
const Position& MoveList::operator[](int idx) const { return moves_[idx]; }

// ═══════════════════════════════════════════════════════════════
//                    Piece base Class 
// ═══════════════════════════════════════════════════════════════
Piece::Piece(Color side, PieceType type)
    : sideColor_(side), pieceType_(type), everMoved_(false) {
}

Color     Piece::getColor()        const { return sideColor_; }
PieceType Piece::getType()         const { return pieceType_; }
bool      Piece::hasMoved()        const { return everMoved_; }
void      Piece::markMoved(bool v) { everMoved_ = v; }

// ═══════════════════════════════════════════════════════════════
//                      Pawn Class 
// ═══════════════════════════════════════════════════════════════
Pawn::Pawn(Color side) : Piece(side, PieceType::PAWN) {}
char Pawn::getSymbol() const { return (sideColor_ == Color::WHITE) ? 'P' : 'p'; }

MoveList Pawn::getCandidateMoves(const Position& square, const Board& board) const {
    MoveList result;
    int advance = (sideColor_ == Color::WHITE) ? -1 : 1;

    Position oneStep(square.row + advance, square.col);
    if (oneStep.isValid() && board.isSquareEmpty(oneStep)) {
        result.add(oneStep);
        bool onStartRank = (sideColor_ == Color::WHITE) ? (square.row == 6)
            : (square.row == 1);
        Position twoStep(square.row + 2 * advance, square.col);
        if (onStartRank && twoStep.isValid() && board.isSquareEmpty(twoStep))
            result.add(twoStep);
    }
    int sideCols[2] = { -1, 1 };
    for (int i = 0; i < 2; ++i) {
        Position diag(square.row + advance, square.col + sideCols[i]);
        if (diag.isValid() && board.isSquareEnemy(diag, sideColor_))
            result.add(diag);
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════
//                       Rook Class 
// ═══════════════════════════════════════════════════════════════
Rook::Rook(Color side) : Piece(side, PieceType::ROOK) {}
char Rook::getSymbol() const { return (sideColor_ == Color::WHITE) ? 'R' : 'r'; }

MoveList Rook::getCandidateMoves(const Position& square, const Board& board) const {
    MoveList result;
    int directions[4][2] = { {-1,0},{1,0},{0,-1},{0,1} };
    for (int d = 0; d < 4; ++d) {
        Position cur(square.row + directions[d][0], square.col + directions[d][1]);
        while (cur.isValid()) {
            if (board.isSquareEmpty(cur)) { result.add(cur); }
            else { if (board.isSquareEnemy(cur, sideColor_)) result.add(cur); break; }
            cur.row += directions[d][0];
            cur.col += directions[d][1];
        }
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════
//                        Knight Class 
// ═══════════════════════════════════════════════════════════════
Knight::Knight(Color side) : Piece(side, PieceType::KNIGHT) {}
char Knight::getSymbol() const { return (sideColor_ == Color::WHITE) ? 'N' : 'n'; }

MoveList Knight::getCandidateMoves(const Position& square, const Board& board) const {
    MoveList result;
    int leaps[8][2] = { {-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1} };
    for (int i = 0; i < 8; ++i) {
        Position dest(square.row + leaps[i][0], square.col + leaps[i][1]);
        if (dest.isValid() && (board.isSquareEmpty(dest) || board.isSquareEnemy(dest, sideColor_)))
            result.add(dest);
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════
//                      Bishop Class 
// ═══════════════════════════════════════════════════════════════
Bishop::Bishop(Color side) : Piece(side, PieceType::BISHOP) {}
char Bishop::getSymbol() const { return (sideColor_ == Color::WHITE) ? 'B' : 'b'; }

MoveList Bishop::getCandidateMoves(const Position& square, const Board& board) const {
    MoveList result;
    int directions[4][2] = { {-1,-1},{-1,1},{1,-1},{1,1} };
    for (int d = 0; d < 4; ++d) {
        Position cur(square.row + directions[d][0], square.col + directions[d][1]);
        while (cur.isValid()) {
            if (board.isSquareEmpty(cur)) { result.add(cur); }
            else { if (board.isSquareEnemy(cur, sideColor_)) result.add(cur); break; }
            cur.row += directions[d][0];
            cur.col += directions[d][1];
        }
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════
//                      Queen Class 
// ═══════════════════════════════════════════════════════════════
Queen::Queen(Color side) : Piece(side, PieceType::QUEEN) {}
char Queen::getSymbol() const { return (sideColor_ == Color::WHITE) ? 'Q' : 'q'; }

MoveList Queen::getCandidateMoves(const Position& square, const Board& board) const {
    MoveList result;
    int directions[8][2] = { {-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1} };
    for (int d = 0; d < 8; ++d) {
        Position cur(square.row + directions[d][0], square.col + directions[d][1]);
        while (cur.isValid()) {
            if (board.isSquareEmpty(cur)) { result.add(cur); }
            else { if (board.isSquareEnemy(cur, sideColor_)) result.add(cur); break; }
            cur.row += directions[d][0];
            cur.col += directions[d][1];
        }
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════
//                        King Class 
// ═══════════════════════════════════════════════════════════════
King::King(Color side) : Piece(side, PieceType::KING) {}
char King::getSymbol() const { return (sideColor_ == Color::WHITE) ? 'K' : 'k'; }

MoveList King::getCandidateMoves(const Position& square, const Board& board) const {
    MoveList result;
    for (int dr = -1; dr <= 1; ++dr)
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            Position dest(square.row + dr, square.col + dc);
            if (dest.isValid() && (board.isSquareEmpty(dest) || board.isSquareEnemy(dest, sideColor_)))
                result.add(dest);
        }
    return result;
}

// ═══════════════════════════════════════════════════════════════
//                        Board Class 
// ═══════════════════════════════════════════════════════════════
Board::Board() { clearBoard(); setupPieces(); }

Board::~Board() {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            delete squares_[r][c];
}

void Board::clearBoard() {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            squares_[r][c] = nullptr;
}

void Board::setupPieces() {
    // Black back rank (row 0)
    squares_[0][0] = new Rook(Color::BLACK);   squares_[0][1] = new Knight(Color::BLACK);
    squares_[0][2] = new Bishop(Color::BLACK); squares_[0][3] = new Queen(Color::BLACK);
    squares_[0][4] = new King(Color::BLACK);   squares_[0][5] = new Bishop(Color::BLACK);
    squares_[0][6] = new Knight(Color::BLACK); squares_[0][7] = new Rook(Color::BLACK);
    for (int c = 0; c < 8; ++c) squares_[1][c] = new Pawn(Color::BLACK);

    // White back rank (row 7)
    squares_[7][0] = new Rook(Color::WHITE);   squares_[7][1] = new Knight(Color::WHITE);
    squares_[7][2] = new Bishop(Color::WHITE); squares_[7][3] = new Queen(Color::WHITE);
    squares_[7][4] = new King(Color::WHITE);   squares_[7][5] = new Bishop(Color::WHITE);
    squares_[7][6] = new Knight(Color::WHITE); squares_[7][7] = new Rook(Color::WHITE);
    for (int c = 0; c < 8; ++c) squares_[6][c] = new Pawn(Color::WHITE);
}

void Board::reset() {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) { delete squares_[r][c]; squares_[r][c] = nullptr; }
    setupPieces();
}

Piece* Board::getPieceAt(const Position& pos) const {
    if (!pos.isValid()) return nullptr;
    return squares_[pos.row][pos.col];
}
Piece* Board::getPieceAt(int row, int col) const { return getPieceAt(Position(row, col)); }

bool Board::isSquareEmpty(const Position& pos) const {
    return pos.isValid() && !squares_[pos.row][pos.col];
}
bool Board::isSquareEnemy(const Position& pos, Color side) const {
    if (!pos.isValid()) return false;
    Piece* pc = squares_[pos.row][pos.col];
    return pc && pc->getColor() != side;
}

void Board::movePiece(const Position& from, const Position& to) {
    delete squares_[to.row][to.col];
    squares_[to.row][to.col] = squares_[from.row][from.col];
    squares_[from.row][from.col] = nullptr;
    if (squares_[to.row][to.col])
        squares_[to.row][to.col]->markMoved(true);
}

void Board::placePiece(const Position& pos, Piece* newPiece) {
    delete squares_[pos.row][pos.col];
    squares_[pos.row][pos.col] = newPiece;
}

Position Board::findKingSquare(Color side) const {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (squares_[r][c] &&
                squares_[r][c]->getType() == PieceType::KING &&
                squares_[r][c]->getColor() == side)
                return Position(r, c);
    return Position(-1, -1);
}

bool Board::isKingInCheck(Color side) const {
    Position kingPos = findKingSquare(side);
    Color    enemy = (side == Color::WHITE) ? Color::BLACK : Color::WHITE;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            Piece* attacker = squares_[r][c];
            if (!attacker || attacker->getColor() != enemy) continue;
            MoveList threats = attacker->getCandidateMoves(Position(r, c), *this);
            for (int i = 0; i < threats.size(); ++i)
                if (threats[i] == kingPos) return true;
        }
    return false;
}

MoveList Board::getLegalMoves(const Position& square, Color side) const {
    MoveList legal;
    Piece* piece = getPieceAt(square);
    if (!piece || piece->getColor() != side) return legal;

    MoveList candidates = piece->getCandidateMoves(square, *this);
    Board& self = const_cast<Board&>(*this);

    for (int i = 0; i < candidates.size(); ++i) {
        bool leavesKingInCheck = false;
        self.tryMove(square, candidates[i], [&]() {
            leavesKingInCheck = self.isKingInCheck(side);
            });
        if (!leavesKingInCheck) legal.add(candidates[i]);
    }
    return legal;
}

bool Board::playerHasLegalMove(Color side) const {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            Piece* piece = squares_[r][c];
            if (!piece || piece->getColor() != side) continue;
            MoveList moves = getLegalMoves(Position(r, c), side);
            if (moves.size() > 0) return true;
        }
    return false;
}

// ═══════════════════════════════════════════════════════════════
//                    Board DisplayIng Code 
// ═══════════════════════════════════════════════════════════════
void Board::draw(const string& whiteName, const string& blackName) const {
    cout << "\n";
    cout << "  [ " << blackName << " - Black ]\n\n";
    cout << "        a    b    c    d    e    f    g    h  \n";
    cout << "      +----+----+----+----+----+----+----+----+\n";
    for (int r = 0; r < 8; ++r) {
        cout << "  " << (8 - r) << "   |";
        for (int c = 0; c < 8; ++c) {
            Piece* pc = squares_[r][c];
            bool   isLight = (r + c) % 2 == 0;
            if (pc)
                cout << " " << pc->getSymbol() << "  |";
            else
                cout << (isLight ? " .  " : "    ") << "|";
        }
        cout << "  " << (8 - r) << "\n";
        cout << "      +----+----+----+----+----+----+----+----+\n";
    }
    cout << "        a    b    c    d    e    f    g    h  \n";
    cout << "\n";
    cout << "  [ " << whiteName << " - White ]\n";
    cout << "\n";
    cout << "  White: K=King  Q=Queen  R=Rook  B=Bishop  N=Knight  P=Pawn\n";
    cout << "  Black: k=King  q=Queen  r=Rook  b=Bishop  n=Knight  p=Pawn\n";
    cout << "\n";
}

// ═══════════════════════════════════════════════════════════════
//                        Game 
// ═══════════════════════════════════════════════════════════════
Game::Game() : activeColor_(Color::WHITE), isOver_(false) {}

void Game::setPlayers(const string& white, const string& black) {
    whiteName_ = white;
    blackName_ = black;
}

Color Game::getOpponent(Color side) const {
    return (side == Color::WHITE) ? Color::BLACK : Color::WHITE;
}

Position Game::parseInput(const string& input, bool& ok) const {
    ok = false;
    if (input.size() < 2) return {};
    char colChar = input[0];
    if (colChar >= 'A' && colChar <= 'Z') colChar += 32;
    char rowChar = input[1];
    if (colChar < 'a' || colChar > 'h') return {};
    if (rowChar < '1' || rowChar > '8') return {};
    ok = true;
    return Position(8 - (rowChar - '0'), colChar - 'a');
}

string Game::squareToString(const Position& pos) const {
    string label = "  ";
    label[0] = (char)('a' + pos.col);
    label[1] = (char)('0' + (8 - pos.row));
    return label;
}

void Game::passTurn() { activeColor_ = getOpponent(activeColor_); }

void Game::handlePromotion(const Position& pos) {
    Piece* piece = board_.getPieceAt(pos);
    if (!piece || piece->getType() != PieceType::PAWN) return;

    bool reachedEnd = (piece->getColor() == Color::WHITE && pos.row == 0) ||
        (piece->getColor() == Color::BLACK && pos.row == 7);
    if (!reachedEnd) return;

    Color  side = piece->getColor();
    string ownerName = (side == Color::WHITE) ? whiteName_ : blackName_;
    char   choice = 'Q';

    cout << "\n";
    cout << "  +------------------------------------------+\n";
    cout << "  |          *** PAWN PROMOTION! ***         |\n";
    cout << "  +------------------------------------------+\n";
    cout << "  |  " << ownerName << ", your pawn reached the end!      \n";
    cout << "  |  Choose a piece to promote to:           |\n";
    cout << "  |    Q = Queen    R = Rook                 |\n";
    cout << "  |    B = Bishop   N = Knight               |\n";
    cout << "  +------------------------------------------+\n";
    cout << "  Your choice: ";
    cin >> choice;
    if (choice >= 'a' && choice <= 'z') choice -= 32;

    Piece* promoted = nullptr;
    switch (choice) {
    case 'R': promoted = new Rook(side);   break;
    case 'B': promoted = new Bishop(side); break;
    case 'N': promoted = new Knight(side); break;
    default:  promoted = new Queen(side);  break;
    }
    board_.placePiece(pos, promoted);
    cout << "  Promoted to '" << promoted->getSymbol() << "'!\n";
}

void Game::run() {
    board_.reset();
    activeColor_ = Color::WHITE;
    isOver_ = false;

    while (!isOver_) {
        clearScreen();

        bool   inCheck = board_.isKingInCheck(activeColor_);
        bool   hasMove = board_.playerHasLegalMove(activeColor_);
        string turnName = (activeColor_ == Color::WHITE) ? whiteName_ : blackName_;
        string turnColor = (activeColor_ == Color::WHITE) ? "White" : "Black";

         //  ────────────────── Check game-ending conditions ──────────────────
        if (!hasMove) {
            board_.draw(whiteName_, blackName_);
            cout << "  +=============================================+\n";
            if (inCheck) {
                string winnerName = (activeColor_ == Color::WHITE) ? blackName_ : whiteName_;
                string winnerColor = (activeColor_ == Color::WHITE) ? "Black" : "White";
                cout << "  |           CHECKMATE! GAME OVER              |\n";
                cout << "  |  " << winnerName << " (" << winnerColor << ") wins! Congratulations!\n";
            }
            else {
                cout << "  |       STALEMATE! The game is a draw.        |\n";
            }
            cout << "  +=============================================+\n\n";
            cout << "  Press ENTER to return to the main menu...";
            cin.ignore();
            cin.get();
            isOver_ = true;
            return;
        }

        //  ──────────────────  Display board ─────────────────────────────────
        board_.draw(whiteName_, blackName_);

        if (inCheck)
            cout << "  *** " << turnName << " (" << turnColor << ") is in CHECK! ***\n";

        cout << "  Turn:  " << turnName << " (" << turnColor << ")\n";
        cout << "  Tip: type '!' at the destination prompt to re-pick your piece.\n\n";

        // ── Outer loop: repeat until a full valid move is made ────
        bool moveMade = false;
        while (!moveMade) {

            //  ────────────────── Piece selection ───────────────────────────
            Position from;
            MoveList legal;

            while (true) {
                cout << "  >> Select your piece (e.g. e2): ";
                string fromInput;
                cin >> fromInput;

                bool ok;
                from = parseInput(fromInput, ok);
                if (!ok) {
                    cout << "  Invalid square. Enter a letter (a-h) and a number (1-8).\n\n";
                    continue;
                }
                Piece* piece = board_.getPieceAt(from);
                if (!piece) {
                    cout << "  No piece on that square. Try again.\n\n";
                    continue;
                }
                if (piece->getColor() != activeColor_) {
                    cout << "  That piece belongs to your opponent. Pick your own.\n\n";
                    continue;
                }
                legal = board_.getLegalMoves(from, activeColor_);
                if (legal.size() == 0) {
                    cout << "  That piece is pinned or has no legal moves. Choose another.\n\n";
                    continue;
                }
                break;
            }

            //  ──────────────────Show legal destinations ───────────────────
            cout << "\n";
            cout << "  Piece: '" << board_.getPieceAt(from)->getSymbol()
                << "'  at  " << squareToString(from) << "\n";
            cout << "  Legal moves:  ";
            for (int i = 0; i < legal.size(); ++i) {
                cout << squareToString(legal[i]);
                if (i < legal.size() - 1) cout << "  ";
            }
            cout << "\n\n";

            // ── ────────────────── Destination selection ─────────────────────
            while (true) {
                cout << "  >> Enter destination (or '!' to re-pick piece): ";
                string toInput;
                cin >> toInput;

                if (toInput == "!") {
                    cout << "\n";
                    break;          // break inner → outer loop re-runs piece selection
                }

                bool ok;
                Position dest = parseInput(toInput, ok);
                if (!ok) {
                    cout << "  Invalid square. Use a letter (a-h) and number (1-8).\n\n";
                    continue;
                }

                bool isLegal = false;
                for (int i = 0; i < legal.size(); ++i)
                    if (legal[i] == dest) { isLegal = true; break; }

                if (!isLegal) {
                    cout << "  That square is not a legal move for this piece. Try again.\n\n";
                    continue;
                }

                board_.movePiece(from, dest);
                handlePromotion(dest);
                passTurn();
                moveMade = true;
                break;          // break inner → moveMade=true exits outer loop too
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════
//                          UI helpers
// ═══════════════════════════════════════════════════════════════
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void showMainMenu() {
    clearScreen();
    cout << "  +=================================================+\n";
    cout << "  |                                                 |\n";
    cout << "  |           T H E   C H E S S   G A M E           |\n";
    cout << "  |                                                 |\n";
    cout << "  +=================================================+\n\n";
    cout << "         1.  New Match  (2 Players)\n";
    cout << "         2.  Exit\n\n";
    cout << "  Select an option (1 or 2): ";
}

bool confirmExit() {
    clearScreen();
    cout << "  +=================================================+\n";
    cout << "  |               CONFIRM EXIT                      |\n";
    cout << "  +=================================================+\n\n";
    cout << "  Are you sure you want to quit? (Y / N): ";
    char ch;
    cin >> ch;
    return (ch == 'Y' || ch == 'y');
}

void collectPlayerNames(string& white, string& black) {
    clearScreen();
    cout << "  +=================================================+\n";
    cout << "  |               PLAYER  SETUP                     |\n";
    cout << "  +=================================================+\n\n";
    cout << "  Player 1 name (White pieces): ";
    cin >> white;
    cout << "  Player 2 name (Black pieces): ";
    cin >> black;
    cout << "\n";
    cout << "  Piece key:\n";
    cout << "    White: K=King  Q=Queen  R=Rook  B=Bishop  N=Knight  P=Pawn\n";
    cout << "    Black: k=King  q=Queen  r=Rook  b=Bishop  n=Knight  p=Pawn\n\n";
    cout << "  Press ENTER to begin...";
    cin.ignore();
    cin.get();
}
