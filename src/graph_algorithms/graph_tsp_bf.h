#pragma once

#include <vector>
#include <limits>
#include <algorithm>
#include "../graph/graph.h"
#include "graph_algorithms.h"

/**
 * @brief Namespace for the Brute Force TSP optimizer.
 */
namespace s21_bf {

/**
 * @brief Solves the Traveling Salesman Problem using a Brute Force approach.
 *
 * This class explores all possible permutations of city visits to find the
 * absolute shortest tour. It is guaranteed to find the optimal solution
 * but is computationally expensive and only feasible for small graphs.
 */
class BruteForceOptimizer {
 public:
  /**
   * @brief Constructs a BruteForceOptimizer.
   * @param graph A reference to the graph object to solve TSP for.
   *              The graph should be complete and weighted for meaningful results.
   */
  explicit BruteForceOptimizer(s21_graph& graph) : graph_(graph) {}
  
  /**
   * @brief Runs the Brute Force algorithm to find the optimal TSP tour.
   *
   * The algorithm generates all permutations of visiting vertices (starting from vertex 0),
   * calculates the distance for each permutation, and returns the shortest one found.
   *
   * @return A TsmResult struct containing the optimal tour and its total distance.
   *         If the graph has 0 vertices, an empty tour with infinite distance is returned (or 0 if n=1).
   *         If the graph has 1 vertex, a tour {0, 0} with distance 0 is returned.
   *         If any tour permutation involves an invalid (non-existent or zero-weight)
   *         edge, that permutation is considered to have infinite distance.
   */
  TsmResult Run() {
    TsmResult result;
    result.distance = std::numeric_limits<double>::infinity();
    
    int n = graph_.Size();
    if (n <= 1) {
      if (n == 1) {
        result.vertices = {0, 0};
        result.distance = 0.0;
      }
      return result;
    }
    
    // Create a vector with all vertices except the first one
    std::vector<int> vertices;
    for (int i = 1; i < n; ++i) {
      vertices.push_back(i);
    }
    
    // Always start from vertex 0
    std::vector<int> best_route;
    double min_distance = std::numeric_limits<double>::infinity();
    
    // Try all permutations of the vertices
    do {
      std::vector<int> current_route = {0};  // Start from vertex 0
      current_route.insert(current_route.end(), vertices.begin(), vertices.end());
      current_route.push_back(0);  // Return to vertex 0
      
      double current_distance = CalculateRouteDistance(current_route);
      
      if (current_distance < min_distance) {
        min_distance = current_distance;
        best_route = current_route;
      }
      
    } while (std::next_permutation(vertices.begin(), vertices.end()));
    
    result.vertices = best_route;
    result.distance = min_distance;
    
    return result;
  }
  
 private:
  s21_graph& graph_; ///< A reference to the graph being processed.
  
  /**
   * @brief Calculates the total distance of a given route (permutation of vertices).
   *
   * @param route A vector of vertex indices representing the tour. The tour is assumed
   *              to start and end at the first vertex in its sequence, but the last vertex
   *              in the input `route` vector must explicitly be the starting vertex to close the loop.
   * @return The total distance of the route. Returns `std::numeric_limits<double>::infinity()`
   *         if the route is invalid (e.g., contains a non-existent edge or an edge with non-positive weight).
   */
  double CalculateRouteDistance(const std::vector<int>& route) {
    double distance = 0.0;
    
    for (size_t i = 0; i < route.size() - 1; ++i) {
      int current = route[i];
      int next = route[i + 1];
      
      // Check if there's an edge between current and next
      double edge_weight = graph_(current, next);
      if (edge_weight <= 0) {
        return std::numeric_limits<double>::infinity();  // Invalid route
      }
      
      distance += edge_weight;
    }
    
    return distance;
  }
};

}  // namespace s21_bf 