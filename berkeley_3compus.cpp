// ============================================================
// Simulación berkeley con 3 computadoras
// Materia: Sistemas Distribuidos - UTN
// ============================================================

#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
using namespace std;

// --- Configuración general ---
const int NODES = 3; // cantidad de computadoras
mutex mtx_io;        // para impresión sincronizada

// --- Relojes simulados (en milisegundos desde un origen arbitrario) ---
vector<long long> clocks_ms(NODES);

// --- Generador de retardos aleatorios ---
int randomDelay(int minMs = 50, int maxMs = 200) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(minMs, maxMs);
    return dis(gen);
}

// --- Función para imprimir el estado actual de los relojes ---
void printClocks(const string &header) {
    lock_guard<mutex> lock(mtx_io);
    cout << "\n" << header << "\n";
    for (int i = 0; i < NODES; i++) {
        cout << " 🖥️ Nodo " << i << " → " << clocks_ms[i] << " ms" << endl;
    }
    cout << "---------------------------------------\n";
}

// --- Proceso coordinador (nodo 0) ---
void masterNode() {
    lock_guard<mutex> lock(mtx_io);
    cout << "\n🌐 Nodo 0 (maestro) inicia sincronización Berkeley...\n";
}

// --- Sincronización con algoritmo de Berkeley ---
void berkeley_sync() {
    masterNode();

    // Simulamos envío de mensajes y recolección de tiempos
    vector<long long> reported(NODES);
    reported[0] = clocks_ms[0]; // el maestro también reporta su tiempo

    for (int i = 1; i < NODES; i++) {
        int delay_send = randomDelay();
        this_thread::sleep_for(chrono::milliseconds(delay_send)); // retardo del envío

        // simulamos que el nodo responde con su tiempo actual
        reported[i] = clocks_ms[i];

        int delay_reply = randomDelay();
        this_thread::sleep_for(chrono::milliseconds(delay_reply)); // retardo de vuelta
    }

    // Calculamos diferencias respecto al reloj del maestro
    vector<long long> offsets(NODES);
    long long sumOffsets = 0;

    for (int i = 0; i < NODES; i++) {
        offsets[i] = reported[i] - reported[0];
        sumOffsets += offsets[i];
    }

    // Promedio de diferencias
    long long avgOffset = sumOffsets / NODES;

    {
        lock_guard<mutex> lock(mtx_io);
        cout << "\nTiempos reportados:\n";
        for (int i = 0; i < NODES; i++)
            cout << " Nodo " << i << ": " << reported[i] << " ms\n";
        cout << "Promedio de offset = " << avgOffset << " ms\n";
    }

    // Ajuste de cada reloj
    for (int i = 0; i < NODES; i++) {
        long long adjust = avgOffset - offsets[i];
        this_thread::sleep_for(chrono::milliseconds(randomDelay(10, 80)));
        clocks_ms[i] += adjust;

        lock_guard<mutex> lock(mtx_io);
        cout << " Ajustando nodo " << i
             << " por " << adjust << " ms → nuevo reloj = " << clocks_ms[i] << " ms\n";
    }

    printClocks("✅ Relojes sincronizados (después de Berkeley):");
}

// --- Función principal ---
int main() {
    cout << "====================================================\n";
    cout << " 🧭 Simulación de Sincronización de Relojes - Berkeley\n";
    cout << "====================================================\n";

    // Asignamos relojes iniciales desfasados
    clocks_ms[0] = 100000; // maestro
    clocks_ms[1] = 100300; // nodo 1 adelantado
    clocks_ms[2] = 99800;  // nodo 2 atrasado

    printClocks("⏰ Relojes iniciales:");

    // Iniciamos sincronización
    berkeley_sync();

    cout << "\n🚀 Fin de la simulación.\n";
    return 0;
}
