#pragma once

#include "EM_EntityInterface.h"		//Parent

//EM Components
#include "EM_Sprite.h"				
#include "EM_Animator.h"			

#include "GameTypes.h"				//Unit Data

#include "AI_Unit_Controller_00.h"		//Action AI
#include "FL_Controller.h"				//Fuzzy Logic System
#include "AI_BehaviourTree_Interface.h" //BT AI Model

/*
	Main gameplay unit for this game.
*/
class Entity_GameUnit : public EM_EntityInterface2D
{
public:

	////////////
	/// Data ///
	////////////


	////////////////////
	/// Constructors ///
	////////////////////

	Entity_GameUnit() { Init(); }
	~Entity_GameUnit() { Release(); }


	/////////////////
	/// Overrides ///
	/////////////////

	void Update(GameTimer& gt) override;
	void Render(DirectX::SpriteBatch& batch, DirectX::DescriptorHeap* heap) override;

	//Optional Overrides (Uncomment as needed)
	void ResetEntityToDefaults() override { }


	//////////////////
	/// Operations ///
	//////////////////

	//Update the AI Module(s)
	ConditionCode UpdateBehaviourAI(GameTimer& gt);

	//Re-enables the unit for play, setting flags, colour etc
	void EnableUnit();
	//Disables the unit for play (i.e when it has no actions left)
	void DisableUnit();

	//Allocates memory for AI module and loads FL setup from file
	void InitialiseFLModule(std::string aiFP, unsigned configIndex);
	//Allocates memory for Action AI model and binds it to this unit
	void InitialiseActionAIModule();
	//Allocates memory for BT AI model (Action AI Module needs to be allocated before this)
	void InitialiseBTAIModule();


	///////////
	/// Get ///
	///////////

	EM_Sprite& GetBaseSprite()			{ return m_MainSprite; }
	EM_Animator& GetBaseAnimator()		{ return m_MainAnimator; }
	UnitData& GetUnitData()				{ return m_Data; }
	FL_Controller* GetFLController()	{ return m_FLModule; }

	AI_Unit_Algorithms_00* GetActionAIModule() { return m_ActionAlgorithms; }
	AI_BehaviourTree_Interface* GetBTController() { return m_BTModule; }

	unsigned GetTeamID()				{ return m_TeamID; }
	bool GetActionFlag()				{ return m_CanAct; }
	bool GetMoveFlag()					{ return m_CanMove; }
	bool GetAIPilotFlag()				{ return m_AIPilotEnabled; }
	bool GetAIUpdateFlag()				{ return m_UpdateAI; }

	//Returns value based on if any actions are available
	bool AnyActionsAvailable();


	///////////
	/// Set ///
	///////////

	void SetTeamID(unsigned newID)						{ m_TeamID = newID; }
	void SetActionFlag(bool act)						{ m_CanAct = act; }
	void SetMoveFlag(bool move)							{ m_CanMove = move; }
	void SetAIUpdateFlag(bool update)					{ m_UpdateAI = update; }
	void SetFLOverrideFlag(bool overrideFlag)			{ m_FLOverrideFlag = overrideFlag; }
	void SetFLOverrideState(FuzzyBehaviourStates state) { m_OverrideState = state; }

private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();
	void Release();

	//Run BT related AI behaviour
	ConditionCode UpdateBehaviourBT();
	//Run FL related AI behaviour
	ConditionCode UpdateBehaviourFL(GameTimer& gt);

	//Runs algorithm pre-ambles to determine unit state
	bool StartFreshCycleFL();
	//Behaviour function for defining the "AGGRO" state
	ConditionCode RunFLBehaviour_Aggro();
	//Behaviour function for defining the "BALANCED" state
	ConditionCode RunFLBehaviour_Balanced();
	//Behaviour function for defining the "DEFESIVE" state
	ConditionCode RunFLBehaviour_Defensive();


	////////////
	/// Data ///
	////////////

	//Sprite/Animator Combo
	EM_Sprite m_MainSprite;
	EM_Animator m_MainAnimator;

	//Local copy of unit data
	UnitData m_Data;

	//Team ID for the game unit
	unsigned m_TeamID = 100;

	//Action & Move Flag
	bool m_CanAct = true;
	bool m_CanMove = true;

	//////////
	/// AI ///
	//////////
	
	//Optional FL Module for Unit AI
	FL_Controller* m_FLModule = nullptr;
	//Optional BT module
	AI_BehaviourTree_Interface* m_BTModule = nullptr;
	//Optional algorithms for AI Unit actions (must be allocated for use with FL & BT modules)
	AI_Unit_Algorithms_00* m_ActionAlgorithms = nullptr;

	//Flag for AI Pilot
	bool m_AIPilotEnabled = false;
	//Update flag for team entry, flag true when any either AI cycle is complete (team will then move past this unit)
	bool m_UpdateAI = true;
	bool m_TeamAIOverride = false;
	
	//Override state & flag (can be set by team)
	FuzzyBehaviourStates m_OverrideState = FuzzyBehaviourStates::COUNT;
	bool m_FLOverrideFlag = false;


	///////////////////////////
	/// FL Action Execution ///
	///////////////////////////

	//Data for pacing the actions of the FL model
	float m_Elapsed = 0.f;
	float m_ActionDelay = 0.2f;
	unsigned m_CurrentActionStep = 0;
	//Run once flag for each update cycle to signal fresh FL cycle
	bool m_FLFreshCycleFlag = true;

};