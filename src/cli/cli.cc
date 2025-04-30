#include "cli.h"

void CLInterface::Loop() {
  int choice = -1;
  do {
    PrintMenu();  //! до цикла?
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
        PrintInformation("Завершение программы");
        break;
      default:
        PrintWarning("Неверная команда");
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
    std::cout << "СРАВНИТЬ АЛГОРИТМЫ РЕШЕНИЯ КОММИВОЯЖЕРА" << std::endl;
    s21_graph_algorithms::AnalyzeTSPAlgorithms(graph_);
    // TODO
  } catch (std::exception& e) {
    PrintWarning(e.what());
  }
}

void CLInterface::SalesmanProblem() {
  if (!CheckGraph()) return;

  try {
    std::cout << "РЕАЛИЗОВАТЬ МУРАВЬИНЫЙ АЛГОРИТМ" << std::endl;
    s21_graph_algorithms::SolveTravelingSalesmanProblem(graph_);
    // TODO
  } catch (std::exception& e) {
    PrintWarning(e.what());
  }
}

void CLInterface::MinimumSpanningTree() {
  if (!CheckGraph()) return;

  try {
    std::cout << "РЕАЛИЗОВАТЬ ПОИСК МИНИМАЛЬНОГО ОСТОВНОГО ДЕРЕВА" << std::endl;
    s21_graph_algorithms::GetLeastSpanningTree(graph_);
    // TODO
  } catch (std::exception& e) {
    PrintWarning(e.what());
  }
}

void CLInterface::ShortestPathBetweenAllVertices() {
  if (!CheckGraph()) return;

  try {
    std::cout << "РЕАЛИЗОВАТЬ ПОИСК КРАТЧАЙШЕГО МЕЖДУ ВСЕМИ" << std::endl;
    s21_graph_algorithms::GetShortestPathsBetweenAllVertices(graph_);
    // TODO
  } catch (std::exception& e) {
    PrintWarning(e.what());
  }
}

void CLInterface::ShortestPathBetweenVertices() {
  if (!CheckGraph()) return;

  int src, dest;

  PrintInput("Введите начальную вершину:");
  if (!(std::cin >> src)) src = -1;
  CleanInput();
  if (!ValidateVertex(src)) return;

  PrintInput("Введите конечную вершину:");
  if (!(std::cin >> dest)) dest = -1;
  CleanInput();
  if (!ValidateVertex(dest)) return;

  try {
    std::cout << "РЕАЛИЗОВАТЬ ПОИСК КРАТЧАЙШЕГО МЕЖДУ " << src << " и " << dest
              << std::endl;
    s21_graph_algorithms::GetShortestPathBetweenVertices(graph_, src, dest);
    // TODO
  } catch (std::exception& e) {
    PrintWarning(e.what());
  }
}

void CLInterface::DFS() {
  if (!CheckGraph()) return;

  int start;
  PrintInput("Введите стартовую вершину:");
  if (!(std::cin >> start)) start = -1;
  CleanInput();
  if (!ValidateVertex(start)) return;

  try {
    graph_.PrintGraph();
    std::cout << "РЕАЛИЗОВАТЬ ОБХОД В ГЛУБИНУ вершина " << start << std::endl;
    s21::vector res = s21_graph_algorithms::DepthFirstSearch(graph_, start);
    for (auto elem : res) std::cout << elem << " ";
    std::cout << std::endl;
  } catch (std::exception& e) {
    PrintWarning(e.what());
  }
}

void CLInterface::BFS() {
  if (!CheckGraph()) return;

  int start;
  PrintInput("Введите стартовую вершину:");
  if (!(std::cin >> start)) start = -1;
  CleanInput();
  if (!ValidateVertex(start)) return;

  try {
    std::cout << "РЕАЛИЗОВАТЬ ОБХОД В ШИРИНУ вершина " << start << std::endl;
    s21_graph_algorithms::BreadthFirstSearch(graph_, start);
    // TODO
  } catch (std::exception& e) {
    PrintWarning(e.what());
  }
}

void CLInterface::LoadGraphFromFile() {
  std::string filename;
  PrintInput("Введите название файла:");
  std::cin >> filename;
  // filename.trim();
  try {
    s21_graph new_graph;
    new_graph.LoadFromFile(filename);
    graph_ = std::move(new_graph);
    is_graph_loaded_ = true;
    PrintInput("Граф успешно загружен");
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
      "\n<<< SIMPLE NAVIGATOR MENU: >>>\n1. Загрузить граф из файла\n2. "
      "Обход графа в ширину\n3. Обход графа в глубину\n4. Кратчайший путь "
      "между двумя вершинами\n5. Кратчайший путь между всеми парами "
      "вершин\n6. Минимальное остовное дерево\n7. Решение задачи "
      "коммивояжера муравьиным алгоритмом\n8. Bonus: Анализ решения задачи "
      "коммивояжера разными алгоритмами\n0. Выход");
  PrintInput("Ваш выбор:");
}

bool CLInterface::CheckGraph() const {
  if (!is_graph_loaded_) {
    PrintWarning("Сначала загрузите граф");
    return false;
  }

  if (graph_.Size() == 0) {
    PrintWarning("Граф пуст");
    return false;
  }
  return true;
}

bool CLInterface::ValidateVertex(int vertex) const {
  if (vertex < 0 || vertex >= graph_.Size()) {
    PrintWarning("Неверный номер вершины");
    return false;
  }
  return true;
}

void CLInterface::PrintInformation(const std::string msg) const {
  std::cout << COLOR_YELLOW << msg << COLOR_RESET << std::endl;
}

void CLInterface::PrintWarning(const std::string msg) const {
  std::cout << COLOR_RED << "<<< " << msg << " >>>" << COLOR_RESET << std::endl;
}

void CLInterface::PrintInput(const std::string msg) const {
  std::cout << COLOR_GREEN << "<-- " << msg << " -->" << COLOR_RESET
            << std::endl;
}