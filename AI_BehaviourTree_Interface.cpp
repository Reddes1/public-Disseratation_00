#include "AI_BehaviourTree_Interface.h"

#include "D3DUtils_Debug.h"

ConditionCode AI_BehaviourTree_Interface::RunAlgorithm()
{
 	if (!m_ParentIndexes.empty())
	{
		DBOUT("Pre-action Parent Index:" << m_ParentIndexes.top());

		//Get node type
		NT type = m_Nodes[m_ParentIndexes.top()].GetNodeTypeID();

		//Main Operation based on node type
		switch (type)
		{
		case NT::ROOT:
			return RunRootAlgorithm(type);
			break;

		case NT::COMPOSITE:
			return RunCompositeAlgorithm(type);
			break;

		case NT::CONDITION:
			return RunConditionAlgorithm(type);
			break;

		case NT::SELECTOR:
			return RunSelectorAlgorithm(type);
			break;
		}
	}
	else
		msg_assert(false, "BehaviourTree Stack is Empty (Should have at least a root).");

	return ConditionCode();
}

void AI_BehaviourTree_Interface::BindUnitController(AI_Unit_Algorithms_00* controller)
{
	if (controller != nullptr)
	{
		m_UnitController = controller;
	}
}

ConditionCode AI_BehaviourTree_Interface::RunRootAlgorithm(NT& type)
{
	//Check to see if the root has expended all children
	if (m_Nodes[m_ParentIndexes.top()].GetParentCompleteStatus())
	{
		//Reset the parent node and get it ready for fresh cycle
		m_Nodes[m_ParentIndexes.top()].ResetNode();
		assert(m_ParentIndexes.size() == 1);

		DBOUT("Root Node Complete!");

		//Return appropriate code
		return ConditionCode::ROOT_RESET;
	}
	//Root still has operable childs
	else
	{
	//Check to see if type is a parent tile
	if (AddIndexIfParent(m_Nodes[m_ParentIndexes.top()].GetCurrentChildIndex()))
	{
		DBOUT("Root To Parent Change, New Index: " << m_ParentIndexes.top())

		//Run the root's algorithm
		assert(m_Nodes[m_ParentIndexes.top()].GetParentIndex() == 0);
		m_Nodes[0].RunNodeFunction();
	}
	//Is a regular action so run it
	else
	{
		DBOUT("Root Executing Normal Action");

		m_Nodes[m_Nodes[m_ParentIndexes.top()].GetCurrentChildIndex()].RunNodeFunction();
		m_Nodes[m_ParentIndexes.top()].RunNodeFunction();
	}
	}
	return ConditionCode();
}

ConditionCode AI_BehaviourTree_Interface::RunCompositeAlgorithm(NT& type)
{
	//Check to see if the composite is done
	if (m_Nodes[m_ParentIndexes.top()].GetParentCompleteStatus())
	{
		//Composite node complete so clear it for next cycle and pop this from the parent index stack
		m_Nodes[m_ParentIndexes.top()].ResetNode();
		m_ParentIndexes.pop();

		DBOUT("Composite Parent Complete, New Index: " << m_ParentIndexes.top());
	}
	//Composite isn't finished so run it
	else
	{
		//Check to see if type is a parent tile
		if (AddIndexIfParent(m_Nodes[m_ParentIndexes.top()].GetCurrentChildIndex()))
		{
			//Parent was found and added to the stack, so run the now on standby condition nodes function to advance it
			//for when it is revisited
			m_Nodes[m_Nodes[m_ParentIndexes.top()].GetParentIndex()].RunNodeFunction();

			DBOUT("Composite Changing to new parent, New Index: " << m_ParentIndexes.top());
		}
		//Is not a parent tile, so is some kind of action, so run it
		else
		{
			DBOUT("Composite Executing Action At Target Index:  " << m_Nodes[m_ParentIndexes.top()].GetCurrentChildIndex());

			//Run action at index and get condition code
			ConditionCode ret = m_Nodes[m_Nodes[m_ParentIndexes.top()].GetCurrentChildIndex()].RunNodeFunction();
			
			//If the condition is not running, then update composite node
			if(ret != ConditionCode::RUNNING)
				m_Nodes[m_ParentIndexes.top()].RunNodeFunction();
			else
				DBOUT("Composite Action Returned Running At Target Index:  " << m_Nodes[m_ParentIndexes.top()].GetCurrentChildIndex());

		}
	}

	return ConditionCode();
}

ConditionCode AI_BehaviourTree_Interface::RunConditionAlgorithm(NT& type)
{
	//Check to see if the condtion node has resolved
	if (m_Nodes[m_ParentIndexes.top()].GetParentCompleteStatus())
	{
		//Check to see if node is fully resolved
		if (m_Nodes[m_ParentIndexes.top()].GetConditionStatus() == CS::CON_RESOLVED)
		{
			//Reset node and pop current parent index
			m_Nodes[m_ParentIndexes.top()].ResetNode();
			m_ParentIndexes.pop();

			DBOUT("Condition Parent Complete, New Index: " << m_ParentIndexes.top());
			return ConditionCode::PARENT_DONE;
		}
		//Not fully resolved
		else
		{
			//Determine if condition resolved properly for single condition mode
			if (m_Nodes[m_ParentIndexes.top()].IsConditionSingleOnly() &&
				m_Nodes[m_ParentIndexes.top()].GetConditionStatus() == CS::COMPLETE)
			{
				//Flag Resolved
				m_Nodes[m_ParentIndexes.top()].FlagConditionResolved();

				//If the target isnt a parent, execute action
				if (!AddIndexIfParent(m_Nodes[m_ParentIndexes.top()].GetCurrentChildIndex()))
				{
					m_Nodes[m_Nodes[m_ParentIndexes.top()].GetCurrentChildIndex()].RunNodeFunction();
				}
				else
				{
					DBOUT("Condition Changing to new parent, New Index: " << m_ParentIndexes.top());
				}
			}
			//To prevent a false positive, affirm that it is not in single condition mode before proceeding
			else if (!m_Nodes[m_ParentIndexes.top()].IsConditionSingleOnly())
			{
				//Flag Resolved
				m_Nodes[m_ParentIndexes.top()].FlagConditionResolved();

				//If the target isnt a parent, execute action
				if (!AddIndexIfParent(m_Nodes[m_ParentIndexes.top()].GetCurrentChildIndex()))
				{
					DBOUT("Condition Executing Action At Target Index:  " << m_Nodes[m_ParentIndexes.top()].GetCurrentChildIndex());
					m_Nodes[m_Nodes[m_ParentIndexes.top()].GetCurrentChildIndex()].RunNodeFunction();
				}
				else
				{
					DBOUT("Condition Changing to new parent, New Index: " << m_ParentIndexes.top());
				}
			}
			//Node is in single condition mode, but it didnt match resolution condition so flag resolved without executing node
			else
			{
				m_Nodes[m_ParentIndexes.top()].FlagConditionResolved();
			}
		}
	}
	//Node has yet to run, so run algorithm
	else
	{
		DBOUT("Condition Node Executing Check, Current Index: " << m_ParentIndexes.top());
		return m_Nodes[m_ParentIndexes.top()].RunNodeFunction();
	}


	return ConditionCode();
}

ConditionCode AI_BehaviourTree_Interface::RunSelectorAlgorithm(NT& type)
{
	//Check to see if the selector is done
	if (m_Nodes[m_ParentIndexes.top()].GetParentCompleteStatus())
	{
		//Selector node complete so clear it for next cycle and pop this from the parent index stack
		m_Nodes[m_ParentIndexes.top()].ResetNode();
		m_ParentIndexes.pop();

		DBOUT("Selector Parent Complete, New Index: " << m_ParentIndexes.top());
		return ConditionCode::PARENT_DONE;
	}
	//Selector isn't finished so run it
	else
	{
		//Run current child and get code
		ConditionCode ret = m_Nodes[m_Nodes[m_ParentIndexes.top()].GetCurrentChildIndex()].RunNodeFunction();

		//If node succeeded, signal node completion
		if (ret == ConditionCode::SUCCESS)
		{
			//Flag the node as done for next frame
			m_Nodes[m_ParentIndexes.top()].SetParentCompleteStatus(true);
			DBOUT("Selector Parent Successful Action at Index: " << m_ParentIndexes.top());

			return ret;
		}
		//If node failed, run selector function to advance node
		else if (ret == ConditionCode::FAILED)
		{
			//Last node failed, so advance the selector node
			m_Nodes[m_ParentIndexes.top()].RunNodeFunction();
			DBOUT("Selector Parent Failed Action at Index: " << m_ParentIndexes.top());

			return ret;
		}
		//If node is still running, skip over
		else if (ret == ConditionCode::RUNNING)
		{
			DBOUT("Selector Parent Action Still Running at Index: " << m_ParentIndexes.top());
			return ret;
		}
		//Unknown code
		else
		{
			msg_assert(false, "RunSelectorAlgorithm(): Node returned unknown code.");
			return ret;
		}
	}

	//Shouldn't be hit
	assert(false);
	return ConditionCode();
}

void AI_BehaviourTree_Interface::Init()
{

}

void AI_BehaviourTree_Interface::Release()
{
	m_Nodes.clear();
}

bool AI_BehaviourTree_Interface::AddIndexIfParent(unsigned index)
{
	NT type = m_Nodes[index].GetNodeTypeID();
	msg_assert(type != NT::ROOT, "AddIndexIfParent(): Root node found inside tree structure.");

	//Check to see if node is of parent type
	if (type == NT::COMPOSITE ||
		type == NT::CONDITION ||
		type == NT::SELECTOR  ||
		type == NT::SEQUENCE)
	{
		DBOUT("Added New Parent Index!");
		//Store the index of this parent on the stack
		m_ParentIndexes.push(m_Nodes[m_ParentIndexes.top()].GetCurrentChildIndex());
		return true;
	}

	return false;
}

void AI_BehaviourTree_Interface::BuildRootNode(AI_BehaviourTree_Node& node, unsigned& nodeCounter, unsigned childCount, std::stack<unsigned>& childCounterStack)
{
	//Error Checking
	msg_assert(m_Nodes.empty(), "BuildRootNode(): Node Array not empty, attempting to place root.");
	
	//Initialise node, insert and increment node count
	node.InitialiseRoot(nodeCounter, childCount);
	m_Nodes.push_back(node);	
	++nodeCounter;

	//Setup containers
	msg_assert(m_ParentIndexes.empty(), "BuildRootNode(): Parent Indexes Array not empty.");
	msg_assert(childCounterStack.empty(), "BuildRootNode(): Child Count Array not empty.");
	m_ParentIndexes.push(0);
	childCounterStack.push(0);
}

void AI_BehaviourTree_Interface::BuildCompositeNode(AI_BehaviourTree_Node& node, unsigned& nodeCounter, unsigned childCount, std::stack<unsigned>& childCounterStack)
{
	//Initialise and configure node
	node.InitialiseComposite(nodeCounter, childCount);
	node.SetParentIndex(m_ParentIndexes.top());
	m_Nodes[m_ParentIndexes.top()].InsertChildIndex(nodeCounter, childCounterStack.top());
	//Store node and parent index
	m_Nodes.push_back(node);
	m_ParentIndexes.push(nodeCounter);
	//Increment Counters
	++nodeCounter;
	childCounterStack.top() += 1;
	//Add new child stack
	childCounterStack.push(0);
}

void AI_BehaviourTree_Interface::BuildConditionParentNode(AI_BehaviourTree_Node& node, unsigned& nodeCounter,
	std::stack<unsigned>& childCounterStack, NF& function, bool conditionType1, bool conditionType1RetCon)
{
	if (conditionType1)
	{
		//Initialise and configure node
		node.InitialiseConditionType1(function, nodeCounter, conditionType1RetCon);
		node.SetParentIndex(m_ParentIndexes.top());
		m_Nodes[m_ParentIndexes.top()].InsertChildIndex(nodeCounter, childCounterStack.top());
	}
	else
	{
		//Initialise and configure node
		node.InitialiseConditionType2(function, nodeCounter);
		node.SetParentIndex(m_ParentIndexes.top());
		m_Nodes[m_ParentIndexes.top()].InsertChildIndex(nodeCounter, childCounterStack.top());
	}
	//Store node and parent index
	m_Nodes.push_back(node);
	m_ParentIndexes.push(nodeCounter);
	//Increment Counters
	++nodeCounter;
	childCounterStack.top() += 1;
	//Add new child stack
	childCounterStack.push(0);

}

void AI_BehaviourTree_Interface::BuildSelectorNode(AI_BehaviourTree_Node& node, unsigned& nodeCounter, unsigned childCount, std::stack<unsigned>& childCounterStack)
{
	//Initialise and configure node
	node.InitialiseSelector(nodeCounter, childCount);
	node.SetParentIndex(m_ParentIndexes.top());
	m_Nodes[m_ParentIndexes.top()].InsertChildIndex(nodeCounter, childCounterStack.top());
	//Store node and parent index
	m_Nodes.push_back(node);
	m_ParentIndexes.push(nodeCounter);
	//Increment Counters
	++nodeCounter;
	childCounterStack.top() += 1;
	//Add new child stack
	childCounterStack.push(0);

}

void AI_BehaviourTree_Interface::BuildActionNode(AI_BehaviourTree_Node& node, unsigned& nodeCounter, std::stack<unsigned>& childCounterStack, NF& function)
{
	//Initialise and configure node
	node.InitialiseAction(function, nodeCounter);
	node.SetParentIndex(m_ParentIndexes.top());
	m_Nodes[m_ParentIndexes.top()].InsertChildIndex(nodeCounter, childCounterStack.top());
	//Store Node
	m_Nodes.push_back(node);
	//Set/Increment value markers
	++nodeCounter;
	childCounterStack.top() += 1;
}

void AI_BehaviourTree_Interface::ClearParentLayer(std::stack<unsigned>& childCounterStack)
{
	msg_assert(!childCounterStack.empty(), "ClearParentLayerStack(): Container empty, function called incorrectly.");
	msg_assert(!m_ParentIndexes.empty(), "ClearParentLayerStack(): Container empty, function called incorrectly.");

	childCounterStack.pop();
	m_ParentIndexes.pop();
}

void AI_BehaviourTree_Interface::CompileAlgorithm_00()
{
	//Branch = Any point where the tree navigates up or down in the construction process	

	/////////////////
	/// Pre-Amble ///
	/////////////////

	//Pre-clear arrays
	while (!m_ParentIndexes.empty())
		m_ParentIndexes.pop();
	m_Nodes.clear();


	const unsigned TOTAL_NODE_COUNT = 24;

	m_Nodes.reserve(TOTAL_NODE_COUNT);
	//Keep track of the number of nodes (for indexing)
	unsigned nodeCounter = 0;
	//Create node (will be changed by each node entry)
	AI_BehaviourTree_Node node;
	//Create function pointer (changed by each entry as needed)
	NF func;
	//Keep track of the current child count for current parent tile being added to (push/pop as changing parents, increment after each node addition)
	std::stack<unsigned> childCounters;


	/////////////////////////
	/// Tree Construction ///
	/////////////////////////

	//
	//Branch 00 (Node(s): 0-1)
	//

	//Root (Node 00)
	BuildRootNode(node, nodeCounter, 1, childCounters);
	//Condition (Node 01)
	func = std::bind(&UC::RunSearchAlgorithm, m_UnitController, UC::SearchAlgorithms::FIND_ALL_ENEMIES);
	BuildConditionParentNode(node, nodeCounter, childCounters, func, false);

	//
	//Branch 01 (Node(s): 2)
	//

	//Composite (Node 02)
	BuildCompositeNode(node, nodeCounter, 6, childCounters);

	//
	//Branch 02 (Node(s): 3-6)
	//

	//Action (Node 03)
	func = std::bind(&UC::RunBTSupportAlgorithm, m_UnitController, UC::BTSupportAlgorithms::WAIT_TIMER_00);
	BuildActionNode(node, nodeCounter, childCounters, func);
	//Action (Node 04)
	func = std::bind(&UC::RunTargetAlgorithm, m_UnitController, UC::TargetAlgorithms::CLOSEST_ENEMY);
	BuildActionNode(node, nodeCounter, childCounters, func);
	//Action (Node 05)
	func = std::bind(&UC::RunBTSupportAlgorithm, m_UnitController, UC::BTSupportAlgorithms::WAIT_TIMER_00);
	BuildActionNode(node, nodeCounter, childCounters, func);
	//Condition (Node 06)
	func = std::bind(&UC::RunBTSupportAlgorithm, m_UnitController, UC::BTSupportAlgorithms::RANGE_GREATER_THAN_ONE);
	BuildConditionParentNode(node, nodeCounter, childCounters, func, false);

	//
	//Branch 03 (Node(s): 7)
	//

	//Selection (Node 07)
	BuildSelectorNode(node, nodeCounter, 2, childCounters);

	//
	//Branch 04 (Node(s): 8-9)
	//

	//Action (Node 08)
	func = std::bind(&UC::RunMovementAlgorithm, m_UnitController, UC::MovementAlgorithms::MOVE_TO_FIRST_RANGE_TILE);
	BuildActionNode(node, nodeCounter, childCounters, func);
	//Action (Node 09)
	func = std::bind(&UC::RunMovementAlgorithm, m_UnitController, UC::MovementAlgorithms::RANDOM_ADJACENT_TILE_TARGET);
	BuildActionNode(node, nodeCounter, childCounters, func);

	//
	//End of Branches 03/04
	//

	ClearParentLayer(childCounters);

	//
	//Branch 05 (Node(s): 10)
	//

	//Action (Node 10)
	func = std::bind(&UC::RunMovementAlgorithm, m_UnitController, UC::MovementAlgorithms::RANDOM_ADJACENT_TILE_TARGET);
	BuildActionNode(node, nodeCounter, childCounters, func);
	
	//
	//End of Branch 02
	//

	ClearParentLayer(childCounters);

	//
	//Branch 06 (Node(s): 11-12)
	//

	//Action (Node 11)
	func = std::bind(&UC::RunBTSupportAlgorithm, m_UnitController, UC::BTSupportAlgorithms::WAIT_TIMER_00);
	BuildActionNode(node, nodeCounter, childCounters, func);
	//Selection (Node 12)
	BuildSelectorNode(node, nodeCounter, 2, childCounters);


	//
	//Branch 07 (Node(s): 13-14)
	//

	//Action (Node 13)
	func = std::bind(&UC::RunAttackAlgorithm, m_UnitController, UC::AttackAlgorithms::ATTACK_UNIT);
	BuildActionNode(node, nodeCounter, childCounters, func);
	//Action (Node 14)
	func = std::bind(&UC::RunAttackAlgorithm, m_UnitController, UC::AttackAlgorithms::ATTACK_FIRST_VALID_ENEMY);
	BuildActionNode(node, nodeCounter, childCounters, func);
	
	//
	//End of Branches 07/01
	//
	
	ClearParentLayer(childCounters);
	ClearParentLayer(childCounters);

	//
	//Branch 08 (Node(s): 15)
	//

	//Composite (Node 15)
	BuildCompositeNode(node, nodeCounter, 8, childCounters);

	//
	//Branch 09 (Node(s): 16-23)
	//

	//Action (Node 16)
	func = std::bind(&UC::RunBTSupportAlgorithm, m_UnitController, UC::BTSupportAlgorithms::WAIT_TIMER_00);
	BuildActionNode(node, nodeCounter, childCounters, func);
	//Action (Node 17)
	func = std::bind(&UC::RunSearchAlgorithm, m_UnitController, UC::SearchAlgorithms::FIND_ALL_ENEMIES_OOR);
	BuildActionNode(node, nodeCounter, childCounters, func);
	//Action (Node 18)
	func = std::bind(&UC::RunBTSupportAlgorithm, m_UnitController, UC::BTSupportAlgorithms::WAIT_TIMER_00);
	BuildActionNode(node, nodeCounter, childCounters, func);
	//Action (Node 19)
	func = std::bind(&UC::RunTargetAlgorithm, m_UnitController, UC::TargetAlgorithms::CLOSEST_ENEMY);
	BuildActionNode(node, nodeCounter, childCounters, func);
	//Action (Node 20)
	func = std::bind(&UC::RunBTSupportAlgorithm, m_UnitController, UC::BTSupportAlgorithms::WAIT_TIMER_00);
	BuildActionNode(node, nodeCounter, childCounters, func);
	//Action (Node 21)
	func = std::bind(&UC::RunMovementAlgorithm, m_UnitController, UC::MovementAlgorithms::MOVE_CLOSER_TO_TARGET);
	BuildActionNode(node, nodeCounter, childCounters, func);
	//Action (Node 22)
	func = std::bind(&UC::RunBTSupportAlgorithm, m_UnitController, UC::BTSupportAlgorithms::WAIT_TIMER_00);
	BuildActionNode(node, nodeCounter, childCounters, func);
	//Action (Node 23)
	func = std::bind(&UC::RunAttackAlgorithm, m_UnitController, UC::AttackAlgorithms::ATTACK_UNIT);
	BuildActionNode(node, nodeCounter, childCounters, func);


	/////////////////////
	/// Post Clean Up ///
	/////////////////////

	msg_assert(m_Nodes.size() == TOTAL_NODE_COUNT, "Node container not equal to node count");

	//Clear parent indices
	while (!m_ParentIndexes.empty())
		m_ParentIndexes.pop();

	//Set first index in parent indices to root
	m_ParentIndexes.push(0);
}



