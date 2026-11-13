#include "depozit.h"
#include <cstring>
#include <cstdio>
#include "Dispecer.h"

int numetoindex(const char *str, const Depozit &d)
{
    if (strcmp(str, "N/A") == 0)
    {
        return -2; // Nu am pus -1 pt ca mi ar fi confundat cazul cu cel in care e un nume necunoscut asa ca pun -2 ca si cum nu ar conta indexul
    }
    int pozitie = -1;
    for (int i = 0; i < d.n; i++)
    {
        if (strcmp(d.marfa[i].nume, str) == 0)
        {
            pozitie = i;
            break;
        }
    }
    return pozitie;
}

// TODO: citeste configuratia (L, ZILE, BANI, N, marfurile si sectiunea ROTATIE).
int citeste(const char *caleIn, Depozit &d)
{
    std::ifstream f(caleIn);
    if (!f)
    {
        return 0;
    }
    char buffer[32];         // pt citirea cuvantului ROTATIE
    char str1[32], str2[32]; // pt citirea celor 2 nume din ROTATIE
    int r;
    f >> d.L >> d.zile >> d.bani >> d.n;

    for (int i = 0; i < d.n; i++)
    {
        f >> d.marfa[i].nume >> d.marfa[i].timp >> d.marfa[i].cost >> d.marfa[i].pret;
    }

    f >> buffer >> r;

    for (int i = 0; i < r; i++)
    {
        f >> str1 >> str2;
        int nr1 = numetoindex(str1, d); // pastrez indexul numelui din vectorul cu marfuri din depozit cu functie ajutatoare
        int nr2 = numetoindex(str2, d);
        if (nr1 < 0 || nr2 == -1) // sar peste cazul in care primul nume este N/A sau negasit in vectorul de nume sau cazul in care al doilea nume e negasit
        {
            continue;
        }
        if (nr2 == -2) // daca numele 2 este N/A
        {
            d.isNA[nr1] = true;
        }
        else
        {
            d.dupaMarfa[nr1][nr2] = true;
        }
    }

    return 1;
}

// TODO: simuleaza zi cu zi (expediaza marfurile gata de expediat, incarca pozitii libere) si scrie
// actiunile + linia "SUMA_FINALA: <bani>" in fisierul de iesire.
int simuleaza(const char *caleOut, Depozit &d)
{
    std::ofstream g(caleOut);
    if (!g)
    {
        return 0;
    }

    Dispecer disp(d, g);
    disp.bestMarfa(); // nu mai este folosit in implementarea ulterioara

    for (int zi = 1; zi <= d.zile; zi++)
    {

        for (int p = 0; p < d.L * d.L; p++)
        {
            if (disp.verifExpediere(p, d.idxMarfa[p], zi) == true) 
            {
                disp.expediaza(p, d.idxMarfa[p], zi); // expediez
            }
        }
        for (int p = 0; p < d.L * d.L; p++)
        {
            int marfa = disp.alegereMarfa(p, zi);
            if (marfa != -1)
            {
                disp.incarca(p, marfa, zi);
            }
        }
    }

    g << "SUMA_FINALA: " << d.bani;
    return 1;
}
