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

#ifndef WSFXIO_HPP
#define WSFXIO_HPP

#include "wsf_export.h"

#include <cstdint>

namespace wsf
{
namespace xio
{
enum ApplicationType : std::int32_t
{
   cAPP_SIMULATION,
   cAPP_USER,
   cAPP_SIMULATION_CONTROLLER
};

enum QueryResult : std::int32_t
{
   //! This bit is set if the query is successful
   cQUERY_SUCCESS_MASK = 0x1000,
   //! This bit is set if the query has failed
   cQUERY_FAIL_MASK = 0x2000,
   //! Query is has not been completed
   cQUERY_NOT_COMPLETE = 0,
   //! Query not supported by application
   cQUERY_NOT_SUPPORTED = cQUERY_FAIL_MASK | 1,
   //! Application could not complete query
   cQUERY_FAILED = cQUERY_FAIL_MASK | 2,
   //! Query's timeout has been exceeded
   cQUERY_TIMED_OUT = cQUERY_FAIL_MASK | 3,
   //! Query failed because the connection was lost
   cQUERY_DISCONNECTED = cQUERY_FAIL_MASK | 4,
   //! Query completed and returned true
   cQUERY_TRUE = cQUERY_SUCCESS_MASK | 1,
   //! Query completed and returned false
   cQUERY_FALSE = cQUERY_SUCCESS_MASK | 2,
   //! Query completed and returned a custom value
   cQUERY_COMPLETE = cQUERY_SUCCESS_MASK | 3
};
} // namespace xio
} // namespace wsf

namespace WsfXIO = wsf::xio;

#endif
