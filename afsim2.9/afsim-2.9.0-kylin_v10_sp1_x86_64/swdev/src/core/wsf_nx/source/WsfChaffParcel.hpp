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
#ifndef WSFCHAFFPARCEL_HPP
#define WSFCHAFFPARCEL_HPP

#include "wsf_nx_export.h"

#include <memory>
#include <unordered_map>

#include "UtCloneablePtr.hpp"
#include "UtTable.hpp"
#include "WsfChaffCloudInterface.hpp"
#include "WsfChaffParcelInterface.hpp"
#include "WsfNonExportableComponentRoles.hpp"
#include "WsfObject.hpp"
#include "WsfSimpleComponent.hpp"

//! Object that represents a parcel or cannister of chaff that is dispensed and forms a chaff cloud.
//! Chaff parcels are treated as spherical, with a radius that grows exponentially to a max bloom diameter.
class WSF_NX_EXPORT WsfChaffParcel : public WsfChaffParcelInterface, public WsfObject, public WsfSimplePlatformComponent
{
public:
   WsfChaffParcel();
   WsfChaffParcel(const WsfChaffParcel& aSrc);
   WsfChaffParcel& operator=(const WsfChaffParcel& aSrc) = delete;
   ~WsfChaffParcel() override                            = default;

   //! @name WsfObject overrides
   //@{
   WsfObject* Clone() const override;
   bool       ProcessInput(UtInput& aInput) override;
   //@}

   //! @name WsfChaffParcelInterface overrides
   //@{
   WsfPlatform* GetPlatform() const override { return GetComponentParent(); }
   double       GetRadialSize() const override { return mRadialSize; }
   void         GetDropLocationWCS(double aDropLocationWCS[3]) const override;
   //@}

   //! Calculates the chaff parcel RCS based on the maximum scatterer RCS for a dispersed cloud viewed broadside
   //! (defined in mFreqMaxRCS_Table) and the bistatic presented area of the chaff cloud as seen by the transmitter and
   //! receiver. References: Schleher, Introduction to Electronic Warfare, pp 185-191
   //! @param aFrequency       The frequency of the transmitter (in Hz)
   //! @param aXmtrLocationWCS Location of the transmitter in WCS frame
   //! @param aRcvrLocationWCS Location of the receiver in WCS frame
   //! @returns Radar cross section of parcel
   double GetRadarCrossSection(double aFrequency, const double aXmtrLocationWCS[3], const double aRcvrLocationWCS[3]) const;

   static bool Find(WsfChaffParcel*& aParcelPtr, const WsfPlatform& aPlatform);

   virtual bool InitializeType() const;

   void Update(double aSimTime);
   bool IsExpired(double aSimTime);

   void AddToCloud(std::shared_ptr<WsfChaffCloudInterface>& aCloudPtr);

   static void ClearMaxRcsCache();

   virtual double GetMaximumRCS() const;

protected:
   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override;
   void*         QueryInterface(int aRole) override;
   bool          Initialize(double aSimTime) override;
   //@}

   virtual double GetMaximumRCS(double aFrequency) const;

private:
   std::shared_ptr<WsfChaffCloudInterface> mCloudPtr; // Pointer to cloud that parcel belongs to, for presented area calculation

   using MaxRcsCache = std::unordered_map<std::pair<WsfStringId, double>, double>;
   static MaxRcsCache sMaxRcsCache; // Cache of (Parcel Type, Frequency) -> Max RCS computed by GetMaximumRCS

   // Inputs
   /// Geometric
   double mBloomDiameter;         // Characteristic diameter of scatterers in parcel as time goes to infinity
   double mExpansionTimeConstant; // Radial size expansion time constant (bloom rate)

   /// Kinematic
   double mTerminalVelocity; // Free fall velocity of chaff parcel (m/s)
   double mDecelerationRate; // Rate at which parcel decelerates when ejected

   /// Lifetime and debugging
   double mExpirationTime; // Time after which the parcel is dropped to expire it
   bool   mDebug;          // Debugging flag

   /// Signal return
   using CurvePtr = UtCloneablePtr<UtTable::Curve>;
   CurvePtr     mFreqMaxRCS_Table; // Input table of maximum RCS for dispersed cloud viewed broadside vs. frequency
   unsigned int mNumberDipoles;    // Number of dipoles, assuming all cut to the same length (spot chaff). Used as
                                // alternative to lookup table, for computing max parcel RCS assuming half-wave dipoles.

   // Drop conditions saved in Initialize
   double mDropLocationWCS[3];     // Parcel location
   double mDropVelocityWCS[3];     // Parcel velocity
   double mDropAccelerationWCS[3]; // Parcel acceleration
   double mFreeFallVelocityWCS[3]; // Free fall velocity vector
   double mDropTime;               // Time at which parcel was dropped

   // Physical attributes computed in Update
   double mRadialSize; // Mean radial size of parcel, computed as exponential growth to value of 0.5 * mBloomDiameter
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfChaffParcel, cWSF_COMPONENT_CHAFF_PARCEL)

namespace std // To use pair as key for unordered_map
{
template<>
class hash<std::pair<WsfStringId, double>>
{
public:
   size_t operator()(const std::pair<WsfStringId, double>& pair) const
   {
      return WsfStringId()(pair.first) ^ std::hash<double>()(pair.second);
   }
};
} // namespace std
#endif
