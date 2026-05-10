#include "Class.h"

int main() {
    enableAnsiColors();
    string whiteName, blackName;
    while (true) {
        showMainMenu();
        const string GREEN = "\033[38;5;220m";
        const string RESET = "\033[0m";
        cout << GREEN;
        char choice;
        cin >> choice;
        cout << RESET;
        if (choice == '2') {
            if (confirmExit()) {

                clearScreen();

                const string GREEN = "\033[38;5;220m";
                const string RESET = "\033[0m";

                string Padding = "                                        ";

                cout << GREEN;

                cout << "\n\n\n\n\n\n\n\n";

                cout << Padding << "              THANK YOU FOR PLAYING                      \n";

                cout << RESET;

                system("pause");
                return 0;
            }
        }
        else if (choice == '1') {
            collectPlayerNames(whiteName, blackName);
            Game game;
            game.setPlayers(whiteName, blackName);
            game.run();
        }
        else {
            clearScreen();

            const string GREEN = "\033[38;5;220m";
            cout << GREEN;
            cout << "\n  Invalid option. Please enter 1 or 2.\n\n";
            system("pause");
        }
    }
    return 0;
}
