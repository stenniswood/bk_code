#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "all_objects.h"
#include "DecisionTree.hpp"




//contrctor
TreeNodes::TreeNodes()
{
    m_NodeID = 0;
}

TreeNodes::TreeNodes(int nodeID/*, string NQA*/)
{
    //create tree node with a specific node ID
    m_NodeID = nodeID;

}

//deconstructor
TreeNodes::~TreeNodes()
{ 
}




