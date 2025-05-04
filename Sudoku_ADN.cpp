#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>

using namespace std;

// Definimos el tamaño del Sudoku (9x9)
const int SIZE = 9;

// Estructura para representar una solución de Sudoku como un "ADN"
struct SudokuSolution {
    vector<vector<int>> grid;
    int fitness;

    // Constructor para inicializar una solución aleatoria
    SudokuSolution() {
        grid.resize(SIZE, vector<int>(SIZE, 0));
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                grid[i][j] = rand() % SIZE + 1;  // Números aleatorios entre 1 y 9
            }
        }
        fitness = calculateFitness();
    }

    // Método para calcular la "aptitud" de la solución
    int calculateFitness() {
        int score = 0;
        
        // Verificar filas
        for (int i = 0; i < SIZE; i++) {
            vector<bool> seen(SIZE + 1, false);
            for (int j = 0; j < SIZE; j++) {
                if (seen[grid[i][j]]) {
                    score--;
                }
                seen[grid[i][j]] = true;
            }
        }

        // Verificar columnas
        for (int j = 0; j < SIZE; j++) {
            vector<bool> seen(SIZE + 1, false);
            for (int i = 0; i < SIZE; i++) {
                if (seen[grid[i][j]]) {
                    score--;
                }
                seen[grid[i][j]] = true;
            }
        }

        // Verificar subcuadrículas de 3x3
        for (int r = 0; r < SIZE; r += 3) {
            for (int c = 0; c < SIZE; c += 3) {
                vector<bool> seen(SIZE + 1, false);
                for (int i = r; i < r + 3; i++) {
                    for (int j = c; j < c + 3; j++) {
                        if (seen[grid[i][j]]) {
                            score--;
                        }
                        seen[grid[i][j]] = true;
                    }
                }
            }
        }

        return score;
    }

    // Método para imprimir el Sudoku
    void printGrid() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                cout << grid[i][j] << " ";
            }
            cout << endl;
        }
    }

    // Método para mutar la solución
    void mutate() {
        int row = rand() % SIZE;
        int col = rand() % SIZE;
        grid[row][col] = rand() % SIZE + 1;
        fitness = calculateFitness();
    }

    // Método para cruzar dos soluciones
    SudokuSolution crossover(const SudokuSolution& other) {
        SudokuSolution child;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                // Cruzamos las soluciones mezclando filas de ambos padres
                if (rand() % 2 == 0) {
                    child.grid[i][j] = grid[i][j];
                } else {
                    child.grid[i][j] = other.grid[i][j];
                }
            }
        }
        child.fitness = child.calculateFitness();
        return child;
    }
};

// Función para generar una población inicial
vector<SudokuSolution> generatePopulation(int populationSize) {
    vector<SudokuSolution> population;
    for (int i = 0; i < populationSize; i++) {
        population.push_back(SudokuSolution());
    }
    return population;
}

// Función para seleccionar los dos mejores individuos
pair<SudokuSolution, SudokuSolution> selectParents(const vector<SudokuSolution>& population) {
    SudokuSolution parent1 = population[0];
    SudokuSolution parent2 = population[1];

    for (const auto& individual : population) {
        if (individual.fitness > parent1.fitness) {
            parent1 = individual;
        }
    }

    for (const auto& individual : population) {
        if (individual.fitness > parent2.fitness && individual.fitness != parent1.fitness) {
            parent2 = individual;
        }
    }

    return {parent1, parent2};
}

int main() {
    srand(time(0));

    const int populationSize = 100;
    const int generations = 1000;

    // Población inicial
    vector<SudokuSolution> population = generatePopulation(populationSize);

    // Evolución
    for (int generation = 0; generation < generations; generation++) {
        // Selección de padres
        auto parents = selectParents(population);
        SudokuSolution parent1 = parents.first;
        SudokuSolution parent2 = parents.second;

        // Cruce y mutación
        SudokuSolution child = parent1.crossover(parent2);
        child.mutate();

        // Reemplazo de un individuo con peor fitness
        auto worstIt = min_element(population.begin(), population.end(), [](const SudokuSolution& a, const SudokuSolution& b) {
            return a.fitness < b.fitness;
        });
        *worstIt = child;

        // Imprimir solución si hemos encontrado una solución perfecta
        if (child.fitness == SIZE * SIZE) {
            cout << "¡Solución encontrada en la generación " << generation + 1 << "!" << endl;
            child.printGrid();
            break;
        }

        if (generation % 100 == 0) {
            cout << "Generación: " << generation + 1 << " | Mejor aptitud: " << parent1.fitness << endl;
        }
    }

    return 0;
}
