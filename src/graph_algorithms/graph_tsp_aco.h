#pragma once

#include <random>
#include <vector>
#include <cmath>      // For pow, abs, isinf, isnan
#include <iostream>   // For potential debugging output
#include <limits>     // For numeric_limits
#include <numeric>    // For std::accumulate
#include <stdexcept>  // For std::runtime_error

#include "../graph/graph.h"
#include "graph_algorithms.h"

/**
 * @brief Namespace for the Ant Colony Optimization TSP solver.
 */
namespace s21_aco {

/**
 * @brief Structure to hold parameters for the Ant Colony Optimization algorithm.
 */
struct AcoParams {
  int num_ants = 10;              ///< Number of ants in the colony.
  int num_iterations = 100;       ///< Number of iterations the algorithm will run.
  double alpha = 1.0;             ///< Influence of pheromone trails (higher alpha means more pheromone influence).
  double beta = 2.0;              ///< Influence of heuristic information (e.g., inverse of distance).
  double evaporation_rate = 0.5;  ///< Rate at which pheromones evaporate (rho, between 0 and 1).
  double Q = 100.0;               ///< Pheromone deposit factor (total amount of pheromone deposited by ants).
  double initial_pheromone = 0.1; ///< Initial pheromone level on all paths.
};

/**
 * @brief Solves the Traveling Salesman Problem using the Ant Colony Optimization (ACO) metaheuristic.
 *
 * Ants construct tours by probabilistically choosing paths based on pheromone levels
 * and heuristic information (typically inverse distance). Pheromones are updated
 * after each iteration, reinforcing paths that are part of shorter tours.
 */
class AntColonyOptimizer {
 public:
  /**
   * @brief Constructs an AntColonyOptimizer.
   * @param graph A reference to the graph object to solve TSP for.
   *              The graph should ideally be complete and weighted for meaningful results.
   * @param params ACO algorithm parameters. Defaults will be used if not provided.
   * @throw std::invalid_argument if the graph contains no cities (vertices).
   */
  AntColonyOptimizer(s21_graph& graph, const AcoParams& params = AcoParams())
      : graph_(graph),
        params_(params),
        num_cities_(graph.Size()),
        random_generator_(std::random_device{}()) {
    if (num_cities_ <= 0) {
      throw std::invalid_argument("Graph must contain at least one city.");
    }
    Initialize();
  }

  /**
   * @brief Runs the Ant Colony Optimization algorithm.
   *
   * Iteratively, ants construct tours, and pheromone trails are updated.
   * The best tour found across all ants and iterations is returned.
   *
   * @return A TsmResult struct containing the best tour found and its distance.
   *         The tour in TsmResult will have the starting node appended at the end to represent a full cycle.
   * @throw std::runtime_error if ACO fails to find any valid tour (e.g., in a disconnected graph
   *         where ants cannot complete a tour of all cities).
   */
  TsmResult Run() {
    TsmResult best_result;
    best_result.vertices = {};
    best_result.distance = std::numeric_limits<double>::max();

    for (int iter = 0; iter < params_.num_iterations; ++iter) {
      std::vector<std::vector<int>> ant_tours(params_.num_ants);
      std::vector<double> ant_tour_lengths(params_.num_ants);

      for (int k = 0; k < params_.num_ants; ++k) {
        ant_tours[k] = ConstructTourForAnt();
        if (ant_tours[k].empty()) continue;  // Skip if ant got stuck

        ant_tour_lengths[k] = CalculateTourLength(ant_tours[k]);

        if (ant_tour_lengths[k] < best_result.distance) {
          best_result.distance = ant_tour_lengths[k];
          best_result.vertices = ant_tours[k];
          // std::cout << "Iter " << iter << ", Ant " << k << ": New best tour
          // length: " << best_result.distance << std::endl;
        }
      }

      UpdatePheromones(ant_tours, ant_tour_lengths);
    }

    // Ensure the result tour is valid (not empty)
    if (best_result.vertices.empty() && num_cities_ > 0) {
      throw std::runtime_error("ACO could not find a valid tour.");
    }

    // Add the starting node to the end to represent the full cycle for TsmResult
    if (!best_result.vertices.empty()) {
      best_result.vertices.push_back(best_result.vertices.front());
    }

    return best_result;
  }

 private:
  s21_graph& graph_;                          ///< Reference to the graph.
  const AcoParams params_;                    ///< ACO parameters.
  int num_cities_;                            ///< Number of cities (vertices) in the graph.
  std::vector<std::vector<double>> pheromones_; ///< Matrix storing pheromone levels between cities.
  std::vector<std::vector<double>> heuristic_info_; ///< Matrix storing heuristic information (e.g., 1/distance).
  std::mt19937 random_generator_;             ///< Random number generator for probabilistic choices.

  /**
   * @brief Initializes pheromone trails and heuristic information.
   *
   * Pheromones are set to an initial value. Heuristic information is calculated typically
   * as the inverse of the distance between cities. Handles cases for non-existent edges
   * or self-loops.
   */
  void Initialize() {
    pheromones_.assign(
        num_cities_, std::vector<double>(num_cities_, params_.initial_pheromone));
    heuristic_info_.assign(num_cities_, std::vector<double>(num_cities_, 0.0));

    for (int i = 0; i < num_cities_; ++i) {
      for (int j = 0; j < num_cities_; ++j) {
        if (i == j) {
          pheromones_[i][j] = 0.0;  // No pheromone on self-loops
          heuristic_info_[i][j] = 0.0;
        } else {
          double dist = graph_(i, j);
          if (dist > 0 && !std::isinf(dist)) {
            heuristic_info_[i][j] = 1.0 / dist;
          } else {
            // Handle non-existent edges or zero distance (problematic for TSP)
            // Set a very small heuristic value or handle it in SelectNextCity
            heuristic_info_[i][j] =
                1e-9;  // Avoid division by zero, low attractiveness
            pheromones_[i][j] =
                0.0;  // Optionally zero pheromone if no direct path
          }
        }
      }
    }
  }

  /**
   * @brief Constructs a tour for a single ant.
   *
   * The ant starts at a random city and iteratively chooses the next city based on
   * pheromone levels and heuristic information until all cities are visited.
   *
   * @return A vector of city indices representing the tour constructed by the ant.
   *         The tour does NOT include the return to the start city here; that's handled
   *         by CalculateTourLength and the final TsmResult.
   *         Returns an empty vector if the ant gets stuck (should not happen in a fully connected graph).
   */
  std::vector<int> ConstructTourForAnt() {
    std::vector<int> tour;
    tour.reserve(num_cities_);
    std::vector<bool> visited(num_cities_, false);

    // Start at a random city
    std::uniform_int_distribution<> start_dist(0, num_cities_ - 1);
    int current_city = start_dist(random_generator_);
    tour.push_back(current_city);
    visited[current_city] = true;

    while (tour.size() < num_cities_) {
      int next_city = SelectNextCity(current_city, visited);
      if (next_city == -1) {
        // Ant got stuck - should not happen in a complete graph TSP
        return {};  // Return empty tour to indicate failure
      }
      tour.push_back(next_city);
      visited[next_city] = true;
      current_city = next_city;
    }

    return tour;
  }

  /**
   * @brief Updates pheromone trails after all ants have completed their tours in an iteration.
   *
   * Pheromones evaporate on all paths, and then ants deposit new pheromones on the
   * paths they traversed, proportional to the quality (inverse length) of their tours.
   *
   * @param ant_tours A vector of tours, where each tour is a vector of city indices.
   * @param ant_tour_lengths A vector of corresponding lengths for each tour in ant_tours.
   */
  void UpdatePheromones(const std::vector<std::vector<int>>& ant_tours,
                        const std::vector<double>& ant_tour_lengths) {
    // 1. Evaporation
    for (int i = 0; i < num_cities_; ++i) {
      for (int j = i + 1; j < num_cities_;
          ++j) {  // Iterate only upper triangle for symmetric TSP
        pheromones_[i][j] *= (1.0 - params_.evaporation_rate);
        pheromones_[j][i] = pheromones_[i][j];
      }
    }

    // 2. Deposition
    for (size_t k = 0; k < ant_tours.size(); ++k) {
      const auto& tour = ant_tours[k];
      if (tour.empty()) continue;  // Skip invalid tours

      double tour_length = ant_tour_lengths[k];
      if (tour_length <= 0 || std::isinf(tour_length) ||
          std::isnan(tour_length)) {
        // std::cerr << "Warning: Skipping ant with invalid tour length: " <<
        // tour_length << std::endl;
        continue;  // Skip invalid tour lengths
      }

      double pheromone_deposit = params_.Q / tour_length;

      for (size_t i = 0; i < tour.size() - 1; ++i) {
        int city1 = tour[i];
        int city2 = tour[i + 1];
        pheromones_[city1][city2] += pheromone_deposit;
        pheromones_[city2][city1] = pheromones_[city1][city2];
      }
      // Deposit on the return edge
      int last_city = tour.back();
      int first_city = tour.front();
      pheromones_[last_city][first_city] += pheromone_deposit;
      pheromones_[first_city][last_city] = pheromones_[last_city][first_city];
    }
  }

  /**
   * @brief Calculates the total length of a given tour.
   *
   * The tour is a sequence of cities. This function sums the distances between
   * consecutive cities in the tour and adds the distance from the last city back to the first.
   *
   * @param tour A vector of city indices representing the tour.
   * @return The total length of the tour. Returns `std::numeric_limits<double>::infinity()`
   *         if the tour is invalid (e.g., less than 2 cities, or contains an invalid edge).
   */
  double CalculateTourLength(const std::vector<int>& tour) const {
    double length = 0.0;
    if (tour.size() < 2)
      return std::numeric_limits<double>::infinity();  // Not a valid tour

    for (size_t i = 0; i < tour.size() - 1; ++i) {
      double dist = graph_(tour[i], tour[i + 1]);
      if (dist <= 0 || std::isinf(dist)) {
        return std::numeric_limits<double>::infinity();  // Invalid edge in tour
      }
      length += dist;
    }
    // Add distance from last city back to the first
    double return_dist = graph_(tour.back(), tour.front());
    if (return_dist <= 0 || std::isinf(return_dist)) {
      return std::numeric_limits<double>::infinity();  // Invalid return edge
    }
    length += return_dist;
    return length;
  }

  /**
   * @brief Selects the next city for an ant based on pheromone and heuristic information.
   *
   * Uses a probabilistic selection rule (roulette wheel selection) where cities with
   * higher pheromone levels and better heuristic values (shorter distances) are more likely
   * to be chosen.
   *
   * @param current_city The city the ant is currently at.
   * @param visited A boolean vector indicating which cities have already been visited by this ant.
   * @return The index of the next city to visit. Returns -1 if no unvisited city can be reached.
   */
  int SelectNextCity(int current_city, const std::vector<bool>& visited) {
    double total_attractiveness = 0.0;
    std::vector<double> probabilities;
    std::vector<int> allowed_cities;
    probabilities.reserve(num_cities_);
    allowed_cities.reserve(num_cities_);

    for (int next_city = 0; next_city < num_cities_; ++next_city) {
      if (!visited[next_city]) {
        double pheromone_level = pheromones_[current_city][next_city];
        double heuristic_level = heuristic_info_[current_city][next_city];

        // Consider only reachable cities with some attractiveness
        if (pheromone_level > 1e-9 ||
            heuristic_level > 1e-9) {  // Use a small threshold
          double attractiveness = std::pow(pheromone_level, params_.alpha) *
                                  std::pow(heuristic_level, params_.beta);
          if (attractiveness < 1e-9) {
            attractiveness = 1e-9;  // Prevent zero probability if possible
          }
          probabilities.push_back(attractiveness);
          allowed_cities.push_back(next_city);
          total_attractiveness += attractiveness;
        }
      }
    }

    if (allowed_cities.empty()) {
      return -1;  // No valid next city found
    }

    // Roulette wheel selection
    std::uniform_real_distribution<> dist(0.0, total_attractiveness);
    double roulette_spin = dist(random_generator_);
    double cumulative_prob = 0.0;

    for (size_t i = 0; i < allowed_cities.size(); ++i) {
      cumulative_prob += probabilities[i];
      if (roulette_spin <= cumulative_prob) {
        return allowed_cities[i];
      }
    }

    // In case of floating point issues
    return allowed_cities.back();
  }
};

}  // namespace s21_aco
