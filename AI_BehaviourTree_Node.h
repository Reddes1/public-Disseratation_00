#pragma once
#include <functional>			//Function Pointers

#include "AI_Definitions.h"
#include "AI_Unit_Controller_00.h"	//Function Source

/*
	Store information require for nodes to operate properly within tree structure
*/
class AI_BehaviourTree_Node
{
public:

	///////////////////
	/// Definitions ///
	///////////////////

	enum class NodeType : unsigned
	{
		ROOT,
		ACTION,
		CONDITION,
		SELECTOR,
		COMPOSITE,
		SEQUENCE,
		UNDEFINED
	};

	enum class ConditionStatus : unsigned
	{
		COMPLETE,
		COMPLETE_UNMET_CON,
		INCOMPLETE,
		CON_RESOLVED,
		COUNT
	};

	//Skip code used for condition node for when any resolution has no linked task
	static const unsigned INVALID_CODE = 0xFFFFFFFF;

	//Default function definition. Must return a condition code and takes unsigned value (algorithm ID)
	typedef std::function<ConditionCode()> NodeFunc;


	////////////////////
	/// Constructors ///
	////////////////////

	AI_BehaviourTree_Node() { }
	~AI_BehaviourTree_Node() { }

	////////////////// 
	/// Operations ///
	//////////////////


	//Run node operation, returning condition code
	ConditionCode RunNodeFunction();

	//Resets all the functionality pieces of the (called when leaving a parent node)
	void ResetNode();

	//Call once, then call "InsertNewChildIndex()" for each child (nodePosition MUST be = 0)
	void InitialiseRoot(unsigned nodePositionIndex, unsigned childCount);

	//Call once (no other calls required)
	void InitialiseAction(NodeFunc func, unsigned nodePositionIndex);

	//Call once, then call "InsertNewChildIndex()" for each child
	void InitialiseComposite(unsigned nodePositionIndex, unsigned childCount);

	//Initialises the node as Condition Type 1 (only one condition), so need to define what the return type is.
	//Insert ONE child index with: InsertNewChildIndex()
	void InitialiseConditionType1(NodeFunc func, unsigned nodePositionIndex, bool retCondition);
	//Initialises the node as Condition Type 2 (2 return types), index 0 = true, index 1 = false.
	//Insert TWO child indexes with: InsertNewChildIndex()
	void InitialiseConditionType2(NodeFunc func, unsigned nodePositionIndex);

	//Call once, then call "InsertNewChildIndex()" for each child
	void InitialiseSelector(unsigned nodePositionIndex, unsigned childCount);



	//Insert child postion index at index (will error if attempting to insert at unspecified target index)
	void InsertChildIndex(unsigned childIndex, unsigned indexPosition);


	///////////
	/// Get ///
	///////////

	NodeType GetNodeTypeID() { return m_NodeTypeID; }
	unsigned GetParentIndex() { return m_ParentNodeIndex; }
	bool GetParentCompleteStatus() { return m_ParentComplete; }
	//Gets the current child index that composite is pointing at
	unsigned GetCurrentChildIndex() { return m_ChildIndexes[m_ChildArrayPos]; }


	///////////
	/// Set ///
	///////////

	void SetNodeID(NodeType nodeID) { m_NodeTypeID = nodeID; }
	void SetParentIndex(unsigned index) { m_ParentNodeIndex = index; };
	void SetParentCompleteStatus(bool done) { m_ParentComplete = done; }


	/////////////////
	/// Condition ///
	/////////////////

	bool GetConditionIndexValid() { return m_SingleConditionRetReq; }
	bool IsConditionSingleOnly() { return m_SingleConditionOnly; }
	ConditionStatus GetConditionStatus() { return m_ConditionStatus; }
	void FlagConditionResolved() { m_ConditionStatus = ConditionStatus::CON_RESOLVED; }


private:

	//////////////////
	/// Operations ///
	//////////////////
	
	ConditionCode RootNodeFunction();
	ConditionCode ActionNodeFunction();
	ConditionCode CompositeNodeFunction();
	ConditionCode ConditionNodeFunction();
	ConditionCode SelectorNodeFunction();

	///////////////////
	/// Shared Data ///
	///////////////////

	//Store parent node child indexes
	std::vector<unsigned> m_ChildIndexes;

	//This nodes index in array
	unsigned m_NodeIndex = INVALID_CODE;
	//This nodes parent (if root, this will not have one)
	unsigned m_ParentNodeIndex = INVALID_CODE;
	//Define what type of node this is
	NodeType m_NodeTypeID = NodeType::UNDEFINED;

	//Function that this node will execute
	NodeFunc m_Func;

	//Points the current child node being accessed
	unsigned m_ChildArrayPos = 0;

	//Flag to indicate the parent flag (regardless of type) has resolved
	bool m_ParentComplete = false;

	//////////////////////
	/// Condition Data ///
	//////////////////////

	//For single condition mode
	bool m_SingleConditionOnly = false;
	bool m_SingleConditionRetReq = false;
	//Flags that node has specifically resolved a node to point to after condition resolution
	ConditionStatus m_ConditionStatus = ConditionStatus::INCOMPLETE;
};