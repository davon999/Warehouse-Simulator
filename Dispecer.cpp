#include "Dispecer.h"

Dispecer::Dispecer(Depozit &d, std::ofstream &g) : d(d), g(g)
{
}
bool Dispecer::verifIncarcare(int p, int m, int zi)
{
    if (d.ocupat[p] == true)
    {
        return false; // poz ocupata
    }
    if (d.bani < d.marfa[m].cost)
    {
        return false; // nu am suficienti bani
    }
    if (zi + d.marfa[m].timp > d.zile)
    {
        return false; // nu ma incadrz in timp cu expedierea si se term zilele
    }
    if (d.folosit[p] == false)
    {
        return d.isNA[m];
    }
    if (zi < d.ziUltimeiExpedieri[p] + 1)
    {
        return false; // daca nu a trecut cooldownul pt tip dif de marfa
    }
    if (d.idxUltimeiExpedieri[p] == m)
    {
        if (zi - d.ziUltimeiExpedieri[p] < 3)
        {
            return false; // daca nu a trecut cooldownul pt acelasi tip de marfa
        }
    }
    if (d.isNA[m] == false && d.dupaMarfa[m][d.idxUltimeiExpedieri[p]] == false)
    {
        return false; // daca nu este de tip N/A si nu poate fi pus dupa tipul de marfa ocupat anterior
    }
    return true;
}

void Dispecer::incarca(int p, int m, int zi)
{
    d.bani = d.bani - d.marfa[m].cost;
    d.ocupat[p] = true;
    d.ziIncarcare[p] = zi;
    d.folosit[p] = true;
    d.idxMarfa[p] = m;

    int linie = p / d.L;
    int coloana = p % d.L;

    g << zi << " incarca " << linie / 10 << linie % 10 << "x" << coloana / 10 << coloana % 10 << " " << d.marfa[m].nume << "\n";
}

int Dispecer::alegereMarfa(int p, int zi) // verific cea mai buna succesiune de 2 marfuri
{
    int winner = -1;
    double maxPairScore = -1;
    for (int i = 0; i < d.n; i++)
    {
        if (verifIncarcare(p, i, zi) == false)
        {
            continue;
        }
        double ratio = returnRatio(i); 
        for (int j = 0; j < d.n; j++)
        {
            if (d.dupaMarfa[j][i] == true || d.isNA[j] == true) // poate fii pusa dupa i
            {
                int cooldown = 1;
                if (i == j)
                {
                    cooldown = 3; // i = j si trb cooldown de 3 zile
                }
                if (zi + d.marfa[i].timp + cooldown + d.marfa[j].timp > d.zile) // sa nu raman cu marfa neexpediata ca se termina zilelel
                {
                    continue;
                }
                if (ratio < sumaPereche(i, j))
                {
                    ratio = sumaPereche(i, j); //retin scorul cel mai mare al unei perechi
                }
            }
        }
        if (ratio > maxPairScore)
        {
            maxPairScore = ratio;
            winner = i; // pozitia marfii care este in maxPairScore
        }
    }
    return winner;
}

bool Dispecer::verifExpediere(int p, int m, int zi)
{
    if (d.ocupat[p] == false)
    {
        return false;
    }
    if (d.ziIncarcare[p] + d.marfa[m].timp <= zi)
    {
        return true; // e gata de expediat
    }
    return false;
}

void Dispecer::expediaza(int p, int m, int zi)
{
    d.bani = d.bani + d.marfa[m].pret;
    d.ocupat[p] = false;
    d.ziUltimeiExpedieri[p] = zi;
    d.idxUltimeiExpedieri[p] = m;

    int linie = p / d.L;
    int coloana = p % d.L;

    g << zi << " expediaza " << linie / 10 << linie % 10 << "x" << coloana / 10 << coloana % 10 << "\n";
}

void Dispecer::bestMarfa() // a devenit nefolosita caci am optimizat pe calea cu succesiune de doua marfuri (o foloseam inainte)
{
    int aux = 0;
    for (int i = 0; i < d.n; i++)
    {
        d.best[i] = i;
    }
    for (int i = 0; i < d.n; i++)
    {
        for (int j = i; j < d.n; j++)
        {
            if (returnRatio(d.best[i]) < returnRatio(d.best[j]))
            {
                aux = d.best[i];
                d.best[i] = d.best[j];
                d.best[j] = aux;
            }
        }
    }
}

double Dispecer::returnRatio(int idx)
{
    return (double)(d.marfa[idx].pret - d.marfa[idx].cost) / (d.marfa[idx].timp + 1);
}

double Dispecer::sumaPereche(int m1, int m2)
{
    int cooldown = 1;
    if (m1 == m2)
    {
        cooldown = 3;
    }
    return (double)(d.marfa[m1].pret - d.marfa[m1].cost + d.marfa[m2].pret - d.marfa[m2].cost) / (d.marfa[m1].timp + d.marfa[m2].timp + cooldown + 1);
} 