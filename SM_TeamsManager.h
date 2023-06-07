#pragma once

#include "D3DUtils.h"			//Utils
#include "GameConstants.h"		//Game values
#include "Entity_GameUnit.h"	//Gameplay Unit

#include "TeamEntry.h"			//Team Class

/*
	Sub-manager that mananges each team in the game. Holds units, team information like funds etc.
*/
class SM_TeamsManager
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

	SM_TeamsManager() { Init(); }
	~SM_TeamsManager() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	//Adds a new, uninitialised team entry to the teams container. Returns new team index.
	size_t AddNewTeam();

	//Updates all 
	void UpdateAllTeams(GameTimer& gt);
	//Renders all the teams
	void RenderAllTeams(DirectX::SpriteBatch& sb, DirectX::DescriptorHeap* heap);

	//Updates specific team
	void UpdateTeamByIndex(unsigned index, GameTimer& gt) { m_Teams[index]->UpdateUnits(gt); }
	//Render specific team 
	void RenderTeamByIndex(unsigned index, DirectX::SpriteBatch& sb, DirectX::DescriptorHeap* heap) { m_Teams[index]->RenderUnits(sb, heap); }

	//Searches all teams for any unit at a given coordinate and returns it
	Entity_GameUnit* SearchAllTeamsForUnitAtCoordinate(int x, int y, bool mustBeActive);

	//Processes a given units death, cleaning up and freeing the unit from associated team
	void ProcessUnitDeath(Entity_GameUnit* deadUnit);

	//Sets all teams unit to active (call at the end of turn)
	void EnableAllTeamsAllUnits();

	//Disable all teams units
	void DisableAllTeamsAllUnits();

	//Runs a full reset on all teams.
	void ResetAllTeams();

	///////////
	/// Get ///
	///////////
	
	//Gets the team entry by container index (pass an external index value
	TeamEntry* GetTeamAtIndex(unsigned short teamIndex)	{ return m_Teams[teamIndex]; }
	//Gets the number of teams indexed in container
	size_t GetNumberOfTeams() { return m_Teams.size(); }
	//Get currently active team
	TeamIndexes GetActiveTeamIndex() { return m_ActiveTeam; }
	//Get currently active team as unsigned
	unsigned GetActiveTeamIndexAsNumber() { return static_cast<unsigned>(m_ActiveTeam); }

	///////////
	/// Set ///
	///////////

	//Sets the active team
	void SetActiveTeam(TeamIndexes teamID) { m_ActiveTeam = teamID; }
	//Increments to the next defined team, wrapping round to 00 if reaching last team
	void SetTeamToNextTeam();

	///////////////
	/// Testing ///
	///////////////
	
	//Test call to setup teams with pre-configured requirements
	void GameplayScenarioSetup_00();


private:

	//////////////////
	/// Operations ///
	//////////////////

	//Basic Init, setups with global game constants. Units are mem allocated but not initialised with any details or texture informations.
	void Init();

	void Release();
	
	////////////
	/// Data ///
	////////////
	
	//Stores each teams individual data
	std::vector<TeamEntry*> m_Teams;

	//Track the active team
	TeamIndexes m_ActiveTeam = TeamIndexes::TEAM_00;

};