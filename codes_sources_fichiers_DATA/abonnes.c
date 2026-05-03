/* Gestion de la liste des abonnes.
Les abonnes ne sont jamais supprimes definitivement : on les rend inactifs.
Ainsi l'historique sera archive dans le fichier DATA/09abonnes.txt. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "declarations.h"

void lire_champ_abonne(char ligne[], int *position, char champ[]);
int choisir_abonne();
int saisir_modification_abonne_perso(struct ABONNE *abonne);
int saisir_modification_compte_perso(struct COMPTE *compte);
int saisir_ajout_abonne_retour(struct ABONNE *abonne);
int saisir_modification_abonne_admin(struct ABONNE *abonne);
int saisir_champ_modification(char nom_champ[], char ancienne[], char nouvelle[], int taille_max, int champ);
int champ_est_retour_abonne(char saisie[]);
int categorie_abonne_valide(char cat[]);
int rechercher_compte_par_id_abonne(char id_compte[]);
void completer_id_compte_abonne_si_possible(struct ABONNE *abonne);
void synchroniser_compte_lie_abonne(int indice_abonne);

/* Lecture d'un champ separe par un point-virgule.
position est passee par adresse pour reprendre la lecture au champ suivant. */
void lire_champ_abonne(char ligne[], int *position, char champ[])
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

/* Retour au champ precedent pour les saisies d'abonne.
On garde cette fonction locale pour ne pas multiplier les strcmp dans chaque boucle. */
int champ_est_retour_abonne(char saisie[])
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

//Les categories d'abonne acceptees dans le fichier sont AA - Adulte, AJ - Jeune et AF - famille
int categorie_abonne_valide(char cat[])
{
    if ((strcmp(cat, "AA") == 0) ||
        (strcmp(cat, "AJ") == 0) ||
        (strcmp(cat, "AF") == 0))
    {
        return 1;
    }

    return 0;
}

// Recherche d'un compte par son identifiant
int rechercher_compte_par_id_abonne(char id_compte[])
{
    int i;

    if (strcmp(id_compte, "") == 0)
    {
        return NON_TROUVE;
    }

    for (i = 0 ; i < nbcomptes ; i++)
    {
        if (strcmp(tab_comptes[i].id_compte, id_compte) == 0)
        {
            return i;
        }
    }

    return NON_TROUVE;
}

/* Completer le champ id_compte si le fichier ancien n'avait pas cette colonne.
Le lien se fait d'abord par email, puis par nom et prenom.
Par exemple, cela relie Jean-Michel Lerysur la liste d'abonne avec le compte jeanmichel_pise. */
void completer_id_compte_abonne_si_possible(struct ABONNE *abonne)
{
    int i;
    int trouve;

    trouve = 0;

    if (strcmp((*abonne).id_compte, "") == 0)
    {
        for (i = 0 ; (i < nbcomptes) && (trouve == 0) ; i++)
        {
            if (strcmp(tab_comptes[i].droit_compte, "abonne") == 0)
            {
                if ((strcmp((*abonne).email, "") != 0) &&
                    (strcmp((*abonne).email, tab_comptes[i].email) == 0))
                {
                    strcpy((*abonne).id_compte, tab_comptes[i].id_compte);
                    trouve = 1;
                }
            }
        }

        for (i = 0 ; (i < nbcomptes) && (trouve == 0) ; i++)
        {
            if (strcmp(tab_comptes[i].droit_compte, "abonne") == 0)
            {
                if ((strcmp((*abonne).nom, tab_comptes[i].nom) == 0) &&
                    (strcmp((*abonne).prenom, tab_comptes[i].prenom) == 0))
                {
                    strcpy((*abonne).id_compte, tab_comptes[i].id_compte);
                    trouve = 1;
                }
            }
        }
    }
}

/* Si un abonne est lie a un compte, on met a jour les informations du compte.
Le droit et le mot de passe ne sont pas modifiable par abonne lui-meme. */
void synchroniser_compte_lie_abonne(int indice_abonne)
{
    int indice_compte;

    if ((indice_abonne < 0) || (indice_abonne >= nbabonnes))
    {
        printf("Abonne incorrect. Synchronisation impossible.\n");
    }
    else
    {
        indice_compte = rechercher_compte_par_id_abonne(tab_abonnes[indice_abonne].id_compte);

        if (indice_compte != NON_TROUVE)
        {
            strcpy(tab_comptes[indice_compte].nom, tab_abonnes[indice_abonne].nom);
            strcpy(tab_comptes[indice_compte].prenom, tab_abonnes[indice_abonne].prenom);
            strcpy(tab_comptes[indice_compte].email, tab_abonnes[indice_abonne].email);
            strcpy(tab_comptes[indice_compte].tel, tab_abonnes[indice_abonne].tel);

            sauvegarder_comptes("DATA/07comptes.txt");
        }
    }
}

/* Demande si un champ doit etre modifie.
Retourne :
1 si on continue au champ suivant,
0 si toute la modification est annulee,
9 si on revient au champ precedent.
Si l'utilisateur repond non, l'ancienne valeur est conservee.
Si l'utilisateur repond oui puis laisse vide, l'ancienne valeur est aussi conservee. */
int saisir_champ_modification(char nom_champ[], char ancienne[], char nouvelle[], int taille_max, int champ)
{
    char reponse[TAILLE_ID];
    int fini;

    fini = 0;
    strcpy(nouvelle, ancienne);

    while (fini == 0)
    {
        if (champ > 1)
        {
            printf("9 ou RETOUR - Revenir au champ precedent\n");
        }
        printf("0 - Annuler toute la modification\n");
        printf("Ancien %s : %s\n", nom_champ, ancienne);
        lire_chaine_taille("Est-ce que tu veux modifier (o/n) : ", reponse, TAILLE_ID);

        if (strcmp(reponse, "0") == 0)
        {
            return 0;
        }
        else if (champ_est_retour_abonne(reponse))
        {
            if (champ > 1)
            {
                return 9;
            }
            else
            {
                printf("Il n'y a pas de champ precedent. Entrez 0 pour annuler.\n");
            }
        }
        else if ((reponse[0] == 'o') || (reponse[0] == 'O'))
        {
            printf("Nouveau %s : ", nom_champ);
            lire_chaine_taille("", nouvelle, taille_max);

            if (strcmp(nouvelle, "0") == 0)
            {
                return 0;
            }
            else if (champ_est_retour_abonne(nouvelle))
            {
                if (champ > 1)
                {
                    return 9;
                }
                else
                {
                    printf("Il n'y a pas de champ precedent. Entrez 0 pour annuler.\n");
                }
            }
            else
            {
                if (strcmp(nouvelle, "") == 0)
                {
                    strcpy(nouvelle, ancienne);
                }
                return 1;
            }
        }
        else if ((reponse[0] == 'n') || (reponse[0] == 'N') || (strcmp(reponse, "") == 0))
        {
            strcpy(nouvelle, ancienne);
            return 1;
        }
        else
        {
            printf("Choix incorrect. Entrez o, n, 0 ou RETOUR.\n");
        }
    }

    return 1;
}

//Creation d'un abonne de secours si le fichier abonne a ete initialise
void creer_abonnes_defaut()
{
    nbabonnes = 0;

    strcpy(tab_abonnes[nbabonnes].id_abonne, "AA001");
    strcpy(tab_abonnes[nbabonnes].id_compte, "jeanmichel_pise");
    strcpy(tab_abonnes[nbabonnes].nom, "Lery");
    strcpy(tab_abonnes[nbabonnes].prenom, "Jean-Michel");
    strcpy(tab_abonnes[nbabonnes].date_naissance, "");
    strcpy(tab_abonnes[nbabonnes].email, "jeanmichel.lery@gmail.com");
    strcpy(tab_abonnes[nbabonnes].ville, "");
    strcpy(tab_abonnes[nbabonnes].cp, "");
    strcpy(tab_abonnes[nbabonnes].tel, "");
    strcpy(tab_abonnes[nbabonnes].id_cat_abonne, "AA");
    tab_abonnes[nbabonnes].actif = 1;

    nbabonnes++;
}

//Chargement du fichier des abonnes
int charger_abonnes(char nom_fichier[])
{
    FILE *fentree;
    char ligne[TAILLE_LIGNE];
    char champ[TAILLE_LIBELLE];
    int position;
    int format_avec_compte;
    struct ABONNE abonne;

    nbabonnes = 0;
    fentree = fopen(nom_fichier, "r");

    if (fentree == NULL)
    {
        creer_abonnes_defaut();
        sauvegarder_abonnes(nom_fichier);
    }
    else
    {
        fgets(ligne, TAILLE_LIGNE, fentree);
        format_avec_compte = 0;
        position = 0;
        lire_champ_abonne(ligne, &position, champ);
        lire_champ_abonne(ligne, &position, champ);
        if (strcmp(champ, "id_compte") == 0)
        {
            format_avec_compte = 1;
        }

        while ((fgets(ligne, TAILLE_LIGNE, fentree) != NULL) && (nbabonnes < MAX_ABONNES))
        {
            if (strcmp(ligne, "") != 0)
            {
                position = 0;

                lire_champ_abonne(ligne, &position, abonne.id_abonne);
                if (format_avec_compte)
                {
                    lire_champ_abonne(ligne, &position, abonne.id_compte);
                }
                else
                {
                    strcpy(abonne.id_compte, "");
                }
                lire_champ_abonne(ligne, &position, abonne.nom);
                lire_champ_abonne(ligne, &position, abonne.prenom);
                lire_champ_abonne(ligne, &position, abonne.date_naissance);
                lire_champ_abonne(ligne, &position, abonne.email);
                lire_champ_abonne(ligne, &position, abonne.ville);
                lire_champ_abonne(ligne, &position, abonne.cp);
                lire_champ_abonne(ligne, &position, abonne.tel);
                lire_champ_abonne(ligne, &position, abonne.id_cat_abonne);
                lire_champ_abonne(ligne, &position, champ);

                if (strcmp(champ, "") == 0)
                {
                    abonne.actif = 1;
                }
                else
                {
                    abonne.actif = atoi(champ);
                }

                completer_id_compte_abonne_si_possible(&abonne);

                if (strcmp(abonne.id_abonne, "") != 0)
                {
                    tab_abonnes[nbabonnes] = abonne;
                    nbabonnes++;
                }
            }
        }

        fclose(fentree);
    }

    return nbabonnes;
}

//Sauvegarde du fichier des abonnes
int sauvegarder_abonnes(char nom_fichier[])
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
        fprintf(fsortie, "id_abonnes;id_compte;nom_abonnes;prenom_abonnes;ddn_abonnes;mail_abonnes;ville_abonnes;cp_abonnes;numtel_abonnes;id_cat_abonne;actif\n");

        for (i = 0 ; i < nbabonnes ; i++)
        {
            fprintf(fsortie, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%d\n",
                    tab_abonnes[i].id_abonne,
                    tab_abonnes[i].id_compte,
                    tab_abonnes[i].nom,
                    tab_abonnes[i].prenom,
                    tab_abonnes[i].date_naissance,
                    tab_abonnes[i].email,
                    tab_abonnes[i].ville,
                    tab_abonnes[i].cp,
                    tab_abonnes[i].tel,
                    tab_abonnes[i].id_cat_abonne,
                    tab_abonnes[i].actif);
        }

        fclose(fsortie);
    }

    return 1;
}

//Generation d'un identifiant d'abonne
void generer_id_abonne(char id_abonne[], char id_cat_abonne[])
{
    int i;
    int max_id;
    int numero;

    max_id = 0;

    for (i = 0 ; i < nbabonnes ; i++)
    {
        if ((tab_abonnes[i].id_abonne[0] == id_cat_abonne[0]) && (tab_abonnes[i].id_abonne[1] == id_cat_abonne[1]))
        {
            numero = extraire_nombre_id(tab_abonnes[i].id_abonne);

            if (numero > max_id)
            {
                max_id = numero;
            }
        }
    }

    sprintf(id_abonne, "%s%03d", id_cat_abonne, max_id + 1);
}

/* Recherche de l'abonne lie au compte connecte.
On utilise d'abord la nouvelle colonne id_compte, puis l'email,
puis le nom et le prenom pour rester compatible avec les anciennes donnees. */
int rechercher_abonne_compte(int indice_compte)
{
    int i;

    if ((indice_compte < 0) || (indice_compte >= nbcomptes))
    {
        return NON_TROUVE;
    }

    for (i = 0 ; i < nbabonnes ; i++)
    {
        if ((strcmp(tab_abonnes[i].id_compte, "") != 0) &&
            (strcmp(tab_abonnes[i].id_compte, tab_comptes[indice_compte].id_compte) == 0))
        {
            return i;
        }
    }

    for (i = 0 ; i < nbabonnes ; i++)
    {
        if ((strcmp(tab_comptes[indice_compte].email, "") != 0) &&
            (strcmp(tab_abonnes[i].email, tab_comptes[indice_compte].email) == 0))
        {
            return i;
        }
    }

    for (i = 0 ; i < nbabonnes ; i++)
    {
        if ((strcmp(tab_abonnes[i].nom, tab_comptes[indice_compte].nom) == 0) &&
            (strcmp(tab_abonnes[i].prenom, tab_comptes[indice_compte].prenom) == 0))
        {
            return i;
        }
    }

    return NON_TROUVE;
}

//Meme recherche, mais on exige que l'abonne soit actif.
int rechercher_abonne_actif_compte(int indice_compte)
{
    int indice;

    indice = rechercher_abonne_compte(indice_compte);

    if (indice == NON_TROUVE)
    {
        return NON_TROUVE;
    }

    if (tab_abonnes[indice].actif == 1)
    {
        return indice;
    }

    return NON_TROUVE;
}

//Affichage des abonnes selon leur etat actif ou inactif
void afficher_abonnes_par_actif(int actif)
{
    int i;
    int compteur;

    compteur = 0;

    printf("\nListe des abonnes ");
    if (actif)
    {
        printf("actifs\n");
    }
    else
    {
        printf("inactifs\n");
    }

    printf("+-----------------------------------------------------------------------------------------------------------------------------------------------------+\n");
    printf("%-5s %-8s %-16s %-18s %-18s %-12s %-30s %-16s %-8s %-10s %-6s\n",
           "No", "ID", "Compte", "Nom", "Prenom", "Naissance", "Email", "Ville", "CP", "Cat", "Actif");
    printf("+-----------------------------------------------------------------------------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbabonnes ; i++)
    {
        if (tab_abonnes[i].actif == actif)
        {
            compteur++;
            printf("%-5d %-8s %-16s %-18s %-18s %-12s %-30s %-16s %-8s %-10s %-6d\n",
                   compteur,
                   tab_abonnes[i].id_abonne,
                   tab_abonnes[i].id_compte,
                   tab_abonnes[i].nom,
                   tab_abonnes[i].prenom,
                   tab_abonnes[i].date_naissance,
                   tab_abonnes[i].email,
                   tab_abonnes[i].ville,
                   tab_abonnes[i].cp,
                   tab_abonnes[i].id_cat_abonne,
                   tab_abonnes[i].actif);
        }
    }

    if (compteur == 0)
    {
        printf("Aucun abonne dans cette liste.\n");
    }

    printf("+-----------------------------------------------------------------------------------------------------------------------------------------------------+\n");
}

// Affichage avec choix actif ou inactif
void afficher_abonnes()
{
    int choix;

    printf("1 - Afficher les abonnes actifs\n");
    printf("2 - Afficher les abonnes inactifs\n");
    choix = lire_entier("Votre choix : ");

    if (choix == 2)
    {
        afficher_abonnes_par_actif(0);
    }
    else
    {
        afficher_abonnes_par_actif(1);
    }
}

//Choix d'un abonne selon son etat actif ou inactif
int choisir_abonne_par_actif(int actif)
{
    int i;
    int choix;
    int compteur;
    int indices[MAX_ABONNES];

    compteur = 0;
    afficher_abonnes_par_actif(actif);

    for (i = 0 ; i < nbabonnes ; i++)
    {
        if (tab_abonnes[i].actif == actif)
        {
            indices[compteur] = i;
            compteur++;
        }
    }

    if (compteur == 0)
    {
        return NON_TROUVE;
    }

    choix = lire_entier("Choisissez un abonne (0 pour Retour a l'etape precedente) : ");

    if ((choix < 1) || (choix > compteur))
    {
        if (choix != 0)
        {
            printf("Choix incorrect. Entrez un numero d abonne affiche ou 0 pour retourner.\n");
        }
        return NON_TROUVE;
    }

    return indices[choix - 1];
}


/* Choix d'un abonne actif en filtrant la categorie.
Ce controle evite d'utiliser une carte Adulte avec un abonne Jeune, par exemple. */
int choisir_abonne_actif_categorie(char id_cat_abonne[])
{
    int i;
    int choix;
    int compteur;
    int indices[MAX_ABONNES];

    compteur = 0;

    printf("\nListe des abonnes actifs de categorie %s\n", id_cat_abonne);
    printf("+-----------------------------------------------------------------------------------------------------------------------------------------------------+\n");
    printf("%-5s %-8s %-16s %-18s %-18s %-12s %-30s %-16s %-8s %-10s %-6s\n",
           "No", "ID", "Compte", "Nom", "Prenom", "Naissance", "Email", "Ville", "CP", "Cat", "Actif");
    printf("+-----------------------------------------------------------------------------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbabonnes ; i++)
    {
        if ((tab_abonnes[i].actif == 1) && (strcmp(tab_abonnes[i].id_cat_abonne, id_cat_abonne) == 0))
        {
            indices[compteur] = i;
            compteur++;
            printf("%-5d %-8s %-16s %-18s %-18s %-12s %-30s %-16s %-8s %-10s %-6d\n",
                   compteur,
                   tab_abonnes[i].id_abonne,
                   tab_abonnes[i].id_compte,
                   tab_abonnes[i].nom,
                   tab_abonnes[i].prenom,
                   tab_abonnes[i].date_naissance,
                   tab_abonnes[i].email,
                   tab_abonnes[i].ville,
                   tab_abonnes[i].cp,
                   tab_abonnes[i].id_cat_abonne,
                   tab_abonnes[i].actif);
        }
    }

    if (compteur == 0)
    {
        printf("Aucun abonne actif dans cette categorie.\n");
        return NON_TROUVE;
    }

    printf("+-----------------------------------------------------------------------------------------------------------------------------------------------------+\n");
    choix = lire_entier("Choisissez un abonne (0 pour Retour a l'etape precedente) : ");

    if ((choix < 1) || (choix > compteur))
    {
        if (choix != 0)
        {
            printf("Choix incorrect. Entrez un numero d abonne affiche ou 0 pour retourner.\n");
        }
        return NON_TROUVE;
    }

    return indices[choix - 1];
}

//Choix d'un abonne par l'administrateur : actif ou inactif
int choisir_abonne()
{
    int choix;

    printf("1 - Choisir dans les abonnes actifs\n");
    printf("2 - Choisir dans les abonnes inactifs\n");
    choix = lire_entier("Votre choix : ");

    if (choix == 2)
    {
        return choisir_abonne_par_actif(0);
    }

    return choisir_abonne_par_actif(1);
}

/* Saisie d'un nouvel abonne.
Nom, prenom, email, telephone et categorie sont obligatoires.
Le compte lie est facultatif. */
int saisir_ajout_abonne_retour(struct ABONNE *abonne)
{
    int champ;
    int annuler;
    char saisie[TAILLE_LIBELLE];

    champ = 1;
    annuler = 0;

    printf("\nAjout d'un abonne\n");
    printf("Champs obligatoires : nom, prenom, email, telephone, categorie.\n");
    printf("0 - Annuler toute la saisie\n");
    printf("9 ou RETOUR - Revenir au champ precedent a partir du deuxieme champ\n");

    while ((champ <= 9) && (annuler == 0))
    {
        printf("\n");
        if (champ > 1)
        {
            printf("9 - Retour au champ precedent\n");
        }
        printf("0 - Annuler toute la saisie\n");

        if (champ == 1)
        {
            lire_chaine_taille("Nom : ", saisie, TAILLE_NOM);
        }
        else if (champ == 2)
        {
            lire_chaine_taille("Prenom : ", saisie, TAILLE_NOM);
        }
        else if (champ == 3)
        {
            lire_chaine_taille("Date de naissance AAAA-MM-JJ : ", saisie, TAILLE_DATE);
        }
        else if (champ == 4)
        {
            lire_chaine_taille("Email : ", saisie, TAILLE_EMAIL);
        }
        else if (champ == 5)
        {
            lire_chaine_taille("Ville : ", saisie, TAILLE_NOM);
        }
        else if (champ == 6)
        {
            lire_chaine_taille("Code postal : ", saisie, TAILLE_TEL);
        }
        else if (champ == 7)
        {
            lire_chaine_taille("Telephone : ", saisie, TAILLE_TEL);
        }
        else if (champ == 8)
        {
            lire_chaine_taille("Categorie abonne (AA, AJ, AF) : ", saisie, TAILLE_ID);
        }
        else
        {
            lire_chaine_taille("Compte lie (id_compte ou vide si aucun) : ", saisie, TAILLE_ID);
        }

        if (strcmp(saisie, "0") == 0)
        {
            annuler = 1;
        }
        else if (champ_est_retour_abonne(saisie))
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
        else if (((champ == 1) || (champ == 2) || (champ == 4) || (champ == 7) || (champ == 8)) && (strcmp(saisie, "") == 0))
        {
            printf("Ce champ est obligatoire. Il ne peut pas etre vide.\n");
        }
        else if ((champ == 8) && (! categorie_abonne_valide(saisie)))
        {
            printf("Categorie incorrecte. Les valeurs possibles sont AA, AJ ou AF.\n");
        }
        else if ((champ == 9) && (strcmp(saisie, "") != 0) && (rechercher_compte_par_id_abonne(saisie) == NON_TROUVE))
        {
            printf("Compte inconnu. Laissez vide ou Entrez un id_compte existant.\n");
        }
        else
        {
            if (champ == 1)
            {
                strcpy((*abonne).nom, saisie);
            }
            else if (champ == 2)
            {
                strcpy((*abonne).prenom, saisie);
            }
            else if (champ == 3)
            {
                strcpy((*abonne).date_naissance, saisie);
            }
            else if (champ == 4)
            {
                strcpy((*abonne).email, saisie);
            }
            else if (champ == 5)
            {
                strcpy((*abonne).ville, saisie);
            }
            else if (champ == 6)
            {
                strcpy((*abonne).cp, saisie);
            }
            else if (champ == 7)
            {
                strcpy((*abonne).tel, saisie);
            }
            else if (champ == 8)
            {
                strcpy((*abonne).id_cat_abonne, saisie);
            }
            else
            {
                strcpy((*abonne).id_compte, saisie);
            }
            champ++;
        }
    }

    if (annuler)
    {
        printf("Ajout annule pendant la saisie.\n");
        return 0;
    }

    return 1;
}

//Ajout d'un abonne
void ajouter_abonne()
{
    struct ABONNE abonne;

    if (nbabonnes >= MAX_ABONNES)
    {
        printf("Tableau des abonnes plein.\n");
    }
    else
    {
        strcpy(abonne.id_compte, "");
        strcpy(abonne.nom, "");
        strcpy(abonne.prenom, "");
        strcpy(abonne.date_naissance, "");
        strcpy(abonne.email, "");
        strcpy(abonne.ville, "");
        strcpy(abonne.cp, "");
        strcpy(abonne.tel, "");
        strcpy(abonne.id_cat_abonne, "");
        abonne.actif = 1;

        if (! saisir_ajout_abonne_retour(&abonne))
        {
            attendre_retour_etape("Retour au menu des abonnes");
        }
        else
        {
            generer_id_abonne(abonne.id_abonne, abonne.id_cat_abonne);
            completer_id_compte_abonne_si_possible(&abonne);

            printf("\nAbonne a ajouter\n");
            printf("%-18s : %s\n", "ID", abonne.id_abonne);
            printf("%-18s : %s\n", "Compte lie", abonne.id_compte);
            printf("%-18s : %s\n", "Nom", abonne.nom);
            printf("%-18s : %s\n", "Prenom", abonne.prenom);
            printf("%-18s : %s\n", "Naissance", abonne.date_naissance);
            printf("%-18s : %s\n", "Email", abonne.email);
            printf("%-18s : %s\n", "Ville", abonne.ville);
            printf("%-18s : %s\n", "CP", abonne.cp);
            printf("%-18s : %s\n", "Telephone", abonne.tel);
            printf("%-18s : %s\n", "Categorie", abonne.id_cat_abonne);

            if (confirmer_action())
            {
                tab_abonnes[nbabonnes] = abonne;
                nbabonnes++;
                synchroniser_compte_lie_abonne(nbabonnes - 1);
                sauvegarder_abonnes("DATA/09abonnes.txt");
                printf("Abonne ajoute et fichier source mis a jour.\n");
            }
            else
            {
                printf("Ajout annule.\n");
            }
        }
    }
}

/* Saisie de modification par l'administrateur.
Pour chaque champ, on demande d'abord si l'utilisateur veut le modifier.
Le statut actif n'est pas modifie ici. */
int saisir_modification_abonne_admin(struct ABONNE *abonne)
{
    int champ;
    int annuler;
    int retour;
    char saisie[TAILLE_LIBELLE];

    champ = 1;
    annuler = 0;

    printf("\nModification d'un abonne\n");
    printf("Entree vide : garder l'ancienne valeur.\n");
    printf("0 - Annuler toute la modification\n");
    printf("9 ou RETOUR - Revenir au champ precedent a partir du deuxieme champ\n");
    printf("Le statut actif ne se modifie pas ici. Utilisez les actions rendre inactif ou activer.\n");

    while ((champ <= 9) && (annuler == 0))
    {
        printf("\n");

        if (champ == 1)
        {
            retour = saisir_champ_modification("nom", (*abonne).nom, saisie, TAILLE_NOM, champ);
            if (retour == 1) 
            {
                strcpy((*abonne).nom, saisie);
            }
        }
        else if (champ == 2)
        {
            retour = saisir_champ_modification("prenom", (*abonne).prenom, saisie, TAILLE_NOM, champ);
            if (retour == 1) 
            {
                strcpy((*abonne).prenom, saisie);
            }
        }
        else if (champ == 3)
        {
            retour = saisir_champ_modification("date de naissance", (*abonne).date_naissance, saisie, TAILLE_DATE, champ);
            if (retour == 1) 
            {
                strcpy((*abonne).date_naissance, saisie);
            }
        }
        else if (champ == 4)
        {
            retour = saisir_champ_modification("email", (*abonne).email, saisie, TAILLE_EMAIL, champ);
            if (retour == 1) 
            {
                strcpy((*abonne).email, saisie);
            }
        }
        else if (champ == 5)
        {
            retour = saisir_champ_modification("ville", (*abonne).ville, saisie, TAILLE_NOM, champ);
            if (retour == 1) 
            {
                strcpy((*abonne).ville, saisie);
            }
        }
        else if (champ == 6)
        {
            retour = saisir_champ_modification("code postal", (*abonne).cp, saisie, TAILLE_TEL, champ);
            if (retour == 1) 
            {
                strcpy((*abonne).cp, saisie);
            }
        }
        else if (champ == 7)
        {
            retour = saisir_champ_modification("telephone", (*abonne).tel, saisie, TAILLE_TEL, champ);
            if (retour == 1) 
            {
                strcpy((*abonne).tel, saisie);
            }
        }
        else if (champ == 8)
        {
            retour = saisir_champ_modification("categorie abonne", (*abonne).id_cat_abonne, saisie, TAILLE_ID, champ);
            if (retour == 1)
            {
                if (categorie_abonne_valide(saisie))
                {
                    strcpy((*abonne).id_cat_abonne, saisie);
                }
                else
                {
                    printf("Categorie incorrecte. Les valeurs possibles sont AA, AJ ou AF.\n");
                    retour = -1;
                }
            }
        }
        else
        {
            retour = saisir_champ_modification("compte lie", (*abonne).id_compte, saisie, TAILLE_ID, champ);
            if (retour == 1)
            {
                if ((strcmp(saisie, "") == 0) ||
                    (rechercher_compte_par_id_abonne(saisie) != NON_TROUVE))
                {
                    strcpy((*abonne).id_compte, saisie);
                }
                else
                {
                    printf("Compte inconnu. Laissez vide ou Entrez un id_compte existant.\n");
                    retour = -1;
                }
            }
        }

        if (retour == 0)
        {
            annuler = 1;
        }
        else if (retour == 9)
        {
            champ--;
        }
        else if (retour == 1)
        {
            champ++;
        }
        else
        {
            /* retour == -1 : erreur de controle, on reste sur le meme champ. */
        }
    }

    if (annuler)
    {
        printf("Modification annulee pendant la saisie.\n");
        return 0;
    }

    return 1;
}

//Modification d'un abonne par l'administrateur
void modifier_abonne()
{
    int indice;
    struct ABONNE abonne;

    indice = choisir_abonne();

    if (indice == NON_TROUVE)
    {
        printf("Retour a l'etape precedente.\n");
    }
    else
    {
        abonne = tab_abonnes[indice];

        if (! saisir_modification_abonne_admin(&abonne))
        {
            attendre_retour_etape("Retour au menu des abonnes");
        }
        else
        {
            completer_id_compte_abonne_si_possible(&abonne);

            if (confirmer_action())
            {
                tab_abonnes[indice] = abonne;
                synchroniser_compte_lie_abonne(indice);
                sauvegarder_abonnes("DATA/09abonnes.txt");
                printf("Abonne modifie et fichier source mis a jour.\n");
            }
            else
            {
                printf("Modification annulee.\n");
            }
        }
    }
}

/* Saisie des champs modifiables d'un abonne connecte.
La categorie et le statut ne sont pas demandes : ils restent non modifiables.
Pour chaque champ, on demande si l'utilisateur veut le modifier. */
int saisir_modification_abonne_perso(struct ABONNE *abonne)
{
    int champ;
    int annuler;
    int retour;
    char saisie[TAILLE_LIBELLE];

    champ = 1;
    annuler = 0;

    printf("\nModification des informations d'abonne\n");
    printf("Entree vide : garder l'ancienne valeur.\n");
    printf("0 - Annuler toute la modification\n");
    printf("9 ou RETOUR - Revenir au champ precedent a partir du deuxieme champ\n");

    while ((champ <= 7) && (annuler == 0))
    {
        printf("\n");

        if (champ == 1)
        {
            retour = saisir_champ_modification("nom", (*abonne).nom, saisie, TAILLE_NOM, champ);
            if (retour == 1) 
            {
                strcpy((*abonne).nom, saisie);
            }
        }
        else if (champ == 2)
        {
            retour = saisir_champ_modification("prenom", (*abonne).prenom, saisie, TAILLE_NOM, champ);
            if (retour == 1) 
            {
                strcpy((*abonne).prenom, saisie);
            }
        }
        else if (champ == 3)
        {
            retour = saisir_champ_modification("date de naissance", (*abonne).date_naissance, saisie, TAILLE_DATE, champ);
            if (retour == 1) 
            {
                strcpy((*abonne).date_naissance, saisie);
            }
        }
        else if (champ == 4)
        {
            retour = saisir_champ_modification("email", (*abonne).email, saisie, TAILLE_EMAIL, champ);
            if (retour == 1) 
            {
                strcpy((*abonne).email, saisie);
            }
        }
        else if (champ == 5)
        {
            retour = saisir_champ_modification("ville", (*abonne).ville, saisie, TAILLE_NOM, champ);
            if (retour == 1) 
            {
                strcpy((*abonne).ville, saisie);
            }
        }
        else if (champ == 6)
        {
            retour = saisir_champ_modification("code postal", (*abonne).cp, saisie, TAILLE_TEL, champ);
            if (retour == 1) 
            {
                strcpy((*abonne).cp, saisie);
            }
        }
        else
        {
            retour = saisir_champ_modification("telephone", (*abonne).tel, saisie, TAILLE_TEL, champ);
            if (retour == 1) 
            {
                strcpy((*abonne).tel, saisie);
            }
        }

        if (retour == 0)
        {
            annuler = 1;
        }
        else if (retour == 9)
        {
            champ--;
        }
        else
        {
            champ++;
        }
    }

    if (annuler)
    {
        printf("Modification annulee pendant la saisie.\n");
        return 0;
    }

    return 1;
}

/* Saisie des informations personnelles d'un compte utilisateur.
Pour chaque champ, on demande si l'utilisateur veut le modifier. */
int saisir_modification_compte_perso(struct COMPTE *compte)
{
    int champ;
    int annuler;
    int retour;
    char saisie[TAILLE_LIBELLE];

    champ = 1;
    annuler = 0;

    printf("\nModification des informations personnelles\n");
    printf("Entree vide : garder l'ancienne valeur.\n");
    printf("0 - Annuler toute la modification\n");
    printf("9 ou RETOUR - Revenir au champ precedent a partir du deuxieme champ\n");

    while ((champ <= 4) && (annuler == 0))
    {
        printf("\n");

        if (champ == 1)
        {
            retour = saisir_champ_modification("nom", (*compte).nom, saisie, TAILLE_NOM, champ);
            if (retour == 1) 
            {
                strcpy((*compte).nom, saisie);
            }
        }
        else if (champ == 2)
        {
            retour = saisir_champ_modification("prenom", (*compte).prenom, saisie, TAILLE_NOM, champ);
            if (retour == 1) 
            {
                strcpy((*compte).prenom, saisie);
            }
        }
        else if (champ == 3)
        {
            retour = saisir_champ_modification("email", (*compte).email, saisie, TAILLE_EMAIL, champ);
            if (retour == 1) 
            {
                strcpy((*compte).email, saisie);
            }
        }
        else
        {
            retour = saisir_champ_modification("telephone", (*compte).tel, saisie, TAILLE_TEL, champ);
            if (retour == 1) 
            {
                strcpy((*compte).tel, saisie);
            }
        }

        if (retour == 0)
        {
            annuler = 1;
        }
        else if (retour == 9)
        {
            champ--;
        }
        else
        {
            champ++;
        }
    }

    if (annuler)
    {
        printf("Modification annulee pendant la saisie.\n");
        return 0;
    }

    return 1;
}

//Affichage de la liste abonne
void afficher_infos_abonne(int indice)
{
    if ((indice < 0) || (indice >= nbabonnes))
    {
        printf("Fiche abonne incorrecte.\n");
    }
    else
    {
        printf("\nMes informations d'abonne\n");
        printf("%-22s : %s\n", "ID abonne", tab_abonnes[indice].id_abonne);
        printf("%-22s : %s\n", "Compte lie", tab_abonnes[indice].id_compte);
        printf("%-22s : %s\n", "Nom", tab_abonnes[indice].nom);
        printf("%-22s : %s\n", "Prenom", tab_abonnes[indice].prenom);
        printf("%-22s : %s\n", "Date naissance", tab_abonnes[indice].date_naissance);
        printf("%-22s : %s\n", "Email", tab_abonnes[indice].email);
        printf("%-22s : %s\n", "Ville", tab_abonnes[indice].ville);
        printf("%-22s : %s\n", "Code postal", tab_abonnes[indice].cp);
        printf("%-22s : %s\n", "Telephone", tab_abonnes[indice].tel);
        printf("%-22s : %s (non modifiable)\n", "Categorie", tab_abonnes[indice].id_cat_abonne);
        printf("%-22s : %d (non modifiable)\n", "Actif", tab_abonnes[indice].actif);
    }
}

/* Modification des informations de l'abonne connecte.
La categorie et l'etat actif ne sont pas modifiables ici. */
void modifier_mes_infos_abonne()
{
    int indice;
    int choix;
    struct ABONNE abonne;

    indice = rechercher_abonne_compte(indice_compte_connecte);

    if (indice == NON_TROUVE)
    {
        printf("Aucune fiche abonne trouvee pour ce compte.\n");
        attendre_retour_etape("Retour au menu principal");
    }
    else
    {
        afficher_infos_abonne(indice);

        printf("\n1 - Modifier mes informations\n");
        printf("0 - Retour au menu principal\n");
        choix = lire_entier("Votre choix : ");

        if (choix != 1)
        {
            printf("Retour au menu principal.\n");
        }
        else
        {
            abonne = tab_abonnes[indice];

            if (! saisir_modification_abonne_perso(&abonne))
            {
                attendre_retour_etape("Retour a mes informations d'abonne");
            }
            else
            {
                printf("\nRappel : la categorie d'abonnement et le statut actif ne sont pas modifiables ici.\n");
                printf("Categorie conservee : %s\n", abonne.id_cat_abonne);
                printf("Statut conserve     : %d\n", abonne.actif);

                if (confirmer_action())
                {
                    if (strcmp(abonne.id_compte, "") == 0)
                    {
                        strcpy(abonne.id_compte, tab_comptes[indice_compte_connecte].id_compte);
                    }
                    tab_abonnes[indice] = abonne;

                    /* On met aussi a jour les informations du compte connecte.
                    Cela garde la reservation pour soi coherente avec la fiche abonne. */
                    strcpy(tab_comptes[indice_compte_connecte].nom, abonne.nom);
                    strcpy(tab_comptes[indice_compte_connecte].prenom, abonne.prenom);
                    strcpy(tab_comptes[indice_compte_connecte].email, abonne.email);
                    strcpy(tab_comptes[indice_compte_connecte].tel, abonne.tel);

                    sauvegarder_abonnes("DATA/09abonnes.txt");
                    sauvegarder_comptes("DATA/07comptes.txt");
                    printf("Informations abonne modifiees.\n");
                }
                else
                {
                    printf("Modification annulee.\n");
                }

                attendre_retour_etape("Retour au menu principal");
            }
        }
    }
}


//Affiche les informations personnelles d'un compte utilisateur simple
void afficher_infos_compte(int indice)
{
    if ((indice < 0) || (indice >= nbcomptes))
    {
        printf("Compte incorrect.\n");
    }
    else
    {
        printf("\nMes informations personnelles\n");
        printf("%-18s : %s\n", "ID compte", tab_comptes[indice].id_compte);
        printf("%-18s : %s (non modifiable)\n", "Droit", tab_comptes[indice].droit_compte);
        printf("%-18s : %s\n", "Nom", tab_comptes[indice].nom);
        printf("%-18s : %s\n", "Prenom", tab_comptes[indice].prenom);
        printf("%-18s : %s\n", "Email", tab_comptes[indice].email);
        printf("%-18s : %s\n", "Telephone", tab_comptes[indice].tel);
    }
}

/* Modification des informations personnelles d'un utilisateur simple.
Le droit du compte et le mot de passe ne sont pas modifies ici. */
void modifier_mes_infos_compte()
{
    int choix;
    struct COMPTE compte;

    afficher_infos_compte(indice_compte_connecte);

    printf("\n1 - Modifier mes informations\n");
    printf("0 - Retour au menu principal\n");
    choix = lire_entier("Votre choix : ");

    if (choix != 1)
    {
        printf("Retour au menu principal.\n");
    }
    else
    {
        compte = tab_comptes[indice_compte_connecte];

        if (! saisir_modification_compte_perso(&compte))
        {
            attendre_retour_etape("Retour a mes informations personnelles");
        }
        else
        {
            printf("\nRappel : le droit du compte et le mot de passe ne sont pas modifies ici.\n");

            if (confirmer_action())
            {
                tab_comptes[indice_compte_connecte] = compte;
                sauvegarder_comptes("DATA/07comptes.txt");
                printf("Informations personnelles modifiees.\n");
            }
            else
            {
                printf("Modification annulee.\n");
            }

            attendre_retour_etape("Retour au menu principal");
        }
    }
}


//inactivation de l'abonne
void supprimer_abonne()
{
    int indice;

    indice = choisir_abonne_par_actif(1);

    if (indice == NON_TROUVE)
    {
        printf("Retour a l'etape precedente.\n");
    }
    else
    {
        printf("\nAbonne a rendre inactif : %s %s\n",
               tab_abonnes[indice].prenom,
               tab_abonnes[indice].nom);

        if (confirmer_action())
        {
            tab_abonnes[indice].actif = 0;
            sauvegarder_abonnes("DATA/09abonnes.txt");
            printf("Abonne rendu inactif.\n");
        }
        else
        {
            printf("Action annulee.\n");
        }
    }
}

//Activation d'un abonne inactif
void activer_abonne()
{
    int indice;

    indice = choisir_abonne_par_actif(0);

    if (indice == NON_TROUVE)
    {
        printf("Retour a l'etape precedente.\n");
    }
    else
    {
        printf("\nAbonne a activer : %s %s\n",
               tab_abonnes[indice].prenom,
               tab_abonnes[indice].nom);

        if (confirmer_action())
        {
            tab_abonnes[indice].actif = 1;
            sauvegarder_abonnes("DATA/09abonnes.txt");
            printf("Abonne active.\n");
        }
        else
        {
            printf("Action annulee.\n");
        }
    }
}

//Menu de gestion des abonnes
void menu_abonnes()
{
    int choix;

    choix = -1;

    while (choix != 0)
    {
        printf("\n");
        printf("+------------------------------------------------+\n");
        printf("             GESTION DES ABONNES\n");
        printf("+------------------------------------------------+\n");
        printf("1 - Ajouter un abonne\n");
        printf("2 - Modifier un abonne\n");
        printf("3 - Rendre un abonne inactif\n");
        printf("4 - Activer un abonne inactif\n");
        printf("5 - Afficher les abonnes actifs ou inactifs\n");
        printf("6 - Sauvegarder les abonnes\n");
        printf("0 - Retour menu principal\n");

        choix = lire_entier("Votre choix : ");

        switch (choix)
        {
            case 1 :
                ajouter_abonne();
                break;

            case 2 :
                modifier_abonne();
                break;

            case 3 :
                supprimer_abonne();
                break;

            case 4 :
                activer_abonne();
                break;

            case 5 :
                afficher_abonnes();
                break;

            case 6 :
                sauvegarder_abonnes_source_et_archive();
                break;

            case 0 :
                break;

            default :
                printf("Choix incorrect. Entrez un numero du menu ou 0 pour retourner.\n");
                break;
        }
    }
}
