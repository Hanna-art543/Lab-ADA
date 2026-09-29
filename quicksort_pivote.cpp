#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

const int MAX = 4000;

int particion(float a[], int ini, int fin) {
    float pivote = a[fin];
    int i = ini - 1;
    for (int j = ini; j < fin; j++) {
        if (a[j] <= pivote) {
            i++;
            swap(a[i], a[j]);
        }
    }
    swap(a[i + 1], a[fin]);
    return i + 1;
}

// pivote = primer elemento
void quicksortFijo(float a[], int ini, int fin) {
    if (ini < fin) {
        swap(a[ini], a[fin]);
        int p = particion(a, ini, fin);
        quicksortFijo(a, ini, p - 1);
        quicksortFijo(a, p + 1, fin);
    }
}

void quicksortAleatorio(float a[], int ini, int fin) {
    if (ini < fin) {
        int r = ini + rand() % (fin - ini + 1); // pivote al azar
        swap(a[r], a[fin]);
        int p = particion(a, ini, fin);
        quicksortAleatorio(a, ini, p - 1);
        quicksortAleatorio(a, p + 1, fin);
    }
}

// solo me interesa el puntaje (tercer campo)
int leerPuntajes(string ruta, float a[]) {
    ifstream archivo(ruta);
    string linea, campo;
    int n = 0;
    while (getline(archivo, linea) && n < MAX) {
        if (linea.empty()) continue;
        stringstream ss(linea);
        getline(ss, campo, '|');
        getline(ss, campo, '|');
        getline(ss, campo, '|');
        a[n++] = stof(campo);
    }
    return n;
}

double medir(float datos[], int n, bool aleatorio) {
    float aux[MAX];
    for (int i = 0; i < n; i++)
        aux[i] = datos[i];

    clock_t inicio = clock();
    if (aleatorio)
        quicksortAleatorio(aux, 0, n - 1);
    else
        quicksortFijo(aux, 0, n - 1);
    return double(clock() - inicio) / CLOCKS_PER_SEC * 1000;
}

int main() {
    srand(time(0));
    string archivos[] = {"aleatorio_4000", "ordenado_4000", "casi_ordenado_4000", "adverso_4000", "repetidos_4000"};
    float datos[MAX];

    ofstream csv("resultados/quicksort_pivotes.csv");
    csv << "archivo,n,tiempo_fijo_ms,tiempo_aleatorio_ms" << endl;

    for (string nombre : archivos) {
        int n = leerPuntajes("datos/" + nombre + ".txt", datos);
        if (n == 0) {
            cout << "No se encontro datos/" << nombre << ".txt" << endl;
            continue;
        }
        double tFijo = medir(datos, n, false);
        double tAleatorio = medir(datos, n, true);

        cout << nombre << " -> fijo: " << tFijo << " ms  |  aleatorio: " << tAleatorio << " ms" << endl;
        csv << nombre << ".txt," << n << "," << tFijo << "," << tAleatorio << endl;
    }

    cout << "Resultados guardados en resultados/quicksort_pivotes.csv" << endl;
    return 0;
}