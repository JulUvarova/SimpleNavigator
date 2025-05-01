#pragma once

#include <vector>
#include <random>
#include "../graph/graph.h" 
#include "graph_algorithms.h" 

namespace s21_aco {

// Structure for ACO parameters
struct AcoParams {
    int num_ants = 10;
    int num_iterations = 100;
    double alpha = 1.0; // Pheromone influence
    double beta = 2.0;  // Heuristic influence (distance)
    double evaporation_rate = 0.5; // Rho
    double Q = 100.0; // Pheromone deposit factor
    double initial_pheromone = 0.1;
};

class AntColonyOptimizer {
public:
    AntColonyOptimizer(s21_graph& graph, const AcoParams& params = AcoParams());

    TsmResult Run(); 

private:
    s21_graph& graph_;
    const AcoParams params_;
    int num_cities_;
    std::vector<std::vector<double>> pheromones_;
    std::vector<std::vector<double>> heuristic_info_;
    std::mt19937 random_generator_; // Random number generator

    void Initialize();
    std::vector<int> ConstructTourForAnt();
    void UpdatePheromones(const std::vector<std::vector<int>>& ant_tours,
                          const std::vector<double>& ant_tour_lengths);
    double CalculateTourLength(const std::vector<int>& tour) const;
    int SelectNextCity(int current_city, const std::vector<bool>& visited);
};

} // namespace s21_aco

