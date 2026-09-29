#include <iostream>
#include <functional>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <cmath>

using namespace std;
using namespace chrono;

// ESTRUCTURA DE DATOS

struct Alumno {
    int dni;
    string nombre;
    float puntaje;
    string turno;
    int aula;
    string proceso;
    string area;
    string sede;
    int grupo;
};

// ESTADISTICAS
struct Estadisticas {

    long long comparaciones = 0;
    long long intercambios = 0;

    int profundidadMax = 0;

    size_t memoriaAux = 0;

    // Estadisticas del algoritmo hibrido
    int particionesQuick = 0;
    int usosInsertion = 0;
    int usosMerge = 0;

    // Cantidad de particiones consideradas malas
    int particionesMalas = 0;
};

// FUNCIONES BASICAS
// Orden descendente por puntaje
bool mayorPuntaje(const Alumno& a, const Alumno& b, Estadisticas& e) {
    e.comparaciones++;
    return a.puntaje > b.puntaje;
}

// Intercambio
void intercambiar(vector<Alumno>& A, int i, int j, Estadisticas& e) {
    if (i != j) {
        swap(A[i], A[j]);
        e.intercambios++;
    }
}

// 1. INSERTION SORT
void insertionSort(vector<Alumno>& A, int inicio, int fin, Estadisticas& e) {
    for (int i = inicio + 1;
         i <= fin;
         i++) {

        Alumno actual = A[i];

        int j = i - 1;

        while (j >= inicio &&
               mayorPuntaje(actual, A[j], e)) {

            A[j + 1] = A[j];

            e.intercambios++;

            j--;
        }

        A[j + 1] = actual;
    }
}

// 2. QUICK SORT
// Particion de Quick Sort
int particionQuick(vector<Alumno>& A, int inicio, int fin, Estadisticas& e) {
    // Utilizamos el elemento central como pivote
    int pivote =
        inicio + (fin - inicio) / 2;

    intercambiar(A, pivote, fin, e);
    int i = inicio;

    for (int j = inicio; j < fin; j++) {
        if (mayorPuntaje(A[j], A[fin], e)) {
            intercambiar(A, i, j, e);
            i++;
        }
    }

    intercambiar(A, i, fin, e);
    return i;
}

// 3. MERGE SORT
void merge(vector<Alumno>& A, vector<Alumno>& aux, int inicio, int medio, int fin, Estadisticas& e) {
    int i = inicio;
    int j = medio + 1;
    int k = inicio;

    while (i <= medio && j <= fin) {

        if (mayorPuntaje(A[j], A[i], e)) {
            aux[k++] = A[j++];
        }
        else {
            aux[k++] = A[i++];
        }
        e.intercambios++;
    }

    while (i <= medio) {

        aux[k++] = A[i++];

        e.intercambios++;
    }

    while (j <= fin) {

        aux[k++] = A[j++];

        e.intercambios++;
    }

    for (int x = inicio;
         x <= fin;
         x++) {

        A[x] = aux[x];

        e.intercambios++;
    }
}

void mergeSortRec(vector<Alumno>& A, vector<Alumno>& aux, int inicio, int fin, int profundidad,  Estadisticas& e) {
    if (inicio >= fin) {
        return;
    }

    e.profundidadMax = max(e.profundidadMax, profundidad);

    int medio = inicio + (fin - inicio) / 2;

    mergeSortRec(A, aux, inicio, medio, profundidad + 1, e);

    mergeSortRec(A, aux, medio + 1, fin, profundidad + 1, e);

    merge(A, aux, inicio, medio, fin, e);
}

// MERGE SORT PARA EL HIBRIDO
void ejecutarMergeEnRango(vector<Alumno>& A, vector<Alumno>& aux, int inicio, int fin, int profundidad, Estadisticas& e) {

    if (inicio >= fin) {
        return;
    }

    e.usosMerge++;

    mergeSortRec(A, aux, inicio, fin, profundidad, e);
}

// QUICK SORT HIBRIDO

// Regla:
// Si n <= 16 -> Insertion
// Si particion es muy desbalanceada varias veces -> Merge
// Si todo esta normal -> Quick

void hibridoRec(vector<Alumno>& A,
                vector<Alumno>& aux,
                int inicio,
                int fin,
                int profundidad,
                int particionesMalasConsecutivas,
                Estadisticas& e) {

    while (inicio < fin) {

        int n =
            fin - inicio + 1;

        e.profundidadMax =
            max(e.profundidadMax,
                profundidad);

        // REGLA 1: PARTE PEQUEÑA -> INSERTION SORT
        if (n <= 16) {

            insertionSort(A, inicio, fin, e);
            e.usosInsertion++;
            return;
        }

        // QUICK SORT
        e.particionesQuick++;

        int posicion =
            particionQuick(A,
                           inicio,
                           fin,
                           e);

        // Cantidad de elementos a cada lado
        int izquierda =
            posicion - inicio;

        int derecha =
            fin - posicion;

        int mayorParte =
            max(izquierda,
                derecha);

        // DETECTAR PARTICION DESBALANCEADA

        bool particionMala = mayorParte > n * 0.875;

        if (particionMala) {
            e.particionesMalas++;
            particionesMalasConsecutivas++;
        }
        else {
            particionesMalasConsecutivas = 0;
        }

        // REGLA 2:
        // SI QUICK GENERA PARTICIONES MUY MALAS
        // -> MERGE SORT

        if (particionesMalasConsecutivas >= 2) {

            ejecutarMergeEnRango(A,
                                 aux,
                                 inicio,
                                 fin,
                                 profundidad,
                                 e);

            return;
        }

        // PROCESAR PRIMERO LA PARTE PEQUEÑA
        if (izquierda < derecha) {

            if (inicio < posicion - 1) {

                hibridoRec(A,
                           aux,
                           inicio,
                           posicion - 1,
                           profundidad + 1,
                           particionesMalasConsecutivas,
                           e);
            }

            inicio = posicion + 1;
        }
        else {

            if (posicion + 1 < fin) {

                hibridoRec(A,
                           aux,
                           posicion + 1,
                           fin,
                           profundidad + 1,
                           particionesMalasConsecutivas,
                           e);
            }

            fin = posicion - 1;
        }

        profundidad++;
    }
}

// ALGORITMO HIBRIDO
void algoritmoHibrido(vector<Alumno>& A,
                      Estadisticas& e) {

    if (A.size() < 2) {
        return;
    }

    // Memoria auxiliar para cuando se necesite Merge Sort
    vector<Alumno> aux(A.size());

    hibridoRec(A,
               aux,
               0,
               A.size() - 1,
               0,
               0,
               e);

    e.memoriaAux =
        A.size() * sizeof(Alumno);
}

// FUNCIONES PARA EJECUTAR LOS 3 ALGORITMOS
void ejecutarInsertion(vector<Alumno>& A, Estadisticas& e) {

    if (!A.empty()) {
        insertionSort(A, 0, A.size() - 1, e);
    }
    e.memoriaAux =
        sizeof(Alumno);
}

void ejecutarQuick(vector<Alumno>& A, Estadisticas& e) {

    if (!A.empty()) {

        // Quick clasico
        function<void(int, int, int)> quickRec;

        quickRec =
            [&](int inicio, int fin, int profundidad) {
                if (inicio >= fin) {
                    return;
                }

                e.profundidadMax = max(e.profundidadMax, profundidad);
                int pivote =
                    particionQuick(A, inicio, fin, e);
                quickRec(inicio, pivote - 1, profundidad + 1);

                quickRec(pivote + 1, fin, profundidad + 1);
            };

        quickRec(0, A.size() - 1, 0);
    }

    e.memoriaAux = sizeof(Alumno) + e.profundidadMax * 64;
}


void ejecutarMerge(vector<Alumno>& A,
                   Estadisticas& e) {

    if (!A.empty()) {

        vector<Alumno> aux(A.size());

        mergeSortRec(A,
                     aux,
                     0,
                     A.size() - 1,
                     0,
                     e);

        e.memoriaAux =
            A.size() * sizeof(Alumno);
    }
}

void ejecutarHibrido(vector<Alumno>& A,
                     Estadisticas& e) {

    algoritmoHibrido(A, e);
}

// LISTA DE LOS 3 ALGORITMOS
struct Algoritmo {

    string nombre;

    void (*ejecutar)(
        vector<Alumno>&,
        Estadisticas&);
};

const vector<Algoritmo> ALGORITMOS = {

    {"Insertion", ejecutarInsertion},

    {"Quick Sort", ejecutarQuick},

    {"Merge Sort", ejecutarMerge},

    {"HIBRIDO", ejecutarHibrido}
};

// VERIFICAR SI ESTA ORDENADO

bool estaOrdenado(
    const vector<Alumno>& A) {

    for (size_t i = 1;
         i < A.size();
         i++) {

        if (A[i - 1].puntaje <
            A[i].puntaje) {

            return false;
        }
    }

    return true;
}

// OBTENER DNIs

vector<int> obtenerDNIs(
    const vector<Alumno>& A) {

    vector<int> dnis;

    for (const Alumno& a : A) {

        dnis.push_back(a.dni);
    }

    sort(dnis.begin(),
         dnis.end());

    return dnis;
}

// LEER ARCHIVOS

vector<Alumno> leerArchivo(
    const string& ruta) {

    vector<Alumno> alumnos;

    ifstream archivo(ruta);

    if (!archivo.is_open()) {

        return alumnos;
    }

    string linea;

    while (getline(archivo, linea)) {

        if (linea.empty()) {
            continue;
        }

        stringstream ss(linea);

        string campo[7];

        for (int i = 0;
             i < 7;
             i++) {

            getline(ss,
                    campo[i],
                    '|');
        }

        Alumno a;

        a.dni = stoi(campo[0]);

        a.nombre = campo[1];

        a.puntaje = stof(campo[2]);

        a.turno = campo[3];

        a.aula = stoi(campo[4]);

        a.proceso = campo[5];

        a.area = campo[6];

        a.sede = "";

        a.grupo = 0;

        alumnos.push_back(a);
    }

    archivo.close();

    return alumnos;
}

// MEDICION
struct Medicion {

    string nombre;

    Estadisticas estadisticas;

    long long tiempo = 0;

    bool correcto = false;
};


Medicion medir(
    const Algoritmo& algoritmo,
    const vector<Alumno>& original,
    const vector<int>& dnisOriginales) {

    Medicion resultado;

    resultado.nombre =
        algoritmo.nombre;

    long long mejorTiempo = -1;

    // Ejecutar 3 veces
    for (int repeticion = 0;
         repeticion < 3;
         repeticion++) {

        vector<Alumno> copia =
            original;

        Estadisticas e;

        auto inicio =
            steady_clock::now();

        algoritmo.ejecutar(copia,
                           e);

        auto fin =
            steady_clock::now();

        long long tiempo =
            duration_cast<microseconds>(
                fin - inicio).count();

        if (mejorTiempo == -1 ||
            tiempo < mejorTiempo) {

            mejorTiempo =
                tiempo;
        }

        if (repeticion == 2) {

            resultado.estadisticas =
                e;

            resultado.correcto =
                estaOrdenado(copia) &&
                obtenerDNIs(copia) ==
                dnisOriginales;
        }
    }

    resultado.tiempo =
        mejorTiempo;

    return resultado;
}

// ARCHIVOS DE PRUEBA
const vector<string> ARCHIVOS = {

    "aleatorio_100.txt",
    "aleatorio_500.txt",
    "aleatorio_1000.txt",
    "aleatorio_2000.txt",
    "aleatorio_4000.txt",

    "repetidos_4000.txt",
    "casi_ordenado_4000.txt",
    "ordenado_4000.txt",
    "adverso_4000.txt"
};

// PRUEBAS EXPERIMENTALES
void pruebasExperimentales() {

    filesystem::create_directories(
        "resultados");

    ofstream csv(
        "resultados/resultados_benchmark.csv");

    csv << "archivo,n,algoritmo,"
        << "comparaciones,intercambios,"
        << "tiempo_us,memoria_aux_bytes,"
        << "prof_max,particiones_quick,"
        << "usos_insertion,usos_merge,"
        << "particiones_malas,correcto\n";

    cout << "\n";
    cout << "                 PRUEBAS EXPERIMENTALES\n";
    cout << "============================================================\n";

    for (const string& nombreArchivo :
         ARCHIVOS) {

        vector<Alumno> datos =
            leerArchivo(
                "datos/" +
                nombreArchivo);

        if (datos.empty()) {

            cout << "\nNo se encontro: datos/"
                 << nombreArchivo
                 << "\n";

            continue;
        }

        vector<int> dnisOriginales =
            obtenerDNIs(datos);

        cout << "\n";
        cout << "Archivo: "
             << nombreArchivo
             << "  N = "
             << datos.size()
             << "\n";

        cout << left
             << setw(17)
             << "Algoritmo"
             << right
             << setw(14)
             << "Comparaciones"
             << setw(14)
             << "Intercambios"
             << setw(13)
             << "Tiempo(us)"
             << setw(13)
             << "MemAux(B)"
             << setw(10)
             << "ProfMax"
             << setw(10)
             << "Correcto"
             << "\n";

        cout << string(91, '-')
             << "\n";

        for (const Algoritmo& algoritmo :
             ALGORITMOS) {

            Medicion m =
                medir(algoritmo,
                      datos,
                      dnisOriginales);

            cout << left
                 << setw(17)
                 << m.nombre
                 << right
                 << setw(14)
                 << m.estadisticas.comparaciones
                 << setw(14)
                 << m.estadisticas.intercambios
                 << setw(13)
                 << m.tiempo
                 << setw(13)
                 << m.estadisticas.memoriaAux
                 << setw(10)
                 << m.estadisticas.profundidadMax
                 << setw(10)
                 << (m.correcto
                         ? "SI"
                         : "NO")
                 << "\n";

            csv << nombreArchivo << ","
                << datos.size() << ","
                << m.nombre << ","
                << m.estadisticas.comparaciones
                << ","
                << m.estadisticas.intercambios
                << ","
                << m.tiempo
                << ","
                << m.estadisticas.memoriaAux
                << ","
                << m.estadisticas.profundidadMax
                << ","
                << m.estadisticas.particionesQuick
                << ","
                << m.estadisticas.usosInsertion
                << ","
                << m.estadisticas.usosMerge
                << ","
                << m.estadisticas.particionesMalas
                << ","
                << (m.correcto ? 1 : 0)
                << "\n";
        }
    }

    csv.close();

    // TABLA DE COMPLEJIDAD

    cout << "\n";
    cout << "============================================================\n";
    cout << "              COMPLEJIDAD DE LOS ALGORITMOS\n";
    cout << "============================================================\n";

    cout << left
         << setw(18)
         << "Algoritmo"
         << setw(14)
         << "Mejor"
         << setw(14)
         << "Promedio"
         << setw(14)
         << "Peor"
         << "Espacio\n";

    cout << string(70, '-')
         << "\n";

    cout << left
         << setw(18)
         << "Insertion"
         << setw(14)
         << "O(n)"
         << setw(14)
         << "O(n^2)"
         << setw(14)
         << "O(n^2)"
         << "O(1)\n";

    cout << setw(18)
         << "Quick Sort"
         << setw(14)
         << "O(n log n)"
         << setw(14)
         << "O(n log n)"
         << setw(14)
         << "O(n^2)"
         << "O(log n)\n";

    cout << setw(18)
         << "Merge Sort"
         << setw(14)
         << "O(n log n)"
         << setw(14)
         << "O(n log n)"
         << setw(14)
         << "O(n log n)"
         << "O(n)\n";

    cout << setw(18)
         << "HIBRIDO"
         << setw(14)
         << "O(n)"
         << setw(14)
         << "O(n log n)"
         << setw(14)
         << "O(n log n)"
         << "O(n)\n";

    cout << "\nCSV generado en:\n";

    cout << "resultados/resultados_benchmark.csv\n";
}

// ASIGNAR SEDE Y GRUPO
void asignarSedeYGrupo(
    vector<Alumno>& alumnos) {

    const int TAM_GRUPO = 10;

    for (size_t i = 0;
         i < alumnos.size();
         i++) {

        int bloque =
            i / TAM_GRUPO;

        if (bloque % 2 == 0) {

            alumnos[i].sede =
                "Sede A";
        }
        else {

            alumnos[i].sede =
                "Sede B";
        }

        alumnos[i].grupo =
            bloque + 1;
    }
}

// DESCRIBIR LA ESTRATEGIA
string estrategiaHibrido(
    const Estadisticas& e) {

    string resultado;

    resultado =
        "Quick=" +
        to_string(e.particionesQuick);

    resultado +=
        " Insertion=" +
        to_string(e.usosInsertion);

    resultado +=
        " Merge=" +
        to_string(e.usosMerge);

    resultado +=
        " Malas=" +
        to_string(e.particionesMalas);

    return resultado;
}

// GENERAR RANKING

void generarRanking(
    const string& archivo) {

    vector<Alumno> alumnos =
        leerArchivo(
            "datos/" +
            archivo);

    if (alumnos.empty()) {

        cout << "\nNo se pudo leer: "
             << archivo
             << "\n";

        return;
    }

    // Agrupar por proceso y area
    map<string, vector<Alumno>> grupos;

    for (const Alumno& alumno :
         alumnos) {

        string clave =
            alumno.proceso +
            " - " +
            alumno.area;

        grupos[clave].push_back(
            alumno);
    }

    cout << "\n";
    cout << "============================================================\n";
    cout << "                 RANKING POR PROCESO Y AREA\n";
    cout << "============================================================\n";

    cout << left
         << setw(32)
         << "Proceso - Area"
         << right
         << setw(8)
         << "N"
         << setw(14)
         << "Comparaciones"
         << setw(14)
         << "Intercambios"
         << setw(12)
         << "Tiempo(us)"
         << setw(10)
         << "Correcto"
         << "\n";

    cout << string(90, '-')
         << "\n";

    filesystem::create_directories(
        "resultados");

    ofstream salida(
        "resultados/ranking_" +
        archivo);

    salida << "proceso|area|puesto|dni|nombre|"
           << "puntaje|sede|grupo\n";

    long long totalComparaciones = 0;
    long long totalIntercambios = 0;
    long long totalTiempo = 0;

    bool todoCorrecto = true;

    for (auto& grupo :
         grupos) {

        vector<Alumno>& lista =
            grupo.second;

        Estadisticas e;

        auto inicio =
            steady_clock::now();

        algoritmoHibrido(lista,
                         e);

        auto fin =
            steady_clock::now();

        long long tiempo =
            duration_cast<microseconds>(
                fin - inicio).count();

        bool correcto =
            estaOrdenado(lista);

        if (!correcto) {

            todoCorrecto = false;
        }

        asignarSedeYGrupo(lista);

        totalComparaciones +=
            e.comparaciones;

        totalIntercambios +=
            e.intercambios;

        totalTiempo +=
            tiempo;

        cout << left
             << setw(32)
             << grupo.first
             << right
             << setw(8)
             << lista.size()
             << setw(14)
             << e.comparaciones
             << setw(14)
             << e.intercambios
             << setw(12)
             << tiempo
             << setw(10)
             << (correcto
                     ? "SI"
                     : "NO")
             << "\n";

        // Guardar ranking
        for (size_t i = 0;
             i < lista.size();
             i++) {

            Alumno& a =
                lista[i];

            salida
                << a.proceso << "|"
                << a.area << "|"
                << i + 1 << "|"
                << a.dni << "|"
                << a.nombre << "|"
                << fixed
                << setprecision(2)
                << a.puntaje << "|"
                << a.sede << "|"
                << a.grupo
                << "\n";
        }
    }

    cout << string(90, '-')
         << "\n";

    cout << left
         << setw(32)
         << "TOTAL"
         << right
         << setw(8)
         << alumnos.size()
         << setw(14)
         << totalComparaciones
         << setw(14)
         << totalIntercambios
         << setw(12)
         << totalTiempo
         << setw(10)
         << (todoCorrecto
                 ? "SI"
                 : "NO")
         << "\n";

    salida.close();

    cout << "\nRanking generado en:\n";

    cout << "resultados/ranking_"
         << archivo
         << "\n";
}

// MAIN

int main(int argc,
         char** argv) {

    string archivoRanking;

    if (argc > 1) {

        archivoRanking =
            argv[1];
    }
    else {

        archivoRanking =
            "aleatorio_4000.txt";
    }

    cout << "\n";
    cout << "============================================================\n";
    cout << "       ACADEMIA XYZ - ORDENAMIENTO DE ALUMNOS\n";
    cout << "              ALGORITMO HIBRIDO ADAPTATIVO\n";
    cout << "============================================================\n";

    filesystem::create_directories(
        "resultados");

    // Ejecutar pruebas
    pruebasExperimentales();

    // Generar ranking
    generarRanking(
        archivoRanking);

    cout << "\nPrograma terminado.\n";

    return 0;
}