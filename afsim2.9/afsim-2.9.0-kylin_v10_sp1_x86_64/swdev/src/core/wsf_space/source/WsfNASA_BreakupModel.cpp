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

/*

The implementation here is based on the following references. Some details of this
implementation are not specified in these references. Any such choices are noted
in comments below.

(1) NASA's New Breakup Model of Evolve 4.0, N.L. Johnson, P. H. Krisko, J.-C. Liou and
    P.D. Anz-Meador, Adv. Space Res., 28, 9, pp. 1377-1384, 2001.

(2) Proper Implementation of the 1998 NASA Breakup Model, P. Krisko, Orbital Debris Quarterly
    News, 15, 4, October 2011.

Some further evidence from the following is used in this implementation.

(3) Analysis of Debris from the Collision of the Cosmos 2251 and the Iridium 33 Satellites,
    Ting Wang, Science & Global Security, 18:87-118, 2010.

*/

#include "WsfNASA_BreakupModel.hpp"

#include <cmath>
#include <iostream>

#include "UtCast.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfPlatform.hpp"

namespace // {anonymous}
{

constexpr double cEXPLOSION_N_PREFACTOR{6.0};
constexpr double cEXPLOSION_N_SCALING{-1.6};

constexpr double cCOLLISION_N_PREFACTOR{0.1};
constexpr double cCOLLISION_N_MASS_SCALING{0.75};
constexpr double cCOLLISION_N_SIZE_SCALING{-1.71};

// Explosions need the mass and size of the object.
constexpr size_t cEXPLOSION_MASS_INDEX   = 0;
constexpr size_t cEXPLOSION_LENGTH_INDEX = 1;
constexpr size_t cNUM_EXPLOSION_PARAMS   = 2;

// Collisions need the mass and size of the object.
constexpr size_t cCOLLISION_MASS_INDEX   = 0;
constexpr size_t cCOLLISION_LENGTH_INDEX = 1;
constexpr size_t cNUM_COLLISION_PARAMS   = 2;

// This specifies how close the two progenitors need to be to
// have a collision. If the progenitors are a greater distance than this
// multiple of the sum of their modeled sizes away from each other,
// then no collision will be modeled.
constexpr double cCOLLISION_NEARNESS_FACTOR = 5.0;

} // namespace

WsfNASA_BreakupModel::WsfNASA_BreakupModel(WsfSimulation& aSimulation)
   : WsfSatelliteBreakupModel(aSimulation)
{
}

size_t WsfNASA_BreakupModel::GetFragmentCount() const
{
   return mFragments.size();
}

WsfSatelliteBreakupModel::Fragment WsfNASA_BreakupModel::GetFragment(size_t aIndex) const
{
   Fragment retval{};
   if (aIndex < mFragments.size())
   {
      retval = mFragments[aIndex];
   }
   return retval;
}

std::vector<double> WsfNASA_BreakupModel::PackInputParameters(double aMass, double aSize)
{
   std::vector<double> retval(2);
   retval[cEXPLOSION_MASS_INDEX]   = aMass;
   retval[cEXPLOSION_LENGTH_INDEX] = aSize;
   return retval;
}

bool WsfNASA_BreakupModel::ModelExplosion(WsfPlatform& aPlatform, const std::vector<double>& aParams)
{
   if (!FinalizeExplosionModel(aPlatform, aParams))
   {
      return false;
   }

   double upperLimit = ExplosionN(mMinFragmentSize);
   double lowerLimit = ExplosionN(mProgenitorOne.mLength);
   auto   drawLc     = [this, &upperLimit, &lowerLimit]()
   { return InverseExplosionN(GetSimulation().GetRandom().Uniform(lowerLimit, upperLimit)); };

   // Do the bulk of the draw.
   double depositedMass{0.0};
   int    currentId{1};
   int    loopLimit = static_cast<int>(floor(upperLimit));
   mFragments.clear();
   for (int i = 0; i < loopLimit; ++i)
   {
      if (depositedMass / mProgenitorOne.mMass > 1.0 - mLargeFragmentFraction)
      {
         break;
      }
      Fragment frag = GenerateExplosiveFragment(drawLc(), currentId);
      mFragments.push_back(frag);
      depositedMass += frag.mMass;
      ++currentId;
   }

   // Now draw the rest as a number of large fragments.
   // NOTE: When selecting the large fragments, some choice needs to be
   //   made for their size. The method here is to draw from the higher end of
   //   the size of the object.
   double largestLc = std::min(1.0, mProgenitorOne.mLength * 0.9);
   int    numLarger = GetSimulation().GetRandom().Uniform(2, 8);
   for (int largeFragment = 0; largeFragment < numLarger; ++largeFragment)
   {
      bool     lastPiece = largeFragment == numLarger - 1;
      Fragment frag      = GenerateExplosiveLargeFragment(largestLc,
                                                     mProgenitorOne.mMass - depositedMass,
                                                     lastPiece,
                                                     currentId,
                                                     mProgenitorOne.mLocation,
                                                     mProgenitorOne.mVelocity);
      if (frag.IsInitialized())
      {
         mFragments.push_back(frag);
         depositedMass += frag.mMass;
         ++currentId;
      }
   }

   EnsureMomentumConservation(mFragments);

   return true;
}

bool WsfNASA_BreakupModel::FinalizeExplosionModel(WsfPlatform& aPlatform, const std::vector<double>& aParams)
{
   bool retval{false};
   if (aParams.size() == cNUM_EXPLOSION_PARAMS)
   {
      aPlatform.GetLocationECI(mProgenitorOne.mLocation.GetData());
      aPlatform.GetVelocityECI(mProgenitorOne.mVelocity.GetData());
      mProgenitorOne.mMass   = aParams[cEXPLOSION_MASS_INDEX];
      mProgenitorOne.mLength = aParams[cEXPLOSION_LENGTH_INDEX];
      mProgenitorOne.mName   = aPlatform.GetName();
      if (mDebrisName.empty())
      {
         mDebrisName = aPlatform.GetName() + "_debris";
      }
      retval = true;
   }
   return retval;
}

//! This method generates a fragment with the given size.
WsfNASA_BreakupModel::Fragment WsfNASA_BreakupModel::GenerateExplosiveFragment(double aLc, int aId)
{
   Fragment retval{};
   retval.mLength = aLc;
   if (mModelAsSpacecraft)
   {
      retval.mAoverM = AoverM_Spacecraft(aLc);
   }
   else
   {
      retval.mAoverM = AoverM_RocketBody(aLc);
   }
   retval.mArea        = AfromLc(aLc);
   retval.mMass        = retval.mArea / retval.mAoverM;
   double deltaV_Mag   = DeltaV_Explosion(retval.mAoverM);
   retval.mDeltaV_ECI  = GenerateRandomVelocity(deltaV_Mag);
   retval.mLocationECI = mProgenitorOne.mLocation;
   retval.mVelocityECI = mProgenitorOne.mVelocity + retval.mDeltaV_ECI;
   retval.mName        = mDebrisName + std::to_string(aId);
   return retval;
}

//! This method is used to generate a large fragment.
//!
//! This method needs to be used for the final fragments as it will guarantee that
//! mass is conserved. This method contains some implementation choices not
//! specified by (1) or (2).
//!
//! \param aMinLc         - The minimum size of a produced fragment.
//! \param aMassRemaining - The mass remaining to deposit in fragments.
//! \param aIsLast        - True if this is the last fragment.
//! \param aId            - The id of the fragment.
//! \param aLocation      - The location of the explosive breakup.
//! \param aVelocity      - The velocity of the explosive breakup.
WsfNASA_BreakupModel::Fragment WsfNASA_BreakupModel::GenerateExplosiveLargeFragment(double         aMinLc,
                                                                                    double         aMassRemaining,
                                                                                    bool           aIsLast,
                                                                                    int            aId,
                                                                                    const UtVec3d& aLocation,
                                                                                    const UtVec3d& aVelocity)
{
   Fragment retval{};
   if (aMassRemaining > 0.0)
   {
      if (aIsLast)
      {
         // This is dictated by mass conservation.
         retval.mMass = aMassRemaining;
      }
      else
      {
         // This is an implementation choice.
         retval.mMass = GetSimulation().GetRandom().Uniform(0.3, 0.5) * aMassRemaining;
      }

      // This is an implementation choice. The fragments will not be generated into
      // previous bins (smaller than aMinLc), and will not be generated larger than
      // the input object size.
      retval.mLength = GetSimulation().GetRandom().Uniform(aMinLc, mProgenitorOne.mLength);
      retval.mArea   = AfromLc(retval.mLength);

      // Note that because we pick length (effectively picking area) and mass, the
      // A/M values for large fragments will not follow the distribution in (1).
      retval.mAoverM      = retval.mArea / retval.mMass;
      double deltaV_Mag   = DeltaV_Explosion(retval.mAoverM);
      retval.mDeltaV_ECI  = GenerateRandomVelocity(deltaV_Mag);
      retval.mLocationECI = aLocation;
      retval.mVelocityECI = aVelocity + retval.mDeltaV_ECI;
      retval.mName        = mDebrisName + std::to_string(aId);
   }
   return retval;
}

bool WsfNASA_BreakupModel::ModelCollision(WsfPlatform&               aTargetPlatform,
                                          const std::vector<double>& aTargetParams,
                                          WsfPlatform&               aImpactorPlatform,
                                          const std::vector<double>& aImpactorParams)
{
   bool retval = FinalizeCollisionModel(aTargetPlatform, aTargetParams, aImpactorPlatform, aImpactorParams);
   if (retval)
   {
      if (mIsCatastrophicCollision)
      {
         retval = ModelCatastrophicCollision();
      }
      else
      {
         retval = ModelNonCatastrophicCollision();
      }
   }
   return retval;
}

bool WsfNASA_BreakupModel::FinalizeCollisionModel(WsfPlatform&               aTargetPlatform,
                                                  const std::vector<double>& aTargetParams,
                                                  WsfPlatform&               aImpactorPlatform,
                                                  const std::vector<double>& aImpactorParams)
{
   bool retval{false};

   aTargetPlatform.GetLocationECI(mProgenitorOne.mLocation.GetData());
   aImpactorPlatform.GetLocationECI(mProgenitorTwo.mLocation.GetData());
   mProgenitorOne.mLength = aTargetParams[cCOLLISION_LENGTH_INDEX];
   mProgenitorTwo.mLength = aImpactorParams[cCOLLISION_LENGTH_INDEX];

   // test that they are 'close'
   UtVec3d diff  = mProgenitorOne.mLocation - mProgenitorTwo.mLocation;
   double  limit = cCOLLISION_NEARNESS_FACTOR * (mProgenitorOne.mLength + mProgenitorTwo.mLength);
   if (diff.Magnitude() <= limit)
   {
      mProgenitorOne.mName = aTargetPlatform.GetName();
      mProgenitorOne.mMass = aTargetParams[cCOLLISION_MASS_INDEX];
      aTargetPlatform.GetVelocityECI(mProgenitorOne.mVelocity.GetData());

      mProgenitorTwo.mName = aImpactorPlatform.GetName();
      mProgenitorTwo.mMass = aImpactorParams[cCOLLISION_MASS_INDEX];
      aImpactorPlatform.GetVelocityECI(mProgenitorTwo.mVelocity.GetData());

      if (mProgenitorOne.mMass > mProgenitorTwo.mMass)
      {
         std::swap(mProgenitorOne, mProgenitorTwo);
      }

      UtVec3d vdiff           = mProgenitorOne.mVelocity - mProgenitorTwo.mVelocity;
      mCollisionRelativeSpeed = vdiff.Magnitude();
      ComputeCollisionMass();
      if (mDebrisName.empty())
      {
         mDebrisName = mProgenitorOne.mName + "_" + mProgenitorTwo.mName + "_debris";
      }
      retval = true;
   }
   else
   {
      auto logger = ut::log::warning() << "Collision progenitors are not near enough to collide.";
      logger.AddNote() << "They are " << diff.Magnitude() << " m apart, and the limit is " << limit << " m.";
      logger.AddNote() << "Target Platform: " << aTargetPlatform.GetName();
      logger.AddNote() << "Impactor Platform: " << aImpactorPlatform.GetName();
   }

   return retval;
}

bool WsfNASA_BreakupModel::ModelCatastrophicCollision()
{
   ModelCollisionDebris(mCollisionMass, mProgenitorOne);
   ModelCollisionDebris(mCollisionMass, mProgenitorTwo);
   return true;
}

bool WsfNASA_BreakupModel::ModelNonCatastrophicCollision()
{
   // NOTE: This assumes that of the collision involved mass, some of it
   //   will 'blow through' the target, and some will generate debris that
   //   tends to remain on closer orbits to the target. This factor
   //   decides how the mass is divided among those two portions of the
   //   generated debris.
   constexpr double cNON_CATASTROPHIC_MASS_DIVISION_FACTOR{0.9};

   // Work out various progenitors
   Progenitor blowThrough{mProgenitorOne};
   blowThrough.mMass = mCollisionMass * cNON_CATASTROPHIC_MASS_DIVISION_FACTOR;

   Progenitor nearbyDebris{mProgenitorTwo};
   nearbyDebris.mMass   = mCollisionMass * (1.0 - cNON_CATASTROPHIC_MASS_DIVISION_FACTOR);
   nearbyDebris.mLength = mProgenitorOne.mLength;

   // Now setup fragments
   double blowthroughMass  = ModelCollisionDebris(mCollisionMass, blowThrough, false);
   blowThrough.mMass       = blowthroughMass;
   size_t blowthroughCount = mFragments.size();
   double nearbyMass       = ModelCollisionDebris(mCollisionMass, nearbyDebris, false);

   Progenitor cratered{mProgenitorTwo};
   cratered.mMass = mProgenitorOne.mMass + mProgenitorTwo.mMass - blowthroughMass - nearbyMass;
   mFragments.push_back(GenerateCrateredFragment(cratered));
   cratered.mMass += nearbyMass;

   // given the generated masses, solve the collision problem for the final velocities
   // then correct momenta and add the initial offsets.
   Progenitor finalOne;
   Progenitor finalTwo;
   std::tie(finalOne, finalTwo) = SolveCollisionProblem(blowThrough, cratered);

   auto iter = mFragments.begin() + blowthroughCount;
   std::for_each(mFragments.begin(),
                 iter,
                 [&finalOne](Fragment& aFragment)
                 { aFragment.mVelocityECI = aFragment.mDeltaV_ECI + finalOne.mVelocity; });
   std::for_each(iter,
                 mFragments.end(),
                 [&finalTwo](Fragment& aFragment)
                 { aFragment.mVelocityECI = aFragment.mDeltaV_ECI + finalTwo.mVelocity; });
   EnsureMomentumConservation(mFragments.begin(), iter);
   EnsureMomentumConservation(iter, mFragments.end());

   return true;
}

double WsfNASA_BreakupModel::ModelCollisionDebris(double aTotalMass, const Progenitor& aProgenitor, bool aGenerateLargeFragments)
{
   double upperLimit = CollisionN(mMinFragmentSize, aTotalMass);
   double lowerLimit = CollisionN(aProgenitor.mLength, aTotalMass);
   auto   drawLc     = [this, &upperLimit, &lowerLimit, &aTotalMass]()
   { return InverseCollisionN(GetSimulation().GetRandom().Uniform(lowerLimit, upperLimit), aTotalMass); };

   std::vector<Fragment> addedFragments{};
   double                depositedMass{0.0};
   int                   currentId = ut::safe_cast<int, size_t>(mFragments.size());
   int                   loopLimit = static_cast<int>(floor(upperLimit));
   for (int i = 0; i < loopLimit; ++i)
   {
      if (aGenerateLargeFragments && depositedMass / aProgenitor.mMass > 1.0 - mLargeFragmentFraction)
      {
         break;
      }
      Fragment frag = GenerateCollisionalFragment(drawLc(), currentId, aProgenitor.mLocation, aProgenitor.mVelocity);
      if (frag.mMass + depositedMass > aProgenitor.mMass)
      {
         // If the generated fragment is too large, we should break and proceed to large fragment generation.
         break;
      }
      addedFragments.push_back(frag);
      depositedMass += frag.mMass;
      ++currentId;
   }

   if (aGenerateLargeFragments)
   {
      // NOTE: When selecting the large fragments, some choice needs to be
      //   made for their size. The method here is to draw from the higher end of
      //   the size of the object.
      double largestLc = std::min(1.0, aProgenitor.mLength * 0.9);
      int    numLarger = GetSimulation().GetRandom().Uniform(2, 8);
      for (int largeFragment = 0; largeFragment < numLarger; ++largeFragment)
      {
         bool     lastPiece = largeFragment == numLarger - 1;
         Fragment frag      = GenerateCollisionalLargeFragment(largestLc,
                                                          aProgenitor.mMass - depositedMass,
                                                          lastPiece,
                                                          currentId,
                                                          aProgenitor.mLocation,
                                                          aProgenitor.mVelocity);
         if (frag.IsInitialized())
         {
            addedFragments.push_back(frag);
            depositedMass += frag.mMass;
            ++currentId;
         }
      }

      EnsureMomentumConservation(addedFragments);
   }

   // Add them to the overall collection.
   mFragments.insert(mFragments.end(), addedFragments.begin(), addedFragments.end());

   return depositedMass;
}

WsfNASA_BreakupModel::Fragment WsfNASA_BreakupModel::GenerateCollisionalFragment(double         aLc,
                                                                                 int            aId,
                                                                                 const UtVec3d& aLocation,
                                                                                 const UtVec3d& aVelocity)
{
   Fragment retval{};
   retval.mLength = aLc;
   if (mModelAsSpacecraft)
   {
      retval.mAoverM = AoverM_Spacecraft(aLc);
   }
   else
   {
      retval.mAoverM = AoverM_RocketBody(aLc);
   }
   retval.mArea        = AfromLc(aLc);
   retval.mMass        = retval.mArea / retval.mAoverM;
   double deltaV_Mag   = DeltaV_Collision(retval.mAoverM);
   retval.mDeltaV_ECI  = GenerateRandomVelocity(deltaV_Mag);
   retval.mLocationECI = aLocation;
   retval.mVelocityECI = aVelocity + retval.mDeltaV_ECI;
   retval.mName        = mDebrisName + std::to_string(aId);
   return retval;
}

WsfNASA_BreakupModel::Fragment WsfNASA_BreakupModel::GenerateCollisionalLargeFragment(double         aMinLc,
                                                                                      double         aMassRemaining,
                                                                                      bool           aIsLast,
                                                                                      int            aId,
                                                                                      const UtVec3d& aLocation,
                                                                                      const UtVec3d& aVelocity)
{
   Fragment retval{};
   if (aMassRemaining > 0.0)
   {
      if (aIsLast)
      {
         // This is dictated by mass conservation.
         retval.mMass = aMassRemaining;
      }
      else
      {
         // This is an implementation choice.
         retval.mMass = GetSimulation().GetRandom().Uniform(0.3, 0.5) * aMassRemaining;
      }

      retval.mLength = GetSimulation().GetRandom().Uniform(aMinLc, mProgenitorOne.mLength);

      if (mModelAsSpacecraft)
      {
         retval.mAoverM = AoverM_Spacecraft(retval.mLength);
      }
      else
      {
         retval.mAoverM = AoverM_RocketBody(retval.mLength);
      }

      retval.mArea = retval.mAoverM * retval.mMass;

      double deltaV_Mag   = DeltaV_Collision(retval.mAoverM);
      retval.mDeltaV_ECI  = GenerateRandomVelocity(deltaV_Mag);
      retval.mLocationECI = aLocation;
      retval.mVelocityECI = aVelocity + retval.mDeltaV_ECI;
      retval.mName        = mDebrisName + std::to_string(aId);
   }
   return retval;
}

WsfNASA_BreakupModel::Fragment WsfNASA_BreakupModel::GenerateCrateredFragment(const Progenitor& aProgenitor)
{
   Fragment retval{};
   retval.mMass        = aProgenitor.mMass;
   retval.mLength      = aProgenitor.mLength;
   retval.mArea        = AfromLc(retval.mLength);
   retval.mAoverM      = retval.mArea / retval.mMass;
   retval.mDeltaV_ECI  = UtVec3d{};
   retval.mLocationECI = aProgenitor.mLocation;
   retval.mVelocityECI = aProgenitor.mVelocity + retval.mDeltaV_ECI;
   retval.mName        = mProgenitorTwo.mName + "_cratered";
   return retval;
}

std::pair<WsfNASA_BreakupModel::Progenitor, WsfNASA_BreakupModel::Progenitor>
WsfNASA_BreakupModel::SolveCollisionProblem(const Progenitor& aSmall, const Progenitor& aLarge) const
{
   // 1) Move to COM frame
   double  totalMass          = aSmall.mMass + aLarge.mMass;
   UtVec3d momentumCOM        = aSmall.mVelocity * aSmall.mMass + aLarge.mVelocity * aLarge.mMass;
   UtVec3d velocityCOM        = momentumCOM * (1.0 / totalMass);
   UtVec3d initVelocityOneCOM = aSmall.mVelocity - velocityCOM;
   double  initSpeedOneCOM    = initVelocityOneCOM.Magnitude();
   UtVec3d initVelocityTwoCOM = aLarge.mVelocity - velocityCOM;
   double  initSpeedTwoCOM    = initVelocityTwoCOM.Magnitude();

   // 2) Compute final large cluster overall motions
   double finalMassOne = aSmall.mMass;
   double finalMassTwo = aLarge.mMass;
   // This is actually 2 * T
   double  totalT = aSmall.mMass * initSpeedOneCOM * initSpeedOneCOM + aLarge.mMass * initSpeedTwoCOM * initSpeedTwoCOM;
   double  finalSpeedOneCOM    = sqrt(totalT / (finalMassOne * (1.0 + finalMassOne / finalMassTwo)));
   double  finalSpeedTwoCOM    = sqrt(totalT / (finalMassTwo * (1.0 + finalMassTwo / finalMassOne)));
   UtVec3d finalVelocityOneCOM = initVelocityOneCOM * (finalSpeedOneCOM / initSpeedOneCOM);
   UtVec3d finalVelocityTwoCOM = initVelocityTwoCOM * (finalSpeedTwoCOM / initSpeedTwoCOM);

   // 3) Return from COM frame
   Progenitor returnOne{aSmall};
   returnOne.mMass     = finalMassOne;
   returnOne.mVelocity = finalVelocityOneCOM + velocityCOM;

   Progenitor returnTwo{aLarge};
   returnTwo.mMass     = finalMassTwo;
   returnTwo.mVelocity = finalVelocityTwoCOM + velocityCOM;

   return std::make_pair(returnOne, returnTwo);
}

//! Pick a velocity with a random direction with the given magnitude.
//!
//! Reference (1) does not specify how the directions of the changes in
//! velocity of the fragments are to be chosen. This simply selects
//! from a uniform distribution on S^2. This will not guarantee momentum
//! conservation, so we correct that with CorrectMomentumConservation.
UtVec3d WsfNASA_BreakupModel::GenerateRandomVelocity(double aMagnitude)
{
   double cosTheta = GetSimulation().GetRandom().Uniform(-1.0, 1.0);
   double sinTheta = sqrt(1.0 - cosTheta * cosTheta);
   double phi      = GetSimulation().GetRandom().Uniform(0.0, UtMath::cTWO_PI);
   double cosPhi   = cos(phi);
   double sinPhi   = sin(phi);
   return UtVec3d{aMagnitude * sinTheta * cosPhi, aMagnitude * sinTheta * sinPhi, aMagnitude * cosTheta};
}

void WsfNASA_BreakupModel::EnsureMomentumConservation(std::vector<Fragment>& aFragments)
{
   EnsureMomentumConservation(aFragments.begin(), aFragments.end());
}

void WsfNASA_BreakupModel::EnsureMomentumConservation(std::vector<Fragment>::iterator aStart,
                                                      std::vector<Fragment>::iterator aEnd)
{
   UtVec3d pTotal{};
   double  mass{0.0};
   std::for_each(aStart,
                 aEnd,
                 [&pTotal, &mass](const Fragment& aFragment)
                 {
                    pTotal += aFragment.mDeltaV_ECI * aFragment.mMass;
                    mass += aFragment.mMass;
                 });

   pTotal *= (-1.0 / mass);

   std::for_each(aStart,
                 aEnd,
                 [&pTotal](Fragment& aFragment)
                 {
                    aFragment.mDeltaV_ECI += pTotal;
                    aFragment.mVelocityECI += pTotal;
                 });
}

double WsfNASA_BreakupModel::ExplosionN(double aLc) const
{
   return mExplosionS_Factor * cEXPLOSION_N_PREFACTOR * pow(aLc, cEXPLOSION_N_SCALING);
}

double WsfNASA_BreakupModel::InverseExplosionN(double aCn) const
{
   return pow(aCn / (cEXPLOSION_N_PREFACTOR * mExplosionS_Factor), 1.0 / cEXPLOSION_N_SCALING);
}

bool WsfNASA_BreakupModel::IsCatastrophicCollision() const
{
   constexpr double cCATASTROPHIC_ETA_LIMIT = 40000.0; // This is 40 kJ / kg.
   double eta = 0.5 * mProgenitorOne.mMass * mCollisionRelativeSpeed * mCollisionRelativeSpeed / mProgenitorTwo.mMass;
   return eta > cCATASTROPHIC_ETA_LIMIT;
}

double WsfNASA_BreakupModel::CatastrophicCollisionMass() const
{
   return mProgenitorOne.mMass + mProgenitorTwo.mMass;
}

double WsfNASA_BreakupModel::NonCatastrophicCollisionMass() const
{
   constexpr double cMPS_PER_KPS = 1000.0;
   // NOTE: There is a disagreement between (1) and (2) on the scaling here. This uses (2).
   return mProgenitorOne.mMass * pow(mCollisionRelativeSpeed / cMPS_PER_KPS, 2.0);
}

void WsfNASA_BreakupModel::ComputeCollisionMass()
{
   mIsCatastrophicCollision = IsCatastrophicCollision();
   if (mIsCatastrophicCollision)
   {
      mCollisionMass = CatastrophicCollisionMass();
   }
   else
   {
      mCollisionMass = NonCatastrophicCollisionMass();
   }
}

double WsfNASA_BreakupModel::CollisionN(double aLc, double aCollisionMass)
{
   return cCOLLISION_N_PREFACTOR * pow(aCollisionMass, cCOLLISION_N_MASS_SCALING) * pow(aLc, cCOLLISION_N_SIZE_SCALING);
}

double WsfNASA_BreakupModel::InverseCollisionN(double aCn, double aCollisionMass)
{
   return pow(aCn / (cCOLLISION_N_PREFACTOR * pow(aCollisionMass, cCOLLISION_N_MASS_SCALING)),
              1.0 / cCOLLISION_N_SIZE_SCALING);
}

//! Perform a LERP operation with constant limits.
//!
//! Unlike the standard UtMath::Lerp operation, which extrapolates past the provided
//! endpoints, this will limit the values produced to fall in the range [@p aRangeMin, @p aRangeMax].
//! This is a linear interpolating between two regions of constant value.
//!
//! \param aValue    - The value of the parameter at which to get the limited interpolated value.
//! \param aValueMin - The lower end of the parameter range over which the linear interpolation occurs.
//! \param aValueMax - The upper end of the parameter range over which the linear interpolation occurs.
//! \param aRangeMin - The constant value that is returned when @p aValue < @p aValueMin.
//! \param aRangeMax - The constant value that is returned when @p aValue > @p aValueMax.
//! \returns         - The limited linear interpolation.
double WsfNASA_BreakupModel::LimitedLerp(double aValue, double aValueMin, double aValueMax, double aRangeMin, double aRangeMax)
{
   return UtMath::Limit(UtMath::Lerp(aValue, aValueMin, aValueMax, aRangeMin, aRangeMax), aRangeMin, aRangeMax);
}

double WsfNASA_BreakupModel::BlendFunctionWeight(double aLc)
{
   // NOTE: The connection between the small size and large size A/M distributions is not
   //       made explicitly in (1). The choice made here is to linearly interpolate between them.
   constexpr double cBLEND_LC_MIN{0.08};
   constexpr double cBLEND_LC_MAX{0.11};
   constexpr double cBLEND_RANGE_MIN{0.0};
   constexpr double cBLEND_RANGE_MAX{1.0};

   return LimitedLerp(aLc, cBLEND_LC_MIN, cBLEND_LC_MAX, cBLEND_RANGE_MIN, cBLEND_RANGE_MAX);
}

double WsfNASA_BreakupModel::AoverM_SmallSize(double aLambdaC)
{
   constexpr double cAOM_SMALL_LC_MIN{-1.75};
   constexpr double cAOM_SMALL_LC_MAX{-1.25};
   constexpr double cAOM_SMALL_RANGE_MIN{-0.3};
   constexpr double cAOM_SMALL_RANGE_MAX{-1.0};

   constexpr double cAOM_SMALL_SIGMA_LC_MIN{-3.5};
   constexpr double cAOM_SMALL_SIGMA_LC_MAX{0.0};
   constexpr double cAOM_SMALL_SIGMA_RANGE_MIN{0.2};
   constexpr double cAOM_SMALL_SIGMA_RANGE_MAX{2.0 / 3.0};

   double mu = LimitedLerp(aLambdaC, cAOM_SMALL_LC_MIN, cAOM_SMALL_LC_MAX, cAOM_SMALL_RANGE_MIN, cAOM_SMALL_RANGE_MAX);

   // NOTE: This does not follow the pattern of all other coefficients in that there is no
   //  upper bound to aLambdaC. This implies the direct use of Lerp in this instance.
   double sigma{cAOM_SMALL_SIGMA_RANGE_MIN};
   if (aLambdaC > cAOM_SMALL_SIGMA_LC_MIN)
   {
      sigma = UtMath::Lerp(aLambdaC,
                           cAOM_SMALL_SIGMA_LC_MIN,
                           cAOM_SMALL_SIGMA_LC_MAX,
                           cAOM_SMALL_SIGMA_RANGE_MIN,
                           cAOM_SMALL_SIGMA_RANGE_MAX);
   }

   double chi = GetSimulation().GetRandom().Normal(mu, sigma);
   return pow(10.0, chi);
}

double WsfNASA_BreakupModel::AoverM_RocketBodyTermOne(double aLambdaC)
{
   constexpr double cAOM_RB_TERM_ONE_MU_LC_MIN{-0.5};
   constexpr double cAOM_RB_TERM_ONE_MU_LC_MAX{0.0};
   constexpr double cAOM_RB_TERM_ONE_MU_RANGE_MIN{-0.45};
   constexpr double cAOM_RB_TERM_ONE_MU_RANGE_MAX{-0.9};

   constexpr double cAOM_RB_TERM_ONE_SIGMA{0.55};

   double mu = LimitedLerp(aLambdaC,
                           cAOM_RB_TERM_ONE_MU_LC_MIN,
                           cAOM_RB_TERM_ONE_MU_LC_MAX,
                           cAOM_RB_TERM_ONE_MU_RANGE_MIN,
                           cAOM_RB_TERM_ONE_MU_RANGE_MAX);

   double chi = GetSimulation().GetRandom().Normal(mu, cAOM_RB_TERM_ONE_SIGMA);
   return pow(10.0, chi);
}

double WsfNASA_BreakupModel::AoverM_RocketBodyTermTwo(double aLambdaC)
{
   constexpr double cAOM_RB_TERM_TWO_MU{-0.9};

   constexpr double cAOM_RB_TERM_TWO_SIGMA_LC_MIN{-1.0};
   constexpr double cAOM_RB_TERM_TWO_SIGMA_LC_MAX{0.1};
   constexpr double cAOM_RB_TERM_TWO_SIGMA_RANGE_MIN{0.28};
   constexpr double cAOM_RB_TERM_TWO_SIGMA_RANGE_MAX{0.1};

   double sigma = LimitedLerp(aLambdaC,
                              cAOM_RB_TERM_TWO_SIGMA_LC_MIN,
                              cAOM_RB_TERM_TWO_SIGMA_LC_MAX,
                              cAOM_RB_TERM_TWO_SIGMA_RANGE_MIN,
                              cAOM_RB_TERM_TWO_SIGMA_RANGE_MAX);
   double chi   = GetSimulation().GetRandom().Normal(cAOM_RB_TERM_TWO_MU, sigma);
   return pow(10.0, chi);
}

double WsfNASA_BreakupModel::AoverM_RocketBody(double aLc)
{
   constexpr double cAOM_RB_ALPHA_LC_MIN{-1.4};
   constexpr double cAOM_RB_ALPHA_LC_MAX{0.0};
   constexpr double cAOM_RB_ALPHA_LC_RANGE_MIN{1.0};
   constexpr double cAOM_RB_ALPHA_LC_RANGE_MAX{0.5};

   double lambdaC = log10(aLc);
   double alpha =
      LimitedLerp(lambdaC, cAOM_RB_ALPHA_LC_MIN, cAOM_RB_ALPHA_LC_MAX, cAOM_RB_ALPHA_LC_RANGE_MIN, cAOM_RB_ALPHA_LC_RANGE_MAX);
   double blend = BlendFunctionWeight(aLc);

   // The distribution is a weighted sum of Gaussian distributions. However, we only
   // need to compute one weight directly. The others are left in comments for clarity.
   double w1 = blend * alpha;
   // double w2 = blend * (1.0 - alpha);   // Note that w1 + w2 = blend
   // double w3 = 1.0 - blend;

   double retval{};
   double choice = GetSimulation().GetRandom().Uniform(0.0, 1.0);
   if (choice <= w1)
   {
      retval = AoverM_RocketBodyTermOne(lambdaC);
   }
   else if (choice <= blend)
   {
      retval = AoverM_RocketBodyTermTwo(lambdaC);
   }
   else
   {
      retval = AoverM_SmallSize(lambdaC);
   }
   return retval;
}

double WsfNASA_BreakupModel::AoverM_SpacecraftTermOne(double aLambdaC)
{
   constexpr double cAOM_SC_TERM_ONE_MU_LC_MIN{-1.1};
   constexpr double cAOM_SC_TERM_ONE_MU_LC_MAX{0.0};
   constexpr double cAOM_SC_TERM_ONE_MU_LC_RANGE_MIN{-0.6};
   constexpr double cAOM_SC_TERM_ONE_MU_LC_RANGE_MAX{-0.95};

   constexpr double cAOM_SC_TERM_ONE_SIGMA_LC_MIN{-1.3};
   constexpr double cAOM_SC_TERM_ONE_SIGMA_LC_MAX{-0.3};
   constexpr double cAOM_SC_TERM_ONE_SIGMA_RANGE_MIN{0.1};
   constexpr double cAOM_SC_TERM_ONE_SIGMA_RANGE_MAX{0.3};

   double mu    = LimitedLerp(aLambdaC,
                           cAOM_SC_TERM_ONE_MU_LC_MIN,
                           cAOM_SC_TERM_ONE_MU_LC_MAX,
                           cAOM_SC_TERM_ONE_MU_LC_RANGE_MIN,
                           cAOM_SC_TERM_ONE_MU_LC_RANGE_MAX);
   double sigma = LimitedLerp(aLambdaC,
                              cAOM_SC_TERM_ONE_SIGMA_LC_MIN,
                              cAOM_SC_TERM_ONE_SIGMA_LC_MAX,
                              cAOM_SC_TERM_ONE_SIGMA_RANGE_MIN,
                              cAOM_SC_TERM_ONE_SIGMA_RANGE_MAX);
   double chi   = GetSimulation().GetRandom().Normal(mu, sigma);
   return pow(10.0, chi);
}

double WsfNASA_BreakupModel::AoverM_SpacecraftTermTwo(double aLambdaC)
{
   constexpr double cAOM_SC_TERM_TWO_MU_LC_MIN{-0.7};
   constexpr double cAOM_SC_TERM_TWO_MU_LC_MAX{-0.1};
   constexpr double cAOM_SC_TERM_TWO_MU_LC_RANGE_MIN{-1.2};
   constexpr double cAOM_SC_TERM_TWO_MU_LC_RANGE_MAX{-2.0};

   constexpr double cAOM_SC_TERM_TWO_SIGMA_LC_MIN{-0.5};
   constexpr double cAOM_SC_TERM_TWO_SIGMA_LC_MAX{-0.3};
   constexpr double cAOM_SC_TERM_TWO_SIGMA_RANGE_MIN{0.5};
   constexpr double cAOM_SC_TERM_TWO_SIGMA_RANGE_MAX{0.3};

   double mu    = LimitedLerp(aLambdaC,
                           cAOM_SC_TERM_TWO_MU_LC_MIN,
                           cAOM_SC_TERM_TWO_MU_LC_MAX,
                           cAOM_SC_TERM_TWO_MU_LC_RANGE_MIN,
                           cAOM_SC_TERM_TWO_MU_LC_RANGE_MAX);
   double sigma = LimitedLerp(aLambdaC,
                              cAOM_SC_TERM_TWO_SIGMA_LC_MIN,
                              cAOM_SC_TERM_TWO_SIGMA_LC_MAX,
                              cAOM_SC_TERM_TWO_SIGMA_RANGE_MIN,
                              cAOM_SC_TERM_TWO_SIGMA_RANGE_MAX);
   double chi   = GetSimulation().GetRandom().Normal(mu, sigma);
   return pow(10.0, chi);
}

double WsfNASA_BreakupModel::AoverM_Spacecraft(double aLc)
{
   constexpr double cAOM_SC_ALPHA_LC_MIN{-1.95};
   constexpr double cAOM_SC_ALPHA_LC_MAX{0.55};
   constexpr double cAOM_SC_ALPHA_RANGE_MIN{0.0};
   constexpr double cAOM_SC_ALPHA_RANGE_MAX{1.0};

   double lambdaC = log10(aLc);
   double alpha =
      LimitedLerp(lambdaC, cAOM_SC_ALPHA_LC_MIN, cAOM_SC_ALPHA_LC_MAX, cAOM_SC_ALPHA_RANGE_MIN, cAOM_SC_ALPHA_RANGE_MAX);
   double blend = BlendFunctionWeight(aLc);

   // The distribution is a weighted sum of Gaussian distributions. However, we only
   // need to compute one weight directly. The others are left in comments for clarity.
   double w1 = blend * alpha;
   // double w2 = blend * (1.0 - alpha);      // Note that w1 + w2 = blend
   // double w3 = 1.0 - blend;

   double retval{};
   double choice = GetSimulation().GetRandom().Uniform(0.0, 1.0);
   if (choice <= w1)
   {
      retval = AoverM_SpacecraftTermOne(lambdaC);
   }
   else if (choice <= blend)
   {
      retval = AoverM_SpacecraftTermTwo(lambdaC);
   }
   else
   {
      retval = AoverM_SmallSize(lambdaC);
   }
   return retval;
}

double WsfNASA_BreakupModel::AfromLc(double aLc)
{
   constexpr double cA_FROM_LC_LIMIT{0.00167};
   constexpr double cA_FROM_LC_LOW_PREFACTOR{0.540424};
   constexpr double cA_FROM_LC_LOW_SCALING{2.0};
   constexpr double cA_FROM_LC_HIGH_PREFACTOR{0.556945};
   constexpr double cA_FROM_LC_HIGH_SCALING{2.0047077};

   double retval{};
   if (aLc < cA_FROM_LC_LIMIT)
   {
      retval = cA_FROM_LC_LOW_PREFACTOR * pow(aLc, cA_FROM_LC_LOW_SCALING);
   }
   else
   {
      retval = cA_FROM_LC_HIGH_PREFACTOR * pow(aLc, cA_FROM_LC_HIGH_SCALING);
   }
   return retval;
}

double WsfNASA_BreakupModel::DeltaV_Explosion(double aAoverM)
{
   constexpr double cDELTA_V_EXPLOSION_MU_SLOPE{0.2};
   constexpr double cDELTA_V_EXPLOSION_MU_INTERCEPT{1.85};
   constexpr double cDELTA_V_EXPLOSION_SIGMA{0.4};

   double chi = log10(aAoverM);
   double mu  = cDELTA_V_EXPLOSION_MU_SLOPE * chi + cDELTA_V_EXPLOSION_MU_INTERCEPT;
   double nu  = GetSimulation().GetRandom().Normal(mu, cDELTA_V_EXPLOSION_SIGMA);
   return pow(10.0, nu);
}

double WsfNASA_BreakupModel::DeltaV_Collision(double aAoverM)
{
   // The NASA Breakup Model as described in (1) does a bad job matching realistic
   // collisions. See Figure 7 in (3), for example. So we adopt a simpler approach
   // here that does not tend to create very large delta v values.
   constexpr double cDELTA_V_COLLISION_MU{2.0};
   constexpr double cDELTA_V_COLLISION_SIGMA{0.3};

   double nu = GetSimulation().GetRandom().Normal(cDELTA_V_COLLISION_MU, cDELTA_V_COLLISION_SIGMA);
   return pow(10.0, nu);
}
