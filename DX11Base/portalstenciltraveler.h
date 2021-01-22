#pragma once

#include "portalstencil.h"


class OBB;

class PortalStencilTraveler
{
public:
	virtual PortalStencilType GetEntrancePortal() const { return m_entrancePortal; }
	virtual void SetEntrancePortal(PortalStencilType type) { m_entrancePortal = type; }
	virtual OBB* GetOBB() { return &m_obb; }

	virtual void Swap() = 0;
	virtual dx::XMVECTOR GetTravelerPosition() const = 0;

protected:
	PortalStencilType m_entrancePortal;
	OBB m_obb;
};
