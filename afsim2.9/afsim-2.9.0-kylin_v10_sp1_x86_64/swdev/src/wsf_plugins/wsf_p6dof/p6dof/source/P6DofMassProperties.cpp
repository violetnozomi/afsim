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

#include "P6DofMassProperties.hpp"

#include <list>

#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

P6DofMassProperties::P6DofMassProperties()
   : mBaseMass_lbm(0)
   , mBaseIxx_slugft2(0)
   , mBaseIyy_slugft2(0)
   , mBaseIzz_slugft2(0)
   , mBaseCmPosRelToRef_ft(0.0, 0.0, 0.0)
   , mCurrentMass_lbm(0)
   , mCurrentIxx_slugft2(0)
   , mCurrentIyy_slugft2(0)
   , mCurrentIzz_slugft2(0)
   , mCurrentCmPosRelToRef_ft(0.0, 0.0, 0.0)
{
}

P6DofMassProperties& P6DofMassProperties::operator+=(const P6DofMassProperties& aSrc)
{
   double additionalMass_lbm = aSrc.GetMass_lbs();

   // Calculate the mass ratio between the current mass and the additional mass
   double newMass_lbs = mCurrentMass_lbm + additionalMass_lbm;

   if (newMass_lbs == 0)
   {
      // Do nothing, just return
      return *this;
   }

   double massRatio = additionalMass_lbm / newMass_lbs;

   // Calculate the new CM location, based on mass ratio
   UtVec3dX additionalLoc_ft    = aSrc.GetCmPosRelToRef_ft();
   UtVec3dX vectorToLocation_ft = additionalLoc_ft - mCurrentCmPosRelToRef_ft;
   vectorToLocation_ft *= massRatio;
   UtVec3dX newCmPos_ft = mCurrentCmPosRelToRef_ft + vectorToLocation_ft;

   // Add the rotational inertia from the additional mass to the new inertia
   double newIxx_slugft2 = mCurrentIxx_slugft2 + aSrc.GetIxx_slugft2();
   double newIyy_slugft2 = mCurrentIyy_slugft2 + aSrc.GetIyy_slugft2();
   double newIzz_slugft2 = mCurrentIzz_slugft2 + aSrc.GetIzz_slugft2();

   // Calculate the vectors to the new CM
   UtVec3dX vectorFromOldCMtoNewCM_ft          = newCmPos_ft - mCurrentCmPosRelToRef_ft;
   UtVec3dX vectorFromAdditionalMasstoNewCM_ft = newCmPos_ft - additionalLoc_ft;

   // Use parallel axis theorem to adjust the current mass to the new CM
   newIxx_slugft2 += (vectorFromOldCMtoNewCM_ft.X() * vectorFromOldCMtoNewCM_ft.X()) * mCurrentMass_lbm;
   newIyy_slugft2 += (vectorFromOldCMtoNewCM_ft.Y() * vectorFromOldCMtoNewCM_ft.Y()) * mCurrentMass_lbm;
   newIzz_slugft2 += (vectorFromOldCMtoNewCM_ft.Z() * vectorFromOldCMtoNewCM_ft.Z()) * mCurrentMass_lbm;

   // Use parallel axis theorem to adjust the additional mass to the new CM
   newIxx_slugft2 += (vectorFromAdditionalMasstoNewCM_ft.X() * vectorFromAdditionalMasstoNewCM_ft.X()) * additionalMass_lbm;
   newIyy_slugft2 += (vectorFromAdditionalMasstoNewCM_ft.Y() * vectorFromAdditionalMasstoNewCM_ft.Y()) * additionalMass_lbm;
   newIzz_slugft2 += (vectorFromAdditionalMasstoNewCM_ft.Z() * vectorFromAdditionalMasstoNewCM_ft.Z()) * additionalMass_lbm;

   // Update the new mass
   mCurrentMass_lbm = newMass_lbs;

   // Update the new rotational inertia
   mCurrentIxx_slugft2 = newIxx_slugft2;
   mCurrentIyy_slugft2 = newIyy_slugft2;
   mCurrentIzz_slugft2 = newIzz_slugft2;

   // Update the new CM location
   mCurrentCmPosRelToRef_ft = newCmPos_ft;

   return *this;
}

double P6DofMassProperties::GetMass_lbs() const
{
   return mCurrentMass_lbm;
}

double P6DofMassProperties::GetIxx_slugft2() const
{
   return mCurrentIxx_slugft2;
}

double P6DofMassProperties::GetIyy_slugft2() const
{
   return mCurrentIyy_slugft2;
}

double P6DofMassProperties::GetIzz_slugft2() const
{
   return mCurrentIzz_slugft2;
}

UtVec3dX P6DofMassProperties::GetCmPosRelToRef_ft() const
{
   return mCurrentCmPosRelToRef_ft;
}

double P6DofMassProperties::GetBaseMass_lbs() const
{
   return mBaseMass_lbm;
}

double P6DofMassProperties::GetBaseIxx_slugft2() const
{
   return mBaseIxx_slugft2;
}

double P6DofMassProperties::GetBaseIyy_slugft2() const
{
   return mBaseIyy_slugft2;
}

double P6DofMassProperties::GetBaseIzz_slugft2() const
{
   return mBaseIzz_slugft2;
}

UtVec3dX P6DofMassProperties::GetBaseCmPosRelToRef_ft() const
{
   return mBaseCmPosRelToRef_ft;
}

void P6DofMassProperties::ClearData()
{
   mBaseMass_lbm    = 0;
   mBaseIxx_slugft2 = 0;
   mBaseIyy_slugft2 = 0;
   mBaseIzz_slugft2 = 0;
   mBaseCmPosRelToRef_ft.Set(0.0, 0.0, 0.0);

   SetCurrentDataToBaseData();
}

void P6DofMassProperties::SetCurrentDataToBaseData()
{
   mCurrentMass_lbm         = mBaseMass_lbm;
   mCurrentIxx_slugft2      = mBaseIxx_slugft2;
   mCurrentIyy_slugft2      = mBaseIyy_slugft2;
   mCurrentIzz_slugft2      = mBaseIzz_slugft2;
   mCurrentCmPosRelToRef_ft = mBaseCmPosRelToRef_ft;
}

bool P6DofMassProperties::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "mass_properties")
   {
      UtInputBlock block(aInput, "end_mass_properties");
      while (block.ReadCommand())
      {
         if (command == "mass")
         {
            double mass = 0.0;
            aInput.ReadValueOfType(mass, UtInput::cMASS);
            mBaseMass_lbm = mass * UtMath::cLB_PER_KG;
         }
         else if (command == "moment_of_inertia_ixx")
         {
            double inertia;
            aInput.ReadValueOfType(inertia, UtInput::cANGULAR_INERTIA);
            mBaseIxx_slugft2 = (inertia * (UtMath::cFT_PER_M * UtMath::cFT_PER_M)) / UtMath::cKG_PER_SLUG;
         }
         else if (command == "moment_of_inertia_iyy")
         {
            double inertia;
            aInput.ReadValueOfType(inertia, UtInput::cANGULAR_INERTIA);
            mBaseIyy_slugft2 = (inertia * (UtMath::cFT_PER_M * UtMath::cFT_PER_M)) / UtMath::cKG_PER_SLUG;
         }
         else if (command == "moment_of_inertia_izz")
         {
            double inertia;
            aInput.ReadValueOfType(inertia, UtInput::cANGULAR_INERTIA);
            mBaseIzz_slugft2 = (inertia * (UtMath::cFT_PER_M * UtMath::cFT_PER_M)) / UtMath::cKG_PER_SLUG;
         }
         else if (command == "center_of_mass_x")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);
            double value_ft = value * UtMath::cFT_PER_M;
            double x, y, z;
            mBaseCmPosRelToRef_ft.Get(x, y, z);
            x = value_ft;
            mBaseCmPosRelToRef_ft.Set(x, y, z);
         }
         else if (command == "center_of_mass_y")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);
            double value_ft = value * UtMath::cFT_PER_M;
            double x, y, z;
            mBaseCmPosRelToRef_ft.Get(x, y, z);
            y = value_ft;
            mBaseCmPosRelToRef_ft.Set(x, y, z);
         }
         else if (command == "center_of_mass_z")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);
            double value_ft = value * UtMath::cFT_PER_M;
            double x, y, z;
            mBaseCmPosRelToRef_ft.Get(x, y, z);
            z = value_ft;
            mBaseCmPosRelToRef_ft.Set(x, y, z);
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofMassProperties::ProcessInput().";
            out.AddNote() << "Command: " << command;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // At this point, the mass properties data has been read, so we now set
      // the "current" data to the recently read-in "base" data
      SetCurrentDataToBaseData();
   }

   return myCommand;
}

void P6DofMassProperties::SetBaseMassProperties(double          aBaseMass_lbm,
                                                double          aBaseIxx_slugft2,
                                                double          aBaseIyy_slugft2,
                                                double          aBaseIzz_slugft2,
                                                const UtVec3dX& aBaseCmPosRelToRef_ft)
{
   mBaseMass_lbm         = aBaseMass_lbm;
   mBaseIxx_slugft2      = aBaseIxx_slugft2;
   mBaseIyy_slugft2      = aBaseIyy_slugft2;
   mBaseIzz_slugft2      = aBaseIzz_slugft2;
   mBaseCmPosRelToRef_ft = aBaseCmPosRelToRef_ft;

   mCurrentMass_lbm         = aBaseMass_lbm;
   mCurrentIxx_slugft2      = aBaseIxx_slugft2;
   mCurrentIyy_slugft2      = aBaseIyy_slugft2;
   mCurrentIzz_slugft2      = aBaseIzz_slugft2;
   mCurrentCmPosRelToRef_ft = aBaseCmPosRelToRef_ft;
}

void P6DofMassProperties::AddMassAtLocation(double          aMass_lbm,
                                            double          aIxx_slugft2,
                                            double          aIyy_slugft2,
                                            double          aIzz_slugft2,
                                            const UtVec3dX& aLocation_ft)
{
   // Calculate the mass ratio between the current mass and the additional mass
   double newMass_lbs = mCurrentMass_lbm + aMass_lbm;

   if (newMass_lbs == 0)
   {
      // Do nothing, just return
      return;
   }

   double massRatio = aMass_lbm / newMass_lbs;

   // Calculate the new CM location, based on mass ratio
   UtVec3dX vectorToLocation_ft = aLocation_ft - mCurrentCmPosRelToRef_ft;
   vectorToLocation_ft *= massRatio;
   UtVec3dX newCmPos_ft = mCurrentCmPosRelToRef_ft + vectorToLocation_ft;

   // Add the rotational inertia from the additional mass to the new inertia
   double newIxx_slugft2 = mCurrentIxx_slugft2 + aIxx_slugft2;
   double newIyy_slugft2 = mCurrentIyy_slugft2 + aIyy_slugft2;
   double newIzz_slugft2 = mCurrentIzz_slugft2 + aIzz_slugft2;

   // Calculate the vectors to the new CM
   UtVec3dX vectorFromOldCMtoNewCM_ft          = newCmPos_ft - mCurrentCmPosRelToRef_ft;
   UtVec3dX vectorFromAdditionalMasstoNewCM_ft = newCmPos_ft - aLocation_ft;

   // Use parallel axis theorem to adjust the current mass to the new CM
   newIxx_slugft2 += (vectorFromOldCMtoNewCM_ft.X() * vectorFromOldCMtoNewCM_ft.X()) * mCurrentMass_lbm;
   newIyy_slugft2 += (vectorFromOldCMtoNewCM_ft.Y() * vectorFromOldCMtoNewCM_ft.Y()) * mCurrentMass_lbm;
   newIzz_slugft2 += (vectorFromOldCMtoNewCM_ft.Z() * vectorFromOldCMtoNewCM_ft.Z()) * mCurrentMass_lbm;

   // Use parallel axis theorem to adjust the additional mass to the new CM
   newIxx_slugft2 += (vectorFromAdditionalMasstoNewCM_ft.X() * vectorFromAdditionalMasstoNewCM_ft.X()) * aMass_lbm;
   newIyy_slugft2 += (vectorFromAdditionalMasstoNewCM_ft.Y() * vectorFromAdditionalMasstoNewCM_ft.Y()) * aMass_lbm;
   newIzz_slugft2 += (vectorFromAdditionalMasstoNewCM_ft.Z() * vectorFromAdditionalMasstoNewCM_ft.Z()) * aMass_lbm;

   // Update the new mass
   mCurrentMass_lbm = newMass_lbs;

   // Update the new rotational inertia
   mCurrentIxx_slugft2 = newIxx_slugft2;
   mCurrentIyy_slugft2 = newIyy_slugft2;
   mCurrentIzz_slugft2 = newIzz_slugft2;

   // Update the new CM location
   mCurrentCmPosRelToRef_ft = newCmPos_ft;
}

void P6DofMassProperties::AddMassAtLocation(double aMass_lbm, const UtVec3dX& aLocation_ft)
{
   // This is a "point mass" so there is no "base" rotational inertia
   AddMassAtLocation(aMass_lbm, 0.0, 0.0, 0.0, aLocation_ft);
}

void P6DofMassProperties::MoveToLocation(const UtVec3dX& aLocation_ft)
{
   // Calculate the new CM location, based on mass ratio
   // UtVec3dX vectorToLocation_ft = aLocation_ft - mCurrentCmPosRelToRef_ft;
   // UtVec3dX newCmPos_ft = mCurrentCmPosRelToRef_ft + vectorToLocation_ft;

   // The rotational inertia begins with the current value
   double newIxx_slugft2 = mCurrentIxx_slugft2;
   double newIyy_slugft2 = mCurrentIyy_slugft2;
   double newIzz_slugft2 = mCurrentIzz_slugft2;

   // Calculate the vector to the new CM
   UtVec3dX vectorFromOldCMtoNewCM_ft = aLocation_ft - mCurrentCmPosRelToRef_ft;

   // Use parallel axis theorem to adjust the current mass to the new CM
   newIxx_slugft2 += (vectorFromOldCMtoNewCM_ft.X() * vectorFromOldCMtoNewCM_ft.X()) * mCurrentMass_lbm;
   newIyy_slugft2 += (vectorFromOldCMtoNewCM_ft.Y() * vectorFromOldCMtoNewCM_ft.Y()) * mCurrentMass_lbm;
   newIzz_slugft2 += (vectorFromOldCMtoNewCM_ft.Z() * vectorFromOldCMtoNewCM_ft.Z()) * mCurrentMass_lbm;

   // Update the new rotational inertia
   mCurrentIxx_slugft2 = newIxx_slugft2;
   mCurrentIyy_slugft2 = newIyy_slugft2;
   mCurrentIzz_slugft2 = newIzz_slugft2;

   // Update the new CM location
   // mCurrentCmPosRelToRef_ft = newCmPos_ft;
   mCurrentCmPosRelToRef_ft = aLocation_ft;
}
