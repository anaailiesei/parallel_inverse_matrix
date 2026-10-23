# Inversarea unei matrice

### Scurta descriere
Vom calcula inversa unei matrice folosind metoda Gauss–Jordan. Matricele pe care le generam sunt inversabile. Pentru a obtine matrice inversabile, folosim urmatoarele metode de generare (toate construiesc matrice diagonal dominante, deci nenule si, in practica, bine conditionate pentru eliminare): matrice diagonal-dominanta cu elemente aleatorii, matrice tridiagonala (diagonal-dominanta), matrice rara (k-sparse) diagonal-dominanta.

### Echipa proiectului
- Ailiesei Ana-Maria, 341C2
- Olteanu Andreea-Denisa, 341C2

### Numele asistentului
Predescu Ioan-Alexandru

# Saptamana 1

In prima saptamana ne-am definit mai bine proiectul si am inceput implementarea initiala pentru algoritmul de inversare a unei matrici. Am ales sa calculam inversa acesteia folosind metoda Gauss-Jordan, urmand ca aceasta implementare sa fie punct de referinta pentru paralelizarile ce aveau sa urmeze.

## Definirea proiectului si implementarea codului secvential

Am ales limbajul C++ pentru a scrie codul si sa folosim Gauss-Jordan pentru inversarea matricei intrucat aceasta metoda este numeric stabila si reduce riscul impartirilor la zero prin pivotare.

### Cum functioneaza algoritmul Gauss-Jordan

Algoritmul Gauss-Jordan se bazeaza pe transformarea unei matrici augmentate(alaturarea matricei initiale si a matricei identitate) in forma sa esalonata redusa. Procesul este unul iterativ si trateaza fiecare coloana pe rand: se selecteaza pivotul, se normalizeaza linia curenta astfel incat pivotul sa devina 1 si se elimina toate celelalte valori de pe coloana respectiva. La final, in jumatatea stanga obtinem matricea identitate si in jumatatea dreapta obtinem matricea inversa.

## Crearea de module

Pentru a mentine codul algoritmilor cat mai usor de citit, am izolat logica de I/O, validare si testare in module separate: helpers.h si helpers.cpp. Acestea ne permit testarea usoara pentru orice implementare. 

## Crearea unui Makefile

Am creat Makefile-ul care contine regulile de build si clean.

## Generarea fisierelor de test si testarea corectitudinii codului

Am finalizat codul secvential, am adaugat fisiere cu matrici patratice si am verificat(folosind un calculator pentru inversa unei matrici) ca rezultatele obtinute sunt corecte.

## Rezumat

In prima saptamana am stabilit ce avem de facut, am implementat si testat algoritmul secvential, am modularizat codul, si am creat un Makefile.

# Saptamana 2

In aceasta saptamana am inceput implementarea variantelor paralele pentru algoritmul secvential. Metodele folosite au fost: Pthreads(POSIX Threads), OpenMP(Open Multi-Processing) si MPI(Message Passing Interface).

## Discutarea detaliilor

Am analizat codul secvential si am identificat zonele critice care permit paralelizarea:
- Descompunerea datelor: am ales paralelizarea buclelor principale(main loops) responsabile de eliminarea elementelor de pe coloana.
- Independenta operatiilor: deoarece operatiile pe liniile matricei sunt independente intre ele(mai putin linia pivot) putem executa calculele simultan.

## Stabilirea modului de lucru

Pentru a lucra eficient in paralel si a evita conflictele in cod am ales sa folosim branch-uri pentru a lucra la proiect.

## Testarea corectitudinii algoritmilor paraleli

La finalul fiecarei implementari paralele am testat(folosind functii ale modulului helpers) ca rezultatele obtinute sunt la fel ca rezultatele de la algoritmul secvential pentru a asigura corectitudinea algoritmului paralel.

## Rezumat

Saptamana aceasta am transformat algoritmul secvential in trei algoritmi pareleli functionali folosind Pthreads, OpenMP si MPI.

# Saptamanile 3 si 4

Aceste doua saptamani au fost dedicate profiling-ului, compararii celor patru implementari si remedierii posibilelor probleme.

## Schimbarea ierarhiei de fisiere

Pentru a gestiona eficient rularile multipe necesare profiling-ului, am restructurat proiectul dupa cum urmeaza:
- Am izolat fiecare algoritm in propriul director, permitand rularea unor scripturi de rulare specifice(profile.sh);
- Am adaptat Makefile-ul la noua structura si am adaugat flag-uril necesare pentru compilarea corecta pe arhitectura cluster-ului.

## Scrierea script-urilor

Am avut nevoie de urmatoarele script-uri:
- Script pentru fiecare algoritm(profile.sh, aflat in directorul algoritmului respectiv) pentru rularea pe cluster cu sbatch pentru o matrice data;
- Un script care executa secvential intregul set de date de intrare cu profile.sh.

## Ajustarea datelor de intrare

In urma rularii script-urilor am constatat ca dimensiunile matricelor existente sunt prea mici. Pentru a rezolva aceasta problema am implementat un script(generate_mat.py) pentru a genera o matrice de o dimensiune data. Prin testare am observat si ca este necesar de o matrice de peste 300 de elemente pentru a putea analiza cu VTune rezultatele.

## Profiling

Am rulat script-urile pentru 4, 8 si 16 nuclee pe matrici cu dimensiuni de 300, 500, 600, 700 respectiv 800.

## Rezumat

In aceste doua saptamani am creat un mediu de testare automatizat, am generat seturi de date adecvate si am colectat datele necesare pentru analiza finala.

# Analiza Rezultatelor

In aceasta sectiune vom analiza performanta algoritmului de inversare a matricei Gauss-Jordan comparand implementarile paralele(Pthreads, OpenMP si MPI) si cea secventiala. Scopul este de a evidentia diferentele de scalabilitate, overhead si eficienta.

## Detalii de Implementare

- Secvential -> executie pe un singur fir, complexitate O(n^3)
- Pthreads -> paralelizare folosind bariere (pthread_barrier_t) pentru sincronizarea intre etapele de pivotare si eliminare, firele sunt create o singura data pentru a minimiza overhead-ul
- OpenMP -> Paralelizare bazata pe directive (#pragma omp parallel for), modelul utilizat este Fork-Join la nivelul buclelor interne.
- MPI (Message Passing Interface) -> paralelizare prin procese distribuite, matricea este impartita pe randuri, iar linia pivot este transmisa catre toate procesele folosind MPI_Bcast.

## Testare

Testarea s-a facut pe cluster folosind sbatch. Pentru toti algoritmii s-au folosit matrci de dimensiune 300, 500, 600, 700 si 800. In plus, pentru algoritmii paraleli s-a testat pentru fiecare matice executia cu 4, 8 si 16 nuclee.

## Grafice

Urmeaza sa prezentam analiza pentru matricea de 800x800.

### Secvential

![Grafic Secvential](VTunePoze/Secvential.png)
*Figura 1: Grafic VTune Secvential*

### Pthreads

![Grafic Pthreads](VTunePoze/Pthreads4.png)
*Figura 2: Grafic VTune Pthreads cu 4 core-uri*

![Grafic Pthreads](VTunePoze/Pthreads8.png)
*Figura 3: Grafic VTune Pthreads cu 8 core-uri*

![Grafic Pthreads](VTunePoze/Pthreads16.png)
*Figura 4: Grafic VTune Pthreads cu 16 core-uri*

### OpenMP

![Grafic OpenMP](VTunePoze/omp4.png)
*Figura 5: Grafic VTune OpenMP cu 4 core-uri*

![Grafic OpenMP](VTunePoze/omp8.png)
*Figura 6: Grafic VTune OpenMP cu 8 core-uri*

![Grafic OpenMP](VTunePoze/omp16.png)
*Figura 7: Grafic VTune OpenMP cu 16 core-uri*

### MPI

![Grafic MPI](VTunePoze/MPI4.png)
*Figura 8: Grafic VTune MPI cu 4 core-uri*

![Grafic MPI](VTunePoze/MPI_8.png)
*Figura 9: Grafic VTune MPI cu 8 core-uri*

![Grafic MPI](VTunePoze/MPI_16.png)
*Figura 10: Grafic VTune MPI cu 16 core-uri*

### Analiza graficelor

Pentru analiza graficelor am ales sa punem rezultatele doar pentru matricea de dimensiune 800 pentru a nu incarca documentul cu poze, urmand ca mai jos sa avem grafice pentru speed-up unde se vor putea observa timpii pentru toate matricele pentru care am rulat script-ul.

In urma imaginilor de mai sus avem tabelul urmator care arata timpii de executie pentru fiecare algoritm in functie de numarul de nuclee pentru matricea de dimensiune 800.

| Nr Nuclee | Pthreads | OpenMP | MPI   | Secvential |
| --------- | -------- | ------ | ----- | ---------- |
| 1         | -        | -      | -     | 10.77s     |
| 4         | 5.73s    | 7.12s  | 6.01s | -          |
| 8         | 3.53s    | 4.21s  | 4.86s | -          |
| 16        | 2.21s    | 2.61s  | 4.51s | -          |

### Speedup

![Comparatie Speed-up](SpeedupGraphics/Speedup600.png)
*Figura 11*

![Comparatie Speed-up](SpeedupGraphics/Speedup700.png)
*Figura 12*

![Comparatie Speed-up](SpeedupGraphics/Speedup800.png)
*Figura 13*

1. Pthreads
- A stabilit cel mai bun timp initial(5.73s), demonstrand un overhead minim de gestionare.
- Prin dublarea nucleelor la 8, timpul a scazut la 3.53s (Speedup ~1.62x), iar la 16 nuclee a ajuns la 2.21s.

2. OpenMP
- A fost cea mai lenta varianta paralela(7.12s)la rularea cu 4 nuclee din cauza modelului Fork-Join repetat care introduce overhead la fiecare iteratie.
- Scalarea a fost surprinzator de eficienta după pragul de 4 nuclee, recuperand decalajul fata de MPI la 8 nuclee(4.22s) si depasindu-l la 16(2.61s).

3. MPI
- La inceput a fost aproape de Pthreads(6.01s) deoarece overhead-ul de comunicare era inca gestionabil.
- Scalarea s-a plafonat destul de repede, la 8 nuclee a ajuns la un timp de 4.86s (nu foarte departe de OpenMP), iar la 16 procese a ajuns sub timpul algoritnului cu OpenMP(4.51s), iar imbunatatirea de la 8 la 16 procese a fost de doar ~7%.

### Overhead vs. dimensiunea matricii

| Nuclee | Secvential | Pthreads | OpenMP | MPI | Dimensiune |
|---:|---:|---:|---:|---:|---:|
|1| 0.580s | | | | **300 x 300** |
| 4  | | 0.635s | 0.715s | 1.860s |
| 8  | | 0.435s | 0.515s | 1.950s | 
| 16 | | 0.235s | 0.315s | 2.050s |
| | | | | |
|1| 2.610s| | | | **500 x 500** |
| 4  | | 1.710s | 2.029s | 2.711s | 
| 8  | | 1.010s | 1.320s | 2.910s |
| 16 | | 0.710s | 0.920s | 3.110s |
| | | | | |
|1| 7.370s || | | **600 x 600**  |
| 4  | | 2.831s | 3.232s | 3.506s |
| 8  | | 1.710s | 2.091s | 3.244s |
| 16 | | 1.190s | 1.347s | 3.415s |
| | | | | |
|1| 9.010s | | | | **700 x 700**  |
| 4  | | 4.359s | 4.893s | 4.929s |
| 8  | | 2.535s | 2.955s | 4.862s |
| 16 | | 1.647s | 2.000s | 4.510s |
| | | | | |
|1| 10.770s | | | | **800 x 800**  |
| 4  | | 5.731s | 7.126s | 6.019s |
| 8  | | 3.535s | 4.217s | 4.862s |
| 16 | | 2.218s | 2.609s | 4.510s |


![Speedupall](SpeedupGraphics/Speedup_all.png)
*Figura 14*

Pentru matrice mici (ex. 300×300), overhead-ul paralelizarii (creare/scheduling de thread-uri, sincronizari, runtime OpenMP, iar la MPI si pornirea proceselor + comunicarea/broadcast-ul) domina timpul total, astfel incat unele variante paralele ajung mai lente decat rularea secventiala (de ex. Pthreads 4 nuclee: 0.635s vs 0.580s, OpenMP 4 nuclee: 0.715s vs 0.580s, iar MPI este semnificativ mai lent); pe masura ce dimensiunea creste, costul de calcul O(n^3) al Gauss–Jordan devine suficient de mare incat sa amortizeze acest overhead, iar implementari precum Pthreads/OpenMP obtin speedup-uri ridicate (ex. ~6.2× la 600×600 cu 16 nuclee), in timp ce MPI incepe sa fie avantajos abia de la dimensiuni mai mari, dar scaleaza mai slab din cauza overhead-ului de comunicare.

Scaderea speedup-ului pentru dimensiuni mai mari (ex. 700×700, 800×800) este cel mai probabil cauzata de limitari de memorie (working set mai mare decat cache => mai multe cache miss-uri si saturarea bandwidth-ului) si de cresterea costului de sincronizare/coordoare intre thread-uri (iar la MPI, si a comunicarii).


## Concluzii
Analiza cu 4 nuclee arata ca MPI este initial o alternativa viabila pentru Pthreads, dar isi pierde eficienta rapid odata cu cresterea numarului de procese. Prin urmare, Pthreads ramane solutia optima indiferent de numarul de nuclee (4, 8 sau 16), oferind cel mai bun control asupra resurselor.

## Resurse:
* https://www.tutorialspoint.com/finding-inverse-of-a-matrix-using-gauss-jordan-method-in-cplusplus
