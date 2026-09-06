// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMUTIL_HPP
#define WSFCOMMUTIL_HPP

#include "wsf_export.h"

#include <string>
#include <vector>

// Forward Declarations
class WsfCommandChain;
class WsfGroup;
class WsfPlatform;

namespace wsf
{
namespace comm
{

class Comm;

namespace util
{

//! These calls are temporary functionality to enable legacy
//! comm behavior in various objects/plugins/etc. that have yet to be updated to fully
//! utilize the new comm framework as intended. It is fully intended that the 'SelectDevice'
//! and 'SelectRecipients' methods will be removed upon full adoption of the new comm
//! enhancements, as these calls are typically needed due to tight coupling with the
//! platform-targeted legacy comm model and command chains, which is undesirable.
enum Recipient
{
   cCOMMANDER    = 1,
   cPEERS        = 2,
   cSUBORDINATES = 4
};

//! @name Selection methods
//@{
WSF_EXPORT Comm* SelectDevice(WsfPlatform* aSourcePtr, WsfPlatform* aTargetPtr);

WSF_EXPORT void SelectRecipients(WsfCommandChain* aCmdChainPtr, int aRecipientMask, std::vector<WsfPlatform*>& aRecipients);

WSF_EXPORT void SelectRecipients(WsfPlatform* aSenderPtr, WsfGroup* aGroupName, std::vector<WsfPlatform*>& aRecipients);
//@}

//! The following are methods are used to support legacy comms, or to
//! help maintain the original behavior of the comms library.
//@{

//! These methods support legacy usage of the master/slave network name idiom.
//! While this is not standard comm functionality, this provides legacy support
//! and a generic way of specifying network inclusion (and by extension, comm addressing)
//! that does not intrude on basic comm functionality. It simply provides a common
//! way to specify string identifiers for networks based on the platform owning the
//! comm and its relationship within a command chain.
WSF_EXPORT std::string LocalSlaveNetworkName(Comm& aComm);
WSF_EXPORT std::string LocalMasterNetworkName(Comm& aComm);
//@}

} // namespace util

} // namespace comm
} // namespace wsf

#endif
