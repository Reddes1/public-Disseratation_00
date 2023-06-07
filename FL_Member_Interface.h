#pragma once

#include <string>
#include <vector>

class FL_Member_Interface
{
public:

	///////////////////
	/// Definitions ///
	///////////////////

	enum class MemberTypes
	{
		TRIANGLE,
		TRAPEZOID,
		UNDEFINED
	};

	////////////////////
	/// Constructors ///
	////////////////////


	//////////////////
	/// Operations ///
	//////////////////

	//Basic init with all values being defined, calculating the range of values
	virtual void InitMember(std::string memberName, std::vector<float>& values) = 0;

	//Takes value, calculates membership value, stores and returns it. Must be defined by child classes
	virtual float CalculateMembership(float value) = 0;

	//Calculate the range of values. Can override, default option provided via start/end of vector calculation
	virtual void CalculateValueRange() { m_Range = *m_AnchorPoints.end() - *m_AnchorPoints.begin(); }
	
	///////////
	/// Set ///
	///////////

	void SetMemberName(std::string name) { m_Name = name; }
	void SetMemberID(int id) { m_MemberID = id; }
	void SetMemberTypeID(MemberTypes typeID);

	///////////
	/// Get ///
	///////////

	std::string& GetName() { return m_Name; }
	int GetMemberID() { return m_MemberID; }
	MemberTypes GetMemberTypeID() { return m_MemberTypeID; }

	float GetValueRange() { return m_Range; }
	float GetMembershipValue() { return m_MembershipValue; }

	float GetPointAtIndex(unsigned index);


protected:

	//////////////////
	/// Operations ///
	//////////////////


	////////////
	/// Data ///
	////////////

	//Member Name
	std::string m_Name;
	//Anchor points describing the shape
	std::vector<float> m_AnchorPoints;
	//Store the range of values this member represents
	float m_Range = 0.f;
	//Calculated membership value within a range of 0-1
	float m_MembershipValue = 0.f;
	//Member ID within a given set
	int m_MemberID = -1;
	//Member type ID
	MemberTypes m_MemberTypeID = MemberTypes::UNDEFINED;


};