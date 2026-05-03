/* Gestion des comptes et authentification.
Le logiciel doit demander un login avant d'afficher les menus.
Le profil connecte decide ensuite des droits d'acces. */

#include <stdio.h>
#include <string.h>
#include "declarations.h"

void lire_champ_compte(char ligne[], int *position, char champ[], char separateur);

void lire_champ_compte(char ligne[], int *position, char champ[], char separateur)
{
    int i;

    i = 0;

    while ((ligne[*position] != separateur) && (ligne[*position] != '\0') && (ligne[*position] != '\n'))
    {
        champ[i] = ligne[*position];
        i++;
        (*position)++;
    }

    champ[i] = '\0';

    if (ligne[*position] == separateur)
    {
        (*position)++;
    }
}

/*notre logiciel ne prend pas en charge de la creation des autres comptes utilisateurs. 
on a cree ces trois comptes de test par default pour assurer que ces comptes peuvent toujours login
meme si l'admin a initialise les donnees*/
void creer_comptes_defaut()
{
    nbcomptes = 0;

    strcpy(tab_comptes[nbcomptes].id_compte, "yuan_pise");
    strcpy(tab_comptes[nbcomptes].droit_compte, "admin");
    strcpy(tab_comptes[nbcomptes].mdp_hache, "adminpise");
    strcpy(tab_comptes[nbcomptes].nom, "Ding");
    strcpy(tab_comptes[nbcomptes].prenom, "Yuan");
    strcpy(tab_comptes[nbcomptes].email, "yuan.d@outlook.fr");
    strcpy(tab_comptes[nbcomptes].tel, "0748440231");
    nbcomptes++;

    strcpy(tab_comptes[nbcomptes].id_compte, "amira_pise");
    strcpy(tab_comptes[nbcomptes].droit_compte, "utilisateur");
    strcpy(tab_comptes[nbcomptes].mdp_hache, "userpise");
    strcpy(tab_comptes[nbcomptes].nom, "Kairouani");
    strcpy(tab_comptes[nbcomptes].prenom, "Amira");
    strcpy(tab_comptes[nbcomptes].email, "amirakairouani22@gmail.com");
    strcpy(tab_comptes[nbcomptes].tel, "");
    nbcomptes++;

    strcpy(tab_comptes[nbcomptes].id_compte, "jeanmichel_pise");
    strcpy(tab_comptes[nbcomptes].droit_compte, "abonne");
    strcpy(tab_comptes[nbcomptes].mdp_hache, "abonnepise");
    strcpy(tab_comptes[nbcomptes].nom, "Lery");
    strcpy(tab_comptes[nbcomptes].prenom, "Jean-Michel");
    strcpy(tab_comptes[nbcomptes].email, "jeanmichel.lery@gmail.com");
    strcpy(tab_comptes[nbcomptes].tel, "");
    nbcomptes++;
}

//Chargement du fichier des comptes.
int charger_comptes(char nom_fichier[])
{
    FILE *fentree;
    char ligne[TAILLE_LIGNE];
    char separateur;
    int position;
    struct COMPTE compte;

    nbcomptes = 0;
    fentree = fopen(nom_fichier, "r");

    if (fentree == NULL)
    {
        printf("Fichier comptes absent. Creation des comptes de test.\n");
        creer_comptes_defaut();
        sauvegarder_comptes(nom_fichier);
    }
    else
    {
        fgets(ligne, TAILLE_LIGNE, fentree); //ligne d'entete

        while ((fgets(ligne, TAILLE_LIGNE, fentree) != NULL) && (nbcomptes < MAX_COMPTES))
        {
            if (strcmp(ligne, "") != 0)
            {
                separateur = ';';

                position = 0;
                lire_champ_compte(ligne, &position, compte.id_compte, separateur);
                lire_champ_compte(ligne, &position, compte.droit_compte, separateur);
                lire_champ_compte(ligne, &position, compte.mdp_hache, separateur);
                lire_champ_compte(ligne, &position, compte.nom, separateur);
                lire_champ_compte(ligne, &position, compte.prenom, separateur);
                lire_champ_compte(ligne, &position, compte.email, separateur);
                lire_champ_compte(ligne, &position, compte.tel, separateur);

                if (strcmp(compte.id_compte, "") != 0)
                {
                    tab_comptes[nbcomptes] = compte;
                    nbcomptes++;
                }
            }
        }
        fclose(fentree);
    }

    return nbcomptes;
}

//Sauvegarde des comptes dans un fichier texte.
int sauvegarder_comptes(char nom_fichier[])
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
        fprintf(fsortie, "id_compte;droit_compte;mdp_hache;nom;prenom;email;tel\n");

        for (i = 0 ; i < nbcomptes ; i++)
        {
            fprintf(fsortie, "%s;%s;%s;%s;%s;%s;%s\n",
                    tab_comptes[i].id_compte,
                    tab_comptes[i].droit_compte,
                    tab_comptes[i].mdp_hache,
                    tab_comptes[i].nom,
                    tab_comptes[i].prenom,
                    tab_comptes[i].email,
                    tab_comptes[i].tel);
        }
        fclose(fsortie);
    }

    return 1;
}

//processus de connexion :
//pour l'instant pas de limite d'essai de connexion en cas de fausse saisie de mdp
int authentifier()
{
    char login[TAILLE_ID];
    char mot_de_passe[TAILLE_MDP];
    int i;
    int trouve;

    trouve = 0;

    while (! trouve)
    {
        printf("\n");
        printf("+----------------------------------------+\n");
        printf("              CONNEXION\n");
        printf("+----------------------------------------+\n");
        lire_mot("Login : ", login);
        lire_mot("Mot de passe : ", mot_de_passe);

        i = 0;
        while ((! trouve) && (i < nbcomptes))
        {
            if ((strcmp(tab_comptes[i].id_compte, login))==0 &&
                (strcmp(tab_comptes[i].mdp_hache, mot_de_passe) == 0))
            {
                trouve = 1;
                indice_compte_connecte = i;
            }
            else
            {
                i++;
            }
        }

        if (! trouve)
        {
            printf("Erreur de connexion. Login ou mot de passe incorrect.\n");
        }
    }

    afficher_compte_connecte();
    return indice_compte_connecte;
}

// Affichage du compte connecte.
void afficher_compte_connecte()
{
    if (indice_compte_connecte != NON_TROUVE)
    {
        printf("\nConnecte : %s %s (%s)\n",
               tab_comptes[indice_compte_connecte].prenom,
               tab_comptes[indice_compte_connecte].nom,
               tab_comptes[indice_compte_connecte].droit_compte);
    }
}

int compte_est_admin()
{
    if (indice_compte_connecte == NON_TROUVE)
    {
        return 0;
    }

    return (strcmp(tab_comptes[indice_compte_connecte].droit_compte, "admin") == 0);
}

int compte_est_abonne()
{
    if (indice_compte_connecte == NON_TROUVE)
    {
        return 0;
    }

    return (strcmp(tab_comptes[indice_compte_connecte].droit_compte, "abonne") == 0);
}

int compte_est_utilisateur()
{
    if (indice_compte_connecte == NON_TROUVE)
    {
        return 0;
    }

    return (strcmp(tab_comptes[indice_compte_connecte].droit_compte, "utilisateur") == 0);
}

//les reservation par choix du compte effectue par l'administrateur
int choisir_compte()
{
    int i;
    int choix;

    printf("\nListe des comptes\n");
    printf("+-------------------------------------------------------------------------------------+\n");
    printf("%-5s %-18s %-14s %-20s %-20s\n", "No", "Login", "Droit", "Nom", "Prenom");
    printf("+-------------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbcomptes ; i++)
    {
        printf("%-5d %-18s %-14s %-20s %-20s\n",
               i + 1,
               tab_comptes[i].id_compte,
               tab_comptes[i].droit_compte,
               tab_comptes[i].nom,
               tab_comptes[i].prenom);
    }

    printf("+-------------------------------------------------------------------------------------+\n");

    choix = lire_entier("Choisissez un compte (0 pour Retour a l etape precedente) : ");

    if ((choix < 1) || (choix > nbcomptes))
    {
        return NON_TROUVE;
    }

    return choix - 1;
}
