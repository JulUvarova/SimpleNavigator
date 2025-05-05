#pragma once

#include <vector>
#include <limits>
#include <algorithm>
#include "../graph/graph.h"
#include "graph_algorithms.h"

namespace s21_bf {

class BruteForceOptimizer {
 public:
  explicit BruteForceOptimizer(s21_graph& graph) : graph_(graph) {}
  
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
  s21_graph& graph_;
  
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