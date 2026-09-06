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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFDISUTIL_HPP
#define WSFDISUTIL_HPP

#include "wsf_export.h"

#include <cstddef> // for size_t

#include "UtMemory.hpp"

class DisClockTime;
class DisEntityId;
class DisPdu;
#include "DisTypes.hpp"
class WsfDisInterface;
class WsfDisPlatform;
class WsfExtInterface;
class WsfScenario;
class WsfSimulation;

//! A collection of utility functions that eliminate the need to include WsfDisInterface.hpp.
//!
//! WsfDisInterface.hpp is a very complex header file and simply including it causes the compile
//! time for a file to increase dramatically. By implementing the following methods below, which
//! are simply pass-through to the same-named method in WsfDisInterface, the inclusion of
//! WsfDisInterface.hpp was removed from many files.
//!
//! There is obviously a very small run-time penalty because of the extra level of indirection,
//! but any extra time is dwarfed by the rest of the simulation.
//! @see WsfDisInterface.
namespace WsfDisUtil
{
WSF_EXPORT bool DebugEnabled(WsfDisInterface* aInterfacePtr);

WSF_EXPORT WsfDisPlatform* FindDisPlatform(WsfDisInterface* aInterfacePtr, const DisEntityId& aEntityId);

WSF_EXPORT WsfDisPlatform* GetDisPlatform(WsfDisInterface* aInterfacePtr, size_t aPlatformIndex);

WSF_EXPORT void GetEntityIdFromIndex(WsfDisInterface* aInterfacePtr, size_t aPlatformIndex, DisEntityId& aEntityId);

WSF_EXPORT WsfExtInterface* GetExtInterface(WsfDisInterface* aInterfacePtr);

WSF_EXPORT DisUint32 GetNextRequestNumber(WsfDisInterface* aInterfacePtr);

WSF_EXPORT DisUint8 GetReliabilityService(WsfDisInterface* aInterfacePtr);

WSF_EXPORT const WsfScenario& GetScenario(WsfDisInterface* aInterfacePtr);

WSF_EXPORT WsfSimulation& GetSimulation(WsfDisInterface* aInterfacePtr);

WSF_EXPORT double GetSimTime(WsfDisInterface* aInterfacePtr);

WSF_EXPORT double GetTimeFromPdu(WsfDisInterface* aInterfacePtr, DisPdu& aPdu);

WSF_EXPORT void PutPdu(WsfDisInterface* aInterfacePtr, double aSimTime, std::unique_ptr<DisPdu> aPduPtr);

WSF_EXPORT void SimTimeToDisClockTime(double aSimTime, DisClockTime& aDisClockTime);

// Used in WsfExchanges.  Returns bool true if successful.
WSF_EXPORT bool WsfToDisPlayers(WsfDisInterface* aInterfacePtr,
                                size_t           aOriginatingIndex,
                                size_t           aRequestingIndex,
                                size_t           aRespondingIndex,
                                bool&            aSenderIsRequestor,
                                bool&            aSenderIsResponder,
                                bool&            aRequestorIsLocal,
                                bool&            aResponderIsLocal,
                                WsfDisPlatform*& aRequesterPtr,
                                WsfDisPlatform*& aResponderPtr,
                                DisEntityId&     aOriginatingId,
                                DisEntityId&     aRequestorId,
                                DisEntityId&     aResponderId);

// Returns bool true if resolving players was successful.
WSF_EXPORT bool DisToWsfPlayers(WsfDisInterface*   aInterfacePtr,
                                const DisEntityId& aOriginatingId,
                                const DisEntityId& aRequestorId,
                                const DisEntityId& aResponderId,
                                bool&              aSenderIsRequestor,
                                bool&              aSenderIsResponder,
                                bool&              aRequestorIsLocal,
                                bool&              aResponderIsLocal,
                                WsfDisPlatform*&   aRequesterPtr,
                                WsfDisPlatform*&   aResponderPtr,
                                size_t&            aRequestingIndex,
                                size_t&            aRespondingIndex);
} // namespace WsfDisUtil

#endif
