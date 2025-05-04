#include "graph_algorithms.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

TEST(GraphAlgorithmsTest, ConnectedDirectedWeighted) {
  s21_graph graph;
  std::string filename = "test_graph.txt";

  {
    std::ofstream file(filename);
    file << "0 5 0 0 0 0 \n"
            "0 0 3 0 0 0 \n"
            "0 0 0 0 0 4 \n"
            "2 0 0 0 7 0 \n"
            "0 0 0 0 0 1 \n"
            "0 0 0 0 0 0 \n";
  }
  graph.LoadFromFile(filename);
  graph.PrintGraph();
  std::filesystem::remove(filename);

  auto dfs = s21_graph_algorithms::DepthFirstSearch(graph, 1);
  auto bfs = s21_graph_algorithms::BreadthFirstSearch(graph, 1);
  auto shortest_path_dijkstra =
      s21_graph_algorithms::GetShortestPathBetweenVertices(graph, 3, 2);
  auto shortest_path_floyd_warshall =
      s21_graph_algorithms::GetShortestPathsBetweenAllVertices(graph);
  // auto prim_tree = s21_graph_algorithms::GetLeastSpanningTree(graph);

  EXPECT_EQ(dfs.size(), 3);
  EXPECT_EQ(bfs.size(), 3);
  EXPECT_EQ(shortest_path_dijkstra.first, 10);
  EXPECT_EQ(shortest_path_dijkstra.second.size(), 4);

}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
