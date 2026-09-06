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

#include "VehicleLandingGear.hpp"

#include <exception>
#include <QDir>
#include <QFile>
#include <QString>
#include <QTextStream>

#include "P6DofUtils.hpp"
#include "UtInput.hpp"
#include "UtInputFile.hpp"
#include "UtMath.hpp"

namespace Designer
{

VehicleLandingGear::VehicleLandingGear()
{
}

VehicleLandingGear::~VehicleLandingGear()
{

}

void VehicleLandingGear::OutputSingleLandingGear(QTextStream& aStream, sGroundReactionPoint* aGear)
{
   aStream << "         ground_reaction_point    " << QString(aGear->name.c_str()) << endl;

   if (!aGear->controlSurfaceName.empty())
   {
      aStream << "            control_surface_name                 " << QString(aGear->controlSurfaceName.c_str()) << endl;
   }

   if (!aGear->brakingControlSurfaceName.empty())
   {
      aStream << "            braking_control_surface_name         " << QString(aGear->brakingControlSurfaceName.c_str()) << endl;
   }

   if (!aGear->steeringControlSurfaceName.empty())
   {
      aStream << "            steering_control_surface_name        " << QString(aGear->steeringControlSurfaceName.c_str()) << endl;
   }

   if (!aGear->nwsAngleControlSurfaceName.empty())
   {
      aStream << "            nws_angle_control_surface_name       " << QString(aGear->nwsAngleControlSurfaceName.c_str()) << endl;
   }

   if (aGear->isNoseGear)
   {
      aStream << "            is_nose_gear                         true" << endl;
   }
   else
   {
      aStream << "            is_nose_gear                         false" << endl;
   }

   aStream << "            gear_extended_relative_position_x    " << aGear->gearExtendedRelativePosition_ft.X() << " ft" << endl;
   aStream << "            gear_extended_relative_position_y    " << aGear->gearExtendedRelativePosition_ft.Y() << " ft" << endl;
   aStream << "            gear_extended_relative_position_z    " << aGear->gearExtendedRelativePosition_ft.Z() << " ft" << endl;
   aStream << "            gear_compression_vector_x            " << aGear->gearCompressionVector.X() << endl;
   aStream << "            gear_compression_vector_y            " << aGear->gearCompressionVector.Y() << endl;
   aStream << "            gear_compression_vector_z            " << aGear->gearCompressionVector.Z() << endl;
   aStream << "            gear_rolling_vector_x                " << aGear->gearRollingVector.X() << endl;
   aStream << "            gear_rolling_vector_y                " << aGear->gearRollingVector.Y() << endl;
   aStream << "            gear_rolling_vector_z                " << aGear->gearRollingVector.Z() << endl;
   aStream << "            spring_constant_lbs_per_ft           " << aGear->springConstant_lbs_per_ft << endl;
   aStream << "            damper_constant_lbs_per_fps          " << aGear->damperConstant_lbs_per_fps  << endl;
   aStream << "            uncompressed_length                  " << aGear->uncompressedLength_ft << " ft" << endl;
   aStream << "            max_compression                      " << aGear->maxCompression_ft << " ft" << endl;
   aStream << "            rolling_coefficient_of_friction      " << aGear->rollingCoefficientOfFriction << endl;
   aStream << "            braking_coefficient_of_friction      " << aGear->brakingCoefficientOfFriction << endl;
   aStream << "            scuffing_coefficient_of_friction     " << aGear->scuffingCoefficientOfFriction << endl;

   if (aGear->ignoreFriction)
   {
      aStream << "            ignore_friction                      true" << endl;
   }
   else
   {
      aStream << "            ignore_friction                      false" << endl;
   }

   if (aGear->isLandingGear)
   {
      aStream << "            is_landing_gear                      true" << endl;
   }
   else
   {
      aStream << "            is_landing_gear                      false" << endl;
   }

   aStream << "         end_ground_reaction_point" << endl;
   aStream << "" << endl;
}

double VehicleLandingGear::CalcNominalHeightAboveGroundOnGear_ft()
{
   // TODO -- FUTURE_IMPROVEMENT_NOTE: Replace the default of zero
   return 0.0;
}

} // namespace Designer
