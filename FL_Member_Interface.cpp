#include "FL_Member_Interface.h"

#include <assert.h>

void FL_Member_Interface::SetMemberTypeID(MemberTypes typeID)
{
	//Member type shouldn't generally be set to this, so assert false just in case
	assert(typeID != MemberTypes::UNDEFINED);

	m_MemberTypeID = typeID;
}

float FL_Member_Interface::GetPointAtIndex(unsigned index)
{
	assert(index <= m_AnchorPoints.size());
	return m_AnchorPoints[index];
}
