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

#ifndef VEHICLE_LANDINGGEAR_HPP
#define VEHICLE_LANDINGGEAR_HPP

#include <QTextStream>
#include <string>

#include "UtVec3dX.hpp"

// VehicleLandingGear is used to generate AFSIM P6DOF data files.

namespace Designer
{
   class VehicleLandingGear
   {
      public:

         VehicleLandingGear();
         ~VehicleLandingGear();

         // This calculates the nominal height above the ground when on the landing gear
         // when empty (no fuel or weapons). It is helpful when placing an aircraft
         // on its gear at scenario start.
         double CalcNominalHeightAboveGroundOnGear_ft();

      protected:

         struct sGroundReactionPoint
         {
            std::string name;
            std::string controlSurfaceName;
            std::string brakingControlSurfaceName;
            std::string steeringControlSurfaceName;
            std::string nwsAngleControlSurfaceName;
            bool        isNoseGear;
            UtVec3dX    gearExtendedRelativePosition_ft;
            UtVec3dX    gearCompressionVector;
            UtVec3dX    gearRollingVector;
            double      springConstant_lbs_per_ft;
            double      damperConstant_lbs_per_fps;
            double      uncompressedLength_ft;
            double      maxCompression_ft;
            double      rollingCoefficientOfFriction;
            double      brakingCoefficientOfFriction;
            double      scuffingCoefficientOfFriction;
            double      ignoreFriction;
            double      isLandingGear;
         };

         std::vector<sGroundReactionPoint*> mLandingGearList;
         std::string                        mNWS_EnableControlName;
         double                             mNominalHeightAboveGroundOnGear_ft = 1.0;  // Default is 1 ft

         void OutputSingleLandingGear(QTextStream& aStream, sGroundReactionPoint* aGear);
   };
}
#endif // !VEHICLE_LANDINGGEAR_HPP
