
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <iomanip>
#include <filesystem>
#include <numeric>
#include <algorithm>

using namespace std;

struct Alumno {
    int dni;
    string nombre;
    double puntaje;
    string turno;
    int aula;
    string proceso;
    string area;
};

const vector<string> NOMBRES = {
    "Ana Perez", "Luis Torres", "Carlos Diaz", "Maria Lopez", "Juan Soto",
    "Carla Flores", "Pedro Quispe", "Lucia Ramos", "Diego Mamani", "Sofia Vargas",
    "Jose Condori", "Valeria Huaman", "Miguel Apaza", "Daniela Choque", "Andres Paredes",
    "Camila Nina", "Jorge Calla", "Fernanda Lima", "Alonso Ticona", "Gabriela Flores"
};
const vector<string> PROCESOS = { "Ordinario", "CEPREUNSA", "Extraordinario", "CEPRE 5TOS" };
const vector<string> AREAS    = { "Ingenierias", "Biomedicas", "Sociales" };

const double PUNTAJE_MAX      = 100.0;   // escala de 0 a 100
const int    ALUMNOS_POR_AULA = 45;
const unsigned SEMILLA        = 2024;   // semilla fija: los archivos son reproducibles

// Datos de la academia (turno, aula, proceso, area) al azar
vector<Alumno> armarAlumnos(const vector<double>& puntajes, mt19937& gen) {
    int n = (int)puntajes.size();
    int aulas = (n + ALUMNOS_POR_AULA - 1) / ALUMNOS_POR_AULA;

    uniform_int_distribution<int> turno(0, 1);
    uniform_int_distribution<int> aula(1, aulas);
    uniform_int_distribution<int> proceso(0, (int)PROCESOS.size() - 1);
    uniform_int_distribution<int> area(0, (int)AREAS.size() - 1);

    vector<Alumno> alumnos(n);
    for (int i = 0; i < n; i++) {
        alumnos[i].dni     = 40000001 + i;
        alumnos[i].nombre  = NOMBRES[i % NOMBRES.size()];
        alumnos[i].puntaje = puntajes[i];
        alumnos[i].turno   = (turno(gen) == 0) ? "Manana" : "Tarde";
        alumnos[i].aula    = aula(gen);
        alumnos[i].proceso = PROCESOS[proceso(gen)];
        alumnos[i].area    = AREAS[area(gen)];
    }
    return alumnos;
}

// ------------------------------------------------------------
// Escenarios de puntajes
// ------------------------------------------------------------
vector<double> puntajesAleatorios(int n, mt19937& gen) {
    uniform_real_distribution<double> d(0.0, PUNTAJE_MAX);
    vector<double> p(n);
    for (double& x : p) x = d(gen);
    return p;
}

// Solo 41 valores posibles (0.0, 0.5, ..., 20.0) -> muchisimos repetidos
vector<double> puntajesRepetidos(int n, mt19937& gen) {
    uniform_int_distribution<int> d(0, 40);
    vector<double> p(n);
    for (double& x : p) x = d(gen) / 2.0;
    return p;
}

// Ordenado de mayor a menor, con 5% de posiciones alteradas
vector<double> puntajesCasiOrdenados(int n, mt19937& gen) {
    vector<double> p(n);
    for (int i = 0; i < n; i++) p[i] = PUNTAJE_MAX - PUNTAJE_MAX * i / (n - 1);

    uniform_int_distribution<int> pos(0, n - 1);
    uniform_real_distribution<double> nuevo(0.0, PUNTAJE_MAX);
    int cambios = n * 5 / 100;
    for (int i = 0; i < cambios; i++) p[pos(gen)] = nuevo(gen);
    return p;
}

// Ordenado ascendente (el ranking se pide de mayor a menor, asi que
// es la entrada completamente invertida)
vector<double> puntajesOrdenados(int n) {
    vector<double> p(n);
    for (int i = 0; i < n; i++) p[i] = PUNTAJE_MAX * i / (n - 1);
    return p;
}

// CASO ADVERSO PARA QUICKSORT CLASICO
vector<double> puntajesAdversos(int n) {
    vector<int> etiqueta(n), rango(n);
    iota(etiqueta.begin(), etiqueta.end(), 0);

    int lo = 0, hi = n - 1, k = 0;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        rango[etiqueta[mid]] = k++;          // el pivote es el mejor puntaje restante
        swap(etiqueta[mid], etiqueta[hi]);   // Lomuto: pivote al final
        swap(etiqueta[lo], etiqueta[hi]);    // ...y termina en 'lo' (nada se movio)
        lo++;
    }

    vector<double> p(n);
    for (int i = 0; i < n; i++)
        p[i] = PUNTAJE_MAX * (n - 1 - rango[i]) / (n - 1);
    return p;
}

// ------------------------------------------------------------
// Guardar archivo
void guardarArchivo(const string& nombreArchivo, const vector<Alumno>& alumnos) {
    ofstream archivo("datos/" + nombreArchivo);
    if (!archivo) {
        cout << "Error al crear: " << nombreArchivo << endl;
        return;
    }
    for (const Alumno& a : alumnos) {
        archivo << a.dni << "|"
                << a.nombre << "|"
                << fixed << setprecision(2) << a.puntaje << "|"
                << a.turno << "|"
                << a.aula << "|"
                << a.proceso << "|"
                << a.area << "\n";
    }
    cout << "Generado: datos/" << nombreArchivo
         << " (" << alumnos.size() << " alumnos)" << endl;
}

int main() {
    filesystem::create_directories("datos");
    mt19937 gen(SEMILLA);

    for (int n : {100, 500, 1000, 2000, 4000})
        guardarArchivo("aleatorio_" + to_string(n) + ".txt",
                       armarAlumnos(puntajesAleatorios(n, gen), gen));

    guardarArchivo("repetidos_4000.txt",
                   armarAlumnos(puntajesRepetidos(4000, gen), gen));
    guardarArchivo("casi_ordenado_4000.txt",
                   armarAlumnos(puntajesCasiOrdenados(4000, gen), gen));
    guardarArchivo("ordenado_4000.txt",
                   armarAlumnos(puntajesOrdenados(4000), gen));
    guardarArchivo("adverso_4000.txt",
                   armarAlumnos(puntajesAdversos(4000), gen));

    cout << "\nDatos generados correctamente en la carpeta 'datos'.\n";
    return 0;
}
