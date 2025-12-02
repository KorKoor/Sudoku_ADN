#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include <set>
#include <locale.h>

using namespace std;

const int SIZE = 9;
const int BLOCK_SIZE = 3;
const int POPULATION_SIZE = 5000;
const int MAX_GENERATIONS = 50000;
const double MUTATION_RATE = 0.05;

vector< vector<int> > initial_puzzle_ref;

struct SudokuSolution {
    vector< vector<int> > grid;
    int fitness;

    SudokuSolution() {
        grid = initial_puzzle_ref;

        for (int i = 0; i < SIZE; i++) {
            set<int> fixed_numbers;
            vector<int> empty_cells_j;

            for (int j = 0; j < SIZE; j++) {
                if (grid[i][j] != 0) {
                    fixed_numbers.insert(grid[i][j]);
                } else {
                    empty_cells_j.push_back(j);
                }
            }

            vector<int> missing_numbers;
            for (int k = 1; k <= SIZE; k++) {
                if (fixed_numbers.find(k) == fixed_numbers.end()) {
                    missing_numbers.push_back(k);
                }
            }

            random_shuffle(missing_numbers.begin(), missing_numbers.end());
            for (size_t k = 0; k < empty_cells_j.size(); k++) {
                grid[i][empty_cells_j[k]] = missing_numbers[k];
            }
        }
        fitness = calculateFitness();
    }

    // Evaluación de aptitud 
    int calculateFitness() const {
        int conflicts = 0;

        // Columnas
        for (int j = 0; j < SIZE; j++) {
            set<int> seen;
            for (int i = 0; i < SIZE; i++) {
                if (seen.count(grid[i][j])) {
                    conflicts++;
                } else {
                    seen.insert(grid[i][j]);
                }
            }
        }

        // Bloques 3x3
        for (int r = 0; r < SIZE; r += BLOCK_SIZE) {
            for (int c = 0; c < SIZE; c += BLOCK_SIZE) {
                set<int> seen;
                for (int i = r; i < r + BLOCK_SIZE; i++) {
                    for (int j = c; j < c + BLOCK_SIZE; j++) {
                        if (seen.count(grid[i][j])) {
                            conflicts++;
                        } else {
                            seen.insert(grid[i][j]);
                        }
                    }
                }
            }
        }
        return conflicts;
    }

    // Imprimir nuestro Sudoku
    void printGrid() const {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                cout << grid[i][j]
                     << ((j + 1) % BLOCK_SIZE == 0 && j != SIZE - 1 ? " | " : " ");
            }
            cout << endl;
            if ((i + 1) % BLOCK_SIZE == 0 && i != SIZE - 1) {
                cout << "---------------------" << endl;
            }
        }
    }
};

// Cruce
SudokuSolution crossover(const SudokuSolution& parent1, const SudokuSolution& parent2) {
    SudokuSolution child = parent1;
    int crossover_point = rand() % SIZE;

    for (int i = 0; i < SIZE; i++) {
        if (i > crossover_point) {
            child.grid[i] = parent2.grid[i];
        }
    }
    child.fitness = child.calculateFitness();
    return child;
}

// Mutación 
void mutate(SudokuSolution& individual) {
    if ((double)rand() / RAND_MAX > MUTATION_RATE) {
        return;
    }

    int row = rand() % SIZE;
    vector<int> mutable_cols;
    for (int j = 0; j < SIZE; j++) {
        if (initial_puzzle_ref[row][j] == 0) {
            mutable_cols.push_back(j);
        }
    }

    if (mutable_cols.size() >= 2) {
        int idx1 = rand() % mutable_cols.size();
        int idx2 = rand() % mutable_cols.size();
        while (idx1 == idx2) { idx2 = rand() % mutable_cols.size(); }

        swap(individual.grid[row][mutable_cols[idx1]], individual.grid[row][mutable_cols[idx2]]);
        individual.fitness = individual.calculateFitness();
    }
}

// Selección por torneo 
SudokuSolution tournamentSelection(const vector<SudokuSolution>& population, int tournamentSize = 5) {
    SudokuSolution best = population[rand() % POPULATION_SIZE];
    for (int i = 1; i < tournamentSize; i++) {
        const SudokuSolution& current = population[rand() % POPULATION_SIZE];
        if (current.fitness < best.fitness) {
            best = current;
        }
    }
    return best;
}

int main() {
    setlocale(LC_ALL, ""); // Para mostrar acentos correctamente
    srand(time(0));
    cout << "--- Algoritmo Genético para Sudoku ---\n" << endl;

    // Sudoku inicial
    int puzzle[9][9] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };

    for (int i = 0; i < SIZE; i++) {
        vector<int> row;
        for (int j = 0; j < SIZE; j++) {
            row.push_back(puzzle[i][j]);
        }
        initial_puzzle_ref.push_back(row);
    }

    cout << "Puzzle Inicial:" << endl;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            cout << initial_puzzle_ref[i][j]
                 << ((j + 1) % BLOCK_SIZE == 0 && j != SIZE - 1 ? " | " : " ");
        }
        cout << endl;
        if ((i + 1) % BLOCK_SIZE == 0 && i != SIZE - 1) {
            cout << "---------------------" << endl;
        }
    }
    cout << endl;

    // Generar población inicial
    vector<SudokuSolution> population;
    for (int i = 0; i < POPULATION_SIZE; i++) {
        population.push_back(SudokuSolution());
    }

    SudokuSolution bestSolution = population[0];
    int totalMutations = 0;
    int generationsUsed = 0;

    for (int generation = 0; generation < MAX_GENERATIONS; generation++) {
    generationsUsed = generation;

    for (size_t i = 0; i < population.size(); i++) {
        if (population[i].fitness < bestSolution.fitness) {
            bestSolution = population[i];
        }
    }

    if (bestSolution.fitness == 0) {
        cout << "¡SOLUCIÓN ENCONTRADA en la Generación: " << generation + 1 << "!" << endl;
        break;
    }

    vector<SudokuSolution> newPopulation;
    newPopulation.push_back(bestSolution);

    while (newPopulation.size() < POPULATION_SIZE) {
        SudokuSolution parent1 = tournamentSelection(population);
        SudokuSolution parent2 = tournamentSelection(population);

        SudokuSolution child = crossover(parent1, parent2);

        int beforeFitness = child.fitness;
        mutate(child);
        if (child.fitness != beforeFitness) {
            totalMutations++;
        }

        newPopulation.push_back(child);
    }

    population = newPopulation;
	}

	// Resumen final 
	cout << "\n Resumen Final" << endl;
	cout << "Generaciones ejecutadas: " << generationsUsed + 1 << endl;
	cout << "Mutaciones totales: " << totalMutations << endl;

	cout << "\nSudoku resultante:" << endl;
	bestSolution.printGrid();
}
