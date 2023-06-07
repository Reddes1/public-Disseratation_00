#pragma once

/*
	Store common information between AI techniques, or components amongst AI techniques here
*/

///////////////////
/// Fuzzy Logic ///
///////////////////

enum class FuzzyBehaviourStates : unsigned
{
	AGGRO,
	BALANCED,
	DEFENSIVE,
	COUNT
};

///////////////////////
/// Behaviour Trees ///
///////////////////////

//Condition return codes for informing and controlling the flow of operation in AI behaviour

enum class ConditionCode : unsigned
{
	SUCCESS,
	FAILED,
	RUNNING,
	NO_OPERATION,
	ROOT_RESET,
	PARENT_RUNNING,
	PARENT_DONE,
	CONDITION_OK,
	CONDITION_ERROR,
	AI_CYCLE_COMPLETE,
	AI_NOT_OPERABLE,
	CC_ERROR
};