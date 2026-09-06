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
#ifndef WSFCHAFFCLOUD_HPP
#define WSFCHAFFCLOUD_HPP

#include "wsf_nx_export.h"

#include <list>
#include <memory>

#include "UtEntity.hpp"
#include "WsfChaffCloudInterface.hpp"
#include "WsfChaffParcelInterface.hpp"
#include "WsfDraw.hpp"
#include "WsfSinglePlatformObserver.hpp"
#include "WsfUniqueId.hpp"

//! A chaff cloud that maintains a list of parcels ejected from a chaff weapon
class WSF_NX_EXPORT WsfChaffCloud : public WsfChaffCloudInterface, public WsfSinglePlatformObserver, public WsfUniqueId
{
public:
   WsfChaffCloud(WsfSimulation& aSimulation, bool aDrawApproximation = false);
   ~WsfChaffCloud() override;
   WsfChaffCloud(const WsfChaffCloud& aSrc) = delete;
   WsfChaffCloud& operator=(const WsfChaffCloud& aSrc) = delete;

   void AddParcel(WsfChaffParcelInterface* aParcelPtr) override;
   void DrawCloudApproximation(bool aEnabled) { mDrawApproximation = aEnabled; }

   // Returns bistatic presented area of cloud for computing signal return of each parcel
   // aXmtrLocationWCS [input] : Transmitter location in WCS coordinates
   // aRcvrLocationWCS [input] : Receiver location in WCS coordinates
   // aNumberParcels   [output]: Number of parcels in the cloud
   double GetBistaticPresentedArea(const double aXmtrLocationWCS[3],
                                   const double aRcvrLocationWCS[3],
                                   size_t&      aNumberParcels) const override;

protected:
   // Single platform observer overrides to get notified when parcels expire or are updated (moved)
   void OnPlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr) override;
   void OnPlatformUpdated(double aSimTime, WsfPlatform* aPlatformPtr) override;

private:
   void   UpdateFirstParcelEjectPoint();
   double GetPresentedArea(const double aViewpointLocationWCS[3]) const;
   void   EraseWsfDrawCommands();

   using ParcelList = std::list<const WsfChaffParcelInterface*>;

   ParcelList mParcels;               // The list of parcels that comprise the chaff cloud
   UtEntity   mFirstParcelEjectPoint; // Dummy entity representing the location of first parcel's ejection point
   double     mCloudCenterWCS[3];     // Location of center of cloud in WCS frame
   double     mCloudAxisUnitWCS[3];   // Unit vector in WCS pointing in direction of cloud axis
   double     mAreaFront;             // Cloud cross-sectional area as viewed from front
   double     mAreaSide;              // Cloud cross-sectional area as viewed from side

   bool                     mDrawApproximation;
   std::unique_ptr<WsfDraw> mDrawPtr; // Used to draw cloud approximation for debugging purposes
   WsfStringId              mDrawCloudId;
   WsfStringId              mDrawXmtrId;
   WsfStringId              mDrawRcvrId;
};
#endif
