/* Programme principal. Le menu change selon le droit du compte connecte. */

#include <stdio.h>
#include "declarations.h"

/* Demande de chargement au debut pour l'administrateur.
Le chargement n'est pas automatique, car l'administrateur doit le lancer lui-meme. */
void demander_chargement_depart_admin()
{
    int choix;

    choix = -1;

    while ((choix != 0) && (choix != 1))
    {
        printf("\n");
        printf("+-------------------------------------------------+\n");
        printf("              CHARGEMENT DE LA SAISON\n");
        printf("+-------------------------------------------------+\n");
        printf("1 - Charger la saison maintenant\n");
        printf("0 - Continuer sans charger la saison\n");
        choix = lire_entier("Votre choix : ");

        switch (choix)
        {
            case 1 :
                charger_saison_admin();
                break;
            case 0 :
                printf("Saison non chargée. Si vous allez changer l'idée, vous pourrez la charger depuis le menu.\n");
                break;
            default :
                printf("Choix incorrect. Tapez 1 pour charger ou 0 pour continuer.\n");
                break;
        }
    }
}

int main()
{
    int choix;
    int continuer;

    choix = -1;
    continuer = 1;

    /* Au lancement, on remet les tableaux a zero.
    On charge seulement les comptes, car ils sont nécessaires pour se connecter.
    La saison n'est pas considerée comme active par defaut (0 au départ).
    Si l'administrateur l'a chargée auparavant, l'état est renouvelé dans le fichier DATA/etat_saison. */
    initialiser_donnees();
    initialiser_donnees_systeme();

    charger_comptes("DATA/07comptes.txt");

    /* Si un administrateur a déjà chargé la saison dans un lancement précédent,
       l'état est memorise dans DATA/etat_saison.txt.
       On recharge donc les fichiers de saison en mémoire pour tous les comptes. */
    charger_saison_si_active();

    /* Connexion obligatoire avant tout acces au logiciel. */
    authentifier();

    /* Si l'administrateur se connecte, il peut charger la saison tout de suite avant d'entrer dans le menu principal.
    Les autres comptes ne peuvent pas charger la saison. */
    if (compte_est_admin())
    {
        if (saison_chargee)
        {
            printf("Saison déjà activée.\n");
        }
        else
        {
            demander_chargement_depart_admin();
        }
    }

    while (continuer)
    {
        printf("\n");
        printf("+-------------------------------------------------+\n");
        printf("   COMEDIE-FRANCAISE - GESTION DES RESERVATIONS\n");
        printf("+-------------------------------------------------+\n");
        afficher_compte_connecte();

        if (compte_est_admin())
        {
            printf("1 - Consultation et réservation par spectacle\n");
            printf("2 - Consultation et réservation par salle\n");
            printf("3 - Gestion des spectacles\n");
            printf("4 - Gestion des représentations\n");
            printf("5 - Gestion des réservations\n");
            printf("6 - Gestion des abonnés\n");
            printf("7 - Initialiser la saison\n");
            printf("8 - Charger la saison\n");
            printf("9 - Sauvegarder les données\n");
            printf("10 - Quitter\n");
        }
        else if (compte_est_abonne())
        {
            printf("1 - Consultation et réservation par spectacle\n");
            printf("2 - Consultation et réservation par salle\n");
            printf("3 - Mes réservations\n");
            printf("4 - Mes informations d'abonné\n");
            printf("5 - Quitter\n");
        }
        else
        {
            printf("1 - Consultation et réservation par spectacle\n");
            printf("2 - Consultation et réservation par salle\n");
            printf("3 - Mes réservations\n");
            printf("4 - Mes informations personnelles\n");
            printf("5 - Quitter\n");
        }

        choix = lire_entier("Votre choix : ");

        if (compte_est_admin())
        {
            switch (choix)
            {
                case 1 :
                    if (verifier_donnees_consultation())
                    {
                        consulter_par_spectacle();
                    }
                    break;

                case 2 :
                    if (verifier_donnees_consultation())
                    {
                        consulter_par_salle();
                    }
                    break;

                case 3 :
                    if (verifier_donnees_consultation())
                    {
                        menu_gestion_spectacles();
                    }
                    break;

                case 4 :
                    if (verifier_donnees_consultation())
                    {
                        menu_gestion_representations();
                    }
                    break;

                case 5 :
                    if (verifier_donnees_consultation())
                    {
                        menu_reservation_admin();
                    }
                    break;

                case 6 :
                    menu_abonnes();
                    break;

                case 7 :
                    initialiser_nouvelle_saison();
                    break;

                case 8 :
                    charger_saison_admin();
                    break;

                case 9 :
                    sauvegarder_toutes_les_donnees();
                    break;

                case 10 :
                    sauvegarder_toutes_les_donnees();
                    printf("Merci. Au revoir.\n");
                    continuer = 0;
                    break;

                default :
                    printf("Choix incorrect. Tapez un choix du menu administrateur.\n");
                    break;
            }
        }
        else if (compte_est_abonne())
        {
            switch (choix)
            {
                case 1 :
                    if (verifier_donnees_consultation())
                    {
                        consulter_par_spectacle();
                    }
                    break;

                case 2 :
                    if (verifier_donnees_consultation())
                    {
                        consulter_par_salle();
                    }
                    break;

                case 3 :
                    consulter_reservations_compte(indice_compte_connecte);
                    break;

                case 4 :
                    modifier_mes_infos_abonne();
                    break;

                case 5 :
                    if (nbreservations > 0)
                    {
                        sauvegarder_reservations("DATA/08reservations.txt");
                    }
                    if (nbabonnes > 0)
                    {
                        sauvegarder_abonnes("DATA/09abonnes.txt");
                    }
                    printf("Merci. Au revoir.\n");
                    continuer = 0;
                    break;

                default :
                    printf("Choix incorrect. Tapez un choix du menu utilisateur.\n");
                    break;
            }
        }
        else
        {
            switch (choix)
            {
                case 1 :
                    if (verifier_donnees_consultation())
                    {
                        consulter_par_spectacle();
                    }
                    break;

                case 2 :
                    if (verifier_donnees_consultation())
                    {
                        consulter_par_salle();
                    }
                    break;

                case 3 :
                    consulter_reservations_compte(indice_compte_connecte);
                    break;

                case 4 :
                    modifier_mes_infos_compte();
                    break;

                case 5 :
                    if (nbreservations > 0)
                    {
                        sauvegarder_reservations("DATA/08reservations.txt");
                    }
                    sauvegarder_comptes("DATA/07comptes.txt");
                    printf("Merci. Au revoir.\n");
                    continuer = 0;
                    break;

                default :
                    printf("Choix incorrect. Tapez un choix du menu utilisateur.\n");
                    break;
            }
        }
    }
}
