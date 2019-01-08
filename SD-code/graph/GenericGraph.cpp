#include <unordered_map>
#include <cassert>
#include <queue>
#include <stack>
#include <functional>


// Notes and questions
// 1. Code duplication - WeightedDirectedGraph::BFS and WeightedDirectedGraph::DFS are mostly identical
//  - Can you remove the code duplication?
//  - Can you make the code better - more clear, more easily readable, more easily debuggable?
// 2. Why does DFS and DFSRecursive print the nodes in different order? Can you fix this?
// 3. All three walks do not call the callback with @to = starting node.
//  - How can this be fixed?
//  - What needs to change in the graph interface
// 4. The graph class implements directed graph but is used as undirected
//  - Is this a problem, and if yes what is it?
//  - How can the class change to support both directed and undirected graphs
//  - Can it be done with one class and non runtime conditions?
// 5. Where is the graph implementation wasting memory?
//  - How can it be solved without major changes to the class?


/// Class implementing directed weighted graph
/// @tparam NodeType - the type held in each node
/// @tparam EdgeType - the type of the weight (usually numeric)
template <typename NodeType, typename EdgeType = float>
class WeightedDirectedGraph
{
public:
	typedef NodeType node;
	typedef EdgeType edge;

	typedef std::unordered_map<node, edge> EdgesMap;
	typedef std::unordered_map<node, EdgesMap> NodeMap;

	typedef typename NodeMap::iterator node_iter;
	typedef typename EdgesMap::iterator edge_iter;

	typedef typename NodeMap::const_iterator const_node_iter;
	typedef typename EdgesMap::const_iterator const_edge_iter;

	/// Visit callback used for DFS/BFS, if it returns false, walk will stop, otherwise it will continue
	/// Will be called with nullptr @from and @edge when calling for the start node
	/// @param from - pointer to origin node or nullptr if visiting first node
	/// @param edge - edge connecting @from and @to, nullptr if @from is nullptr
	/// @param to - the destination node
	/// @return - true if walk should continue, false to stop
	typedef std::function<bool(const node &from, const edge &edge, const node &to)> VisitCallback;

private:
	NodeMap graphNodes;
	int nodeCount = 0;
	int edgeCount = 0;
public:

	/// Add node to the graph, if node already exists does nothing
	/// @param n - the node
	/// @return - true if added, false otherwise
	bool addNode(const node &n) {
		node_iter it = graphNodes.find(n);
		if (it == graphNodes.end()) {
			graphNodes.insert(std::make_pair(n, EdgesMap()));
			++nodeCount;
			return true;
		}

		return false;
	}

	/// Connect two existing nodes in the graph
	/// @param from - origin node
	/// @param to - destination node
	/// @return - true if edge is created, false otherwise
	bool addEdge(const node &from, const node &to, const edge &edge) {
		node_iter fromIt = graphNodes.find(from);
		if (fromIt == graphNodes.end()) {
			return false;
		}

		if (graphNodes.find(to) == graphNodes.end()) {
			return false;
		}

		EdgesMap &adjacent = fromIt->second;
		adjacent[to] = edge;
		++edgeCount;
		return true;
	}

	/// Remove edge between two nodes
	/// @param from - origin node
	/// @param to - destination node
	/// @return - true if edge is removed, false otherwise
	bool removeEdge(const node &from, const node &to) {
		node_iter fromIt = graphNodes.find(from);
		if (fromIt == graphNodes.end()) {
			return false;
		}

		if (graphNodes.find(to) == graphNodes.end()) {
			return false;
		}

		EdgesMap &adjecent = fromIt->second;
		if (adjecent.erase(to) != 0) {
			--edgeCount;
			return true;
		}
		return false;
	}

	/// Remove node from the graph and all edges from and to it
	/// @param n - the node
	/// @return - true if node was removed, false otherwise
	bool removeNode(const node &n) {
		node_iter nIt = graphNodes.find(n);
		if (nIt == graphNodes.end()) {
			return false;
		}
		--nodeCount;
		int removedEdges = nIt->second.size();
		const int erasedCount = graphNodes.erase(n);
		assert(erasedCount != 0 && "Should erase atleast one node");

		// check all nodes
		for (node_iter nIt = graphNodes.begin(); nIt != graphNodes.end(); ++nIt) {
			EdgesMap &adjecent = nIt->second;
			edge_iter edgeIt = adjecent.find(n);
			// if this node points to the delete node, remove the edge
			if (edgeIt != adjecent.end()) {
				++removedEdges;
				adjecent.erase(edgeIt);
			}
		}
		edgeCount -= removedEdges;
		return true;
	}

	/// Walk the graph using BFS, and call callback for each visited node in the order of visiting
	/// @param start - the node that BFS will start from
	/// @param visit - callback executed on each node, if it returns false, BFS will stop
	/// @return - false if can't walk graph, true otherwise
	bool BFS(const node &start, VisitCallback visit) {
		node_iter startIter = graphNodes.find(start);
		if (startIter == graphNodes.end()) {
			return false;
		}
		struct VisitData {
			const node *from;
			const node *to;
			const edge *edge;
		};

		std::unordered_map<node, bool> visited;
		for (node_iter nIt = graphNodes.begin(); nIt != graphNodes.end(); ++nIt) {
			const node &n = nIt->first;
			visited[n] = false;
		}

		std::queue<VisitData> front;
		visited[start] = true;
		front.push(VisitData{ nullptr, &start, nullptr });

		while (!front.empty()) {
			VisitData current = front.front();
			front.pop();
			if (current.from && !visit(*current.from, *current.edge, *current.to)) {
				return true;
			}

			const node &from = *current.to;
			visited[from] = true;

			EdgesMap &adjecent = graphNodes[*current.to];
			for (edge_iter eIt = adjecent.begin(); eIt != adjecent.end(); ++eIt) {
				const node &to = eIt->first;
				if (!visited[to]) {
					visited[to] = true;
					front.push(VisitData{ &from, &to, &(eIt->second) });
				}
			}
		}

		return true;
	}

	/// Walk the graph using DFS (using stack), and call callback for each visited node in the order of visiting
	/// @param start - the node that DFS will start from
	/// @param visit - callback executed on each node, if it returns false, DFS will stop
	/// @return - false if can't walk graph, true otherwise
	bool DFS(const node &start, VisitCallback visit) {
		node_iter startIter = graphNodes.find(start);
		if (startIter == graphNodes.end()) {
			return false;
		}
		struct VisitData {
			const node *from;
			const node *to;
			const edge *edge;
		};

		std::unordered_map<node, bool> visited;
		for (node_iter nIt = graphNodes.begin(); nIt != graphNodes.end(); ++nIt) {
			const node &n = nIt->first;
			visited[n] = false;
		}

		std::stack<VisitData> front;
		visited[start] = true;
		front.push(VisitData{ nullptr, &start, nullptr });

		while (!front.empty()) {
			VisitData current = front.top();
			front.pop();
			if (current.from && !visit(*current.from, *current.edge, *current.to)) {
				return true;
			}

			const node &from = *current.to;
			visited[from] = true;

			EdgesMap &adjecent = graphNodes[*current.to];
			for (edge_iter eIt = adjecent.begin(); eIt != adjecent.end(); ++eIt) {
				const node &to = eIt->first;
				if (!visited[to]) {
					visited[to] = true;
					front.push(VisitData{ &from, &to, &(eIt->second) });
				}
			}
		}

		return true;
	}

	/// Walk the graph using DFS (using recursion), and call callback for each visited node in the order of visiting
	/// @param start - the node that DFS will start from
	/// @param visit - callback executed on each node, if it returns false, DFS will stop
	/// @return - false if can't walk graph, true otherwise
	bool DFSRecursive(const node &start, VisitCallback visit) {
		node_iter startIter = graphNodes.find(start);
		if (startIter == graphNodes.end()) {
			return false;
		}
		struct VisitData {
			const node *from;
			const node *to;
			const edge *edge;
		};

		std::unordered_map<node, bool> visited;
		for (node_iter nIt = graphNodes.begin(); nIt != graphNodes.end(); ++nIt) {
			const node &n = nIt->first;
			visited[n] = false;
		}

		visited[start] = true;

		DFSRecursiveWalk(start, visit, visited);

		return true;
	}

	typedef std::unordered_map<node, edge> DistanceMap;

	/// Run Dijkstra's algorithm on the graph and return a table
	///   mapping all nodes to their distance based on the given starting node
	/// This Algorithm will not work if the graph contains "negative" edges, such that edge(0) + negativeEdge < edge(0)
	/// Unreachable nodes will have their distance set to std::numeric_limits<edge>::max()
	/// NOTE: This algorithm needs "infinity" and "zero" value for the edge type
	///   - for "infinity" it will use std::numeric_limits<edge>::max()
	///   - for "zero" value it will use edge(0)
	///   - edge type needs to have operator+(edge) and operator<(edge) and operator==
	/// NOTE: This is basic implementation of the algorithm which can be optimized in several ways
	/// @param start - the node that will be used to calculate the distance
	/// @return - map of all nodes to their distance, empty if start is not in the graph
	DistanceMap Dijkstra(const node &start) const {
		if (graphNodes.find(start) == graphNodes.end()) {
			return DistanceMap{};
		}

		struct QuePair {
			node node; ///< Current node
			edge distance; ///< Actual distance to reach this node from the start

			/// This will actually compute operator> for the distance
			/// Since std::priority_queue<T> will put the highest priority on top
			/// It computes !< and !== since STL will commonly require only operator< and operator== for items in it's containers
			bool operator<(const QuePair &other) const {
				return !(distance < other.distance) && !(distance == other.distance);
			}
		};

		DistanceMap distances;
		for (const_node_iter it = graphNodes.begin(); it != graphNodes.end(); ++it) {
			distances[it->first] = std::numeric_limits<edge>::max();
		}

		std::priority_queue<QuePair> que;
		// add the start to the que of nodes
		que.push(QuePair{ start, edge(0) });
		// update the distance from start ot itself to 0
		distances[start] = edge(0);

		while (!que.empty()) {
			QuePair current = que.top();
			que.pop();

			const_node_iter nodeEdges = graphNodes.find(current.node);
			assert(nodeEdges != graphNodes.end() && "Edge pointing to invalid node in the graph");

			// go trough each adjacent node and try to relax the distance
			for (const_edge_iter it = nodeEdges->second.begin(); it != nodeEdges->second.end(); ++it) {
				const node &to = it->first;
				const edge &edgeDist = it->second;
				const edge &newTotalDistance = current.distance + edgeDist;

				assert(!(newTotalDistance < current.distance) && "Negative edge in the graph!");

				// check if the new computed distance optimizes the saved in @distances
				if (newTotalDistance < distances[to]) {
					distances[to] = newTotalDistance;
					que.push(QuePair{ to, newTotalDistance });
				}
			}
		}

		return distances;
	}

private:
	bool DFSRecursiveWalk(const node &current, VisitCallback visit, std::unordered_map<node, bool> &visited) {
		EdgesMap &adjecent = graphNodes[current];
		for (edge_iter eIt = adjecent.begin(); eIt != adjecent.end(); ++eIt) {
			const node &to = eIt->first;
			if (!visited[to]) {
				if (!visit(current, eIt->second, to)) {
					return false;
				}
				visited[to] = true;
				if (!DFSRecursiveWalk(to, visit, visited)) {
					return false;
				}
			}
		}
		return true;
	}
};

/// Test with integer nodes and integer edges
void testGraph1() {
	// source https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
	const int V = 9;
	int graph[V][V] = {
	   {0, 4, 0, 0, 0, 0, 0, 8, 0},
	   {4, 0, 8, 0, 0, 0, 0, 11, 0},
	   {0, 8, 0, 7, 0, 4, 0, 0, 2},
	   {0, 0, 7, 0, 9, 14, 0, 0, 0},
	   {0, 0, 0, 9, 0, 10, 0, 0, 0},
	   {0, 0, 4, 14, 10, 0, 2, 0, 0},
	   {0, 0, 0, 0, 0, 2, 0, 1, 6},
	   {8, 11, 0, 0, 0, 0, 1, 0, 7},
	   {0, 0, 2, 0, 0, 0, 6, 7, 0}
	};

	WeightedDirectedGraph<int, int> testG;
	for (int c = 0; c < V; c++) {
		testG.addNode(c);
	}

	for (int c = 0; c < V; ++c) {
		for (int r = 0; r < V; ++r) {
			if (graph[c][r]) {
				testG.addEdge(c, r, graph[c][r]);
				testG.addEdge(r, c, graph[c][r]);
			}
		}
	}

	const int start = 0;
	auto printWalk = [](int from, int edge, int to) {
		printf("%d -> %d (%d)\n", from, to, edge);
		return true;
	};

	puts("------------------------------ BFS:");
	testG.BFS(start, printWalk);

	auto map = testG.Dijkstra(start);
	printf("------------------------------ Dijkstra from %d\n", start);
	for (auto &p : map) {
		printf("%d -> %d : %d\n", start, p.first, p.second);
	}
}


/// Add edge to the graph as if was undirected - actually adds two edges for each call
/// @param graph - the graph
/// @param from - one of the nodes
/// @param to - the other node
/// @param edge - the edge to add
template <typename node, typename edge>
void addUDEdge(WeightedDirectedGraph<node, edge> &graph, const node &from, const node &to, const edge &edge) {
	graph.addEdge(from, to, edge);
	graph.addEdge(to, from, edge);
}


/// Test the graph with some cities
void testMapGraph() {
	typedef WeightedDirectedGraph<std::string, int> CityMap;

	CityMap bg;

	bg.addNode("sofia");
	bg.addNode("plovdiv");
	bg.addNode("varna");
	bg.addNode("burgas");
	bg.addNode("ruse");
	bg.addNode("smolian");
	bg.addNode("vidin");

	// need to specify first template argument, since argument 2 and 3 are char[N] but for graph node = std::string and it is ambiguous
	addUDEdge<std::string>(bg, "sofia", "vidin", 196);
	addUDEdge<std::string>(bg, "ruse", "vidin", 357);
	addUDEdge<std::string>(bg, "sofia", "plovdiv", 138);
	addUDEdge<std::string>(bg, "sofia", "smolian", 170);
	addUDEdge<std::string>(bg, "plvodiv", "varna", 280);
	addUDEdge<std::string>(bg, "plovdiv", "burgas", 223);
	addUDEdge<std::string>(bg, "varna", "ruse", 171);
	addUDEdge<std::string>(bg, "varna", "burgas", 87);

	auto printWalk = [](const std::string &from, int edge, const std::string &to) {
		printf("%s -> %s (%d)\n", from.c_str(), to.c_str(), edge);
		return true;
	};
	const std::string start = "sofia";

	puts("------------------------------ BFS:");
	bg.BFS(start, printWalk);

	puts("------------------------------ DFS:");
	bg.DFS(start, printWalk);

	puts("------------------------------ DFS recursive:");
	bg.DFSRecursive(start, printWalk);

	{
		printf("------------------------------ Dijkstra from %s\n", start.c_str());
		CityMap::DistanceMap distances = bg.Dijkstra(start);

		for (CityMap::DistanceMap::iterator it = distances.begin(); it != distances.end(); ++it) {
			printf("%s -> %s : %d\n", start.c_str(), it->first.c_str(), it->second);
		}
	}

	{
		puts("Adding some fake nodes to create shortcut sofia -> varna");
		bg.addNode("d1");
		bg.addNode("d2");
		bg.addNode("d3");
		addUDEdge<std::string>(bg, "sofia", "d1", 0);
		addUDEdge<std::string>(bg, "d1", "d2", 0);
		addUDEdge<std::string>(bg, "d2", "d3", 0);
		addUDEdge<std::string>(bg, "d3", "varna", 0);

		puts("------------------------------ BFS:");
		bg.BFS(start, printWalk);
		printf("------------------------------ Dijkstra from %s\n", start.c_str());
		CityMap::DistanceMap distances = bg.Dijkstra(start);

		for (CityMap::DistanceMap::iterator it = distances.begin(); it != distances.end(); ++it) {
			printf("%s -> %s : %d\n", start.c_str(), it->first.c_str(), it->second);
		}
	}
}

int main() {
	testMapGraph();

	testGraph1();

	getchar();
}
