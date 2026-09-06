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
#include "WsfChaffCloud.hpp"

#include <sstream>

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfPlatform.hpp"

namespace
{
WsfStringId GetWsfDrawId(const char* prefix, unsigned int id)
{
   std::ostringstream oss;
   oss << prefix << id;
   return WsfStringId(oss.str());
}
} // namespace

// Convenience macros to generate IDs for WsfDraw
#define WSF_DRAW_ID(PREFIX) GetWsfDrawId(PREFIX, GetUniqueId())
#define WSF_DRAW_ID_CLOUD WSF_DRAW_ID("chaff_cloud_draw_approx_")
#define WSF_DRAW_ID_XMTR WSF_DRAW_ID("chaff_cloud_xmtr_vec_")
#define WSF_DRAW_ID_RCVR WSF_DRAW_ID("chaff_cloud_rcvr_vec_")

WsfChaffCloud::WsfChaffCloud(WsfSimulation& aSimulation, bool aDrawApproximation /* = false */)
   : mParcels()
   , mFirstParcelEjectPoint()
   , mAreaFront(0.0)
   , mAreaSide(0.0)
   , mDrawApproximation(aDrawApproximation)
   , mDrawPtr(new WsfDraw(aSimulation))
{
   AssignUniqueId(&aSimulation);
   mDrawPtr->SetLayer("CHAFF_CLOUD");

   mDrawCloudId = WSF_DRAW_ID_CLOUD;
   mDrawXmtrId  = WSF_DRAW_ID_XMTR;
   mDrawRcvrId  = WSF_DRAW_ID_RCVR;

   UtVec3d::Set(mCloudCenterWCS, 0.0);
   UtVec3d::Set(mCloudAxisUnitWCS, 0.0);
}

WsfChaffCloud::~WsfChaffCloud()
{
   // Should not have any parcels left because cloud doesn't get destroyed until last parcel expires, but just in case...
   for (auto parcelPtr : mParcels)
   {
      parcelPtr->GetPlatform()->DetachObserver(this);
   }

   EraseWsfDrawCommands();
}

void WsfChaffCloud::AddParcel(WsfChaffParcelInterface* aParcelPtr)
{
   WsfPlatform* platformPtr = aParcelPtr->GetPlatform();
   if (platformPtr)
   {
      mParcels.push_back(aParcelPtr);
      // Subscribe to notifications on the platform (e.g. when it is deleted upon parcel expiration)
      platformPtr->AttachObserver(this);

      // If first parcel added to cloud, update dummy ejection point
      if (mParcels.size() == 1)
      {
         UpdateFirstParcelEjectPoint();
      }
   }
}

double WsfChaffCloud::GetBistaticPresentedArea(const double aXmtrLocationWCS[3],
                                               const double aRcvrLocationWCS[3],
                                               size_t&      aNumberParcels) const
{
   // Set parcel count
   aNumberParcels = mParcels.size();

   double pAreaBistatic = 0.0; // zero area if no parcels in cloud
   if (!mParcels.empty())
   {
      // Get presented area seen by transmitter
      double pAreaXmtr = GetPresentedArea(aXmtrLocationWCS);
      // Get presented area seen by receiver
      double pAreaRcvr = GetPresentedArea(aRcvrLocationWCS);
      // Average areas to get "bistatic" presented area
      pAreaBistatic = (pAreaXmtr + pAreaRcvr) / 2.0;
   }

   if (mDrawApproximation)
   {
      // Draw line from transmitter to cloud center
      mDrawPtr->SetId(mDrawXmtrId);
      mDrawPtr->Erase(mDrawXmtrId);
      mDrawPtr->SetColor(1.0, 0.0, 0.0, 1.0);
      mDrawPtr->BeginLines();
      mDrawPtr->VertexWCS(aXmtrLocationWCS);
      mDrawPtr->VertexWCS(mCloudCenterWCS);
      mDrawPtr->End();

      // Draw line from receiver to cloud center
      mDrawPtr->SetId(mDrawRcvrId);
      mDrawPtr->Erase(mDrawRcvrId);
      mDrawPtr->SetColor(0.0, 0.0, 1.0, 1.0);
      mDrawPtr->BeginLines();
      mDrawPtr->VertexWCS(aRcvrLocationWCS);
      mDrawPtr->VertexWCS(mCloudCenterWCS);
      mDrawPtr->End();
   }

   return pAreaBistatic;
}

void WsfChaffCloud::OnPlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // Find parcel that was deleted and remove it from the list.
   // If it was the first parcel, also update the first parcel ejection point.
   auto it = std::find_if(mParcels.begin(),
                          mParcels.end(),
                          [aPlatformPtr](const WsfChaffParcelInterface* aParcelPtr)
                          { return (aParcelPtr->GetPlatform() == aPlatformPtr); });
   if (it != mParcels.end())
   {
      it = mParcels.erase(it);
      if (mParcels.empty()) // If cloud is now empty, erase any draw commands
      {
         EraseWsfDrawCommands();
      }
      else if (it == mParcels.begin())
      {
         UpdateFirstParcelEjectPoint();
      }
   }
}

// Update the dummy entity representing the location and orientation of the first parcel's ejection point. Used in
// computing cross-sectional areas and cloud axis, and called when first parcel is added or removed from the cloud.
void WsfChaffCloud::UpdateFirstParcelEjectPoint()
{
   if (!mParcels.empty())
   {
      auto* firstParcelPtr = mParcels.front();

      // Set dummy entity's location to drop location of first parcel
      double dropLocWCS[3];
      firstParcelPtr->GetDropLocationWCS(dropLocWCS);
      mFirstParcelEjectPoint.SetLocationWCS(dropLocWCS);

      // Set dummy entity's orientation to first parcel's orientation
      double psiWCS, thetaWCS, phiWCS;
      firstParcelPtr->GetPlatform()->GetOrientationWCS(psiWCS, thetaWCS, phiWCS);
      mFirstParcelEjectPoint.SetOrientationWCS(psiWCS, thetaWCS, phiWCS);

      // Compute unit vector pointing in direction of cloud
      double ecsVector[3] = {1.0, 0.0, 0.0};
      mFirstParcelEjectPoint.ConvertECSVectorToWCS(mCloudAxisUnitWCS, ecsVector);
   }
}

// Calculates the linear size of the chaff cloud and associated cross-sectional areas as viewed from front and
// broad-side. These areas will be used when computing the bistatic presented area.
void WsfChaffCloud::OnPlatformUpdated(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // Initialize average radius of parcels in cloud
   double avgRadius = 0.0;

   double minDistance = DBL_MAX;
   double maxDistance = -DBL_MAX;

   for (auto parcelPtr : mParcels)
   {
      // Increment average radius
      avgRadius += parcelPtr->GetRadialSize();

      // Distance of parcel from ejection point of first parcel along cloud axis
      double parcelLocECS[3];
      mFirstParcelEjectPoint.GetRelativeLocationECS(parcelPtr->GetPlatform(), parcelLocECS);
      minDistance = std::min(minDistance, parcelLocECS[0]);
      maxDistance = std::max(maxDistance, parcelLocECS[0]);
   }

   // If max separation is zero, force a value
   if (maxDistance == 0.0)
   {
      maxDistance = 1.0;
   }

   // Compute location of center of cloud in WCS coordinates
   double cloudCenterECS[3] = {(maxDistance + minDistance) / 2.0, 0.0, 0.0};
   mFirstParcelEjectPoint.ConvertECSToWCS(cloudCenterECS, mCloudCenterWCS);

   // Compute average radius of cone
   avgRadius /= mParcels.size();
   // Cross-sectional area as viewed from front
   mAreaFront = UtMath::cPI * avgRadius * avgRadius;
   // Cross-sectional area as viewed from side
   mAreaSide = 2.0 * (maxDistance - minDistance) * avgRadius + mAreaFront;

   if (mDrawApproximation)
   {
      double headingNED, pitchNED, rollNED;
      mFirstParcelEjectPoint.GetOrientationNED(headingNED, pitchNED, rollNED);
      mDrawPtr->SetColor(1.0, 0.5, 0.5, 0.5);
      mDrawPtr->SetId(mDrawCloudId);
      mDrawPtr->Erase(mDrawCloudId);
      mDrawPtr->BeginEllipsoid(headingNED * UtMath::cDEG_PER_RAD,
                               pitchNED * UtMath::cDEG_PER_RAD,
                               rollNED * UtMath::cDEG_PER_RAD,
                               (maxDistance - minDistance) / 2.0,
                               avgRadius,
                               avgRadius);
      mDrawPtr->VertexWCS(mCloudCenterWCS);
      mDrawPtr->End();
   }
}

// Compute presented area of this cloud as seen from a given viewpoint
double WsfChaffCloud::GetPresentedArea(const double aViewpointLocationWCS[3]) const
{
   // Location of viewpoint relative to cloud center
   double relativeLocationWCS[3];
   UtVec3d::Subtract(relativeLocationWCS, aViewpointLocationWCS, mCloudCenterWCS);
   UtVec3d::Normalize(relativeLocationWCS);

   // Projection of relative location vector onto cloud axis
   double projx = UtVec3d::DotProduct(relativeLocationWCS, mCloudAxisUnitWCS);
   // Projection of relative location vector perpendicular to cloud axis. Because of representation and roundoff errors,
   // limit the argument of the square root function to a minimum of zero
   double projy = sqrt(std::max(0.0, 1.0 - projx * projx));

   // Calculate presented area as the RMS value of the weighted side and front areas
   return sqrt(pow(projx * mAreaFront, 2.0) + pow(projy * mAreaSide, 2.0));
}

// Erase any draw commands
void WsfChaffCloud::EraseWsfDrawCommands()
{
   mDrawPtr->Erase(mDrawCloudId);
   mDrawPtr->Erase(mDrawXmtrId);
   mDrawPtr->Erase(mDrawRcvrId);
}
