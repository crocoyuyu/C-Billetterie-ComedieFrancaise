/* Consultation de la saison (spectacle, salle, representation, jauge, categorie des places, tarif)
Apres avoir consulte une representation, l'admin ou l'user peut decider s'il veut effectuer une reservation de cette representation
masi les focntions des reservations se trouvent dans le fichier c reservations.c */

#include <stdio.h>
#include <string.h>
#include "declarations.h"

void menu_consultation()
{
    int choix;

    choix = -1;

    while (choix != 0)
    {
        printf("\n");
        printf("+----------------------------------------+\n");
        printf("      CONSULTATION DE LA SAISON\n");
        printf("+----------------------------------------+\n");
        printf("1 - Consultation et reservation par spectacle\n");
        printf("2 - Consultation et reservation par salle\n");
        printf("0 - Retour menu principal\n");

        choix = lire_entier("Votre choix : ");

        switch (choix)
        {
            case 1 :
                consulter_par_spectacle();
                break;

            case 2 :
                consulter_par_salle();
                break;

            case 0 :
                break;

            default :
                printf("Choix incorrect.\n");
                break;
        }
    }
}

// Parcours progressifs apres le choix d'une representation
// Chaque fois l'user a consulte une information, il peut revenir au niveau precedent mais pas directement au menu
void parcourir_actions_representation(int indice_representation)
{
    int choix_admin;
    int retour_action;
    int retour_categories;
    int indice_categorie;
    int resultat_reservation;

    retour_action = 0;

    while (retour_action == 0)
    {
        afficher_jauges_representation(indice_representation);

        choix_admin = 1;
        if (compte_est_admin())
        {
            printf("\nVous voulez : \n");//actions a faire apres la consultation de representations
            printf("1 - Continuer pour effectuer une reservation\n");
            printf("2 - Exporter les reservations de cette representation\n");
            printf("0 - Retour au choix des representations\n");
            choix_admin = lire_entier("Votre choix : ");
        }

        if (choix_admin == 0)
        {
            retour_action = 1;
        }
        else if (choix_admin == 2)
        {
            exporter_reservations_representation_indice(indice_representation);
            attendre_retour_etape("Retour a l'action apres consultation de la representation");
        }
        else if (choix_admin == 1)
        {
            if (! representation_disponible(indice_representation))
            {
                printf("Cette representation est complete.\n");
                attendre_retour_etape("Retour au choix des representations"); //representation complete, on retourne pour choisir une autre representation
                retour_action = 1;
            }
            else //representation dispo, on va choisir une categorie des places
            {
                retour_categories = 0;

                while (retour_categories == 0)
                {
                    indice_categorie = choisir_categorie_consultation(indice_representation);

                    if (indice_categorie == RETOUR_CHOIX_CATEGORIES)
                    {
                        // La categorie choisie etait complete. On retourne pour choisir une autre categorie.
                        retour_categories = 0;
                    }
                    else if (indice_categorie == NON_TROUVE)
                    {
                        //Si on a pas trouve l'indice de la categorie que l'user a entre, 
                        // on retourne pour choisir une autre categorie.
                        retour_categories = 1;
                        retour_action = 1;
                    }
                    else //tout va bien et on peut reserver cette categorie de cette representation
                    {
                        resultat_reservation = reserver_depuis_consultation(indice_representation,indice_categorie,NON_TROUVE);

                        if (resultat_reservation == RETOUR_CHOIX_CATEGORIES)
                        {
                            //retour volontaire au choix de la categorie
                            retour_categories = 0;
                        }
                        else
                        {
                            retour_categories = 1;
                            retour_action = 1;
                        }
                    }
                }
            }
        }
        else
        {
            printf("Choix incorrect.\n");
        }
    }
}

/* Consultation par spectacle.
Le parcours est : spectacle -> representation -> jauge -> categorie -> beneficiaire -> tarif -> reservation. */
void consulter_par_spectacle()
{
    int indice_spectacle;
    int indice_representation;
    int spectacle_passe;
    int fin;
    int fin_representations;

    fin = 0;

    while (fin == 0)
    {
        indice_spectacle = choisir_spectacle_consultation("", 0, &spectacle_passe);

        if (indice_spectacle == NON_TROUVE)
        {
            fin = 1;
        }
        else
        {
            if (spectacle_passe)
            {
                afficher_representations_spectacle(tab_spectacles[indice_spectacle].id_spectacle);
                printf("Spectacle passe : les representations ne sont plus reservables.\n");
                attendre_retour_etape("Retour au choix des spectacles de la saison");
            }
            else
            {
                fin_representations = 0;

                while (fin_representations == 0)
                {
                    indice_representation = choisir_representation_consultation(tab_spectacles[indice_spectacle].id_spectacle, "", 0);

                    if (indice_representation == NON_TROUVE)
                    {
                        fin_representations = 1;
                    }
                    else
                    {
                        parcourir_actions_representation(indice_representation);
                    }
                }
            }
        }
    }
}


/* Consultation par salle.
Le parcours est le meme que par spectacle, mais on commence par choisir une salle. */
void consulter_par_salle()
{
    int indice_salle;
    int indice_spectacle;
    int indice_representation;
    int spectacle_passe;
    int fin;
    int fin_salle;
    int fin_representations;

    fin = 0;

    while (fin == 0)
    {
        indice_salle = choisir_salle();

        if (indice_salle == NON_TROUVE)
        {
            fin = 1;
        }
        else
        {
            fin_salle = 0;

            while (fin_salle == 0)
            {
                indice_spectacle = choisir_spectacle_consultation(tab_salles[indice_salle].id_salle, 1, &spectacle_passe);

                if (indice_spectacle == NON_TROUVE)
                {
                    fin_salle = 1;
                }
                else
                {
                    if (spectacle_passe)
                    {
                        afficher_representations_spectacle_salle(tab_spectacles[indice_spectacle].id_spectacle, tab_salles[indice_salle].id_salle);
                        printf("Spectacle passe dans cette salle : pas de reservation possible.\n");
                        attendre_retour_etape("Retour aux spectacles de cette salle");
                    }
                    else
                    {
                        fin_representations = 0;

                        while (fin_representations == 0)
                        {
                            indice_representation = choisir_representation_consultation(tab_spectacles[indice_spectacle].id_spectacle, tab_salles[indice_salle].id_salle, 1);

                            if (indice_representation == NON_TROUVE)
                            {
                                fin_representations = 1;
                            }
                            else
                            {
                                parcourir_actions_representation(indice_representation);
                            }
                        }
                    }
                }
            }
        }
    }
}


/* apres un scanf, on nettoie ce qui reste dans l'entree clavier.
Cette fonction evite que le scanf suivant lise un ancien retour a la ligne. */
void vider_buffer_clavier()
{
    while(getchar()!='\n');
}

//Lecture d'un entier au clavier. scanf retourne 1 si la saisie est correcte, sinon on retourne -1.
int lire_entier(char ch[])
{
    int nombre;
    int ok;

    printf("%s", ch);
    ok = scanf("%d", &nombre);
    vider_buffer_clavier();

    if (ok != 1)
    {
        nombre = -1;
    }

    return nombre;
}

//Lecture d'un mot sans espace.
void lire_mot(char ch[], char mot[])
{
    printf("%s", ch);
    scanf("%s", mot);
    vider_buffer_clavier();
}

//Lecture d'une chaine en gardant les espaces.
void lire_chaine(char ch[], char chaine[])
{
    lire_chaine_taille(ch, chaine, TAILLE_NOM);
}

//j'ai separe ces deux parce que la taille_max que je voulais mettre e argument n'est pas toujours taille_nom, mais ca arrive souvent quand c'est la taille_nom
void lire_chaine_taille(char ch[], char chaine[], int taille_max)
{
    int i;
    int c;

    printf("%s", ch);
    i = 0;
    c = getchar();

    while ((c != '\n') && (c != EOF) && (i < taille_max - 1))
    {
        chaine[i] = c;
        i++;
        c = getchar();
    }

    chaine[i] = '\0';

    while ((c != '\n') && (c != EOF))
    {
        c = getchar();
    }
}

/* Pause volontaire avant de revenir a l'etape precedente.
Le but est d'eviter un retour trop rapide au menu quand l'utilisateur vient
de consulter une liste ou un resultat final. */
void attendre_retour_etape(char message[])
{
    int choix;

    choix = -1;

    while (choix != 0)
    {
        printf("\n%s\n", message);
        choix = lire_entier("Entrez 0 pour retourner : ");

        if (choix != 0)
        {
            printf("Choix incorrect. Entrez 0 pour retourner.\n");
        }
    }
}

//Rechercher un spectacle
int rechercher_spectacle_par_id(char id_spectacle[])
{
    int i;

    for (i = 0 ; i < nbspectacles ; i++)
    {
        if (strcmp(tab_spectacles[i].id_spectacle, id_spectacle) == 0)
        {
            return i;
        }
    }

    return NON_TROUVE;
}

//Rechercher une representation
int rechercher_representation_par_id(char id_representation[])
{
    int i;

    for (i = 0 ; i < nbrepresentations ; i++)
    {
        if (strcmp(tab_representations[i].id_representation, id_representation) == 0)
        {
            return i;
        }
    }

    return NON_TROUVE;
}

//Rechercher une salle
int rechercher_salle_par_id(char id_salle[])
{
    int i;

    for (i = 0 ; i < nbsalles ; i++)
    {
        if (strcmp(tab_salles[i].id_salle, id_salle) == 0)
        {
            return i;
        }
    }

    return NON_TROUVE;
}

//Supprimer toutes les representations liees a un spectacle supprime
void supprimer_representations_spectacle(char id_spectacle[])
{
    int i;
    int j;

    i = 0;
    while (i < nbrepresentations)
    {
        if (strcmp(tab_representations[i].id_spectacle, id_spectacle) == 0)
        {
            //Avant suppression, on sauvegarde les reservations de la representation
            sauvegarder_reservations_representation_supprimee(i);
            supprimer_reservations_representation(tab_representations[i].id_representation);

            for (j = i ; j < nbrepresentations - 1 ; j++)
            {
                tab_representations[j] = tab_representations[j + 1];
            }
            nbrepresentations--;
        }
        else
        {
            i++;
        }
    }

    sauvegarder_reservations("DATA/08reservations.txt");
}

//Affichage des spectacles
void afficher_spectacles()
{
    int i;

    printf("\n");
    printf("+------------------------------------------------------------------------------+\n");
    printf("%-5s %-10s %-60s\n", "No", "ID", "Spectacle");
    printf("+------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbspectacles ; i++)
    {
        printf("%-5d %-10s %-60s\n",
               i + 1,
               tab_spectacles[i].id_spectacle,
               tab_spectacles[i].nom_spectacle);
    }

    printf("+------------------------------------------------------------------------------+\n");
}

//Choisir un spectacle dans la liste affichee
int choisir_spectacle()
{
    int choix;

    afficher_spectacles();
    choix = lire_entier("Choisissez un spectacle (0 pour Retour a l'etape precedente) : ");

    if ((choix < 1) || (choix > nbspectacles))
    {
        if (choix != 0)
        {
            printf("Choix incorrect. Entrez un numero de spectacle affiche ou 0 pour retourner.\n");
        }
        return NON_TROUVE;
    }

    return choix - 1;
}


//Tester si un spectacle a au moins une representation dans une salle
int spectacle_dans_salle(int indice_spectacle, char id_salle[])
{
    int i;

    for (i = 0 ; i < nbrepresentations ; i++)
    {
        if ((strcmp(tab_representations[i].id_spectacle, tab_spectacles[indice_spectacle].id_spectacle) == 0) && (strcmp(tab_representations[i].id_salle, id_salle) == 0))
        {
            return 1;
        }
    }

    return 0;
}

/* Un spectacle est passe si aucune representation a venir n'existe.
Si id_salle est renseigne, on ne regarde que les representations de cette salle. */
int spectacle_est_passe(char id_spectacle[], char id_salle[])
{
    int i;
    int trouve_representation;
    int trouve_a_venir;
    int meme_spectacle;
    int meme_salle;

    trouve_representation = 0;
    trouve_a_venir = 0;

    for (i = 0 ; i < nbrepresentations ; i++)
    {
        meme_spectacle = (strcmp(tab_representations[i].id_spectacle, id_spectacle) == 0);
        meme_salle = 1;

        if (strcmp(id_salle, "") != 0)
        {
            meme_salle = (strcmp(tab_representations[i].id_salle, id_salle) == 0);
        }

        if ((meme_spectacle) && (meme_salle))
        {
            trouve_representation = 1;
            if (! representation_passee(i))
            {
                trouve_a_venir = 1;
            }
        }
    }

    if (! trouve_representation)
    {
        return 1;
    }

    return (! trouve_a_venir);
}

// Choix d'un spectacle en separant les spectacles passes et a venir
int choisir_spectacle_consultation(char id_salle[], int avec_salle, int *spectacle_passe)
{
    int i;
    int choix;
    int compteur;
    int compteur_passe;
    int compteur_avenir;
    int passe;
    int afficher;
    int indices[MAX_SPECTACLES];
    int etats[MAX_SPECTACLES];
    int indice_salle_affiche;

    compteur = 0;
    compteur_passe = 0;
    compteur_avenir = 0;

    printf("\n");
    printf("+--------------------------------------------------------------------------------+\n");
    if (avec_salle)
    {
        indice_salle_affiche = rechercher_salle_par_id(id_salle);
        if (indice_salle_affiche != NON_TROUVE)
        {
            printf("Spectacles de la salle %s\n", tab_salles[indice_salle_affiche].nom_salle);
        }
        else
        {
            printf("Spectacles de la salle %s\n", id_salle);
        }
    }
    else
    {
        printf("Spectacles de la saison\n");
    }
    printf("+--------------------------------------------------------------------------------+\n");

    printf("\n--- Spectacles deja passes ---\n");
    for (i = 0 ; i < nbspectacles ; i++)
    {
        afficher = 1;
        if (avec_salle)
        {
            afficher = spectacle_dans_salle(i, id_salle);
        }

        if (afficher)
        {
            passe = spectacle_est_passe(tab_spectacles[i].id_spectacle, id_salle);
            if (passe)
            {
                compteur++;
                compteur_passe++;
                indices[compteur - 1] = i;
                etats[compteur - 1] = 1;
                printf("%-5d %-10s %-60s\n", compteur, tab_spectacles[i].id_spectacle, tab_spectacles[i].nom_spectacle);
            }
        }
    }

    if (compteur_passe == 0)
    {
        if (avec_salle)
        {
            printf("Aucun spectacle deja passe pour cette salle.\n");
        }
        else
        {
            printf("Aucun spectacle deje passe.\n");
        }
    }

    printf("\n--- Spectacles a venir ---\n");
    for (i = 0 ; i < nbspectacles ; i++)
    {
        afficher = 1;
        if (avec_salle)
        {
            afficher = spectacle_dans_salle(i, id_salle);
        }

        if (afficher)
        {
            passe = spectacle_est_passe(tab_spectacles[i].id_spectacle, id_salle);
            if (! passe)
            {
                compteur++;
                compteur_avenir++;
                indices[compteur - 1] = i;
                etats[compteur - 1] = 0;
                printf("%-5d %-10s %-60s\n", compteur, tab_spectacles[i].id_spectacle, tab_spectacles[i].nom_spectacle);
            }
        }
    }

    if (compteur_avenir == 0)
    {
        if (avec_salle)
        {
            printf("Aucun spectacle a venir pour cette salle.\n");
        }
        else
        {
            printf("Aucun spectacle a venir.\n");
        }
    }

    if (compteur == 0)
    {
        printf("Aucun spectacle trouve.\n");
        return NON_TROUVE;
    }

    if (avec_salle)
    {
        choix = lire_entier("Choisissez un spectacle (0 pour Retour au choix de la salle) : ");
    }
    else
    {
        choix = lire_entier("Choisissez un spectacle (0 pour Retour au menu principal) : ");
    }

    if ((choix < 1) || (choix > compteur))
    {
        if (choix != 0)
        {
            printf("Choix incorrect. Entrez un numero de spectacle affiche ou 0 pour retourner.\n");
        }
        return NON_TROUVE;
    }

    *spectacle_passe = etats[choix - 1];
    return indices[choix - 1];
}

//Afficher les representations d'un spectacle
void afficher_representations_spectacle(char id_spectacle[])
{
    int i;
    int compteur;
    char statut[20];

    compteur = 0;

    printf("\n");
    printf("+----------------------------------------------------------------------------------------------------------------+\n");
    printf("%-5s %-8s %-12s %-18s %-8s %-35s %-12s\n", "No", "ID", "Date", "Horaire", "Salle", "Nom salle", "Statut");
    printf("+----------------------------------------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbrepresentations ; i++)
    {
        if (strcmp(tab_representations[i].id_spectacle, id_spectacle) == 0)
        {
            compteur++;
            if (representation_passee(i))
            {
                strcpy(statut, "Passe");
            }
            else if (representation_disponible(i))
            {
                strcpy(statut, "Disponible");
            }
            else
            {
                strcpy(statut, "Complet");
            }

            printf("%-5d %-8s %-12s %-18s %-8s %-35s %-12s\n",
                   compteur,
                   tab_representations[i].id_representation,
                   tab_representations[i].date_representation,
                   tab_representations[i].horaire_representation,
                   tab_representations[i].id_salle,
                   tab_representations[i].nom_salle,
                   statut);
        }
    }

    if (compteur == 0)
    {
        printf("Aucune representation pour ce spectacle.\n");
    }

    printf("+----------------------------------------------------------------------------------------------------------------+\n");
}

//Choix d'une representation d'un spectacle
int choisir_representation_spectacle(char id_spectacle[])
{
    int i;
    int choix;
    int compteur;

    compteur = 0;

    afficher_representations_spectacle(id_spectacle);
    choix = lire_entier("Choisissez une representation (0 pour Retour au choix du spectacle) : ");

    if (choix <= 0)
    {
        return NON_TROUVE;
    }

    for (i = 0 ; i < nbrepresentations ; i++)
    {
        if (strcmp(tab_representations[i].id_spectacle, id_spectacle) == 0)
        {
            compteur++;
            if (compteur == choix)
            {
                return i;
            }
        }
    }

    return NON_TROUVE;
}


//Afficher les spectacles programmes dans une salle sans separer par statut
void afficher_spectacles_salle(char id_salle[])
{
    int i;
    int compteur;

    compteur = 0;

    printf("\n");
    printf("+------------------------------------------------------------------------------+\n");
    printf("%-5s %-10s %-60s\n", "No", "ID", "Spectacle");
    printf("+------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbspectacles ; i++)
    {
        if (spectacle_dans_salle(i, id_salle))
        {
            compteur++;
            printf("%-5d %-10s %-60s\n",
                   compteur,
                   tab_spectacles[i].id_spectacle,
                   tab_spectacles[i].nom_spectacle);
        }
    }

    if (compteur == 0)
    {
        printf("Aucun spectacle dans cette salle.\n");
    }

    printf("+------------------------------------------------------------------------------+\n");
}

//Choisir un spectacle parmi ceux d'une salle
int choisir_spectacle_salle(char id_salle[])
{
    int i;
    int choix;
    int compteur;

    compteur = 0;

    afficher_spectacles_salle(id_salle);
    choix = lire_entier("Choisissez un spectacle (0 pour Retour au choix de la salle) : ");

    if (choix <= 0)
    {
        return NON_TROUVE;
    }

    for (i = 0 ; i < nbspectacles ; i++)
    {
        if (spectacle_dans_salle(i, id_salle))
        {
            compteur++;
            if (compteur == choix)
            {
                return i;
            }
        }
    }

    return NON_TROUVE;
}

//Afficher les representations d'un spectacle dans une salle precise
void afficher_representations_spectacle_salle(char id_spectacle[], char id_salle[])
{
    int i;
    int compteur;
    char statut[20];

    compteur = 0;

    printf("\n");
    printf("+----------------------------------------------------------------------------------------------------------------+\n");
    printf("%-5s %-8s %-12s %-18s %-8s %-35s %-12s\n", "No", "ID", "Date", "Horaire", "Salle", "Nom salle", "Statut");
    printf("+----------------------------------------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbrepresentations ; i++)
    {
        if ((strcmp(tab_representations[i].id_spectacle, id_spectacle) == 0) &&
            (strcmp(tab_representations[i].id_salle, id_salle) == 0))
        {
            compteur++;
            if (representation_passee(i))
            {
                strcpy(statut, "Passe");
            }
            else if (representation_disponible(i))
            {
                strcpy(statut, "Disponible");
            }
            else
            {
                strcpy(statut, "Complet");
            }

            printf("%-5d %-8s %-12s %-18s %-8s %-35s %-12s\n",
                   compteur,
                   tab_representations[i].id_representation,
                   tab_representations[i].date_representation,
                   tab_representations[i].horaire_representation,
                   tab_representations[i].id_salle,
                   tab_representations[i].nom_salle,
                   statut);
        }
    }

    if (compteur == 0)
    {
        printf("Aucune representation pour ce spectacle dans cette salle.\n");
    }

    printf("+----------------------------------------------------------------------------------------------------------------+\n");
}

//Choisir une representation d'un spectacle dans une salle
int choisir_representation_spectacle_salle(char id_spectacle[], char id_salle[])
{
    int i;
    int choix;
    int compteur;

    compteur = 0;

    afficher_representations_spectacle_salle(id_spectacle, id_salle);
    choix = lire_entier("Choisissez une representation (0 pour Retour aux spectacles de cette salle) : ");

    if (choix <= 0)
    {
        return NON_TROUVE;
    }

    for (i = 0 ; i < nbrepresentations ; i++)
    {
        if ((strcmp(tab_representations[i].id_spectacle, id_spectacle) == 0) && (strcmp(tab_representations[i].id_salle, id_salle) == 0))
        {
            compteur++;
            if (compteur == choix)
            {
                return i;
            }
        }
    }

    return NON_TROUVE;
}


//Une representation est disponible si au moins une categorie contient encore une place
int representation_disponible(int indice_representation)
{
    int i;
    int restantes;

    if ((indice_representation < 0) || (indice_representation >= nbrepresentations))
    {
        return 0;
    }

    for (i = 0 ; i < nbcategories ; i++)
    {
        if (strcmp(tab_categories[i].id_salle, tab_representations[indice_representation].id_salle) == 0)
        {
            restantes = calculer_places_restantes(tab_representations[indice_representation].id_representation, tab_categories[i].id_categorie_billet);
            if (restantes > 0)
            {
                return 1;
            }
        }
    }

    return 0;
}

//Choix d'une representation a venir, avec le filtre disponible ou non
int choisir_representation_consultation(char id_spectacle[], char id_salle[], int avec_salle)
{
    int i;
    int choix;
    int compteur;
    int seulement_disponibles;
    int disponible;
    int afficher;
    int indices[MAX_REPRESENTATIONS];
    char statut[20];

    seulement_disponibles = lire_entier("Afficher uniquement les representations disponibles (1 oui / 0 non) : ");
    if (seulement_disponibles != 1)
    {
        seulement_disponibles = 0;
    }

    compteur = 0;

    printf("\nRepresentations a venir\n");
    printf("+----------------------------------------------------------------------------------------------------------------+\n");
    printf("%-5s %-8s %-12s %-18s %-8s %-35s %-12s\n", "No", "ID", "Date", "Horaire", "Salle", "Nom salle", "Statut");
    printf("+----------------------------------------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbrepresentations ; i++)
    {
        afficher = 0;

        if (strcmp(tab_representations[i].id_spectacle, id_spectacle) == 0)
        {
            afficher = 1;
        }

        if ((avec_salle) && (strcmp(tab_representations[i].id_salle, id_salle) != 0))
        {
            afficher = 0;
        }

        if ((afficher) && (! representation_passee(i)))
        {
            disponible = representation_disponible(i);

            if ((! seulement_disponibles) || (disponible))
            {
                if (disponible)
                {
                    strcpy(statut, "Disponible");
                }
                else
                {
                    strcpy(statut, "Complet");
                }

                compteur++;
                indices[compteur - 1] = i;
                printf("%-5d %-8s %-12s %-18s %-8s %-35s %-12s\n",
                       compteur,
                       tab_representations[i].id_representation,
                       tab_representations[i].date_representation,
                       tab_representations[i].horaire_representation,
                       tab_representations[i].id_salle,
                       tab_representations[i].nom_salle,
                       statut);
            }
        }
    }

    if (compteur == 0)
    {
        printf("Aucune representation a venir pour ce spectacle.\n");
        return NON_TROUVE;
    }

    printf("+----------------------------------------------------------------------------------------------------------------+\n");
    if (avec_salle)
    {
        choix = lire_entier("Choisissez une representation (0 pour Retour aux spectacles de cette salle) : ");
    }
    else
    {
        choix = lire_entier("Choisissez une representation (0 pour Retour aux spectacles de la saison) : ");
    }

    if ((choix < 1) || (choix > compteur))
    {
        if (choix != 0)
        {
            printf("Choix incorrect. Entrez un numero de representation affiche ou 0 pour retourner.\n");
        }
        return NON_TROUVE;
    }

    if (! representation_disponible(indices[choix - 1]))
    {
        if (compte_est_admin())
        {
            printf("Cette representation est complete : consultation et export possibles, reservation impossible.\n");
        }
        else
        {
            printf("Cette representation est complete. Reservation impossible.\n");
            return NON_TROUVE;
        }
    }

    return indices[choix - 1];
}

//Choix d'une categorie de place apres l'affichage de la jauge
int choisir_categorie_consultation(int indice_representation)
{
    int i;
    int choix;
    int compteur;
    int restantes;
    int indices[MAX_CATEGORIES];

    if ((indice_representation < 0) || (indice_representation >= nbrepresentations))
    {
        return NON_TROUVE;
    }

    compteur = 0;

    printf("\nChoix de la categorie de place\n");
    printf("+-----------------------------------------------------------------------------------+\n");
    printf("%-5s %-10s %-35s %-12s %-12s\n", "No", "Cat", "Categorie", "Jauge", "Restantes");
    printf("+-----------------------------------------------------------------------------------+\n");

    for (i = 0 ; i < nbcategories ; i++)
    {
        if (strcmp(tab_categories[i].id_salle, tab_representations[indice_representation].id_salle) == 0)
        {
            restantes = calculer_places_restantes(tab_representations[indice_representation].id_representation,tab_categories[i].id_categorie_billet);
            compteur++;
            indices[compteur - 1] = i;
            printf("%-5d %-10s %-35s %-12d %-12d\n",
                   compteur,
                   tab_categories[i].id_categorie_billet,
                   tab_categories[i].categorie_billet,
                   tab_categories[i].jauge_categorie_billet,
                   restantes);
        }
    }

    if (compteur == 0)
    {
        printf("Aucune categorie trouvee.\n");
        return NON_TROUVE;
    }

    printf("+-----------------------------------------------------------------------------------+\n");
    choix = lire_entier("Choisissez une categorie (0 pour Retour au choix de la representation) : ");

    if ((choix < 1) || (choix > compteur))
    {
        if (choix != 0)
        {
            printf("Choix incorrect. Entrez un numero de categorie affiche ou 0 pour retourner.\n");
        }
        return NON_TROUVE;
    }

    restantes = calculer_places_restantes(tab_representations[indice_representation].id_representation, tab_categories[indices[choix - 1]].id_categorie_billet);
    if (restantes <= 0)
    {
        printf("Cette categorie est complete.\n");
        attendre_retour_etape("Retour au choix des categories");
        return RETOUR_CHOIX_CATEGORIES;
    }

    return indices[choix - 1];
}

//Choix propose apres l'affichage des tarifs : choisir les tarifs d'une autre representation ou retouner au choix du spectacle
int choisir_suite_tarifs()
{
    int choix;

    choix = -1;

    while ((choix != 0) && (choix != 1))
    {
        printf("\n");
        printf("1 - Consulter les tarifs d'une autre representation\n");
        printf("0 - Retour aux spectacles\n");
        choix = lire_entier("Votre choix : ");

        if ((choix != 0) && (choix != 1))
        {
            printf("Choix incorrect.\n");
        }
    }

    return choix;
}

//Afficher tous les tarifs possibles d'une representation
void afficher_tarifs_representation(int indice_representation)
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

        printf("\n");
        printf("+------------------------------------------------------------------------------------------------------------------------+\n");
        printf("Representation : %s\n", rep.id_representation);
        printf("Spectacle      : %s\n", rep.nom_spectacle);
        printf("Date           : %s\n", rep.date_representation);
        printf("Horaire        : %s\n", rep.horaire_representation);
        printf("Salle          : %s - %s\n", rep.id_salle, rep.nom_salle);
        printf("+------------------------------------------------------------------------------------------------------------------------+\n");
        printf("%-8s %-24s %-14s %-8s %-8s %-8s %-40s\n", "Cat", "Categorie billet", "Type", "Jauge", "Reste", "Tarif", "Profil");
        printf("+------------------------------------------------------------------------------------------------------------------------+\n");

        for (i = 0 ; i < nbtarifs ; i++)
        {
            if (tarif_correspond_representation(i, indice_representation))
            {
                restantes = calculer_places_restantes(rep.id_representation,tab_tarifs[i].id_categorie_billet);

                printf("%-8s %-24s %-14s %-8d %-8d %-8d %-40s\n",
                    tab_tarifs[i].id_categorie_billet,
                    tab_tarifs[i].categorie_billet,
                    tab_tarifs[i].type_tarif,
                    tab_tarifs[i].jauge_categorie_billet,
                    restantes,
                    tab_tarifs[i].tarif_brut,
                    tab_tarifs[i].profil_tarif);
                trouve = 1;
            }
        }

        if (! trouve)
        {
            printf("Aucun tarif trouve pour cette representation.\n");
        }

        printf("+------------------------------------------------------------------------------------------------------------------------+\n");
    }
}
