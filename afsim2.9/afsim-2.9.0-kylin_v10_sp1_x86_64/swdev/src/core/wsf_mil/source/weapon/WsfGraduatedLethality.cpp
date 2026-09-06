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

#include "WsfGraduatedLethality.hpp"

#include <algorithm>
#include <iostream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"
#include "WsfWeaponEngagement.hpp"

//! Default constructor
WsfGraduatedLethality::WsfGraduatedLethality(WsfScenario& aScenario)
   : WsfExplicitWeaponEffects(aScenario)
   , m2DOffsets(false)
   , mPkInterp(false)
   , mDefIsNull(true)
   , mTables()
   , mTableIndex(0)
{
   WsfStringId DEFAULT = "WSF_PLATFORM";

   // Set and use ONLY the intercept lethality for this class:
   SetUseInterceptPk();

   // Set a default lethality of Zero, unless set otherwise.
   Table table(DEFAULT);
   table.AddPoint(Point(0.0, 0.0));
   mTables.push_back(table);
}

// virtual
WsfWeaponEffects* WsfGraduatedLethality::Clone() const
{
   return new WsfGraduatedLethality(*this);
}

// virtual
bool WsfGraduatedLethality::PreInitialize()
{
   bool success = WsfExplicitWeaponEffects::PreInitialize();
   if (success)
   {
      double maxRadiusSquared = 0.0;

      // Sort all lethality tables in decreasing radiusSquared order.
      for (mTableIndex = 0; mTableIndex != mTables.size(); ++mTableIndex)
      {
         success &= mTables[mTableIndex].Sort(maxRadiusSquared);

         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "WsfGraduatedLethality: PreInitialize():";
            if (mTableIndex == 0)
            {
               out.AddNote() << "Pk Table for weapon effects:" << GetType();
            }
            const Table& table    = mTables[mTableIndex];
            double       greatest = 0.0;
            { // RAII block
               auto pkTable = out.AddNote() << "Pk Table [" << mTableIndex << "] for " << table.TypeId() << " is:";

               for (unsigned int j = 0; j != table.Size(); ++j)
               {
                  double radSqrd = table.Pt(j).RadSqrd();
                  double rad     = sqrt(radSqrd);
                  double Pk      = table.Pt(j).Pk();
                  pkTable.AddNote() << "Radius: " << rad;
                  pkTable.AddNote() << "Pk: " << Pk;
                  if (radSqrd > greatest)
                     greatest = radSqrd;
               }
            }

            auto sampled = out.AddNote() << "Sampled results returned from table [" << mTableIndex << "] "
                                         << table.TypeId() << " are:";
            for (int k = 0; k < 21; ++k)
            {
               double r2 = greatest * (k / 19.0);
               sampled.AddNote() << "Radius: " << sqrt(r2);
               sampled.AddNote() << "Pk: " << table.GetPk(r2, mPkInterp);
            }
         }
      }

      double maxRadius     = sqrt(maxRadiusSquared);
      double maxRadiusBase = MaximumEffectiveRadius();
      if (maxRadiusBase == 0.0)
      {
         if (m2DOffsets)
         {
            // A 3-D radius will always be equal or larger than a 2-D, because of
            // impact height mismatches... so expand by a factor to compensate.
            SetMaxiumEffectiveRadius(3.0 * maxRadius);
         }
         else
         {
            // Allow a little slop to allow for numerical issues.
            SetMaxiumEffectiveRadius(1.2 * maxRadius);
         }
      }
      else if (maxRadius > maxRadiusBase)
      {
         auto out =
            ut::log::warning() << "In weapon_effects - maximum radius from tables exceeds explicit maximum_radius.";
         out.AddNote() << "Type: " << GetType();
      }
   }
   return success;
}

// virtual
bool WsfGraduatedLethality::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if ((command == "use_3d_radius") || (command == "use_3d_offsets"))
   {
      m2DOffsets = false;
   }
   else if ((command == "use_2d_radius") || (command == "use_2d_offsets"))
   {
      m2DOffsets = true;
   }
   else if (command == "discrete")
   {
      mPkInterp = false;
   }
   else if (command == "interpolated")
   {
      mPkInterp = true;
   }
   else if (command == "target_type")
   {
      std::string typeName;
      aInput.ReadValue(typeName);
      WsfStringId typeId = typeName;
      if ((typeName == "WSF_PLATFORM") || (typeName == "default") || (typeName == "DEFAULT"))
      {
         mTableIndex = 0;
      }
      else
      {
         if (TableExistsForType(typeId))
         {
            throw UtInput::BadValue(aInput, "Duplicate target_type specified.");
         }
         mTables.emplace_back(typeId);
         mTableIndex = static_cast<unsigned int>(mTables.size()) - 1;
      }
      ProcessBlock(aInput);
   }
   else if (ReadPkAndRadius(aInput))
   {
   }
   else if ((command == "use_launch_pk") || (command == "use_intercept_pk") || (command == "launch_pk") ||
            (command == "intercept_pk") || (command == "use_pk_table"))
   {
      throw UtInput::BadValue(aInput, "Incompatible option: " + command);
   }
   else
   {
      myCommand = WsfExplicitWeaponEffects::ProcessInput(aInput);
   }

   return myCommand;
}

// protected virtual
void WsfGraduatedLethality::CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr)
{
   // Note:  Since this just passed a vulnerability radius test in the parent class,
   // just use the value here for calculation of Pk, if possible, to save time...

   double missDistanceSquared = LastTestRadiusSquared();
   if ((m2DOffsets) || (missDistanceSquared == 0.0))
   {
      // Get the declared or computed 3D miss distance.
      // (See WsfWeaponEngagement::GetMissDistance for more information).

      double missDistance = GetEngagement()->GetMissDistance(aTargetPtr);

      if (m2DOffsets)
      {
         // If 2D offsets are desired then the declared/computed 3D miss distance must be converted to a
         // 2D distance. Because we don't have the 3D components that went into the 3D miss distance, we'll
         // get the 2D and 3D distances between the current locations and use them to ratio the miss distance.

         WsfPlatform* weaponPtr = GetEngagement()->GetWeaponPlatform();
         if (weaponPtr != nullptr)
         {
            double weaponLocWCS[3];
            double targetLocWCS[3];
            GetEngagement()->GetWeaponLocationWCS(weaponPtr, weaponLocWCS);
            GetEngagement()->GetTargetLocationWCS(aTargetPtr, targetLocWCS);

            // Compute NED offset from computed weapon location.

            UtEntity weapon(*weaponPtr);
            weapon.SetLocationWCS(weaponLocWCS);
            double targetLocNED[3];
            weapon.ConvertWCSToNED(targetLocWCS, targetLocNED);
            double distNED = UtVec3d::Magnitude(targetLocNED);
            double distNE  = sqrt((targetLocNED[0] * targetLocNED[0]) + (targetLocNED[1] * targetLocNED[1]));
            if (distNED > 1.0E-4)
            {
               missDistance *= (distNE / distNED);
            }
         }
      }
      missDistanceSquared = missDistance * missDistance;
   }

   FindTable(aTargetPtr);
   SetInterceptPk(GetPk(missDistanceSquared));
}

//! This private procedure is used to prevent duplicating code in several places.
// private
void WsfGraduatedLethality::AddPoint(double aRadius, double aPk)
{
   if ((mDefIsNull) && (mTableIndex == 0))
   {
      // The very first table is the default one for all WSF_PLATFORM types,
      // and it is pre-loaded with one value for a Pk of zero, to be non-lethal.
      // If we are going to specify any value from the input stream, it must
      // over-write this single first point.  Any points added after that one
      // may be treated in the normal way.
      mTables[mTableIndex].SetPoint(0, aRadius, aPk);
      mDefIsNull = false;
   }
   else
   {
      mTables[mTableIndex].AddPoint(Point(aRadius, aPk));
   }
}

// private
void WsfGraduatedLethality::FindTable(WsfPlatform* aTargetPtr)
{
   mTableIndex = 0;
   // The default table is index zero, corresponding to
   // all WSF_PLATFORM types, so start at index one.
   for (unsigned int i = 1; i != mTables.size(); ++i)
   {
      if (aTargetPtr->IsA_TypeOf(mTables[i].TypeId()))
      {
         mTableIndex = i;
      }
   }
}

// private
double WsfGraduatedLethality::GetPk(double aMissDistSquared) const
{
   // Now for this miss distance, find out what the Pk should be:
   return mTables[mTableIndex].GetPk(aMissDistSquared, mPkInterp);
}

// private
void WsfGraduatedLethality::ProcessBlock(UtInput& aInput)
{
   std::string blockTerminator = "end_" + aInput.GetCommand();
   std::string command;
   while (aInput.ReadCommand(command))
   {
      // A table is terminated by either the block terminator, or a command that isn't
      // a pk/radius command. The original syntax required a terminator, but many similar
      // constructs simply end a table at the first command that doesn't define a table entry.
      // We let them do it both ways.

      if (command == blockTerminator)
      {
         break;
      }
      else if (!ReadPkAndRadius(aInput))
      {
         aInput.PushBack(aInput.GetCommand());
         break;
      }
   }
   mTableIndex = 0; // Subsequent unbounded entries use the 'default' entry.
}

//! This private method is used to prevent code duplication.
//! There are two different contexts in which reading a
//! Radius and a Pk value (in either order) is needed.
// private
bool WsfGraduatedLethality::ReadPkAndRadius(UtInput& aInput)
{
   std::string command(aInput.GetCommand());

   // "Pk" and "radius" values must be specified as a
   // matched (X,Y) point, but the order is not important.
   if (command == "radius_and_pk")
   {
      double Pk     = 0.0;
      double radius = 0.0;

      aInput.ReadValueOfType(radius, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(radius, 0.0);

      aInput.ReadValue(Pk);
      aInput.ValueInClosedRange(Pk, 0.0, 1.0);

      AddPoint(radius, Pk);
   }
   else if (command == "pk_and_radius")
   {
      double Pk     = 0.0;
      double radius = 0.0;

      aInput.ReadValue(Pk);
      aInput.ValueInClosedRange(Pk, 0.0, 1.0);

      aInput.ReadValueOfType(radius, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(radius, 0.0);

      AddPoint(radius, Pk);
   }
   else
   {
      return false;
   }
   return true;
}

//! Returns true if a table exists for the specified type.
// private
bool WsfGraduatedLethality::TableExistsForType(WsfStringId aTypeId) const
{
   bool found = false;
   for (unsigned int i = 0; i != mTables.size(); ++i)
   {
      if (aTypeId == mTables[i].TypeId())
      {
         found = true;
         break;
      }
   }
   return found;
}

// =================== TABLE CLASS METHODS BELOW ===============================

WsfGraduatedLethality::Table::Table()
   : mTypeId()
   , mCanInterpolate(false)
   , mValues()
{
}

WsfGraduatedLethality::Table::Table(WsfStringId aTypeId)
   : mTypeId(aTypeId)
   , mCanInterpolate(false)
   , mValues()
{
}

double WsfGraduatedLethality::Table::GetPk(double aMissDistSquared, bool aInterpolate) const
{
   // Now for this miss distance, find out what the Pk should be:
   double Pk = 0.0;

   // If aMissDistSquared is less than the minimum, set Pk to maximum...
   if (aMissDistSquared <= mValues[0].RadSqrd())
   {
      Pk = mValues[0].Pk();
   }
   else
   {
      // There are two possibilities... discrete or interpolated Pk:
      if (aInterpolate && mCanInterpolate)
      {
         bool done = false;

         // INTERPOLATED:  March through the vector, and find the
         // first Pk with a larger rSquared than missDistSquared

         double prevPk       = 0.0;
         double prevRSquared = 0.0;
         double rSquared     = 0.0;

         auto iter = mValues.begin();

         while (!done && iter != mValues.end())
         {
            if (iter->RadSqrd() < aMissDistSquared)
            {
               prevPk       = iter->Pk();
               prevRSquared = iter->RadSqrd();
            }
            else
            {
               Pk       = iter->Pk();
               rSquared = iter->RadSqrd();
               done     = true;
            }
            ++iter;
         }

         // Do not extrapolate past the ends of the input values,
         // so if still not done, set Pk to zero...
         if (!done)
         {
            Pk = 0.0;
         }
         else
         {
            if (aInterpolate)
            {
               // Standard linear interpolation...
               Pk = prevPk + (prevPk - Pk) * (prevRSquared - aMissDistSquared) / (rSquared - prevRSquared);
            }
         }
      }
      else
      {
         // DISCRETE:  March through the vector IN REVERSE ORDER, and find the
         // highest Pk which does not have an rSquared less than missDistSquared
         bool done = false;

         auto iter = mValues.end();

         while ((!done) && (iter != mValues.begin()))
         {
            --iter;
            if (iter == mValues.begin())
            {
               done = true;
            }
            else if (iter->RadSqrd() >= aMissDistSquared)
            {
               Pk = iter->Pk();
            }
            else
            {
               done = true;
            }
         }
      }
   }

   return Pk;
}

bool WsfGraduatedLethality::Table::Sort(double& aMaxRSquared)
{
   bool success = true;

   // Sort the Points increasing radiusSquared
   auto iterA = mValues.begin();
   auto iterB = mValues.end();

   if (iterA == iterB)
   {
      ut::log::error() << "No radius/Pk values were specified.";
      success = false;
   }
   else
   {
      sort(iterA, iterB);

      // 2. Verify that Pk decreases with increasing radiusSquared.
      iterA = mValues.begin();
      iterB = iterA + 1;

      if (iterB == mValues.end())
      {
         // (Only one value of radius and lethality was input.)
         mCanInterpolate = false;
      }
      else
      {
         mCanInterpolate = true;
         while (iterB != mValues.end())
         {
            if ((iterA->Pk() < iterB->Pk()) || (iterA->RadSqrd() >= iterB->RadSqrd()))
            {
               ut::log::error() << "Pk values do not decrease with increasing miss distance.";
               success = false;
            }

            ++iterA;
            ++iterB;
         }
      }

      // 3. Save off max radius to assure that the base class will not incorrectly limit our greater radius
      aMaxRSquared = std::max(aMaxRSquared, mValues.back().RadSqrd());
   }
   return success;
}
