#include <stdexcept>

class s21_graph {
 public:
  void LoadGraphFromFile(std::string& filename);
  void ExportGraphToDot(std::string& filename);

  int Size() const;
};