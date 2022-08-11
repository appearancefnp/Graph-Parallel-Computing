#include "graph.h"

#include <iostream>
#include <fstream>
#include <stack>
#include <tuple>

Edge::Edge() {}

Edge::Edge(const size_t time) {
  time_ = time;
  connected_ = true;
}

void Edge::connect() {
  connected_ = true;
}
void Edge::disconnect() {
  connected_ = false;
}
bool Edge::isConnected() {
  return connected_;
}

/////////////////// graphing /////////////////////////

DependencyGraph::DependencyGraph() {}

void DependencyGraph::setNodes(size_t number_of_nodes) {
  adjecency_matrix_.clear();
  adjecency_matrix_.resize(number_of_nodes);
  for (auto& row : adjecency_matrix_) {
    row.resize(number_of_nodes);
  }
}

void DependencyGraph::addDirectedEdge(const size_t from, const size_t to, const size_t time) {
  adjecency_matrix_[from][to].connect();
}

bool DependencyGraph::parseTextFile(const std::string& filename) {
  std::ifstream in_file(filename.c_str());

  if (in_file.is_open() == false) {
    return false;
  }

  size_t nodes;
  size_t from, to, time;

  in_file >> nodes;
  setNodes(nodes);
  while (in_file.eof() == false) {
    in_file >> from >> to >> time;

    if (from >= nodes || to >= nodes) {
      std::cout << "Wrong indices for adding directed edge" << std::endl;
    }

    addDirectedEdge(from, to, time);
    std::cout << "Adding edge from: " << from << " -> " << to << std::endl;
  }

  in_file.close();

  return true;
}

void DependencyGraph::removeTransitivity() {
  if (adjecency_matrix_.empty()) {
    return;
  }

  // remove self connections
  for (size_t i = 0; i < adjecency_matrix_.size(); i++) {
    auto& edge = adjecency_matrix_[i][i];
    edge.disconnect();
  }

  // remove transitive edges
  for (size_t j = 0; j < adjecency_matrix_.size(); j++) {
    for (size_t i = 0; i < adjecency_matrix_.size(); i++) {
      if (adjecency_matrix_[i][j].isConnected()) {
        for (size_t k = 0; k < adjecency_matrix_.size(); k++) {
          if (adjecency_matrix_[j][k].isConnected()) {
            adjecency_matrix_[i][k].disconnect();
          }
        }
      }
    }
  }
}

void DependencyGraph::printConnections() {
  for (size_t j = 0; j < adjecency_matrix_.size(); j++) {
    for (size_t i = 0; i < adjecency_matrix_.size(); i++) {
      if (adjecency_matrix_[j][i].isConnected()) {
        std::cout << "Connection: " << j << " -> " << i << std::endl;
      }
    }
  }
}

std::tuple<std::set<size_t>, std::set<size_t>, bool>
DependencyGraph::dfsVisit(const size_t i, std::set<size_t> discovered, std::set<size_t> finished) {
  discovered.insert(i);
  bool has_cycle{false};
  for (size_t j = 0; j < adjecency_matrix_.size(); j++) {
    if (adjecency_matrix_[i][j].isConnected() == false) {
      continue;
    }
    // detect if a cycle has occured
    if (discovered.find(j) != discovered.end()) {
      has_cycle = true;
    }

    // recurse into dfs
    if (finished.find(j) != finished.end()) {
      dfsVisit(j, discovered, finished);
    }
  }

  discovered.erase(discovered.find(i));
  finished.insert(i);

  return {discovered, finished, has_cycle};
}

bool DependencyGraph::hasCycles() {
  std::set<size_t> discovered;
  std::set<size_t> finished;
  bool has_cycle = false;

  for (size_t i = 0; i < adjecency_matrix_.size(); i++) {
    if (discovered.find(i) == discovered.end() && finished.find(i) == finished.end()) {
      std::tie(discovered, finished, has_cycle) = dfsVisit(i, discovered, finished);
    }
  }

  return false;
}

std::vector<size_t> DependencyGraph::dependentNodesDFS(const size_t node) {
  // perform DFS to find dependent nodes
  std::vector<bool> visited(adjecency_matrix_.size(), false);
  std::vector<size_t> dependent_nodes;

  std::stack<size_t> stack;
  stack.push(node);
  visited[node] = true;

  while (!stack.empty()) {
    auto i = stack.top();
    stack.pop();

    for (size_t j = 0; j < adjecency_matrix_.size(); j++) {
      if (adjecency_matrix_[i][j].isConnected() == false) {
        continue;
      }

      if (visited[j] == false) {
        dependent_nodes.emplace_back(j);
        visited[j] = true;
        stack.push(j);
      }
    }
  }
  return dependent_nodes;
}

std::vector<std::vector<size_t>> DependencyGraph::dependentNodes() {
  // find for each node a dependency list
  std::vector<std::vector<size_t>> dependent_nodes_vec(adjecency_matrix_.size());

  for (size_t i = 0; i < dependent_nodes_vec.size(); i++) {
    dependent_nodes_vec[i] = dependentNodesDFS(i);
  }

  return dependent_nodes_vec;
}