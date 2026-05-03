Projet C - Comédie Française 
Réalisé par Yuan Ding et Amira Kairouani
----------

Compilation
----------
cd chemin/pour/aller/sur/cfvf
make
./comedie

Comptes de test
----------
- yuan_pise                  mot de passe : adminpise       role : admin
- amira_pise                 mot de passe : userpise        role : utilisateur
- jeanmichel_pise            mot de passe : abonnepise      role : abonne

Fichiers DATA
----------
Les fichiers DATA sont au format txt avec le separateur ;

DATA/00spectacle_25_26.txt        : Spectacles de la saison
DATA/01info_salle.txt             : Salles et leurs jauges totales
DATA/02cat_billet_salle.txt       : Catégories de billets par salle et les jauges de chaque catégorie
DATA/03cat_horaire.txt            : Catégories des horaires des représentation
DATA/04cat_profil.txt             : Profils tarifaires
DATA/05representation_25_26.txt   : Représentations de la saison, dates au format AAAA-MM-JJ
DATA/06info_tarif.txt             : Tarifs
DATA/07comptes.txt                : Comptes de connexion
DATA/08reservations.txt           : Réservations sauvegardées (Pour faciliter les tests, la représentation du 9 juillet 2026 à 20h30 pour DÉSHONORÉE UN CRIME D’HONNEUR EN CALABRE est complète par défaut)
DATA/09abonnes.txt                : Liste des abonnes
DATA/10spectacle_salle.txt        : Association entre les salles et les spectacles
DATA/11cat_abonnes.txt            : Catégories des abonnés
DATA/etat_saison.txt		  : Statut de la saison (chargé ou non chargé)
DATA/Archive			  : Dossier pour stocker les données de la saison avant l'initialisation de la saison et la logout de l'admin afin de ne pas perdre les données mais aussi pour les autres demandes spéciales de sauvegardes 
DATA/exports_reservations	  : Dossier pour sauvegarder les exports de réservation d'une représentation spéciale sous demande
DATA/representation_supprimee	  : Les archives des réservations lors de la suppression d'un spectacle ou d'une représentation afin de garder les infos pour effectuer les remboursements

Modules principaux
------------------
declarations.h	   : Regroupement de toutes les déclarations préliminaires
main.c		   : Tronc commun / menu principal du programme
authentification.c : Chargement des comptes et connexion
chargement.c       : Chargement de la saison et initialisation de la saison
consultation.c     : Consultation par spectacle ou par salle
admin.c            : Gestion admin des spectacles et représentations
reservations.c     : Création et gestion des réservations 
abonnes.c          : Gestion des abonnés 

Règles de gestion principales
------------------
- Les menus sont différents selon le droit du compte connecté.
- Les utilisateurs simples et les abonnés n'ont pas le droit d'initialiser ou de charger la saison et c'est un droit est réservé à l'administrateur.
- Si la saison est chargée au lancement par l'admin : il n'est pas demandé de recharger a chaque lancement.
- Avant l'initialisation, les réservations sont sauvegardées dans DATA/archive.
- La consultation se fait par spectacle ou par salle.
- Les spectacles sont séparés entre spectacles passés et spectacles à venir. 
- Les représentations vont être affichée sous statut : Disponible, Complet ou Passée.
- Une représentation passée n'est plus réservable.
- L'utilisateur peut choisir d'afficher seulement les représentations disponibles d'un spectacle.
- La réservation se fait dans l'ordre : choix de représentation => consultation des places restantes => choix du catégorie => choix du tarif => confirmation.
- Les tarifs abonnes sont affiches uniquement aux administrateurs et aux comptes abonnes actifs.
- Quand un tarif abonné est choisi, la réservation doit être forcément liée avec un abonné sur la liste des abonnés.
- Un abonne inactif reste dans l'historique, mais ne peut pas beneficier des tarifs abonnes.
- Les abonnés ne seront pas supprimés définitivement : ils sont rendus inactifs et ils peuvent être réactivé.
- Un abonné connecté peut modifier ses informations, sauf sa catégorie et son état actif.


Tests principales à effectuer
------------------
connexion admin yuan/adminpise :
*Chargement de la saison
*Consultation par salle - Studio-Théâtre - DÉSHONORÉE UN CRIME D’HONNEUR EN CALABRE - montrer toutes les représentations 
	- choisir la représentation du 9 juillet 2026 à 20h30 - non réservable 
	- choisir une autre représentation du spectacle 
		- réserver pour l'user Amira (user ayant un compte) - max 10 réservations pour une représentation
		- réserver pour l'abonné Jean-Michel (abonné ayant un compte) - max 10 réservations pour une représentation (max 2 réservation tarif carte abonné adulte)
		- réserver pour l'abonné Nathalie Fleuret (la liste d'abonné) - max 10 réservations pour une représentation (max 2 réservation tarif carte abonné adulte famille et 4 réservations tarif carte abonné jeune famille )
		- réserver en saisissant les infos du spectateur 
*Ajout de deux spectacles
*Modification du nom d'un spectacle
*Ajout d'une représentation pour chaque spectacles ajoutés
*Suppression d'un spectacle - toutes les représentations liées vont être supprimées aussi - les infos de réservation de toutes ces représentations	vont être sauvegardés dans DATA/exports_reservations
*Suppression d'une représentation - les infos de réservation de toutes cette représentations vont être sauvegardés dans DATA/exports_reservations
*Ajout d'un abonné
*Modifications des infos d'un abonné
*Inactiver un abonné
*Réactiver un abonné
*Sauvegarder toutes les données dans l'archive

connexion utilisateur amira/userpise :
*Consultation par spectacle

connexion abonne jeanmichel_pise/abonnepise :
*Modifications des informations de l'abonnement

connexion admin yuan/adminpise :
*Initialisation de la saison
*Remettre les données archivés dans DATA pour remplacer les fichiers vides (Il faut assurer ces fichiers ont les même noms dans la liste des fichiers DATA)
*Recharger la saison

Autres Notes
------------------
Nous avons utilisé certaines syntaxes et fonctions comme sprintf, sscanf, localtime, la gestion des chaînes de caractères contenant des espaces, la création de répertoires avec des commandes Unix, ainsi que le Makefile et la compilation séparée. Certaines de ces notions n’ont pas été apprises directement pendant le cours, mais nous les avons trouvées dans le polycours. Nous les avons utilisées de manière limitée, car elles étaient utiles, voire nécessaires, pour répondre aux besoins de notre projet : génération automatique d’identifiants, manipulation des dates réelle, archivage des données et organisation du programme en plusieurs fichiers. Le projet n’est pas forcément très complexe sur le plan algorithmique, mais nous avons consacré beaucoup de temps à définir le parcours utilisateur, à organiser les menus et à construire des fichiers de données sources cohérents. Comme un des membres du projet avait travaillé dans plusieurs théâtres, nous avons aussi essayé de rendre le fonctionnement proche d’un cas réel. Il reste néanmoins plusieurs points à améliorer. Le contrôle de saisie des dates et des heures pourrait être ajouté afin d’éviter des erreurs. Certaines fonctions peuvent également être encore simplifiées ou supprimées, car le programme ont été modifiées au fur et à mesure de l’avancement du projet. Il est donc possible qu’il reste quelques procédures redondantes ou d’anciennes fonctions qui n’ont pas encore été supprimées ou remplacées définitivement. Nous avons cependant essayé de garder une structure claire, avec des noms de fonctions explicites, des fichiers séparés par thème et des commentaires pour expliquer les choix principaux du programme.
