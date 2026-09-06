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
#ifndef WSFCHAFFMOVER_HPP
#define WSFCHAFFMOVER_HPP

#include "wsf_nx_export.h"

#include "WsfChaffParcel.hpp"
#include "WsfMover.hpp"

//! A mover that simply provides a hook into the simulation for a chaff parcel dispensed from a chaff weapon. Currently
//! it only delegates its Update to the parcel, to allow the parcel to update its location, velocity and other physical
//! characteristics. It is not exposed through the list of mover types in a scenario. Therefore it cannot be
//! instantiated by an input file but is only instantiated and attached to the launched platform by the chaff weapon.
class WSF_NX_EXPORT WsfChaffMover : public WsfMover
{
public:
   WsfChaffMover(const WsfScenario& aScenario);
   WsfChaffMover(const WsfChaffMover& aSrc) = delete;
   WsfChaffMover& operator=(const WsfChaffMover& aSrc) = delete;
   ~WsfChaffMover() override                           = default;

   //! @name Common infrastructure methods.
   //@{
   //! Clone this object.
   //! @return a pointer to the new object.
   WsfMover* Clone() const override;
   bool      Initialize(double aSimTime) override;
   void      Update(double aSimTime) override;
   //@}

private:
   //! A pointer to the chaff parcel extension on the platform, for convenience
   WsfChaffParcel* mParcelPtr;
};

#endif
