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

#ifndef DISENUMS_HPP
#define DISENUMS_HPP

#include "dis_export.h"

#include <algorithm>
#include <stdexcept>
#include <string>

#include "DisTypes.hpp"

namespace DisEnum
{
namespace Pdu
{
namespace Version
{
enum Enum
{
#define X(Value, String, Enumeration) Enumeration = Value,
#include "DisVersion.xenum"
#undef X
};
DIS_EXPORT const bool IsValid(DisEnum8 v);
DIS_EXPORT const std::string& ToString(Enum v);
DIS_EXPORT const Enum         ToEnum(const std::string& v);
} // namespace Version
namespace Type
{
enum Enum
{
#define X(Value, String, Enumeration) Enumeration = Value,
#include "DisType.xenum"
#undef X
   // deprecated, now IFF
   IffAtcNavaids = 28,
   // deprecated, now Transfer Ownership
   TransferControl = 35,
   // Deprecated, now Entity Damage Status
   DirectedEnergyDamageStatus = 69,
};
DIS_EXPORT const bool IsValid(DisEnum8 t);
DIS_EXPORT const std::string& ToString(Enum t);
DIS_EXPORT const Enum         ToEnum(const std::string& t);
} // namespace Type
namespace Family
{
enum Enum
{
#define X(Value, String, Enumeration) Enumeration = Value,
#include "DisFamily.xenum"
#undef X
};
DIS_EXPORT const bool IsValid(DisEnum8 f);
DIS_EXPORT const std::string& ToString(Enum f);
DIS_EXPORT const Enum         ToEnum(const std::string& f);
} // namespace Family

namespace Status
{
namespace TEI
{
enum
{
   NoDifference = 0,
   Difference   = 1
};
// All values of 1 bit enum are defined
inline bool IsValid(DisEnum8)
{
   return true;
}
DIS_EXPORT const std::string& ToString(DisEnum8 aTEI);
} // namespace TEI
namespace LVC
{
enum
{
   NoStatement  = 0,
   Live         = 1,
   Virtual      = 2,
   Constructive = 3
};
// All values of 2 bit enum are defined
inline bool IsValid(DisEnum8)
{
   return true;
}
DIS_EXPORT const std::string& ToString(DisEnum8 aLVC);
} // namespace LVC
namespace CEI
{
enum
{
   NotCoupled = 0,
   Coupled    = 1
};
// All values of 1 bit enum are defined
inline bool IsValid(DisEnum8)
{
   return true;
}
DIS_EXPORT const std::string& ToString(DisEnum8 aCEI);
} // namespace CEI
namespace FTI
{
enum
{
   Munition   = 0,
   Expendable = 1
};
// All values of 1 bit enum are defined
inline bool IsValid(DisEnum8)
{
   return true;
}
DIS_EXPORT const std::string& ToString(DisEnum8 aFTI);
} // namespace FTI
namespace DTI
{
enum
{
   Munition             = 0,
   Expendable           = 1,
   NonMunitionExplosion = 2
};
inline bool IsValid(DisEnum8 aDTI)
{
   return aDTI <= NonMunitionExplosion;
}
DIS_EXPORT const std::string& ToString(DisEnum8 aDTI);
} // namespace DTI
namespace RAI
{
enum
{
   NoStatement = 0,
   Unattached  = 1,
   Attached    = 2
};
inline bool IsValid(DisEnum8 aRAI)
{
   return aRAI <= Attached;
}
DIS_EXPORT const std::string& ToString(DisEnum8 aRAI);
} // namespace RAI
namespace IAI
{
enum
{
   NoStatement = 0,
   Unattached  = 1,
   Attached    = 2
};
inline bool IsValid(DisEnum8 aIAI)
{
   return aIAI <= Attached;
}
DIS_EXPORT const std::string& ToString(DisEnum8 aIAI);
} // namespace IAI
namespace ISM
{
enum
{
   Regeneration = 0,
   Interactive  = 1,
};
// All values of 1 bit enum are defined
inline bool IsValid(DisEnum8)
{
   return true;
}
DIS_EXPORT const std::string& ToString(DisEnum8 aISM);
} // namespace ISM
namespace AII
{
enum
{
   NotActive = 0,
   Active    = 1
};
// All values of 1 bit enum are defined
inline bool IsValid(DisEnum8)
{
   return true;
}
DIS_EXPORT const std::string& ToString(DisEnum8 aAII);
} // namespace AII
DIS_EXPORT bool IsValid(DisEnum8 aPduStatus, DisEnum8 aPduType);
DIS_EXPORT std::string ToString(DisEnum8 aPduStatus, DisEnum8 aPduType);
} // namespace Status

namespace Reliability
{
enum
{
   Acknowledged   = 0,
   Unacknowledged = 1
};
}

namespace EventType
{
enum
{
   Other                        = 0,
   RanOutOfAmmunition           = 2,
   KilledInAction               = 3,
   Damage                       = 4,
   MobilityDisabled             = 5,
   FireDisabled                 = 6,
   RanOutOfFuel                 = 7,
   EntityInitialization         = 8,
   RequestForIndirectFire       = 9,  // Also used for CAS mission
   IndirectFire                 = 10, // Also used for CAS fire
   MinefieldEntry               = 11,
   MinefieldDetonation          = 12,
   VehicleMasterPowerOn         = 13,
   VehicleMasterPowerOff        = 14,
   AggregateStateChangRequested = 15,
   PreventCollisionDetonation   = 16,
   OwnershipReport              = 17
};
}

namespace OwnershipStatus
{
enum
{
   Other                  = 0,
   NewOwner               = 1,
   OwnershipQueryResponse = 2,
   OwnershipConflict      = 4,
   AutoResolveConflict    = 5, // Local Entity Cancelled
   ManualResolveConflict  = 6, // Local Entity Cancelled
   RemoveEntityReceived   = 7, // Local Entity Cancelled
};
}
} // namespace Pdu
} // namespace DisEnum

#endif
