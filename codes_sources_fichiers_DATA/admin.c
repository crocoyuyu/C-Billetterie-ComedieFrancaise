/* ajout et suppression de spectacles et de representations. */

#include <stdio.h>
#include <string.h>
#include "declarations.h"

//Menu d'administration
void menu_gestion()
{
    int choix;

    choix = -1;

    while (choix != 0)
    {
        printf("\n");
        printf("+------------------------------------------------+\n");
        printf("              GESTION DE LA SAISON\n");
        printf("+------------------------------------------------+\n");
        printf("1 - Ajouter un spectacle\n");
        printf("2 - Supprimer un spectacle\n");
        printf("3 - Modifier le nom d'un spectacle\n");
        printf("4 - Ajouter une representation\n");
        printf("5 - Supprimer une representation\n");
        printf("6 - Consulter la jauge d'une representation\n");
        /*La modification directe d'une representation n'est pas proposee dans ce projet.
        Si on modifie une representation, les tarifs peuvent changer selon l'horaire, la date ou la salle.
        Les spectateurs peuvent aussi ne plus pouvoir assister a la representation modifiee.
        Donc, dans ce projet, si le metier veut modifier l'info d'une representation,
        il doit donc supprimer l'ancienne representation puis ajouter une nouvelle representation.
        Avant la suppression, les donnees de reservation doivent etre sauvegardees.
        Le metier doit ensuite recharger les informations de reservation et contacter les spectateurs.
        Si des spectateurs ne veulent plus assister, le metier supprime leur reservation et realise le remboursement.*/
        printf("7 - Sauvegarder les spectacles\n");
        printf("8 - Sauvegarder les representations\n");
        printf("0 - Retour menu principal\n");

        choix = lire_entier("Votre choix : ");

        switch (choix)
        {
            case 1 :
                ajouter_spectacle();
                break;

            case 2 :
                supprimer_spectacle();
                break;

            case 3 :
                modifier_spectacle();
                break;

            case 4 :
                ajouter_representation();
                break;

            case 5 :
                supprimer_representation();
                break;

            case 6 :
                afficher_jauges_representation_admin();
                break;

            case 7 :
                sauvegarder_spectacles_source_et_archive();
                break;

            case 8 :
                sauvegarder_representations_source_et_archive();
                break;

            case 0 :
                break;

            default :
                printf("Choix incorrect.\n");
                break;
        }
    }
}

//Ajouter un spectacle en fin de tableau, apres confirmation
void ajouter_spectacle()
{
    char nom_spectacle[TAILLE_NOM];
    int annuler;

    annuler = 0;

    if (nbspectacles >= MAX_SPECTACLES)
    {
        printf("Tableau des spectacles plein.\n");
        annuler = 1;
    }

    if (annuler == 0)
    {
        lire_chaine("Nom du spectacle (0 pour Retour a l'etape precedente) : ", nom_spectacle);

        if (strcmp(nom_spectacle, "0") == 0)
        {
            printf("Retour a l'etape precedente.\n");
            annuler = 1;
        }
    }

    if (annuler == 0)
    {
        generer_id_spectacle(tab_spectacles[nbspectacles].id_spectacle);
        strcpy(tab_spectacles[nbspectacles].nom_spectacle, nom_spectacle);

        printf("\nSpectacle a ajouter\n");
        printf("%-12s : %s\n", "ID", tab_spectacles[nbspectacles].id_spectacle);
        printf("%-12s : %s\n", "Nom", tab_spectacles[nbspectacles].nom_spectacle);

        if (confirmer_action())
        {
            nbspectacles++;
            sauvegarder_spectacles("DATA/00spectacle_25_26.txt");
            printf("Spectacle ajoute et fichier source mis a jour.\n");
        }
        else
        {
            printf("Ajout annule.\n");
        }
    }
}

//Supprimer un spectacle par decalage des cases suivantes du tableau
void supprimer_spectacle()
{
    int indice;
    int i;
    int annuler;

    annuler = 0;

    afficher_spectacles();
    indice = lire_entier("Choisissez le numero du spectacle a supprimer (0 pour Retour a l'etape precedente) : ");

    if (indice == 0)
    {
        printf("Retour a l'etape precedente.\n");
        annuler = 1;
    }

    if (annuler == 0)
    {
        indice = indice - 1;

        if ((indice < 0) || (indice >= nbspectacles))
        {
            printf("Choix invalide.\n");
            annuler = 1;
        }
    }

    if (annuler == 0)
    {
        printf("\nSpectacle a supprimer\n");
        printf("%-12s : %s\n", "ID", tab_spectacles[indice].id_spectacle);
        printf("%-12s : %s\n", "Nom", tab_spectacles[indice].nom_spectacle);
        printf("Les representations liees seront aussi supprimees.\n");

        if (confirmer_action())
        {
            supprimer_representations_spectacle(tab_spectacles[indice].id_spectacle);

            for (i = indice ; i < nbspectacles - 1 ; i++)
            {
                tab_spectacles[i] = tab_spectacles[i + 1];
            }

            nbspectacles--;
            sauvegarder_spectacles("DATA/00spectacle_25_26.txt");
            sauvegarder_representations("DATA/05representation_25_26.txt");
            sauvegarder_reservations("DATA/08reservations.txt");
            printf("Spectacle supprime et fichiers sources mis a jour.\n");
        }
        else
        {
            printf("Suppression annulee.\n");
        }
    }
}

//Modifier le nom d'un spectacle
void modifier_spectacle()
{
    int indice_spectacle;
    int i;
    int annuler;
    char ancien_nom[TAILLE_NOM];
    char nouveau_nom[TAILLE_NOM];
    char id_spectacle[TAILLE_ID];

    annuler = 0;

    indice_spectacle = choisir_spectacle();

    if (indice_spectacle == NON_TROUVE)
    {
        printf("Retour a l'etape precedente.\n");
        annuler = 1;
    }

    if (annuler == 0)
    {
        lire_chaine("Nouveau nom du spectacle (0 pour annuler et retour au menu precedent) : ", nouveau_nom);

        if (strcmp(nouveau_nom, "0") == 0)
        {
            printf("Retour a l'etape precedente.\n");
            annuler = 1;
        }
    }

    if (annuler == 0)
    {
        strcpy(ancien_nom, tab_spectacles[indice_spectacle].nom_spectacle);
        strcpy(id_spectacle, tab_spectacles[indice_spectacle].id_spectacle);

        printf("\nModification du spectacle\n");
        printf("%-20s : %s\n", "ID", id_spectacle);
        printf("%-20s : %s\n", "Ancien nom", ancien_nom);
        printf("%-20s : %s\n", "Nouveau nom", nouveau_nom);
        printf("Attention : cette modification peut aussi avoir un impact sur les representations liees.\n");

        if (confirmer_action())
        {
            strcpy(tab_spectacles[indice_spectacle].nom_spectacle, nouveau_nom);

            /* On met aussi a jour le nom stocke dans les representations du spectacle. */
            for (i = 0 ; i < nbrepresentations ; i++)
            {
                if (strcmp(tab_representations[i].id_spectacle, id_spectacle) == 0)
                {
                    strcpy(tab_representations[i].nom_spectacle, nouveau_nom);
                }
            }

            sauvegarder_spectacles("DATA/00spectacle_25_26.txt");
            sauvegarder_representations("DATA/05representation_25_26.txt");
            printf("Spectacle modifie et fichiers sources mis a jour.\n");
        }
        else
        {
            printf("Modification annulee.\n");
        }
    }
}

int extraire_nombre_id(char id[])
{
    int i;
    int nombre;

    i = 0;
    nombre = 0;

    while (id[i] != '\0')
    {
        if ((id[i] >= '0') && (id[i] <= '9'))
        {
            nombre = (nombre * 10) + (id[i] - '0');
        }
        i++;
    }

    return nombre;
}

//Generer le prochain id spectacle en gardant le prefixe S
void generer_id_spectacle(char id_spectacle[])
{
    int i;
    int max_id;
    int numero;

    max_id = 0;

    for (i = 0 ; i < nbspectacles ; i++)
    {
        numero = extraire_nombre_id(tab_spectacles[i].id_spectacle);
        if (numero > max_id)
        {
            max_id = numero;
        }
    }

    sprintf(id_spectacle, "S%03d", max_id+1);
    /* On a utilise sprintf ici qui se presente dans le polycours p.53 pour gerer le format S001
    sinon on peut aussi faire comme ca, mais ca va etre trop complique au niveau du syntaxe : 
    id_spectacle[0] = 'S';
    id_spectacle[1] = (nouveau_id / 100) + '0';
    id_spectacle[2] = ((nouveau_id / 10) % 10) + '0';
    id_spectacle[3] = (nouveau_id % 10) + '0';
    id_spectacle[4] = '\0';
    */
}

//Generer le prochain id representation en gardant le prefixe R
void generer_id_representation(char id_representation[])
{
    int i;
    int max_id;
    int numero;

    max_id = 0;

    for (i = 0 ; i < nbrepresentations ; i++)
    {
        numero = extraire_nombre_id(tab_representations[i].id_representation);
        if (numero > max_id)
        {
            max_id = numero;
        }
    }

    //Meme logique que pour les spectacles : prefixe R + numero sur 3 chiffres
    sprintf(id_representation, "R%03d", max_id+1);
}

//Ajouter une representation : choix du spectacle -> jour -> heure -> date -> salle
void ajouter_representation()
{
    int indice_spectacle;
    int indice_salle;
    int annuler;
    char heure[TAILLE_HEURE];
    char date_representation[TAILLE_DATE];
    char libelle[TAILLE_LIBELLE];

    annuler = 0;

    if (nbrepresentations >= MAX_REPRESENTATIONS)
    {
        printf("Tableau des representations plein.\n");
        annuler = 1;
    }

    if (annuler == 0)
    {
        indice_spectacle = choisir_spectacle();

        if (indice_spectacle == NON_TROUVE)
        {
            printf("Retour a l'etape precedente.\n");
            annuler = 1;
        }
    }

    if (annuler == 0)
    {
        indice_salle = choisir_salle();

        if (indice_salle == NON_TROUVE)
        {
            printf("Retour a l'etape precedente.\n");
            annuler = 1;
        }
    }

    if (annuler == 0)
    {
        if (!choisir_jour(tab_representations[nbrepresentations].jour_representation))
        {
            printf("Retour a l'etape precedente.\n");
            annuler = 1;
        }
    }

    if (annuler == 0)
    {
        printf("Exemples d'heure : 20h30, 14h00, 19h00. (0 pour annuler et retour au menu precedent) \n");
        lire_mot("Heure representation : ", heure);

        if (strcmp(heure, "0") == 0)
        {
            printf("Retour a l'etape precedente.\n");
            annuler = 1;
        }
    }

    if (annuler == 0)
    {
        lire_mot("Date representation AAAA-MM-JJ (0 pour annuler et retour au menu precedent) : ", date_representation);

        if (strcmp(date_representation, "0") == 0)
        {
            printf("Retour a l'etape precedente.\n");
            annuler = 1;
        }
    }

    if (annuler == 0)
    {
        generer_id_representation(tab_representations[nbrepresentations].id_representation);

        strcpy(tab_representations[nbrepresentations].id_spectacle,tab_spectacles[indice_spectacle].id_spectacle);
        strcpy(tab_representations[nbrepresentations].nom_spectacle,tab_spectacles[indice_spectacle].nom_spectacle);
        strcpy(tab_representations[nbrepresentations].heure_representation, heure);
        strcpy(tab_representations[nbrepresentations].date_representation, date_representation);
        concatener_libelle_horaire(tab_representations[nbrepresentations].jour_representation,tab_representations[nbrepresentations].heure_representation,libelle);
        strcpy(tab_representations[nbrepresentations].horaire_representation, libelle);
        generer_id_cat_representation(libelle,tab_representations[nbrepresentations].id_cat_representation);
        strcpy(tab_representations[nbrepresentations].id_salle,tab_salles[indice_salle].id_salle);
        strcpy(tab_representations[nbrepresentations].nom_salle,tab_salles[indice_salle].nom_salle);

        printf("\nRepresentation a ajouter\n");
        printf("%-20s : %s\n", "ID", tab_representations[nbrepresentations].id_representation);
        printf("%-20s : %s\n", "Spectacle", tab_representations[nbrepresentations].nom_spectacle);
        printf("%-20s : %s\n", "Date", tab_representations[nbrepresentations].date_representation);
        printf("%-20s : %s\n", "Jour", tab_representations[nbrepresentations].jour_representation);
        printf("%-20s : %s\n", "Heure", tab_representations[nbrepresentations].heure_representation);
        printf("%-20s : %s\n", "Horaire", tab_representations[nbrepresentations].horaire_representation);
        printf("%-20s : %s\n", "Cat horaire", tab_representations[nbrepresentations].id_cat_representation);
        printf("%-20s : %s\n", "Salle", tab_representations[nbrepresentations].nom_salle);

        if (confirmer_action())
        {
            nbrepresentations++;
            sauvegarder_representations("DATA/05representation_25_26.txt");
            printf("Representation ajoutee et fichier source mis a jour.\n");
            afficher_tarifs_generes_representation(nbrepresentations - 1);
        }
        else
        {
            printf("Ajout annule.\n");
        }
    }
}

// Choix du jour par menu numerote pour eviter une saisie libre difficile a controler
int choisir_jour(char jour[])
{
    int choix;
    int resultat;

    resultat = 0;

    printf("\nChoisir un jour\n");
    printf("1 - lundi\n");
    printf("2 - mardi\n");
    printf("3 - mercredi\n");
    printf("4 - jeudi\n");
    printf("5 - vendredi\n");
    printf("6 - samedi\n");
    printf("7 - dimanche\n");
    printf("0 - Retour\n");

    choix = lire_entier("Votre choix : ");

    switch (choix)
    {
        case 1 :
            strcpy(jour, "lundi");
            resultat = 1;
            break;

        case 2 :
            strcpy(jour, "mardi");
            resultat = 1;
            break;

        case 3 :
            strcpy(jour, "mercredi");
            resultat = 1;
            break;

        case 4 :
            strcpy(jour, "jeudi");
            resultat = 1;
            break;

        case 5 :
            strcpy(jour, "vendredi");
            resultat = 1;
            break;

        case 6 :
            strcpy(jour, "samedi");
            resultat = 1;
            break;

        case 7 :
            strcpy(jour, "dimanche");
            resultat = 1;
            break;

        default :
            resultat = 0;
            break;
    }

    return resultat;
}

//Afficher toutes les salles puis retourne l'indice choisi
int choisir_salle()
{
    int i;
    int choix;
    int resultat;

    resultat = NON_TROUVE;

    printf("\nListe des salles\n");
    printf("+-------------------------------------------------------------------+\n");
    printf("%-5s %-8s %-10s %-45s\n", "No", "ID", "Jauge", "Nom salle");
    printf("+-------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbsalles ; i++)
    {
        printf("%-5d %-8s %-10d %-45s\n",
               i + 1,
               tab_salles[i].id_salle,
               tab_salles[i].jauge_total_salle,
               tab_salles[i].nom_salle);
    }

    printf("+-------------------------------------------------------------------+\n");

    choix = lire_entier("Choisissez une salle (0 pour Retour a l'etape precedente) : ");

    if ((choix >= 1) && (choix <= nbsalles))
    {
        resultat = choix - 1;
    }

    return resultat;
}

// Concatener le jour et l'heure pour obtenir le libelle horaire de la representation
void concatener_libelle_horaire(char jour[], char heure[], char libelle[])
{
    strcpy(libelle, jour);
    strcat(libelle, " ");
    strcat(libelle, heure);
}

//Rechercher l'id de categorie horaire correspondant au libelle genere
void generer_id_cat_representation(char libelle[], char id_cat[])
{
    int i;
    int trouve;

    i = 0;
    trouve = 0;

    while ((i < nbhoraires) && (trouve == 0))
    {
        if (strcmp(tab_horaires[i].libelle_horaire, libelle) == 0)
        {
            strcpy(id_cat, tab_horaires[i].id_cat_horaire);
            trouve = 1;
        }

        i++;
    }

    if (trouve == 0)
    {
        strcpy(id_cat, "CH001");
    }
}

// Afficher les tarifs disponibles pour une representation nouvellement creee
void afficher_tarifs_generes_representation(int indice_representation)
{
    afficher_tarifs_representation(indice_representation);
}

int confirmer_action()
{
    char reponse[10];

    printf("Confirmer l'action (o/n) ? ");
    scanf("%s", reponse);
    vider_buffer_clavier();

    if ((reponse[0] == 'o') || (reponse[0] == 'O'))
    {
        return 1;
    }

    return 0;
}

//Supprimer une representation par selection du spectacle puis de la representation
void supprimer_representation()
{
    int indice_spectacle;
    int indice_representation;
    int i;
    int annuler;

    annuler = 0;

    indice_spectacle = choisir_spectacle();

    if (indice_spectacle == NON_TROUVE)
    {
        printf("Retour a l'etape precedente.\n");
        annuler = 1;
    }

    if (annuler == 0)
    {
        indice_representation = choisir_representation_spectacle(tab_spectacles[indice_spectacle].id_spectacle);

        if (indice_representation == NON_TROUVE)
        {
            printf("Retour a l'etape precedente.\n");
            annuler = 1;
        }
    }

    if (annuler == 0)
    {
        printf("\nRepresentation a supprimer\n");
        printf("%-20s : %s\n", "ID", tab_representations[indice_representation].id_representation);
        printf("%-20s : %s\n", "Spectacle", tab_representations[indice_representation].nom_spectacle);
        printf("%-20s : %s\n", "Date", tab_representations[indice_representation].date_representation);
        printf("%-20s : %s\n", "Horaire", tab_representations[indice_representation].horaire_representation);
        printf("%-20s : %s\n", "Salle", tab_representations[indice_representation].nom_salle);

        if (confirmer_action())
        {
            /* Avant de supprimer la representation, on archive ses reservations. */
            sauvegarder_reservations_representation_supprimee(indice_representation);
            supprimer_reservations_representation(tab_representations[indice_representation].id_representation);

            for (i = indice_representation ; i < nbrepresentations - 1 ; i++)
            {
                tab_representations[i] = tab_representations[i + 1];
            }

            nbrepresentations--;
            sauvegarder_representations("DATA/05representation_25_26.txt");
            sauvegarder_reservations("DATA/08reservations.txt");
            printf("Representation supprimee et fichiers sources mis a jour.\n");
        }
        else
        {
            printf("Suppression annulee.\n");
        }
    }
}

/* Menu separe pour la gestion des spectacles.
Il utilise les memes sous-programmes que l'ancien menu general. */
void menu_gestion_spectacles()
{
    int choix;

    choix = -1;

    while (choix != 0)
    {
        printf("\n");
        printf("+------------------------------------------------+\n");
        printf("             GESTION DES SPECTACLES\n");
        printf("+------------------------------------------------+\n");
        printf("1 - Ajouter un spectacle\n");
        printf("2 - Modifier le nom d'un spectacle\n");
        printf("3 - Supprimer un spectacle\n");
        printf("4 - Sauvegarder les spectacles\n");
        printf("0 - Retour menu principal\n");

        choix = lire_entier("Votre choix : ");

        switch (choix)
        {
            case 1 :
                ajouter_spectacle();
                break;

            case 2 :
                modifier_spectacle();
                break;

            case 3 :
                supprimer_spectacle();
                break;

            case 4 :
                sauvegarder_spectacles_source_et_archive();
                break;

            case 0 :
                break;

            default :
                printf("Choix incorrect.\n");
                break;
        }
    }
}

//Menu pour la gestion des representations
void menu_gestion_representations()
{
    int choix;

    choix = -1;

    while (choix != 0)
    {
        printf("\n");
        printf("+------------------------------------------------+\n");
        printf("          GESTION DES REPRESENTATIONS\n");
        printf("+------------------------------------------------+\n");
        printf("1 - Ajouter une representation\n");
        printf("2 - Supprimer une representation\n");
        printf("3 - Consulter la jauge d'une representation\n");
        printf("4 - Sauvegarder les representations\n");
        printf("0 - Retour menu principal\n");

        choix = lire_entier("Votre choix : ");

        switch (choix)
        {
            case 1 :
                ajouter_representation();
                break;

            case 2 :
                supprimer_representation();
                break;

            case 3 :
                afficher_jauges_representation_admin();
                break;

            case 4 :
                sauvegarder_representations_source_et_archive();
                break;

            case 0 :
                break;

            default :
                printf("Choix incorrect.\n");
                break;
        }
    }
}
