// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCHAFFPARCELTYPES_HPP
#define WSFCHAFFPARCELTYPES_HPP

#include "wsf_nx_export.h"

#include "WsfChaffParcel.hpp"
#include "WsfObjectTypeList.hpp"

class WsfChaffParcelTypes : public WsfObjectTypeList<WsfChaffParcel>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_NX_EXPORT WsfChaffParcelTypes&       Get(WsfScenario& aScenario);
   static WSF_NX_EXPORT const WsfChaffParcelTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfChaffParcelTypes(WsfScenario& aScenario);
   ~WsfChaffParcelTypes() override;

   bool InitializeType(WsfChaffParcel* aTypePtr) override;
};

#endif
