// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#include "WsfExoAtmosphericLethality.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <limits>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfWeaponEngagement.hpp"

// namespace {
//    bool operator<(const pair<double, double>& aLHS, const pair<double, double>& aRHS)
//    {
//       return aLHS.first < aRHS.first;
//    }
// }

//! Default constructor
WsfExoAtmosphericLethality::WsfExoAtmosphericLethality(WsfScenario& aScenario)
   : WsfExplicitWeaponEffects(aScenario)
   , mEnableDebris(false)
   , mImpactVelocityPoints()
   , mImpactAnglePoints()
   ,
   // mDebrisTypes(),
   mRandom()
{
   SetUseInterceptPk();
}

// virtual
WsfWeaponEffects* WsfExoAtmosphericLethality::Clone() const
{
   return new WsfExoAtmosphericLethality(*this);
}

// virtual
bool WsfExoAtmosphericLethality::PreInitialize()
{
   bool success = WsfExplicitWeaponEffects::PreInitialize();

   sort(mImpactVelocityPoints.begin(), mImpactVelocityPoints.end());
   sort(mImpactAnglePoints.begin(), mImpactAnglePoints.end());

   unsigned int i = 0;
   for (i = 0; i != mImpactVelocityPoints.size(); ++i)
   {
      // cout << "FYI:  Exoatmospheric Sort:  V, Pk = " << mImpactVelocityPoints[i].first
      //    << ", " << mImpactVelocityPoints[i].second << endl;
      if (i > 0)
      {
         if (mImpactVelocityPoints[i - 1].first >= mImpactVelocityPoints[i].first)
         {
            auto out = ut::log::info() << "Lethality did not sort into a continuously increasing velocity for Pk.";
            out.AddNote() << "Weapon: " << GetEngagement()->GetWeaponPlatform()->GetName();
            out.AddNote() << "Lethality: " << GetName();
            return false;
         }
      }
   }
   for (i = 0; i != mImpactAnglePoints.size(); ++i)
   {
      // cout << "FYI:  Exoatmospheric Sort:  V, Pk = " << mImpactAnglePoints[i].first
      //    << ", " << mImpactAnglePoints[i].second << endl;
      if (i > 0)
      {
         if (mImpactAnglePoints[i - 1].first >= mImpactAnglePoints[i].first)
         {
            auto out = ut::log::info() << "Lethality did not sort into a continuously increasing angle for Pk.";
            out.AddNote() << "Weapon: " << GetEngagement()->GetWeaponPlatform()->GetName();
            out.AddNote() << "Lethality: " << GetName();
            return false;
         }
      }
   }

   return success;
}

// virtual
bool WsfExoAtmosphericLethality::Initialize(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   // Initialize the random number stream.  Each effect has a different seed.
   unsigned int seed = GetSimulation()->LockRandom().Uniform<unsigned>();
   GetSimulation()->UnlockRandom();
   mRandom.SetSeed(seed);

   return WsfExplicitWeaponEffects::Initialize(aSimTime, aEngagementPtr);
}

// virtual
bool WsfExoAtmosphericLethality::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if ((command == "impact_angle_and_pk") || (command == "impact_angle_and_Pk"))
   {
      double angle;
      double pk;
      aInput.ReadValueOfType(angle, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(angle, 0.0);
      aInput.ValueLessOrEqual(angle, UtMath::cPI);
      aInput.ReadValue(pk);
      aInput.ValueGreaterOrEqual(pk, 0.0);
      aInput.ValueLessOrEqual(pk, 1.0);
      mImpactAnglePoints.push_back(std::pair<double, double>(angle, pk));
   }
   else if ((command == "impact_velocity_and_pk") || (command == "impact_velocity_and_Pk"))
   {
      double speed;
      double pk;
      aInput.ReadValueOfType(speed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(speed, 0.0);
      aInput.ReadValue(pk);
      aInput.ValueGreaterOrEqual(pk, 0.0);
      aInput.ValueLessOrEqual(pk, 1.0);
      mImpactVelocityPoints.push_back(std::pair<double, double>(speed, pk));
   }
   /*else if (command == "impact_debris")
   {
      //mEnableDebris = true; // TODO - Not currently available
      UtInputBlock block(aInput);
      ProcessDebrisBlock(block);
   }*/
   else if ((command == "use_launch_pk") || (command == "launch_pk") || (command == "intercept_pk"))
   {
      throw UtInput::BadValue(aInput, "Incompatible option: " + command);
   }
   else
   {
      myCommand = WsfExplicitWeaponEffects::ProcessInput(aInput);
   }

   return myCommand;
}

/*void WsfExoAtmosphericLethality::ProcessDebrisBlock(UtInputBlock& aInputBlock)
{
   while (aInputBlock.ReadCommand())
   {
      string command = aInputBlock.GetCommand();
      UtInput& input = aInputBlock.GetInput();
      if (command == "enabled")
      {
         input.ReadValue(mEnableDebris);
         if (mEnableDebris)
         {
            throw UtInput::BadValue(input, "Debris creation after a weapon impact is not currently available.");
         }
      }
      else if (command == "default")
      {
         if (mDebrisTypes.empty())
         {
            mDebrisTypes.push_back(DebrisType());
         }
         UtInputBlock block(aInputBlock.GetInput());
         mDebrisTypes[0].ProcessBlock(block);
      }
      else if (! ProcessInput(aInputBlock.GetInput()))
      {
         throw UtInput::UnknownCommand(aInputBlock.GetInput());
      }
   }
}*/

// protected
double WsfExoAtmosphericLethality::InterpPkArray(double aIV_Value, const Points& aArray) const
{
   // Simple linear interpolation of the data.  Do not extrapolate past the ends.

   // If no array, then this IV does not contribute.  Pk contribution = 1.0
   if (aArray.empty())
   {
      return 1.0;
   }

   // If below the valid IV data, return the lowest without extrapolating
   if (aIV_Value <= aArray[0].first)
   {
      return aArray[0].second;
   }

   // If above the valid IV data, return the highest DV, without extrapolating
   size_t index = aArray.size() - 1;
   if (aIV_Value >= aArray[index].first)
   {
      return aArray[index].second;
   }

   // Otherwise interpolate:
   double X1 = aArray[0].first;
   double Y1 = aArray[0].second;

   for (index = 1; index != aArray.size(); ++index)
   {
      if (aArray[index].first < aIV_Value)
      {
         X1 = aArray[index].first;
         Y1 = aArray[index].second;
      }
      else
      {
         double X2 = aArray[index].first;
         double Y2 = aArray[index].second;
         // Now do the interpolation:
         double interpolant = (aIV_Value - X1) / (X2 - X1);
         return Y1 + interpolant * (Y2 - Y1);
      }
   }
   assert(false);
   return 0.0;
}


// protected virtual
void WsfExoAtmosphericLethality::CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr)
{
   // This Pk will use true interceptor and target velocities to determine
   // Pk as a function of closing velocity, and/or intercept angle.  It does
   // not try to calculate a true miss distance, as the weapon fuse already
   // contains much elaborate code to do so.  This method merely assumes that
   // everything has proceeded according to plan so far, and that a sufficient
   // closing speed and intercept angle is the only degradation to consider:

   if (Defeated())
   {
      SetInterceptPk(0.0);
      return;
   }

   double rangeRate   = 0.0;
   double impactAngle = 0.0;
   CalcInterceptGeometry(aTargetPtr, rangeRate, impactAngle);

   double velocityPk = GetVelocityPk(rangeRate);
   double anglePk    = GetAnglePk(impactAngle);

   SetInterceptPk(velocityPk * anglePk);
}

double WsfExoAtmosphericLethality::GetVelocityPk(double aVelocityValue)
{
   // Simple linear interpolation of the data
   return InterpPkArray(aVelocityValue, mImpactVelocityPoints);
}

double WsfExoAtmosphericLethality::GetAnglePk(double aAngleValue)
{
   // Simple linear interpolation of the data
   return InterpPkArray(aAngleValue, mImpactAnglePoints);
}

// virtual
void WsfExoAtmosphericLethality::Terminate(double aSimTime)
{
   if (!mEnableDebris)
   {
      // No debris modeled.  The parent method alone is adequate for this task.
      WsfExplicitWeaponEffects::Terminate(aSimTime);
      return;
   }

   // We need to model a debris field.  Determine how to place the particles...
   // Distribute them in a 3-D field about this point, based upon velocity vectors at impact:
   double wcsLocation[3];
   GetEngagement()->GetWeaponPlatform()->GetLocationWCS(wcsLocation);
   UtEntity entity;
   entity.SetLocationWCS(wcsLocation);

   // Before calling the parent method, save off the velocity
   // vectors of all killed platforms.  (Assumedly there will
   // only be one killed platform for a hit-to-kill weapon like
   // this, but the architecture will allow for more than one.)
   // double preCollisionMomentum[] = { 0.0, 0.0, 0.0 };

   /*vector<PlatformData> deadPlatforms;
   if (mEnableDebris)
   {
      double tempV[3];
      double mass;
      for (unsigned int i = 0; i != KilledPlatformCount(); ++i)
      {
         WsfPlatform* deadPlatPtr = GetSimulation()->GetPlatformByIndex(KilledPlatform(i));
         if (deadPlatPtr != 0)
         {
            deadPlatPtr->GetVelocityWCS(tempV);
            mass = deadPlatPtr->GetMass();
            PlatformData data(mass, tempV);
            preCollisionMomentum[0] += mass * tempV[0];
            preCollisionMomentum[1] += mass * tempV[1];
            preCollisionMomentum[2] += mass * tempV[2];
            deadPlatforms.push_back(data);
         }
      }

      // Now add in the effect of the weapon platform itself:
      WsfPlatform* weapPlatPtr = GetEngagement()->GetWeaponPlatform();
      mass = weapPlatPtr->GetMass();
      weapPlatPtr->GetVelocityWCS(tempV);
      preCollisionMomentum[0] += mass * tempV[0];
      preCollisionMomentum[1] += mass * tempV[1];
      preCollisionMomentum[2] += mass * tempV[2];
   }

   // This is the method that actually removes the dead platforms.
   WsfExplicitWeaponEffects::Terminate(aSimTime);

   // Here is where we place a spreading debris field in place
   // of the platform(s) that were destroyed and have been removed.
   for (unsigned int j = 0; j != deadPlatforms.size(); ++j)
   {
      // TODO - Get the correct debris characteristics, corresponding
      // to the type of platform that was destroyed.
      DebrisType& debris = mDebrisTypes[0];

      double cumulativePlacedDebrisMass = 0.0;

      double tempVelWCS[3];
      UtVec3d::Set(tempVelWCS, deadPlatforms[j].mVelWCS);
      double platformMass = deadPlatforms[j].mMass;
      double massToSpray = platformMass * debris.mDetectableMassFraction;

      double tempVelNED[3];
      entity.ConvertWCSToNED(tempVelWCS, tempVelNED);
      double speed = UtVec3d::Magnitude(tempVelNED);

      double heading = atan2(tempVelNED[1], tempVelNED[0]);
      double pitch   = -asin(tempVelNED[2]/speed);
      double roll    = 0.0;
      entity.SetOrientationNED(heading, pitch, roll);

      while (cumulativePlacedDebrisMass < massToSpray)
      {
         // Model additon of "debris" platform ONLY if the fragment is above detectable particle size.
         const double cSigma = 0.25; // Places essentially all results within 0-1.0 range.
         double fragmentMass = mRandom.Rayleigh(cSigma) * massToSpray;

         if (fragmentMass > debris.mMinDetectableParticleMass)
         {
            // We passed the detectability criteria.  Place the "Debris" platform.

            // Now choose a particle velocity.
            // Assume a Rayleigh distribution for projectile dispersion,
            // spread uniformly about 360 degrees radially.
            double radialDispersion = mRandom.Rayleigh(debris.mProjectileDispersion);
            double clockAngleForDisp = mRandom.Uniform() * UtMath::cTWO_PI;
            double x = radialDispersion * cos(clockAngleForDisp);
            double y = radialDispersion * sin(clockAngleForDisp);

            // Now model tangential velocity variation.  Speed * normal distribution;
            double newSpeed = speed * mRandom.Normal(debris.mTangentialVelocitySigma);

            double wcsActualVel[] = {
               newSpeed * 0.0, 0.0, 0.0 };
            //   tempVel[1] * UtRandom::Normal(1.0, debris.c)}
            //cumulativePlacedDebrisMass += fragmentMass;

         }
      }
   }*/
}

// ================ SubClass DebrisType =================
/*WsfExoAtmosphericLethality::DebrisType::DebrisType()
  : mProjectileDispersion(0.3),
    mTangentialVelocitySigma(0.2),
    mDetectableMassFraction(0.5),
    //mMinParticleMass(0.001),
    //mMaxParticleMass(0.010),
    mCDA(2.0)
{}

void WsfExoAtmosphericLethality::DebrisType::ProcessBlock(UtInputBlock& aInputBlock)
{
   while (aInputBlock.ReadCommand())
   {
      if (! ProcessInput(aInputBlock.GetInput()))
      {
         throw UtInput::UnknownCommand(aInputBlock.GetInput());
      }
   }
}

bool WsfExoAtmosphericLethality::DebrisType::ProcessInput(UtInput& aInput)
{
   string command = aInput.GetCommand();
   if (command == "projectile_dispersion")
   {
      aInput.ReadValueOfType(mProjectileDispersion, UtInput::cANGLE);
      aInput.ValueGreater(mProjectileDispersion, 0.0);
      aInput.ValueLess(mProjectileDispersion, 1.0);
   }
   else if (command == "tangential_velocity_sigma")
   {
      aInput.ReadValue(mTangentialVelocitySigma);
      aInput.ValueGreater(mTangentialVelocitySigma, 0.0);
      aInput.ValueLess(mTangentialVelocitySigma, 1.0);
   }
   else if (command == "detectable_mass_fraction")
   {
      aInput.ReadValue(mDetectableMassFraction);
      aInput.ValueGreater(mDetectableMassFraction, 0.0);
      aInput.ValueLess(mDetectableMassFraction, 1.0);
   }
   //else if (command == "maximum_particle_mass")
   //{
   //   aInput.ReadValueOfType(mMaxParticleMass, UtInput::cMASS);
   //   aInput.ValueGreater(mMaxParticleMass, 0.0);
   //}
   //else if (command == "minimum_particle_mass")
   //{
   //   aInput.ReadValueOfType(mMinParticleMass, UtInput::cMASS);
   //   aInput.ValueGreater(mMinParticleMass, 0.0);
   //}
   else if (command == "flat_plate_area")
   {
      aInput.ReadValueOfType(mCDA, UtInput::cAREA);
      aInput.ValueGreater(mCDA, 0.0);
   }
   else
   {
      return false;
   }
   return true;
}*/
