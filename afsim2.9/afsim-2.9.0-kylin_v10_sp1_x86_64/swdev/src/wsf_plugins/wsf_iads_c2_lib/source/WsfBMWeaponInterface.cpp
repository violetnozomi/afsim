// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// UNCLASSIFIED

#include "WsfBMWeaponInterface.hpp"

#include <WsfWeapon.hpp>

#include <logger.hpp>

WsfBMWeaponInterface::WsfBMWeaponInterface()
{
   m_record = std::shared_ptr<WsfBMWeaponInfoRecord>(new WsfBMWeaponInfoRecord);
#ifdef IADSLIB_POINTER_TRACE
   HCL_TRACE_LOGGER() << "WsfBMWeaponInterface(): m_record = " << m_record << std::endl;
#endif
}

WsfBMWeaponInterface::WsfBMWeaponInterface(const WsfBMWeaponInterface& aSrc)
   : WsfObject(aSrc)
{
#ifdef IADSLIB_POINTER_TRACE
   HCL_TRACE_LOGGER() << "WsfBMWeaponInterface(const WsfBMWeaponInterface& aSrc): m_record = " << m_record << " aSrc = " << aSrc.m_record << std::endl;
#endif
   m_record.reset(new WsfBMWeaponInfoRecord);
   *m_record = *aSrc.m_record;
}

WsfBMWeaponInterface::~WsfBMWeaponInterface()
{
#ifdef IADSLIB_POINTER_TRACE
   HCL_TRACE_LOGGER() << "~WsfBMWeaponInterface(): m_record = " << m_record << std::endl;
#endif
}

WsfBMWeaponInterface* WsfBMWeaponInterface::Clone() const
{
#ifdef IADSLIB_POINTER_TRACE
   HCL_TRACE_LOGGER() << "WsfBMWeaponInterface::Clone() m_record = " << m_record << std::endl;
#endif
   return new WsfBMWeaponInterface(*this);
}

std::shared_ptr<WsfBMWeaponInfoRecord>& WsfBMWeaponInterface::GetRawRecord()
{
#ifdef IADSLIB_POINTER_TRACE
   HCL_TRACE_LOGGER() << "WsfBMWeaponInterface::GetRecordRef() m_record = " << m_record << std::endl;
#endif
   return m_record;
}
