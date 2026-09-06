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

#ifndef DISACTIONENUMS_HPP
#define DISACTIONENUMS_HPP

namespace DisEnum
{
namespace Action
{
namespace Request
{
enum
{
   OTHER                = 0,
   DATA                 = 22,
   STATUS               = 23,
   SEND_OBJECT_STATE    = 24,
   START_PDU            = 37,
   INIT_INTERNAL_PARAMS = 39,
   RUN                  = 42,
   JOIN_EXERCISE        = 45,
   RESIGN_EXERCISE      = 46,
   TIME_ADVANCE         = 47,
   EXTERNAL_SUICIDE     = 200,
   FUEL_FREEZE          = 201,
   EXTERNAL_ACTIVATE    = 202,
   EXTERNAL_REPOSITION  = 203
};
}
namespace RequestStatus
{
enum
{
   OTHER                          = 0,
   PENDING                        = 1,
   EXECUTING                      = 2,
   PARTIALLY_COMPLETE             = 3,
   COMPLETE                       = 4,
   REQUEST_REJECTED               = 5,
   RETRANSMIT_REQUEST_NOW         = 6,
   RETRANSMIT_REQUEST_LATER       = 7,
   INVALID_TIME_PARAMETERS        = 8,
   SIMULATION_TIME_EXCEEDED       = 9,
   REQUEST_DONE                   = 10,
   TACCSF_LOS_REPLY_TYPE_1        = 100,
   TACCSF_LOS_REPLY_TYPE_2        = 101,
   JOIN_EXERCISE_REQUEST_REJECTED = 201
};
}
} // namespace Action
} // namespace DisEnum

#endif
