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

#ifndef WSFCOMMTYPES_HPP
#define WSFCOMMTYPES_HPP

#include "WsfComm.hpp"
#include "WsfObjectTypeList.hpp"

namespace wsf
{
namespace comm
{

class Types : public WsfObjectTypeList<Comm>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT Types&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const Types& Get(const WsfScenario& aScenario);
   //@}

   Types(WsfScenario& aScenario);
   ~Types() override = default;
};

} // namespace comm
} // namespace wsf

#endif
