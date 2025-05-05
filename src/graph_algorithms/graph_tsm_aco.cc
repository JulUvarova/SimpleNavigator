#include "graph_tsm_aco.h"

#include <cmath>      // For pow, abs, isinf, isnan
#include <iostream>   // For potential debugging output
#include <limits>     // For numeric_limits
#include <numeric>    // For std::accumulate
#include <stdexcept>  // For std::runtime_error

namespace s21_aco {

AntColonyOptimizer::AntColonyOptimizer(s21_graph& graph,
                                       const AcoParams& params)
    : graph_(graph),
      params_(params),
      num_cities_(graph.Size()),
      random_generator_(std::random_device{}()) {
  if (num_cities_ <= 0) {
    throw std::invalid_argument("Graph must contain at least one city.");
  }
  Initialize();
}

void AntColonyOptimizer::Initialize() {
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

TsmResult AntColonyOptimizer::Run() {
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

double AntColonyOptimizer::CalculateTourLength(
    const std::vector<int>& tour) const {
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

std::vector<int> AntColonyOptimizer::ConstructTourForAnt() {
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
      // If the graph can be incomplete, this needs robust handling.
      std::cerr << "Warning: Ant got stuck!" << std::endl;
      return {};  // Return empty tour to indicate failure
    }
    tour.push_back(next_city);
    visited[next_city] = true;
    current_city = next_city;
  }

  return tour;
}

int AntColonyOptimizer::SelectNextCity(int current_city,
                                       const std::vector<bool>& visited) {
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

void AntColonyOptimizer::UpdatePheromones(
    const std::vector<std::vector<int>>& ant_tours,
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

}  // namespace s21_aco