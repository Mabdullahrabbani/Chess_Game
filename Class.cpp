#include "Class.h"
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#endif

void enableAnsiColors() {
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD  m = 0;
    GetConsoleMode(h, &m);
    SetConsoleMode(h, m | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

Pawn::Pawn(Color s) : Piece(s, PieceType::PAWN) {}
char Pawn::getSymbol() const { return sideColor_ == Color::WHITE ? 'P' : 'p'; }

MoveList Pawn::getCandidateMoves(const Position& sq, const Board& board) const {
    MoveList r;
    int adv = (sideColor_ == Color::WHITE) ? -1 : 1;

    Position one(sq.row + adv, sq.col);
    if (one.isValid() && board.isSquareEmpty(one)) {
        r.add(one);
        bool onStart = (sideColor_ == Color::WHITE) ? (sq.row == 6) : (sq.row == 1);
        Position two(sq.row + 2 * adv, sq.col);
        if (onStart && two.isValid() && board.isSquareEmpty(two)) r.add(two);
    }

    int diagOffsets[2] = { -1, 1 };
    for (int i = 0; i < 2; ++i) {
        Position d(sq.row + adv, sq.col + diagOffsets[i]);
        if (d.isValid() && board.isSquareEnemy(d, sideColor_)) r.add(d);
    }
    return r;
}

static MoveList slidingMoves(const Position& sq, const Board& board,
    Color side, const int dirs[][2], int n) {
    MoveList r;
    for (int d = 0; d < n; ++d) {
        Position c(sq.row + dirs[d][0], sq.col + dirs[d][1]);
        while (c.isValid()) {
            if (board.isSquareEmpty(c))                      r.add(c);
            else { if (board.isSquareEnemy(c, side)) r.add(c); break; }
            c.row += dirs[d][0];
            c.col += dirs[d][1];
        }
    }
    return r;
}

Rook::Rook(Color s) : Piece(s, PieceType::ROOK) {}
char Rook::getSymbol() const { return sideColor_ == Color::WHITE ? 'R' : 'r'; }
MoveList Rook::getCandidateMoves(const Position& sq, const Board& board) const {
    static const int dirs[4][2] = { {-1,0},{1,0},{0,-1},{0,1} };
    return slidingMoves(sq, board, sideColor_, dirs, 4);
}

Bishop::Bishop(Color s) : Piece(s, PieceType::BISHOP) {}
char Bishop::getSymbol() const { return sideColor_ == Color::WHITE ? 'B' : 'b'; }
MoveList Bishop::getCandidateMoves(const Position& sq, const Board& board) const {
    static const int dirs[4][2] = { {-1,-1},{-1,1},{1,-1},{1,1} };
    return slidingMoves(sq, board, sideColor_, dirs, 4);
}

Queen::Queen(Color s) : Piece(s, PieceType::QUEEN) {}
char Queen::getSymbol() const { return sideColor_ == Color::WHITE ? 'Q' : 'q'; }
MoveList Queen::getCandidateMoves(const Position& sq, const Board& board) const {
    static const int dirs[8][2] = { {-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1} };
    return slidingMoves(sq, board, sideColor_, dirs, 8);
}

Knight::Knight(Color s) : Piece(s, PieceType::KNIGHT) {}
char Knight::getSymbol() const { return sideColor_ == Color::WHITE ? 'N' : 'n'; }
MoveList Knight::getCandidateMoves(const Position& sq, const Board& board) const {
    MoveList r;
    static const int leaps[8][2] = { {-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1} };
    for (int i = 0; i < 8; ++i) {
        Position d(sq.row + leaps[i][0], sq.col + leaps[i][1]);
        if (d.isValid() && (board.isSquareEmpty(d) || board.isSquareEnemy(d, sideColor_))) r.add(d);
    }
    return r;
}

King::King(Color s) : Piece(s, PieceType::KING) {}
char King::getSymbol() const { return sideColor_ == Color::WHITE ? 'K' : 'k'; }
MoveList King::getCandidateMoves(const Position& sq, const Board& board) const {
    MoveList r;
    for (int dr = -1; dr <= 1; ++dr)
        for (int dc = -1; dc <= 1; ++dc) {
            if (!dr && !dc) continue;
            Position d(sq.row + dr, sq.col + dc);
            if (d.isValid() && (board.isSquareEmpty(d) || board.isSquareEnemy(d, sideColor_))) r.add(d);
        }
    return r;
}

Board::Board() { clearBoard(); setupPieces(); }
Board::~Board() { for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) delete squares_[r][c]; }

void Board::clearBoard() {
    for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) squares_[r][c] = nullptr;
}

void Board::setupPieces() {
    auto backRank = [&](Color cl, int row) {
        squares_[row][0] = new Rook(cl);   squares_[row][1] = new Knight(cl);
        squares_[row][2] = new Bishop(cl); squares_[row][3] = new Queen(cl);
        squares_[row][4] = new King(cl);   squares_[row][5] = new Bishop(cl);
        squares_[row][6] = new Knight(cl); squares_[row][7] = new Rook(cl);
        };
    backRank(Color::BLACK, 0);
    for (int c = 0; c < 8; ++c) squares_[1][c] = new Pawn(Color::BLACK);
    backRank(Color::WHITE, 7);
    for (int c = 0; c < 8; ++c) squares_[6][c] = new Pawn(Color::WHITE);
}

void Board::reset() {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) { delete squares_[r][c]; squares_[r][c] = nullptr; }
    setupPieces();
}

Piece* Board::getPieceAt(const Position& p) const { return p.isValid() ? squares_[p.row][p.col] : nullptr; }
Piece* Board::getPieceAt(int r, int c)      const { return getPieceAt(Position(r, c)); }

bool Board::isSquareEmpty(const Position& p) const { return p.isValid() && !squares_[p.row][p.col]; }
bool Board::isSquareEnemy(const Position& p, Color side) const {
    Piece* pc = p.isValid() ? squares_[p.row][p.col] : nullptr;
    return pc && pc->getColor() != side;
}

void Board::movePiece(const Position& from, const Position& to) {
    delete squares_[to.row][to.col];
    squares_[to.row][to.col] = squares_[from.row][from.col];
    squares_[from.row][from.col] = nullptr;
    if (squares_[to.row][to.col]) squares_[to.row][to.col]->markMoved(true);
}

void Board::placePiece(const Position& pos, Piece* p) {
    delete squares_[pos.row][pos.col];
    squares_[pos.row][pos.col] = p;
}

Position Board::findKingSquare(Color side) const {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            Piece* p = squares_[r][c];
            if (p && p->getType() == PieceType::KING && p->getColor() == side) return { r, c };
        }
    return { -1, -1 };
}

bool Board::isKingInCheck(Color side) const {
    Position kp = findKingSquare(side);
    Color    enemy = (side == Color::WHITE) ? Color::BLACK : Color::WHITE;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            Piece* a = squares_[r][c];
            if (!a || a->getColor() != enemy) continue;
            MoveList t = a->getCandidateMoves({ r, c }, *this);
            for (int i = 0; i < t.size(); ++i) if (t[i] == kp) return true;
        }
    return false;
}

MoveList Board::getLegalMoves(const Position& sq, Color side) const {
    MoveList legal;
    Piece* pc = getPieceAt(sq);
    if (!pc || pc->getColor() != side) return legal;
    MoveList cands = pc->getCandidateMoves(sq, *this);
    Board& self = const_cast<Board&>(*this);
    for (int i = 0; i < cands.size(); ++i) {
        bool inCheck = false;
        self.tryMove(sq, cands[i], [&] { inCheck = self.isKingInCheck(side); });
        if (!inCheck) legal.add(cands[i]);
    }
    return legal;
}

bool Board::playerHasLegalMove(Color side) const {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            Piece* p = squares_[r][c];
            if (p && p->getColor() == side && getLegalMoves({ r, c }, side).size() > 0) return true;
        }
    return false;
}

void Board::draw(const string& wN, const string& bN) const {
    const string RST = "\033[0m";
    const string LT = "\033[48;5;229m", DK = "\033[48;5;130m";
    const string WPC = "\033[38;5;51m\033[1m", BPC = "\033[38;5;16m\033[1m";
    const string BRD = "\033[1;36m", CRD = "\033[1;33m";
    const string AQU = "\033[38;5;51m\033[1m", MAG = "\033[1;35m";
    const string PAD = "                    ";
    const int    W = 52;

    auto centre = [&](const string& t) -> string {
        int sp = W - (int)t.size(), l = sp / 2;
        return string(l, ' ') + t + string(sp - l, ' ');
        };

    cout << "\n" << PAD << MAG << centre("[ " + bN + " - Black ]") << RST << "\n\n";
    cout << PAD << CRD << "      a    b    c    d    e    f    g    h  " << RST << "\n";
    cout << PAD << BRD << "    -----|----|----|----|----|----|----|-----" << RST << "\n";

    for (int r = 0; r < 8; ++r) {
        cout << PAD << CRD << " " << (8 - r) << "  " << RST << BRD << "|" << RST;
        for (int c = 0; c < 8; ++c) {
            Piece* pc = squares_[r][c];
            string sq = ((r + c) % 2 == 0) ? LT : DK;
            if (pc) cout << sq << (pc->getColor() == Color::WHITE ? WPC : BPC) << " " << pc->getSymbol() << "  " << RST;
            else    cout << sq << "    " << RST;
            cout << BRD << "|" << RST;
        }
        cout << CRD << " " << (8 - r) << RST << "\n";
        cout << PAD << BRD << "    |----|----|----|----|----|----|----|-----" << RST << "\n";
    }

    cout << PAD << CRD << "      a    b    c    d    e    f    g    h  " << RST << "\n\n";
    cout << PAD << AQU << centre("[ " + wN + " - White ]") << RST << "\n\n";
    cout << PAD << AQU << "White" << RST << ": K=King  Q=Queen  R=Rook  B=Bishop  N=Knight  P=Pawn\n";
    cout << PAD << MAG << "Black" << RST << ": k=King  q=Queen  r=Rook  b=Bishop  n=Knight  p=Pawn\n\n";
}

void  Game::setPlayers(const string& w, const string& b) { whiteName_ = w; blackName_ = b; }
Color Game::getOpponent(Color s)  const { return s == Color::WHITE ? Color::BLACK : Color::WHITE; }
void  Game::passTurn() { activeColor_ = getOpponent(activeColor_); }

Position Game::parseInput(const string& in, bool& ok) const {
    ok = false;
    if (in.size() < 2) return {};
    char cc = in[0], rc = in[1];
    if (cc >= 'A' && cc <= 'Z') cc += 32;
    if (cc < 'a' || cc > 'h' || rc < '1' || rc > '8') return {};
    ok = true;
    return { 8 - (rc - '0'), cc - 'a' };
}

string Game::squareToString(const Position& p) const {
    return string(1, (char)('a' + p.col)) + (char)('0' + (8 - p.row));
}

void Game::handlePromotion(const Position& pos) {
    Piece* pc = board_.getPieceAt(pos);
    if (!pc || pc->getType() != PieceType::PAWN) return;
    bool reached = (pc->getColor() == Color::WHITE && pos.row == 0) ||
        (pc->getColor() == Color::BLACK && pos.row == 7);
    if (!reached) return;

    Color  side = pc->getColor();
    string pName = (side == Color::WHITE) ? whiteName_ : blackName_;
    string sideStr = (side == Color::WHITE) ? "White" : "Black";

    const string G = "\033[38;5;220m";
    const string RST = "\033[0m";
    const string P = "                                        ";

    char ch = 0;
    while (true) {
        clearScreen();
        cout << G << "\n\n\n\n\n\n\n";
        cout << P << "====================================================\n\n";
        cout << P << "                   PAWN PROMOTION!                  \n\n";
        cout << P << "====================================================\n\n";
        cout << P << "        Player : " << pName << " (" << sideStr << ")\n\n";
        cout << P << "        Choose your promotion piece:\n\n";
        cout << P << "                 [Q]  Queen\n\n";
        cout << P << "                 [R]  Rook\n\n";
        cout << P << "                 [B]  Bishop\n\n";
        cout << P << "                 [N]  Knight\n\n";
        cout << P << "====================================================\n\n";
        cout << P << "           Enter your choice (Q / R / B / N): " << RST;

        cin >> ch;
        if (ch >= 'a' && ch <= 'z') ch -= 32;
        if (ch == 'Q' || ch == 'R' || ch == 'B' || ch == 'N') break;

        clearScreen();
        cout << G << "\n\n\n\n\n\n\n";
        cout << P << "====================================================\n\n";
        cout << P << "                   PAWN PROMOTION!                  \n\n";
        cout << P << "====================================================\n\n";
        cout << P << "        Invalid choice!  Please enter Q, R, B or N.\n\n";
        cout << P << "====================================================\n\n";
        cout << P << "           Press ENTER to try again..." << RST;
        cin.ignore(); cin.get();
    }

    Piece* prom = (ch == 'R') ? (Piece*)new Rook(side) :
        (ch == 'B') ? (Piece*)new Bishop(side) :
        (ch == 'N') ? (Piece*)new Knight(side) : (Piece*)new Queen(side);
    board_.placePiece(pos, prom);

    clearScreen();
    cout << G << "\n\n\n\n\n\n\n";
    cout << P << "====================================================\n\n";
    cout << P << "                   PAWN PROMOTION!                  \n\n";
    cout << P << "====================================================\n\n";
    cout << P << "        Promoted to '" << prom->getSymbol() << "'!  Good luck!\n\n";
    cout << P << "====================================================\n\n";
    cout << P << "           Press ENTER to continue..." << RST;
    cin.ignore(); cin.get();
}

void Game::run() {
    board_.reset();
    activeColor_ = Color::WHITE;
    isOver_ = false;

    while (!isOver_) {
        clearScreen();
        bool   inCheck = board_.isKingInCheck(activeColor_);
        bool   hasMove = board_.playerHasLegalMove(activeColor_);
        string tName = (activeColor_ == Color::WHITE) ? whiteName_ : blackName_;
        string tCol = (activeColor_ == Color::WHITE) ? "White" : "Black";

        if (!hasMove) {
            clearScreen();
            const string G = "\033[38;5;220m";
            const string RST = "\033[0m";
            const string P = "                                        ";

            if (inCheck) {
                string wn = (activeColor_ == Color::WHITE) ? blackName_ : whiteName_;
                string wc = (activeColor_ == Color::WHITE) ? "Black" : "White";
                cout << G << "\n\n\n\n\n\n\n";
                cout << P << "====================================================\n\n";
                cout << P << "               *** CHECKMATE! GAME OVER ***         \n\n";
                cout << P << "====================================================\n\n";
                cout << P << "                   " << wn << " (" << wc << ") wins!\n\n";
                cout << P << "                    Congratulations!\n\n";
                cout << P << "====================================================\n\n";
                cout << P << "         Press ENTER to return to main menu..." << RST;
            }
            else {
                cout << G << "\n\n\n\n\n\n\n";
                cout << P << "====================================================\n\n";
                cout << P << "                  *** STALEMATE! ***                \n\n";
                cout << P << "====================================================\n\n";
                cout << P << "                 The game is a draw.\n\n";
                cout << P << "          Both players fought with great honor!\n\n";
                cout << P << "====================================================\n\n";
                cout << P << "         Press ENTER to return to main menu..." << RST;
            }

            cin.ignore(); cin.get();
            isOver_ = true;
            return;
        }

        board_.draw(whiteName_, blackName_);
        if (inCheck) cout << "  *** " << tName << " (" << tCol << ") is in CHECK! ***\n";
        cout << "  Turn:  " << tName << " (" << tCol << ")\n"
            << "  Tip: type '!' at the destination prompt to re-pick your piece.\n\n";

        bool moveMade = false;
        while (!moveMade) {
            Position from;
            MoveList legal;

            while (true) {
                cout << "  >> Select your piece (e.g. e2): ";
                string inp; cin >> inp;
                bool ok; from = parseInput(inp, ok);
                if (!ok) { cout << "  Invalid square. Use a-h and 1-8.\n\n";  continue; }
                Piece* pc = board_.getPieceAt(from);
                if (!pc) { cout << "  No piece on that square.\n\n";          continue; }
                if (pc->getColor() != activeColor_) { cout << "  That's your opponent's piece.\n\n";    continue; }
                legal = board_.getLegalMoves(from, activeColor_);
                if (!legal.size()) { cout << "  Piece is pinned or has no moves.\n\n"; continue; }
                break;
            }

            cout << "\n  Piece: '" << board_.getPieceAt(from)->getSymbol()
                << "'  at  " << squareToString(from) << "\n  Legal moves: ";
            for (int i = 0; i < legal.size(); ++i)
                cout << squareToString(legal[i]) << (i < legal.size() - 1 ? "  " : "");
            cout << "\n\n";

            while (true) {
                cout << "  >> Destination (or '!' to re-pick): ";
                string inp; cin >> inp;
                if (inp == "!") { cout << "\n"; break; }
                bool ok; Position dest = parseInput(inp, ok);
                if (!ok) { cout << "  Invalid square.\n\n"; continue; }

                bool isLegal = false;
                for (int i = 0; i < legal.size(); ++i) if (legal[i] == dest) { isLegal = true; break; }
                if (!isLegal) { cout << "  Not a legal move. Try again.\n\n"; continue; }

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

static void printBox(const string& P, const string& G, const string& R,
    const string lines[], int n) {
    cout << G << "\n\n\n\n\n\n\n";
    cout << P << "====================================================\n\n";
    for (int i = 0; i < n; ++i) cout << lines[i] << "\n";
    cout << P << "====================================================\n\n" << R;
}

void showMainMenu() {
    clearScreen();
    const string G = "\033[38;5;220m", R = "\033[0m";
    const string P = "                                        ";
    const string lines[] = {
        P + "          WELCOME TO THE THRONE OF TACTICS          ",
        "",
        P + "                 [1] START NEW MATCH",
        "",
        P + "                 [2] EXIT GAME",
        ""
    };
    printBox(P, G, R, lines, 6);
    cout << G << P << "           Select an option (1 or 2): " << R;
}

bool confirmExit() {
    while (true) {
        clearScreen();
        const string G = "\033[38;5;220m", R = "\033[0m";
        const string P = "                                        ";
        const string lines[] = {
            P + "                    CONFIRMATION",
            "",
            P + "        Are you sure you want to quit?",
            "",
            P + "                 [Y] YES",
            "",
            P + "                 [N] NO",
            ""
        };
        printBox(P, G, R, lines, 8);
        cout << G << P << "                 Enter your choice: " << R;

        char ch; cin >> ch;
        if (ch == 'Y' || ch == 'y') return true;
        if (ch == 'N' || ch == 'n') return false;

        clearScreen();
        cout << G << "\n\n\n\n\n\n\n";
        cout << P << "====================================================\n\n";
        cout << P << "                    CONFIRMATION                    \n\n";
        cout << P << "====================================================\n\n";
        cout << P << "        Invalid choice!  Please enter Y or N.\n\n";
        cout << P << "====================================================\n\n";
        cout << P << "           Press ENTER to try again..." << R;
        cin.ignore(); cin.get();
    }
}

void collectPlayerNames(string& white, string& black) {
    clearScreen();
    const string G = "\033[38;5;220m", R = "\033[0m";
    const string P = "                                        ";
    const string P2 = "                                   ";

    cout << G << "\n\n\n\n\n\n\n";
    cout << P << "====================================================\n\n";
    cout << P << "                 PLAYER REGISTRATION                \n\n";
    cout << P << "====================================================\n\n";
    cout << P << "       Enter name for Player 1 (White): "; cin >> white;
    cout << "\n" << P << "       Enter name for Player 2 (Black): "; cin >> black;
    cout << "\n\n" << P << "               Press ENTER to start...." << R;
    cin.ignore(); cin.get();

    clearScreen();
    cout << G << "\n\n\n\n\n\n\n";
    cout << P << "====================================================\n\n";
    cout << P << "                      REMINDER                      \n\n";
    cout << P << "====================================================\n\n";
    cout << P << "                   Piece Legend:\n\n";
    cout << P2 << "  White: K=King  Q=Queen  R=Rook  B=Bishop  N=Knight  P=Pawn\n\n";
    cout << P2 << "  Black: k=King  q=Queen  r=Rook  b=Bishop  n=Knight  p=Pawn\n\n";
    cout << P << "               Press ENTER to start...." << R;
    cin.get();
}
