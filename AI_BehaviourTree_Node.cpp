#include "AI_BehaviourTree_Node.h"

#include "D3DUtils_Debug.h"

ConditionCode AI_BehaviourTree_Node::RunNodeFunction()
{
	//Run operation differently based on node type
	switch (m_NodeTypeID)
	{
	case NodeType::ACTION:
		return ActionNodeFunction();
		break;

	case NodeType::ROOT:
		return RootNodeFunction();
		break;

	case NodeType::COMPOSITE:
		return CompositeNodeFunction();
		break;

	case NodeType::CONDITION:
		return ConditionNodeFunction();
		break;

	case NodeType::SELECTOR:
		return SelectorNodeFunction();
		break;

	default:
		msg_assert(false, "AI_BehaviourTree_Node attempted to action on uninitialised node.");
	}

	//Should never hit this
	return ConditionCode::CC_ERROR;
}

void AI_BehaviourTree_Node::ResetNode()
{
	m_ParentComplete = false;
	m_SingleConditionRetReq = false;
	m_ChildArrayPos = 0;
	m_ConditionStatus = ConditionStatus::INCOMPLETE;
}

void AI_BehaviourTree_Node::InitialiseRoot(unsigned nodePositionIndex, unsigned childCount)
{
	//Store position & set type
	m_NodeIndex = nodePositionIndex;
	m_NodeTypeID = NodeType::ROOT;

	//Reserve and create nodes equal to expected child count (replaced with real indexes later)
	m_ChildIndexes.clear();
	m_ChildIndexes.shrink_to_fit();
	m_ChildIndexes.reserve(childCount);
	m_ChildIndexes.insert(m_ChildIndexes.begin(), childCount, INVALID_CODE);

}

void AI_BehaviourTree_Node::InitialiseAction(NodeFunc func, unsigned nodePositionIndex)
{
	m_Func = func;
	m_NodeIndex = nodePositionIndex;
	m_NodeTypeID = NodeType::ACTION;

	m_ChildIndexes.clear();
	m_ChildIndexes.shrink_to_fit();
}

void AI_BehaviourTree_Node::InitialiseComposite(unsigned nodePositionIndex, unsigned childCount)
{
	//Store position & set type
	m_NodeIndex = nodePositionIndex;
	m_NodeTypeID = NodeType::COMPOSITE;

	//Reserve and create nodes equal to expected child count (replaced with real indexes later)
	m_ChildIndexes.clear();
	m_ChildIndexes.shrink_to_fit();
	m_ChildIndexes.reserve(childCount);
	m_ChildIndexes.insert(m_ChildIndexes.begin(), childCount, INVALID_CODE);
}

void AI_BehaviourTree_Node::InitialiseConditionType1(NodeFunc func, unsigned nodePositionIndex, bool retCondition)
{
	//Store position & set type
	m_Func = func;
	m_NodeIndex = nodePositionIndex;
	m_NodeTypeID = NodeType::CONDITION;

	//Setup condition parameters
	m_SingleConditionOnly = true;
	m_SingleConditionRetReq = retCondition;

	//Reserve and create nodes equal to child count (replaced with real indexes later)
	m_ChildIndexes.clear();
	m_ChildIndexes.shrink_to_fit();
	m_ChildIndexes.reserve(1);
	m_ChildIndexes.insert(m_ChildIndexes.begin(), 1, INVALID_CODE);
}

void AI_BehaviourTree_Node::InitialiseConditionType2(NodeFunc func, unsigned nodePositionIndex)
{
	//Store position & set type
	m_Func = func;
	m_NodeIndex = nodePositionIndex;
	m_NodeTypeID = NodeType::CONDITION;

	//Setup condition parameters
	m_SingleConditionOnly = false;

	//Reserve and create nodes equal to child count (replaced with real indexes later)
	m_ChildIndexes.clear();
	m_ChildIndexes.shrink_to_fit();
	m_ChildIndexes.reserve(2);
	m_ChildIndexes.insert(m_ChildIndexes.begin(), 2, INVALID_CODE);		
}

void AI_BehaviourTree_Node::InitialiseSelector(unsigned nodePositionIndex, unsigned childCount)
{
	//Store position & set type
	m_NodeIndex = nodePositionIndex;
	m_NodeTypeID = NodeType::SELECTOR;

	//Reserve and create nodes equal to child count (replaced with real indexes later)
	m_ChildIndexes.clear();
	m_ChildIndexes.shrink_to_fit();
	m_ChildIndexes.reserve(childCount);
	m_ChildIndexes.insert(m_ChildIndexes.begin(), childCount, INVALID_CODE);
}

void AI_BehaviourTree_Node::InsertChildIndex(unsigned childIndex, unsigned indexPosition)
{
	msg_assert(indexPosition <= m_ChildIndexes.size(), "InsertChildIndex(): Attempted to insert beyond vector size.");
	m_ChildIndexes[indexPosition] = childIndex;
}

ConditionCode AI_BehaviourTree_Node::RootNodeFunction()
{
	//Increment current child index
	++m_ChildArrayPos;

	//See if composite has completed its run
	if (m_ChildArrayPos >= m_ChildIndexes.size())
	{
		//Flag done (model will reset values)
		m_ParentComplete = true;
		return ConditionCode::SUCCESS;
	}

	return ConditionCode::RUNNING;
}

ConditionCode AI_BehaviourTree_Node::ActionNodeFunction()
{
	return m_Func();
}

ConditionCode AI_BehaviourTree_Node::CompositeNodeFunction()
{
	//Increment current child index
	++m_ChildArrayPos;

	//See if composite has completed its run
	if (m_ChildArrayPos >= m_ChildIndexes.size())
	{
		//Flag done (model will reset values)
		m_ParentComplete = true;
		return ConditionCode::SUCCESS;
	}

	return ConditionCode::RUNNING;
}

ConditionCode AI_BehaviourTree_Node::ConditionNodeFunction()
{
	//Run condition function
	ConditionCode ret = m_Func();

	//If the function returns running (may be a multi-frame check), return appropriate code
	if (ret == ConditionCode::RUNNING)
		return ret;
	else
	{
		//Is this condition single only?
		if (m_SingleConditionOnly)
		{
			if ((m_SingleConditionRetReq == true && ret == ConditionCode::SUCCESS) ||
				(m_SingleConditionRetReq == false && ret == ConditionCode::FAILED))
			{
				//Flag condition as met and parent complete
				m_ConditionStatus = ConditionStatus::COMPLETE;
				m_ParentComplete = true;
				//Return the condition code
				return ret;
			}
			//Condition not met
			else
			{
				m_ConditionStatus = ConditionStatus::COMPLETE_UNMET_CON;
				m_ParentComplete = true;
				//Return the condition code
				return ret;
			}

		}
		//Must be using two conditions
		else
		{
			if (ret == ConditionCode::SUCCESS)
			{
				//Set index and flag done
				m_ChildArrayPos = 0;
				m_ParentComplete = true;
				m_ConditionStatus = ConditionStatus::COMPLETE;
				//Return the condition code
				return ret;
			}
			else if (ret == ConditionCode::FAILED)
			{
				//Set index and flag done
				m_ChildArrayPos = 1;
				m_ParentComplete = true;
				m_ConditionStatus = ConditionStatus::COMPLETE;
				//Return the condition code
				return ret;
			}
			//Unknown return code
			else
			{
				msg_assert(false, "ConditionNodeFunction(): Returned unknown condition code, unable to process.");
				//Flag parent as complete, but unable to provide condition index
				m_ParentComplete = true;
				return ConditionCode::CONDITION_ERROR;
			}

		}
	}

	//Should never be hit
	assert(false);
	return ConditionCode::CONDITION_ERROR;
}

ConditionCode AI_BehaviourTree_Node::SelectorNodeFunction()
{
	//Increment current child index
	++m_ChildArrayPos;

	//See if composite has completed its run
	if (m_ChildArrayPos >= m_ChildIndexes.size())
	{
		//Flag done (model will reset values)
		m_ParentComplete = true;
		return ConditionCode::SUCCESS;
	}

	return ConditionCode::RUNNING;
}
