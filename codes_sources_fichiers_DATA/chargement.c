/* Ce fichier sert a charger des fichiers texte de la saison */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "declarations.h"

struct SPECTACLE tab_spectacles[MAX_SPECTACLES];
struct SALLE tab_salles[MAX_SALLES];
struct CATEGORIE_BILLET_SALLE tab_categories[MAX_CATEGORIES];
struct CATEGORIE_HORAIRE tab_horaires[MAX_HORAIRES];
struct CATEGORIE_PROFIL tab_profils[MAX_PROFILS];
struct REPRESENTATION tab_representations[MAX_REPRESENTATIONS];
struct TARIF tab_tarifs[MAX_TARIFS];
struct COMPTE tab_comptes[MAX_COMPTES];
struct RESERVATION tab_reservations[MAX_RESERVATIONS];
struct ABONNE tab_abonnes[MAX_ABONNES];

int nbspectacles = 0;
int nbsalles = 0;
int nbcategories = 0;
int nbhoraires = 0;
int nbprofils = 0;
int nbrepresentations = 0;
int nbtarifs = 0;
int nbcomptes = 0;
int nbreservations = 0;
int nbabonnes = 0;
int indice_compte_connecte = NON_TROUVE;
int saison_chargee = 0;

/* Nous avons choisi de lire les fichiers ligne par ligne avec fgets,
puis champ par champ avec le separateur. 
Cette methode evite les limites de fscanf, 
notamment pour les noms de spectacles contenant des espaces, 
et rend le chargement plus fiable. 
*/
int lire_ligne_fichier(FILE *f, char ligne[]);
void lire_champ(char ligne[], int *position, char champ[]);
void preparer_dossier_archive(char dossier_archive[]);
void construire_nom_archive_dans_dossier(char nom_archive[], char dossier_archive[], char base[]);
void construire_nom_archive(char nom_archive[], char base[]);
void ecrire_etat_saison(int etat);


/* Lecture de l'etat de la saison dans un fichier. par defaut c'est non charge
Cette information est conservee meme si le logiciel est ferme.
0 veut dire : aucune saison active.
1 veut dire : une saison a deja ete chargee par l'administrateur. */
int lire_etat_saison()
{
    FILE *fentree;
    char ligne[TAILLE_LIGNE];
    int etat;

    etat = 0;
    fentree = fopen("DATA/etat_saison.txt", "r");

    if (fentree != NULL)
    {
        if (fgets(ligne, TAILLE_LIGNE, fentree) != NULL)
        {
            etat = atoi(ligne);
        }
        fclose(fentree);
    }

    if (etat != 1)
    {
        etat = 0;
    }

    return etat;
}

/* ecriture de l'etat de la saison dans un fichier texte.
Ce fichier sert uniquement a memoriser si la saison est active entre deux lancements. */
void ecrire_etat_saison(int etat)
{
    FILE *fsortie;

    fsortie = fopen("DATA/etat_saison.txt", "w");

    if (fsortie == NULL)
    {
        printf("Pb acces fichier DATA/etat_saison.txt\n");
    }
    else
    {
        if (etat == 1)
        {
            fprintf(fsortie, "1\n");
        }
        else
        {
            fprintf(fsortie, "0\n");
        }
        fclose(fsortie);
    }
}

//on appelle les chargements de tous les fichiers
int charger_toutes_les_donnees()
{
    int ok;

    ok = 1;

    //On remet les donnees de saison a zero. Cela evite de doubler les lignes si l'administrateur charge deux fois.
    initialiser_donnees();

    if (charger_spectacles("DATA/00spectacle_25_26.txt") <= 0 ||
        charger_salles("DATA/01info_salle.txt") <= 0 ||
        charger_categories("DATA/02cat_billet_salle.txt") <= 0 ||
        charger_horaires("DATA/03cat_horaire.txt") <= 0 ||
        charger_profils("DATA/04cat_profil.txt") <= 0 ||
        charger_representations("DATA/05representation_25_26.txt") <= 0 ||
        charger_tarifs("DATA/06info_tarif.txt") <= 0) {
        ok = 0;
    }

    printf("\nResultat du chargement\n");
    printf("Spectacles : %d\n", nbspectacles);
    printf("Salles : %d\n", nbsalles);
    printf("Categories : %d\n", nbcategories);
    printf("Horaires : %d\n", nbhoraires);
    printf("Profils : %d\n", nbprofils);
    printf("Representations : %d\n", nbrepresentations);
    printf("Tarifs : %d\n", nbtarifs);

    if (! ok) //ok==0
    {
        printf("Le chargement est incomplet. Verifiez les fichiers dans DATA.\n");
    }

    return ok;
}

/* Rechargement de la saison par l'administrateur.
On charge les fichiers de saison, puis les reservations et les abonnes.
Les comptes ne sont pas recharges ici, car ils sont deja charges pour la connexion. */
int charger_saison_admin()
{
    int ok;

    printf("\nChargement de la saison\n");

    nbreservations = 0;
    nbabonnes = 0;

    ok = charger_toutes_les_donnees();

    charger_reservations("DATA/08reservations.txt");
    charger_abonnes("DATA/09abonnes.txt");

    printf("Reservations : %d\n", nbreservations);
    printf("Abonnes : %d\n", nbabonnes);

    if (ok)
    {
        saison_chargee = 1;
        ecrire_etat_saison(1);
        printf("Saison chargee. Vous pouvez maintenant consulter et reserver.\n");
        printf("etat memorise dans DATA/etat_saison.txt.\n");
    }
    else
    {
        saison_chargee = 0;
        ecrire_etat_saison(0);
        printf("Chargement incomplet. Verifiez les fichiers DATA avant de continuer.\n");
    }

    return ok;
}

/* Au lancement du logiciel, si DATA/etat_saison.txt indique que la saison est active,
le programme recharge automatiquement les fichiers en memoire.
c'est pour la recuperation de l'etat que l'administrateur a valide auparavant. */
int charger_saison_si_active()
{
    int etat;
    int ok;

    etat = lire_etat_saison();
    ok = 0;

    if (etat == 1)
    {
        nbreservations = 0;
        nbabonnes = 0;

        ok = charger_toutes_les_donnees();
        charger_reservations("DATA/08reservations.txt");
        charger_abonnes("DATA/09abonnes.txt");

        if (ok)
        {
            saison_chargee = 1;
            printf("Saison active rechargee automatiquement depuis DATA.\n");
        }
        else
        {
            saison_chargee = 0;
            printf("etat saison actif, mais chargement incomplet.\n");
            printf("Un administrateur doit verifier les fichiers DATA et recharger la saison.\n");
        }
    }
    else
    {
        saison_chargee = 0;
    }

    return saison_chargee;
}

// Verifier que les donnees minimales sont chargees avant la consultation ou la gestion des representations
int verifier_donnees_consultation()
{
    if ((nbspectacles <= 0) || (nbrepresentations <= 0) || (nbtarifs <= 0))
    {
        printf("Veuillez d'abord charger la saison.\n");
        return 0;
    }

    return 1;
}

//Remettre tous les compteurs a zero. Cela evite de conserver de vieilles donnees et doubler le chargement
void initialiser_donnees()
{
    nbspectacles = 0;
    nbsalles = 0;
    nbcategories = 0;
    nbhoraires = 0;
    nbprofils = 0;
    nbrepresentations = 0;
    nbtarifs = 0;
}

// Remettre a zero les donnees liees aux comptes, reservations et abonnes.
// Cette initialisation est separee car les comptes doivent exister avant la connexion.
void initialiser_donnees_systeme()
{
    nbcomptes = 0;
    nbreservations = 0;
    nbabonnes = 0;
    indice_compte_connecte = NON_TROUVE;
}

//Lecture d'une ligne
int lire_ligne_fichier(FILE *f, char ligne[])
{
    int longueur;

    if (fgets(ligne, TAILLE_LIGNE, f) == NULL)
    {
        return 0;
    }

    longueur = strlen(ligne);

    if ((longueur > 0) && (ligne[longueur - 1] == '\n'))
    {
        ligne[longueur - 1] = '\0';
        longueur--;
    }

    return 1;
}

/* Recopie un champ jusqu'au point-virgule ou la fin de ligne.
La variable position est passee par adresse pour que la fonction suivante continue au bon endroit dans la ligne. */
void lire_champ(char ligne[], int *position, char champ[])
{
    int i;

    i = 0;

    while ((ligne[*position] != ';') && (ligne[*position] != '\0'))
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

//Charger le fichier des spectacles. La premiere ligne est ignoree
int charger_spectacles(char nom_fichier[])
{
    FILE *fentree;
    char ligne[TAILLE_LIGNE];
    int position;
    struct SPECTACLE spect;

    nbspectacles = 0;
    fentree = fopen(nom_fichier, "r");

    if (fentree == NULL)
    {
        printf("Pb acces fichier %s\n", nom_fichier);
    }
    else
    {
        lire_ligne_fichier(fentree, ligne); 

        while ((lire_ligne_fichier(fentree, ligne)) && (nbspectacles < MAX_SPECTACLES))//avec le contrôle de lire_ligne, je n'ai pas besoin de feof
        {
            if (strcmp(ligne, "") != 0)
            {
                position = 0;
                lire_champ(ligne, &position, spect.id_spectacle);
                lire_champ(ligne, &position, spect.nom_spectacle);
                tab_spectacles[nbspectacles] = spect;
                nbspectacles++;
            }
        }
        fclose(fentree);
    }

    return nbspectacles;
}

// Charger les salles. La jauge est lue comme une chaine puis convertie en entier avec atoi.
int charger_salles(char nom_fichier[])
{
    FILE *fentree;
    char ligne[TAILLE_LIGNE];
    char champ[TAILLE_LIBELLE];
    int position;
    struct SALLE salle;

    nbsalles = 0;
    fentree = fopen(nom_fichier, "r");

    if (fentree == NULL)
    {
        printf("Pb acces fichier %s\n", nom_fichier);
    }
    else
    {
        lire_ligne_fichier(fentree, ligne);

        while ((lire_ligne_fichier(fentree, ligne)) && (nbsalles < MAX_SALLES))
        {
            if (strcmp(ligne, "") != 0)
            {
                position = 0;
                lire_champ(ligne, &position, salle.id_salle);
                lire_champ(ligne, &position, salle.nom_salle);
                lire_champ(ligne, &position, champ);
                salle.jauge_total_salle = atoi(champ);
                tab_salles[nbsalles] = salle;
                nbsalles++;
            }
        }
        fclose(fentree);
    }

    return nbsalles;
}

//Charger les categories de billets par salle
int charger_categories(char nom_fichier[])
{
    FILE *fentree;
    char ligne[TAILLE_LIGNE];
    char champ[TAILLE_LIBELLE];
    int position;
    struct CATEGORIE_BILLET_SALLE cat;

    nbcategories = 0;
    fentree = fopen(nom_fichier, "r");

    if (fentree == NULL)
    {
        printf("Pb acces fichier %s\n", nom_fichier);
    }
    else
    {
        lire_ligne_fichier(fentree, ligne);

        while ((lire_ligne_fichier(fentree, ligne)) && (nbcategories < MAX_CATEGORIES))
        {
            if (strcmp(ligne, "") != 0)
            {
                position = 0;
                lire_champ(ligne, &position, cat.id_salle);
                lire_champ(ligne, &position, cat.nom_salle);
                lire_champ(ligne, &position, cat.id_categorie_billet);
                lire_champ(ligne, &position, cat.categorie_billet);
                lire_champ(ligne, &position, champ);
                cat.jauge_categorie_billet = atoi(champ);
                tab_categories[nbcategories] = cat;
                nbcategories++;
            }
        }
        fclose(fentree);
    }

    return nbcategories;
}

//Charger la table de reference des horaires
int charger_horaires(char nom_fichier[])
{
    FILE *fentree;
    char ligne[TAILLE_LIGNE];
    int position;
    struct CATEGORIE_HORAIRE horaire;

    nbhoraires = 0;
    fentree = fopen(nom_fichier, "r");

    if (fentree == NULL)
    {
        printf("Pb acces fichier %s\n", nom_fichier);
    }
    else
    {
        lire_ligne_fichier(fentree, ligne);

        while ((lire_ligne_fichier(fentree, ligne)) && (nbhoraires < MAX_HORAIRES))
        {
            if (strcmp(ligne, "") != 0)
            {
                position = 0;
                lire_champ(ligne, &position, horaire.libelle_horaire);
                lire_champ(ligne, &position, horaire.id_cat_horaire);
                tab_horaires[nbhoraires] = horaire;
                nbhoraires++;
            }
        }
        fclose(fentree);
    }

    return nbhoraires;
}

//Charger les profils tarifaires
int charger_profils(char nom_fichier[])
{
    FILE *fentree;
    char ligne[TAILLE_LIGNE];
    int position;
    struct CATEGORIE_PROFIL profil;

    nbprofils = 0;
    fentree = fopen(nom_fichier, "r");

    if (fentree == NULL)
    {
        printf("Pb acces fichier %s\n", nom_fichier);
    }
    else
    {
        lire_ligne_fichier(fentree, ligne);

        while ((lire_ligne_fichier(fentree, ligne)) && (nbprofils < MAX_PROFILS))
        {
            if (strcmp(ligne, "") != 0)
            {
                position = 0;
                lire_champ(ligne, &position, profil.id_cat_profil);
                lire_champ(ligne, &position, profil.profil_tarif);
                lire_champ(ligne, &position, profil.type_tarif);
                tab_profils[nbprofils] = profil;
                nbprofils++;
            }
        }
        fclose(fentree);
    }

    return nbprofils;
}

//Charger les representations
int charger_representations(char nom_fichier[])
{
    FILE *fentree;
    char ligne[TAILLE_LIGNE];
    int position;
    struct REPRESENTATION rep;

    nbrepresentations = 0;
    fentree = fopen(nom_fichier, "r");

    if (fentree == NULL)
    {
        printf("Pb acces fichier %s\n", nom_fichier);
    }
    else
    {
        lire_ligne_fichier(fentree, ligne);

        while ((lire_ligne_fichier(fentree, ligne)) && (nbrepresentations < MAX_REPRESENTATIONS))
        {
            if (strcmp(ligne, "") != 0)
            {
                position = 0;
                lire_champ(ligne, &position, rep.id_representation);
                lire_champ(ligne, &position, rep.id_spectacle);
                lire_champ(ligne, &position, rep.nom_spectacle);
                lire_champ(ligne, &position, rep.jour_representation);
                lire_champ(ligne, &position, rep.heure_representation);
                lire_champ(ligne, &position, rep.horaire_representation);
                lire_champ(ligne, &position, rep.id_cat_representation);
                lire_champ(ligne, &position, rep.date_representation);
                lire_champ(ligne, &position, rep.id_salle);
                lire_champ(ligne, &position, rep.nom_salle);
                tab_representations[nbrepresentations] = rep;
                nbrepresentations++;
            }
        }
        fclose(fentree);
    }

    return nbrepresentations;
}

//Charger les tarifs
int charger_tarifs(char nom_fichier[])
{
    FILE *fentree;
    char ligne[TAILLE_LIGNE];
    char champ[TAILLE_LIBELLE];
    int position;
    struct TARIF tarif;

    nbtarifs = 0;
    fentree = fopen(nom_fichier, "r");

    if (fentree == NULL)
    {
        printf("Pb acces fichier %s\n", nom_fichier);
    }
    else
    {
        lire_ligne_fichier(fentree, ligne);

        while ((lire_ligne_fichier(fentree, ligne)) && (nbtarifs < MAX_TARIFS))
        {
            if (strcmp(ligne, "") != 0)
            {
                position = 0;
                lire_champ(ligne, &position, tarif.id_categorie_billet);
                lire_champ(ligne, &position, tarif.id_salle);
                lire_champ(ligne, &position, tarif.nom_salle);
                lire_champ(ligne, &position, tarif.categorie_billet);
                lire_champ(ligne, &position, champ);
                tarif.jauge_categorie_billet = atoi(champ);
                lire_champ(ligne, &position, tarif.type_tarif);
                lire_champ(ligne, &position, tarif.id_cat_profil);
                lire_champ(ligne, &position, tarif.id_cat_horaire);
                lire_champ(ligne, &position, tarif.profil_tarif);
                lire_champ(ligne, &position, tarif.horaire_tarif);
                lire_champ(ligne, &position, champ);
                tarif.tarif_brut = atoi(champ);
                tab_tarifs[nbtarifs] = tarif;
                nbtarifs++;
            }
        }
        fclose(fentree);
    }

    return nbtarifs;
}

//Preparation d'un dossier d'archive pour ne pas tout mettre sans ordre dans DATA/archive.
void preparer_dossier_archive(char dossier_archive[])
{
    time_t secondes;
    struct tm *date_machine;
    char commande[TAILLE_LIGNE];

    secondes = time(NULL);
    date_machine = localtime(&secondes);

    sprintf(dossier_archive,
            "DATA/archive/archive_%04d%02d%02d_%02d%02d%02d",
            date_machine->tm_year + 1900,
            date_machine->tm_mon + 1,
            date_machine->tm_mday,
            date_machine->tm_hour,
            date_machine->tm_min,
            date_machine->tm_sec);

    // system() est utilise ici pour executer des commandes unix et creer le repertoire.
    sprintf(commande, "mkdir -p %s", dossier_archive);
    system(commande);
}

//Construction du nom d'un fichier archive dans le dossier prepare
void construire_nom_archive_dans_dossier(char nom_archive[], char dossier_archive[], char base[])
{
    sprintf(nom_archive, "%s/%s.txt", dossier_archive, base);
}

void construire_nom_archive(char nom_archive[], char base[])
{
    char dossier_archive[TAILLE_NOM];

    preparer_dossier_archive(dossier_archive);
    construire_nom_archive_dans_dossier(nom_archive, dossier_archive, base);
}

//Sauvegarder les spectacles de la saison
int sauvegarder_spectacles(char nom_fichier[])
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
        fprintf(fsortie, "id_spectacle;nom_spectacle\n");
        for (i = 0 ; i < nbspectacles ; i++)
        {
            fprintf(fsortie, "%s;%s\n",
                    tab_spectacles[i].id_spectacle,
                    tab_spectacles[i].nom_spectacle);
        }
        fclose(fsortie);
    }

    return 1;
}

//Sauvegarde les representations
int sauvegarder_representations(char nom_fichier[])
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
        fprintf(fsortie, "id_representation;id_spectacle;nom_spectacle;jour_representation;heure_representation;horraire_representation;id_cat_representation;date_representation;id_salle;nom_salle\n");

        for (i = 0 ; i < nbrepresentations ; i++)
        {
            fprintf(fsortie, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s\n",
                    tab_representations[i].id_representation,
                    tab_representations[i].id_spectacle,
                    tab_representations[i].nom_spectacle,
                    tab_representations[i].jour_representation,
                    tab_representations[i].heure_representation,
                    tab_representations[i].horaire_representation,
                    tab_representations[i].id_cat_representation,
                    tab_representations[i].date_representation,
                    tab_representations[i].id_salle,
                    tab_representations[i].nom_salle);
        }
        fclose(fsortie);
    }

    return 1;
}



// Archiver de toutes les donnees chargees en memoire.
void archiver_toutes_les_donnees()
{
    FILE *fsortie;
    int i;
    char nom_archive[TAILLE_NOM];
    char dossier_archive[TAILLE_NOM];

    preparer_dossier_archive(dossier_archive);
    construire_nom_archive_dans_dossier(nom_archive, dossier_archive, "00spectacle_25_26");
    sauvegarder_spectacles(nom_archive);

    construire_nom_archive_dans_dossier(nom_archive, dossier_archive, "01info_salle");
    fsortie = fopen(nom_archive, "w");
    if (fsortie != NULL)
    {
        fprintf(fsortie, "id_salle;nom_salle;jauge_total_salle\n");
        for (i = 0 ; i < nbsalles ; i++)
        {
            fprintf(fsortie, "%s;%s;%d\n", tab_salles[i].id_salle,
                    tab_salles[i].nom_salle,
                    tab_salles[i].jauge_total_salle);
        }
        fclose(fsortie);
    }

    construire_nom_archive_dans_dossier(nom_archive, dossier_archive, "02cat_billet_salle");
    fsortie = fopen(nom_archive, "w");
    if (fsortie != NULL)
    {
        fprintf(fsortie, "id_salle;nom_salle;id_categorie_billet;categorie_billet;jauge_categorie_billet\n");
        for (i = 0 ; i < nbcategories ; i++)
        {
            fprintf(fsortie, "%s;%s;%s;%s;%d\n", tab_categories[i].id_salle,
                    tab_categories[i].nom_salle,
                    tab_categories[i].id_categorie_billet,
                    tab_categories[i].categorie_billet,
                    tab_categories[i].jauge_categorie_billet);
        }
        fclose(fsortie);
    }

    construire_nom_archive_dans_dossier(nom_archive, dossier_archive, "03cat_horaire");
    fsortie = fopen(nom_archive, "w");
    if (fsortie != NULL)
    {
        fprintf(fsortie, "libelle_horaire;id_cat_horaire\n");
        for (i = 0 ; i < nbhoraires ; i++)
        {
            fprintf(fsortie, "%s;%s\n", tab_horaires[i].libelle_horaire,
                    tab_horaires[i].id_cat_horaire);
        }
        fclose(fsortie);
    }

    construire_nom_archive_dans_dossier(nom_archive, dossier_archive, "04cat_profil");
    fsortie = fopen(nom_archive, "w");
    if (fsortie != NULL)
    {
        fprintf(fsortie, "id_cat_profil;profil_tarif;type_tarif\n");
        for (i = 0 ; i < nbprofils ; i++)
        {
            fprintf(fsortie, "%s;%s;%s\n", tab_profils[i].id_cat_profil,
                    tab_profils[i].profil_tarif,
                    tab_profils[i].type_tarif);
        }
        fclose(fsortie);
    }

    construire_nom_archive_dans_dossier(nom_archive, dossier_archive, "05representation_25_26");
    sauvegarder_representations(nom_archive);

    construire_nom_archive_dans_dossier(nom_archive, dossier_archive, "06info_tarif");
    fsortie = fopen(nom_archive, "w");
    if (fsortie != NULL)
    {
        fprintf(fsortie, "id_categorie_billet;id_salle;nom_salle;categorie_billet;jauge_categorie_billet;type_tarif;id_cat_profil;id_cat_horaire;profil_tarif;horaire_tarif;tarif_brut\n");
        for (i = 0 ; i < nbtarifs ; i++)
        {
            fprintf(fsortie, "%s;%s;%s;%s;%d;%s;%s;%s;%s;%s;%d\n",
                    tab_tarifs[i].id_categorie_billet,
                    tab_tarifs[i].id_salle,
                    tab_tarifs[i].nom_salle,
                    tab_tarifs[i].categorie_billet,
                    tab_tarifs[i].jauge_categorie_billet,
                    tab_tarifs[i].type_tarif,
                    tab_tarifs[i].id_cat_profil,
                    tab_tarifs[i].id_cat_horaire,
                    tab_tarifs[i].profil_tarif,
                    tab_tarifs[i].horaire_tarif,
                    tab_tarifs[i].tarif_brut);
        }
        fclose(fsortie);
    }

    construire_nom_archive_dans_dossier(nom_archive, dossier_archive, "07comptes");
    sauvegarder_comptes(nom_archive);

    construire_nom_archive_dans_dossier(nom_archive, dossier_archive, "08reservations");
    sauvegarder_reservations(nom_archive);

    construire_nom_archive_dans_dossier(nom_archive, dossier_archive, "09abonnes");
    sauvegarder_abonnes(nom_archive);

    construire_nom_archive_dans_dossier(nom_archive, dossier_archive, "10spectacle_salle");
    fsortie = fopen(nom_archive, "w");
    if (fsortie != NULL)
    {
        fprintf(fsortie, "Archive_non_rechargee_par_le_programme\n");
        /*le fichier 10spectacle_salle ne sert pas exactement au fonctionnement de notre logiciel, 
        tous nos fichiers sont convertis en txt a partir d'un formulaire excel, 
        c'etait au depart d'un tableau croisee mais on l'a pas supprime parce que
        ca peut servir pour verifier l'info lors du test de consultation des spectacles par la salle, 
        */
        fclose(fsortie);
    }

    construire_nom_archive_dans_dossier(nom_archive, dossier_archive, "11cat_abonnes");
    fsortie = fopen(nom_archive, "w");
    if (fsortie != NULL)
    {
        fprintf(fsortie, "id_cat_abonne;nom_cat_abonne\n");
        fprintf(fsortie, "AA;Adulte\n");
        fprintf(fsortie, "AJ;Jeune\n");
        fprintf(fsortie, "AF;Famille\n");
        fclose(fsortie);
    }

    printf("Donnees archivees dans %s.\n", dossier_archive);
}

/* Vider les fichiers de saison apres archivage. 
On garde les comptes car l'administrateur doit rester connecte au logiciel. */
void vider_fichiers_saison()
{
    FILE *fsortie;

    nbspectacles = 0;
    nbsalles = 0;
    nbcategories = 0;
    nbhoraires = 0;
    nbprofils = 0;
    nbrepresentations = 0;
    nbtarifs = 0;
    nbreservations = 0;
    nbabonnes = 0;

    sauvegarder_spectacles("DATA/00spectacle_25_26.txt");

    fsortie = fopen("DATA/01info_salle.txt", "w");
    if (fsortie != NULL)
    {
        fprintf(fsortie, "id_salle;nom_salle;jauge_total_salle\n");
        fclose(fsortie);
    }

    fsortie = fopen("DATA/02cat_billet_salle.txt", "w");
    if (fsortie != NULL)
    {
        fprintf(fsortie, "id_salle;nom_salle;id_categorie_billet;categorie_billet;jauge_categorie_billet\n");
        fclose(fsortie);
    }

    fsortie = fopen("DATA/03cat_horaire.txt", "w");
    if (fsortie != NULL)
    {
        fprintf(fsortie, "libelle_horaire;id_cat_horaire\n");
        fclose(fsortie);
    }

    fsortie = fopen("DATA/04cat_profil.txt", "w");
    if (fsortie != NULL)
    {
        fprintf(fsortie, "id_cat_profil;profil_tarif;type_tarif\n");
        fclose(fsortie);
    }

    sauvegarder_representations("DATA/05representation_25_26.txt");

    fsortie = fopen("DATA/06info_tarif.txt", "w");
    if (fsortie != NULL)
    {
        fprintf(fsortie, "id_categorie_billet;id_salle;nom_salle;categorie_billet;jauge_categorie_billet;type_tarif;id_cat_profil;id_cat_horaire;profil_tarif;horaire_tarif;tarif_brut\n");
        fclose(fsortie);
    }

    sauvegarder_reservations("DATA/08reservations.txt");
    sauvegarder_abonnes("DATA/09abonnes.txt");

    fsortie = fopen("DATA/10spectacle_salle.txt", "w");
    if (fsortie != NULL)
    {
        fprintf(fsortie, "id_spectacle;id_salle\n");
        fclose(fsortie);
    }

    fsortie = fopen("DATA/11cat_abonnes.txt", "w");
    if (fsortie != NULL)
    {
        fprintf(fsortie, "id_cat_abonne;nom_cat_abonne\n");
        fprintf(fsortie, "AA;Adulte\n");
        fprintf(fsortie, "AJ;Jeune\n");
        fprintf(fsortie, "AF;Famille\n");
        fclose(fsortie);
    }

    saison_chargee = 0;
    ecrire_etat_saison(0);
}

/* Sauvegarde des principales donnees modifiables.
Il renouvelle les fichiers sources et cree aussi une archive. */
void sauvegarder_toutes_les_donnees()
{
    if ((nbspectacles <= 0) && (nbrepresentations <= 0) && (nbtarifs <= 0))
    {
        printf("Aucune saison chargee : sauvegarde non effectuee pour eviter d'ecraser les fichiers sources.\n");
    }
    else{
        sauvegarder_spectacles("DATA/00spectacle_25_26.txt");
        sauvegarder_representations("DATA/05representation_25_26.txt");
        sauvegarder_comptes("DATA/07comptes.txt");
        sauvegarder_reservations("DATA/08reservations.txt");
        sauvegarder_abonnes("DATA/09abonnes.txt");

        archiver_toutes_les_donnees();

        printf("Donnees sauvegardees dans les fichiers sources et dans un dossier DATA/archive/.\n");
    }

}

//renouvelle le fichier source + archive pour les spectacles
void sauvegarder_spectacles_source_et_archive()
{
    char nom_archive[TAILLE_NOM];

    sauvegarder_spectacles("DATA/00spectacle_25_26.txt");
    construire_nom_archive(nom_archive, "00spectacle_25_26");
    sauvegarder_spectacles(nom_archive);
    printf("Spectacles sauvegardes dans DATA/00spectacle_25_26.txt et dans %s\n", nom_archive);
}

//renouvelle le fichier source + archive pour les representations
void sauvegarder_representations_source_et_archive()
{
    char nom_archive[TAILLE_NOM];

    sauvegarder_representations("DATA/05representation_25_26.txt");
    construire_nom_archive(nom_archive, "05representation_25_26");
    sauvegarder_representations(nom_archive);
    printf("Representations sauvegardees dans DATA/05representation_25_26.txt et dans %s\n", nom_archive);
}

//renouvelle le fichier source + archive pour les abonnes
void sauvegarder_abonnes_source_et_archive()
{
    char nom_archive[TAILLE_NOM];

    sauvegarder_abonnes("DATA/09abonnes.txt");
    construire_nom_archive(nom_archive, "09abonnes");
    sauvegarder_abonnes(nom_archive);
    printf("Abonnes sauvegardes dans DATA/09abonnes.txt et dans %s\n", nom_archive);
}

//renouvelle le fichier source + archive pour les reservations
void sauvegarder_reservations_source_et_archive()
{
    char nom_archive[TAILLE_NOM];

    sauvegarder_reservations("DATA/08reservations.txt");
    construire_nom_archive(nom_archive, "08reservations");
    sauvegarder_reservations(nom_archive);
    printf("Reservations sauvegardees dans DATA/08reservations.txt et dans %s\n", nom_archive);
}

/* Initialisation d'une nouvelle saison.
Cette action est reservee a l'administrateur par le menu principal.
Avant de supprimer les anciennes donnees, on archive toutes les informations. */
int initialiser_nouvelle_saison()
{
    printf("\nInitialisation de la saison\n");
    printf("Les spectacles, salles, categories, horaires, profils, tarifs, reservations et abonnes seront archives.\n");
    printf("Les fichiers d'archive seront ranges dans DATA/archive/.\n");

    if (! confirmer_action())
    {
        printf("Initialisation annulee.\n");
        return 0;
    }

    archiver_toutes_les_donnees();
    vider_fichiers_saison();

    printf("Ancienne saison initialisee.\n");
    printf("etat de saison mis a 0 dans DATA/etat_saison.txt.\n");
    printf("Les comptes restent disponibles pour continuer a utiliser le logiciel.\n");
    printf("Vous pouvez maintenant placer les nouvelles donnees dans DATA.\n");
    printf("Il faut assurer que les noms des nouveaux fichiers a charger dan DATA sont bons .\n");
    printf("Ensuite, utilisez l'option Charger la saison du menu administrateur.\n");

    return 1;
}