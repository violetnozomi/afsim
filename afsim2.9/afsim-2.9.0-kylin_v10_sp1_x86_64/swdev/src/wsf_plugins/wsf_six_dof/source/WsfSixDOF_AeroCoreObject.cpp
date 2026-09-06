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

#include "WsfSixDOF_AeroCoreObject.hpp"

#include <cstdio>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"

bool wsf::six_dof::AeroCoreObject::Initialize()
{
   return true;
}

std::string wsf::six_dof::AeroCoreObject::GetModeName() const
{
   return mModeName;
}

void wsf::six_dof::AeroCoreObject::SetModeName(const std::string& aName)
{
   mModeName = aName;
}

double wsf::six_dof::AeroCoreObject::CL_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad)
{
   if (mCL_AlphaBetaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[3];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aBeta_rad;
   lookupArgs[2] = aAlpha_rad;
   return mCL_AlphaBetaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::AeroCoreObject::Cd_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad)
{
   if (mCd_AlphaBetaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[3];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aBeta_rad;
   lookupArgs[2] = aAlpha_rad;
   return mCd_AlphaBetaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::AeroCoreObject::CY_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad)
{
   if (mCY_AlphaBetaMachTablePtr == nullptr)
   {
      return 0.0;
   }

   double lookupArgs[3];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aBeta_rad;
   lookupArgs[2] = aAlpha_rad;
   return mCY_AlphaBetaMachTablePtr->Lookup(lookupArgs);
}

double wsf::six_dof::AeroCoreObject::CalculateAeroCLArea(double aMach, double aAlpha_rad)
{
   return mRefArea_sqft * CL_AlphaBetaMach(aMach, aAlpha_rad, 0.0);
}

double wsf::six_dof::AeroCoreObject::CalculateAeroCdArea(double aMach, double aAlpha_rad)
{
   return mRefArea_sqft * Cd_AlphaBetaMach(aMach, aAlpha_rad, 0.0);
}

double wsf::six_dof::AeroCoreObject::GetRefArea_sqft() const
{
   return mRefArea_sqft;
}
