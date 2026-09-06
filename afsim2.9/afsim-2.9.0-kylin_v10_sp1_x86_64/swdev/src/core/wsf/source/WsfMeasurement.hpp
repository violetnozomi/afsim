// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFMEASUREMENT_HPP
#define WSFMEASUREMENT_HPP

#include "wsf_export.h"

#include <map>
#include <string>

#include "UtCloneablePtr.hpp"
class UtInput;
#include "UtMeasurementData.hpp"
#include "UtMemory.hpp"
class WsfFilter;
class WsfSimulation;
#include "WsfTrackExtrapolation.hpp"
#include "WsfTrackExtrapolators.hpp"
#include "WsfTypes.hpp"

//! A measurement class that provides WSF-specific interfaces and data extrapolation.
class WSF_EXPORT WsfMeasurement : public UtMeasurementData
{
public:
   WsfMeasurement();
   WsfMeasurement(const WsfMeasurement& aSrc) = default;
   virtual WsfMeasurement* Clone() const;

   virtual bool ProcessInput(UtInput& aInput);
   virtual void Update(double aSimTime){};

   virtual bool IsTrack() const { return false; }

   //! @name Virtual filter methods.
   //@{

   //! Return a pointer to the filter.  If no filter is present, the pointer will be zero.
   virtual WsfFilter* GetFilter() { return nullptr; }

   //! Return a pointer to the filter (const version).  If no filter is present, the pointer will be zero.
   virtual const WsfFilter* GetFilter() const { return nullptr; }
   //
   //@}

   //! @name Extrapolation methods.
   //@{

   virtual bool GetExtrapolatedLocationWCS(double aSimTime, double aLocationWCS[3]) const;

   //! Extrapolate measurement data based on stored wsf::TrackExtrapolation objects.
   //! @param aSimTime The time to which the data should be extrapolated.
   //! @param aExtrapolationType The type of extrapolation to be performed, obtained from the track extrapolation
   //! object's GetTypeId method.
   //! @param aData The output extrapolated data.
   //! @return Whether the data were extrapolated.
   //! @note Extrapolation is most commonly performed for wsf::KinematicState, to include WCS location
   //! (WsfMeasurement::GetExtrapolatedLocationWCS).
   template<typename T>
   bool GetExtrapolatedData(double aSimTime, const std::string& aExtrapolationType, T& aData) const
   {
      const wsf::TrackExtrapolators* extrapolatorsPtr = GetTrackExtrapolators();
      if (extrapolatorsPtr != nullptr)
      {
         const wsf::TrackExtrapolationBase* extrapolatorBasePtr = extrapolatorsPtr->Find(aExtrapolationType);
         if (extrapolatorBasePtr != nullptr)
         {
            auto extrapolatorPtr = dynamic_cast<const wsf::TrackExtrapolation<T>*>(extrapolatorBasePtr);
            std::unique_ptr<UtAttribute<T>> dataPtr = extrapolatorPtr->Extrapolate(aSimTime, *this);
            if (dataPtr != nullptr)
            {
               aData = dataPtr->Get();
               return true;
            }
         }
      }
      return false;
   }
   //@}

   virtual void KinematicUpdate(double aSimTime);

   //! Return the simulation object this measurement is associated with, or nullptr if WsfMeasurement::Initialize was not called.
   WsfSimulation* GetSimulation() const { return mSimulationPtr; }

   //! Set the simulation object associated with this measurement.
   //! @note A simulation does not need to be set unless data extrapolation (GetExtrapolatedData) is desired.
   void SetSimulation(WsfSimulation& aSimulation) { mSimulationPtr = &aSimulation; }

   void SetSphericalMeasurement(double aSimTime, double aAzimuth, double aElevation, double aRange, double aRangeRate);

   //@}

   //! @name Spatial domain methods.
   //! The spatial domain represents the domain in which that track is thought to exist (land, air, space, etc.)
   //! Some systems report tracks differently depending on their domain.
   //@{
   //! Get the spatial domain of the platform.
   WsfSpatialDomain GetSpatialDomain() const { return (WsfSpatialDomain)mSpatialDomain.Get(); }
   //! Set the spatial domain of the platform.
   void SetSpatialDomain(const WsfSpatialDomain& aSpatialDomain) { mSpatialDomain.Set(aSpatialDomain); }
   //! Return a default altitude based on the platform's spatial domain.
   double GetDefaultAltitude() const { return WsfMeasurement::GetDefaultAltitude(GetSpatialDomain()); }
   //@}

protected:
   // Default altitude methods for estimating location from range-bearing tracks
   static double GetDefaultAltitude(WsfSpatialDomain aDomain);

   using WsfSpatialDomainInformation = UtInformation<int>;

   WsfSpatialDomainInformation    mSpatialDomain;
   const wsf::TrackExtrapolators* GetTrackExtrapolators() const;

   WsfSimulation* mSimulationPtr{nullptr};

private:
   static double cDEFAULT_ALTITUDE_LAND;       //! Concept of a default land altitude
   static double cDEFAULT_ALTITUDE_AIR;        //! Concept of a default air altitude
   static double cDEFAULT_ALTITUDE_SPACE;      //! Concept of a default space altitude
   static double cDEFAULT_ALTITUDE_SURFACE;    //! Concept of a default surface altitude
   static double cDEFAULT_ALTITUDE_SUBSURFACE; //! Concept of a default subsurface altitude

   mutable ut::CloneablePtr<wsf::TrackExtrapolators> mExtrapolatorsPtr{nullptr};
};

#endif
