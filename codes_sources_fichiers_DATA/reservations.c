/* Gestion des reservations et des places restantes(jauge).
Les places restantes sont calculees par categorie de billet.
Les tarifs Jeune, Adulte ou Abonne d'une meme categorie partagent donc la meme jauge. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h> //calcul du localtime pour filtrer les representations passee et a venir
#include "declarations.h"

void lire_champ_reservation(char ligne[], int *position, char champ[]);
int choisir_representation_toutes();
int choisir_reservation_toutes();
int compte_par_id(char id_compte[]);
int chaine_contient_abonn(char ch[]);
int tarif_autorise_pour_droit(int indice_tarif, char droit_compte[]);
int tarif_autorise_pour_consultation(int indice_tarif);
int tarif_correspond_representation(int indice_tarif, int indice_representation);
void categorie_abonne_pour_tarif(int indice_tarif, char id_cat_abonne[]);
int tarif_abonne_compatible_avec_compte(int indice_tarif, int indice_compte);
int tarif_autorise_pour_reservation_compte(int indice_tarif, int indice_compte, int sans_compte);
int choisir_compte_user_ou_abonne();
int choisir_tarif_categorie_pour_compte(int indice_representation, char id_categorie_billet[], int indice_compte, int sans_compte);
int tarif_autorise_pour_beneficiaire_admin(int indice_tarif, int mode_beneficiaire, int indice_compte, int indice_abonne);
int choisir_tarif_categorie_pour_beneficiaire_admin(int indice_representation, char id_categorie_billet[], int mode_beneficiaire, int indice_compte, int indice_abonne);
int champ_est_retour(char saisie[]);
int champ_est_obligatoire_vide(int champ, char saisie[]);
int choisir_nombre_billets_tarif(int places_restantes, int indice_representation, int indice_tarif, int indice_abonne);
int compter_billets_abonne_representation(int indice_representation, int indice_abonne, int type_compteur);
int compter_billets_compte_representation(int indice_representation, int indice_compte);
int compter_billets_beneficiaire_representation(int indice_representation, int mode_beneficiaire, int indice_compte, int indice_abonne);
int choisir_nombre_billets_tarif_beneficiaire(int places_restantes, int indice_representation, int indice_tarif, int indice_abonne, int mode_beneficiaire, int indice_compte, int indice_abonne_liste);
int reservation_appartient_abonne(int indice_reservation, int indice_abonne);
int tarif_famille_adulte(int indice_tarif);
int tarif_famille_jeune(int indice_tarif);
int rechercher_indice_abonne_beneficiaire(int indice_tarif, int mode_beneficiaire, int indice_compte_reservation, int indice_abonne_liste);
int rechercher_jauge_initiale(char id_salle[], char id_categorie_billet[]);
int saisir_infos_spectateur(int indice_compte, struct RESERVATION *reservation);
int saisir_infos_spectateur_libre(struct RESERVATION *reservation);
void copier_abonne_dans_reservation(int indice_abonne, struct RESERVATION *reservation);
void remplir_reservation(struct RESERVATION *reservation, int indice_compte, int indice_representation, int indice_tarif);
void afficher_recap_reservation(struct RESERVATION reservation);
void afficher_ligne_reservation(int numero, struct RESERVATION reservation);
void sauvegarder_reservations_auto();

//Lecture d'un champ separe par un point-virgule
void lire_champ_reservation(char ligne[], int *position, char champ[])
{
    int i;

    i = 0;

    while ((ligne[*position] != ';') && (ligne[*position] != '\0') && (ligne[*position] != '\n'))
    {
        champ[i] = ligne[*position];
        i++;
        (*position)++;
    }

    champ[i] = '\0';

    if (ligne[*position] == ';')
    {
        (*position)++;
    }
}


//Chargement du fichier des reservations. 
// Si le fichier n'existe pas encore, on considere qu'il n'y a aucune reservation
int charger_reservations(char nom_fichier[])
{
    FILE *fentree;
    char ligne[TAILLE_LIGNE];
    char champ[TAILLE_LIBELLE];
    int position;
    struct RESERVATION reservation;

    nbreservations = 0;
    fentree = fopen(nom_fichier, "r");

    if (fentree == NULL)
    {
        printf("Aucun fichier de reservations charge.\n");
    }
    else
    {
        fgets(ligne, TAILLE_LIGNE, fentree);

        while ((fgets(ligne, TAILLE_LIGNE, fentree) != NULL) && (nbreservations < MAX_RESERVATIONS))
        {
            if (strcmp(ligne, "") != 0)
            {
                position = 0;
                lire_champ_reservation(ligne, &position, reservation.id_reservation);
                lire_champ_reservation(ligne, &position, reservation.id_compte);
                lire_champ_reservation(ligne, &position, reservation.id_representation);
                lire_champ_reservation(ligne, &position, reservation.id_spectacle);
                lire_champ_reservation(ligne, &position, reservation.nom_spectacle);
                lire_champ_reservation(ligne, &position, reservation.date_representation);
                lire_champ_reservation(ligne, &position, reservation.horaire_representation);
                lire_champ_reservation(ligne, &position, reservation.id_salle);
                lire_champ_reservation(ligne, &position, reservation.nom_salle);
                lire_champ_reservation(ligne, &position, reservation.id_categorie_billet);
                lire_champ_reservation(ligne, &position, reservation.categorie_billet);
                lire_champ_reservation(ligne, &position, reservation.id_cat_profil);
                lire_champ_reservation(ligne, &position, reservation.profil_tarif);
                lire_champ_reservation(ligne, &position, reservation.type_tarif);
                lire_champ_reservation(ligne, &position, champ);
                reservation.tarif_brut = atoi(champ);
                lire_champ_reservation(ligne, &position, reservation.nom_spectateur);
                lire_champ_reservation(ligne, &position, reservation.prenom_spectateur);
                lire_champ_reservation(ligne, &position, reservation.tel_spectateur);
                lire_champ_reservation(ligne, &position, reservation.email_spectateur);

                if (strcmp(reservation.id_reservation, "") != 0)
                {
                    tab_reservations[nbreservations] = reservation;
                    nbreservations++;
                }
            }
        }
        fclose(fentree);
    }

    return nbreservations;
}

//Sauvegarde de toutes les reservations.
int sauvegarder_reservations(char nom_fichier[])
{
    FILE *fsortie;
    int i;

    fsortie = fopen(nom_fichier, "w");

    if (fsortie == NULL)
    {
        printf("Pb acces fichier %s\n", nom_fichier);
        return 0;
    }
    else
    {
        fprintf(fsortie, "id_reservation;id_compte;id_representation;id_spectacle;nom_spectacle;date_representation;horaire_representation;id_salle;nom_salle;id_categorie_billet;categorie_billet;id_cat_profil;profil_tarif;type_tarif;tarif_brut;nom_spectateur;prenom_spectateur;tel_spectateur;email_spectateur\n");

        for (i = 0 ; i < nbreservations ; i++)
        {
            fprintf(fsortie, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%d;%s;%s;%s;%s\n",
                    tab_reservations[i].id_reservation,
                    tab_reservations[i].id_compte,
                    tab_reservations[i].id_representation,
                    tab_reservations[i].id_spectacle,
                    tab_reservations[i].nom_spectacle,
                    tab_reservations[i].date_representation,
                    tab_reservations[i].horaire_representation,
                    tab_reservations[i].id_salle,
                    tab_reservations[i].nom_salle,
                    tab_reservations[i].id_categorie_billet,
                    tab_reservations[i].categorie_billet,
                    tab_reservations[i].id_cat_profil,
                    tab_reservations[i].profil_tarif,
                    tab_reservations[i].type_tarif,
                    tab_reservations[i].tarif_brut,
                    tab_reservations[i].nom_spectateur,
                    tab_reservations[i].prenom_spectateur,
                    tab_reservations[i].tel_spectateur,
                    tab_reservations[i].email_spectateur);
        }
        fclose(fsortie);
    }

    return 1;
}

//Renouvellement automatique du fichier en cas de creation ou suppression de reservation
void sauvegarder_reservations_auto()
{
    sauvegarder_reservations("DATA/08reservations.txt");
}

//Generation d'un identifiant
void generer_id_reservation(char id_reservation[])
{
    int i;
    int max_id;
    int numero;

    max_id = 0;

    for (i = 0 ; i < nbreservations ; i++)
    {
        numero = extraire_nombre_id(tab_reservations[i].id_reservation);
        if (numero > max_id)
        {
            max_id = numero;
        }
    }

    sprintf(id_reservation, "RES%04d", max_id + 1);
}

//Recherche d'un compte par son identifiant. */
int compte_par_id(char id_compte[])
{
    int i;

    for (i = 0 ; i < nbcomptes ; i++)
    {
        if (strcmp(tab_comptes[i].id_compte, id_compte) == 0)
        {
            return i;
        }
    }

    return NON_TROUVE;
}

//Recherche simple du mot "abonn" sans tenir compte des majuscules
//On ne teste que les lettres utiles pour accepter abonne ou abonne accentue
int chaine_contient_abonn(char ch[])
{
    int i;
    char c1, c2, c3, c4, c5;

    i = 0;
    while (ch[i] != '\0')
    {
        c1 = tolower(ch[i]);
        c2 = tolower(ch[i + 1]);
        c3 = tolower(ch[i + 2]);
        c4 = tolower(ch[i + 3]);
        c5 = tolower(ch[i + 4]);

        if ((c1 == 'a') && (c2 == 'b') && (c3 == 'o') && (c4 == 'n') && (c5 == 'n'))
        {
            return 1;
        }
        i++;
    }

    return 0;
}

/* Filtrage des tarifs selon le droit du compte. 
Un utilisateur simple ne voit pas les tarifs abonnes
Un abonne et un administrateur peuvent choisir les tarifs standards et abonnes. */
int tarif_autorise_pour_droit(int indice_tarif, char droit_compte[])
{
    int tarif_abonne;

    tarif_abonne = 0;

    if (chaine_contient_abonn(tab_tarifs[indice_tarif].type_tarif) ||
        chaine_contient_abonn(tab_tarifs[indice_tarif].profil_tarif))
    {
        tarif_abonne = 1;
    }

    if (strcmp(droit_compte, "utilisateur") == 0)
    {
        return (! tarif_abonne);
    }

    return 1;
}

//Tester si un tarif est reserve aux abonnes
int tarif_est_abonne(int indice_tarif)
{
    if ((indice_tarif < 0) || (indice_tarif >= nbtarifs))
    {
        return 0;
    }

    if (chaine_contient_abonn(tab_tarifs[indice_tarif].type_tarif) || chaine_contient_abonn(tab_tarifs[indice_tarif].profil_tarif))
    {
        return 1;
    }

    return 0;
}

/* Associer la categorie d'abonne pour un tarif abonne
CP009 = Carte Adulte - AA
CP010 = Carte Jeune - AJ
CP011 / CP012 = Carte Famille - AF
*/
void categorie_abonne_pour_tarif(int indice_tarif, char id_cat_abonne[])
{
    strcpy(id_cat_abonne, "");

    if ((indice_tarif >= 0) && (indice_tarif < nbtarifs))
    {
        if (strcmp(tab_tarifs[indice_tarif].id_cat_profil, "CP009") == 0)
        {
            strcpy(id_cat_abonne, "AA");
        }
        else if (strcmp(tab_tarifs[indice_tarif].id_cat_profil, "CP010") == 0)
        {
            strcpy(id_cat_abonne, "AJ");
        }
        else if ((strcmp(tab_tarifs[indice_tarif].id_cat_profil, "CP011") == 0) ||
                 (strcmp(tab_tarifs[indice_tarif].id_cat_profil, "CP012") == 0))
        {
            strcpy(id_cat_abonne, "AF");
        }
    }
}

/* Verifier que le compte abonne connecte correspond bien a la categorie du tarif choisi.
Exemple : Carte Adulte -> seulement affiche pour un abonne actif de categorie AA. */
int tarif_abonne_compatible_avec_compte(int indice_tarif, int indice_compte)
{
    int indice_abonne;
    char id_cat_abonne[TAILLE_ID];

    if (! tarif_est_abonne(indice_tarif))
    {
        return 1;
    }

    categorie_abonne_pour_tarif(indice_tarif, id_cat_abonne);
    indice_abonne = rechercher_abonne_actif_compte(indice_compte);

    if (indice_abonne == NON_TROUVE)
    {
        return 0;
    }

    if (strcmp(id_cat_abonne, "") == 0)
    {
        return 1;
    }

    if (strcmp(tab_abonnes[indice_abonne].id_cat_abonne, id_cat_abonne) == 0)
    {
        return 1;
    }

    return 0;
}

/* Filtrer des tarifs pendant la consultation.
Un administrateur voit les tarifs abonnes mais il devra ensuite choisir un abonne actif.
Un compte abonne ne voit que les tarifs compatibles avec sa categorie d'abonnement. */
int tarif_autorise_pour_consultation(int indice_tarif)
{
    if (! tarif_est_abonne(indice_tarif))
    {
        return 1;
    }

    if (compte_est_admin())
    {
        return 1;
    }

    if (compte_est_abonne())
    {
        return tarif_abonne_compatible_avec_compte(indice_tarif, indice_compte_connecte);
    }

    return 0;
}

/* Filtrer des tarifs quand l'administrateur reserve pour un beneficiaire.
sans_compte vaut 1 quand le spectateur n'a pas de compte : dans ce cas seuls les tarifs normaux sont proposes. */
int tarif_autorise_pour_reservation_compte(int indice_tarif, int indice_compte, int sans_compte)
{
    if (sans_compte)
    {
        return (! tarif_est_abonne(indice_tarif));
    }

    if ((indice_compte < 0) || (indice_compte >= nbcomptes))
    {
        return 0;
    }

    if (strcmp(tab_comptes[indice_compte].droit_compte, "utilisateur") == 0)
    {
        return (! tarif_est_abonne(indice_tarif));
    }

    if (strcmp(tab_comptes[indice_compte].droit_compte, "abonne") == 0)
    {
        if (! tarif_est_abonne(indice_tarif))
        {
            return 1;
        }

        return tarif_abonne_compatible_avec_compte(indice_tarif, indice_compte);
    }

    return 1;
}

/* Tester si une saisie demande un retour au champ precedent.
Cette fonction evite de repeter plusieurs fois les memes strcmp. */
int champ_est_retour(char saisie[])
{
    if ((strcmp(saisie, "9") == 0) ||
        (strcmp(saisie, "RETOUR") == 0) ||
        (strcmp(saisie, "retour") == 0) ||
        (strcmp(saisie, "Retour") == 0))
    {
        return 1;
    }

    return 0;
}

/* Filtrer des tarifs pour l'administrateur selon le type de beneficiaire.
mode_beneficiaire :
1 = compte utilisateur ou abonne,
2 = spectateur sans compte et n'est pas dans la liste d'abonne,
3 = abonne actif de la liste. */
int tarif_autorise_pour_beneficiaire_admin(int indice_tarif, int mode_beneficiaire, int indice_compte, int indice_abonne)
{
    char id_cat_abonne[TAILLE_ID];

    if (mode_beneficiaire == 2)
    {
        return (! tarif_est_abonne(indice_tarif));
    }

    if (mode_beneficiaire == 1)
    {
        return tarif_autorise_pour_reservation_compte(indice_tarif, indice_compte, 0);
    }

    if (mode_beneficiaire == 3)
    {
        if ((indice_abonne < 0) || (indice_abonne >= nbabonnes))
        {
            return 0;
        }

        if (tab_abonnes[indice_abonne].actif != 1)
        {
            return 0;
        }

        if (! tarif_est_abonne(indice_tarif))
        {
            return 1;
        }

        categorie_abonne_pour_tarif(indice_tarif, id_cat_abonne);

        if (strcmp(id_cat_abonne, "") == 0)
        {
            return 1;
        }

        if (strcmp(tab_abonnes[indice_abonne].id_cat_abonne, id_cat_abonne) == 0)
        {
            return 1;
        }
    }

    return 0;
}

/* Verifier si une reservation deja enregistree correspond a un abonne.
On compare d'abord l'email, puis le nom et le prenom si l'email n'est pas disponible.
Cela marche pour les abonnes avec compte et pour les abonnes de la liste sans compte. */
int reservation_appartient_abonne(int indice_reservation, int indice_abonne)
{
    if ((indice_reservation < 0) || (indice_reservation >= nbreservations))
    {
        return 0;
    }

    if ((indice_abonne < 0) || (indice_abonne >= nbabonnes))
    {
        return 0;
    }

    if ((strcmp(tab_abonnes[indice_abonne].email, "") != 0) && (strcmp(tab_reservations[indice_reservation].email_spectateur, tab_abonnes[indice_abonne].email) == 0))
    {
        return 1;
    }

    if ((strcmp(tab_reservations[indice_reservation].nom_spectateur, tab_abonnes[indice_abonne].nom) == 0) &&
        (strcmp(tab_reservations[indice_reservation].prenom_spectateur, tab_abonnes[indice_abonne].prenom) == 0))
    {
        return 1;
    }

    return 0;
}

//Reconnait le tarif adulte d'une carte famille - AF - CP011
int tarif_famille_adulte(int indice_tarif)
{
    if ((indice_tarif < 0) || (indice_tarif >= nbtarifs))
    {
        return 0;
    }

    if (strcmp(tab_tarifs[indice_tarif].id_cat_profil, "CP011") == 0)
    {
        return 1;
    }

    return 0;
}

// Reconnait le tarif jeune d'une carte famille - AJ - CP012
int tarif_famille_jeune(int indice_tarif)
{
    if ((indice_tarif < 0) || (indice_tarif >= nbtarifs))
    {
        return 0;
    }

    if (strcmp(tab_tarifs[indice_tarif].id_cat_profil, "CP012") == 0)
    {
        return 1;
    }

    return 0;
}

/* Compter les billets deja pris par un meme abonne pour une representation.
type_compteur :
1 = Carte Adulte
2 = Carte Jeune
3 = Carte Famille total
4 = Carte Famille adulte
5 = Carte Famille jeune. */
int compter_billets_abonne_representation(int indice_representation, int indice_abonne, int type_compteur)
{
    int i;
    int compteur;

    compteur = 0;

    if ((indice_representation < 0) || (indice_representation >= nbrepresentations))
    {
        return 0;
    }

    for (i = 0 ; i < nbreservations ; i++)
    {
        if (strcmp(tab_reservations[i].id_representation, tab_representations[indice_representation].id_representation) == 0)
        {
            if (reservation_appartient_abonne(i, indice_abonne))
            {
                if ((type_compteur == 1) && (strcmp(tab_reservations[i].id_cat_profil, "CP009") == 0))
                {
                    compteur++;
                }
                else if ((type_compteur == 2) && (strcmp(tab_reservations[i].id_cat_profil, "CP010") == 0))
                {
                    compteur++;
                }
                else if ((type_compteur == 3) &&
                         ((strcmp(tab_reservations[i].id_cat_profil, "CP011") == 0) || (strcmp(tab_reservations[i].id_cat_profil, "CP012") == 0)))
                {
                    compteur++;
                }
                else if ((type_compteur == 4) && (strcmp(tab_reservations[i].id_cat_profil, "CP011") == 0))
                {
                    compteur++;
                }
                else if ((type_compteur == 5) && (strcmp(tab_reservations[i].id_cat_profil, "CP012") == 0))
                {
                    compteur++;
                }
            }
        }
    }

    return compteur;
}

/* Compter les billets deja reserves par un compte pour une representation.
Une reservation d'une place correspond a une ligne dans le fichier txt de reservation*/
int compter_billets_compte_representation(int indice_representation, int indice_compte)
{
    int i;
    int compteur;

    compteur = 0;

    if ((indice_representation < 0) || (indice_representation >= nbrepresentations))
    {
        return 0;
    }

    if ((indice_compte < 0) || (indice_compte >= nbcomptes))
    {
        return 0;
    }

    for (i = 0 ; i < nbreservations ; i++)
    {
        if ((strcmp(tab_reservations[i].id_representation,tab_representations[indice_representation].id_representation) == 0) &&
            (strcmp(tab_reservations[i].id_compte,tab_comptes[indice_compte].id_compte) == 0))
        {
            compteur++;
        }
    }

    return compteur;
}

/* Compter les billets deja reserves pour le beneficiaire courant.
mode_beneficiaire garde la meme logique que dans le menu :
1 = compte utilisateur ou abonne, 2 = sans compte et non abonne, 3 = abonne de la liste. */
int compter_billets_beneficiaire_representation(int indice_representation, int mode_beneficiaire, int indice_compte, int indice_abonne)
{
    int i;
    int compteur;

    compteur = 0;

    if ((indice_representation < 0) || (indice_representation >= nbrepresentations))
    {
        return 0;
    }

    if (mode_beneficiaire == 1)
    {
        compteur = compter_billets_compte_representation(indice_representation, indice_compte);
    }
    else if (mode_beneficiaire == 3)
    {
        for (i = 0 ; i < nbreservations ; i++)
        {
            if (strcmp(tab_reservations[i].id_representation, tab_representations[indice_representation].id_representation) == 0)
            {
                if (reservation_appartient_abonne(i, indice_abonne))
                {
                    compteur++;
                }
            }
        }
    }

    return compteur;
}

/* Contrôle le nombre de billets en tenant compte du beneficiaire.
Les tarifs abonnes gardent les limites de carte.
Les autres tarifs sont limites a 10 billets au total pour un meme compte ou un meme abonne sur une representation. */
int choisir_nombre_billets_tarif_beneficiaire(int places_restantes, int indice_representation, int indice_tarif, int indice_abonne, int mode_beneficiaire, int indice_compte, int indice_abonne_liste)
{
    int deja_reserve;
    int maximum_beneficiaire;

    deja_reserve = 0;

    if (compte_est_admin())
    {
        if (mode_beneficiaire == 1)
        {
            deja_reserve = compter_billets_beneficiaire_representation(indice_representation,1,indice_compte,NON_TROUVE);
        }
        else if (mode_beneficiaire == 3)
        {
            deja_reserve = compter_billets_beneficiaire_representation(indice_representation,3,NON_TROUVE,indice_abonne_liste);
        }
    }
    else
    {
        deja_reserve = compter_billets_beneficiaire_representation(indice_representation,1,indice_compte_connecte,NON_TROUVE);
    }

    if (deja_reserve >= 10)
    {
        printf("Limite atteinte : ce beneficiaire a deja 10 billets pour cette representation.\n");
        printf("Il ne peut plus reserver pour cette representation.\n");
        return 0;
    }

    maximum_beneficiaire = 10 - deja_reserve;

    if (places_restantes > maximum_beneficiaire)
    {
        printf("Nombre limite de reservation : il reste %d billet(s) possible(s) sur cette representation.\n",
               maximum_beneficiaire);
        return choisir_nombre_billets_tarif(maximum_beneficiaire,indice_representation,indice_tarif,indice_abonne);
    }

    return choisir_nombre_billets_tarif(places_restantes,indice_representation,indice_tarif,indice_abonne);
}

// Identifier l'abonne a utiliser pour les contrôles du nombre des reservation par tarifs abonnes.
int rechercher_indice_abonne_beneficiaire(int indice_tarif, int mode_beneficiaire, int indice_compte_reservation, int indice_abonne_liste)
{
    int indice_abonne;

    indice_abonne = NON_TROUVE;

    if (! tarif_est_abonne(indice_tarif))
    {
        return NON_TROUVE;
    }

    if (compte_est_admin())
    {
        if (mode_beneficiaire == 1)
        {
            if ((indice_compte_reservation >= 0) && (indice_compte_reservation < nbcomptes))
            {
                if (strcmp(tab_comptes[indice_compte_reservation].droit_compte, "abonne") == 0)
                {
                    indice_abonne = rechercher_abonne_actif_compte(indice_compte_reservation);
                }
            }
        }
        else if (mode_beneficiaire == 3)
        {
            indice_abonne = indice_abonne_liste;
        }
    }
    else if (compte_est_abonne())
    {
        indice_abonne = rechercher_abonne_actif_compte(indice_compte_connecte);
    }

    return indice_abonne;
}

/* Choix du nombre de billets.
Pour un tarif normal, le nombre doit etre entre 1 et 10, ou 0 pour retourner.
Pour un tarif abonne, on applique la limite de la carte :
Carte Adulte = 2 places, Carte Jeune = 2 places,
Carte Famille = 6 places avec au maximum 2 adultes et 4 jeunes. */
int choisir_nombre_billets_tarif(int places_restantes, int indice_representation, int indice_tarif, int indice_abonne)
{
    int choix;
    int maximum;
    int reste_carte;
    int total_famille;
    int adultes_famille;
    int jeunes_famille;
    char id_cat_abonne[TAILLE_ID];

    maximum = places_restantes;

    if (! tarif_est_abonne(indice_tarif))
    {
        if (maximum > 10)
        {
            maximum = 10;
        }
    }

    if (tarif_est_abonne(indice_tarif))
    {
        categorie_abonne_pour_tarif(indice_tarif, id_cat_abonne);
        reste_carte = 0;

        if ((indice_abonne < 0) || (indice_abonne >= nbabonnes))
        {
            printf("Aucun abonne actif trouve pour contrôler la limite de reservation par carte.\n");
            return 0;
        }

        if (strcmp(id_cat_abonne, "AA") == 0)
        {
            reste_carte = 2 - compter_billets_abonne_representation(indice_representation, indice_abonne, 1);
            printf("Carte Adulte : 2 places maximum par representation.\n");
        }
        else if (strcmp(id_cat_abonne, "AJ") == 0)
        {
            reste_carte = 2 - compter_billets_abonne_representation(indice_representation, indice_abonne, 2);
            printf("Carte Jeune : 2 places maximum par representation.\n");
        }
        else if (strcmp(id_cat_abonne, "AF") == 0)
        {
            total_famille = compter_billets_abonne_representation(indice_representation, indice_abonne, 3);
            adultes_famille = compter_billets_abonne_representation(indice_representation, indice_abonne, 4);
            jeunes_famille = compter_billets_abonne_representation(indice_representation, indice_abonne, 5);

            if (tarif_famille_adulte(indice_tarif))
            {
                reste_carte = 2 - adultes_famille;
                printf("Carte Famille : 1 ou 2 adultes maximum par representation.\n");
            }
            else if (tarif_famille_jeune(indice_tarif))
            {
                if (adultes_famille <= 0)
                {
                    printf("Carte Famille : il faut d'abord reserver 1 ou 2 adultes avec le tarif famille adulte.\n");
                    reste_carte = 0;
                }
                else
                {
                    reste_carte = 4 - jeunes_famille;
                    printf("Carte Famille : 1 a 4 jeunes moins de 28 ans par representation.\n");
                }
            }
            else
            {
                reste_carte = 6 - total_famille;
            }

            if (reste_carte > (6 - total_famille))
            {
                reste_carte = 6 - total_famille;
            }

            printf("Carte Famille : 6 places maximum par representation.\n");
        }
        else
        {
            reste_carte = maximum;
        }

        if (reste_carte < maximum)
        {
            maximum = reste_carte;
        }
    }

    if (maximum <= 0)
    {
        if (tarif_est_abonne(indice_tarif))
        {
            printf("Limite de places abonnee atteinte pour cette representation.\n");
        }
        else
        {
            printf("Cette categorie est complete.\n");
        }
        return 0;
    }

    choix = -1;

    while ((choix != 0) && ((choix < 1) || (choix > maximum)))
    {
        printf("\nReservation:\n");
        printf("Places restantes : %d\n", places_restantes);
        printf("Maximum des billets autorises pour cette representation : %d\n", maximum);
        printf("0 - Retour au choix du tarif\n");
        choix = lire_entier("Nombre de billets : ");

        if ((choix != 0) && ((choix < 1) || (choix > maximum)))
        {
            printf("Choix incorrect. Le nombre doit etre entre 1 et %d, ou entrez 0 pour retourner.\n", maximum);
        }
    }

    return choix;
}

/* Choix d'un compte beneficiaire par l'administrateur.
On affiche uniquement les comptes utilisateur et abonne */
int choisir_compte_user_ou_abonne()
{
    int i;
    int choix;
    int compteur;
    int indices[MAX_COMPTES];

    compteur = 0;

    printf("\nComptes utilisateur et abonne dans le systeme\n");
    printf("+----------------------------------------------------------------------------------------------------------------+\n");
    printf("%-5s %-18s %-14s %-18s %-18s %-35s\n", "No", "Compte", "Droit", "Nom", "Prenom", "Email");
    printf("+----------------------------------------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbcomptes ; i++)
    {
        if ((strcmp(tab_comptes[i].droit_compte, "utilisateur") == 0) ||
            (strcmp(tab_comptes[i].droit_compte, "abonne") == 0))
        {
            compteur++;
            indices[compteur - 1] = i;
            printf("%-5d %-18s %-14s %-18s %-18s %-35s\n",
                   compteur,
                   tab_comptes[i].id_compte,
                   tab_comptes[i].droit_compte,
                   tab_comptes[i].nom,
                   tab_comptes[i].prenom,
                   tab_comptes[i].email);
        }
    }

    printf("+----------------------------------------------------------------------------------------------------------------+\n");

    if (compteur == 0)
    {
        printf("Aucun compte trouve.\n");
        return NON_TROUVE;
    }

    choix = lire_entier("Choisissez un compte (0 pour Retour au choix du beneficiaire) : ");

    if ((choix < 1) || (choix > compteur))
    {
        if (choix != 0)
        {
            printf("Choix incorrect. Entrez un numero affiche ou 0 pour retourner.\n");
        }
        return NON_TROUVE;
    }

    return indices[choix - 1];
}

/* Choix d'un tarif pour une categorie, mais en fonction du compte beneficiaire.
Cette fonction sert surtout a l'administrateur : le choix du compte est fait avant le choix du tarif pour ne pas afficher de tarifs masques. */
int choisir_tarif_categorie_pour_compte(int indice_representation, char id_categorie_billet[], int indice_compte, int sans_compte)
{
    int i;
    int choix;
    int compteur;
    int restantes;
    int indices[MAX_TARIFS];
    struct REPRESENTATION rep;

    if ((indice_representation < 0) || (indice_representation >= nbrepresentations))
    {
        return NON_TROUVE;
    }

    rep = tab_representations[indice_representation];
    compteur = 0;

    printf("\nTarifs disponibles pour le beneficiaire choisi\n");
    if (! sans_compte)
    {
        printf("Compte : %s - %s %s (%s)\n",
               tab_comptes[indice_compte].id_compte,
               tab_comptes[indice_compte].prenom,
               tab_comptes[indice_compte].nom,
               tab_comptes[indice_compte].droit_compte);
    }
    else
    {
        printf("Spectateur sans compte : seuls les tarifs normaux sont proposes.\n");
    }

    printf("+------------------------------------------------------------------------------------------------------------------------+\n");
    printf("%-5s %-8s %-28s %-18s %-8s %-10s %-35s\n", "No", "Cat", "Categorie", "Type", "Tarif", "Reste", "Profil");
    printf("+------------------------------------------------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbtarifs ; i++)
    {
        if ((tarif_correspond_representation(i, indice_representation)) &&
            (strcmp(tab_tarifs[i].id_categorie_billet, id_categorie_billet) == 0) &&
            (tarif_autorise_pour_reservation_compte(i, indice_compte, sans_compte)))
        {
            restantes = calculer_places_restantes(rep.id_representation,tab_tarifs[i].id_categorie_billet);
            compteur++;
            indices[compteur - 1] = i;
            printf("%-5d %-8s %-28s %-18s %-8d %-10d %-35s\n",
                   compteur,
                   tab_tarifs[i].id_categorie_billet,
                   tab_tarifs[i].categorie_billet,
                   tab_tarifs[i].type_tarif,
                   tab_tarifs[i].tarif_brut,
                   restantes,
                   tab_tarifs[i].profil_tarif);
        }
    }

    if (compteur == 0)
    {
        printf("Aucun tarif disponible pour ce beneficiaire.\n");
        return NON_TROUVE;
    }

    printf("+------------------------------------------------------------------------------------------------------------------------+\n");
    choix = lire_entier("Choisissez un tarif (0 pour Retour a l'etape precedente) : ");

    if ((choix < 1) || (choix > compteur))
    {
        return NON_TROUVE;
    }

    restantes = calculer_places_restantes(rep.id_representation,tab_tarifs[indices[choix - 1]].id_categorie_billet);

    if (restantes <= 0)
    {
        printf("Cette categorie est complete.\n");
        return NON_TROUVE;
    }

    return indices[choix - 1];
}

// Meme principe que choisir_tarif_categorie_pour_compte, mais avec le cas d'un abonne actif dans la liste des abonnes
int choisir_tarif_categorie_pour_beneficiaire_admin(int indice_representation, char id_categorie_billet[], int mode_beneficiaire, int indice_compte, int indice_abonne)
{
    int i;
    int choix;
    int compteur;
    int restantes;
    int indices[MAX_TARIFS];
    struct REPRESENTATION rep;

    if ((indice_representation < 0) || (indice_representation >= nbrepresentations))
    {
        return NON_TROUVE;
    }

    rep = tab_representations[indice_representation];
    compteur = 0;

    printf("\nTarifs disponibles pour le beneficiaire choisi\n");

    if (mode_beneficiaire == 1)
    {
        printf("Compte : %s - %s %s (%s)\n",
               tab_comptes[indice_compte].id_compte,
               tab_comptes[indice_compte].prenom,
               tab_comptes[indice_compte].nom,
               tab_comptes[indice_compte].droit_compte);
    }
    else if (mode_beneficiaire == 2)
    {
        printf("Spectateur sans compte : seuls les tarifs normaux sont proposes.\n");
    }
    else if (mode_beneficiaire == 3)
    {
        printf("Abonne de la liste : %s %s - categorie %s\n",
               tab_abonnes[indice_abonne].prenom,
               tab_abonnes[indice_abonne].nom,
               tab_abonnes[indice_abonne].id_cat_abonne);
    }

    printf("+------------------------------------------------------------------------------------------------------------------------+\n");
    printf("%-5s %-8s %-28s %-18s %-8s %-10s %-35s\n", "No", "Cat", "Categorie", "Type", "Tarif", "Reste", "Profil");
    printf("+------------------------------------------------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbtarifs ; i++)
    {
        if ((tarif_correspond_representation(i, indice_representation)) &&
            (strcmp(tab_tarifs[i].id_categorie_billet, id_categorie_billet) == 0) &&
            (tarif_autorise_pour_beneficiaire_admin(i, mode_beneficiaire, indice_compte, indice_abonne)))
        {
            restantes = calculer_places_restantes(rep.id_representation,tab_tarifs[i].id_categorie_billet);
            compteur++;
            indices[compteur - 1] = i;
            printf("%-5d %-8s %-28s %-18s %-8d %-10d %-35s\n",
                   compteur,
                   tab_tarifs[i].id_categorie_billet,
                   tab_tarifs[i].categorie_billet,
                   tab_tarifs[i].type_tarif,
                   tab_tarifs[i].tarif_brut,
                   restantes,
                   tab_tarifs[i].profil_tarif);
        }
    }

    if (compteur == 0)
    {
        printf("Aucun tarif disponible pour ce beneficiaire.\n");
        return NON_TROUVE;
    }

    printf("+------------------------------------------------------------------------------------------------------------------------+\n");
    choix = lire_entier("Choisissez un tarif (0 pour Retour au choix du beneficiaire) : ");

    if ((choix < 1) || (choix > compteur))
    {
        return NON_TROUVE;
    }

    restantes = calculer_places_restantes(rep.id_representation,tab_tarifs[indices[choix - 1]].id_categorie_billet);

    if (restantes <= 0)
    {
        printf("Cette categorie est complete.\n");
        attendre_retour_etape("Retour au choix du tarif");
        return NON_TROUVE;
    }

    return indices[choix - 1];
}

/* Verifier si un tarif doit etre propose pour la representation choisie.
La regle normale est : meme salle, meme categorie et meme horaire.
Certains horaires de representation sont classes CH001 = Autres qui est egale a tarif indiiferencie.
Mais il y a un defaut dans le fichier info_tarif, le tarif indiiferencie n'est pas enregistre pour certaines salles,
on prend alors le premier Plein tarif adulte de la meme salle et de la meme categorie de billet.*/
int tarif_correspond_representation(int indice_tarif, int indice_representation)
{
    int i;
    int plein_tarif_deja_trouve;
    int deja_affiche_avant;
    struct REPRESENTATION rep;

    if ((indice_tarif < 0) || (indice_tarif >= nbtarifs))
    {
        return 0;
    }

    if ((indice_representation < 0) || (indice_representation >= nbrepresentations))
    {
        return 0;
    }

    rep = tab_representations[indice_representation];

    if (strcmp(tab_tarifs[indice_tarif].id_salle, rep.id_salle) != 0)
    {
        return 0;
    }

    // Cas normal : le tarif correspond exactement a l'horaire de la representation
    if (strcmp(tab_tarifs[indice_tarif].id_cat_horaire, rep.id_cat_representation) == 0)
    {
        return 1;
    }

    // le tarif horaire indifferencie qui se presente dans le fichier
    if (strcmp(tab_tarifs[indice_tarif].id_cat_horaire, "CH001") == 0)
    {
        return 1;
    }

    /* Cas de secours : si aucun Plein tarif exact ou indifferent n'existe
       pour cette salle et cette categorie, on garde le premier Plein tarif disponible. */
    if (strcmp(tab_tarifs[indice_tarif].id_cat_profil, "CP001") == 0)
    {
        plein_tarif_deja_trouve = 0;

        for (i = 0 ; i < nbtarifs ; i++)
        {
            if ((strcmp(tab_tarifs[i].id_salle, rep.id_salle) == 0) &&
                (strcmp(tab_tarifs[i].id_categorie_billet, tab_tarifs[indice_tarif].id_categorie_billet) == 0) &&
                (strcmp(tab_tarifs[i].id_cat_profil, "CP001") == 0) &&
                ((strcmp(tab_tarifs[i].id_cat_horaire, rep.id_cat_representation) == 0) ||
                 (strcmp(tab_tarifs[i].id_cat_horaire, "CH001") == 0)))
            {
                plein_tarif_deja_trouve = 1;
            }
        }

        if (! plein_tarif_deja_trouve)
        {
            deja_affiche_avant = 0;

            for (i = 0 ; i < indice_tarif ; i++)
            {
                if ((strcmp(tab_tarifs[i].id_salle, rep.id_salle) == 0) &&
                    (strcmp(tab_tarifs[i].id_categorie_billet, tab_tarifs[indice_tarif].id_categorie_billet) == 0) &&
                    (strcmp(tab_tarifs[i].id_cat_profil, "CP001") == 0))
                {
                    deja_affiche_avant = 1;
                }
            }

            if (! deja_affiche_avant)
            {
                return 1;
            }
        }
    }

    return 0;
}

//Recherche de la jauge de depart d'une categorie dans une salle
int rechercher_jauge_initiale(char id_salle[], char id_categorie_billet[])
{
    int i;

    for (i = 0 ; i < nbcategories ; i++)
    {
        if ((strcmp(tab_categories[i].id_salle, id_salle) == 0) &&
            (strcmp(tab_categories[i].id_categorie_billet, id_categorie_billet) == 0))
        {
            return tab_categories[i].jauge_categorie_billet;
        }
    }

    return 0;
}

//Calcul des places restantes d'une categorie pour une representation
int calculer_places_restantes(char id_representation[], char id_categorie_billet[])
{
    int indice_representation;
    int jauge_initiale;
    int nb_places_prises;
    int i;

    indice_representation = rechercher_representation_par_id(id_representation);

    if (indice_representation == NON_TROUVE)
    {
        return 0;
    }

    jauge_initiale = rechercher_jauge_initiale(tab_representations[indice_representation].id_salle,id_categorie_billet);
    nb_places_prises = 0;

    for (i = 0 ; i < nbreservations ; i++)
    {
            if ((strcmp(tab_reservations[i].id_representation, id_representation) == 0) &&
                (strcmp(tab_reservations[i].id_categorie_billet, id_categorie_billet) == 0))
            {
                nb_places_prises++;
            }

    }

    return jauge_initiale - nb_places_prises;
}

// Tester si la date d'une representation est deja passee.
// La comparaison se fait avec la date locale de la machine, au format AAAA-MM-JJ
int representation_passee(int indice_representation)
{
    int annee_rep;
    int mois_rep;
    int jour_rep;
    int date_rep;
    int date_locale;
    time_t secondes;
    struct tm *date_machine;

    if ((indice_representation < 0) || (indice_representation >= nbrepresentations))
    {
        return 1;//1 = la representation est passee - 0 = la representation n'est pas encore passee
    }

    if (sscanf(tab_representations[indice_representation].date_representation,"%d-%d-%d", &annee_rep, &mois_rep, &jour_rep) != 3)
    {
        return 1;
    }
    /*on lit la date de la representation de format AAAA-MM-JJ avec sscanf, 
    Si sscanf ne reussit pas a lire 3 nombres, ca veut dire que le format de la date est faux,
    on considere pas la representation comme passee (non reservable) aussi. */

    time(&secondes);//time ecrit le temps actuel dans la variable secondes
    date_machine = localtime(&secondes);//localtime transforme les secondes en date locale lisible avec tm_year, tm_mon et tm_mday

    date_rep = (annee_rep * 10000) + (mois_rep * 100) + jour_rep;

    date_locale = (((*date_machine).tm_year + 1900) * 10000) + (((*date_machine).tm_mon + 1) * 100)+ (*date_machine).tm_mday;

    return (date_rep < date_locale);
}

//Afficher la jauge exacte de chaque categorie pour une representation
void afficher_jauges_representation(int indice_representation)
{
    int i;
    int trouve;
    int restantes;
    struct REPRESENTATION rep;

    if ((indice_representation < 0) || (indice_representation >= nbrepresentations))
    {
        printf("Representation incorrecte.\n");
    }
    else
    {
        rep = tab_representations[indice_representation];
        trouve = 0;

        printf("\nJauge de la representation\n");
        printf("%-18s : %s\n", "Representation", rep.id_representation);
        printf("%-18s : %s\n", "Spectacle", rep.nom_spectacle);
        printf("%-18s : %s\n", "Date", rep.date_representation);
        printf("%-18s : %s\n", "Horaire", rep.horaire_representation);
        printf("%-18s : %s\n", "Salle", rep.nom_salle);
        printf("+-----------------------------------------------------------------------------------+\n");
        printf("%-10s %-35s %-12s %-12s\n", "ID", "Categorie", "Jauge", "Restantes");
        printf("+-----------------------------------------------------------------------------------+\n");

        for (i = 0 ; i < nbcategories ; i++)
        {
            if (strcmp(tab_categories[i].id_salle, rep.id_salle) == 0)
            {
                restantes = calculer_places_restantes(rep.id_representation,tab_categories[i].id_categorie_billet);
                printf("%-10s %-35s %-12d %-12d\n",
                    tab_categories[i].id_categorie_billet,
                    tab_categories[i].categorie_billet,
                    tab_categories[i].jauge_categorie_billet,
                    restantes);
                trouve = 1;
            }
        }

        if (! trouve)
        {
            printf("Aucune categorie trouvee pour cette salle.\n");
        }

        printf("+-----------------------------------------------------------------------------------+\n");
    }
}

//Choix d'une representation parmi toutes les representations chargees
int choisir_representation_toutes()
{
    int i;
    int choix;

    printf("\nListe des representations\n");
    printf("+----------------------------------------------------------------------------------------------------------------------+\n");
    printf("%-5s %-8s %-12s %-18s %-45s %-25s\n", "No", "ID", "Date", "Horaire", "Spectacle", "Salle");
    printf("+----------------------------------------------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbrepresentations ; i++)
    {
        printf("%-5d %-8s %-12s %-18s %-45s %-25s\n",
               i + 1,
               tab_representations[i].id_representation,
               tab_representations[i].date_representation,
               tab_representations[i].horaire_representation,
               tab_representations[i].nom_spectacle,
               tab_representations[i].nom_salle);
    }

    printf("+----------------------------------------------------------------------------------------------------------------------+\n");

    choix = lire_entier("Choisissez une representation (0 pour Retour au menu de reservation) : ");

    if ((choix < 1) || (choix > nbrepresentations))
    {
        if (choix != 0)
        {
            printf("Choix incorrect. Entrez un numero de representation affiche ou 0 pour retourner.\n");
        }
        return NON_TROUVE;
    }

    return choix - 1;
}

/* Choix du tarif. Plusieurs tarifs peuvent partager la meme categorie : c'est normal. */
int choisir_tarif_reservation(int indice_representation, char droit_compte[], int indice_reservation_ignoree)
{
    int i;
    int choix;
    int compteur;
    int restantes;
    struct REPRESENTATION rep;

    rep = tab_representations[indice_representation];
    compteur = 0;

    printf("\nTarifs disponibles\n");
    printf("+------------------------------------------------------------------------------------------------------------------------+\n");
    printf("%-5s %-8s %-28s %-18s %-8s %-10s %-35s\n", "No", "Cat", "Categorie", "Type", "Tarif", "Reste", "Profil");
    printf("+------------------------------------------------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbtarifs ; i++)
    {
        if ((tarif_correspond_representation(i, indice_representation)) &&
            (tarif_autorise_pour_droit(i, droit_compte)))
        {
            restantes = calculer_places_restantes(rep.id_representation,tab_tarifs[i].id_categorie_billet);
            compteur++;
            printf("%-5d %-8s %-28s %-18s %-8d %-10d %-35s\n",
                   compteur,
                   tab_tarifs[i].id_categorie_billet,
                   tab_tarifs[i].categorie_billet,
                   tab_tarifs[i].type_tarif,
                   tab_tarifs[i].tarif_brut,
                   restantes,
                   tab_tarifs[i].profil_tarif);
        }
    }

    printf("+------------------------------------------------------------------------------------------------------------------------+\n");

    if (compteur == 0)
    {
        printf("Aucun tarif disponible pour ce profil.\n");
        return NON_TROUVE;
    }

    choix = lire_entier("Choisissez un tarif (0 pour Retour a l'etape precedente) : ");

    if (choix <= 0)
    {
        return NON_TROUVE;
    }

    compteur = 0;
    for (i = 0 ; i < nbtarifs ; i++)
    {
        if ((tarif_correspond_representation(i, indice_representation)) &&
            (tarif_autorise_pour_droit(i, droit_compte)))
        {
            compteur++;
            if (compteur == choix)
            {
                restantes = calculer_places_restantes(rep.id_representation,tab_tarifs[i].id_categorie_billet);
                if (restantes <= 0)
                {
                    printf("Representation complete pour cette categorie.\n");
                    return NON_TROUVE;
                }
                return i;
            }
        }
    }

    return NON_TROUVE;
}

/* Choix d'un tarif pour une categorie deja choisie dans la consultation.
On affiche uniquement les tarifs autorises pour le compte connecte. */
int choisir_tarif_categorie_reservation(int indice_representation, char id_categorie_billet[], int indice_reservation_ignoree)
{
    int i;
    int choix;
    int compteur;
    int restantes;
    int indices[MAX_TARIFS];
    struct REPRESENTATION rep;

    if ((indice_representation < 0) || (indice_representation >= nbrepresentations))
    {
        return NON_TROUVE;
    }

    rep = tab_representations[indice_representation];
    compteur = 0;

    printf("\nTarifs disponibles pour la categorie choisie\n");
    printf("+------------------------------------------------------------------------------------------------------------------------+\n");
    printf("%-5s %-8s %-28s %-18s %-8s %-10s %-35s\n", "No", "Cat", "Categorie", "Type", "Tarif", "Reste", "Profil");
    printf("+------------------------------------------------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbtarifs ; i++)
    {
        if ((tarif_correspond_representation(i, indice_representation)) &&
            (strcmp(tab_tarifs[i].id_categorie_billet, id_categorie_billet) == 0) &&
            (tarif_autorise_pour_consultation(i)))
        {
            restantes = calculer_places_restantes(rep.id_representation,tab_tarifs[i].id_categorie_billet);
            compteur++;
            indices[compteur - 1] = i;
            printf("%-5d %-8s %-28s %-18s %-8d %-10d %-35s\n",
                   compteur,
                   tab_tarifs[i].id_categorie_billet,
                   tab_tarifs[i].categorie_billet,
                   tab_tarifs[i].type_tarif,
                   tab_tarifs[i].tarif_brut,
                   restantes,
                   tab_tarifs[i].profil_tarif);
        }
    }

    if (compteur == 0)
    {
        printf("Aucun tarif disponible pour ce compte et cette categorie.\n");
        return NON_TROUVE;
    }

    printf("+------------------------------------------------------------------------------------------------------------------------+\n");
    choix = lire_entier("Choisissez un tarif (0 pour Retour a l'etape precedente) : ");

    if ((choix < 1) || (choix > compteur))
    {
        return NON_TROUVE;
    }

    restantes = calculer_places_restantes(rep.id_representation,tab_tarifs[indices[choix - 1]].id_categorie_billet);

    if (restantes <= 0)
    {
        printf("Cette categorie est complete.\n");
        return NON_TROUVE;
    }

    return indices[choix - 1];
}

//Remplit les champs de la reservation a partir du compte, de la representation et du tarif
void remplir_reservation(struct RESERVATION *reservation, int indice_compte, int indice_representation, int indice_tarif)
{
    strcpy((*reservation).id_compte, tab_comptes[indice_compte].id_compte);
    strcpy((*reservation).id_representation, tab_representations[indice_representation].id_representation);
    strcpy((*reservation).id_spectacle, tab_representations[indice_representation].id_spectacle);
    strcpy((*reservation).nom_spectacle, tab_representations[indice_representation].nom_spectacle);
    strcpy((*reservation).date_representation, tab_representations[indice_representation].date_representation);
    strcpy((*reservation).horaire_representation, tab_representations[indice_representation].horaire_representation);
    strcpy((*reservation).id_salle, tab_representations[indice_representation].id_salle);
    strcpy((*reservation).nom_salle, tab_representations[indice_representation].nom_salle);
    strcpy((*reservation).id_categorie_billet, tab_tarifs[indice_tarif].id_categorie_billet);
    strcpy((*reservation).categorie_billet, tab_tarifs[indice_tarif].categorie_billet);
    strcpy((*reservation).id_cat_profil, tab_tarifs[indice_tarif].id_cat_profil);
    strcpy((*reservation).profil_tarif, tab_tarifs[indice_tarif].profil_tarif);
    strcpy((*reservation).type_tarif, tab_tarifs[indice_tarif].type_tarif);
    (*reservation).tarif_brut = tab_tarifs[indice_tarif].tarif_brut;
}

//Copie les informations d'un compte dans la reservation
void copier_compte_dans_reservation(int indice_compte, struct RESERVATION *reservation)
{
    strcpy((*reservation).nom_spectateur, tab_comptes[indice_compte].nom);
    strcpy((*reservation).prenom_spectateur, tab_comptes[indice_compte].prenom);
    strcpy((*reservation).email_spectateur, tab_comptes[indice_compte].email);
    strcpy((*reservation).tel_spectateur, tab_comptes[indice_compte].tel);
}

//Saisie libre avec possibilite de revenir au champ precedent
// Le nom, le prenom et l'email sont obligatoires : une chaine vide ne passe pas
int saisir_infos_spectateur_libre(struct RESERVATION *reservation)
{
    int champ;
    int annuler;
    char saisie[TAILLE_EMAIL];

    champ = 1;
    annuler = 0;

    printf("\nSaisie des informations du spectateur\n");
    printf("A chaque champ : Entrez 0 pour annuler toute la saisie.\n");
    printf("A partir du deuxieme champ : Entrez 9 ou RETOUR pour revenir au champ precedent.\n");
    printf("Nom, prenom et email sont obligatoires.\n");

    while ((champ <= 4) && (annuler == 0))
    {
        printf("\n");
        if (champ > 1)
        {
            printf("9 - Retour au champ precedent\n");
        }
        printf("0 - Annuler toute la saisie\n");

        if (champ == 1)
        {
            lire_chaine_taille("Nom du spectateur : ", saisie, TAILLE_NOM);
        }
        else if (champ == 2)
        {
            lire_chaine_taille("Prenom du spectateur : ", saisie, TAILLE_NOM);
        }
        else if (champ == 3)
        {
            lire_chaine_taille("Telephone du spectateur : ", saisie, TAILLE_TEL);
        }
        else
        {
            lire_chaine_taille("Email du spectateur : ", saisie, TAILLE_EMAIL);
        }

        if (strcmp(saisie, "0") == 0)
        {
            annuler = 1;
        }
        else if (champ_est_retour(saisie))
        {
            if (champ > 1)
            {
                champ--;
                printf("Retour au champ precedent.\n");
            }
            else
            {
                printf("Il n'y a pas de champ precedent. Entrez 0 pour annuler.\n");
            }
        }
        else if (((champ == 1) || (champ == 2) || (champ == 4)) && (strcmp(saisie, "") == 0))
        {
            printf("Ce champ est obligatoire. Il ne peut pas etre vide.\n");
        }
        else
        {
            if (champ == 1)
            {
                strcpy((*reservation).nom_spectateur, saisie);
            }
            else if (champ == 2)
            {
                strcpy((*reservation).prenom_spectateur, saisie);
            }
            else if (champ == 3)
            {
                strcpy((*reservation).tel_spectateur, saisie);
            }
            else
            {
                strcpy((*reservation).email_spectateur, saisie);
            }

            champ++;
        }
    }

    if (annuler)
    {
        printf("Saisie du spectateur annulee.\n");
        return 0;
    }

    return 1;
}


/* Saisie des informations du spectateur.
Si la reservation est pour le compte lui-meme, on copie les informations du compte.
Sinon on utilise la saisie libre avec retour au champ precedent
Si le spectateur n'a pas de compte, on va utiliser la saisie libre aussi. */
int saisir_infos_spectateur(int indice_compte, struct RESERVATION *reservation)
{
    char reponse[10];
    int pour_soi;

    pour_soi = 0;

    printf("Reservation pour le titulaire du compte (o/n) ? ");
    scanf("%s", reponse);
    vider_buffer_clavier();

    if ((reponse[0] == 'o') || (reponse[0] == 'O'))
    {
        pour_soi = 1;
    }

    if (pour_soi)
    {
        copier_compte_dans_reservation(indice_compte, reservation);
        return 1;
    }
    else
    {
        return saisir_infos_spectateur_libre(reservation);
    }
}

/* Copie les informations d'un abonne actif dans la reservation.
Cette copie evite d'attribuer un tarif abonne a une personne non enregistree. */
void copier_abonne_dans_reservation(int indice_abonne, struct RESERVATION *reservation)
{
    strcpy((*reservation).nom_spectateur, tab_abonnes[indice_abonne].nom);
    strcpy((*reservation).prenom_spectateur, tab_abonnes[indice_abonne].prenom);
    strcpy((*reservation).email_spectateur, tab_abonnes[indice_abonne].email);
    strcpy((*reservation).tel_spectateur, tab_abonnes[indice_abonne].tel);
}

//Creation d'une reservation depuis la consultation
int reserver_depuis_consultation(int indice_representation, int indice_categorie, int indice_tarif)
{
    int indice_abonne;
    int indice_compte_reservation;
    int indice_abonne_liste;
    int mode_beneficiaire;
    int choix_beneficiaire;
    int retour_beneficiaire;
    int restantes;
    int nb_billets;
    int i;
    int retour;
    int terminer;
    char id_cat_abonne[TAILLE_ID];
    struct RESERVATION reservation;
    struct RESERVATION reservation_a_sauver;

    retour = 0;
    retour_beneficiaire = 0;
    terminer = 0;
    mode_beneficiaire = 0;
    indice_abonne_liste = NON_TROUVE;
    indice_compte_reservation = indice_compte_connecte;
    strcpy(id_cat_abonne, "");

    if (indice_categorie == NON_TROUVE)
    {
        return 0;
    }

    if (representation_passee(indice_representation))
    {
        printf("Cette representation est passee. Reservation impossible.\n");
        return 0;
    }

    if (nbreservations >= MAX_RESERVATIONS)
    {
        printf("Tableau des reservations plein.\n");
        return 0;
    }

    while ((terminer == 0) && (retour == 0))
    {
        if (compte_est_admin())
        {
            mode_beneficiaire = 0;

            while ((mode_beneficiaire == 0) && (retour == 0))
            {
                printf("\nBeneficiaire de la reservation\n");
                printf("1 - Le spectateur a un compte utilisateur ou abonne\n");
                printf("2 - Le spectateur n'a pas de compte et n'est pas abonne\n");
                printf("3 - Le spectateur est un abonne dans notre liste\n");
                printf("0 - Retour au choix de la categorie\n");
                choix_beneficiaire = lire_entier("Votre choix : ");

                if (choix_beneficiaire == 0)
                {
                    retour = RETOUR_CHOIX_CATEGORIES;
                }
                else if (choix_beneficiaire == 1)
                {
                    indice_compte_reservation = choisir_compte_user_ou_abonne();
                    if (indice_compte_reservation != NON_TROUVE)
                    {
                        mode_beneficiaire = 1;
                    }
                }
                else if (choix_beneficiaire == 2)
                {
                    mode_beneficiaire = 2;
                    indice_compte_reservation = indice_compte_connecte;
                    printf("Spectateur sans compte : les tarifs abonnes ne seront pas proposes.\n");
                }
                else if (choix_beneficiaire == 3)
                {
                    indice_abonne_liste = choisir_abonne_par_actif(1);
                    if (indice_abonne_liste != NON_TROUVE)
                    {
                        mode_beneficiaire = 3;
                        indice_compte_reservation = indice_compte_connecte;
                    }
                }
                else
                {
                    printf("Choix incorrect.\n");
                }
            }
        }
        else
        {
            mode_beneficiaire = 1;
            indice_compte_reservation = indice_compte_connecte;
        }

        if (retour == 0)
        {
            indice_tarif = NON_TROUVE;
            nb_billets = 0;
            retour_beneficiaire = 0;

            while ((indice_tarif == NON_TROUVE) && (retour == 0) && (terminer == 0)&&(retour_beneficiaire == 0))
            {
                if (compte_est_admin())
                {
                    indice_tarif = choisir_tarif_categorie_pour_beneficiaire_admin(indice_representation,tab_categories[indice_categorie].id_categorie_billet,mode_beneficiaire,indice_compte_reservation,indice_abonne_liste);

                    if (indice_tarif == NON_TROUVE)
                    {
                        //Retour au choix du beneficiaire
                        mode_beneficiaire = 0;
                    }
                }
                else
                {
                    indice_tarif = choisir_tarif_categorie_reservation(indice_representation,tab_categories[indice_categorie].id_categorie_billet,NON_TROUVE);

                    if (indice_tarif == NON_TROUVE)
                    {
                        return RETOUR_CHOIX_CATEGORIES;
                    }
                }

                if ((compte_est_admin()) && (indice_tarif == NON_TROUVE))
                {
                    //On sort de la boucle tarif et on recommence au beneficiaire
                    retour_beneficiaire = 1;
                }

                if (indice_tarif != NON_TROUVE)
                {
                    indice_abonne = rechercher_indice_abonne_beneficiaire(indice_tarif,mode_beneficiaire,indice_compte_reservation,indice_abonne_liste);

                    if ((tarif_est_abonne(indice_tarif)) && (indice_abonne == NON_TROUVE))
                    {
                        printf("Reservation impossible : aucun abonnement actif compatible.\n");
                        indice_tarif = NON_TROUVE;
                    }
                    else
                    {
                        restantes = calculer_places_restantes(tab_representations[indice_representation].id_representation,tab_tarifs[indice_tarif].id_categorie_billet);

                        nb_billets = choisir_nombre_billets_tarif_beneficiaire(restantes,indice_representation,indice_tarif,indice_abonne,mode_beneficiaire,indice_compte_reservation,indice_abonne_liste);

                        if (nb_billets == 0)
                        {
                            indice_tarif = NON_TROUVE;
                        }
                    }
                }
            }

            if ((indice_tarif != NON_TROUVE) && (nb_billets > 0))
            {
                if ((nbreservations + nb_billets) > MAX_RESERVATIONS)
                {
                    printf("Tableau des reservations plein pour cette quantite.\n");
                    return 0;
                }

                remplir_reservation(&reservation, indice_compte_reservation, indice_representation, indice_tarif);

                if (tarif_est_abonne(indice_tarif))
                {
                    categorie_abonne_pour_tarif(indice_tarif, id_cat_abonne);

                    if (compte_est_admin())
                    {
                        if (mode_beneficiaire == 1)
                        {
                            if (strcmp(tab_comptes[indice_compte_reservation].droit_compte, "abonne") == 0)
                            {
                                indice_abonne = rechercher_abonne_actif_compte(indice_compte_reservation);
                            }
                        }
                        else if (mode_beneficiaire == 3)
                        {
                            indice_abonne = indice_abonne_liste;
                        }
                    }
                    else if (compte_est_abonne())
                    {
                        //Un abonne connecte reserve un tarif abonne uniquement avec son propre abonnement.
                        indice_abonne = rechercher_abonne_actif_compte(indice_compte_connecte);
                    }

                    if (indice_abonne == NON_TROUVE)
                    {
                        printf("Reservation annulee : aucun abonnement actif compatible.\n");
                        return 0;
                    }
                    else if ((strcmp(id_cat_abonne, "") != 0) &&
                             (strcmp(tab_abonnes[indice_abonne].id_cat_abonne, id_cat_abonne) != 0))
                    {
                        printf("Reservation annulee : categorie abonne attendue %s.\n", id_cat_abonne);
                        return 0;
                    }
                    else
                    {
                        copier_abonne_dans_reservation(indice_abonne, &reservation);
                    }
                }
                else
                {
                    if (compte_est_admin())
                    {
                        if (mode_beneficiaire == 2)
                        {
                            if (! saisir_infos_spectateur_libre(&reservation))
                            {
                                return 0;
                            }
                        }
                        else if (mode_beneficiaire == 3)
                        {
                            copier_abonne_dans_reservation(indice_abonne_liste, &reservation);
                        }
                        else
                        {
                            copier_compte_dans_reservation(indice_compte_reservation, &reservation);
                        }
                    }
                    else
                    {
                        if (! saisir_infos_spectateur(indice_compte_connecte, &reservation))
                        {
                            return 0;
                        }
                    }
                }

                printf("\nRecapitulatif de reservation\n");
                afficher_recap_reservation(reservation);
                printf("%-20s : %d\n", "Nombre de billets", nb_billets);
                printf("%-20s : %d\n", "Montant total", nb_billets * reservation.tarif_brut);

                if (confirmer_action())
                {
                    for (i = 0 ; i < nb_billets ; i++)
                    {
                        reservation_a_sauver = reservation;
                        generer_id_reservation(reservation_a_sauver.id_reservation);
                        tab_reservations[nbreservations] = reservation_a_sauver;
                        nbreservations++;
                    }

                    sauvegarder_reservations_auto();
                    printf("%d reservation(s) creee(s).\n", nb_billets);
                    attendre_retour_etape("Retour au choix des representations");
                }
                else
                {
                    printf("Reservation annulee.\n");
                    attendre_retour_etape("Retour au choix des representations");
                }

                terminer = 1;
            }
        }
    }

    if (retour == RETOUR_CHOIX_CATEGORIES)
    {
        return RETOUR_CHOIX_CATEGORIES;
    }

    return 1;
}

//Affichage recapitulatif avant confirmation.
void afficher_recap_reservation(struct RESERVATION reservation)
{
    printf("\nRecapitulatif de reservation\n");
    printf("%-20s : %s\n", "Reservation", reservation.id_reservation);
    printf("%-20s : %s\n", "Compte", reservation.id_compte);
    printf("%-20s : %s\n", "Spectacle", reservation.nom_spectacle);
    printf("%-20s : %s\n", "Date", reservation.date_representation);
    printf("%-20s : %s\n", "Horaire", reservation.horaire_representation);
    printf("%-20s : %s\n", "Salle", reservation.nom_salle);
    printf("%-20s : %s\n", "Categorie", reservation.categorie_billet);
    printf("%-20s : %s\n", "Profil tarif", reservation.profil_tarif);
    printf("%-20s : %d\n", "Tarif", reservation.tarif_brut);
    printf("%-20s : %s %s\n", "Spectateur", reservation.prenom_spectateur, reservation.nom_spectateur);
    printf("%-20s : %s\n", "Telephone", reservation.tel_spectateur);
    printf("%-20s : %s\n", "Email", reservation.email_spectateur);
}

//Creation d'une reservation pour un compte donne
void creer_reservation_utilisateur(int indice_compte)
{
    int indice_representation;
    int indice_tarif;
    int annuler;
    struct RESERVATION reservation;

    annuler = 0;

    if (nbreservations >= MAX_RESERVATIONS)
    {
        printf("Tableau des reservations plein.\n");
        annuler = 1;
    }

    if (annuler == 0)
    {
        indice_representation = choisir_representation_toutes();
        if (indice_representation == NON_TROUVE)
        {
            annuler = 1;
        }
    }

    if (annuler == 0)
    {
        if (representation_passee(indice_representation))
        {
            printf("Cette representation est passee. Elle n'est plus reservable.\n");
            attendre_retour_etape("Retour au choix de la representation");
            annuler = 1;
        }
    }

    if (annuler == 0)
    {
        indice_tarif = choisir_tarif_reservation(indice_representation,
                                                 tab_comptes[indice_compte].droit_compte,
                                                 NON_TROUVE);
        if (indice_tarif == NON_TROUVE)
        {
            annuler = 1;
        }
    }

    if (annuler == 0)
    {
        generer_id_reservation(reservation.id_reservation);
        remplir_reservation(&reservation, indice_compte, indice_representation, indice_tarif);

        if (! saisir_infos_spectateur(indice_compte, &reservation))
        {
            annuler = 1;
        }
    }

    if (annuler == 0)
    {
        afficher_recap_reservation(reservation);

        if (confirmer_action())
        {
            tab_reservations[nbreservations] = reservation;
            nbreservations++;
            sauvegarder_reservations_auto();
            printf("Reservation creee.\n");
            afficher_recap_reservation(reservation);
            attendre_retour_etape("Retour au menu de reservation");
        }
        else
        {
            printf("Reservation annulee.\n");
            attendre_retour_etape("Retour au menu de reservation");
        }
    }
}


//Creation de reservation par l'administrateur : il choisit d'abord le compte beneficiaire
void creer_reservation_admin()
{
    int indice_compte;

    indice_compte = choisir_compte();

    if (indice_compte == NON_TROUVE)
    {
        printf("Retour a l'etape precedente.\n");
    }
    else
    {
        creer_reservation_utilisateur(indice_compte);
    }
}

//Affichage d'une reservation
void afficher_ligne_reservation(int numero, struct RESERVATION reservation)
{
    printf("%-5d %-10s %-8s %-45s %-12s %-25s %-35s %-7d %-18s %-18s %-30s\n",
           numero,
           reservation.id_reservation,
           reservation.id_representation,
           reservation.nom_spectacle,
           reservation.date_representation,
           reservation.categorie_billet,
           reservation.profil_tarif,
           reservation.tarif_brut,
           reservation.nom_spectateur,
           reservation.prenom_spectateur,
           reservation.email_spectateur);
}

//Choix d'une reservation parmi toutes les reservations.
int choisir_reservation_toutes()
{
    int i;
    int choix;

    if (nbreservations == 0)
    {
        printf("Aucune reservation.\n");
        return NON_TROUVE;
    }

    printf("\nListe des reservations\n");
    printf("+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+\n");
    printf("%-5s %-10s %-8s %-45s %-12s %-25s %-35s %-7s %-18s %-18s %-30s\n",
           "No", "Resa", "Rep", "Spectacle", "Date", "Categorie", "Profil", "Tarif", "Nom", "Prenom", "Email");
    printf("+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbreservations ; i++)
    {
        afficher_ligne_reservation(i + 1, tab_reservations[i]);
    }

    printf("+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+\n");

    choix = lire_entier("Choisissez une reservation (0 pour Retour au choix de la representation) : ");

    if ((choix < 1) || (choix > nbreservations))
    {
        if (choix != 0)
        {
            printf("Choix incorrect. Entrez un numero de reservation affiche ou 0 pour retourner.\n");
        }
        return NON_TROUVE;
    }

    return choix - 1;
}

//Consultation des reservations du compte connecte ou d'un compte donne
void consulter_reservations_compte(int indice_compte)
{
    int i;
    int compteur;

    compteur = 0;

    printf("\nMes reservations\n");
    printf("+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+\n");
    printf("%-5s %-10s %-8s %-45s %-12s %-25s %-35s %-7s %-18s %-18s %-30s\n",
           "No", "Resa", "Rep", "Spectacle", "Date", "Categorie", "Profil", "Tarif", "Nom", "Prenom", "Email");
    printf("+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbreservations ; i++)
    {
        if (strcmp(tab_reservations[i].id_compte, tab_comptes[indice_compte].id_compte) == 0)
        {
            compteur++;
            afficher_ligne_reservation(compteur, tab_reservations[i]);
        }
    }

    if (compteur == 0)
    {
        printf("Aucune reservation pour ce compte.\n");
    }

    printf("+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+\n");
}

/* Choix de la representation pour la gestion des reservations.
Le parcours est : spectacles -> representations -> reservation. */
int choisir_representation_parcours_reservation()
{
    int indice_spectacle;
    int indice_representation;

    indice_spectacle = choisir_spectacle();

    if (indice_spectacle == NON_TROUVE)
    {
        return NON_TROUVE;
    }

    indice_representation = choisir_representation_spectacle(tab_spectacles[indice_spectacle].id_spectacle);

    return indice_representation;
}

//Affichage des reservations d'une representation deja choisie
void afficher_reservations_representation_indice(int indice_representation)
{
    int i;
    int compteur;

    if ((indice_representation < 0) || (indice_representation >= nbrepresentations))
    {
        printf("Representation incorrecte.\n");
    }
    else
    {
        compteur = 0;

        printf("\nReservations de la representation %s\n", tab_representations[indice_representation].id_representation);
        printf("Spectacle : %s\n", tab_representations[indice_representation].nom_spectacle);
        printf("Date      : %s\n", tab_representations[indice_representation].date_representation);
        printf("Horaire   : %s\n", tab_representations[indice_representation].horaire_representation);
        printf("+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+\n");
        printf("%-5s %-10s %-8s %-45s %-12s %-25s %-35s %-7s %-18s %-18s %-30s\n",
            "No", "Resa", "Rep", "Spectacle", "Date", "Categorie", "Profil", "Tarif", "Nom", "Prenom", "Email");
        printf("+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+\n");

        for (i = 0 ; i < nbreservations ; i++)
        {
            if (strcmp(tab_reservations[i].id_representation,
                    tab_representations[indice_representation].id_representation) == 0)
            {
                compteur++;
                afficher_ligne_reservation(compteur, tab_reservations[i]);
            }
        }

        if (compteur == 0)
        {
            printf("Aucune reservation pour cette representation.\n");
        }
        printf("+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+\n");
    }
}

//Choix d'une reservation apres le choix de la representation
int choisir_reservation_representation(int indice_representation)
{
    int i;
    int choix;
    int compteur;
    int indices[MAX_RESERVATIONS];

    if ((indice_representation < 0) || (indice_representation >= nbrepresentations))
    {
        return NON_TROUVE;
    }

    compteur = 0;
    afficher_reservations_representation_indice(indice_representation);

    for (i = 0 ; i < nbreservations ; i++)
    {
        if (strcmp(tab_reservations[i].id_representation,
                   tab_representations[indice_representation].id_representation) == 0)
        {
            indices[compteur] = i;
            compteur++;
        }
    }

    if (compteur == 0)
    {
        return NON_TROUVE;
    }

    choix = lire_entier("Choisissez une reservation (0 pour Retour au choix de la representation) : ");

    if ((choix < 1) || (choix > compteur))
    {
        if (choix != 0)
        {
            printf("Choix incorrect. Entrez un numero affiche ou 0 pour retourner.\n");
        }
        return NON_TROUVE;
    }

    return indices[choix - 1];
}

//Liste des reservations par representation
void consulter_reservations_representation()
{
    int indice_representation;

    indice_representation = choisir_representation_parcours_reservation();

    if (indice_representation == NON_TROUVE)
    {
        printf("Retour a l'etape precedente.\n");
    }
    else
    {
        afficher_reservations_representation_indice(indice_representation);
    }
}

//Export TXT des reservations d'une representation deja choisie
void exporter_reservations_representation_indice(int indice_representation)
{
    int i;
    int compteur;
    char nom_fichier[TAILLE_NOM];
    FILE *fsortie;

    if ((indice_representation < 0) || (indice_representation >= nbrepresentations))
    {
        printf("Representation incorrecte.\n");
    }
    else
    {
        sprintf(nom_fichier, "DATA/exports_reservations/reservations_%s.txt",
                tab_representations[indice_representation].id_representation);
        fsortie = fopen(nom_fichier, "w");

        if (fsortie == NULL)
        {
            printf("Pb acces fichier %s\n", nom_fichier);
        }
        else
        {
            compteur = 0;
            fprintf(fsortie, "Representation;%s\n", tab_representations[indice_representation].id_representation);
            fprintf(fsortie, "Spectacle;%s\n", tab_representations[indice_representation].nom_spectacle);
            fprintf(fsortie, "Date;%s\n", tab_representations[indice_representation].date_representation);
            fprintf(fsortie, "Horaire;%s\n", tab_representations[indice_representation].horaire_representation);
            fprintf(fsortie, "Salle;%s\n", tab_representations[indice_representation].nom_salle);
            fprintf(fsortie, "\n");
            fprintf(fsortie, "id_reservation;nom_spectacle;nom;prenom;telephone;email;categorie;profil_tarif;tarif\n");

            for (i = 0 ; i < nbreservations ; i++)
            {
                if (strcmp(tab_reservations[i].id_representation,
                        tab_representations[indice_representation].id_representation) == 0)
                {
                    fprintf(fsortie, "%s;%s;%s;%s;%s;%s;%s;%s;%d\n",
                            tab_reservations[i].id_reservation,
                            tab_reservations[i].nom_spectacle,
                            tab_reservations[i].nom_spectateur,
                            tab_reservations[i].prenom_spectateur,
                            tab_reservations[i].tel_spectateur,
                            tab_reservations[i].email_spectateur,
                            tab_reservations[i].categorie_billet,
                            tab_reservations[i].profil_tarif,
                            tab_reservations[i].tarif_brut);
                    compteur++;
                }
            }

            fclose(fsortie);
            printf("%d reservation(s) exportee(s) dans %s\n", compteur, nom_fichier);
        }
    }
}

// Export TXT des reservations d'une representation
void exporter_reservations_representation()
{
    int indice_representation;

    indice_representation = choisir_representation_parcours_reservation();

    if (indice_representation == NON_TROUVE)
    {
        printf("Retour a l'etape precedente.\n");
    }
    else
    {
        exporter_reservations_representation_indice(indice_representation);
    }
}

//Dans la consultation, apres avoir consulte une representation,l'administrateur peut exporter directement ses reservations.
void proposer_export_reservations_consultation(int indice_representation)
{
    int choix;

    if (compte_est_admin())
    {
        printf("\nExport admin\n");
        printf("1 - Exporter les reservations de cette representation\n");
        printf("0 - Ne pas exporter\n");
        choix = lire_entier("Votre choix : ");

        if (choix == 1)
        {
            exporter_reservations_representation_indice(indice_representation);
        }
    }
}

//Suppression d'une reservation par l'administrateur.
void supprimer_reservation_admin()
{
    int indice_representation;
    int indice_reservation;
    int i;

    indice_representation = choisir_representation_parcours_reservation();

    if (indice_representation == NON_TROUVE)
    {
        printf("Retour a l'etape precedente.\n");
    }
    else
    {
        indice_reservation = choisir_reservation_representation(indice_representation);

        if (indice_reservation == NON_TROUVE)
        {
            printf("Aucune reservation choisie.\n");
        }
        else
        {
            afficher_recap_reservation(tab_reservations[indice_reservation]);

            if (confirmer_action())
            {
                for (i = indice_reservation ; i < nbreservations - 1 ; i++)
                {
                    tab_reservations[i] = tab_reservations[i + 1];
                }

                nbreservations--;
                sauvegarder_reservations_auto();
                printf("Reservation supprimee.\n");
            }
            else
            {
                printf("Suppression annulee.\n");
            }
        }
    }
}

//Menu admin de gestion des reservations
void menu_reservation_admin()
{
    int choix;

    choix = -1;

    while (choix != 0)
    {
        printf("\n");
        printf("+------------------------------------------------+\n");
        printf("          GESTION DES RESERVATIONS\n");
        printf("+------------------------------------------------+\n");
        printf("1 - Consulter les reservations d'une representation\n");
        printf("2 - Supprimer une reservation\n");
        printf("3 - Exporter les reservations d'une representation\n");
        printf("4 - Sauvegarder toutes les reservations\n");
        printf("0 - Retour menu principal\n");
        //pour modifier une reservation, on supprime ici et ensuite on va sur consultation de la saison pour reserver une nouvelle

        choix = lire_entier("Votre choix : ");

        switch (choix)
        {
            case 1 :
                consulter_reservations_representation();
                break;

            case 2 :
                supprimer_reservation_admin();
                break;

            case 3 :
                exporter_reservations_representation();
                break;

            case 4 :
                sauvegarder_reservations_source_et_archive();
                break;

            case 0 :
                break;

            default :
                printf("Choix incorrect.\n");
                break;
        }
    }
}

//Menu reserve aux comptes utilisateur et abonne. Les nouvelles reservations se font depuis la consultation. */
void menu_reservation_utilisateur()
{
    consulter_reservations_compte(indice_compte_connecte);
}

//Fonction appelee avant la suppression d'une representation. Elle cree un fichier dans DATA/representation_supprimee/ avec les reservations a conserver
int sauvegarder_reservations_representation_supprimee(int indice_representation)
{
    FILE *fsortie;
    char nom_fichier[TAILLE_NOM];
    int i;
    int compteur;

    if ((indice_representation < 0) || (indice_representation >= nbrepresentations))
    {
        return 0;
    }

    sprintf(nom_fichier, "DATA/representation_supprimee/%s_reservations.txt",tab_representations[indice_representation].id_representation);

    fsortie = fopen(nom_fichier, "w");

    if (fsortie == NULL)
    {
        printf("Pb acces fichier %s\n", nom_fichier);
        return 0;
    }
    else
    {
        compteur = 0;
        fprintf(fsortie, "Representation supprimee\n");
        fprintf(fsortie, "ID;%s\n", tab_representations[indice_representation].id_representation);
        fprintf(fsortie, "Spectacle;%s\n", tab_representations[indice_representation].nom_spectacle);
        fprintf(fsortie, "Date;%s\n", tab_representations[indice_representation].date_representation);
        fprintf(fsortie, "Horaire;%s\n", tab_representations[indice_representation].horaire_representation);
        fprintf(fsortie, "Salle;%s\n", tab_representations[indice_representation].nom_salle);
        fprintf(fsortie, "\nReservations\n");
        fprintf(fsortie, "id_reservation;nom;prenom;telephone;email;categorie;profil_tarif;tarif\n");

        for (i = 0 ; i < nbreservations ; i++)
        {
            if (strcmp(tab_reservations[i].id_representation,
                       tab_representations[indice_representation].id_representation) == 0)
            {
                fprintf(fsortie, "%s;%s;%s;%s;%s;%s;%s;%d\n",
                        tab_reservations[i].id_reservation,
                        tab_reservations[i].nom_spectateur,
                        tab_reservations[i].prenom_spectateur,
                        tab_reservations[i].tel_spectateur,
                        tab_reservations[i].email_spectateur,
                        tab_reservations[i].categorie_billet,
                        tab_reservations[i].profil_tarif,
                        tab_reservations[i].tarif_brut);
                compteur++;
            }
        }

        fclose(fsortie);
        printf("Sauvegarde suppression : %d reservation(s) dans %s\n", compteur, nom_fichier);
    }

    return 1;
}

//Suppression des reservations liees a une representation supprimee
void supprimer_reservations_representation(char id_representation[])
{
    int i;
    int j;

    i = 0;
    while (i < nbreservations)
    {
        if (strcmp(tab_reservations[i].id_representation, id_representation) == 0)
        {
            for (j = i ; j < nbreservations - 1 ; j++)
            {
                tab_reservations[j] = tab_reservations[j + 1];
            }
            nbreservations--;
        }
        else
        {
            i++;
        }
    }
}

//Menu admin : consultation de la jauge exacte 
void afficher_jauges_representation_admin()
{
    int indice_spectacle;
    int indice_representation;

    indice_spectacle = choisir_spectacle();

    if (indice_spectacle == NON_TROUVE)
    {
        printf("Retour a l'etape precedente.\n");
    }
    else
    {
        indice_representation = choisir_representation_spectacle(tab_spectacles[indice_spectacle].id_spectacle);

        if (indice_representation == NON_TROUVE)
        {
            printf("Retour a l'etape precedente.\n");
        }
        else
        {
            afficher_jauges_representation(indice_representation);
        }
    }
}