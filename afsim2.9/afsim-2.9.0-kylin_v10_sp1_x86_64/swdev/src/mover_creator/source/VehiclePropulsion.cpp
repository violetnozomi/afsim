// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VehiclePropulsion.hpp"

#include <exception>
#include <QDir>
#include <QFile>
#include <QString>
#include <QTextStream>

#include "AeroEngine.hpp"
#include "P6DofUtils.hpp"
#include "UtInput.hpp"
#include "UtInputFile.hpp"
#include "UtMath.hpp"

namespace Designer
{

VehiclePropulsion::VehiclePropulsion()
   : mAfterBurnerEquipped(false)
   , mThrustReverserEquipped(false)
   , mThrustVectoringYawEquipped(false)
   , mThrustVectoringPitchEquipped(false)
{
}

VehiclePropulsion::~VehiclePropulsion()
{
   // for (auto& engine : mEngineList)
   // {
   //    delete engine;
   // }
   // mEngineList.clear();

   for (auto& fuelTank : mFuelTankList)
   {
      delete fuelTank;
   }
   mFuelTankList.clear();
}

size_t VehiclePropulsion::AddEngine(AeroEngine* aEngine)
{
   ++mMaxMapIndex;
   mEngineMap[mMaxMapIndex] = aEngine;
   return mMaxMapIndex;
}

void VehiclePropulsion::RemoveEngine(AeroEngine* aEngine)
{
   for (auto iter = mEngineMap.begin(); iter != mEngineMap.end(); ++iter)
   {
      AeroEngine* obj = (*iter).second;
      if (obj != nullptr)
      {
         if (obj == aEngine)
         {
            // Free up memory
            delete obj;
            // Remove this object from the map
            mEngineMap.erase(iter);
            return;
         }
      }
   }
}

void VehiclePropulsion::ClearAllEngines()
{
   for (auto iter = mEngineMap.begin(); iter != mEngineMap.end(); ++iter)
   {
      AeroEngine* obj = (*iter).second;
      if (obj != nullptr)
      {
         // Free up memory
         delete obj;
      }
   }

   mEngineMap.clear();
}

void VehiclePropulsion::ResetVehiclePropulsion()
{
   mEngineMap.clear();
}

int VehiclePropulsion::GetNumEngines()
{
   return (int)mEngineMap.size();
}

bool VehiclePropulsion::NameIsUnique(const std::string& aName)
{
   for (auto& object : mEngineMap)
   {
      AeroEngine* engine = object.second;
      if (aName == engine->GetName())
      {
         // Match found -- name is not unique
         return false;
      }
   }

   return true;
}

} // namespace Designer
