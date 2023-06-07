#pragma once

#include "FL_Member_Interface.h"	//Parent Class

///////////////////////
/// Triangle Member ///
///////////////////////

class FL_Member_Triangle : public FL_Member_Interface
{
public:

	///////////////////
	/// Definitions ///
	///////////////////

	enum class AnchorPoints : unsigned short
	{
		POINT_0,
		POINT_1,
		POINT_2,
		COUNT
	};

	////////////////////
	/// Constructors ///
	////////////////////

	FL_Member_Triangle() { Init(); }


	/////////////////
	/// Overrides ///
	/////////////////

	//Main initialisation function
	void InitMember(std::string memberName, std::vector<float>& values) override;

	//Main calculation function
	float CalculateMembership(float value);

private:

	///////////////////
	/// Definitions ///
	///////////////////

	//Slope descriptions used in membership calculation
	enum class Slopes : unsigned short
	{
		SLOPE_DOWN,
		SLOPE_UP,
		UNDEFINED
	};

	//////////////////
	/// Operations ///
	//////////////////

	//Basic Init
	void Init();

	//Calculates the mid point on a slope for membership calculations
	void CalculateMidPoint() { m_AnchorPoints[1] = (m_AnchorPoints[2] + m_AnchorPoints[0]) / 2.f; }
	void CalculateValueRange() { m_Range = m_AnchorPoints[2] - m_AnchorPoints[0]; }
	//Calculates the range of values from the midpoint to given value (used in member calculation)
	float CalculateValueRange(float point);

};


////////////////////////
/// Trapezoid Member ///
////////////////////////

class FL_Member_Trapezoid : public FL_Member_Interface
{
public:

	///////////////////
	/// Definitions ///
	///////////////////

	enum class AnchorPoints : unsigned short
	{
		POINT_0,
		POINT_1,
		POINT_2,
		POINT_3,
		COUNT
	};

	////////////////////
	/// Constructors ///
	////////////////////

	FL_Member_Trapezoid() { Init(); }


	/////////////////
	/// Overrides ///
	/////////////////

	//Main initialisation function
	void InitMember(std::string memberName, std::vector<float>& values) override;

	//Main calculation function
	float CalculateMembership(float value);

private:

	///////////////////
	/// Definitions ///
	///////////////////

	//Slope descriptions used in membership calculation
	enum class Slopes : unsigned short
	{
		SLOPE_DOWN,
		SLOPE_UP,
		UNDEFINED
	};

	//////////////////
	/// Operations ///
	//////////////////

	//Basic Init
	void Init();

	//Calculates the mid point on a slope for membership calculations
	void CalculateMidPoint() { m_AnchorPoints[1] = (m_AnchorPoints[2] + m_AnchorPoints[0]) / 2.f; }
	void CalculateValueRange() { m_Range = m_AnchorPoints[2] - m_AnchorPoints[0]; }
	//Calculates the range of values from the midpoint to given value (used in member calculation)
	float CalculateValueRange(float point);

};