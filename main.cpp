#include "depozit.h"
#include <iostream>

// Punct de intrare: primeste caile fisierelor si apeleaza functiile din modul.
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Utilizare: ./app <intrare> <iesire>\n";
        return 1;
    }
    static Depozit d;
    if (!citeste(argv[1], d)) {
        std::cerr << "Eroare la citirea fisierului de intrare.\n";
        return 1;
    }
    if (!simuleaza(argv[2], d)) {
        std::cerr << "Eroare la scrierea fisierului de iesire.\n";
        return 1;
    }
    return 0;
}
