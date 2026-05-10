#include "Class.h"

int main() {
    enableAnsiColors();

    const string G = "\033[38;5;220m", R = "\033[0m";
    const string P = "                                        ";
    string whiteName, blackName;

    while (true) {
        showMainMenu();
        cout << G;
        char choice; cin >> choice;
        cout << R;

        if (choice == '1') {
            collectPlayerNames(whiteName, blackName);
            Game game;
            game.setPlayers(whiteName, blackName);
            game.run();
        }
        else if (choice == '2') {
            if (confirmExit()) {
                clearScreen();
                cout << G << "\n\n\n\n\n\n\n";
                cout << P << "====================================================\n\n";
                cout << P << "              THANK YOU FOR PLAYING!                \n\n";
                cout << P << "====================================================\n\n";
                cout << P << "              Press ENTER to exit..." << R;
                cin.ignore(); cin.get();
                return 0;
            }
        }
        else {
            clearScreen();
            cout << G << "\n\n\n\n\n\n\n";
            cout << P << "====================================================\n\n";
            cout << P << "                   INVALID INPUT                    \n\n";
            cout << P << "====================================================\n\n";
            cout << P << "          Please enter 1 or 2.\n\n";
            cout << P << "====================================================\n\n";
            cout << P << "           Press ENTER to try again..." << R;
            cin.ignore(); cin.get();
        }
    }

    return 0;
}
