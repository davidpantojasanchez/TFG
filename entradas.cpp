#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
using namespace std;

string x(int varIndex);
string terreno(int terIndex, int maxValor);
string T(int maxTerrenos, int maxValor);
string relacion(int maxTerrenos, int maxValor);
string M(int maxRelaciones, int maxTerrenos, int maxValor);
vector<int> numerosAleatorios(int n, int min, int max);
string conjuncion(int maxVariables);
string disyuncion(int numConj, int numVars);
string eje(int NumTerrenos, int NumRelaciones);
string tqbf(int NumVars, int NumConj);

const double FactorNumRelacionesMax = 0.9;
const double FactorNumConjMax = 0.9;
const double FactorNumRelacionesMin = 0.5;
const double FactorNumConjMin = 0.5;

void entradas(int nEJE, int nTQBF, int escalaEJE, int escalaTQBF) {
    ofstream file;
    file.open("entradas.txt");
    srand(static_cast<unsigned int>(time(0)));

    for (int i = 0; i < nEJE; i++) {
        file << eje(escalaEJE, rand() % int((FactorNumRelacionesMax-FactorNumRelacionesMin)*escalaEJE) + FactorNumRelacionesMin*escalaEJE);
    }

    for (int i = 0; i < nTQBF; i++) {
        file << tqbf(escalaTQBF, rand() % int((FactorNumConjMax-FactorNumConjMin)*escalaTQBF) + FactorNumConjMin*escalaEJE);
    }

    file.close();
}

string x(int varIndex) {
    return "x" + to_string(varIndex);
}

// EJE

string eje(int NumTerrenos, int NumRelaciones) {
    int MaxValor = NumTerrenos*2;
    int o = NumTerrenos*MaxValor*0.12 + rand() % int(NumTerrenos*MaxValor*0.06) + 1;
    int dini1 = rand() % MaxValor/2 + 1;
    int dini2 = rand() % MaxValor/2 + 1;
    dini2 += dini2/3.0 + 1;
    return "eje:" + T(NumTerrenos, MaxValor) + ":" + M(NumRelaciones, NumTerrenos/3, MaxValor) + ":" + to_string(dini1) + ":" + to_string(dini2) + ":" + to_string(o) + "\n";
}

string terreno(int terIndex, int maxValor) {
    // int c = rand() % maxValor + 1;
    int c = maxValor;
    int g = rand() % (c*2) + 1;
    return "(" + x(terIndex) + ", " + to_string(c) + ", " +  to_string(g) + ")";
}

string T(int numTerrenos, int maxValor) {
    string terrenos;

    vector<int> na = numerosAleatorios(numTerrenos/2, 1, maxValor/2);
    for (int i = 1; i <= numTerrenos/2; i++) {
        terrenos += terreno(i, na[i-1]);
    }

    na = numerosAleatorios(numTerrenos/2 + 1, maxValor/2+1, maxValor);
    for (int i = numTerrenos/2 + 1; i <= numTerrenos; i++) {
        terrenos += terreno(i, na[i-1]);
    }

    return terrenos;
}

string relacion(int maxTerrenos, int maxValor) {
    string relacion;
    int n = max(rand() % maxTerrenos, 2);
    int g = rand() % ((maxValor+1) * n / 3) + 1;
    relacion += "(" + to_string(g);
    for (int i : numerosAleatorios(n, 1, maxTerrenos)) {
        relacion += ", " + x(i);
    }
    relacion += ")";
    return relacion;
}

string M(int numRelaciones, int maxTerrenos, int maxValor) {
    string relaciones;
    for (int i = 0; i <= numRelaciones; i++) {
        relaciones += relacion(maxTerrenos, maxValor);
    }
    return relaciones;
}

vector<int> numerosAleatorios(int n, int min, int max) {
    vector<int> list;
    for (int i = min; i <= max; i++) {
        list.push_back(i);
    }
    random_shuffle(list.begin(), list.end());
    list.resize(n);
    return list;
}

// TQBF

string tqbf(int NumVars, int NumConj) {
    string r = "tqbf:";
    for (int i = 0; i <= NumVars; i++) {
        r += x(i) + " ";
    }
    return r + ":" + disyuncion(NumConj, NumVars) + "\n";
}

string conjuncion(int maxVariables) {
    int numVars = rand() % maxVariables + 1;
    string conjuncion;
    for (int i = 1; i <= numVars; ++i) {
        if ((rand() % 2) == 0)
            conjuncion += "!";
        conjuncion += x(rand() % maxVariables + 1);
        if (i != numVars) {
            conjuncion += " ^ ";
        }
    }
    return conjuncion;
}

string disyuncion(int numConj, int numVars) {
    string disyuncion;
    for (int i = 1; i <= numConj; ++i) {
        disyuncion += "(" + conjuncion(numVars) + ")";
        if (i != numConj) {
            disyuncion += " v ";
        }
    }
    return disyuncion;
}

