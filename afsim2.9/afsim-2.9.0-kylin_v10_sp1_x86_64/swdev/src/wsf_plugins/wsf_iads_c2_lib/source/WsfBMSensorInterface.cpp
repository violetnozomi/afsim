// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// UNCLASSIFIED

//#include "WsfBMSensorInterface.hpp"
//#include <logger.hpp>
//
//
//WsfBMSensorInterface::WsfBMSensorInterface()
//{
//
//   m_record = std::shared_ptr<WsfBMSensorInfoRecord> (new WsfBMSensorInfoRecord);
//#ifdef IADSLIB_POINTER_TRACE
//   HCL_TRACE_LOGGER() << "WsfBMSensorInterface(): m_record = " << m_record << std::endl;
//#endif
//}
//
//WsfBMSensorInterface::WsfBMSensorInterface(const WsfBMSensorInterface& aSrc)
//   : WsfObject (aSrc)
//{
//#ifdef IADSLIB_POINTER_TRACE
//   HCL_TRACE_LOGGER() << "WsfBMSensorInterface(const WsfBMSensorInterface& aSrc): m_record = " << m_record << " aSrc = " << aSrc.m_record << std::endl;
//#endif
//   m_record.reset (new WsfBMSensorInfoRecord);
//   *m_record = *aSrc.m_record;
//}
//
//WsfBMSensorInterface::~WsfBMSensorInterface()
//{
//#ifdef IADSLIB_POINTER_TRACE
//   HCL_TRACE_LOGGER() << "~WsfBMSensorInterface(): m_record = " << m_record << std::endl;
//#endif
//}
//
//
//WsfBMSensorInterface* WsfBMSensorInterface::Clone() const
//{
//#ifdef IADSLIB_POINTER_TRACE
//   HCL_TRACE_LOGGER() << "WsfBMSensorInterface::Clone() m_record = " << m_record << std::endl;
//#endif
//   return new WsfBMSensorInterface(*this);
//}
//
//
//std::shared_ptr<WsfBMSensorInfoRecord> &WsfBMSensorInterface::GetRawRecord()
//{
//#ifdef IADSLIB_POINTER_TRACE
//   HCL_TRACE_LOGGER() << "WsfBMSensorInterface::GetRecordRef() m_record = " << m_record << std::endl;
//#endif
//   return m_record;
//}
