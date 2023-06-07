#pragma once
#include "AI_BehaviourTree_Interface.h"		//Parent
#include "AI_Unit_Controller_00.h"			//Unit Action Algorithms + Data

/*
	Behaviour tree model 00.
*/
class AI_BehaviourTree_00 : public AI_BehaviourTree_Interface
{
public:


	////////////
	/// Data ///
	////////////


	///////////////////
	/// Definitions ///
	///////////////////


	////////////////////
	/// Constructors ///
	////////////////////

	AI_BehaviourTree_00() { Init(); }
	~AI_BehaviourTree_00() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////


	///////////
	/// Get ///
	///////////


	///////////
	/// Set ///
	///////////


private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();
	void Release();

	////////////
	/// Data ///
	////////////

	//Track current node index (relative to vector)
	unsigned m_CurrentNodeIndex = 0;
};