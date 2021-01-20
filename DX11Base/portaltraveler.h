#pragma once

#include "portal.h"


class OBB;

class PortalTraveler
{
public:
	virtual OBB* GetOBB() = 0;
	virtual PortalType GetEntrancePortal() const = 0;
	virtual void SetEntrancePortal(PortalType type) = 0;
	virtual void Swap() = 0;
};
