// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WSFCHAFFCLOUDINTERFACE_HPP
#define WSFCHAFFCLOUDINTERFACE_HPP

#include <cstddef> // for size_t

#include "WsfChaffParcelInterface.hpp"

// An abstract base class that serves as the interface to a chaff cloud for parcels and chaff weapons.
// This interface removes the circular dependency between chaff clouds and parcels.
class WsfChaffCloudInterface
{
public:
   virtual ~WsfChaffCloudInterface() = default;

   // Returns bistatic presented area of cloud for computing signal return of each parcel
   // aXmtrLocationWCS [input] : Transmitter location in WCS coordinates
   // aRcvrLocationWCS [input] : Receiver location in WCS coordinates
   // aNumberParcels   [output]: Number of parcels in the cloud
   virtual double GetBistaticPresentedArea(const double aXmtrLocationWCS[3],
                                           const double aRcvrLocationWCS[3],
                                           size_t&      aNumberParcels) const = 0;

   // Add a chaff parcel to the cloud
   // aParcelPtr : A pointer to the parcel. The cloud does not assume ownership of the parcel.
   virtual void AddParcel(WsfChaffParcelInterface* aParcelPtr) = 0;
};
#endif
