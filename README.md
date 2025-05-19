Simple Chess (Powered by AI)
Installation

Après avoir cloné le dépôt :
Sur Linux (Ubuntu) (déjà testé)

Installez la bibliothèque SFML nécessaire pour l'interface graphique avec la commande :
sudo apt install libsfml-dev 

Sur macOS (non testé)
Installez Homebrew si ce n'est pas déjà fait :
bash

/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

Installez SFML :
bash

brew install sfml


Sur Windows (non testé)

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg

.\bootstrap-vcpkg.bat
.\vcpkg install sfml


Ce que vous devez avoir comme fichiers:
![Capture d’écran du 2025-05-19 22-53-00](https://github.com/user-attachments/assets/4dd55371-ca3d-432f-8bd6-9a63af555be4)



Compilation

Compilez le projet en ligne de commande avec :
make
Utilisation
Mode IA vs IA

Exécutez le programme avec :
./miniChess
Pour observer deux IA s'affronter.

Vous pouvez modifier :

    La profondeur de recherche (depth) - ligne 16 du fichier miniChess.c

    Le nombre maximum de coups (maxMoves) - ligne 17 du même fichier

*(Valeurs par défaut : depth = 3, maxMoves = 70)*
Mode Joueur vs IA

exmple d'affichage
![Capture d’écran du 2025-05-19 21-42-20](https://github.com/user-attachments/assets/c8fd14aa-b4af-46cc-8f67-657762aa087e)


Exécutez l'interface graphique avec :
./gui

Vous pouvez ajuster la difficulté de l'IA en modifiant la profondeur (1, 3 ou 5) à la ligne 218 du fichier main_gui.cpp.
Note : Une profondeur plus élevée augmente le temps de calcul.

Exemple d'affichage

![Capture d’écran du 2025-05-19 21-42-52](https://github.com/user-attachments/assets/7d9ed3ef-4347-4aa4-a675-1fc626944dc3)
