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

#ifndef DISTRANSACTIONENUMS_HPP
#define DISTRANSACTIONENUMS_HPP

namespace DisEnum
{
namespace Logistics
{
namespace Service
{
enum
{
   OTHER    = 0,
   RESUPPLY = 1,
   REPAIR   = 2
};
}
namespace Repair
{
namespace Completed
{
enum
{
   NONE = 0,
   ALL  = 1
};
}
namespace Result
{
enum
{
   OTHER       = 0,
   ENDED       = 1,
   INVALID     = 2,
   INTERRUPTED = 3,
   CANCELED    = 4
};
}
} // namespace Repair
} // namespace Logistics
namespace Transaction
{
namespace Role
{
enum
{
   OTHER      = 0,
   INITIATOR  = 1,
   RESPONDENT = 2
};
}
namespace Initiator
{
namespace State
{
enum
{
   READY      = 0,
   REQUESTING = 1,
   MONITORING = 2
};
}
} // namespace Initiator
namespace Respondent
{
namespace State
{
enum
{
   READY      = 0,
   RESPONDING = 1,
   FINISHED   = 2
};
}
} // namespace Respondent
} // namespace Transaction
} // namespace DisEnum

#endif
