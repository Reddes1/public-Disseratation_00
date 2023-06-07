#include "FL_Members.h"

#include <assert.h>

///////////////////////
/// Triangle Member ///
///////////////////////

void FL_Member_Triangle::InitMember(std::string memberName, std::vector<float>& values)
{
	//Set name
	m_Name = memberName;

	//Store as many values as needed
	for (unsigned i(0); i < static_cast<unsigned>(AnchorPoints::COUNT); ++i)
	{
		m_AnchorPoints[i] = values[i];
	}

	//Assert points properly represent a triangle shape
	assert(m_AnchorPoints[0] <= m_AnchorPoints[1]);
	assert(m_AnchorPoints[2] >= m_AnchorPoints[1]);

	//Calculate the value range
	CalculateValueRange();
}

float FL_Member_Triangle::CalculateMembership(float value)
{
	//Run preliminary checks to see if the membership value is automatically 0 or 1
	if (value == m_AnchorPoints[1])
		return m_MembershipValue = 1.f;
	if (value <= m_AnchorPoints[0] || value >= m_AnchorPoints[2])
		return m_MembershipValue = 0.f;

	//Value is within range, determine what type of calculation to run based on
	//where it places on the triangle (slope down or up).
	Slopes slope = Slopes::UNDEFINED;
	if (value > m_AnchorPoints[0] && value < m_AnchorPoints[1])
		slope = Slopes::SLOPE_UP;
	else
		slope = Slopes::SLOPE_DOWN;

	//Run different calculation based on sub-shape id
	switch (slope)
	{
	case Slopes::SLOPE_UP:

		//Calculate slope membership
		return m_MembershipValue = (value - m_AnchorPoints[0]) / CalculateValueRange(m_AnchorPoints[0]);
		break;

	case Slopes::SLOPE_DOWN:

		//Calculate slope membership
		return m_MembershipValue = (m_AnchorPoints[2] - value) / CalculateValueRange(m_AnchorPoints[2]);
		break;
	}

	//Error has occured if this is hit
	assert(false);
	return 0.0f;
}

void FL_Member_Triangle::Init()
{
	//Reserve and insert start values
	m_AnchorPoints.reserve(static_cast<unsigned>(AnchorPoints::COUNT));
	m_AnchorPoints.insert(m_AnchorPoints.begin(), static_cast<unsigned>(AnchorPoints::COUNT), 0);
}

float FL_Member_Triangle::CalculateValueRange(float point)
{
	//Is this point down the left slope?
	if (point <= m_AnchorPoints[1])
		return m_AnchorPoints[1] - point;
	//Right slope
	else
		return point - m_AnchorPoints[1];

	return 0.0f;
}

////////////////////////
/// Trapezoid Member ///
////////////////////////

void FL_Member_Trapezoid::InitMember(std::string memberName, std::vector<float>& values)
{
	//Set name
	m_Name = memberName;

	//Store as many values as needed
	for (unsigned i(0); i < static_cast<unsigned>(AnchorPoints::COUNT); ++i)
	{
		m_AnchorPoints[i] = values[i];
	}

	//Assert points properly represent a trapezoid shape
	assert(m_AnchorPoints[0] <= m_AnchorPoints[1]);		//Left Slope
	assert(m_AnchorPoints[3] >= m_AnchorPoints[2]);		//Right Slope
	assert(m_AnchorPoints[2] >= m_AnchorPoints[1]);		//Top

	//Calculate the value range
	CalculateValueRange();
}

float FL_Member_Trapezoid::CalculateMembership(float value)
{
	//Run preliminary checks to see if the membership value is automatically 0 or 1
	if ((value >= m_AnchorPoints[1] && value <= m_AnchorPoints[2]) || value == m_AnchorPoints[1] || value == m_AnchorPoints[2])
		return m_MembershipValue = 1.f;
	if (value <= m_AnchorPoints[0] || value >= m_AnchorPoints[3])
		return m_MembershipValue = 0.f;

	//Edge case: if point 0 & 1 are both equal to 0 then the flat portion goes from 0-2 so check for this
	if (m_AnchorPoints[0] == m_AnchorPoints[1] && value <= m_AnchorPoints[2])
		return m_MembershipValue = 1.f;

	//Value is within range, determine what type of calculation to run based on
	//where it places on the trapezoid (slope down or up).
	Slopes slope = Slopes::UNDEFINED;
	if (value > m_AnchorPoints[0] && value < m_AnchorPoints[1])
		slope = Slopes::SLOPE_UP;
	else
		slope = Slopes::SLOPE_DOWN;

	//Run different calculation based on sub-shape id
	switch (slope)
	{
	case Slopes::SLOPE_UP:



		//Calculate slope membership
		return m_MembershipValue = (value - m_AnchorPoints[0]) / CalculateValueRange(m_AnchorPoints[0]);
		break;

	case Slopes::SLOPE_DOWN:

		//Calculate slope membership
		return m_MembershipValue = (m_AnchorPoints[3] - value) / CalculateValueRange(m_AnchorPoints[3]);
		break;
	}

	return 0.0f;
}

void FL_Member_Trapezoid::Init()
{
	//Reserve and insert start values
	m_AnchorPoints.reserve(static_cast<unsigned>(AnchorPoints::COUNT));
	m_AnchorPoints.insert(m_AnchorPoints.begin(), static_cast<unsigned>(AnchorPoints::COUNT), 0);
}

float FL_Member_Trapezoid::CalculateValueRange(float point)
{
	//Is this point down the left slope?
	if (point <= m_AnchorPoints[1])
		return m_AnchorPoints[1] - point;
	//Right slope
	else
		return point - m_AnchorPoints[2];

	return 0.0f;
}
