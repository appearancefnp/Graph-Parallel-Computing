# Parallel Task Execution

Given a graph that represents dependencies of tasks, run tasks in parallel if all of the dependency nodes have finished.

![dependency graph](https://www.researchgate.net/profile/Sergei-Shudler/publication/320678407/figure/fig1/AS:580526858014720@1515419796704/Task-dependency-graph-each-node-contains-the-task-time-and-the-highlighted-tasks-form.png)

For example, if node 2 has finished processing, it can allow to process nodes 8, 7, 4.

## Build and run

```
mkdir build
cd build
cmake ..
make -j7
./bin/dependency-tree graph.txt
```