#include <set>
#include <string>
#include <vector>

class Edge {
 public:
  Edge();
  Edge(const size_t time);
  void disconnect();
  void connect();
  bool isConnected();

 private:
  bool connected_{false};
  size_t time_{0};
};

class DependencyGraph {
 public:
  DependencyGraph();
  void setNodes(size_t number_of_nodes);
  void addDirectedEdge(const size_t from, const size_t to, const size_t time);
  void removeTransitivity();
  bool parseTextFile(const std::string& filename);
  void printConnections();
  bool hasCycles();
  std::vector<std::vector<size_t>> dependentNodes();

 private:
  std::vector<size_t> dependentNodesDFS(const size_t node);
  std::tuple<std::set<size_t>, std::set<size_t>, bool> dfsVisit(const size_t i, std::set<size_t> discovered,
                                                                std::set<size_t> finished);
  std::vector<std::vector<Edge>> adjecency_matrix_;
};