#include "Class.h"

int main() {
    string whiteName, blackName;

    while (true) {
        showMainMenu();
        char choice;
        cin >> choice;

        if (choice == '2') {
            if (confirmExit()) {
                clearScreen();
                cout << "\n  Thanks for playing. Goodbye!\n\n";
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
            cout << "\n  Invalid option. Please enter 1 or 2.\n\n";
            system("pause");
        }
    }

    return 0;
}