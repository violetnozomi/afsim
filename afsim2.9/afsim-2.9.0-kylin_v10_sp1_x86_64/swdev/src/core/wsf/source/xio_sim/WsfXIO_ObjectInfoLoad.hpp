// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFXIO_OBJECTINFOLOAD_HPP
#define WSFXIO_OBJECTINFOLOAD_HPP

#include "wsf_export.h"

class WsfArticulatedPart;
class WsfEM_Rcvr;
class WsfEM_Xmtr;
class WsfMover;
class WsfPlatform;
class WsfPlatformPart;
class WsfProcessor;
class WsfSensor;
class WsfXIO_ArticulatedPartInfo;
class WsfXIO_CommInfo;
class WsfXIO_EM_RcvrInfo;
class WsfXIO_EM_XmtrInfo;
class WsfXIO_MoverInfo;
class WsfXIO_PlatformInfo;
class WsfXIO_PlatformPartInfo;
class WsfXIO_ProcessorInfo;
class WsfXIO_SensorInfo;
class WsfXIO_Simulation;

namespace wsf
{
namespace comm
{
class Comm;
}
} // namespace wsf

//! A collection of routines that populate XIO 'info' structures from their corresponding WSF object.
//!
//! These routines operate only within WSF simulations that are using the XIO simulation capability.
//! The XIO simulation side will call these routines to populate XIO 'info' structures from their
//! corresponding WSF object. The data can then be sent to any XIO application - even applications
//! that don't use WSF objects.
namespace WsfXIO_ObjectInfoLoad
{
// NOTE: The routines could have all been named 'Load', but it causes a lot of casting when
//       Load<X> for a derived class calls its base class Load<Y>.

WSF_EXPORT void Load_EM_Rcvr(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_EM_RcvrInfo& aInfo, const WsfEM_Rcvr& aRcvr);

WSF_EXPORT void Load_EM_Xmtr(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_EM_XmtrInfo& aInfo, const WsfEM_Xmtr& aXmtr);

WSF_EXPORT void Load_PlatformPart(WsfXIO_Simulation&       aXIO_Simulation,
                                  WsfXIO_PlatformPartInfo& aInfo,
                                  const WsfPlatformPart&   aPart);

WSF_EXPORT void Load_ArticulatedPart(WsfXIO_Simulation&          aXIO_Simulation,
                                     WsfXIO_ArticulatedPartInfo& aInfo,
                                     const WsfArticulatedPart&   aPart);

WSF_EXPORT void Load_Mover(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_MoverInfo& aInfo, const WsfMover& aMover);

WSF_EXPORT void Load_Comm(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_CommInfo& aInfo, const wsf::comm::Comm& aComm);

WSF_EXPORT void Load_Processor(WsfXIO_Simulation&    aXIO_Simulation,
                               WsfXIO_ProcessorInfo& aInfo,
                               const WsfProcessor&   aProcessor);

WSF_EXPORT void Load_Sensor(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_SensorInfo& aInfo, const WsfSensor& aSensor);

WSF_EXPORT void Load_Platform(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_PlatformInfo& aInfo, const WsfPlatform& aPlatform);

WSF_EXPORT WsfXIO_PlatformPartInfo* CreatePlatformPart(WsfXIO_Simulation& aXIO_Simulation, WsfPlatformPart& aPart);
} // namespace WsfXIO_ObjectInfoLoad

#endif
