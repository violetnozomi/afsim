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

#ifndef WSFNASABREAKUPMODEL_HPP
#define WSFNASABREAKUPMODEL_HPP

#include "WsfSatelliteBreakupModel.hpp"

class WsfNASA_BreakupModel : public WsfSatelliteBreakupModel
{
public:
   explicit WsfNASA_BreakupModel(WsfSimulation& aSimulation);
   ~WsfNASA_BreakupModel() override = default;

   size_t   GetFragmentCount() const override;
   Fragment GetFragment(size_t aIndex) const override;

   //! Return the name of the progenitor of the explosive breakup.
   const std::string& GetExplosionProgenitor() const { return mProgenitorOne.mName; }

   //! Return the mass involved in the explosive breakup.
   double GetExplosionMass() const { return mProgenitorOne.mMass; }

   //! Return the name of the smaller mass progenitor in a collisional breakup.
   const std::string& GetCollisionProgenitorOne() const { return mProgenitorOne.mName; }

   //! Return the mass of the smaller mass progenitor in a collisional breakup.
   double GetCollisionProgenitorOneMass() const { return mProgenitorOne.mMass; }

   //! Return the name of the larger mass progenitor in a collisional breakup.
   const std::string& GetCollisionProgenitorTwo() const { return mProgenitorTwo.mName; }

   //! Return the mass of the larger mass progenitor in a collisional breakup.
   double GetCollisionProgenitorTwoMass() const { return mProgenitorTwo.mMass; }

   //! Return the mass of debris produced by the collision.
   double GetCollisionMass() const { return mCollisionMass; }

   //! Return the name template of the produced debris.
   //!
   //! The debris created by this model will be given names that are
   //! equal to the result of this method with a unique numerical
   //! identifier appended.
   //!
   //! \returns - The string template used to construct fragment names.
   const std::string& GetDebrisName() const { return mDebrisName; }

   //! Return the S factor for the explosive breakup.
   double GetExplosionS_Factor() const { return mExplosionS_Factor; }

   //! Return the fraction of the mass that will be deposited as large fragments.
   double GetLargeFragmentMassFraction() const { return mLargeFragmentFraction; }

   //! Return if the object involved in the breakup should be modeled as a spacecraft.
   bool IsModeledAsSpacecraft() const { return mModelAsSpacecraft; }

   //! Return the minimum fragment size that will be produced.
   double GetMinFragmentSize() const { return mMinFragmentSize; }

   //! Set the name template of the produced debris.
   //!
   //! If no debris name is set for this object, the progenitor object(s) name(s) will
   //! be used to generate the template name.
   //!
   //! \param aDebrisName - a string to use as the name template for produced debris.
   void SetDebrisName(const std::string& aDebrisName) { mDebrisName = aDebrisName; }

   //! Set the S factor for an explosive breakup.
   void SetExplosionS_Factor(double aS_Factor) { mExplosionS_Factor = aS_Factor; }

   //! Set the large fragment mass fraction.
   void SetLargeFragmentMassFraction(double aLargeFragmentFraction)
   {
      mLargeFragmentFraction = std::min(1.0, std::max(0.0, aLargeFragmentFraction));
   }

   //! Set if the objects will be modeled as a spacecraft.
   //!
   //! This model will treat progenitor objects as either a spacecraft or a rocket body.
   //! The produced A/M values will depend on this choice.
   //!
   //! \param aModelAsSpacecraft - set to true if the progenitors should be treated as spacecraft.
   void SetModeledAsSpacecraft(bool aModelAsSpacecraft) { mModelAsSpacecraft = aModelAsSpacecraft; }

   //! Set the minimum size of a fragment produced by the model.
   void SetMinFragmentSize(double aMinSize) { mMinFragmentSize = aMinSize; }

   //! A utility method to pack progenitor input parameters.
   static std::vector<double> PackInputParameters(double aMass, double aSize);

private:
   //! The data needed for a progenitor in the breakup model.
   struct Progenitor
   {
      std::string mName{};       //! Name of the platform
      UtVec3d     mLocation{};   //! ECI location at breakup time
      UtVec3d     mVelocity{};   //! ECI velocity at breakup time
      double      mMass{-1.0};   //! Modeled mass
      double      mLength{-1.0}; //! Modeled size
   };

   // Explosive breakup modeling
   bool     ModelExplosion(WsfPlatform& aPlatform, const std::vector<double>& aParams) override;
   bool     FinalizeExplosionModel(WsfPlatform& aPlatform, const std::vector<double>& aParams);
   Fragment GenerateExplosiveFragment(double aLc, int aId);
   Fragment GenerateExplosiveLargeFragment(double         aMinLc,
                                           double         aMassRemaining,
                                           bool           aIsLast,
                                           int            aId,
                                           const UtVec3d& aLocation,
                                           const UtVec3d& aVelocity);

   // Collisional breakup modeling
   bool     ModelCollision(WsfPlatform&               aTargetPlatform,
                           const std::vector<double>& aTargetParams,
                           WsfPlatform&               aImpactorPlatform,
                           const std::vector<double>& aImpactorParams) override;
   bool     FinalizeCollisionModel(WsfPlatform&               aTargetPlatform,
                                   const std::vector<double>& aTargetParams,
                                   WsfPlatform&               aImpactorPlatform,
                                   const std::vector<double>& aImpactorParams);
   bool     ModelCatastrophicCollision();
   bool     ModelNonCatastrophicCollision();
   double   ModelCollisionDebris(double aTotalMass, const Progenitor& aProgenitor, bool aGenerateLargeFragments = true);
   Fragment GenerateCollisionalFragment(double aLc, int aId, const UtVec3d& aLocation, const UtVec3d& aVelocity);
   Fragment GenerateCollisionalLargeFragment(double         aMinLc,
                                             double         aMassRemaining,
                                             bool           aIsLast,
                                             int            aId,
                                             const UtVec3d& aLocation,
                                             const UtVec3d& aVelocity);
   Fragment GenerateCrateredFragment(const Progenitor& aProgenitor);
   std::pair<Progenitor, Progenitor> SolveCollisionProblem(const Progenitor& aSmall, const Progenitor& aLarge) const;

   // Utilities for dealing with velocities
   UtVec3d     GenerateRandomVelocity(double aMagnitude);
   static void EnsureMomentumConservation(std::vector<Fragment>& aFragments);
   static void EnsureMomentumConservation(std::vector<Fragment>::iterator aStart, std::vector<Fragment>::iterator aEnd);

   // NASA model routines
   double        ExplosionN(double aLc) const;
   double        InverseExplosionN(double aCn) const;
   bool          IsCatastrophicCollision() const;
   double        CatastrophicCollisionMass() const;
   double        NonCatastrophicCollisionMass() const;
   void          ComputeCollisionMass();
   static double CollisionN(double aLc, double aCollisionMass);
   static double InverseCollisionN(double aCn, double aCollisionMass);
   static double LimitedLerp(double aValue, double aValueMin, double aValueMax, double aRangeMin, double aRangeMax);
   static double BlendFunctionWeight(double aLc);
   double        AoverM_SmallSize(double aLambdaC);
   double        AoverM_RocketBodyTermOne(double aLambdaC);
   double        AoverM_RocketBodyTermTwo(double aLambdaC);
   double        AoverM_RocketBody(double aLc);
   double        AoverM_SpacecraftTermOne(double aLambdaC);
   double        AoverM_SpacecraftTermTwo(double aLambdaC);
   double        AoverM_Spacecraft(double aLc);
   static double AfromLc(double aLc);
   double        DeltaV_Explosion(double aAoverM);
   double        DeltaV_Collision(double aAoverM);

   //! Progenitor one is the platform experiencing explosive breakup, or the smaller
   //! mass in a collisional breakup.
   Progenitor mProgenitorOne{};

   //! Progenitor two is not used in an explosive breakup, or is the larger mass in
   //! a collisional breakup.
   Progenitor mProgenitorTwo{};

   std::string           mDebrisName{};           //! The name of the generated debris.
   std::vector<Fragment> mFragments{};            //! The generated Fragments.
   double                mMinFragmentSize{0.1};   //! The minimum size of any generated fragment.
   double                mExplosionS_Factor{1.0}; //! S factor in explosion N scaling; typically in range [0.1, 1.0].
   double                mLargeFragmentFraction{0.5}; //! The fraction of the mass that is deposited in large fragments.
   double                mCollisionRelativeSpeed{0.0};    //! The collision's relative speed.
   double                mCollisionMass{-1.0};            //! The mass involved in the collision.
   bool                  mIsCatastrophicCollision{false}; //! Indicates if the collision is catastrophic.
   bool                  mModelAsSpacecraft{true};        //! Model as spacecraft instead of as rocket body.
};

#endif // WSFNASABREAKUPMODEL_HPP
