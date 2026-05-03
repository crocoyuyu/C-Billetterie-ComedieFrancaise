/* Ce fichier regroupe les constantes, les structures, les variables globales 
et les declarations preliminaires des sous-programmes. */

#include <stdio.h>
#include <string.h>

//Constantes des tailles max des tableaux
#define MAX_SPECTACLES 100
#define MAX_SALLES 100
#define MAX_CATEGORIES 1500
#define MAX_HORAIRES 100
#define MAX_PROFILS 100
#define MAX_REPRESENTATIONS 1500
#define MAX_TARIFS 2500
#define MAX_COMPTES 50
#define MAX_RESERVATIONS 3000
#define MAX_ABONNES 500

//Constantes des tailles des chaines de caracteres
#define TAILLE_ID 20
#define TAILLE_NOM 160
#define TAILLE_JOUR 30
#define TAILLE_HEURE 30
#define TAILLE_DATE 30
#define TAILLE_LIBELLE 220
#define TAILLE_LIGNE 1000
#define TAILLE_EMAIL 120
#define TAILLE_TEL 30
#define TAILLE_DROIT 30
#define TAILLE_MDP 120
#define TAILLE_REPERTOIRE 80

#define NON_TROUVE -1
#define RETOUR_CHOIX_CATEGORIES -2

//Structure du fichier 00spectacle_25_26.txt
struct SPECTACLE
{
    char id_spectacle[TAILLE_ID];
    char nom_spectacle[TAILLE_NOM];
};

//Structure du fichier 01info_salle.txt
struct SALLE
{
    char id_salle[TAILLE_ID];
    char nom_salle[TAILLE_NOM];
    int jauge_total_salle;
};

//Structure du fichier 02cat_billet_salle.txt
//Association entre une salle et ses categories de billets.
struct CATEGORIE_BILLET_SALLE
{
    char id_salle[TAILLE_ID];
    char nom_salle[TAILLE_NOM];
    char id_categorie_billet[TAILLE_ID];
    char categorie_billet[TAILLE_LIBELLE];
    int jauge_categorie_billet;
};

//Structure du fichier 03cat_horaire.txt
//Table de reference des categories horaires
struct CATEGORIE_HORAIRE
{
    char libelle_horaire[TAILLE_LIBELLE];
    char id_cat_horaire[TAILLE_ID];
};

//Structure du fichier 04cat_profil.txt
//Table de reference des profils tarifaires
struct CATEGORIE_PROFIL
{
    char id_cat_profil[TAILLE_ID];
    char profil_tarif[TAILLE_LIBELLE];
    char type_tarif[TAILLE_LIBELLE];
};

//Structure du fichier 05representation_25_26.txt
//Une representation lie un spectacle, une date, un horaire et une salle
struct REPRESENTATION
{
    char id_representation[TAILLE_ID];
    char id_spectacle[TAILLE_ID];
    char nom_spectacle[TAILLE_NOM];
    char jour_representation[TAILLE_JOUR];
    char heure_representation[TAILLE_HEURE];
    char horaire_representation[TAILLE_LIBELLE];
    char id_cat_representation[TAILLE_ID];
    char date_representation[TAILLE_DATE];
    char id_salle[TAILLE_ID];
    char nom_salle[TAILLE_NOM];
};

//Structure du fichier 06info_tarif.txt
//Un tarif depend de la salle, de la categorie de billet, du profil et de l'horaire
struct TARIF
{
    char id_categorie_billet[TAILLE_ID];
    char id_salle[TAILLE_ID];
    char nom_salle[TAILLE_NOM];
    char categorie_billet[TAILLE_LIBELLE];
    int jauge_categorie_billet;
    char type_tarif[TAILLE_LIBELLE];
    char id_cat_profil[TAILLE_ID];
    char id_cat_horaire[TAILLE_ID];
    char profil_tarif[TAILLE_LIBELLE];
    char horaire_tarif[TAILLE_LIBELLE];
    int tarif_brut;
};

//Structure du fichier 07comptes.txt
struct COMPTE
{
    char id_compte[TAILLE_ID];
    char droit_compte[TAILLE_DROIT];
    char mdp_hache[TAILLE_MDP]; //Le champ mdp_hache garde ce nom pour preparer une future evolution de hachage
    char nom[TAILLE_NOM];
    char prenom[TAILLE_NOM];
    char email[TAILLE_EMAIL];
    char tel[TAILLE_TEL];
};

//Structure du fichier 08reservations.txt
struct RESERVATION
{
    char id_reservation[TAILLE_ID];
    char id_compte[TAILLE_ID];
    char id_representation[TAILLE_ID];
    char id_spectacle[TAILLE_ID];
    char nom_spectacle[TAILLE_NOM];
    char date_representation[TAILLE_DATE];
    char horaire_representation[TAILLE_LIBELLE];
    char id_salle[TAILLE_ID];
    char nom_salle[TAILLE_NOM];
    char id_categorie_billet[TAILLE_ID];
    char categorie_billet[TAILLE_LIBELLE];
    char id_cat_profil[TAILLE_ID];
    char profil_tarif[TAILLE_LIBELLE];
    char type_tarif[TAILLE_LIBELLE];
    int tarif_brut;
    char nom_spectateur[TAILLE_NOM];
    char prenom_spectateur[TAILLE_NOM];
    char tel_spectateur[TAILLE_TEL];
    char email_spectateur[TAILLE_EMAIL];
};

//Structure du fichier 09abonnes.txt
struct ABONNE
{
    char id_abonne[TAILLE_ID];
    char id_compte[TAILLE_ID]; //compte lie a l'abonne, vide si aucun compte
    char nom[TAILLE_NOM];
    char prenom[TAILLE_NOM];
    char date_naissance[TAILLE_DATE];
    char email[TAILLE_EMAIL];
    char ville[TAILLE_NOM];
    char cp[TAILLE_TEL];
    char tel[TAILLE_TEL];
    char id_cat_abonne[TAILLE_ID];
    int actif;
};

//Variables globales
extern struct SPECTACLE tab_spectacles[MAX_SPECTACLES];
extern struct SALLE tab_salles[MAX_SALLES];
extern struct CATEGORIE_BILLET_SALLE tab_categories[MAX_CATEGORIES];
extern struct CATEGORIE_HORAIRE tab_horaires[MAX_HORAIRES];
extern struct CATEGORIE_PROFIL tab_profils[MAX_PROFILS];
extern struct REPRESENTATION tab_representations[MAX_REPRESENTATIONS];
extern struct TARIF tab_tarifs[MAX_TARIFS];
extern struct COMPTE tab_comptes[MAX_COMPTES];
extern struct RESERVATION tab_reservations[MAX_RESERVATIONS];
extern struct ABONNE tab_abonnes[MAX_ABONNES];

extern int nbspectacles;
extern int nbsalles;
extern int nbcategories;
extern int nbhoraires;
extern int nbprofils;
extern int nbrepresentations;
extern int nbtarifs;
extern int nbcomptes;
extern int nbreservations;
extern int nbabonnes;
extern int indice_compte_connecte;
extern int saison_chargee;

//Declarations preliminaires des procedures et fonctions

/*chargement.c*/

//Initialisation des donnees
void initialiser_donnees();
void initialiser_donnees_systeme();

//Chargement des fichiers de donneees de la saison
int charger_spectacles(char nom_fichier[]);
int charger_salles(char nom_fichier[]);
int charger_categories(char nom_fichier[]);
int charger_horaires(char nom_fichier[]);
int charger_profils(char nom_fichier[]);
int charger_representations(char nom_fichier[]);
int charger_tarifs(char nom_fichier[]);
int charger_toutes_les_donnees();

//Chargement manuel de la saison par l'admin et sauvegarde de l'etat de chargement
int charger_saison_admin();
int charger_saison_si_active();
int lire_etat_saison();
void ecrire_etat_saison(int etat);

//Controle avant consultation
int verifier_donnees_consultation();

//Initialisation et archivage d'une saison par l'administrateur
int initialiser_nouvelle_saison();
void sauvegarder_toutes_les_donnees();
void archiver_toutes_les_donnees();
void demander_chargement_depart_admin();

//Sauvegarde des fichier
int sauvegarder_spectacles(char nom_fichier[]);
int sauvegarder_representations(char nom_fichier[]);
void sauvegarder_spectacles_source_et_archive();
void sauvegarder_representations_source_et_archive();
void sauvegarder_abonnes_source_et_archive();
void sauvegarder_reservations_source_et_archive();

//Fonctions generales de saisie, lecture clavier et confirmation
void vider_buffer_clavier();
int lire_entier(char ch[]);
void lire_mot(char ch[], char mot[]);
void lire_chaine(char ch[], char chaine[]);
void lire_chaine_taille(char ch[], char chaine[], int taille_max);
void attendre_retour_etape(char message[]);
int confirmer_action();

/*consultation.c*/

//Affichage et choix des spectacles
void afficher_spectacles();
int choisir_spectacle();

//Affichage et choix des representations par spectacle ou par salle
void afficher_representations_spectacle(char id_spectacle[]);
int choisir_representation_spectacle(char id_spectacle[]);
void afficher_spectacles_salle(char id_salle[]);
int choisir_spectacle_salle(char id_salle[]);
void afficher_representations_spectacle_salle(char id_spectacle[], char id_salle[]);
int choisir_representation_spectacle_salle(char id_spectacle[], char id_salle[]);

//Affichage des tarifs
int choisir_suite_tarifs();
void afficher_tarifs_representation(int indice_representation);

//Menus principaux de consultation
void consulter_par_spectacle();
void consulter_par_salle();
void menu_consultation();

//Verification de disponibilite et statut passe et a venir
int representation_disponible(int indice_representation);
int spectacle_est_passe(char id_spectacle[], char id_salle[]);

//Parcours de consultation
int choisir_spectacle_consultation(char id_salle[], int avec_salle, int *spectacle_passe);
int choisir_representation_consultation(char id_spectacle[], char id_salle[], int avec_salle);
int choisir_categorie_consultation(int indice_representation);

//Choix des tarifs
int choisir_tarif_categorie_reservation(int indice_representation, char id_categorie_billet[], int indice_reservation_ignoree);
int choisir_tarif_categorie_pour_compte(int indice_representation, char id_categorie_billet[], int indice_compte, int sans_compte);
int choisir_compte_user_ou_abonne();

//Association entre reservation et la consultation 
int reserver_depuis_consultation(int indice_representation, int indice_categorie, int indice_tarif);

//Controle des tarifs abonnes
int tarif_est_abonne(int indice_tarif);
void categorie_abonne_pour_tarif(int indice_tarif, char id_cat_abonne[]);
int tarif_abonne_compatible_avec_compte(int indice_tarif, int indice_compte);

//Correspondance entre tarif et representation
int tarif_correspond_representation(int indice_tarif, int indice_representation);

/*admin.c*/
//menus
void menu_gestion();
void menu_gestion_spectacles();
void menu_gestion_representations();

//Gestion des spectacles
void ajouter_spectacle();
void supprimer_spectacle();
void modifier_spectacle();

//Gestion des representations
void ajouter_representation();
void supprimer_representation();
void afficher_jauges_representation_admin();

//Generation automatique des identifiants
void generer_id_spectacle(char id_spectacle[]);
void generer_id_representation(char id_representation[]);

//Aide a la saisie d'une representation
int choisir_jour(char jour[]);
int choisir_salle();
void concatener_libelle_horaire(char jour[], char heure[], char libelle[]);
void generer_id_cat_representation(char libelle[], char id_cat[]);
void afficher_tarifs_generes_representation(int indice_representation);

//Recherche
int extraire_nombre_id(char id[]);
int rechercher_spectacle_par_id(char id_spectacle[]);
int rechercher_representation_par_id(char id_representation[]);
int rechercher_salle_par_id(char id_salle[]);

//Suppression des donnees liees a un spectacle
void supprimer_representations_spectacle(char id_spectacle[]);

/*authentification.c*/
//Chargement, sauvegarde et creation des comptes
int charger_comptes(char nom_fichier[]);
int sauvegarder_comptes(char nom_fichier[]);
void creer_comptes_defaut();

//Connexion et affichage du compte connecte
int authentifier();
void afficher_compte_connecte();

//Controle du type de compte
int compte_est_admin();
int compte_est_abonne();
int compte_est_utilisateur();

//Choix d'un compte dans la liste
int choisir_compte();

/*reservation.c*/
//Chargement, et sauvegarde des reservations
int charger_reservations(char nom_fichier[]);
int sauvegarder_reservations(char nom_fichier[]);
void generer_id_reservation(char id_reservation[]);

//Menus de reservation
void menu_reservation_admin();
void menu_reservation_utilisateur();

//Creation et suppression des reservations
void creer_reservation_admin();
void creer_reservation_utilisateur(int indice_compte);
void supprimer_reservation_admin();

//Consultation des reservations
void consulter_reservations_compte(int indice_compte);
void consulter_reservations_representation();

//Export des reservations
void exporter_reservations_representation();
void exporter_reservations_representation_indice(int indice_representation);
void proposer_export_reservations_consultation(int indice_representation);

//Jauge et places restantes
void afficher_jauges_representation(int indice_representation);
int calculer_places_restantes(char id_representation[], char id_categorie_billet[]);
int representation_passee(int indice_representation);

//Choix du tarif
int choisir_tarif_reservation(int indice_representation, char droit_compte[], int indice_reservation_ignoree);

//Sauvegarde ou suppression des reservations liees a une representation
int sauvegarder_reservations_representation_supprimee(int indice_representation);
void supprimer_reservations_representation(char id_representation[]);

//controle du nombre de billets
int choisir_nombre_billets_tarif(int places_restantes, int indice_representation, int indice_tarif, int indice_abonne);
int choisir_nombre_billets_tarif_beneficiaire(int places_restantes, int indice_representation, int indice_tarif, int indice_abonne, int mode_beneficiaire, int indice_compte, int indice_abonne_liste);

//Compter des billets deja reserves par beneficiaire
int compter_billets_compte_representation(int indice_representation, int indice_compte);
int compter_billets_beneficiaire_representation(int indice_representation, int mode_beneficiaire, int indice_compte, int indice_abonne);

//Parcours pour la gestion des reservations
int choisir_representation_parcours_reservation();
int choisir_reservation_representation(int indice_representation);
void afficher_reservations_representation_indice(int indice_representation);

/*abonnes.c*/
//Chargement, sauvegarde et creation des abonnes
int charger_abonnes(char nom_fichier[]);
int sauvegarder_abonnes(char nom_fichier[]);
void creer_abonnes_defaut();

//Menu
void menu_abonnes();

//Ajout, modification, inactivation et activation
void ajouter_abonne();
void modifier_abonne();
void supprimer_abonne();
void activer_abonne();

//Affichage et generation des identifiants abonnes
void afficher_abonnes();
void generer_id_abonne(char id_abonne[], char id_cat_abonne[]);

//Affichage selon le statut actif/inactif
void afficher_abonnes_par_actif(int actif);
int choisir_abonne_par_actif(int actif);

//Recherche du lien entre compte et liste abonne
int choisir_abonne_actif_categorie(char id_cat_abonne[]);
int rechercher_abonne_actif_compte(int indice_compte);
int rechercher_abonne_compte(int indice_compte);

//Informations personnelles abonne et compte
void afficher_infos_abonne(int indice);
void modifier_mes_infos_abonne();
void afficher_infos_compte(int indice);
void modifier_mes_infos_compte();