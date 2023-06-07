#pragma once

#include "GameConstants.h"

/*
	Comment Describing Class.
*/
class SM_ScenarioManager
{
public:


	////////////
	/// Data ///
	////////////


	///////////////////
	/// Definitions ///
	///////////////////

	enum class ScenarioStates
	{
		CAPTURE_HQ,
		ELIMINATION,
		NO_WIN_CON,
		COUNT
	};


	////////////////////
	/// Constructors ///
	////////////////////

	SM_ScenarioManager() { Init(); }
	~SM_ScenarioManager() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	/*
		Call this each frame. If flagged, checks against specific scenario conditions and executes/flags 
		what it needs for the winning scenario.
	*/
	bool UpdateScenarioManager();

	///////////
	/// Get ///
	///////////

	ScenarioStates GetActiveScenario() { return m_Scenario; }
	bool GetConditionCheckFlag() { return m_ScenarioConditionCheck; }

	///////////
	/// Set ///
	///////////

	void SetActiveScenario(ScenarioStates state) { m_Scenario = state; }
	void SetConditionCheckFlag(bool check) { m_ScenarioConditionCheck = check; }

private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();
	void Release();
	
	//
	// Scenario Condition Checks
	//

	bool CaptureHQConditionCheck();

	//This scenario checks each teams unit roster, and if all units are dead then that team loses
	bool EliminationConditionCheck();

	////////////
	/// Data ///
	////////////

	ScenarioStates m_Scenario = ScenarioStates::NO_WIN_CON;
	bool m_ScenarioConditionCheck = false;

};