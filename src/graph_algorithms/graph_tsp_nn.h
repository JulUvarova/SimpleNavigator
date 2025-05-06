#pragma once

#include <vector>
#include <limits>
#include <algorithm>
#include "../graph/graph.h"
#include "graph_algorithms.h"

/**
 * @brief Namespace for the Nearest Neighbor TSP solver.
 */
namespace s21_nn {

/**
 * @brief Solves the Traveling Salesman Problem using the Nearest Neighbor heuristic.
 *
 * This class implements a greedy approach where the algorithm always chooses
 * the closest unvisited city as the next city in the tour.
 */
class NearestNeighborSolver {
 public:
  /**
   * @brief Constructs a NearestNeighborSolver.
   * @param graph A reference to the graph object to solve TSP for.
   *              The graph should be complete and weighted for meaningful results.
   */
  explicit NearestNeighborSolver(s21_graph& graph) : graph_(graph) {}
  
  /**
   * @brief Runs the Nearest Neighbor algorithm to find a TSP tour.
   *
   * The tour starts at vertex 0, visits each other vertex by always moving to the
   * nearest unvisited neighbor, and finally returns to vertex 0.
   *
   * @return A TsmResult struct containing the constructed tour and its total distance.
   *         If the graph has 0 vertices, an empty tour with distance 0 is returned.
   *         If the graph has 1 vertex, a tour {0, 0} with distance 0 is returned.
   *         If at any point no unvisited reachable neighbor is found (for disconnected graphs),
   *         the tour might be incomplete and its validity depends on the graph structure.
   */
  TsmResult Run() {
    TsmResult result;
    result.distance = 0.0;  // Explicitly reset distance to zero
    result.vertices.clear(); // Ensure the vertices vector is empty
    
    int n = graph_.Size();
    
    if (n <= 1) {
      if (n == 1) {
        result.vertices = {0, 0};
        result.distance = 0.0;
      }
      return result;
    }
    
    // Start from vertex 0
    int current = 0;
    std::vector<bool> visited(n, false);
    visited[current] = true;
    result.vertices.push_back(current);
    
    // Visit all vertices
    for (int i = 1; i < n; ++i) {
      int next = FindNearestNeighbor(current, visited);
      if (next == -1) break;  // No reachable unvisited vertices
      
      visited[next] = true;
      result.vertices.push_back(next);
      
      // Add distance carefully, making sure we're getting the right edge weight
      double edge_weight = graph_(current, next);
      result.distance += edge_weight;
      
      current = next;
    }
    
    // Return to starting point to complete the tour
    if (result.vertices.size() > 1) {
      int first = result.vertices[0];
      result.vertices.push_back(first);
      
      // Add final edge weight to return to start
      double final_edge_weight = graph_(current, first);
      result.distance += final_edge_weight;
    }
    
    return result;
  }
  
 private:
  s21_graph& graph_; ///< A reference to the graph being processed.
  
  /**
   * @brief Finds the nearest unvisited neighbor to the current city.
   *
   * @param current The index of the current city.
   * @param visited A boolean vector indicating which cities have already been visited.
   * @return The index of the nearest unvisited neighbor. Returns -1 if no unvisited
   *         neighbor is found or if all reachable neighbors have been visited.
   */
  int FindNearestNeighbor(int current, const std::vector<bool>& visited) {
    int nearest = -1;
    int min_distance = std::numeric_limits<int>::max();
    
    for (int i = 0; i < graph_.Size(); ++i) {
      if (!visited[i] && graph_(current, i) > 0) {
        if (graph_(current, i) < min_distance) {
          min_distance = graph_(current, i);
          nearest = i;
        }
      }
    }
    
    return nearest;
  }
};

}  // namespace s21_nn 