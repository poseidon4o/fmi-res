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
template <typename NodeType, typename EdgeType=float>
class WeightedDirectedGraph
{
public:
	typedef NodeType node;
	typedef EdgeType edge;

	typedef std::unordered_map<node, edge> EdgesMap;
	typedef std::unordered_map<node, EdgesMap> NodeMap;

	typedef typename NodeMap::iterator node_iter;
	typedef typename EdgesMap::iterator edge_iter;

	/// Visity callback used for DFS/BFS, if it returns false, walk will stop, otherwise it will continue
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
		assert(graphNodes.erase(n) != 0 && "Should erase atleast one node");

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
		front.push(VisitData{nullptr, &start, nullptr});

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
					front.push(VisitData{&from, &to, &(eIt->second)});
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
		front.push(VisitData{nullptr, &start, nullptr});

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
					front.push(VisitData{&from, &to, &(eIt->second)});
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


typedef WeightedDirectedGraph<std::string, int> CityMap;

void addUDEdge(CityMap &map, const std::string &from, const std::string &to, int edge) {
	map.addEdge(from, to, edge);
	map.addEdge(to, from, edge);
}

int main() {
	CityMap bg;

	bg.addNode("sofia");
	bg.addNode("plovdiv");
	bg.addNode("varna");
	bg.addNode("burgas");
	bg.addNode("ruse");
	bg.addNode("smolian");
	bg.addNode("vidin");

	addUDEdge(bg, "sofia", "vidin", 196);
	addUDEdge(bg, "ruse", "vidin", 357);
	addUDEdge(bg, "sofia", "plovdiv", 138);
	addUDEdge(bg, "sofia", "smolian", 170);
	addUDEdge(bg, "plvodiv", "varna", 280);
	addUDEdge(bg, "plovdiv", "burgas", 223);
	addUDEdge(bg, "varna", "ruse", 171);
	addUDEdge(bg, "varna", "burgas", 87);

	auto printWalk = [](const std::string &from, int edge, const std::string &to) {
		printf("%s -> %s (%d)\n", from.c_str(), to.c_str(), edge);
		return true;
	};

	bg.BFS("sofia", printWalk);

	puts("------------------------------");

	bg.DFS("sofia", printWalk);

	puts("------------------------------");

	bg.DFSRecursive("sofia", printWalk);

	puts("------------------------------");
	getchar();
}

