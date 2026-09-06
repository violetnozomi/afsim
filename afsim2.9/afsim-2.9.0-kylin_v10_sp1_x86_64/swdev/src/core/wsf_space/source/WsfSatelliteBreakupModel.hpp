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

#ifndef WSFSATELLITEBREAKUPMODEL_HPP
#define WSFSATELLITEBREAKUPMODEL_HPP

#include <memory>
#include <string>

class UtCalendar;
#include "UtVec3.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

class WsfSatelliteBreakupModel
{
public:
   explicit WsfSatelliteBreakupModel(WsfSimulation& aSimulation)
      : mScenario(aSimulation.GetScenario())
      , mSimulation(aSimulation)
   {
   }
   virtual ~WsfSatelliteBreakupModel() = default;

   // Look at the other constructors and assignment operators.

   bool HasGenerated() const { return mHasGenerated; }

   //! Return the number of pieces of debris that were generated.
   virtual size_t GetFragmentCount() const = 0;

   //! The standard information produced by breakup models for each fragment.
   struct Fragment
   {
      double      mLength{0.0};   //! The size of the fragment [m]
      double      mAoverM{0.0};   //! The ratio of area to mass of the fragment [m^2/kg]
      double      mMass{-1.0};    //! The mass of the fragment [kg]
      double      mArea{0.0};     //! The area of the fragment [m^2]
      UtVec3d     mLocationECI{}; //! The initial location of the fragment [m]
      UtVec3d     mVelocityECI{}; //! The initial velocity of the fragment [m/s]
      UtVec3d     mDeltaV_ECI{};  //! The change in velocity relative to the progenitor of this fragment [m/s]
      std::string mName{};        //! The name of the fragment

      bool IsInitialized() const { return mMass > 0.0; }
   };

   //! Get the indicated fragment's details.
   virtual Fragment GetFragment(size_t aIndex) const = 0;

   bool ExplosiveBreakup(double aSimTime, const std::string& aPlatformName, const std::vector<double>& aParams);

   bool CollisionalBreakup(double                     aSimTime,
                           const std::string&         aTargetPlatform,
                           const std::vector<double>& aTargetParams,
                           const std::string&         aImpactorPlatform,
                           const std::vector<double>& aImpactorParams);

   //! Get the type of the platform for any fragments created by this object.
   const std::string& GetFragmentPlatformType() const { return mFragmentPlatformType; }

   //! Set the type of the platform for any fragments created by this object.
   void SetFragmentPlatformType(const std::string& aPlatformType);

protected:
   //! Compute the fragments produced by the explosion model.
   //!
   //! This method should be implemented by subclasses to actually perform the generation of
   //! fragment characteristics. This method is called before AddFragmentsToSimulation.
   //! If there is some issue generating the full set of fragments, then this will return false.
   //!
   //! \param aPlatform - The platform for which to model an explosive breakup.
   //! \param aParams   - The model parameters for the indicated platform.
   //! \returns         - true if fragments are successfully generated; false otherwise.
   virtual bool ModelExplosion(WsfPlatform& aPlatform, const std::vector<double>& aParams) = 0;

   //! Compute the fragments produced by the collision model.
   //!
   //! This method should be implemented by subclasses to actually perform the generation
   //! of fragment characteristics. This method is called before AddFragmentsToSimulation.
   //! If there is some issue generating the full set of fragments, then this will return false.
   //!
   //! \param aTargetPlatform   - The first platform participating in the collisional breakup.
   //! \param aTargetParams     - The model parameters for the first platform.
   //! \param aImpactorPlatform - The second platform participating in the collisional breakup.
   //! \param aImpactorParams   - The model parameters for the second platform.
   //! \returns                 - true if fragments are successfully generated; false otherwise.
   virtual bool ModelCollision(WsfPlatform&               aTargetPlatform,
                               const std::vector<double>& aTargetParams,
                               WsfPlatform&               aImpactorPlatform,
                               const std::vector<double>& aImpactorParams) = 0;

   WsfSimulation& GetSimulation() const { return mSimulation; }

private:
   std::unique_ptr<WsfPlatform> GetDefaultFragmentPlatformTemplate() const;
   std::unique_ptr<WsfPlatform> GetFragmentPlatformTemplate() const;
   bool                         AddFragment(std::unique_ptr<WsfPlatform> aPlatformPtr,
                                            const Fragment&              aFragment,
                                            const UtCalendar&            aEpoch,
                                            double                       aSimTime);
   bool                         AddFragmentsToSimulation(double aSimTime);
   void                         RemoveProgenitor(std::string aPlatformName, double aSimTime);

   const WsfScenario& mScenario;
   WsfSimulation&     mSimulation;
   std::string        mFragmentPlatformType{};
   bool               mHasGenerated{false};
};

#endif // WSFSATELLITEBREAKUPMODEL_HPP
