#pragma once

#include "RapidJSONLoaderUtils.h"	//File Loading

#include "FL_Set.h"					//Sets
#include "FL_RuleModule.h"			//Rules

class FL_Controller
{
public:

	////////////////////
	/// Constructors ///
	////////////////////

	~FL_Controller();

	//Inits all components for the controller from file (Clears any current sets)
	void InitFromFile(std::string fp, unsigned configIndex);

	//Run logic algorithm for all sets using array of values (ordered for set ordering), returning the resolved state as unsigned
	unsigned RunLogicAlgorithm(std::vector<float>& values);

	//Runs logic algorithm but saves out input set states + output state (in that order), and pastes result to cmd window
	void RunLogicSimulationCMD(std::vector<float>& values, std::vector<unsigned>& saveContainer);

	///////////
	/// Get ///
	///////////

	FL_Set* GetSetAtIndex(unsigned index);
	FL_RuleModule& GetRuleset() { return m_Rules; }
	unsigned GetSetCount() { return static_cast<unsigned>(m_Sets.size()); }

	///////////
	/// Set ///
	///////////

	void SetName(std::string name) { m_Name = name; }


private:

	//////////////////
	/// Operations ///
	//////////////////

	void Release();

	////////////
	/// Data ///
	////////////

	//Controller Name
	std::string m_Name = "N/A";
	//Fuzzy set modules
	std::vector<FL_Set*> m_Sets;
	//Store rule module
	FL_RuleModule m_Rules;
};