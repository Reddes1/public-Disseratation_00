#pragma once

/*
	Simple sub-manager for defining and tracking any number of states for program control in game modes
	or other managers.
*/

class SM_GameStateManager
{
public:


	////////////
	/// Data ///
	////////////


	///////////////////
	/// Definitions ///
	///////////////////

	/*
		Defines the states for the active game state.
	*/
	enum class ActiveGameState : unsigned short
	{
		FREE_LOOK,
		UNIT_ACTION_MENU_OPEN,
		UNIT_INFO_WINDOW_OPEN,
		GAME_MASTER_MENU_OPEN,
		BUILDING_ACTION_MENU_OPEN,
		UNIT_MOVING,
		UNIT_TARGETING,
		UNIT_COMBAT_START,
		UNIT_COMBAT_END,

		////////////////////////////
		/// Win Condition States ///
		////////////////////////////

		WIN_CON_ELIMINATION,
		WIN_CON_HQ_CAPTURED,

		STATE_COUNT
	};
	enum class GamePrepState : unsigned short
	{
		STATE_COUNT
	};


	////////////////////
	/// Constructors ///
	////////////////////

	SM_GameStateManager() { Init(); }
	~SM_GameStateManager() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////


	///////////
	/// Get ///
	///////////

	ActiveGameState GetActiveGameState()			  { return m_ActiveGameState; }
	GamePrepState GetGamePrepState()				  { return m_GamePrepState; }

	ActiveGameState GetPreviousActiveGameState()	  { return m_PreviousGameState; }
	GamePrepState GetPreviousGamePrepState()		  { return m_PreviousPrepState; }

	//Checks to see if a state change occured. This should be called in the active modes update cycle.
	//If update flag returns true, the game mode should do with that information what they need then flag it as false when done.
	bool CheckForActiveStateChange()				  { return m_ActiveStateChanged; }
	bool CheckForPrepStateChange()					  { return m_PrepStateChanged; }


	///////////
	/// Set ///
	///////////

	void SetActiveGameState(ActiveGameState newState); 
	void SetGamePrepState(GamePrepState newState);

	//Should be called and set true following confirmation on state change by modes
	void DisabledActiveStateChangeFlag()			 { m_ActiveStateChanged = false; }
	void DisablePrepStateChangeFlag()				 { m_PrepStateChanged = false; }


private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();
	void Release();
	
	////////////
	/// Data ///
	////////////

	ActiveGameState m_ActiveGameState = ActiveGameState::FREE_LOOK;
	ActiveGameState m_PreviousGameState = ActiveGameState::STATE_COUNT;

	GamePrepState m_GamePrepState = GamePrepState::STATE_COUNT;
	GamePrepState m_PreviousPrepState = GamePrepState::STATE_COUNT;

	//Updater flag(s) to let modes know that a state change happened, and they may do stuff with that information
	bool m_ActiveStateChanged = false;
	bool m_PrepStateChanged = false;

};