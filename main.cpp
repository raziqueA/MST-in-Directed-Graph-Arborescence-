  #include <algorithm>
  #include <iostream>
  #include <limits.h>
  #include <list>
  #include <map>
  #include <queue>
  #include <vector>
  using namespace std;

  void addEddge(int u, int v, int w, map<int, list<pair<int, int>>> &adjList) {
    adjList[u].push_back(make_pair(v, w));
  }
  void printList(map<int, list<pair<int, int>>> adjList) {
    for (auto i : adjList) {
      cout << i.first << "->";
      for (auto j : i.second) {
        cout << "(" << j.first << "," << j.second << "),";
      }
      cout << endl;
    }
  }
  int findSum(map<int, list<pair<int, int>>> adjList) {
    int sum = 0;
    for (auto i : adjList) {
      for (auto j : i.second) {
        sum += j.second;
      }
    }
    return sum;
  }

  void distSolver(map<int, list<pair<int, int>>> adjList, vector<int> &dist,
                  vector<bool> &vis, int src, int distance) {
    vis[src] = 1;
    dist[src] = distance;
    for (auto it : adjList[src]) {
      if (vis[it.first] == 0) {
        distSolver(adjList, dist, vis, it.first, distance + it.second);
      }
    }
  }

  void printDistance(map<int, list<pair<int, int>>> adjList, int n, int src) {
    vector<int> Dist(n + 1, -1);
    Dist[src] = 0;
    vector<bool> vis(n + 1, 0);
    distSolver(adjList, Dist, vis, src, 0);
    for (int i = 1; i <= n; i++) {
      cout << Dist[i] << " ";
    }
    cout << "# ";
  }

  void printParent(map<int, list<pair<int, int>>> adjList, int n, int src) {
    vector<int> parent(n + 1, -1);
    parent[src] = 0;
    for (auto it : adjList) {
      for (auto it2 : it.second) {
        parent[it2.first] = it.first;
      }
    }
    for (int i = 1; i <= n; i++) {
      cout << parent[i] << " ";
    }
  }

  bool checkZeroCycleSolver(map<int, list<pair<int, int>>> &adjList, int n, int s,
                            vector<bool> &vis, vector<bool> &dfsVis,
                            deque<int> &cycleNodes) {
    vis[s] = true;
    dfsVis[s] = true;
    cycleNodes.push_back(s);
    for (auto it : adjList[s]) {
      if (it.second == 0 && !vis[it.first]) {
        bool ans =
            checkZeroCycleSolver(adjList, n, it.first, vis, dfsVis, cycleNodes);
        if (ans == true) {
          return true;
        }
      }
      if (it.second == 0 && vis[it.first] && dfsVis[it.first]) {
        while (!cycleNodes.empty() && cycleNodes.front() != it.first) {
          cycleNodes.pop_front();
        }
        // Extract the cycle nodes and return
        return true;
      }
    }
    dfsVis[s] = false;
    cycleNodes.pop_back();
    return false;
  }
  bool checkZeroCycle(map<int, list<pair<int, int>>> &adjList, int n,
                      deque<int> &nodes) {

    for (int i = 1; i <= n; i++) {
      bool ans = false;
      vector<bool> vis(n + 1, false);
      vector<bool> dfsVis(n + 1, false);
      deque<int> cycleNodes;
      ans = checkZeroCycleSolver(adjList, n, i, vis, dfsVis, cycleNodes);
      if (ans == true) {
        // queue se vector me daal do by reference vector leke main arboro
        // function se leke
        nodes = cycleNodes;
        return true;
      }
    }
    return false;
  }

  void minCostArboro(map<int, list<pair<int, int>>> adjList, int n,
                    map<int, list<pair<int, int>>> &result, int newN) {
    int lastNode = adjList.empty() ? 0 : adjList.rbegin()->first;
    vector<int> indegree(newN, 0);
    map<int, list<pair<int, int>>> reducedEdgeList;
    vector<int> minIndegreeValue(newN, INT_MAX);
    for (auto it : adjList) {
      int u = it.first;
      for (auto it2 : it.second) {
        int v = it2.first;
        int w = it2.second;
        indegree[it2.first]++;
        if (minIndegreeValue[v] > w) {
          minIndegreeValue[v] = w;
        }
        reducedEdgeList[u].push_back(make_pair(v, w));
      }
    }
    // reducing incoming edge
    for (auto &it : reducedEdgeList) {
      int u = it.first;
      for (auto &it2 : it.second) {
        int v = it2.first;
        int w = it2.second;
        // reducedEdgeList[u].push_back(make_pair(v,w-minIndegreeValue[v]));
        it2.second -= minIndegreeValue[v];
      }
    }
    // handling - Base Case
    // check if zero weighted cycle exists

    vector<bool> vis(newN, false);
    deque<int> cycleNodes;
    bool ans = checkZeroCycle(reducedEdgeList, newN - 1, cycleNodes);
    // if ans == true --> combine krdo
    // contract karo
    // i have to cluster all the nodes inside of deque cycleNodes to 1 node named
    // largest named node +1
    if (ans == true) {
      // int newNode = reducedEdgeList.empty() ? 1 :
      // reducedEdgeList.rbegin()->first+1;
      int newNode = newN;
      map<int, list<pair<int, int>>> contractedEdgeList;
      for (auto it : reducedEdgeList) {
        int u = it.first;
        for (auto it2 : it.second) {
          int v = it2.first;
          int w = it2.second;
          // check if u and v both in cycleNodes
          if (find(cycleNodes.begin(), cycleNodes.end(), u) != cycleNodes.end() &&
              find(cycleNodes.begin(), cycleNodes.end(), v) != cycleNodes.end()) {
            // it should be hidden edge inside newNode in contractedEdgeList so do
            // nothing
            continue;
          }
          // if v in cycleNodes and u not in cycleNodes
          if (find(cycleNodes.begin(), cycleNodes.end(), v) != cycleNodes.end() &&
              find(cycleNodes.begin(), cycleNodes.end(), u) == cycleNodes.end()) {
            // // add v to contractedEdgeList
            // check here if edge u->newNode already exists in contractedEdgeList
            // and if it exist and its weight greater than w then replace weight
            // with w
            auto &uKiEdges = contractedEdgeList[u];
            auto it = find_if(uKiEdges.begin(), uKiEdges.end(),
                              [&](const pair<int, int> &edge) {
                                return edge.first == newNode;
                              });

            if (it != uKiEdges.end()) {
              // Edge u->newNode already exists, update the weight to the minimum
              // of existing weight and w
              it->second = min(it->second, w);
            } else {
              // Edge u->newNode doesn't exist, add it to contractedEdgeList
              uKiEdges.emplace_back(newNode, w);
            }
          }
          // if u in cycleNodes and v not in cycleNodes
          //  newNode -> v
          if (find(cycleNodes.begin(), cycleNodes.end(), u) != cycleNodes.end() &&
              find(cycleNodes.begin(), cycleNodes.end(), v) == cycleNodes.end()) {
            // check if there already exist edge from v to newNode in
            // contractedEdgeList and if it exist and its weight greater than w
            // then replace weight with w
            auto &newNodeEdges = contractedEdgeList[newNode];
            auto it = find_if(
                newNodeEdges.begin(), newNodeEdges.end(),
                [&](const pair<int, int> &edge) { return edge.first == v; });

            if (it != newNodeEdges.end()) {
              // Edge newNode->v already exists, update the weight to the minimum
              // of existing weight and w
              it->second = min(it->second, w);
            } else {
              // Edge u->newNode doesn't exist, add it to contractedEdgeList
              newNodeEdges.emplace_back(v, w);
            }
          }
          // check if both not in cycleNodes
          if (find(cycleNodes.begin(), cycleNodes.end(), u) == cycleNodes.end() &&
              find(cycleNodes.begin(), cycleNodes.end(), v) == cycleNodes.end()) {
            // add u and v to contractedEdgeList
            contractedEdgeList[u].emplace_back(v, w);
          }
        }
      }
      int newSize = contractedEdgeList.size();

      minCostArboro(contractedEdgeList, newSize, result, newN + 1);
      map<int, list<pair<int, int>>> mst = result;
      for (auto it : reducedEdgeList) {
        int u = it.first;
        for (auto it2 : it.second) {
          int v = it2.first;
          int w = it2.second;
          // check if u and v both in cycleNodes
          if (find(cycleNodes.begin(), cycleNodes.end(), u) != cycleNodes.end() &&
              find(cycleNodes.begin(), cycleNodes.end(), v) != cycleNodes.end()) {
            continue;
            // All case 1 will be handles later
          }
          // if v in cycleNodes and u not in cycleNodes
          if (find(cycleNodes.begin(), cycleNodes.end(), v) != cycleNodes.end() &&
              find(cycleNodes.begin(), cycleNodes.end(), u) == cycleNodes.end()) {
            // if mst me u->(lastNode, w) exist krta hai toh mst dest for u to v
            pair<int, int> p = {lastNode, w};
            for (auto &line : mst) {
              if (line.first == u) {
                for (auto &ptr : line.second) {
                  if (ptr.first == newNode && ptr.second == w) {
                    ptr.first = v;
                  }
                }
              }
            }
          }
          // if u in cycleNodes and v not in cycleNodes
          if (find(cycleNodes.begin(), cycleNodes.end(), v) == cycleNodes.end() &&
              find(cycleNodes.begin(), cycleNodes.end(), u) != cycleNodes.end()) {
            pair<int, int> p = {v, w};
            // mere pass ek aesi edge ayi hai jo u se v jari hai, or u cluster me
            // hai mujhe check krna hai in mst kya koi edge hai jo newNode se v
            // jari hai or jiska weight w hi ho agar aesi edge hai mst me to
            // mst[u].push_back(v) krdo or remove mst[u].remove({v,w})
            bool flag = false;
            for (auto it3 : mst) {
              int u1 = it3.first;
              for (auto it4 : it3.second) {
                int v1 = it4.first;
                int w1 = it4.second;
                if (v == v1 && w == w1 && u1 == newNode) {
                  mst[u].push_back({v, w});
                  mst[u1].remove(it4);
                  if (mst[u1].size() == 0) {
                    mst.erase(u1);
                    flag = true;
                    break;
                  }
                }
              }
              if (flag == true) {
                break;
              }
            }
          }
          // if u and v both not in cycleNodes
          // ye expansion ke baad hona chahiye mtlab sabse last me
          if (find(cycleNodes.begin(), cycleNodes.end(), v) == cycleNodes.end() &&
              find(cycleNodes.begin(), cycleNodes.end(), u) == cycleNodes.end()) {
            //Explicitly handles after words
          }
        }
      }
      vector<int> inDegreeMst(newN, 0);
      for (auto it : mst) {
        for (auto it2 : it.second) {
          int u = it.first;
          int v = it2.first;
          inDegreeMst[v]++;
        }
      }

      for (auto it : reducedEdgeList) {
        int u = it.first;
        for (auto it2 : it.second) {
          int v = it2.first;
          int w = it2.second;
          // check if u and v both in cycleNodes
          if (find(cycleNodes.begin(), cycleNodes.end(), u) != cycleNodes.end() &&
              find(cycleNodes.begin(), cycleNodes.end(), v) != cycleNodes.end()) {
            // cout<<"Case 1:"<<endl;
            // it should be hidden edge inside newNode in contractedEdgeList so do
            // nothing
            //  cout<<u<<" and "<<v<<" both in cycleNodes"<<endl;
            // ye condition change krna pad sakta hai
            if (inDegreeMst[v] == 0 && w == 0) {
              mst[u].push_back({v, w});
              inDegreeMst[v]++;
            }
          }
        }
      }
      for (auto &it : mst) {
        for (auto &it2 : it.second) {
          int u = it.first;
          int v = it2.first;
          int w = it2.second;
          it2.second += minIndegreeValue[v];
        }
      }
      result = mst;
      
    } else {
      map<int, list<pair<int, int>>> mstEdgeList;
      // find MST for directed graph
      for (auto it : adjList) {
        int u = it.first;
        for (auto it2 : adjList[u]) {
          int v = it2.first;
          int w = it2.second;

          if (indegree[v] != 0) {
            // check karo mst me v me incoming edge hai kya?
            bool isVPresent = false;
            for (auto &it3 : mstEdgeList) {
              int u1 = it3.first;
              for (auto &it4 : it3.second) {
                int v1 = it4.first;
                int w1 = it4.second;
                if (v1 == v) {
                  isVPresent = true;
                  if (it4.second > w) {
                    // mstEdgeList[u].push_back(make_pair(v,w));
                    mstEdgeList[u].push_back({v, w});
                    // erase
                    mstEdgeList[u1].remove({v, w1});
                    if (mstEdgeList[u1].empty()) {
                      mstEdgeList.erase(u1);
                    }
                  }
                  break;
                }
              }
            }
            if (!isVPresent) {
              mstEdgeList[u].push_back(make_pair(v, w));
            }
          }
        }
      }
      result = mstEdgeList;
    }
  }

  int main() {
    int T;
    cin >> T;
    while (T--) {
      map<int, list<pair<int, int>>> adjList;
      int N = 8, M = 13, s = 1;
      cin >> N >> M >> s;
      bool checkNeg = false;
      for (int i = 0; i < M; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        if (w < 0) {
          cout << "-1" << endl;
          checkNeg = true;
        } else {
          addEddge(u, v, w, adjList);
        }
      }
      if(checkNeg){
        continue;
      }
      map<int, list<pair<int, int>>> result;
      minCostArboro(adjList, N, result, N + 1);
      cout << findSum(result) << " ";
      printDistance(result, N, s);
      printParent(result, N, s);
      cout << endl;
    }
    return 0;
  }
