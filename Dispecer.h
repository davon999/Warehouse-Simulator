#ifndef DISPECER_H
#define DISPECER_H
#include "depozit.h"
#include <fstream>

class Dispecer
{
private:
    Depozit &d;
    std::ofstream &g;

public:
    Dispecer(Depozit &d, std::ofstream &g);
    bool verifIncarcare(int p,int m,int zi);
    void incarca(int p,int m,int zi);
    int alegereMarfa(int p,int zi);
    void expediaza(int p,int m,int zi);
    bool verifExpediere(int p,int m,int zi);
    double returnRatio(int idx);
    void bestMarfa(); 
    double sumaPereche(int m1, int m2);
};

#endif