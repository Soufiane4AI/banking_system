# banking_system
Ce projet académique vise à développer un système de gestion bancaire en C connecté à MySQL, permettant aux clients d'effectuer des opérations financières (virements, dépôts, retraits) et de gérer leurs comptes, ainsi qu'aux administrateurs de superviser l'ensemble du système.


Avant de lancer l'application, assurez-vous de :
1. Avoir un serveur MySQL actif.
2. Avoir importé le script 'banque_system.sql' (création de la base 'banque_system').
3. Avoir configuré vos identifiants (User/Password) dans le fichier 'dbconnect.c' .
4. IMPORTANT : Conserver le fichier 'libmysql.dll' dans le même dossier que l'exécutable généré.


METHODE 1 : COMPILATION MANUELLE (Recommandée pour test rapide)

Si vous utilisez un terminal avec GCC (MinGW), lancez la commande suivante à la racine du projet.
(Adaptez les chemins -I et -L selon votre installation MySQL locale).

Commande :
gcc main.c dbconnect.c interface.c new_account.c Admin.c fonctionnalites.c -o banking_system.exe -I"C:/Program Files/MySQL/MySQL Server 8.0/include" -L. -l:libmysql.dll -lws2_32

Lancement :
./banking_system.exe


METHODE 2 : COMPILATION VIA CMAKE 

Cette méthode correspond à l'architecture décrite dans le chapitre "Intégration et Connexion" du rapport.

1. Créer un dossier de build : mkdir build && cd build
2. Générer les makefiles :     cmake .. -G "MinGW Makefiles"
3. Compiler :                  mingw32-make
4. Copier 'libmysql.dll' dans le dossier build.
5. Lancer :                    ./banking_system.exe
