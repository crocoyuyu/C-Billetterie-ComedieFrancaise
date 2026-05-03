# Liste des fichiers sources
SOURCE=main.c chargement.c consultation.c admin.c authentification.c reservations.c abonnes.c

# Liste des fichiers objets
OBJETS=main.o chargement.o consultation.o admin.o authentification.o reservations.o abonnes.o

Comédie : $(OBJETS)
	cc -o comedie $(OBJETS)

main.o : main.c declarations.h
	cc -c main.c

chargement.o : chargement.c declarations.h
	cc -c chargement.c

consultation.o : consultation.c declarations.h
	cc -c consultation.c

admin.o : admin.c declarations.h
	cc -c admin.c

authentification.o : authentification.c declarations.h
	cc -c authentification.c

reservations.o : reservations.c declarations.h
	cc -c reservations.c

abonnes.o : abonnes.c declarations.h
	cc -c abonnes.c