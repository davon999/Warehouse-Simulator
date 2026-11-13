#ifndef DEPOZIT_H
#define DEPOZIT_H

#include <fstream>

// Limite din enuntul temei.
const int MAX_MARFA = 20;
const int MAX_NUME = 32;
const int MAX_POZITII = 99 * 99;

// O marfa pe care o comanzi: nume, timp (dupa cate zile de la comanda ajunge si e
// gata de expediat), cost de incarcare si pret de expediere.
struct Marfa {
    char nume[MAX_NUME];
    int timp;
    int cost;
    int pret;
};

// Starea depozitului: configuratia citita din fisier + starea pozitiilor.
struct Depozit {
    int L;              // depozitul are L x L pozitii de palet
    int zile;           // numarul de zile simulate
    long bani;          // banii curenti
    int n;              // numarul de tipuri de marfa
    Marfa marfa[MAX_MARFA];
    bool isNA[MAX_MARFA];                    // marfa poate fi incarcata dupa orice
    bool dupaMarfa[MAX_MARFA][MAX_MARFA];    // dupaMarfa[a][b] = a poate fi incarcata dupa b

    // starea pozitiilor (index = linie * L + coloana)
    int best[MAX_MARFA]; // vector in care tin cele mai bune marfuri
    bool ocupat[MAX_POZITII];
    int  idxMarfa[MAX_POZITII];   // ce marfa e incarcata
    int  ziIncarcare[MAX_POZITII];
    bool folosit[MAX_POZITII];    // pozitia a fost folosita vreodata
    int  ziUltimeiExpedieri[MAX_POZITII];
    int  idxUltimeiExpedieri[MAX_POZITII];
};

// Citeste configuratia din fisier. Intoarce 1 la succes, 0 la eroare.
int citeste(const char* caleIn, Depozit& d);

// Simuleaza zi cu zi si scrie actiunile + SUMA_FINALA in fisierul de iesire.
// Intoarce 1 la succes, 0 la eroare.
int simuleaza(const char* caleOut, Depozit& d);

#endif
