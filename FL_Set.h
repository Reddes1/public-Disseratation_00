#pragma once

#include "FL_Members.h"		//Member Types

class FL_Set
{
public:
	
	///////////////////
	/// Definitions ///
	///////////////////

	////////////////////
	/// Constructors ///
	////////////////////

	~FL_Set();

	//////////////////
	/// Operations ///
	//////////////////

	//Setup set with member types, allocating for class based on type (RUN ONCE, only call again after running Release())
	void InitSet(unsigned memberCount, std::vector<FL_Member_Interface::MemberTypes> types);

	//Runs the calculation function for each member
	void CalculateMemberships(float inputValue);

	///////////
	/// Get ///
	///////////

	//Finds the member with highest membership value
	FL_Member_Interface* GetHighestMember();

	std::string& GetSetName() { return m_Name; }
	size_t GetMemberCount() { return m_Members.size(); }
	FL_Member_Interface* GetMemberAtIndex(unsigned index);
	int GetSetID() { return m_SetID; }


	///////////
	/// Set ///
	///////////

	void SetName(std::string name) { m_Name = name; }
	void SetID(int id) { m_SetID = id; }

private:


	////////////
	/// Data ///
	////////////

	//Set Name
	std::string m_Name = "Nameless";	
	//Store a vector of member parent pointer to be allocated as required
	std::vector<FL_Member_Interface*> m_Members;
	//Set ID relative to the controller
	int m_SetID = -1;

};