#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <ctime>
#include <cmath>
#include <iomanip>
#include "tipos.cpp"
#include "entradas.cpp"

using namespace std;

void testing(int nEJE, int nTQBF, int profundidad, double escalaEJE, double escalaTQBF, int analizar, int pasesLimite, string modo, double& o, int& INF);
void manual(Configuracion i, int pasesLimite, double o);
void alterno(Configuracion i, bool seguirEstrategia, int profundidad1, int profundidad2, int pasesLimite, int heuristicaElegida1, int heuristicaElegida2, PrecalculoHeuristica precalculo, double o, int INF);
void simulacion(Configuracion i, bool seguirEstrategia, int profundidad1, int profundidad2, int pasesLimite, int heuristicaElegida1, int heuristicaElegida2, PrecalculoHeuristica precalculo, double o, int INF);
bool test(Configuracion i, bool seguirEstrategia, int profundidad1, int profundidad2, int pasesLimite, int heuristicaElegida1, int heuristicaElegida2, PrecalculoHeuristica precalculo, unsigned& clk1, unsigned& clk2, double o, int INF);
Accion juega(Configuracion configuracion, bool seguirEstrategia, int profundidad1, int profundidad2, int pasesLimite, int heuristicaElegida1, int heuristicaElegida2, PrecalculoHeuristica precalculo, double o, int INF);
double valoraMin(Configuracion configuracion, int n, double alfa, double beta, bool seguirEstrategia, int pasesLimite, int heuristicaElegida, PrecalculoHeuristica precalculo, double o, int INF);
double valoraMax(Configuracion configuracion, int n, double alfa, double beta, bool seguirEstrategia, int pasesLimite, int heuristicaElegida, PrecalculoHeuristica precalculo, double o, int INF);
int sumatorio(int start, int end);
Configuracion generarRaiz(bool tqbf, string input1, string input2, string input3, string input4, string input5, PrecalculoHeuristica &precalculo, double& o, int& INF);
void prepararHeuristica(Configuracion configuracion, PrecalculoHeuristica &precalculo, double o);
Configuracion transformarEntradaTQBF(string input1, string input2, double& o, int& INF);
Configuracion entradaEJE(string input1, string input2, string input3, string input4, string input5, double& o, int& INF);

// Número de heurísticas y profundidad máxima que puede ser especificada. Cuando se ejecuta con profundidad ilimitada (valor de profundidad en el comando menor que 0), ese límite se ignora. Si se quiere especificar una profundidad máxima determinada mayor que este número, hay que modificarlo previamente.
const int NumHeuristicas = 5;
const int MaxProfundidad = 10;

/*
Ejemplos

True:
a.exe estrategia:y "tqbf:x1 x2:(x1 ^ x2) v (!x2)"
a.exe estrategia:y "tqbf:x1 x2 x3 x4 x5 x6:(x1 ^ x2) v (x3 ^ x4) v (x5 ^ x6) v (x1 ^ !x2 ^ !x4 ^ !x6)"
a.exe "eje:(a, 1, 2) (b, 1.5, 2)(c,2,0.5):(0.5, a, c) (0.5, b, c):1:1.5:3"

False:
a.exe estrategia:y "tqbf:x1 x2 x3 x4 x5 x6:(x1 ^ x2) v (x3 ^ x4) v (x5 ^ x6)"
a.exe estrategia:y "tqbf:x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10:(x6 ^ !x9 ^ x1) v (x1 ^ !x3 ^ !x5 ^ x2 ^ !x4 ^ !x6) v (x1 ^ x10 ^ x1 ^ !x9 ^ x8) v (!x6 ^ x5) v (x3 ^ x4)"
a.exe "eje:(a, 1, 2) (b, 1.5, 2.5)(c,2,0.5):(3, a, b) (1, b, c):1:1.5:3"

Testing:
a.exe pr:6 esc:14:14 test:50:50
a.exe pr:3 esc:20:20 mod:e an:5 test:50:50

Caso de estudio:
a.exe pa:10 he:5 pr:5 "eje:(m1,400,32.599837) (m2,390,31.7848411) (m3,200,16.2999185) (m4,150,12.2249389) (m5,145,11.8174409) (m6,39,3.17848411) (m7,37,3.01548492)(mo1,360,29.3398533) (mo2,190,15.4849226) (mo3,140,11.409943) (mo4,100,8.14995925) (mo5,25,2.03748981)(g1,310,25.2648737) (g2,200,16.2999185) (g3,160,13.0399348) (g4,110,8.96495518) (g5,90,7.33496333)(a1,220,17.9299104) (a2,180,14.6699267) (a3,120,9.7799511)(l1,250,20.3748981) (l2,150,12.2249389) (l3,110,8.96495518):(136.1,m1,m2,m3,m4,m5,m6,m7)(80.5,mo1,mo2,mo3,mo4,mo5)(86,g1,g2,g3,g4,g5)(52,a1,a2,a3)(51,l1,l2,l3):500:500:250"
*/
int main(int argc, char* argv[]) {
    double o;
    int INF;

    bool seguirEstrategia;
    int profundidad1, profundidad2, pasesLimite, heuristicaElegida1, heuristicaElegida2;
    string argEstrategia = "n", argModo = "s", argHeuristicaElegida1 = "5", argHeuristicaElegida2 = "5", argProfundidad1 = "0", argProfundidad2 = "0", argPases = "2", argIn1, argIn2, argIn3, argIn4, argIn5, entrada, argEscalaEJE = "10", argEscalaTQBF = "10", argAnalizar = "0";   // valores por defecto
    string strEstrategia = "estrategia", strModo = "modo", strHeuristicaElegida = "heuristica", strProfundidad = "profundidad", strPases = "pases", strEje = "eje", strTqbf = "tqbf", strTest = "testing", strMaxProfundidad = "maxprofundidad", strEscala = "escala", strAnalizar = "analizar";
    PrecalculoHeuristica precalculo;
    // Leer el comando de los argumentos del programa
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            string arg = argv[i];
            stringstream ss(arg);
            string key, value;
            getline(ss, key, ':');
            getline(ss, value);
            if (strEstrategia.compare(0, key.length(), key) == 0) {
                argEstrategia = value;
            }
            else if (strModo.compare(0, key.length(), key) == 0) {
                argModo = value;
            }
            else if (strHeuristicaElegida.compare(0, key.length(), key) == 0) {
                stringstream ss(value);
                getline(ss, argHeuristicaElegida1, ':');
                getline(ss, argHeuristicaElegida2);
            }
            else if (strProfundidad.compare(0, key.length(), key) == 0) {
                if (count(value.begin(), value.end(), ':') == 2) {
                    stringstream ss(value);
                    getline(ss, argProfundidad1, ':');
                    getline(ss, argProfundidad2);
                }
                else {
                    argProfundidad1 = argProfundidad2 = value;
                }
            }
            else if (strPases.compare(0, key.length(), key) == 0) {
                argPases = value;
            }
            else if (strEje.compare(0, key.length(), key) == 0) {
                entrada = "eje";
                stringstream ss(value);
                getline(ss, argIn1, ':');
                getline(ss, argIn2, ':');
                getline(ss, argIn3, ':');
                getline(ss, argIn4, ':');
                getline(ss, argIn5);
            }
            else if (strTqbf.compare(0, key.length(), key) == 0) {
                entrada = "tqbf";
                stringstream ss(value);
                getline(ss, argIn1, ':');
                getline(ss, argIn2);
            }

            else if (strTest.compare(0, key.length(), key) == 0) {
                entrada = "t";
                stringstream ss(value);
                getline(ss, argIn1, ':');
                getline(ss, argIn2);
            }
            else if (strEscala.compare(0, key.length(), key) == 0) {
                stringstream ss(value);
                getline(ss, argEscalaEJE, ':');
                getline(ss, argEscalaTQBF);
            }
            else if (strAnalizar.compare(0, key.length(), key) == 0) {
                argAnalizar = value;
            }
        }
    }

    // Leer el comando desde la consola
    else {
        cout << "Escribe \"y\" para analizar unicamente acciones que siguen la estrategia o \"n\" para tener en cuenta todas" << endl;
        cout << "* Estrategia: ";
        getline(cin, argEstrategia);
        cout << "\nEscribe \"s\" para realizar una simulacion automatica, \"m\" para el modo manual y \"a\" para alternar turnos automaticos y manuales" << endl;
        cout << "* Modo (s/m/a): ";
        getline(cin, argModo);
        cout << "\nEscribe un numero para determinar que considera la heuristica elegida del jugador 1\n1: La ganancia del primer jugador\n2: La ganancia de ambos jugadores\n3: La ganancia y la ganancia esperada del primer jugador\n4: La ganancia y la ganancia esperada de ambos jugadores" << endl;
        cout << "* Heuristica J1 (1-" << NumHeuristicas << "): ";
        getline(cin, argHeuristicaElegida1);
        cout << "\nEscribe un numero para determinar que considera la heuristica elegida del jugador 2" << endl;
        cout << "* Heuristica J2 (1-" << NumHeuristicas << "): ";
        getline(cin, argHeuristicaElegida2);
        cout << "\nEscribe un numero entero para determinar la profundidad maxima del arbol de minimax del jugador 1\nLos numeros menores que 1 indican una profundidad ilimitada\nA partir de esa profundidad, se usara una heuristica" << endl;
        cout << "* Profundidad J1: ";
        getline(cin, argProfundidad1);
        cout << "\nEscribe un numero entero para determinar la profundidad maxima del arbol de minimax del jugador 2" << endl;
        cout << "* Profundidad J2: ";
        getline(cin, argProfundidad2);
        cout << "\nEscribe un numero natural\nSi un jugador pasa ese numero de turnos de forma consecutiva, perdera la partida" << endl;
        cout << "* Pases: ";
        getline(cin, argPases);

        cout << "\nEscribe \"tqbf\" si quieres transformar una entrada de TQBF y otro valor si quieres usar directamente una entrada de EJE\n";
        cout << "* EJE/TQBF: ";
        getline(cin, entrada);

        if (entrada != "tqbf") {
            cout << "\nEscribe los terrenos\nPor ejemplo, los dos terrenos \"(id1, 1.2, 1.8) (id2,1,2)\", donde id e id2 son sus identificadores, 1.2 y 1 sus costes y 1.8 y 2 sus ganancias" << endl; 
            cout << "* T: ";
            getline(cin, argIn1);
            cout << "\nEscribe las relaciones\nPor ejemplo, las dos relaciones \"(1, id1, id2) (2.5, id1, id2)\", donde 1 y 2.5 son sus ganancias y ambas contienen los terrenos con identificadores \"id1\" e \"id2\"" << endl; 
            cout << "* M: ";
            getline(cin, argIn2);
            cout << "\nEscribe el dinero inicial del primer jugador" << endl;
            cout << "* dini1: ";
            getline(cin, argIn3);
            cout << "\nEscribe el dinero inicial del segundo jugador" << endl;
            cout << "* dini2: ";
            getline(cin, argIn4);
            cout << "\nEscribe el valor de ganancia objetivo" << endl;
            cout << "* o: ";
            getline(cin, argIn5);
        }
        else {
            cout << "\nEscribe el prefijo (las variables), alternando entre variables cuantificadas existencialmente y universalmente\nPor ejemplo, \"x1 x2 x3\", donde x1 y x3 estan cuanficadas existencialmente y x2 universalmente" << endl; 
            cout << "* Prefijo: ";
            getline(cin, argIn1);
            cout << "\nEscribe la matriz (una disyuncion de conjunciones), usando las variables, parentesis, disyuncion lógica (\"v\") y conjuncion logica (\"^\")\nPor ejemplo, \"(x1 ^ x2) v (x3 ^ x4) v (x5 ^ x6) v (x1 ^ !x2 ^ !x4 ^ !x6)\"" << endl;
            cout << "* Matriz: ";
            getline(cin, argIn2);
        }
    }

    seguirEstrategia = (argEstrategia == "y");
    profundidad1 = stoi(argProfundidad1);
    profundidad2 = stoi(argProfundidad2);
    pasesLimite = stoi(argPases);
    heuristicaElegida1 = stoi(argHeuristicaElegida1);
    heuristicaElegida2 = stoi(argHeuristicaElegida2);
    if (entrada == "t")
        testing(stoi(argIn1), stoi(argIn2), stoi(argProfundidad1), stod(argEscalaEJE), stod(argEscalaTQBF), stoi(argAnalizar), pasesLimite, argModo, o, INF);
    else {
        Configuracion raiz = generarRaiz(entrada == "tqbf", argIn1, argIn2, argIn3, argIn4, argIn5, precalculo, o, INF);
        if (argModo == "m")
            manual(raiz, pasesLimite, o);
        else if (argModo == "s")
            simulacion(raiz, seguirEstrategia, profundidad1, profundidad2, pasesLimite, heuristicaElegida1, heuristicaElegida2, precalculo, o, INF);
        else if (argModo == "a")
            alterno(raiz, seguirEstrategia, profundidad1, profundidad2, pasesLimite, heuristicaElegida1, heuristicaElegida2, precalculo, o, INF);
    }

    return 0;
}

// genera instancias de EJE y TQBF usando entradas.cpp y las ejecuta de la forma especificada (testing por enfrentamientos o testing por comparación con la solución óptima), guardando información útil para comparar los parámetros del algoritmo en out.cpp.
void testing(int nEJE, int nTQBF, int profundidad, double escalaEJE, double escalaTQBF, int analizar, int pasesLimite, string modo, double& o, int& INF) {
    bool seguirEstrategia;
    int heuristicaElegida1;
    int heuristicaElegida2;

    if (profundidad == 0)
        profundidad = 3;

    PrecalculoHeuristica precalculo;
    string argIn1, argIn2, argIn3, argIn4, argIn5, entrada;
    entradas(nEJE, nTQBF, escalaEJE, escalaTQBF);

    ifstream in("entradas.txt");
    ofstream out("out.txt");
    ofstream latex("latex.txt");
    string line, key, value;
    unsigned clks[2][MaxProfundidad+1][NumHeuristicas + 1] = {0};           // [eje/tqbf] [profundidad] [heurística]
    int victorias[2][MaxProfundidad+1][NumHeuristicas + 1] = {0};           // [eje/tqbf] [profundidad] [heurística]
    int victoriasContra[2][NumHeuristicas + 1][NumHeuristicas + 1] = {0};   // [eje/tqbf] [heurística analizada] [heurística opuesta]

    double aciertos[2][MaxProfundidad+1][NumHeuristicas + 1][2] = {0};      // [eje/tqbf] [profundidad] [heurística] [true/false]
    int numTrue[2] = {0};                                                   // [eje/tqbf]

    int numProcesados = 0;
    int progresoTotal1 = 0, progresoTotal2 = 0;
    double progreso = 0, ultimoProgreso = 0;
    int k1 = 0, k2 = 0;
    for (int p1 = 1; p1 <= profundidad; p1++) {
        k1 += pow(2,p1 - 1);
        for (int p2 = 1; p2 <= profundidad; p2++) {
            k2 += pow(2,p2 - 1);
            progresoTotal1 += (k1+k2)*NumHeuristicas*NumHeuristicas*(nEJE+nTQBF);
        }
        progresoTotal2 += k1*NumHeuristicas*(nEJE+nTQBF);
    }

    system("cls");
    cout << setprecision(2) << 0 << " % (0/" << nEJE + nTQBF << ")" << endl;
    while (getline(in, line)) {
        stringstream ss(line);
        getline(ss, key, ':');
        getline(ss, value);

        if (key == "eje") {
            seguirEstrategia = false;
            entrada = "eje";
            stringstream ss(value);
            getline(ss, argIn1, ':');
            getline(ss, argIn2, ':');
            getline(ss, argIn3, ':');
            getline(ss, argIn4, ':');
            getline(ss, argIn5);
        }
        else if (key == "tqbf") {
            seguirEstrategia = true;
            entrada = "tqbf";
            stringstream ss(value);
            getline(ss, argIn1, ':');
            getline(ss, argIn2);
        }
        
        Configuracion raiz = generarRaiz(entrada == "tqbf", argIn1, argIn2, argIn3, argIn4, argIn5, precalculo, o, INF);

        unsigned clk1, clk2;
        k1 = k2 = 0;

        // TESTING POR ENFRENTAMIENTOS

        if (modo == "e") {
            for (int p1 = 1; p1 <= profundidad; p1++) {
                    k1 += pow(2,p1 - 1);
                for (int p2 = 1; p2 <= profundidad; p2++) {
                    k2 += pow(2,p2 - 1);
                    for (int h1 = 1; h1 <= NumHeuristicas; h1++) {
                        for (int h2 = 1; h2 <= NumHeuristicas; h2++) {
                            bool resultado = test(raiz, seguirEstrategia, p1, p2, pasesLimite, h1, h2, precalculo, clk1, clk2, o, INF);
                            
                            clks[entrada == "eje" ? 0 : 1][p1][h1] += clk1;
                            clks[entrada == "eje" ? 0 : 1][p2][h2] += clk2;
                            victorias[entrada == "eje" ? 0 : 1][resultado ? p1 : p2][resultado ? h1 : h2] += 1;
                            victoriasContra[entrada == "eje" ? 0 : 1][resultado ? h1 : h2][resultado ? h2 : h1] += 1;

                            progreso += double(k1+k2)/progresoTotal1 + 1;
                            if (fabs(progreso - ultimoProgreso) >= 0.01) {
                                ultimoProgreso = progreso;
                                system("cls");
                                cout << fixed << setprecision(2) << 100*progreso << "% (" << numProcesados << "/" << nEJE + nTQBF << ")" << endl;
                            }
                        }
                    }
                }

            }
        }

        // TESTING POR COMPARACIÓN CON LA SOLUCIÓN ÓPTIMA

        else {
            bool resultadoOptimo = test(raiz, seguirEstrategia, 0, 0, pasesLimite, 0, 0, precalculo, clk1, clk2, o, INF);
            clks[entrada == "eje" ? 0 : 1][0][0] += clk1 + clk2;

            if (resultadoOptimo)
                numTrue[entrada == "eje" ? 0 : 1] += 1;

            for (int p = 1; p <= profundidad; p++) {
                k1 += pow(2,p - 1);
                for (int h = 1; h <= NumHeuristicas; h++) {
                    
                    bool resultadoHeuristico = test(raiz, seguirEstrategia, p, p, pasesLimite, h, h, precalculo, clk1, clk2, o, INF);
                    clks[entrada == "eje" ? 0 : 1][p][h] += clk1 + clk2;

                    if (resultadoHeuristico == resultadoOptimo)
                        aciertos[entrada == "eje" ? 0 : 1][p][h][resultadoOptimo ? 0 : 1] += 1;
                    
                    progreso += double(k1)/progresoTotal2;
                    if (fabs(progreso - ultimoProgreso) >= 0.01/100) {
                        ultimoProgreso = progreso;
                        system("cls");
                        cout << fixed << setprecision(2) << 100*progreso << "% (" << numProcesados << "/" << nEJE + nTQBF << ")" << endl;
                    }
                }
            }
        }
        
        numProcesados++;
        system("cls");
        cout << fixed << setprecision(2) << 100*progreso << "% (" << numProcesados << "/" << nEJE + nTQBF << ")" << endl;
    }
    system("cls");
    
    // SALIDA TESTING POR ENFRENTAMIENTOS

    if (modo == "e") {

        out << nEJE + nTQBF << " instancias" << endl;
        out << nEJE << " de EJE (de " << escalaEJE << " terrenos)" << endl;
        out << nTQBF << " de TQBF (de " << escalaTQBF << " variables, cuyas instancias de EJE asociadas tienen " << escalaTQBF*3+6 << " terrenos)" << endl;

        latex << "\\noindent " << nEJE + nTQBF << " instancias" << endl << endl;
        latex << "\\noindent " << nEJE << " de EJE (de " << escalaEJE << " terrenos)" << endl << endl;
        latex << "\\noindent "<< nTQBF << " de TQBF (de " << escalaTQBF << " variables, cuyas instancias de EJE asociadas tienen " << escalaTQBF*3+6 << " terrenos)" << endl << endl;
    
        // OUT

        int width = 20;

        // Tabla Total
        out << "Total" << endl;
        out << setw(width) << "Profundidad" << setw(width) << "Heurística" << setw(width) << "Tiempo" << setw(width) << "Victorias" << endl;
        for (int p = 1; p <= profundidad; p++) {
            for (int h = 1; h <= NumHeuristicas; h++) {
                out << fixed << setprecision(2) << setw(width) << p << setw(width) << h << setw(width) << (double(clks[0][p][h] + clks[1][p][h])/CLOCKS_PER_SEC) << "s" << setw(width) << 100*(double(victorias[0][p][h] + victorias[1][p][h]))/((nEJE + nTQBF)*profundidad*NumHeuristicas*2) << "%" << endl;
            }
        }

        // Tabla EJE
        out << "EJE" << endl;
        out << setw(width) << "Profundidad" << setw(width) << "Heurística" << setw(width) << "Tiempo" << setw(width) << "Victorias" << endl;
        for (int p = 1; p <= profundidad; p++) {
            for (int h = 1; h <= NumHeuristicas; h++) {
                out << fixed << setprecision(2) << setw(width) << p << setw(width) << h << setw(width) << (double(clks[0][p][h])/CLOCKS_PER_SEC) << "s" << setw(width) << 100*(double(victorias[0][p][h]))/(nEJE*profundidad*NumHeuristicas*2) << "%" << endl;
            }
        }

        // Tabla TQBF
        out << "TQBF" << endl;
        out << setw(width) << "Profundidad" << setw(width) << "Heurística" << setw(width) << "Tiempo" << setw(width) << "Victorias" << endl;
        for (int p = 1; p <= profundidad; p++) {
            for (int h = 1; h <= NumHeuristicas; h++) {
                out << fixed << setprecision(2) << setw(width) << p << setw(width) << h << setw(width) << (double(clks[1][p][h])/CLOCKS_PER_SEC) << "s" << setw(width) << 100*(double(victorias[1][p][h]))/(nEJE*profundidad*NumHeuristicas*2) << "%" << endl;
            }
        }

        // Tabla heurística analizada
        if (analizar != 0) {
            out << "Heurística " << analizar << endl;
            out << setw(width) << "Heurística opuesta" << setw(width) << "Victorias" << endl;
            for (int h = 1; h <= NumHeuristicas; h++) {
                out << fixed << setprecision(2) << setw(width) << h << setw(width) << 100*(double(victoriasContra[0][analizar][h] + victoriasContra[1][analizar][h])) / ((nEJE+nTQBF)*profundidad*profundidad*2) << "%" << endl;
            }
        }

        // LATEX

        // Tabla Total
        latex << "\\begin{table}[H]" << endl;
        latex << "\\centering" << endl;
        latex << "\\begin{tabular}{|c|c|c|c|}" << endl;
        latex << "\\hline" << endl;
        latex << "\\multicolumn{4}{|c|}{Total} \\\\" << endl;
        latex << "\\hline" << endl;
        latex << "Profundidad & Heurística & Tiempo & Victorias \\\\" << endl;
        for (int p = 1; p <= profundidad; p++) {
            for (int h = 1; h <= NumHeuristicas; h++) {
                latex << "\\hline" << endl;
                latex << fixed << setprecision(2) << p << " & " << h << " & " << (double(clks[0][p][h] + clks[1][p][h])/CLOCKS_PER_SEC) << "s & " << 100*(victorias[0][p][h] + victorias[1][p][h])/((nEJE + nTQBF)*profundidad*NumHeuristicas*2) << "\\% \\\\" << endl;
            }
        }
        latex << "\\hline" << endl;
        latex << "\\end{tabular}" << endl;
        latex << "\\end{table}" << endl;

        // Tabla EJE
        latex << "\\begin{table}[H]" << endl;
        latex << "\\centering" << endl;
        latex << "\\begin{tabular}{|c|c|c|c|}" << endl;
        latex << "\\hline" << endl;
        latex << "\\multicolumn{4}{|c|}{EJE} \\\\" << endl;
        latex << "\\hline" << endl;
        latex << "Profundidad & Heurística & Tiempo & Victorias \\\\" << endl;
        for (int p = 1; p <= profundidad; p++) {
            for (int h = 1; h <= NumHeuristicas; h++) {
                latex << "\\hline" << endl;
                latex << fixed << setprecision(2) << p << " & " << h << " & " << (double(clks[0][p][h])/CLOCKS_PER_SEC) << "s & " << 100*victorias[0][p][h]/(nEJE*profundidad*NumHeuristicas*2) << "\\% \\\\" << endl;
            }
        }
        latex << "\\hline" << endl;
        latex << "\\end{tabular}" << endl;
        latex << "\\end{table}" << endl;

        // Tabla TQBF
        latex << "\\begin{table}[H]" << endl;
        latex << "\\centering" << endl;
        latex << "\\begin{tabular}{|c|c|c|c|}" << endl;
        latex << "\\hline" << endl;
        latex << "\\multicolumn{4}{|c|}{TQBF} \\\\" << endl;
        latex << "\\hline" << endl;
        latex << "Profundidad & Heurística & Tiempo & Victorias \\\\" << endl;
        for (int p = 1; p <= profundidad; p++) {
            for (int h = 1; h <= NumHeuristicas; h++) {
                latex << "\\hline" << endl;
                latex << fixed << setprecision(2) << p << " & " << h << " & " << (double(clks[1][p][h])/CLOCKS_PER_SEC) << "s & " << 100*victorias[1][p][h]/(nEJE*profundidad*NumHeuristicas*2) << "\\% \\\\" << endl;
            }
        }
        latex << "\\hline" << endl;
        latex << "\\end{tabular}" << endl;
        latex << "\\end{table}" << endl;

        // Tabla heurística analizada
        if (analizar != 0) {
            latex << "\\begin{table}[H]" << endl;
            latex << "\\centering" << endl;
            latex << "\\begin{tabular}{|c|c|}" << endl;
            latex << "\\hline" << endl;
            latex << "\\multicolumn{2}{|c|}{Heurística " << analizar << "} \\\\" << endl;
            latex << "\\hline" << endl;
            latex << "Heurística opuesta & Victorias \\\\" << endl;
            for (int h = 1; h <= NumHeuristicas; h++) {
                latex << "\\hline" << endl;
                latex << fixed << setprecision(2) << h << " & " << setw(width) << 100*(double(victoriasContra[0][analizar][h] + victoriasContra[1][analizar][h])) / ((nEJE+nTQBF)*profundidad*profundidad*2) << "\\% \\\\" << endl;
            }
            latex << "\\hline" << endl;
            latex << "\\end{tabular}" << endl;
            latex << "\\end{table}" << endl;
        }
    }

    // SALIDA TESTING POR COMPARACIÓN CON LA SOLUCIÓN ÓPTIMA

    else {

        out << nEJE + nTQBF << " instancias" << endl;
        out << "\t" << nEJE << " de EJE (de " << escalaEJE << " terrenos)" << "\n\t\tTrue: " << numTrue[0] << "\n\t\tFalse: " << nEJE - numTrue[0] << endl;
        out << "\t" << nTQBF << " de TQBF (de " << escalaTQBF << " variables, cuyas instancias de EJE asociadas tienen " << escalaTQBF*3+6 << " terrenos)" << "\n\t\tTrue: " << numTrue[1] << "\n\t\tFalse: " << nTQBF - numTrue[1] << endl;

        latex << "\\noindent " << nEJE + nTQBF << " instancias" << endl << endl;
        latex << "\\noindent " << nEJE << " de EJE (de " << escalaEJE << " terrenos)" << "\n$\\top$: " << numTrue[0] << "\n$\\bot$: " << nEJE - numTrue[0] << endl << endl;
        latex << "\\noindent " << nTQBF << " de TQBF (de " << escalaTQBF << " variables, cuyas instancias de EJE asociadas tienen " << escalaTQBF*3+6 << " terrenos)" << "\n$\\top$: " << numTrue[1] << "\n$\\bot$: " << nTQBF - numTrue[1] << endl << endl;

        // OUT

        out << "\nResolucion optima: " << endl;
        out << "\tTiempo: " << (double(clks[0][0][0] + clks[1][0][0])/CLOCKS_PER_SEC) << "s (" << double(clks[0][0][0])/CLOCKS_PER_SEC << "s para instancias de eje, " << double(clks[1][0][0])/CLOCKS_PER_SEC << "s para instancias de TQBF)\n";

        out << "\nTotal:\n";
        for (int p = 1; p <= profundidad; p++) {
            out << "\n\tProfundidad " << p << endl;
            for (int i = 1; i <= NumHeuristicas; i++) {
                out << "\t\tHeuristica " << i << ":" << endl;
                out << "\t\t\tTiempo: " << (double(clks[0][p][i] + clks[1][p][i])/CLOCKS_PER_SEC) << "s\n";
                out << "\t\t\t\% aciertos: " << 100*(aciertos[0][p][i][0] + aciertos[0][p][i][1] + aciertos[1][p][i][0] + aciertos[1][p][i][1])/(nEJE + nTQBF) << "\%" <<  endl;
            }
        }
        out << "\nEJE:\n";
        for (int p = 1; p <= profundidad; p++) {
            out << "\n\tProfundidad " << p << endl;
            for (int i = 1; i <= NumHeuristicas; i++) {
                out << "\t\tHeuristica " << i << ":" << endl;
                out << "\t\t\tTiempo: " << (double(clks[0][p][i])/CLOCKS_PER_SEC) << "s\n";
                out << "\t\t\t\% aciertos: " << 100*(aciertos[0][p][i][0] + aciertos[0][p][i][1])/nEJE << "\%" << " (true: " << 100*(aciertos[0][p][i][0])/numTrue[0] << "\%, false: " << 100*(aciertos[0][p][i][1])/(nEJE-numTrue[0]) << "\%)" <<  endl;
            }
        }
        out << "\nTQBF:\n";
        for (int p = 1; p <= profundidad; p++) {
            out << "\n\tProfundidad " << p << endl;
            for (int i = 1; i <= NumHeuristicas; i++) {
                out << "\t\tHeuristica " << i << ":" << endl;
                out << "\t\t\tTiempo: " << (double(clks[1][p][i])/CLOCKS_PER_SEC) << "s\n";
                out << "\t\t\t\% aciertos: " << 100*(aciertos[1][p][i][0] + aciertos[1][p][i][1])/nTQBF << "\%" << " (true: " << 100*(aciertos[1][p][i][0])/numTrue[1] << "\%, false: " << 100*(aciertos[1][p][i][1])/(nTQBF-numTrue[1]) << "\%)" <<  endl;
            }
        }

        // LATEX

        // Tabla Total
        latex << "\\begin{table}[H]" << endl;
        latex << "\\centering" << endl;
        latex << "\\begin{tabular}{|c|c|c|c|}" << endl;
        latex << "\\hline" << endl;
        latex << "\\multicolumn{4}{|c|}{Total} \\\\" << endl;
        latex << "\\hline" << endl;
        latex << "Profundidad & Heurística & Tiempo & Aciertos \\\\" << endl;
        for (int p = 1; p <= profundidad; p++) {
            for (int i = 1; i <= NumHeuristicas; i++) {
                latex << "\\hline" << endl;
                latex << p << " & " << i << " & " << (double(clks[0][p][i] + clks[1][p][i])/CLOCKS_PER_SEC) << "s & " << 100*(aciertos[0][p][i][0] + aciertos[0][p][i][1] + aciertos[1][p][i][0] + aciertos[1][p][i][1])/(nEJE + nTQBF) << "\\% \\\\" << endl;
            }
        }

        latex << "\\hline" << endl;
        latex << "\\end{tabular}" << endl;
        latex << "\\end{table}" << endl;

        // Tabla EJE
        latex << "\\begin{table}[H]" << endl;
        latex << "\\centering" << endl;
        latex << "\\begin{tabular}{|c|c|c|c|}" << endl;
        latex << "\\hline" << endl;
        latex << "\\multicolumn{4}{|c|}{EJE} \\\\" << endl;
        latex << "\\hline" << endl;
        latex << "Profundidad & Heurística & Tiempo & Aciertos \\\\" << endl;
        for (int p = 1; p <= profundidad; p++) {
            for (int i = 1; i <= NumHeuristicas; i++) {
                latex << "\\hline" << endl;
                latex << p << " & " << i << " & " << (double(clks[0][p][i])/CLOCKS_PER_SEC) << "s & " << 100*(aciertos[0][p][i][0] + aciertos[0][p][i][1])/nEJE << "\\% \\\\" << endl;
            }
        }

        latex << "\\hline" << endl;
        latex << "\\end{tabular}" << endl;
        latex << "\\end{table}" << endl;

        // Tabla TQBF
        latex << "\\begin{table}[H]" << endl;
        latex << "\\centering" << endl;
        latex << "\\begin{tabular}{|c|c|c|c|}" << endl;
        latex << "\\hline" << endl;
        latex << "\\multicolumn{4}{|c|}{TQBF} \\\\" << endl;
        latex << "\\hline" << endl;
        latex << "Profundidad & Heurística & Tiempo & Aciertos \\\\" << endl;
        for (int p = 1; p <= profundidad; p++) {
            for (int i = 1; i <= NumHeuristicas; i++) {
                latex << "\\hline" << endl;
                latex << p << " & " << i << " & " << (double(clks[1][p][i])/CLOCKS_PER_SEC) << "s & " << 100*(aciertos[1][p][i][0] + aciertos[1][p][i][1])/nTQBF << "\\% \\\\" << endl;
            }
        }

        latex << "\\hline" << endl;
        latex << "\\end{tabular}" << endl;
        latex << "\\end{table}" << endl;

    }

    in.close();
    out.close();
    latex.close();
}

// Con el modo manual, el usuario especifica por la consola las acciones de ambos jugadores.
void manual(Configuracion i, int pasesLimite, double o) {
    bool ganador;
    list<string> valoracion;

    while (!i.terminal(ganador, o, pasesLimite)) {
        cout << i.str();

        list<string> ID = list<string>();
        
        if (i.j())
            cout << "J1: ";
        else
            cout << "J2: ";
        string line;
        getline(cin, line);
        istringstream iss(line);
        string id;
        while (iss >> id) {
            ID.push_front(id);
        }

        Accion a = Accion(ID);
        if (i.j()) {
            for (string s : a.getID()) {
                if (s.substr(0, 3) == "idn") {
                    valoracion.push_back("x" + s.substr(3));
                } else if (s.substr(0, 3) == "idm") {
                    valoracion.push_back("!x" + s.substr(3));
                }
            }
        }

        i = Configuracion(i, a);
        cout << "\n--------------------\n\n";
    }
    cout << i.str();
    if (ganador) {
        cout << "Gana J1" << endl;
        cout << "Valoración: ";
        for (string s : valoracion) {
            cout << s << " ";
        }
        cout << endl;
    }
    else {
        cout << "Gana J2" << endl;
    }
}

// El modo alterno se utiliza para que un usuario juegue a través de la consola contra la máquina.
void alterno(Configuracion i, bool seguirEstrategia, int profundidad1, int profundidad2, int pasesLimite, int heuristicaElegida1, int heuristicaElegida2, PrecalculoHeuristica precalculo, double o, int INF) {
    bool ganador;
    list<string> valoracion;
    while (!i.terminal(ganador, o, pasesLimite)) {
        cout << i.str();

        Accion a;
        if (i.j()) {
            a = juega(i, seguirEstrategia, profundidad1, profundidad2, pasesLimite, heuristicaElegida1, heuristicaElegida2, precalculo, o, INF);
            cout << "J1: " << a.str() << endl;
        }
        else {
            cout << "J2: ";
            list<string> ID = list<string>();
            string line;
            getline(cin, line);
            istringstream iss(line);
            string id;
            while (iss >> id) {
                ID.push_front(id);
            }
            a = Accion(ID);
        }

        if (i.j()) {
            for (string s : a.getID()) {
                if (s.substr(0, 3) == "idn") {
                    valoracion.push_back("x" + s.substr(3));
                } else if (s.substr(0, 3) == "idm") {
                    valoracion.push_back("!x" + s.substr(3));
                }
            }
        }
        else {
            cout << "ACCION: " << a.str();
        }

        i = Configuracion(i, a);
        cout << "\n--------------------\n\n";
    }
    cout << i.str();
    if (ganador) {
        cout << "Gana J1" << endl;
        cout << "Valoración: ";
        for (string s : valoracion) {
            cout << s << " ";
        }
        cout << endl;
    }
    else {
        cout << "Gana J2" << endl;
    }
}

// El modo simulación se ejecuta sin requerir intervención del usuario y escribe en el fichero out.txt el resultado (y las acciones que han llevado a él) del problema suponiendo que ambos jugadores siguen una estrategia óptima, o, si esto fuera demasiado costoso, una estrategia subóptima aceptable utilizando una heurística.
void simulacion(Configuracion i, bool seguirEstrategia, int profundidad1, int profundidad2, int pasesLimite, int heuristicaElegida1, int heuristicaElegida2, PrecalculoHeuristica precalculo, double o, int INF) {
    bool ganador;
    list<string> valoracion;
    ofstream file;
    file.open ("out.txt");
    while (!i.terminal(ganador, o, pasesLimite)) {
        file << i.str();
        Accion a = juega(i, seguirEstrategia, profundidad1, profundidad2, pasesLimite, heuristicaElegida1, heuristicaElegida2, precalculo, o, INF);
        if (i.j()) {
            for (string s : a.getID()) {
                if (s.substr(0, 3) == "idn") {
                    valoracion.push_back("x" + s.substr(3));
                } else if (s.substr(0, 3) == "idm") {
                    valoracion.push_back("!x" + s.substr(3));
                }
            }
        }
        i = Configuracion(i, a);
        file << "ACCION: " << a.str();
        file << "\n--------------------\n\n";
    }
    file << i.str();
    if (ganador) {
        cout << "Gana J1" << endl;
        file << "Gana J1" << endl;
        if (!valoracion.empty())        // para obtener la solución del problema TQBF
            file << "Valoración: ";
        for (string s : valoracion) {
            file << s << " ";
        }
        file << endl;
    }
    else {
        cout << "Gana J2" << endl;
        file << "Gana J2" << endl;
    }
    file.close();
}

// ejecución de una instancia, llamado durante testing. parecido a simulacion()
bool test(Configuracion i, bool seguirEstrategia, int profundidad1, int profundidad2, int pasesLimite, int heuristicaElegida1, int heuristicaElegida2, PrecalculoHeuristica precalculo, unsigned& clk1, unsigned& clk2, double o, int INF) {
    bool ganador;
    unsigned t0, t1;
    clk1 = clk2 = 0;
    while (!i.terminal(ganador, o, pasesLimite)) {
        t0=clock();
        Accion a = juega(i, seguirEstrategia, profundidad1, profundidad2, pasesLimite, heuristicaElegida1, heuristicaElegida2, precalculo, o, INF);
        t1=clock();
        if (i.j())
            clk1 += t1 - t0;
        else
            clk2 += t1 - t0;

        i = Configuracion(i, a);
    }
    return ganador;
}

// Algoritmo minimax con poda alfa-beta
Accion juega(Configuracion configuracion, bool seguirEstrategia, int profundidad1, int profundidad2, int pasesLimite, int heuristicaElegida1, int heuristicaElegida2, PrecalculoHeuristica precalculo, double o, int INF) {
    bool j = configuracion.j();
    Accion mejorA = Accion();
    double mejorV = (j ? -INF-1 : INF+1);

    for (Accion a : configuracion.accionesDesde(seguirEstrategia)) {
        double v = (j ? valoraMin(Configuracion(configuracion, a), j ? profundidad1-1 : profundidad2-1, mejorV, INF, seguirEstrategia, pasesLimite, j ? heuristicaElegida1 : heuristicaElegida2, precalculo, o, INF) : valoraMax(Configuracion(configuracion, a), j ? profundidad1-1 : profundidad2-1, -INF, mejorV, seguirEstrategia, pasesLimite, j ? heuristicaElegida1 : heuristicaElegida2, precalculo, o, INF));
        if (j ? (v > mejorV) : (v < mejorV)) {
            mejorV = v;
            mejorA = a;
        }
    }

    return mejorA;
}

double valoraMin(Configuracion configuracion, int n, double alfa, double beta, bool seguirEstrategia, int pasesLimite, int heuristicaElegida, PrecalculoHeuristica precalculo, double o, int INF) {
    bool ganador;
    if (configuracion.terminal(ganador, o, pasesLimite))
        return (ganador ? INF : -INF);
    else if (n == 0)
        return configuracion.heuristica(heuristicaElegida, precalculo, o);

    for (Accion a : configuracion.accionesDesde(seguirEstrategia)) {
        beta = min(valoraMax(Configuracion(configuracion, a), n-1, alfa, beta, seguirEstrategia, pasesLimite, heuristicaElegida, precalculo, o, INF), beta);
        if (alfa >= beta) break;
    }

    return beta;
}

double valoraMax(Configuracion configuracion, int n, double alfa, double beta, bool seguirEstrategia, int pasesLimite, int heuristicaElegida, PrecalculoHeuristica precalculo, double o, int INF) {
    bool ganador;
    if (configuracion.terminal(ganador, o, pasesLimite))
        return (ganador ? INF : -INF);
    else if (n == 0)
        return configuracion.heuristica(heuristicaElegida, precalculo, o);
    
    for (Accion a : configuracion.accionesDesde(seguirEstrategia)) {
        alfa = max(valoraMin(Configuracion(configuracion, a), n-1, alfa, beta, seguirEstrategia, pasesLimite, heuristicaElegida, precalculo, o, INF), alfa);
        if (alfa >= beta)
            break;
    }

    return alfa;
}

int sumatorio(int start, int end) {
    int r = 0;
    while (start <= end) {
        r += start++;
    }
    return r;
}

// Genera el nodo raíz a partir de una instancia de EJE o TQBF
Configuracion generarRaiz(bool tqbf, string input1, string input2, string input3, string input4, string input5, PrecalculoHeuristica &precalculo, double& o, int& INF) {
    if (tqbf) {
        Configuracion r = transformarEntradaTQBF(input1, input2, o, INF);
        prepararHeuristica(r, precalculo, o);
        return r;
    }
    else {
        Configuracion r = entradaEJE(input1, input2, input3, input4, input5, o, INF);
        prepararHeuristica(r, precalculo, o);
        return r;
    }
}

// Calcula al principio de la ejecución algunos datos que se utilizarán para calcular la heurística de las configuraciones.
void prepararHeuristica(Configuracion configuracion, PrecalculoHeuristica &precalculo, double o) {
    for (auto [key, value] : configuracion.getT()) {
        precalculo.addGMax(max(value.getG(), o));
        precalculo.setContador(key, max(value.getG(), o));
    }
    for (Relacion r : configuracion.getM()) {
        precalculo.addGMax(max(r.getG(), o));
        for (string id : r.getID()) {
            precalculo.addContador(id, max(r.getG(), o)/r.getID().size());
        }
    }
}

// Genera el nodo raíz a partir de una instancia de TQBF
Configuracion transformarEntradaTQBF(string input1, string input2, double& o, int& INF) {
    unordered_map<string, string> varToId;
    unordered_map<string, Terreno> T = unordered_map<string, Terreno>();
    list<Relacion> M = list<Relacion>();

    vector<string> variable_names;
    string current_var;
    for (char c : input1) {
        if (c == ' ') {
            variable_names.push_back(current_var);
            current_var.clear();
        } else {
            current_var += c;
        }
    }
    variable_names.push_back(current_var);

    int f = variable_names.size() + 1;
    INF = 3*sumatorio(1,f)+1;

    for (int i = 1; i <= variable_names.size(); i++) {
        string var_name = variable_names[i-1];
        string is = to_string(i);
        varToId[var_name] = ("idn" + is);
        varToId["!" + var_name] = ("idm" + is);
        Terreno n("idn" + is, i, 1);
        Terreno m("idm" + is, i, 1);
        T["idn" + is] = n;
        T["idm" + is] = m;

        Terreno aux("idaux" + is, i, 0);
        T["idaux" + is] = aux;

        list<string> IDinter1 = list<string>();
        list<string> IDinter2 = list<string>();
        list<string> IDinter3 = list<string>();
        list<string> IDinter4 = list<string>();

        IDinter1.push_front("idn" + is);
        IDinter1.push_front("idm" + is);
        IDinter2.push_front("idn" + is);
        IDinter2.push_front("idm" + is);
        IDinter3.push_front("idn" + is);
        IDinter3.push_front("idm" + is);
        IDinter4.push_front("idn" + is);
        IDinter4.push_front("idm" + is);

        if (i != 1) {
            IDinter1.push_front("idn" + to_string(i-1));
            IDinter2.push_front("idm" + to_string(i-1));
            IDinter3.push_front("idn" + to_string(i-1));
            IDinter4.push_front("idm" + to_string(i-1));
        }

        IDinter1.push_front("idinia");
        IDinter2.push_front("idinia");
        IDinter3.push_front("idinib");
        IDinter4.push_front("idinib");

        if (i % 2 == 0) {
            IDinter1.push_front("idaux" + is);
            IDinter2.push_front("idaux" + is);
        }
        else {
            IDinter3.push_front("idaux" + is);
            IDinter4.push_front("idaux" + is);
        }
        
        Relacion rinter1 = Relacion(INF, IDinter1);
        Relacion rinter2 = Relacion(INF, IDinter2);
        Relacion rinter3 = Relacion(INF, IDinter3);
        Relacion rinter4 = Relacion(INF, IDinter4);
        M.push_front(rinter1);
        M.push_front(rinter2);
        M.push_front(rinter3);
        M.push_front(rinter4);
    }

    Terreno auxf("idaux" + to_string(f), f, 0);
    Terreno inia("idinia", 1.0/100, 3.0/100);
    Terreno inib("idinib", 2.0/100, 1.0);
    Terreno inic("idinic", 3.0/100, 0);
    Terreno fina("idfina", f, 1);
    Terreno finb("idfinb", f, 1);
    T["idaux" + to_string(f)] = auxf;
    T["idinia"] = inia;
    T["idinib"] = inib;
    T["idinic"] = inic;
    T["idfina"] = fina;
    T["idfinb"] = finb;

    list<string> IDini = list<string>();
    IDini.push_front("idinia");
    IDini.push_front("idinic");
    Relacion rini = Relacion(97.0/100, IDini);
    M.push_front(rini);

    for (int i = 1; i <= f; i++) {
        list<string> IDaux1 = list<string>();
        list<string> IDaux2 = list<string>();

        for (int x = 1; x < i; x++) {
            if (x % 2 != i % 2) {
                IDaux1.push_front("idaux" + to_string(x));
                IDaux2.push_front("idaux" + to_string(x));
            }
        }

        IDaux1.push_front("idaux" + to_string(i));
        IDaux2.push_front("idaux" + to_string(i));

        if (i % 2 == 1) {
           IDaux1.push_front("idinia");
           IDaux2.push_front("idinia");
        }
        else {
           IDaux1.push_front("idinib");
           IDaux2.push_front("idinib");
        }

        if (i != 1) {
            IDaux1.push_front("idn" + to_string(i-1));
            IDaux2.push_front("idm" + to_string(i-1));
        }

        Relacion raux1 = Relacion(INF, IDaux1);
        Relacion raux2 = Relacion(INF, IDaux2);
        M.push_front(raux1);
        M.push_front(raux2);
    }

    vector<vector<string>> matriz;
    vector<string> conjuncion;
    current_var.clear();
    for (char c : input2) {                                         
        if (c == ' ' || c == '(' || c == ')') {
            continue;
        }
        else if (c == '^') {
            conjuncion.push_back(current_var);
            current_var.clear();
        }
        else if (c == 'v' || c == '\n') {
            conjuncion.push_back(current_var);
            current_var.clear();
            matriz.push_back(conjuncion);
            conjuncion = vector<string>();
        }
        else {
            current_var += c;
        }
    }
    conjuncion.push_back(current_var);
    matriz.push_back(conjuncion);

    for (vector<string> conjuncion : matriz) {
        list<string> IDfin1 = list<string>();
        list<string> IDfin2 = list<string>();
        list<string> IDfin3 = list<string>();
        list<string> IDfin4 = list<string>();

        for (string var : conjuncion) {
            IDfin1.push_front(varToId[var]);
            IDfin2.push_front(varToId[var]);
            IDfin3.push_front(varToId[var]);
            IDfin4.push_front(varToId[var]);
        }
        
        string f1s = to_string(f-1);
        string fs = to_string(f);
        IDfin1.push_front("idn" + f1s);
        IDfin2.push_front("idm" + f1s);
        IDfin3.push_front("idn" + f1s);
        IDfin4.push_front("idm" + f1s);

        IDfin1.push_front("idfina");
        IDfin2.push_front("idfina");
        IDfin3.push_front("idfinb");
        IDfin4.push_front("idfinb");

        if ((f-1) % 2 == 0) {
            IDfin1.push_front("idaux" + f1s);
            IDfin2.push_front("idaux" + f1s);
            IDfin3.push_front("idaux" + f1s);
            IDfin4.push_front("idaux" + f1s);
        }
        else {
            IDfin1.push_front("idaux" + fs);
            IDfin2.push_front("idaux" + fs);
            IDfin3.push_front("idaux" + fs);
            IDfin4.push_front("idaux" + fs);
        }

        Relacion rfin1 = Relacion(INF, IDfin1);
        Relacion rfin2 = Relacion(INF, IDfin2);
        Relacion rfin3 = Relacion(INF, IDfin3);
        Relacion rfin4 = Relacion(INF, IDfin4);
        M.push_front(rfin1);
        M.push_front(rfin2);
        M.push_front(rfin3);
        M.push_front(rfin4);
    }

    double dini1 = 1.0/100, dini2 = 2.0/100;
    o = INF;
    
    return Configuracion(T, M, dini1, dini2);
}

// Genera el nodo raíz a partir de una instancia de EJE
Configuracion entradaEJE(string input1, string input2, string input3, string input4, string input5, double& o, int& INF) {
    unordered_map<string, Terreno> T;
    list<Relacion> M = list<Relacion>();
    double dini1, dini2;

    INF = 0;

    Terreno terreno = Terreno();
    string tid = "";
    string s = "";
    double tc, tg;
    for (char c : input1) {
        if (c == ' ' || c == '(')
            continue;
        else if (c == ')') {
            tg = stod(s);
            s = "";
            T[tid] = Terreno(tid, tc, tg);
            tid = "";
        }
        else if (c == ',') {
            if (tid == "") {
                tid = s;
                s = "";
            }
            else {
                tc = stod(s);
                s = "";
                INF += tc;
            }
        }
        else {
            s += c;
        }
    }

    double g = -1;
    list<string> ID = list<string>();
    for (char c : input2) {
        if (c == ' ' || c == '(')
            continue;
        else if (c == ')') {
                ID.push_back(s);
                s = "";
                M.push_back(Relacion(g, ID));
                g = -1;
                ID = list<string>();
        }
        else if (c == ',') {
            if (g == -1) {
                g = stod(s);
                s = "";
            }
            else {
                ID.push_back(s);
                s = "";
            }
        }
        else {
            s += c;
        }
    }

    dini1 = stod(input3);
    dini2 = stod(input4);
    o = stod(input5);
    INF += o;

    return Configuracion(T, M, dini1, dini2);
}
