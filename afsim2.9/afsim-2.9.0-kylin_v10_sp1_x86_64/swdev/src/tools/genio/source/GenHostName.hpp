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

#ifndef GENHOSTNAME_HPP
#define GENHOSTNAME_HPP

#include "genio_export.h"

#include <string>
#include <vector>

#include "GenIP.hpp"

namespace GenSockets
{
/** Represents a host name.
 */
class GENIO_EXPORT GenHostName
{
public:
   GenHostName(std::string aHostName) { mHostName = aHostName; }
   bool IsValid() const;
   //! User defined operator
   //! @return The hostname as a string.
                      operator std::string() const { return mHostName; }
                      operator GenIP() const;
   void               GetIP_List(std::vector<GenIP>& aIP_List);
   static GenHostName LocalHostName();

protected:
   std::string mHostName;
};
} // namespace GenSockets
#endif
