#include <iostream>
#include <string>
#include <queue>
#include <list>
#include <unordered_map>

using namespace std;

class Terreno {
public:
    string getId() { return id; }
    double getC() { return c; }
    double getG() { return g; }

    Terreno() {};
    Terreno(string i, double c, double g) : id(i), c(c), g(g) {}

private:
    string id;
    double c;
    double g;

};

class Relacion {
public:
    double getG() { return g; }

    const list<string> getID() { return ID; }

    Relacion(double g, list<string> ID) : g(g), ID(ID) {}

    // Devuelve si la compra de un terreno ("nuevoId") completa la relación. Comprueba si el jugador ha comprado todos los terrenos en la relación y si ésta contiene el terreno nuevo (para no sumar el valor de ganancia varias veces)
    bool completa(unordered_map<string, Terreno> E, string nuevoId) {
        bool tieneNuevoId = false;
        for (string id : ID) {
            if (E.count(id) == 0) {
                return false;
            }
            if (id == nuevoId) {
                tieneNuevoId = true;
            }
        }
        return tieneNuevoId;
    }

private:
    double g;
    list<string> ID;

};

class Accion {
public:
    Accion() {}

    bool empty() {
        return ID.empty();
    }

    const list<string> getID() {
        return ID;
    }

    // Crea una acción a partir de una lista de terrenos que el jugador quiere comprar
    Accion(list<string> ID) : ID(ID) {
        s = "";
        for (string id : ID) {
            s += id + " ";
        }
    }

    // Devuelve una cadena que representa la acción
    string str() {
        return s;
    }

private:
    string s;
    list<string> ID;

};

// Creado al principio de la ejecución de una instancia. Usado para calcular la heurística.
class PrecalculoHeuristica {
public:
    int getGMax() { return gananciaMaxima; }

    double getContador(string key) {
        return contador[key];
    }

    void addGMax(int g) {
        gananciaMaxima += g;
    }

    void setContador(string key, double value) {
        contador[key] = value;
    }

    void addContador(string key, double value) {
        contador[key] += value;
    }

private:
    int gananciaMaxima = 0;
    unordered_map<string, double> contador;

};

class Configuracion {
public:
    const unordered_map<string, Terreno> getT() {
        return T; 
    }

    const list<Relacion> getM() {
        return M;
    }


    // Calcula las acciones que se pueden realizar desde la configuración. si seguirEstrategia es true, se devuelven únicamente las acciones que entran dentro de la estrategia
    list<Accion> accionesDesde(bool seguirEstrategia) {
        if (seguirEstrategia)
            return estrategia();

        list<list<string>> lista;
        double d = (j() ? d1 : d2);
        list<string> padre = list<string>();
        accionesAux(lista, padre, d, unordered_map<string,bool>());
        lista.push_back(padre);

        list<Accion> acciones = list<Accion>();
        for (list<string> l : lista) {
            Accion a = Accion(l);
            acciones.push_back(a);
        }
        return acciones;
    }

    // Devuelve una cadena que representa la configuración
    string str() {
        string r = "";
        r += "CONFIGURACION: \n";
        if (j())
            r += "J1\n";
        else
            r += "J2\n";
        r += "t:" + to_string(t) + "\nd1:" + to_string(d1) + "\nd2:" + to_string(d2) + "\ng1:" + to_string(g1) + "\ng2:" + to_string(g2) + "\npases1:" + to_string(pases1) + "\npases2:" + to_string(pases2);

        r += "\nT\n";
        for (const auto& [key, value] : T) {
            r += "   " + key + ", ";
        }
        r += "\nE1\n";
        for (const auto& [key, value] : E1) {
            r += "   " + key + ", ";
        }
        r += "\nE2\n";
        for (const auto& [key, value] : E2) {
            r += "   " + key + ", ";
        }
        r += "\n";
        return r;
    }


    // Devuelve true si el jugador que tiene que realizar un movimiento desde la configuración es J1 y false si es J2
    bool j() {
        return (t % 2 == 1);
    }


    // Devuelve true si la configuración es terminal e indica en "ganador" qué jugador ha ganado la partida
    bool terminal(bool& ganador, double o, int pasesLimite) {
        if (pases2 >= pasesLimite || g1 >= o) {
            ganador = true;
            return true;
        }
        if (pases1 >= pasesLimite || g2 >= o || T.empty()) {
            ganador = false;
            return true;
        }
        return false;
    }

    // Devuelve la valoración de la configuración, siguiendo la heurística especificada. Requiere el valor de ganancia objetivo (o) y algunos datos sobre la instancia que se han calculado al comienzo de la partida (precalculo)
    double heuristica(int heuristicaElegida, PrecalculoHeuristica precalculo, double o) {
        double h = 0, h2 = 0;
        switch (heuristicaElegida) {   // estas heuristicaElegidas consideran:
            case 1: // la ganancia del primer jugador
            h = g1/o;
            break;

            case 2: // la ganancia de ambos jugadores
            h = (g1/o)/2 + ((o - g2)/o) / 2;
            break;

            case 3: // la ganancia y la ganancia esperada del primer jugador
            for (const auto& [key, value] : E1) {
                h += (precalculo.getContador(key))/precalculo.getGMax();
            }
            break;

            case 4: // la ganancia y la ganancia esperada de ambos jugadores
            for (const auto& [key, value] : E1) {
                h += precalculo.getContador(key) / precalculo.getGMax();
            }
            for (const auto& [key, value] : E2) {
                h2 += precalculo.getContador(key) / precalculo.getGMax();
            }
            h = (h + 1-h2)/2;
            break;

            case 5: // la ganancia y la ganancia esperada de ambos jugadores, valorando menos la ganancia esperada (media de heurística 2 y 4)
            for (const auto& [key, value] : E1) {
                h += precalculo.getContador(key) / precalculo.getGMax();
            }
            for (const auto& [key, value] : E2) {
                h2 += precalculo.getContador(key) / precalculo.getGMax();
            }
            h = ((h + 1-h2)/2 + (g1/o)/2 + ((o - g2)/o)/2) / 2;
            break;
        }
        return h;
    }

    // Crea la configuración siguiente desde la configuración cuando se utiliza la acción indicada
    Configuracion(Configuracion configuracion, Accion accion) {

        d1 = configuracion.d1, d2 = configuracion.d2;
        g1 = configuracion.g1, g2 = configuracion.g2;
        pases1 = configuracion.pases1, pases2 = configuracion.pases2;
        T = copia(configuracion.T);
        E1 = copia(configuracion.E1);
        E2 = copia(configuracion.E2);
        M = copia(configuracion.M);
        
        if (accion.empty()) {
            if (configuracion.j())
                pases1++;
            else
                pases2++;
        }
        else {
            if (configuracion.j())
                pases1 = 0;
            else
                pases2 = 0;
        }

        for (string id : accion.getID()) {

            Terreno terreno = configuracion.T[id];
            T.erase(id);

            if (configuracion.j()) {
                E1.insert({id, terreno});
                d1 -= terreno.getC();
                g1 += terreno.getG() + gAdicionalNueva(E1, id);
            }
            else {
                E2.insert({id, terreno});
                d2 -= terreno.getC();
                g2 += terreno.getG() + gAdicionalNueva(E2, id);
            }

        }

        if (configuracion.j()) d1 += g1;
        else d2 += g2;

        t = configuracion.t + 1;

        i = &configuracion;
    }
    
    // Crea una configuración a partir de unos valores iniciales. Se utiliza para generar el nodo raíz del árbol.
    Configuracion(unordered_map<string, Terreno> terrenos, list<Relacion> relaciones, double dini1, double dini2) {
        t = 1;
        d1 = dini1, d2 = dini2;
        g1 = g2 = 0;
        pases1 = pases2 = 0;
        T = terrenos;
        E1 = unordered_map<string, Terreno>();
        E2 = unordered_map<string, Terreno>();
        M = relaciones;
    }

private:
    int t;
    double d1, d2, g1, g2;
    unordered_map<string, Terreno> T;
    unordered_map<string, Terreno> E1;
    unordered_map<string, Terreno> E2;
    list<Relacion> M;

    int pases1, pases2;
    Configuracion *i = NULL;

    unordered_map<string, Terreno> copia(unordered_map<string, Terreno> o) {
        unordered_map<string, Terreno> c(o);
        return c;
    }

    list<Relacion> copia(list<Relacion> o) {
        list<Relacion> c(o);
        return c;
    }

    // Calcula la ganancia que se obtendrá por haber completado relaciones con la compra de un terreno especificado por "nuevoId"
    double gAdicionalNueva(unordered_map<string, Terreno> E, string nuevoId) {
        double g = 0;
        for (Relacion r : M) {
            if (r.completa(E, nuevoId)) {
                g += r.getG();
            }
        }
        return g;
    }


    // Función auxiliar usada para calcular las acciones desde la configuración sin tener que seguir la estrategia
    void accionesAux(list<list<string>> &lista, list<string> padre, double d, unordered_map<string,bool> visto) {
        for (auto [key, value] : T) {
            if (!visto[key] && value.getC() <= d) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
                list<string> hijo(padre);
                visto[key] = true;
                hijo.push_back(key);
                lista.push_back(hijo);
                accionesAux(lista, hijo, d-value.getC(), visto);
            }
        }
    }

    // Calcula las acciones desde la configuración siguiendo la estrategia
    list<Accion> estrategia() {
        list<Accion> acciones = list<Accion>();
        int k = T.size() + E1.size() + E2.size() - 2;   // 4 + 3*nvar
        int i = (t-1) / 3;

        list<string> lista = list<string>();
        if (t == 1) {
            lista.push_back("idinia");
            acciones.push_back(Accion(lista));
        }
        else if (t == 2) {
            lista.push_back("idinib");
            acciones.push_back(Accion(lista));
        }
        else if (t == 3) {
            lista.push_back("idinic");
            acciones.push_back(Accion(lista));
        }
        else if (t == k+1) {
            lista.push_back("idfina");  // comprar fina o finb es equivalente
            acciones.push_back(Accion(lista));
        }
        else if (t == k+2) {
            lista.push_back("idfinb");
            acciones.push_back(Accion(lista));
        }
        else if ((t % 3) == 1) {    // aux
            lista.push_back("idaux" + to_string(i));
            acciones.push_back(Accion(lista));
        }
        else if ((t % 3) == 2) {    // elige
            lista.push_back("idn" + to_string(i));
            list<string> lista2 = list<string>();
            lista2.push_back("idm" + to_string(i));
            acciones.push_back(Accion(lista));
            acciones.push_back(Accion(lista2));
        }
        else if ((t % 3) == 0) {    // restante
            if (T.count("idn" + to_string(i)) != 0)
                lista.push_back("idn" + to_string(i));
            else
                lista.push_back("idm" + to_string(i));
            acciones.push_back(Accion(lista));
        }

        return acciones;
    }

};
