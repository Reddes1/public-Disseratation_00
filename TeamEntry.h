#pragma once

#include "Entity_GameUnit.h"		//Gameplay Unit

#include "GameConstants.h"			//Game values

/*
	Class holding all the details and behaviours relevant to a team in the game.
*/

class TeamEntry
{
public:

	////////////////////
	/// Constructors ///
	////////////////////

	TeamEntry() { Init(); }
	TeamEntry(short teamID);
	TeamEntry(short teamID, int unitsCap);
	~TeamEntry() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	//Calls update for each unit in team
	void UpdateUnits(GameTimer& gt);
	//Calls render for each unit in team
	void RenderUnits(DirectX::SpriteBatch& sb, DirectX::DescriptorHeap* heap);

	//Finds a free unit (if available), and initialises with call data (call data pending development)
	bool InitialiseNewUnit(unsigned unitTypeID, DirectX::XMFLOAT2 pos);
	//Allocates for FL module using file with flag to set override state
	bool InitialiseFLModule(std::string aiFP, unsigned configIndex, bool enableOverride);

	//Enables all units in the team, if active (Calls units EnableUnit Function)
	void EnableAllUnits();
	//Scrubs the team down, disabling flags so that they can get re-used (i.e fresh game starting)
	void DisableAllUnits();

	//Update current AI
	bool UpdateTeamAI(GameTimer& gt);
	//Indrection to call any AI related resets on units
	void ResetTeamAIData();

	//Sets alive count to 0S
	void ClearUnitsAliveCount() { m_UnitsAlive = 0; }


	///////////
	/// Get ///
	///////////

	FL_Controller* GetFLController() { return m_FLModule; }

	short GetTeamID() { return m_TeamID; }
	bool GetEliminatedFlag() { return m_Eliminated; }

	int GetUnitsAliveCount() { return m_UnitsAlive; }
	int GetMaxUnitsCap() { return m_UnitsCap; }

	int GetCurrentFunds() { return m_Funds; }
	int GetFundsCap() { return m_FundsCap; };


	//Finds any inactive unit in team unit array
	Entity_GameUnit* FindFreeUnit();
	Entity_GameUnit* GetUnitAtIndex(unsigned index) { return m_Units[index]; }
	//Looks for any unit at the given coordinates, with optional flag to only find currently active units
	Entity_GameUnit* GetUnitByCoordinate(int x, int y, bool retOnlyIfActive = true);
	//Get a units index in array by pointer comparison
	bool FindUnitIndexByPointer(unsigned& index, Entity_GameUnit* unit);

	size_t GetUnitContainerSize() { return m_Units.size(); }

	//Get the average hp of the teams currently alive units
	float GetTeamsAverageHP(unsigned avgType = 0);


	///////////
	/// Set ///
	///////////

	//This should only really be set once when the team is created, but you can manually set this here
	void SetTeamID(short newID) { m_TeamID = newID; }
	//For win condition evaluation
	void SetEliminationFlag(bool elimState) { m_Eliminated = elimState; }
	//Enables the team to pilot the units it is responsible (calling AI update on units)
	void SetAIPilotFlag(bool aiEnabled) { m_AIPilotFlag = aiEnabled; }
	//Cycle through each unit and enable the override flag (flags for this team as well as part of algorithm requirements)
	void SetAIOverrideFlagForUnits(bool overrideFlag);

	/*
		Add/Subtract from the teams funds, which defaults to clamping funds between 0 and the cap always.
		Optional control flags for determining the behaviour of the call.
		When used with external state controls, can allow for scenarios such as adding funds that break the cap temporarily or limited overspending etc.
	*/
	void IncrementFunds(int newFunds, bool clampToCap = true, bool clampToZero = true, bool breakCapRule = false, bool breakZeroRule = false);
	//Unrestricted increment call with no control or cap considerations
	void IncrementFundsUnrestricted(int newFunds) { m_Funds += newFunds; }
	//Sets funds to a fixed number. Ignores the funds cap
	void SetFundsAmount(int newFunds) { m_Funds = newFunds; }
	//Sets the the maximum amount of funds this team can have
	void SetFundsCap(int newFundCap) { m_FundsCap = newFundCap; }

	
	//Add or remove from the teams unit cap (will not allow it to go below zero)
	void IncrementUnitsCap(int newCap);
	//Inc/Dec the amount of units
	void IncrementUnitsAlive(int count);
	void SetUnitsCap(unsigned newCap) { m_UnitsCap = newCap; }


private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();
	void Release();

	float TeamAverageType00();

	//////////
	/// AI ///
	//////////

	//Override update that is run once per team turn, passing its determined state to each unit
	bool FLOverrideUpdate(GameTimer& gt);
	bool EndOfAITurnCleanUp();
	bool UnitIndexValidation();
	bool EndTeamTurn();



	////////////
	/// Data ///
	////////////

	//Container of teams units
	std::vector<Entity_GameUnit*> m_Units;
	
	//Internal team ID (this is a secondary ID as manager can use container index for this purpose)
	short m_TeamID = -1;
	
	//Each team will have funds for purchasing units, so store current and funds cap
	int m_Funds = TEAM_STARTING_FUNDS;
	int m_FundsCap = TEAM_MAX_FUNDS;
	//Keep track of the number of active units (i.e number of units currently on the field of play)
	int m_UnitsAlive = 0;
	//Keep information on a unit allowed cap
	unsigned m_UnitsCap = MAX_TEAM_SIZE;
	//Flagging for team being eliminated
	bool m_Eliminated = false;


	//////////
	/// AI ///
	//////////

	//Optional FL AI Module, used to override unit states with own determined state
	FL_Controller* m_FLModule = nullptr;
	//Control flag for enabled/disabling the ability to override unit states
	bool m_FLOverrideFlag = false;
	//Run once check for determining state
	bool m_FLRunOnceDone = false;

	bool m_AIPilotFlag = false;
	unsigned m_CurrentUnitAIIndex = 0;
};