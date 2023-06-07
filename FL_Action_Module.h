#pragma once

#include "AI_Unit_Controller_00.h"		//Algorithms
#include "GameTimer.h"

/*
	Action Module for use with FL Algorithm. Uses algorithms defined by "AI_Unit_Controller_00" as actions.
*/
class FL_Action_Module
{
public:


	////////////
	/// Data ///
	////////////


	///////////////////
	/// Definitions ///
	///////////////////

	enum class BehaviourState : unsigned
	{
		AGGRO,
		BALANCED,
		DEFENSIVE,
		UNDEFINED
	};


	////////////////////
	/// Constructors ///
	////////////////////

	FL_Action_Module() { Init(); }
	~FL_Action_Module() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	bool Update(GameTimer& gt);

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


	bool RunAggroBehaviourLine();
	bool RunBalancedBehaviourLine();
	bool RunDefensiveBehaviourLine();

	////////////
	/// Data ///
	////////////

	//Data for pacing the actions of the FL model
	float m_Elapsed = 0.f;
	float m_ActionDelay = 1.0f;
	unsigned m_CurrentActionStep = 0;
	unsigned m_ActionStepMax = 0;
	bool m_ActionCycleComplete = false;

	//Current behaviour state that the model is running under
	BehaviourState m_BehaviourState = BehaviourState::UNDEFINED;
};