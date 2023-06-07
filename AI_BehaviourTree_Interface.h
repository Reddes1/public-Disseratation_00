#pragma once
#include <functional>		//Function pointers
#include <stack>			//Parent Container

#include "AI_BehaviourTree_Node.h"	//Tree Nodes
#include "AI_Unit_Controller_00.h"	//Unit + Actions

/*
	Template class for defining different behaviour tree implimentations and designs.
*/
class AI_BehaviourTree_Interface
{
public:

	////////////////////
	/// Constructors ///
	////////////////////

	AI_BehaviourTree_Interface() { }
	~AI_BehaviourTree_Interface() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	//Primary Algorithm Call
	virtual ConditionCode RunAlgorithm();

	//Primary BT Model
	void CompileAlgorithm_00();

	///////////
	/// Set ///
	///////////

	void BindUnitController(AI_Unit_Algorithms_00* controller);

protected:

	///////////////////
	/// Definitions ///
	///////////////////

	typedef AI_BehaviourTree_Node::NodeType NT;
	typedef AI_BehaviourTree_Node::NodeFunc NF;
	typedef AI_BehaviourTree_Node::ConditionStatus CS;
	typedef AI_Unit_Algorithms_00 UC;

	//////////////////
	/// Operations ///
	//////////////////

	ConditionCode RunRootAlgorithm(NT& type);
	ConditionCode RunCompositeAlgorithm(NT& type);
	ConditionCode RunConditionAlgorithm(NT& type);
	ConditionCode RunSelectorAlgorithm(NT& type);

	void Init();
	void Release();

	//Checks if node at index is a parent type, and stores it on the stack if so
	bool AddIndexIfParent(unsigned index);

	////Compiles the behaviour tree function pointers into container (uses L-R execution model)
	//virtual bool CompileBehaviourTree() = 0;

	/////////////////////
	/// Tree Building ///
	/////////////////////

	//Build Root Parent + Prep/Error Checking (Run this only ever once during building)
	void BuildRootNode(AI_BehaviourTree_Node& node, unsigned& nodeCounter, unsigned childCount, std::stack<unsigned>& childCounterStack);
	//Build composite-based node
	void BuildCompositeNode(AI_BehaviourTree_Node& node, unsigned& nodeCounter, unsigned childCount, std::stack<unsigned>& childCounterStack);
	//Builds function-based parent nodes (if building condition, flag optional type 1 or 2)
	void BuildConditionParentNode(AI_BehaviourTree_Node& node, unsigned& nodeCounter, std::stack<unsigned>& childCounterStack, NF& function, bool conditionType1, bool conditionType1RetCon = false);
	//Builds selector-based node
	void BuildSelectorNode(AI_BehaviourTree_Node& node, unsigned& nodeCounter, unsigned childCount, std::stack<unsigned>& childCounterStack);
	//Builds action-based node
	void BuildActionNode(AI_BehaviourTree_Node& node, unsigned& nodeCounter, std::stack<unsigned>& childCounterStack, NF& function);

	//Clears a layer off current stacks
	void ClearParentLayer(std::stack<unsigned>& childCounterStack);

	////////////
	/// Data ///
	////////////

	//Hold all the node in contiguous memory (will be accessed by index)
	std::vector<AI_BehaviourTree_Node> m_Nodes;
	//Hold indexes to all the seen parent indexes, winding down when the current
	//parent node has resolved
	std::stack<unsigned> m_ParentIndexes;
	//Binding pointer for unit action module (and the unit it is attached to)
	UC* m_UnitController = nullptr;
};