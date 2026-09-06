// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_MassProperties.hpp"

#include <list>

#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

void wsf::six_dof::MassProperties::AddInertiaFromParallelAxis(double          aAdditionalMass_lbm,
                                                              const UtVec3dX& aPositionDelta_ft,
                                                              double&         aIxx_slugft2,
                                                              double&         aIyy_slugft2,
                                                              double&         aIzz_slugft2)
{
   aIxx_slugft2 += (aPositionDelta_ft.Y() * aPositionDelta_ft.Y() + aPositionDelta_ft.Z() * aPositionDelta_ft.Z()) *
                   aAdditionalMass_lbm;
   aIyy_slugft2 += (aPositionDelta_ft.X() * aPositionDelta_ft.X() + aPositionDelta_ft.Z() * aPositionDelta_ft.Z()) *
                   aAdditionalMass_lbm;
   aIzz_slugft2 += (aPositionDelta_ft.X() * aPositionDelta_ft.X() + aPositionDelta_ft.Y() * aPositionDelta_ft.Y()) *
                   aAdditionalMass_lbm;
}

wsf::six_dof::MassProperties& wsf::six_dof::MassProperties::operator+=(const MassProperties& aSrc)
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

   // Add the rotational inertia from the additional mass to the new inertia
   double newIxx_slugft2 = mCurrentIxx_slugft2 + aSrc.GetIxx_slugft2();
   double newIyy_slugft2 = mCurrentIyy_slugft2 + aSrc.GetIyy_slugft2();
   double newIzz_slugft2 = mCurrentIzz_slugft2 + aSrc.GetIzz_slugft2();

   // Calculate the new CM location, based on mass ratio
   UtVec3dX additionalLoc_ft    = aSrc.GetCmPosRelToRef_ft();
   UtVec3dX vectorToLocation_ft = additionalLoc_ft - mCurrentCmPosRelToRef_ft;
   vectorToLocation_ft *= massRatio;
   UtVec3dX newCmPos_ft = mCurrentCmPosRelToRef_ft + vectorToLocation_ft;

   // Calculate the vectors to the new CM
   UtVec3dX vectorFromOldCMtoNewCM_ft          = newCmPos_ft - mCurrentCmPosRelToRef_ft;
   UtVec3dX vectorFromAdditionalMasstoNewCM_ft = newCmPos_ft - additionalLoc_ft;

   // Use parallel axis theorem to adjust the current mass to the new CM
   AddInertiaFromParallelAxis(mCurrentMass_lbm, vectorFromOldCMtoNewCM_ft, newIxx_slugft2, newIyy_slugft2, newIzz_slugft2);

   // Use parallel axis theorem to adjust the additional mass to the new CM
   AddInertiaFromParallelAxis(additionalMass_lbm,
                              vectorFromAdditionalMasstoNewCM_ft,
                              newIxx_slugft2,
                              newIyy_slugft2,
                              newIzz_slugft2);

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

double wsf::six_dof::MassProperties::GetMass_lbs() const
{
   return mCurrentMass_lbm;
}

double wsf::six_dof::MassProperties::GetIxx_slugft2() const
{
   return mCurrentIxx_slugft2;
}

double wsf::six_dof::MassProperties::GetIyy_slugft2() const
{
   return mCurrentIyy_slugft2;
}

double wsf::six_dof::MassProperties::GetIzz_slugft2() const
{
   return mCurrentIzz_slugft2;
}

double wsf::six_dof::MassProperties::GetBaseIxx_slugft2() const
{
   return mBaseIxx_slugft2;
}

double wsf::six_dof::MassProperties::GetBaseIyy_slugft2() const
{
   return mBaseIyy_slugft2;
}

double wsf::six_dof::MassProperties::GetBaseIzz_slugft2() const
{
   return mBaseIzz_slugft2;
}

UtVec3dX wsf::six_dof::MassProperties::GetCmPosRelToRef_ft() const
{
   return mCurrentCmPosRelToRef_ft;
}

double wsf::six_dof::MassProperties::GetBaseMass_lbs() const
{
   return mBaseMass_lbm;
}

UtVec3dX wsf::six_dof::MassProperties::GetBaseCmPosRelToRef_ft() const
{
   return mBaseCmPosRelToRef_ft;
}

void wsf::six_dof::MassProperties::ClearData()
{
   mBaseMass_lbm    = 0;
   mBaseIxx_slugft2 = 0;
   mBaseIyy_slugft2 = 0;
   mBaseIzz_slugft2 = 0;
   mBaseCmPosRelToRef_ft.Set(0.0, 0.0, 0.0);

   SetCurrentDataToBaseData();
}

void wsf::six_dof::MassProperties::SetCurrentDataToBaseData()
{
   mCurrentMass_lbm         = mBaseMass_lbm;
   mCurrentIxx_slugft2      = mBaseIxx_slugft2;
   mCurrentIyy_slugft2      = mBaseIyy_slugft2;
   mCurrentIzz_slugft2      = mBaseIzz_slugft2;
   mCurrentCmPosRelToRef_ft = mBaseCmPosRelToRef_ft;
}

bool wsf::six_dof::MassProperties::ProcessInput(UtInput& aInput)
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
         else
         {
            auto out = ut::log::error() << "Unrecognized command within MassProperties::ProcessInput().";
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

void wsf::six_dof::MassProperties::SetBaseMassProperties(double          aBaseMass_lbm,
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

void wsf::six_dof::MassProperties::SetBaseMassProperties(double aBaseMass_lbm, const UtVec3dX& aBaseCmPosRelToRef_ft)
{
   mBaseMass_lbm         = aBaseMass_lbm;
   mBaseCmPosRelToRef_ft = aBaseCmPosRelToRef_ft;

   mCurrentMass_lbm         = aBaseMass_lbm;
   mCurrentCmPosRelToRef_ft = aBaseCmPosRelToRef_ft;
}

void wsf::six_dof::MassProperties::AddMassAtLocation(double          aMass_lbm,
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
   AddInertiaFromParallelAxis(mCurrentMass_lbm, vectorFromOldCMtoNewCM_ft, newIxx_slugft2, newIyy_slugft2, newIzz_slugft2);

   // Use parallel axis theorem to adjust the additional mass to the new CM
   AddInertiaFromParallelAxis(aMass_lbm, vectorFromAdditionalMasstoNewCM_ft, newIxx_slugft2, newIyy_slugft2, newIzz_slugft2);

   // Update the new mass
   mCurrentMass_lbm = newMass_lbs;

   // Update the new rotational inertia
   mCurrentIxx_slugft2 = newIxx_slugft2;
   mCurrentIyy_slugft2 = newIyy_slugft2;
   mCurrentIzz_slugft2 = newIzz_slugft2;

   // Update the new CM location
   mCurrentCmPosRelToRef_ft = newCmPos_ft;
}

void wsf::six_dof::MassProperties::AddMassAtLocation(double aMass_lbm, const UtVec3dX& aLocation_ft)
{
   AddMassAtLocation(aMass_lbm, 0.0, 0.0, 0.0, aLocation_ft);
}

void wsf::six_dof::MassProperties::MoveToLocation(const UtVec3dX& aLocation_ft)
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
   AddInertiaFromParallelAxis(mCurrentMass_lbm, vectorFromOldCMtoNewCM_ft, newIxx_slugft2, newIyy_slugft2, newIzz_slugft2);

   // Update the new rotational inertia
   mCurrentIxx_slugft2 = newIxx_slugft2;
   mCurrentIyy_slugft2 = newIyy_slugft2;
   mCurrentIzz_slugft2 = newIzz_slugft2;

   // Update the new CM location
   // mCurrentCmPosRelToRef_ft = newCmPos_ft;
   mCurrentCmPosRelToRef_ft = aLocation_ft;
}
