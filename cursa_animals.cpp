#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm> // Para usar any_of
#include <atomic> // Para usar variables atómicas

using namespace std;

struct Animal {
    string name;
    int baseSpeed; // Velocitat base en unitats per iteració
    int position = 0; // Posició actual
    bool finished = false; // Si ha acabt la cursa

    Animal(string n, int speed) : name(n), baseSpeed(speed) {}
};

// Variable atómica per controlar si la cursa ha acabt
atomic<bool> raceFinished(false);

// Funció per moure l'animal
void runRace(Animal& animal, int finishLine) {
    random_device rd; // Per generació d'aleatoris
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 3); // Generar un increment aleatori entre 1 i 3

    while (!animal.finished && !raceFinished) {
        this_thread::sleep_for(chrono::milliseconds(300)); // Espera un temps per crear efecte visual
        int advance = animal.baseSpeed + dist(gen); // Avanç aleatori

        // Introduir un mecanisme de cansament
        uniform_int_distribution<> fatigueDist(0, 4); // Ajustem per cansament

        if (animal.name == "Llebre") {
            if (fatigueDist(gen) == 1) { // 40% de probabilitat de cansament
                this_thread::sleep_for(chrono::milliseconds(400)); // Temps de descans més llarg
                continue; // No avançar aquest torn
            }
        } else if (animal.name == "Gos") {
            if (fatigueDist(gen) == 0) { // 20% de probabilitat de cansament
                this_thread::sleep_for(chrono::milliseconds(200)); // Temps de descans més curt
                continue; // No avançar aquest torn
            }
        }

        // Actualitza la posició, assegurant-se que no supera la línia de meta
        animal.position += advance;
        if (animal.position > finishLine) {
            animal.position = finishLine; // Limita la posició a la línia de meta
        }

        // Actualitza si l'animal ha acabat
        if (animal.position >= finishLine) {
            animal.finished = true;
            raceFinished = true; // Indica que la cursa ha acabt
        }
    }
}

// Funció per mostrar la posició de tots els animals
void displayRace(const vector<Animal>& animals, int finishLine) {
    while (!raceFinished) {
        system("clear"); // O "cls" en Windows
        for (const auto& animal : animals) {
            cout << "|" << "|: " << string(animal.position / 2, '.') << " " << animal.name << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

int main() {
    const int finishLine = 100; // Distància total de la cursa
    vector<Animal> animals = {
        Animal("Llebre", 6), // Llebre amb velocitat base 5
        Animal("Tortuga", 3), // Tortuga amb velocitat base 2
        Animal("Gos", 5) // Gos amb velocitat base 4
    };

    // Ajustar aleatoriament la velocitat base de la llebre per variar els resultats
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> speedDist(3, 5); // Velocitat aleatòria entre 3 i 5
    animals[0].baseSpeed = speedDist(gen);

    vector<thread> threads;

    // Inicia els threads per cada animal
    for (auto& animal : animals) {
        threads.emplace_back(runRace, ref(animal), finishLine);
    }

    // Inicia la visualització de la cursa
    thread displayThread(displayRace, ref(animals), finishLine);

    // Espera que tots els threads acabin
    for (auto& th : threads) {
        th.join();
    }

    // Espera que el thread de visualització acabi
    displayThread.join();

    // Actualitza la pantalla una última vegada abans de mostrar el resultat
    system("clear"); // O "cls" en Windows
    for (const auto& animal : animals) {
        cout << "|" << "|: " << string(animal.position / 2, '-') << " " << animal.name << endl;
    }

    // Troba el guanyador o empats
    vector<Animal*> winners;
    int maxPosition = 0;

    for (auto& animal : animals) {
        if (animal.position > maxPosition) {
            maxPosition = animal.position;
            winners.clear(); // Neteja els guanyadors anteriors
            winners.push_back(&animal); // Afegeix l'animal actual com a guanyador
        } else if (animal.position == maxPosition) {
            winners.push_back(&animal); // Afegeix a l'animal en cas d'empat
        }
    }

    // Mostra el resultat
    if (winners.size() > 1) {
        cout << "\nEmpat entre: ";
        for (const auto& winner : winners) {
            cout << winner->name << " ";
        }
        cout << "amb una posició de " << maxPosition << "!" << endl;
    } else {
        cout << "\nEl guanyador és " << winners[0]->name << " amb una posició de " << winners[0]->position << "!" << endl;
    }

    return 0;
}