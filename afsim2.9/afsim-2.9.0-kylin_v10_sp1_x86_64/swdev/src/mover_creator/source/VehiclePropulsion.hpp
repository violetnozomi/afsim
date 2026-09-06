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

#ifndef VEHICLE_PROPULSION_HPP
#define VEHICLE_PROPULSION_HPP

#include <QTextStream>
#include <string>

#include "GeometryPropulsionData.hpp"
#include "UtVec3dX.hpp"

// VehiclePropulsion is used to generate AFSIM P6DOF data files.

namespace Designer
{
   class AeroEngine;

   class VehiclePropulsion
   {
      public:

         VehiclePropulsion();
         ~VehiclePropulsion();

         // Add a new engine instance
         size_t AddEngine(AeroEngine* aEngine);

         // Removes the specified engine
         void RemoveEngine(AeroEngine* aEngine);

         // Clear all engines
         void ClearAllEngines();

         int GetNumEngines();

         const std::map<size_t, AeroEngine*>& GetEngineMap() { return mEngineMap; }

         void ResetVehiclePropulsion();

         // Returns true if no existing engine has the specified name
         bool NameIsUnique(const std::string& aName);

      protected:
         std::map<size_t, AeroEngine*> mEngineMap;

         struct sFuelTankData
         {
            std::string name;
            UtVec3dX cgFullRefPos_ft;
            UtVec3dX cgEmptyRefPos_ft;
            double maxQuantity_lbs;
            double currentQuantity_lbs;
            double maxFlowRate_pps;
            double maxFillRate_pps;
            double maxXferRate_pps;
         };
         std::vector<sFuelTankData*> mFuelTankList;

         size_t  mMaxMapIndex = 0;
         bool    mAfterBurnerEquipped;
         bool    mThrustReverserEquipped;
         bool    mThrustVectoringYawEquipped;
         bool    mThrustVectoringPitchEquipped;
   };
}
#endif // !VEHICLE_PROPULSION_HPP
