# Projet-TP-Simulateur-SGF
Projet TP : Simulateur Simplifié d’un Système de Gestion de  Fichiers (SGF)

---------------------------------------------------------------------------------------------------------------------------------

ATTENTION ! : Avant de compiler le code, fermez l'exécutable "Simulateur Gestion de Fichiers" afin d'éviter des erreurs d'accès !

ATTENTION ! : Il faut avoir le compilateur GCC pour compiler le code source!

---------------------------------------------------------------------------------------------------------------------------------

ÉTAPES POUR COMPILER ET EXÉCUTER :

ETAPE 1 : Ouvrir votre terminal :

Vous pouvez utiliser CMD (Invite de Commandes) ou ouvrir le dossier du projet dans VSCode.
Accéder au répertoire du projet :

Si vous utilisez CMD, naviguez jusqu'au dossier contenant les fichiers du projet.
Si vous utilisez VSCode, ouvrez simplement le terminal intégré.
Compiler les fichiers source en fichiers objet :

ETAPE 2 : Exécutez les commandes suivantes pour créer les fichiers objets .o :

gcc -c functsFichierDonnes.c

gcc -c functsFichierMeta.c

gcc -c functsmemoire.c

gcc -c main.c

Après cela, vous trouverez les fichiers suivants dans votre dossier :

functsFichierDonnes.o

functsFichierMeta.o

functsmemoire.o

main.o

ETAPE 3 : Créer l'exécutable :

Utilisez cette commande pour lier les fichiers objets et créer l'exécutable :

gcc -o "Simulateur Gestion de Fichiers" functsFichierDonnes.o functsFichierMeta.o functsmemoire.o main.o

REMARQUE : Vous pouvez remplacer "Simulateur Gestion de Fichiers" par un autre nom pour votre exécutable.

ETAPE 4 : Lancer l'exécutable :

Une fois compilé, exécutez le fichier pour démarrer le simulateur.

---------------------------------------------------------------------------------------------------------

EN CAS DE PROBLÈME :

Si vous rencontrez des erreurs liées au fichier binaire memoire_secondaire.bin :
Vérifiez qu'il est correctement créé.
Si ce n’est pas le cas, modifiez le chemin dans la fonction initialiseDisk() située dans functsmemoire.c.
Mettez également à jour ce chemin dans main.c.

---------------------------------------------------------------------------------------------------------
