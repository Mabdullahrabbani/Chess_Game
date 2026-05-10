#include "Class.h"
#include <cstdlib>
#include <iomanip>
#ifdef _WIN32
#include <windows.h>
#endif

// ═══════════════════════════════════════════════════════════════
//  ANSI color enable
// ═══════════════════════════════════════════════════════════════
void enableAnsiColors()
{
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD  dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}
// ═══════════════════════════════════════════════════════════════
//  MoveList
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
        for (int i = 0; i < count_; ++i) temp[i] = moves_[i];
        delete[] moves_;
        moves_ = temp;
        capacity_ = newCap;
    }
    moves_[count_++] = pos;
}

int MoveList::size() const { return count_; }

void MoveList::clear() {
    delete[] moves_;
    moves_ = nullptr;
    count_ = 0;
    capacity_ = 0;
}

Position& MoveList::operator[](int idx) {
    if (idx < 0 || idx >= count_) {
        cout << "[MoveList ERROR] Index " << idx << " out of range! Size = " << count_ << endl;
        exit(1);
    }
    return moves_[idx];
}

const Position& MoveList::operator[](int idx) const {
    if (idx < 0 || idx >= count_) {
        cout << "[MoveList ERROR] Index " << idx << " out of range! Size = " << count_ << endl;
        exit(1);
    }
    return moves_[idx];
}

// ═══════════════════════════════════════════════════════════════
//  Piece base
// ═══════════════════════════════════════════════════════════════
Piece::Piece(Color side, PieceType type)
    : sideColor_(side), pieceType_(type), everMoved_(false) {
}

Color     Piece::getColor()        const { return sideColor_; }
PieceType Piece::getType()         const { return pieceType_; }
bool      Piece::hasMoved()        const { return everMoved_; }
void      Piece::markMoved(bool v) { everMoved_ = v; }

// ═══════════════════════════════════════════════════════════════
//  Pawn
// ═══════════════════════════════════════════════════════════════
Pawn::Pawn(Color side) : Piece(side, PieceType::PAWN) {}
char Pawn::getSymbol() const { return (sideColor_ == Color::WHITE) ? 'P' : 'p'; }

MoveList Pawn::getCandidateMoves(const Position& square, const Board& board) const {
    MoveList result;
    int advance = (sideColor_ == Color::WHITE) ? -1 : 1;

    Position oneStep(square.row + advance, square.col);
    if (oneStep.isValid() && board.isSquareEmpty(oneStep)) {
        result.add(oneStep);
        bool onStartRank = (sideColor_ == Color::WHITE) ? (square.row == 6) : (square.row == 1);
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
//  Rook
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
//  Knight
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
//  Bishop
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
//  Queen
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
//  King
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
//  Board
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
    squares_[0][0] = new Rook(Color::BLACK);   squares_[0][1] = new Knight(Color::BLACK);
    squares_[0][2] = new Bishop(Color::BLACK); squares_[0][3] = new Queen(Color::BLACK);
    squares_[0][4] = new King(Color::BLACK);   squares_[0][5] = new Bishop(Color::BLACK);
    squares_[0][6] = new Knight(Color::BLACK); squares_[0][7] = new Rook(Color::BLACK);
    for (int c = 0; c < 8; ++c) squares_[1][c] = new Pawn(Color::BLACK);

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
            if (getLegalMoves(Position(r, c), side).size() > 0) return true;
        }
    return false;
}

// ═══════════════════════════════════════════════════════════════
//  Board::draw()
// ═══════════════════════════════════════════════════════════════
void Board::draw(const string& whiteName, const string& blackName) const
{
    const string RESET = "\033[0m";
    const string LIGHT_SQ = "\033[48;5;229m";      
    const string DARK_SQ = "\033[48;5;130m";       
    const string WHITE_PC = "\033[38;5;51m\033[1m";
    const string BLACK_PC = "\033[38;5;16m\033[1m";
    const string BORDER = "\033[1;36m";            
    const string COORD = "\033[1;33m";             
    const string AQUA_LBL = "\033[38;5;51m\033[1m";
    const string MAG_LBL = "\033[1;35m";           

    const string PAD = "                    ";
    const int    BOARD_W = 52;

    auto centre = [&](const string& txt) -> string {
        int space = BOARD_W - (int)txt.size();
        int left = space / 2;
        int right = space - left;
        return string(left, ' ') + txt + string(right, ' ');
        };

    // Black player name — magenta, centered
    cout << "\n" << PAD << MAG_LBL
        << centre("[ " + blackName + " - Black ]")
        << RESET << "\n\n";

    cout << PAD << COORD
        << "      a    b    c    d    e    f    g    h  "
        << RESET << "\n";
    cout << PAD << BORDER
        << "    -----|----|----|----|----|----|----|-----"
        << RESET << "\n";

    for (int r = 0; r < 8; ++r)
    {
        cout << PAD << COORD << " " << (8 - r) << "  " << RESET;
        cout << BORDER << "|" << RESET;

        for (int c = 0; c < 8; ++c)
        {
            Piece* pc = squares_[r][c];
            bool   isLight = (r + c) % 2 == 0;
            string sq = isLight ? LIGHT_SQ : DARK_SQ;

            if (pc)
            {
                string pcColor = (pc->getColor() == Color::WHITE) ? WHITE_PC : BLACK_PC;
                cout << sq << pcColor << " " << pc->getSymbol() << "  " << RESET;
            }
            else
            {
                cout << sq << "    " << RESET;
            }
            cout << BORDER << "|" << RESET;
        }
        cout << COORD << " " << (8 - r) << RESET << "\n";
        cout << PAD << BORDER
            << "    |----|----|----|----|----|----|----|-----"
            << RESET << "\n";
    }

    cout << PAD << COORD
        << "      a    b    c    d    e    f    g    h  "
        << RESET << "\n\n";

    // White player name — aqua, centred
    cout << PAD << AQUA_LBL
        << centre("[ " + whiteName + " - White ]")
        << RESET << "\n\n";

    // Legend — "White" in aqua, "Black" in magenta
    cout << PAD << AQUA_LBL << "White" << RESET
        << ": K=King  Q=Queen  R=Rook  B=Bishop  N=Knight  P=Pawn\n";
    cout << PAD << MAG_LBL << "Black" << RESET
        << ": k=King  q=Queen  r=Rook  b=Bishop  n=Knight  p=Pawn\n";
    cout << "\n";
}

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
    cout << "  |  " << ownerName << ", your pawn reached the end!\n";
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

        board_.draw(whiteName_, blackName_);

        if (inCheck)
            cout << "  *** " << turnName << " (" << turnColor << ") is in CHECK! ***\n";

        cout << "  Turn:  " << turnName << " (" << turnColor << ")\n";
        cout << "  Tip: type '!' at the destination prompt to re-pick your piece.\n\n";

        bool moveMade = false;
        while (!moveMade) {
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

            cout << "\n";
            cout << "  Piece: '" << board_.getPieceAt(from)->getSymbol()
                << "'  at  " << squareToString(from) << "\n";
            cout << "  Legal moves:  ";
            for (int i = 0; i < legal.size(); ++i) {
                cout << squareToString(legal[i]);
                if (i < legal.size() - 1) cout << "  ";
            }
            cout << "\n\n";

            while (true) {
                cout << "  >> Enter destination (or '!' to re-pick piece): ";
                string toInput;
                cin >> toInput;

                if (toInput == "!") { cout << "\n"; break; }

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
                break;
            }
        }
    }
}
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
void showMainMenu() {
    clearScreen();

    const string GREEN = "\033[38;5;220m";
    const string RESET = "\033[0m";

    string Padding = "                                        ";

    cout << GREEN;

    cout << "\n\n\n\n\n\n\n";

    cout << Padding << "====================================================\n\n";
    cout << Padding << "          WELCOME TO THE THRONE OF TECTICS          \n";
    cout << Padding << "====================================================\n\n";

    cout << Padding << "                 [1] START NEW MATCH\n\n";

    cout << Padding << "                 [2] EXIT GAME\n\n";

    cout << Padding << "====================================================\n\n";

    cout << Padding << "           Select an option (1 or 2): ";

    cout << RESET;
}

bool confirmExit() {
    clearScreen();

    const string GREEN = "\033[38;5;220m";
    const string RESET = "\033[0m";

    string Padding = "                                        ";

    cout << GREEN;

    cout << "\n\n\n\n\n\n\n\n";

    cout << Padding << "====================================================\n";
    cout << Padding << "                    CONFIRMATION\n";
    cout << Padding << "====================================================\n\n";

    cout << Padding << "        Are you sure you want to quit?\n\n";

    cout << Padding << "                 [Y] YES\n\n";
    cout << Padding << "                 [N] NO\n\n";

    cout << Padding << "====================================================\n\n";

    cout << Padding << "                 Enter your choice: ";

    cout << RESET;

    char ch;
    cin >> ch;

    return (ch == 'Y' || ch == 'y');
}
void collectPlayerNames(string& white, string& black) {
    clearScreen();

    const string GREEN = "\033[38;5;220m";
    const string RESET = "\033[0m";

    cout << GREEN;

    string Padding = "                                        ";
    string Padding2 = "                                   ";

    cout << "\n\n\n\n\n\n\n";

    cout << Padding << "====================================================\n\n";
    cout << Padding << "                 PLAYER REGISTRATION                \n\n";
    cout << Padding << "====================================================\n\n";

    cout << Padding << "       Enter name for Player 1 (White): ";
    cin >> white;

    cout << "\n";

    cout << Padding << "       Enter name for Player 2 (Black): ";
    cin >> black;

    cout << "\n\n";
    cout << Padding << "               Press ENTER to start....";
    cout << RESET;

    cin.ignore();
    cin.get();
    clearScreen();
    cout << "\n\n\n\n\n\n\n";
    cout << GREEN;

    cout << Padding << "====================================================\n\n";
    cout << Padding << "                      REMINDER                       \n\n";
    cout << Padding << "====================================================\n\n";

    cout << Padding << "                   Piece Legend:\n\n";

    cout << Padding2 << "  White: K=King  Q=Queen  R=Rook  B=Bishop  N=Knight  P=Pawn\n\n";

    cout << Padding2 << "  Black: k=King  q=Queen  r=Rook  b=Bishop  n=Knight  p=Pawn\n\n";

    cout << Padding << "               Press ENTER to start....";
    cout << RESET;

    cin.get();
}
