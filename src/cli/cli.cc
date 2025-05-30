#include "cli.h"

void CLInterface::Loop() {
  int choice = -1;
  do {
    PrintMenu();
    std::cin >> choice;
    if (std::cin.fail()) choice = -1;
    CleanInput();

    switch (choice) {
      case 1:
        LoadGraphFromFile();
        break;
      case 2:
        BFS();
        break;
      case 3:
        DFS();
        break;
      case 4:
        ShortestPathBetweenVertices();
        break;
      case 5:
        ShortestPathBetweenAllVertices();
        break;
      case 6:
        MinimumSpanningTree();
        break;
      case 7:
        SalesmanProblem();
        break;
      case 8:
        SalesmanProblemAnalysis();
        break;
      case 0:
        PrintInformation("Exiting the program");
        break;
      default:
        PrintWarning("Invalid command");
        break;
    }
  } while (choice != 0);
}

void CLInterface::CleanInput() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void CLInterface::SalesmanProblemAnalysis() {
  if (!CheckGraph()) return;

  try {
    int kIterations = 1000;

    std::cout << "Analyzing TSP algorithms for " << kIterations
              << " iterations..." << std::endl;

    // Check graph size for brute force warning
    if (graph_.Size() > 11) {
      std::cout
          << "Warning: Brute Force algorithm may take a very long time with "
          << graph_.Size() << " vertices!" << std::endl;
    }

    // Run each algorithm once first to verify correctness
    std::cout << "\nVerifying algorithms produce correct results:" << std::endl;

    // ACO
    TsmResult aco_result = s21_graph_algorithms::SolveTravelingSalesmanProblem(
        graph_, TSPAlgorithm::ACO);
    std::cout << "ACO Route length: " << aco_result.distance
              << ", Vertices: " << aco_result.vertices.size() << std::endl;

    // NN
    TsmResult nn_result = s21_graph_algorithms::SolveTravelingSalesmanProblem(
        graph_, TSPAlgorithm::NEAREST_NEIGHBOR);
    std::cout << "NN Route length: " << nn_result.distance
              << ", Vertices: " << nn_result.vertices.size() << std::endl;

    // BF (only for small graphs)
    TsmResult bf_result;
    bool bf_enabled = graph_.Size() <= 11;  // Practical limit for brute force
    if (bf_enabled) {
      bf_result = s21_graph_algorithms::SolveTravelingSalesmanProblem(
          graph_, TSPAlgorithm::BRUTE_FORCE);
      std::cout << "BF Route length: " << bf_result.distance
                << ", Vertices: " << bf_result.vertices.size() << std::endl;
    } else {
      std::cout << "BF: Skipped (graph too large)" << std::endl;
    }

    std::cout << "\nBeginning performance analysis:" << std::endl;

    // Variables to store execution times
    double aco_time = 0.0;
    double nn_time = 0.0;
    double bf_time = 0.0;

    // Test Ant Colony Optimization
    std::cout << "Testing Ant Colony Optimization:" << std::endl;
    s21::Timer::Start();
    for (int i = 0; i < kIterations; ++i) {
      TsmResult result = s21_graph_algorithms::SolveTravelingSalesmanProblem(
          graph_, TSPAlgorithm::ACO);
      // Use result to prevent compiler optimization
      if (i == kIterations - 1 && result.distance != aco_result.distance) {
        std::cout << "Warning: Inconsistent ACO results" << std::endl;
      }
    }
    s21::Timer::Stop();
    aco_time = s21::Timer::GetElapsedTimeMs();

    // Test Nearest Neighbor
    std::cout << "Testing Nearest Neighbor:" << std::endl;
    s21::Timer::Start();
    for (int i = 0; i < kIterations; ++i) {
      TsmResult result = s21_graph_algorithms::SolveTravelingSalesmanProblem(
          graph_, TSPAlgorithm::NEAREST_NEIGHBOR);
      // Use result to prevent compiler optimization
      if (i == kIterations - 1 && result.distance != nn_result.distance) {
        std::cout << "Warning: Inconsistent NN results" << std::endl;
      }
    }
    s21::Timer::Stop();
    nn_time = s21::Timer::GetElapsedTimeMs();

    // Test Brute Force
    if (bf_enabled) {
      std::cout << "Testing Brute Force:" << std::endl;
      s21::Timer::Start();
      for (int i = 0; i < kIterations; ++i) {
        TsmResult result = s21_graph_algorithms::SolveTravelingSalesmanProblem(
            graph_, TSPAlgorithm::BRUTE_FORCE);
        // Use result to prevent compiler optimization
        if (i == kIterations - 1 && result.distance != bf_result.distance) {
          std::cout << "Warning: Inconsistent BF results" << std::endl;
        }
      }
      s21::Timer::Stop();
      bf_time = s21::Timer::GetElapsedTimeMs();
    } else {
      std::cout << "Skipping Brute Force test - graph too large." << std::endl;
    }

    // Calculate average time per iteration
    aco_time /= kIterations;
    nn_time /= kIterations;
    if (bf_enabled) {
      bf_time /= kIterations;
    }

    std::cout << "\nAlgorithm comparison summary:" << std::endl;
    std::cout << "Algorithm       | Route Length | Avg Time (ms)" << std::endl;
    std::cout << "----------------|--------------|-------------" << std::endl;
    std::cout << "ACO             | " << std::setw(12) << aco_result.distance
              << " | " << std::setw(12) << aco_time << std::endl;
    std::cout << "Nearest Neighbor| " << std::setw(12) << nn_result.distance
              << " | " << std::setw(12) << nn_time << std::endl;
    if (bf_enabled) {
      std::cout << "Brute Force     | " << std::setw(12) << bf_result.distance
                << " | " << std::setw(12) << bf_time << std::endl;
    }
  } catch (std::exception& e) {
    PrintWarning(e.what());
  }
}

void CLInterface::SalesmanProblem() {
  if (!CheckGraph()) return;

  try {
    std::cout << "Working time for ant colony algorithm:" << std::endl;
    s21::Timer::Start();
    TsmResult result = s21_graph_algorithms::SolveTravelingSalesmanProblem(
        graph_, TSPAlgorithm::ACO);
    s21::Timer::Stop();

    // Print the result
    if (result.distance != std::numeric_limits<double>::infinity() &&
        !result.vertices.empty()) {
      std::cout << "Found route (ant colony algorithm):" << std::endl;
      std::string path_str = "";
      PrintPath(path_str, result.vertices);
      std::cout << "Route length: " +
                       std::to_string(static_cast<int>(result.distance))
                << std::endl;
    } else {
      PrintWarning("Failed to find a valid traveling salesman route.");
    }

  } catch (const std::exception& e) {
    PrintWarning(e.what());
  }
}

void CLInterface::PrintPath(std::string& path_str,
                            const std::vector<int>& result) {
  for (size_t i = 0; i < result.size(); ++i) {
    path_str += std::to_string(result[i] + 1);
    if (i < result.size() - 1) {
      path_str += " -> ";
    }
  }
  std::cout << path_str << "\n";
}

void CLInterface::MinimumSpanningTree() {
  if (!CheckGraph()) return;

  try {
    auto res = s21_graph_algorithms::GetLeastSpanningTree(graph_);

    std::cout << "Adjacency matrix for the minimum spanning tree: "
              << std::endl;
    PrintMatrix(res.second);
    std::cout << "Weight of the minimum spanning tree: " << res.first
              << std::endl;
  } catch (std::exception& e) {
    PrintWarning(e.what());
  }
}

void CLInterface::ShortestPathBetweenAllVertices() {
  if (!CheckGraph()) return;

  try {
    std::vector<std::vector<int>> res =
        s21_graph_algorithms::GetShortestPathsBetweenAllVertices(graph_);

    std::cout << "Distance matrix with shortest paths between all vertices: "
              << std::endl;
    PrintMatrix(res);
  } catch (std::exception& e) {
    PrintWarning(e.what());
  }
}

void CLInterface::PrintMatrix(const std::vector<std::vector<int>>& matrix) {
  std::cout << std::setw(3) << "   |";
  for (int i = 0; i < matrix.size(); ++i) {
    std::cout << std::setw(3) << i + 1 << "|";
  }
  std::cout << std::endl;
  for (int j = 0; j <= matrix.size(); ++j) {
    std::cout << "---+";
  }
  std::cout << std::endl;
  for (int i = 0; i < matrix.size(); ++i) {
    std::cout << std::setw(3) << i + 1 << "|";
    for (int j = 0; j < matrix[i].size(); ++j) {
      std::cout << std::setw(3) << matrix[i][j];
      std::cout << (j == matrix[i].size() - 1 ? "|" : " ");
    }
    std::cout << std::endl;
    for (int j = 0; j <= matrix[i].size(); ++j) {
      std::cout << "---+";
    }
    std::cout << std::endl;
  }
}

void CLInterface::ShortestPathBetweenVertices() {
  if (!CheckGraph()) return;

  int src, dest;

  PrintInput("Enter the starting vertex:");
  if (!(std::cin >> src)) src = -1;
  CleanInput();
  if (!ValidateVertex(src)) return;

  PrintInput("Enter the destination vertex:");
  if (!(std::cin >> dest)) dest = -1;
  CleanInput();
  if (!ValidateVertex(dest)) return;

  try {
    auto [x, path] = s21_graph_algorithms::GetShortestPathBetweenVertices(
        graph_, src - 1, dest - 1);
    if (x == -1) {
      std::cout << "Path between vertices " << src << " and " << dest
                << " does not exist" << std::endl;
      return;
    }
    std::cout << "The smallest distance: " << x << std::endl;
    std::string path_str = "";
    PrintPath(path_str, path);
  } catch (std::exception& e) {
    PrintWarning(e.what());
  }
}

void CLInterface::DFS() {
  if (!CheckGraph()) return;

  int start;
  PrintInput("Enter the starting vertex:");
  if (!(std::cin >> start)) start = -1;
  CleanInput();
  if (!ValidateVertex(start)) return;

  try {
    auto res = s21_graph_algorithms::DepthFirstSearch(graph_, start - 1);
    for (auto elem : res) std::cout << elem + 1 << " ";
    std::cout << std::endl;
  } catch (std::exception& e) {
    PrintWarning(e.what());
  }
}

void CLInterface::BFS() {
  if (!CheckGraph()) return;

  int start;
  PrintInput("Enter the starting vertex:");
  if (!(std::cin >> start)) start = -1;
  CleanInput();
  if (!ValidateVertex(start)) return;

  try {
    auto res = s21_graph_algorithms::BreadthFirstSearch(graph_, start - 1);
    for (auto elem : res) std::cout << elem + 1 << " ";
    std::cout << std::endl;
  } catch (std::exception& e) {
    PrintWarning(e.what());
  }
}

void CLInterface::LoadGraphFromFile() {
  std::string filename;
  PrintInput("Enter the filename:");
  std::cin >> filename;
  // filename.trim();
  try {
    s21_graph new_graph;
    new_graph.LoadFromFile(filename);
    graph_ = std::move(new_graph);
    is_graph_loaded_ = true;
    PrintInput("Graph loaded successfully");
    graph_.PrintGraph();

    std::string dot_filename = filename + ".dot";
    graph_.ExportToDot(dot_filename);
  } catch (std::exception& e) {
    is_graph_loaded_ = false;
    PrintWarning(e.what());
  }
}

void CLInterface::PrintMenu() const {
  PrintInformation(
      "\n<<< SIMPLE NAVIGATOR MENU: >>>\n1. Load graph from file\n2. "
      "Breadth-first search\n3. Depth-first search\n4. Shortest path between "
      "two vertices\n5. Shortest paths between all pairs of vertices\n6. "
      "Minimum spanning tree\n7. Solve traveling salesman problem using ant "
      "colony algorithm\n8. Bonus: Analyze TSP solutions with different "
      "algorithms\n0. Exit");
  PrintInput("Your choice:");
}

bool CLInterface::CheckGraph() const {
  if (!is_graph_loaded_) {
    PrintWarning("Please load the graph first");
    return false;
  }

  if (graph_.Size() == 0) {
    PrintWarning("The graph is empty");
    return false;
  }
  return true;
}

bool CLInterface::ValidateVertex(int vertex) const {
  if (vertex < 1 || vertex > graph_.Size()) {
    PrintWarning("Invalid vertex number");
    return false;
  }
  return true;
}

void CLInterface::PrintInformation(const std::string& msg) const {
  std::cout << COLOR_YELLOW << msg << COLOR_RESET << std::endl;
}

void CLInterface::PrintWarning(const std::string& msg) const {
  std::cout << COLOR_RED << "<<< " << msg << " >>>" << COLOR_RESET << std::endl;
}

void CLInterface::PrintInput(const std::string& msg) const {
  std::cout << COLOR_GREEN << "<-- " << msg << " -->" << COLOR_RESET
            << std::endl;
}