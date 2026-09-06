// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMROUTERTYPES_HPP
#define WSFCOMMROUTERTYPES_HPP

#include "wsf_export.h"

#include "WsfCommRouter.hpp"
#include "WsfObjectTypeList.hpp"

namespace wsf
{
namespace comm
{
namespace router
{

class Types : public WsfObjectTypeList<Router>
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

} // namespace router
} // namespace comm
} // namespace wsf

#endif
