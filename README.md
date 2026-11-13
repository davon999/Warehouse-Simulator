# Tema 2 – Depozit portuar

# Descriere

Hala unui port are L x L pozitii de palet așezate pe rânduri și coloane, iar fiecare marfă vine pe un palet.
Se porneste cu o suma de bani si trebuie luate decizii in functie de de ce marfa este in depozit si de specificatiile fiecarei marfa in parte,
Pe fiecare palet se pot face doua operatii :
`incarca` - rezerva pozitia de pe palet si marfa este platita
 Marfa ajunge in depozit dupa ceva timp (depinde de fiecare marfa in parte) si mai dureaza o zi pana sa poate fi expediata 
`expediaza` - elibereaza marfa de pe palet si incaseaza banii

Obiectiv: maximizarea `SUMA_FINALA = bani_inițiali + venituri − cheltuieli`

# Cum rulezi

Am folosit make care produce ./app , pt rulare : ./app tests/public/01.in out.txt, apoi validarea cu `python3 judge_public.py tests/public/01.in out.txt1`
Fiecare progres/milestone din punctul meu de vedere l am pushuit pe `git` am folosit `git add .` `git commit -m "mesajul meu"` `git push`

# Cum am organizat programul - Structura codului :

Am creat clasa `Dispecer` pentru a putea realiza operatii cu fiecare marfa din depozit si organiza toate datele

`Dispecer.h` - headerul unde am declarat functiile clasei si campurile acesteia
`Dispecer.cpp` - locul in care am implementat functiile
`depozit.h` - headerul unde sunt declarate structurile Depozit si Marfa fiecare cu campurile sale si functiile de citire si simulare
`depozit.cpp` - aici se face citirea din fisier si se simuleaza zilele
`main.cpp1` - contine dor main si apeleaza functiile programului
`Makefile` - compileaza cu `g++ -std=c++17 -O2 -Wall -o app *.cpp`

# Aici explic fiecare functie folosita in parte 

`int citeste(const char* caleIn, Depozit& d)` - citesc din fisier prima linie mai intai si salvez datele in d care este de tipul Depozit,
apoi citesc n linii reprezentand nr de marfuri si fiecare informatie despre aceasta in parte. Apoi am citit intr un buffer cuvantul `ROTATIE` , deoarece nu
trebuie salvat nicaieri deoarece e inutil urmat de variabila r notata de mine unde am nr de rotatii 
Apoi cele r rotatii unde pt fiecare rotatie am citit numele de marfa in doua charuri `str1` si `str2` pe care le am converit ulterior in pozitia lor din 
vectorul de marfuri cu functia `numeToIndex` creata de mine care returneaaza : pozitia sa din vector , -1 in cazul in care nu exista in vector sau -2 in cazul in care charul este cuvantul `N/A` si este un caz special
Apoi in functie de valorile returnate completez campurile `isNa` si `dupaMarfa`

# O sa continui cu functiile din Dispecer , deoarece in simuleaza m am folosit de ele in mare parte si asa are sens logic sa explic:

`Dispecer(Depozit &d, std::ofstream &g);` - constructorul clasei

`bool verifIncarcare(int p,int m,int zi);` - returneaza `true` in cazul in care marfa respecta conditiile de incarcare 
returneaza `false` in oricare din cazurile in care : pozitia de pe palet este ocupata , am mai putini bani decat costa marfa, nu ma incadrez in timp cu durata ,
daca pozitia paletului n a mai fost ocupata pana acum am doua cazuri : marfa are campul `N/A` o adaug direct sau nu are deci nu poate fi incarcata pe aceasta poz,
daca nu a trecut cooldownul de cand a fost ultima expediere : 1 zi pt marfa diferita sau 3 pt aceeasi marfa, daca nici de tip `N/A` nu e si nici nu poate fi pusa
dupa marfa anterioara

`void incarca(int p,int m,int zi);` - functia simuleaza incarcarea - imi scade suma de bani cu costul marfii, marchez pozitia ca ocupata si salvez datele adica ziua,indexul marfii adica pozitia din vectorul de marfuri si setez campul folosit la `true`
p este pozitia paletelor iar cu /10 aflu linia si cu %10 coloana asa ca am afisat mesajul corespunzator incarcarii

`bool verifExpediere(int p,int m,int zi);` - daca poz nu e ocupata nu am ce expedia -> `false`
daca a trecut deja timpul de asteptare din momentul in care am incarcat o -> `true` e gata de expediat

`void expediaza(int p,int m,int zi);` - functia simuleaza expediere - imi creste suma cu pretul marfii pe care o expediez eliberez pozitia p din `ocupat[p]` salvez ziua si indexul marfii respective si afisez mesajul aferent expedierii

Astea sunt functiile folosite in prima implementare si programul rula scoateam aprox 3 mil suma fara nicio filtrare sau rationament pur si simplu luam fiecare marfa
 o verificam cu verifIncarcare si verifExpediere . Apoi m am gandit sa aleg de fiecare data varianta cea mai buna pe moment adica sa selectez marfa care era cea mai valoroasa din punct de vedere profit/timp asa ca am adaugat in depozit.h campul int best[MAX_MARFA]; unde salvam cele mai bune marfuri sortate descrescator dupa ratio cu ajutorul functiei : 

`void bestMarfa();` - initial le am pus direct marfurile apoi am inceput sa le parcurg si sa le interschimb in functie de ratio cu ajutorul functiei create de mine
`double returnRatio(int idx);` - returneaza profitul fiecarei marfi raportata la (timp+1) deoarece am luat in considerare si cazul de o zi cooldown (cand expediez de pe poz p in ziua z pot incarca ceva de abia in ziua z+1)

 Cu rationamentul acesta de profit/timp am reusit sa scot 9 mil suma , dar tot nu luam cele mai bune cazuri in vizor ci doar alegeam marfa cea mai buna pt momentul respectiv nu si pt viitor. Dau aici un simplu caz :
 am marfa A - ratio 10.33 , marfa B - ratio 8.6 , marfa C - 7.5 dar dupa marfa A nu mai poate fi pus nimic deoarece poate depozitul avea campurile d.isNa[poz lui A]=false si d.dupaMarfa[poz lui C][poz lui B] , iar cu functia mea best() alegeam A cand defapt faceam profit mai mare pe termen mai lng alegand C. Asadar am abordat un rationament de a verifica cu "un pas inainte" si salvand ratio-ul unei perechi cele mai optime in loc de doar ratio-ul unei singure marfi

`int alegereMarfa(int p,int zi);` - parcurg cu un for fiecare marfa in parte , initial daca nu este apta de incarcare (verific cu `verifIncarcare`) sar peste iteratia respectiva,in caz contrar este buna si salvez in ratio initial ratio-ul marfii deoarece momentan asta este ratio-ul perechii (n-am inca o a doua marfa). Apoi continui sa ma uit printre marfuri cu un alt for si verific marfurile ce pot fi puse dupa marfa selectata initial. Setez `cooldown-ul` = 1 in caz de sunt diferite sau = 3 in cazul in care vorbim despre marfa cu nume identic. Iarasi daca nu ma incadrez in timp omit cazul ca sa nu raman cu marfa blocata pe palet , iar daca totul este in regula calculez suma perechii de marfuri cu functia ajutatoare creata de mine

`double sumaPereche(int m1, int m2)` - calculeaza ratio-ul astfel : (profit de pe marfa1 + profit de pe marfa 2) / (timp marfa 1 + timp marfa 2 + cooldown + 1)
cooldown-ul fiind timpul dintre acestea de asteptare si `+1` pt timpul de dupa marfa2

Si retin perechea maxima si indexul marfii din perechea cu maxPairScore si returnez pozitia - `i`

`int simuleaza(const char* caleOut, Depozit& d);` - creez o clasa `disp`, parcurg zilele , apoi parcurg pt fiecare zi in parte fiecare pozitie de palet si verific daca este ceva deja gata de expediere acolo cu functia `verifExpediere` , iar daca este expediez si imi colectez banii. Apoi parcurg pozitiile paletilor selectez marfa cu functia de mai devreme `bestMarfa` care aduce cea mai buna marfa si o incarc pe pozitie

In final afisez suma de bani obtinuta



 # OOP-Warehouse
