#pragma once

#include <vector>
#include <limits>
#include <algorithm>
#include "../graph/graph.h"
#include "graph_algorithms.h"

namespace s21_nn {

class NearestNeighborSolver {
 public:
  explicit NearestNeighborSolver(s21_graph& graph) : graph_(graph) {}
  
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
  s21_graph& graph_;
  
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