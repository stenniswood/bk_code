//the decision tree class
#include <vector>
using namespace std;

/*
	BK Algorithm for "Maze Solving"
	Add starting location as the root node.
	
	Scan 4 directions plus 4 diagonals.
	When they hit a wall or object, record the next possible end points.
		ie. Scan the doors on that wall which are accessible.  
		Add them to the tree as leaves.
		if the wall ends with out a corner wall blocking the end.  Record that end.
		Search both directions from the intersection point.
			
		If another wall prevents reaching the end, don't record that corner position.
				It's a dead end.  ie. another direction should provide a door way.

	
*/

//tree node class
class TreeNodes
{
public:
    TreeNodes();
    ~TreeNodes();

    int m_NodeID;
	
	struct location Position;
	float	total_distance;	
	
    vector<TreeNodes> Branch;
};


class DecisionTree
{
public:
    //functions
    void RemoveNode(TreeNodes* node);
    void DisplayTree(TreeNodes* CurrentNode);
    void Output();
    void Query();
    void QueryTree(TreeNodes* rootNode);
    void AddNode1(int ExistingNodeID, int NewNodeID);
    void CreateRootNode(int NodeID);
    void MakeDecision(TreeNodes* node);

    bool SearchAddNode(TreeNodes* CurrentNode, int ExistingNodeID, int NewNodeID);

    TreeNodes 		   m_RootNode;
	vector<TreeNodes*> m_leaves;	// the lowest level.

    DecisionTree();

    virtual ~DecisionTree();
};


