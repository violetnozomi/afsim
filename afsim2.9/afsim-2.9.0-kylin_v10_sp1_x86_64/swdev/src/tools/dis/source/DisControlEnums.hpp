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

#ifndef DISCONTROLENUMS_HPP
#define DISCONTROLENUMS_HPP

namespace DisEnum
{
namespace Control
{
namespace Scheme
{
enum
{
   OTHER              = 0,
   REAL_TIME          = 1,
   SCALED_TIME        = 2,
   SCALED_AND_STEPPED = 3
};
}
// This enumeration is placed in the GetAcknowledgeFlag() attribute of DisAcknowlege.
namespace ResponseToCmdFlag
{
enum
{
   OTHER            = 0,
   CREATE_ENTITY    = 1,
   REMOVE_ENTITY    = 2,
   START_RESUME     = 3,
   STOP_FREEZE      = 4,
   TRANSFER_CONTROL = 5
};
}
namespace Reason
{
enum
{
   OTHER                 = 0,
   RECESS                = 1,
   TERMINATION           = 2,
   SYSTEM_FAILURE        = 3,
   SECURITY_VIOLATION    = 4,
   ENTITY_RECONSTITUTION = 5,
   STOP_FOR_RESET        = 6,
   STOP_FOR_RESTART      = 7,
   ABORT_TRAINING        = 8
};
}
// This enumeration is placed in the GetResponseFlag() attribute of DisAcknowlege.
namespace ResponseCompliance
{
enum
{
   OTHER            = 0,
   ABLE_TO_COMPLY   = 1,
   UNABLE_TO_COMPLY = 2
};
}
} // namespace Control
} // namespace DisEnum

#endif
