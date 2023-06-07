#include "FL_Set.h"

#include <assert.h>

FL_Set::~FL_Set()
{
	//Release
	for (auto& a : m_Members)
	{
		if (a != nullptr)
		{
			delete a;
			a = nullptr;
		}
	}
}

void FL_Set::InitSet(unsigned memberCount, std::vector<FL_Member_Interface::MemberTypes> types)
{
	//Setup member array
	m_Members.reserve(memberCount);
	m_Members.insert(m_Members.begin(), memberCount, nullptr);
	m_Members.shrink_to_fit();

	//Allocate for each member based on type
	for (unsigned i(0); i < memberCount; ++i)
	{
		switch (types[i])
		{
		case FL_Member_Interface::MemberTypes::TRIANGLE:
			m_Members[i] = new FL_Member_Triangle();
			m_Members[i]->SetMemberID(i);
			m_Members[i]->SetMemberTypeID(FL_Member_Interface::MemberTypes::TRIANGLE);
			break;

		case FL_Member_Interface::MemberTypes::TRAPEZOID:
			m_Members[i] = new FL_Member_Trapezoid();
			m_Members[i]->SetMemberID(i);
			m_Members[i]->SetMemberTypeID(FL_Member_Interface::MemberTypes::TRAPEZOID);
			break;
		}
	}
}

void FL_Set::CalculateMemberships(float inputValue)
{
	for (auto& a : m_Members)
		a->CalculateMembership(inputValue);
}

FL_Member_Interface* FL_Set::GetHighestMember()
{
	//Use index zero as the comparitor
	unsigned index = 0;

	//If checked member value is higher, replace
	for (unsigned i(1); i < m_Members.size(); ++i)
		if (m_Members[i]->GetMembershipValue() > m_Members[index]->GetMembershipValue())
			index = i;

	return m_Members[index];
}

FL_Member_Interface* FL_Set::GetMemberAtIndex(unsigned index)
{
	assert(index <= m_Members.size());
	return m_Members[index];
}
