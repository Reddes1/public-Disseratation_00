#pragma once

#include <unordered_map>

class FL_RuleModule
{
public:

	~FL_RuleModule() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	//Initialises the rulesbase with an expected key length, as well as minimum key depth (i.e 0-2 or 0-4)
	//Each entry is given 0 as initialisation value
	void InitRulesContainer(unsigned keyDepth, unsigned strLength);

	//Store names that relate to ruleset output (pre-clears existing names)
	void InitStateNames(std::vector<std::string>& names);
	//Store individual name (does push back so ensure it aligns naturally)
	void InitIndividualName(std::string name) { m_StateNames.push_back(name); }
	//Clear names vector
	void ClearNames() { m_StateNames.clear(); }

	///////////
	/// Set ///
	///////////

	//Using key, set value of rule
	bool SetValueAtKey(std::string key, unsigned value);
	//Using containers, set values at each of the keys provided
	bool SetNumerousValues(std::vector<std::string>& keys, std::vector<unsigned>& values);

	void SetOutputSetName(std::string name) { m_OutputSetName = name; }

	///////////
	/// Get ///
	///////////

	unsigned GetStateViaKey(std::string key);
	unsigned GetActiveState() { return m_ActiveState; }
	std::string GetActiveStateName() { return m_StateNames[m_ActiveState]; }
	std::string GetStateNameAtIndex(unsigned index);
	std::string GetOutputSetName() { return m_OutputSetName; }
	std::string GetLastRuleKey() { return m_LastKey; }

private:

	//////////////////
	/// Operations ///
	//////////////////

	void Release();

	//Start point for recursive algorithm
	void GetPermutations(char* str, unsigned keyDepth, unsigned strLength);
	//Recursive algorithm
	void StorePermutationR(char* str, char* tempStr, unsigned keyDepth, unsigned strLength, unsigned index);

	//Sets the active state using key (ran at end of algorithm)
	void SetActiveState(std::string& key);

	////////////
	/// Data ///
	////////////

	//Name for Outputs
	std::string m_OutputSetName;
	//Store the names for states relative 
	std::vector<std::string> m_StateNames;
	//Store the last key used to access rules table
	std::string m_LastKey = "N/A";
	//Store the last determined state (relates to index of state names, define enum elsewhere)
	unsigned m_ActiveState = 0;


	//Storage space for all possible code combinations
	std::unordered_map<std::string, unsigned> m_RulePermutations;

};