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

#include "Vehicle.hpp"

#include <exception>
#include <QDir>
#include <QFile>
#include <QString>

#include "AeroBody.hpp"
#include "AeroDish.hpp"
#include "AeroEngine.hpp"
#include "AeroFullFlyingSurface.hpp"
#include "AeroHingedSurface.hpp"
#include "AeroJetEngine.hpp"
#include "AeroLandingGear.hpp"
#include "AeroLiquidRocketEngine.hpp"
#include "AeroNacelle.hpp"
#include "AeroRamjetEngine.hpp"
#include "AeroSolidRocketEngine.hpp"
#include "AeroSpeedBrake.hpp"
#include "AeroSurface.hpp"
#include "AeroTable2d.hpp"
#include "AeroTable3d.hpp"
#include "AeroTable4d.hpp"
#include "AeroWing.hpp"
#include "CalculateAeroDialog.hpp"
#include "GeometryBody.hpp"
#include "GeometryDish.hpp"
#include "GeometryFuselage.hpp"
#include "GeometryGLWidget.hpp"
#include "GeometryLandingGear.hpp"
#include "GeometryMassProperties.hpp"
#include "GeometryNacelle.hpp"
#include "GeometryPointMass.hpp"
#include "GeometryPropulsionData.hpp"
#include "GeometrySpeedBrake.hpp"
#include "GeometrySurface.hpp"
#include "GeometryWidget.hpp"
#include "GeometryWing.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "P6DofUtils.hpp"
#include "VehicleAero.hpp"
#include "VehiclePropulsion.hpp"
#include "UtInput.hpp"
#include "UtInputFile.hpp"
#include "UtMath.hpp"

// Unfortunately, we need this Windows hack here
#if defined(_WIN32)
# if defined (GetMessage)
#  undef GetMessage
# endif
#endif

namespace Designer
{

Vehicle::Vehicle()
   : mAero(nullptr)
   , mLandingGear(nullptr)
   , mAtmosphere(UtAtmosphere::CreateNewAtmosphereTable())
{
   // Determine the path to the aero data folder
   QString moverCreatorDataPath = MoverCreatorMainWindow::GetDataPath();
   std::string basePath = moverCreatorDataPath.toStdString();

   mAeroBasePath = basePath + "/Aero/";

   std::string atmFile = basePath + "/Atmosphere/atm_config.txt";

   // Create atmosphere
   auto input = UtInput();
   input.PushInput(ut::make_unique<UtInputFile>(atmFile));
   mAtmosphere.ProcessGlobalInput(input);

   // Set default vehicle data
   double mach                = 0.8;
   double altitude_ft         = 20000.0;
   double altitude_m = altitude_ft * UtMath::cM_PER_FT;

   // Calculate speed, in feet per second
   double speed_fps = mAtmosphere.CalcFpsFromMach(altitude_m, mach);

   // Calculate speed, in knots (knots true airspeed)
   double ktas = mAtmosphere.CalcKtasFromMach(altitude_m, mach);

   // Calculate knots calibrated airspeed
   double kcas = mAtmosphere.CalcKcasFromMach(altitude_m, mach);

   // Calculate dynamic pressure, in lbf per square foot
   double dynamicPressure_psf = mAtmosphere.CalcDynamicPressure_psf(altitude_m, speed_fps);

   // Calculate density, in slugs per cubic foot
   double density_kgm3 = mAtmosphere.Density(altitude_m);
   double density_slugs_ft3 = UtMath::cM_PER_FT * UtMath::cM_PER_FT * UtMath::cM_PER_FT / UtMath::cKG_PER_SLUG * density_kgm3;

   SetVehicleAlphaBeta(0.0, 0.0);
   SetVehicleYawPitchRollRates(0.0, 0.0, 0.0);
   SetVehicleSpeedParameters(speed_fps, mach, ktas, kcas, dynamicPressure_psf, density_slugs_ft3);
}

Vehicle::~Vehicle()
{
}

QString Vehicle::GetVehicleControlConfigurationString() const 
{
   switch (mControlConfig)
   {
      case Designer::Vehicle::cCONFIG_STABS_ONE_TAIL:
         return "cCONFIG_STABS_ONE_TAIL";
         break;
      case Designer::Vehicle::cCONFIG_STABS_TWO_TAILS:
         return "cCONFIG_STABS_TWO_TAILS";
         break;
      case Designer::Vehicle::cCONFIG_STABS_ONE_VENTRAL_TAIL:
         return "cCONFIG_STABS_ONE_VENTRAL_TAIL";
         break;
      case Designer::Vehicle::cCONFIG_STABS_TWO_VENTRAL_TAILS:
         return "cCONFIG_STABS_TWO_VENTRAL_TAILS";
         break;
      case Designer::Vehicle::cCONFIG_STABS_UPPER_LOWER_TAILS:
         return "cCONFIG_STABS_UPPER_LOWER_TAILS";
         break;
      case Designer::Vehicle::cCONFIG_VTAIL:
         return "cCONFIG_VTAIL";
         break;
      case Designer::Vehicle::cCONFIG_INVERTED_VTAIL:
         return "cCONFIG_INVERTED_VTAIL";
         break;
      case Designer::Vehicle::cCONFIG_CANARDS_ONE_TAIL:
         return "cCONFIG_CANARDS_ONE_TAIL";
         break;
      case Designer::Vehicle::cCONFIG_CANARDS_TWO_TAILS:
         return "cCONFIG_CANARDS_TWO_TAILS";
         break;
      case Designer::Vehicle::cCONFIG_CANARDS_STABS_ONE_TAIL:
         return "cCONFIG_CANARDS_STABS_ONE_TAIL";
         break;
      case Designer::Vehicle::cCONFIG_CANARDS_STABS_TWO_TAILS:
         return "cCONFIG_CANARDS_STABS_TWO_TAILS";
         break;
      case Designer::Vehicle::cCONFIG_TAIL_FINS:
         return "cCONFIG_TAIL_FINS";
         break;
      case Designer::Vehicle::cCONFIG_MID_FINS:
         return "cCONFIG_MID_FINS";
         break;
      case Designer::Vehicle::cCONFIG_CANARD_FINS:
         return "cCONFIG_CANARD_FINS";
         break;
      case Designer::Vehicle::cCONFIG_FLYING_WING:
         return "cCONFIG_FLYING_WING";
         break;
      case Designer::Vehicle::cCONFIG_STABS_FLYING_WING:
         return "cCONFIG_STABS_FLYING_WING";
         break;
      case Designer::Vehicle::cNO_CONTROL:
         return "cNO_CONTROL";
         break;
      default:
         break;
   }
   return "";
}

bool Vehicle::IsAircraft()
{
   if (mVehicleTypeString == "Aircraft") { return true; }

   return false;
}

bool Vehicle::IsWeapon()
{
   if (mVehicleTypeString == "Weapon") { return true; }

   return false;
}

void Vehicle::SetVehicleAltitude(double aVehicleAltitude_ft)
{
   mVehicleAltitude_ft = aVehicleAltitude_ft;
   if (mAero != nullptr)
   {
      mAero->SetVehicleAltitude_ft(aVehicleAltitude_ft);
   }
}

void Vehicle::SetTestingMach(double aMach)
{
   mVehicleTestMach = aMach;
   if (mAero != nullptr)
   {
      mAero->SetTestingMach(aMach);
   }
}

double Vehicle::GetTestingMach()
{
   return mVehicleTestMach;
}

void Vehicle::SetAeroDataMaxMach(double aMach)
{
   mMaxTableMach = aMach;
   if (mAero != nullptr)
   {
      mAero->SetAeroDataMaxMach(aMach);
   }
}

void Vehicle::SetAeroDataMaxBeta(double aBeta_deg)
{
   mMaxTableBetaDataPts_deg = aBeta_deg;
   if (mAero != nullptr)
   {
      mAero->SetAeroDataMaxBeta(aBeta_deg);
   }
}

void Vehicle::SetAeroDataAlphas(double aAlphaMin_deg, double aAlphaMax_deg)
{
   mMinTableAlphaDataPts_deg = aAlphaMin_deg;
   mMaxTableAlphaDataPts_deg = aAlphaMax_deg;
   if (mAero != nullptr)
   {
      mAero->SetAeroDataAlphas(aAlphaMin_deg, aAlphaMax_deg);
   }
}

void Vehicle::SetAeroAlphaBetaSymmetry(bool aAlphaSymmetry, bool aBetaSymmetry)
{
   mAlphaSymmetry = aAlphaSymmetry;
   mBetaSymmetry = aBetaSymmetry;
   if (mAero != nullptr)
   {
      mAero->SetAeroAlphaBetaSymmetry(aAlphaSymmetry, aBetaSymmetry);
   }
}

void Vehicle::SetTestingDataAndAeroTableLimits()
{
   if (mAero != nullptr)
   {
      mAero->SetVehicleAltitude_ft(mVehicleAltitude_ft);
      mAero->SetTestingMach(mVehicleTestMach);
      mAero->SetAeroDataMaxMach(mMaxTableMach);
      mAero->SetAeroDataMaxBeta(mMaxTableBetaDataPts_deg);
      mAero->SetAeroDataAlphas(mMinTableAlphaDataPts_deg, mMaxTableAlphaDataPts_deg);
      mAero->SetAeroAlphaBetaSymmetry(mAlphaSymmetry, mBetaSymmetry);
   }
}

void Vehicle::GenerateAeroDataFromGeometry()
{
   // Clear any previous AeroObject data
   if (mAero != nullptr)
   {
      delete mAero;
      mAero = nullptr;
   }

   // Create the new VehicleAero
   mAero = new VehicleAero(this, mAtmosphere);

   // Clear any previous AeroEngine data, since we'll be adding it below
   mPropulsion.ClearAllEngines();

   // Generate the various AeroObject data based on the GeometryObjects
   const std::map<size_t, GeometryObject*>& geometryObjMap = mGeometry.GetGeometryObjectMap();
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;

      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryBody")
      {
         GeometryBody* object = dynamic_cast<GeometryBody*>(geometryObject);

         GenerateAeroBody(object);
      }
      else if (geometryType == "GeometryDish")
      {
         GeometryDish* object = dynamic_cast<GeometryDish*>(geometryObject);

         GenerateAeroDish(object);
      }
      else if (geometryType == "GeometryFuselage")
      {
         // There is no AeroFuselage, just AeroBody
         GeometryBody* object = dynamic_cast<GeometryBody*>(geometryObject);

         // Check if the fuselage has a canopy
         GeometryFuselage* fuselage = dynamic_cast<GeometryFuselage*>(geometryObject);
         bool   canopyPresent = fuselage->CanopyPresent();
         double canopyArea_ft2 = 0.0;

         if (canopyPresent)
         {
            double canopyHeight_ft = fuselage->CanopyHeight();
            double canopyWidth_ft = fuselage->CanopyWidth();
            double effectiveRadius_ft = 0.25 * (canopyHeight_ft + canopyWidth_ft);
            double canopyRefZ_ft = fuselage->CanopyRefZ();
            double fuselageHeight_ft = object->GetHeight_ft();

            // Calculate full circle frontal area of canopy, assuming an ellipse
            double fullArea_ft2 = (canopyHeight_ft * 0.5) * (canopyWidth_ft * 0.5) * UtMath::cPI;

            if ((canopyRefZ_ft + (fuselageHeight_ft * 0.5)) < 0)
            {
               // Canopy center is above top of fuselage
               double h = -canopyRefZ_ft - fuselageHeight_ft;      // Dist above the top of fuselage
               double theta_rad = asin(h / effectiveRadius_ft);    // Angle below horizontal
               double region_rad = UtMath::cPI - 2.0 * theta_rad;  // Angular region to subtract
               double fractionalRegion = region_rad / UtMath::cPI; // Fraction of half circle to subtract
               double halfArea_ft2 = fullArea_ft2 * 0.5;
               double removedArea_ft2 = halfArea_ft2 * fractionalRegion;
               canopyArea_ft2 = fullArea_ft2 - removedArea_ft2;
            }
            else
            {
               // Canopy center is at or within top of fuselage
               double h = fuselageHeight_ft  + canopyRefZ_ft;      // Dist below the top of fuselage
               double theta_rad = asin(h / effectiveRadius_ft);    // Angle below horizontal
               double region_rad = UtMath::cPI - 2.0 * theta_rad;  // Angular region to use
               double fractionalRegion = region_rad / UtMath::cPI; // Fraction of half circle to use
               double halfArea_ft2 = fullArea_ft2 * 0.5;
               canopyArea_ft2 = halfArea_ft2 * fractionalRegion;
            }
         }

         // Check for unexpected negative area
         if (canopyPresent && (canopyArea_ft2 <= 0.0))
         {
            canopyPresent = false;
            canopyArea_ft2 = 0.0;
         }

         GenerateAeroBody(object, canopyPresent, canopyArea_ft2);
      }
      else if (geometryType == "GeometryLandingGear")
      {
         GeometryLandingGear* object = dynamic_cast<GeometryLandingGear*>(geometryObject);

         GenerateAeroLandingGear(object);
      }
      else if (geometryType == "GeometryMassProperties")
      {
         // GeometryMassProperties has no effect on aero
      }
      else if (geometryType == "GeometryNacelle")
      {
         GeometryNacelle* object = dynamic_cast<GeometryNacelle*>(geometryObject);

         GenerateAeroNacelle(object);
      }
      else if (geometryType == "GeometryPointMass")
      {
         // GeometryPointMass has no effect on aero
      }
      else if (geometryType == "GeometryPropulsionData")
      {
         // GeometryPropulsionData has no effect on aero
      }
      else if (geometryType == "GeometrySpeedBrake")
      {
         GeometrySpeedBrake* object = dynamic_cast<GeometrySpeedBrake*>(geometryObject);

         GenerateAeroSpeedBrake(object);
      }
      else if (geometryType == "GeometrySurface")
      {
         GeometrySurface* object = dynamic_cast<GeometrySurface*>(geometryObject);

         GenerateAeroSurface(object);
      }
      else if (geometryType == "GeometryWing")
      {
         GeometryWing* object = dynamic_cast<GeometryWing*>(geometryObject);

         GenerateAeroWing(object);
      }
      else if (geometryType == "GeometryEngine")
      {
         GeometryEngine* object = dynamic_cast<GeometryEngine*>(geometryObject);

         GenerateAeroEngine(object);
      }
      else
      {
         MoverCreatorMainWindow().CreateErrorBox("Unable to process " + QString::fromStdString(geometryType) + " as a geometry type!");
         return;
      }
   }

   // Create the list of movable aero objects
   mAero->GenerateMovableAeroList();

   // Set the aero multipliers, based on vehicle type
   mAero->SetAeroMultipliers();
}

void Vehicle::GenerateAeroBody(GeometryBody* aGeometryObject,
                               bool          aIncludeCanopy,
                               double        aCanopyArea_ft2)
{
   if (mAero == nullptr) { return; } // If we lack the vehicle aero, simply return

   // Forward shapes include Round, Ogive, Cone, and Blunt
   GeometryBody::ForwardShapeType forwardShape = aGeometryObject->GetForwardShapeType();

   // Aft shapes include Round, Ogive, Cone, BoatTail, and Blunt
   GeometryBody::AftShapeType aftShape = aGeometryObject->GetAftShapeType();

   double forwardLength_ft = aGeometryObject->GetForwardShapeLength_ft();
   double aftLength_ft = aGeometryObject->GetAftShapeLength_ft();
   double aftDiam_ft = aGeometryObject->GetAftShapeDiam_ft();

   // Calculate frontal area, assuming an ellipse
   double area_ft2 = (aGeometryObject->GetHeight_ft() * 0.5) * (aGeometryObject->GetWidth_ft() * 0.5) * UtMath::cPI;

   // Get the surface area
   double surfaceArea_ft2 = aGeometryObject->GetMassArea_ft2();

   // Check if there is a canopy. If so, add its area.
   if (aIncludeCanopy && (aCanopyArea_ft2 > 0.0))
   {
      area_ft2 += aCanopyArea_ft2;
   }

   double length_ft = aGeometryObject->GetLength_ft();
   double height_ft = aGeometryObject->GetHeight_ft();
   double width_ft  = aGeometryObject->GetWidth_ft();
   double yaw_rad   = aGeometryObject->GetYawAngle_deg() * UtMath::cRAD_PER_DEG;
   double pitch_rad = aGeometryObject->GetPitchAngle_deg() * UtMath::cRAD_PER_DEG;
   double roll_rad  = aGeometryObject->GetRollAngle_deg() * UtMath::cRAD_PER_DEG;

   std::string geometryObjectName = aGeometryObject->GetName();

   // Replace any spaces with underscores
   std::replace(geometryObjectName.begin(), geometryObjectName.end(), ' ', '_');

   // Handle symmetry
   if (aGeometryObject->IsSymmetrical())
   {
      std::string name = geometryObjectName;

      std::string nameRgtSide = name + "_Right";
      nameRgtSide = mAero->MakeNameUnique(nameRgtSide);

      std::string nameLftSide = name + "_Left";
      nameLftSide = mAero->MakeNameUnique(nameLftSide);

      UtVec3dX refPt_ft = aGeometryObject->GetRefPoint_ft();
      double x = refPt_ft.X();
      double y = std::abs(refPt_ft.Y());
      double z = refPt_ft.Z();
      UtVec3dX refPtRgt_ft(x, y, z);

      // Consider the other body
      double currentBodyY = refPtRgt_ft.Y();
      double delta = currentBodyY - aGeometryObject->GetSymmetryHorizontalY();
      double otherBodyY = currentBodyY - 2.0 * delta;
      UtVec3dX refPtLft_ft(x, otherBodyY, z);

      double pitchRgt_rad = pitch_rad;
      double pitchLft_rad = pitch_rad;
      double yawRgt_rad   = (delta >= 0.0) ? yaw_rad : -yaw_rad;
      double yawLft_rad   = (delta >= 0.0) ? -yaw_rad : yaw_rad;
      double rollRgt_rad  = (delta >= 0.0) ? roll_rad : -roll_rad;
      double rollLft_rad  = (delta >= 0.0) ? -roll_rad : roll_rad;

      // Right side
      AeroBody* aeroObjRgt = new AeroBody(geometryObjectName, this);
      aeroObjRgt->SetData(nameRgtSide,
                          refPtRgt_ft,
                          length_ft,
                          height_ft,
                          width_ft,
                          area_ft2,
                          yawRgt_rad,
                          pitchRgt_rad,
                          rollRgt_rad,
                          forwardLength_ft,
                          forwardShape,
                          aftLength_ft,
                          aftDiam_ft,
                          aftShape,
                          surfaceArea_ft2);
      mAero->AddAeroObject(aeroObjRgt);

      // Left side
      AeroBody* aeroObjLft = new AeroBody(geometryObjectName, this);
      aeroObjLft->SetData(nameLftSide,
                          refPtLft_ft,
                          length_ft,
                          height_ft,
                          width_ft,
                          area_ft2,
                          yawLft_rad,
                          pitchLft_rad,
                          rollLft_rad,
                          forwardLength_ft,
                          forwardShape,
                          aftLength_ft,
                          aftDiam_ft,
                          aftShape,
                          surfaceArea_ft2);
      mAero->AddAeroObject(aeroObjLft);
   }
   else
   {
      std::string name = geometryObjectName;
      name = mAero->MakeNameUnique(name);

      AeroBody* aeroObj = new AeroBody(geometryObjectName, this);
      aeroObj->SetData(name,
                       aGeometryObject->GetRefPoint_ft(),
                       length_ft,
                       height_ft,
                       width_ft,
                       area_ft2,
                       yaw_rad,
                       pitch_rad,
                       roll_rad,
                       forwardLength_ft,
                       forwardShape,
                       aftLength_ft,
                       aftDiam_ft,
                       aftShape,
                       surfaceArea_ft2);
      mAero->AddAeroObject(aeroObj);
   }
}

void Vehicle::GenerateAeroDish(GeometryDish* aGeometryObject)
{
   if (mAero == nullptr) { return; } // If we lack the vehicle aero, simply return

   // Note: There is no symmetry for dishes

   std::string geometryObjectName = aGeometryObject->GetName();

   // Replace any spaces with underscores
   std::replace(geometryObjectName.begin(), geometryObjectName.end(), ' ', '_');

   std::string name = geometryObjectName;
   name = mAero->MakeNameUnique(name);

   AeroDish* aeroObj = new AeroDish(geometryObjectName, this);

   aeroObj->SetData(name,
                    aGeometryObject->GetRefPoint_ft(),
                    aGeometryObject->GetDiameter_ft(),
                    aGeometryObject->GetThickness_ft(),
                    aGeometryObject->GetMassArea_ft2());
   mAero->AddAeroObject(aeroObj);
}

void Vehicle::GenerateAeroLandingGear(GeometryLandingGear* aGeometryObject)
{
   if (mAero == nullptr) { return; } // If we lack the vehicle aero, simply return

   std::string geometryObjectName = aGeometryObject->GetName();

   // Replace any spaces with underscores
   std::replace(geometryObjectName.begin(), geometryObjectName.end(), ' ', '_');

   // Handle symmetry
   if (aGeometryObject->IsSymmetrical())
   {
      std::string name = geometryObjectName;

      std::string nameRgtSide = name + "_Right";
      nameRgtSide = mAero->MakeNameUnique(nameRgtSide);

      std::string nameLftSide = name + "_Left";
      nameLftSide = mAero->MakeNameUnique(nameLftSide);

      UtVec3dX refPt_ft = aGeometryObject->GetRefPoint_ft();
      double x = refPt_ft.X();
      double y = std::abs(refPt_ft.Y());
      double z = refPt_ft.Z();
      UtVec3dX refPtRgt_ft(x, y, z);

      double currentGearY = refPtRgt_ft.Y();
      double delta = currentGearY - aGeometryObject->GetSymmetryHorizontalY();
      double otherGearY = currentGearY - 2.0 * delta;
      UtVec3dX refPtLft_ft(x, otherGearY, z);

      AeroLandingGear* aeroObjRgt = new AeroLandingGear(geometryObjectName, this);

      aeroObjRgt->SetData(nameRgtSide,
                          refPtRgt_ft,
                          aGeometryObject->GetUncompressedLength_ft(),
                          aGeometryObject->GetStrutDiam_ft(),
                          aGeometryObject->GetTireDiam_ft(),
                          aGeometryObject->GetTireWidth_ft(),
                          aGeometryObject->GetMaxAngle_deg(),
                          aGeometryObject->GetCurrentAngle_deg(),
                          aGeometryObject->GetCdMultiplier(),
                          aGeometryObject->GetMassArea_ft2(),
                          aGeometryObject->GetMaxRetractionRate_dps(),
                          aGeometryObject->GetMaxExtensionRate_dps());
      mAero->AddAeroObject(aeroObjRgt);

      AeroLandingGear* aeroObjLft = new AeroLandingGear(geometryObjectName, this);

      aeroObjLft->SetData(nameLftSide,
                          refPtLft_ft,
                          aGeometryObject->GetUncompressedLength_ft(),
                          aGeometryObject->GetStrutDiam_ft(),
                          aGeometryObject->GetTireDiam_ft(),
                          aGeometryObject->GetTireWidth_ft(),
                          aGeometryObject->GetMaxAngle_deg(),
                          aGeometryObject->GetCurrentAngle_deg(),
                          aGeometryObject->GetCdMultiplier(),
                          aGeometryObject->GetMassArea_ft2(),
                          aGeometryObject->GetMaxRetractionRate_dps(),
                          aGeometryObject->GetMaxExtensionRate_dps());
      mAero->AddAeroObject(aeroObjLft);
   }
   else
   {
      std::string name = geometryObjectName;
      name = mAero->MakeNameUnique(name);

      AeroLandingGear* aeroObj = new AeroLandingGear(geometryObjectName, this);

      aeroObj->SetData(name,
                       aGeometryObject->GetRefPoint_ft(),
                       aGeometryObject->GetUncompressedLength_ft(),
                       aGeometryObject->GetStrutDiam_ft(),
                       aGeometryObject->GetTireDiam_ft(),
                       aGeometryObject->GetTireWidth_ft(),
                       aGeometryObject->GetMaxAngle_deg(),
                       aGeometryObject->GetCurrentAngle_deg(),
                       aGeometryObject->GetCdMultiplier(),
                       aGeometryObject->GetMassArea_ft2(),
                       aGeometryObject->GetMaxRetractionRate_dps(),
                       aGeometryObject->GetMaxExtensionRate_dps());
      mAero->AddAeroObject(aeroObj);
   }
}

void Vehicle::GenerateAeroNacelle(GeometryNacelle* aGeometryObject)
{
   if (mAero == nullptr) { return; } // If we lack the vehicle aero, simply return

   // Overall shapes include Rounded, Half-Round-Right, Half-Round-Left,
   // Half-Round-Top, Half-Round-Bottom, Flat-Sided, Flat-Swept-Right,
   // and Flat-Swept-Left
   std::string overallShapeString = aGeometryObject->GetOverallShapeString().toStdString();

   // Aft section types include Blunt and Tapered
   std::string aftSectionString = aGeometryObject->GetAftSectionString().toStdString();

   double forwardSweepLength_ft = aGeometryObject->GetForwardSweepLength_ft();
   double aftSectionLength_ft   = aGeometryObject->GetAftSectionLength_ft();

   double length_ft      = aGeometryObject->GetLength_ft();
   double height_ft      = aGeometryObject->GetHeight_ft();
   double heightInner_ft = aGeometryObject->GetHeightInner_ft();
   double heightOuter_ft = aGeometryObject->GetHeightOuter_ft();
   double width_ft       = aGeometryObject->GetWidth_ft();
   double thickness_ft   = aGeometryObject->GetThickness_ft();
   double yaw_rad   = aGeometryObject->GetYawAngle_deg() * UtMath::cRAD_PER_DEG;
   double pitch_rad = aGeometryObject->GetPitchAngle_deg() * UtMath::cRAD_PER_DEG;
   double roll_rad  = aGeometryObject->GetRollAngle_deg() * UtMath::cRAD_PER_DEG;

   // Get the surface area
   double surfaceArea_ft2 = aGeometryObject->GetMassArea_ft2();

   std::string geometryObjectName = aGeometryObject->GetName();

   // Replace any spaces with underscores
   std::replace(geometryObjectName.begin(), geometryObjectName.end(), ' ', '_');

   // Handle symmetry
   if (aGeometryObject->IsSymmetrical())
   {
      std::string name = geometryObjectName;

      std::string nameRgtSide = name + "_Right";
      nameRgtSide = mAero->MakeNameUnique(nameRgtSide);

      std::string nameLftSide = name + "_Left";
      nameLftSide = mAero->MakeNameUnique(nameLftSide);

      // Ensure proper opposite shape
      std::string oppositeOverallShapeString = overallShapeString;
      if (oppositeOverallShapeString == "Half-Round-Right")
      {
         oppositeOverallShapeString = "Half-Round-Left";
      }
      else if (oppositeOverallShapeString == "Half-Round-Left")
      {
         oppositeOverallShapeString = "Half-Round-Right";
      }
      else if (oppositeOverallShapeString == "Flat-Swept-Right")
      {
         oppositeOverallShapeString = "Flat-Swept-Left";
      }
      else if (oppositeOverallShapeString == "Flat-Swept-Left")
      {
         oppositeOverallShapeString = "Flat-Swept-Right";
      }

      UtVec3dX refPt_ft = aGeometryObject->GetRefPoint_ft();
      double x = refPt_ft.X();
      double y = std::abs(refPt_ft.Y());
      double z = refPt_ft.Z();
      UtVec3dX refPtRgt_ft(x, y, z);

      // Consider the other body
      double currentBodyY = refPtRgt_ft.Y();
      double delta = currentBodyY - aGeometryObject->GetSymmetryHorizontalY();
      double otherBodyY = currentBodyY - 2.0 * delta;
      UtVec3dX refPtLft_ft(x, otherBodyY, z);

      double pitchRgt_rad = pitch_rad;
      double pitchLft_rad = pitch_rad;
      double yawRgt_rad   = (delta >= 0.0) ? yaw_rad : -yaw_rad;
      double yawLft_rad   = (delta >= 0.0) ? -yaw_rad : yaw_rad;
      double rollRgt_rad  = (delta >= 0.0) ? roll_rad : -roll_rad;
      double rollLft_rad  = (delta >= 0.0) ? -roll_rad : roll_rad;

      std::string rightOverallShapeString = (delta >= 0.0) ? overallShapeString : oppositeOverallShapeString;
      std::string leftOverallShapeString = (delta >= 0.0) ? oppositeOverallShapeString : overallShapeString;

      // Right side
      AeroNacelle* aeroObjRgt = new AeroNacelle(geometryObjectName, this);
      aeroObjRgt->SetData(nameRgtSide,
                          refPtRgt_ft,
                          length_ft,
                          height_ft,
                          heightInner_ft,
                          heightOuter_ft,
                          width_ft,
                          thickness_ft,
                          yawRgt_rad,
                          pitchRgt_rad,
                          rollRgt_rad,
                          rightOverallShapeString,
                          forwardSweepLength_ft,
                          aftSectionString,
                          aftSectionLength_ft,
                          surfaceArea_ft2);

      // Left side
      AeroNacelle* aeroObjLft = new AeroNacelle(geometryObjectName, this);
      aeroObjLft->SetData(nameLftSide,
                          refPtLft_ft,
                          length_ft,
                          height_ft,
                          heightInner_ft,
                          heightOuter_ft,
                          width_ft,
                          thickness_ft,
                          yawLft_rad,
                          pitchLft_rad,
                          rollLft_rad,
                          leftOverallShapeString,
                          forwardSweepLength_ft,
                          aftSectionString,
                          aftSectionLength_ft,
                          surfaceArea_ft2);
   }
   else
   {
      std::string name = geometryObjectName;
      name = mAero->MakeNameUnique(name);

      AeroNacelle* aeroObj = new AeroNacelle(geometryObjectName, this);
      aeroObj->SetData(name,
                       aGeometryObject->GetRefPoint_ft(),
                       length_ft,
                       height_ft,
                       heightInner_ft,
                       heightOuter_ft,
                       width_ft,
                       thickness_ft,
                       yaw_rad,
                       pitch_rad,
                       roll_rad,
                       overallShapeString,
                       forwardSweepLength_ft,
                       aftSectionString,
                       aftSectionLength_ft,
                       surfaceArea_ft2);
      mAero->AddAeroObject(aeroObj);
   }
}

void Vehicle::GenerateAeroSpeedBrake(GeometrySpeedBrake* aGeometryObject)
{
   if (mAero == nullptr) { return; } // If we lack the vehicle aero, simply return

   double length_ft             = aGeometryObject->GetLength_ft();
   double width_ft              = aGeometryObject->GetWidth_ft();
   double maxAngle_deg          = aGeometryObject->GetMaxAngle_deg();
   double currentAngle_deg      = aGeometryObject->GetCurrentAngle_deg();
   double cdMultiplier          = aGeometryObject->GetCdMultiplier();
   double maxRetractionRate_dps = aGeometryObject->GetMaxRetractionRate_dps();
   double maxExtensionRate_dps  = aGeometryObject->GetMaxExtensionRate_dps();

   std::string geometryObjectName = aGeometryObject->GetName();

   // Replace any spaces with underscores
   std::replace(geometryObjectName.begin(), geometryObjectName.end(), ' ', '_');

   // Get the surface area
   double surfaceArea_ft2 = aGeometryObject->GetMassArea_ft2();

   // Handle symmetry
   if (aGeometryObject->IsHorizontallySymmetrical())
   {
      std::string name = geometryObjectName;

      std::string nameRgtSide = name + "_Right";
      nameRgtSide = mAero->MakeNameUnique(nameRgtSide);

      std::string nameLftSide = name + "_Left";
      nameLftSide = mAero->MakeNameUnique(nameLftSide);

      UtVec3dX refPt_ft = aGeometryObject->GetRefPoint_ft();
      double x = refPt_ft.X();
      double y = std::abs(refPt_ft.Y());
      double z = refPt_ft.Z();
      UtVec3dX refPtRgt_ft(x, y, z);

      double currentBrakeY = refPtRgt_ft.Y();
      double delta = currentBrakeY - aGeometryObject->GetSymmetryHorizontalY();
      double otherBrakeY = currentBrakeY - 2.0 * delta;
      UtVec3dX refPtLft_ft(x, otherBrakeY, z);

      // Check which side is used
      double rollAngle_rad = aGeometryObject->GetRollAngle_deg() * UtMath::cRAD_PER_DEG;
      double rollAngleRgt_rad = 0.0;
      double rollAngleLft_rad = 0.0;
      if (y >= 0.0)
      {
         // Right side specified
         rollAngleRgt_rad = rollAngle_rad;
         rollAngleLft_rad = -rollAngle_rad;
      }
      else
      {
         // Left side specified
         rollAngleLft_rad = rollAngle_rad;
         rollAngleRgt_rad = -rollAngle_rad;
      }

      AeroSpeedBrake* aeroObjRgt = new AeroSpeedBrake(geometryObjectName, this);

      aeroObjRgt->SetData(nameRgtSide,
                          refPtRgt_ft,
                          length_ft,
                          width_ft,
                          rollAngleRgt_rad,
                          maxAngle_deg,
                          currentAngle_deg,
                          cdMultiplier,
                          surfaceArea_ft2,
                          maxRetractionRate_dps,
                          maxExtensionRate_dps);
      mAero->AddAeroObject(aeroObjRgt);

      AeroSpeedBrake* aeroObjLft = new AeroSpeedBrake(geometryObjectName, this);

      aeroObjLft->SetData(nameLftSide,
                          refPtLft_ft,
                          length_ft,
                          width_ft,
                          rollAngleLft_rad,
                          maxAngle_deg,
                          currentAngle_deg,
                          cdMultiplier,
                          surfaceArea_ft2,
                          maxRetractionRate_dps,
                          maxExtensionRate_dps);
      mAero->AddAeroObject(aeroObjLft);
   }
   else if (aGeometryObject->IsVerticallySymmetrical())
   {
      std::string name = geometryObjectName;

      std::string nameTop = name + "_Top";
      nameTop = mAero->MakeNameUnique(nameTop);

      std::string nameBottom = name + "_Bottom";
      nameBottom = mAero->MakeNameUnique(nameBottom);

      UtVec3dX refPt_ft = aGeometryObject->GetRefPoint_ft();
      double x = refPt_ft.X();
      double y = refPt_ft.Y();
      double z = std::abs(refPt_ft.Z());
      UtVec3dX refPtBottom_ft(x, y, z);

      double currentBrakeZ = refPtBottom_ft.Z();
      double delta = currentBrakeZ - aGeometryObject->GetSymmetryVerticalZ();
      double otherBrakeZ = currentBrakeZ - 2.0 * delta;
      UtVec3dX refPtTop_ft(x, y, otherBrakeZ);

      // Check which side is used
      double rollAngle_rad = aGeometryObject->GetRollAngle_deg() * UtMath::cRAD_PER_DEG;
      double rollAngleTop_rad = 0.0;
      double rollAngleBottom_rad = 0.0;
      if (z >= 0.0)
      {
         // Top specified
         rollAngleTop_rad = rollAngle_rad;
         rollAngleBottom_rad = UtMath::cPI - rollAngle_rad;
      }
      else
      {
         // Bottom specified
         rollAngleBottom_rad = rollAngle_rad;
         rollAngleTop_rad = UtMath::cPI - rollAngle_rad;
      }

      AeroSpeedBrake* aeroObjTop = new AeroSpeedBrake(geometryObjectName, this);

      aeroObjTop->SetData(nameTop,
                          refPtTop_ft,
                          length_ft,
                          width_ft,
                          rollAngleTop_rad,
                          maxAngle_deg,
                          currentAngle_deg,
                          cdMultiplier,
                          surfaceArea_ft2,
                          maxRetractionRate_dps,
                          maxExtensionRate_dps);
      mAero->AddAeroObject(aeroObjTop);

      AeroSpeedBrake* aeroObjBottom = new AeroSpeedBrake(geometryObjectName, this);

      aeroObjBottom->SetData(nameBottom,
                             refPtBottom_ft,
                             length_ft,
                             width_ft,
                             rollAngleBottom_rad,
                             maxAngle_deg,
                             currentAngle_deg,
                             cdMultiplier,
                             surfaceArea_ft2,
                             maxRetractionRate_dps,
                             maxExtensionRate_dps);
      mAero->AddAeroObject(aeroObjBottom);
   }
   else
   {
      std::string name = geometryObjectName;
      name = mAero->MakeNameUnique(name);

      AeroSpeedBrake* aeroObj = new AeroSpeedBrake(geometryObjectName, this);

      aeroObj->SetData(name,
                       aGeometryObject->GetRefPoint_ft(),
                       length_ft,
                       width_ft,
                       aGeometryObject->GetRollAngle_deg()*UtMath::cRAD_PER_DEG,
                       maxAngle_deg,
                       currentAngle_deg,
                       cdMultiplier,
                       surfaceArea_ft2,
                       maxRetractionRate_dps,
                       maxExtensionRate_dps);
      mAero->AddAeroObject(aeroObj);
   }
}

void Vehicle::GenerateAeroSurface(GeometrySurface* aGeometryObject)
{
   if (mAero == nullptr) { return; } // If we lack the vehicle aero, simply return

   std::string geometryObjectName = aGeometryObject->GetName();

   // Replace any spaces with underscores
   std::replace(geometryObjectName.begin(), geometryObjectName.end(), ' ', '_');

   UtVec3dX refPoint_ft      = aGeometryObject->GetRefPoint_ft();
   double span_ft            = aGeometryObject->GetSpan_ft();
   double rootChord_ft       = aGeometryObject->GetRootChord_ft();
   double tipChord_ft        = aGeometryObject->GetTipChord_ft();
   double sweepAngle_rad     = aGeometryObject->GetSweepAngle_deg() * UtMath::cRAD_PER_DEG;
   double dihedralAngle_rad  = aGeometryObject->GetDihedralAngle_deg() * UtMath::cRAD_PER_DEG;
   double incidenceAngle_rad = aGeometryObject->GetIncidenceAngle_deg() * UtMath::cRAD_PER_DEG;
   double thicknessRatio     = aGeometryObject->GetThicknessRatio();
   double oswalds            = aGeometryObject->GetOswaldsEfficiency();
   double aspectRatio        = aGeometryObject->GetAspectRatio();
   double area_ft2           = aGeometryObject->GetPlanformArea_ft2();
   int    numAeroSections    = aGeometryObject->GetNumberofAeroSections();

   std::string airfoilFilename = aGeometryObject->GetAirfoil().toStdString();

   GeometrySurface::eSurfaceControlMotion surfaceControlMotion = aGeometryObject->GetSurfaceControlMotion();

   double  controlSurfaceChordFractionStart = aGeometryObject->GetControlSurfaceChordFractionStart();
   double  controlSurfaceChordFractionEnd   = aGeometryObject->GetControlSurfaceChordFractionEnd();
   double  controlSurfaceSpanFractionStart  = aGeometryObject->GetControlSurfaceSpanFractionStart();
   double  controlSurfaceSpanFractionEnd    = aGeometryObject->GetControlSurfaceSpanFractionEnd();
   double  controlSurfaceMinAngle_deg       = aGeometryObject->GetControlSurfaceMinAngle_deg();
   double  controlSurfaceMaxAngle_deg       = aGeometryObject->GetControlSurfaceMaxAngle_deg();
   bool    useExponentialAngleMapping       = aGeometryObject->GetUseExponentialAngleMapping();
   double  actuatorMinRate_dps              = aGeometryObject->GetActuatorMinRate_dps();
   double  actuatorMaxRate_dps              = aGeometryObject->GetActuatorMaxRate_dps();
   double  actuatorMinAngle_deg             = aGeometryObject->GetActuatorMinAngle_deg();
   double  actuatorMaxAngle_deg             = aGeometryObject->GetActuatorMaxAngle_deg();
   double  symmetryHorizontalY              = aGeometryObject->GetSymmetryHorizontalY();
   double  symmetryVerticalZ                = aGeometryObject->GetSymmetryVerticalZ();

   // Get the surface area
   double surfaceArea_ft2 = aGeometryObject->GetMassArea_ft2();

   // Handle symmetry (May be "Single", "Horizontal", "Vertical", "+ Pattern", or "X Pattern")
   std::string symmetryString = aGeometryObject->GetSymmetryString().toStdString();
   if (symmetryString == "Horizontal")
   {
      std::string name = geometryObjectName;

      std::string nameRgt = name + "_Right";
      nameRgt = mAero->MakeNameUnique(nameRgt);

      std::string nameLft = name + "_Left";
      nameLft = mAero->MakeNameUnique(nameLft);

      // Note: Surfaces use validators to ensure right-side only (y >= 0)

      UtVec3dX refPtRgt = refPoint_ft;
      double dihedralRgt_rad = dihedralAngle_rad;
      double incidenceAngleRgt_rad = incidenceAngle_rad;
      bool invertVectorsRgt = false;

      // Now, determine location of other surface
      double currentSurfaceY = refPtRgt.Y();
      double delta = currentSurfaceY - symmetryHorizontalY;
      double otherSurfaceY = currentSurfaceY - 2.0 * delta;

      UtVec3dX refPtLft(refPtRgt.X(), otherSurfaceY, refPtRgt.Z());
      double dihedralLft_rad = UtMath::cPI - dihedralRgt_rad;
      double incidenceAngleLft_rad = -incidenceAngle_rad;
      bool invertVectorsLft = true;

      if (surfaceControlMotion == GeometrySurface::cControlMotionFixed)
      {
         AeroSurface* aeroObjRgt = new AeroSurface(geometryObjectName, this);
         aeroObjRgt->SetData(nameRgt, refPtRgt, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                             sweepAngle_rad, dihedralRgt_rad, incidenceAngleRgt_rad, oswalds,
                             invertVectorsRgt, aspectRatio, area_ft2, numAeroSections,
                             surfaceArea_ft2);

         aeroObjRgt->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

         mAero->AddAeroObject(aeroObjRgt);
      }
      else if(surfaceControlMotion == GeometrySurface::cControlMotionFullFlying)
      {
         AeroFullFlyingSurface* aeroObjRgt = new AeroFullFlyingSurface(geometryObjectName, this);
         aeroObjRgt->SetData(nameRgt, refPtRgt, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                             sweepAngle_rad, dihedralRgt_rad, incidenceAngleRgt_rad, oswalds,
                             invertVectorsRgt, aspectRatio, area_ft2, numAeroSections,
                             surfaceArea_ft2);

         aeroObjRgt->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                       controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                       actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg);

         mAero->AddAeroObject(aeroObjRgt);
      }
      else // if(surfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
      {
         AeroHingedSurface* aeroObjRgt = new AeroHingedSurface(geometryObjectName, this);
         aeroObjRgt->SetData(nameRgt, refPtRgt, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                             sweepAngle_rad, dihedralRgt_rad, incidenceAngleRgt_rad, oswalds,
                             invertVectorsRgt, aspectRatio, area_ft2, numAeroSections,
                             surfaceArea_ft2);

         aeroObjRgt->GetControlSurface()->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                                            controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                                            actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg,
                                                            controlSurfaceChordFractionStart, controlSurfaceChordFractionEnd, 
                                                            controlSurfaceSpanFractionStart, controlSurfaceSpanFractionEnd);

         mAero->AddAeroObject(aeroObjRgt);
      }

      if (surfaceControlMotion == GeometrySurface::cControlMotionFixed)
      {
         AeroSurface* aeroObjLft = new AeroSurface(geometryObjectName, this);
         aeroObjLft->SetData(nameLft, refPtLft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                             sweepAngle_rad, dihedralLft_rad, incidenceAngleLft_rad, oswalds,
                             invertVectorsLft, aspectRatio, area_ft2, numAeroSections,
                             surfaceArea_ft2);

         aeroObjLft->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

         mAero->AddAeroObject(aeroObjLft);
      }
      else if (surfaceControlMotion == GeometrySurface::cControlMotionFullFlying)
      {
         AeroFullFlyingSurface* aeroObjLft = new AeroFullFlyingSurface(geometryObjectName, this);
         aeroObjLft->SetData(nameLft, refPtLft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                             sweepAngle_rad, dihedralLft_rad, incidenceAngleLft_rad, oswalds,
                             invertVectorsLft, aspectRatio, area_ft2, numAeroSections,
                             surfaceArea_ft2);

         aeroObjLft->SetAdditionalData(airfoilFilename, useExponentialAngleMapping,
                                       controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg,
                                       actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg);

         mAero->AddAeroObject(aeroObjLft);
      }
      else //if (surfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
      {
         AeroHingedSurface* aeroObjLft = new AeroHingedSurface(geometryObjectName, this);
         aeroObjLft->SetData(nameLft, refPtLft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                             sweepAngle_rad, dihedralLft_rad, incidenceAngleLft_rad, oswalds,
                             invertVectorsLft, aspectRatio, area_ft2, numAeroSections,
                             surfaceArea_ft2);

         aeroObjLft->GetControlSurface()->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                                            controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                                            actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg,
                                                            controlSurfaceChordFractionStart, controlSurfaceChordFractionEnd, 
                                                            controlSurfaceSpanFractionStart, controlSurfaceSpanFractionEnd);

         mAero->AddAeroObject(aeroObjLft);
      }

   }
   else if (symmetryString == "Vertical")
   {
      std::string name = geometryObjectName;

      std::string nameTop = name + "_Top";
      nameTop = mAero->MakeNameUnique(nameTop);

      std::string nameBottom = name + "_Bottom";
      nameBottom = mAero->MakeNameUnique(nameBottom);

      // Note: Surfaces use validators to ensure right-side only (y >= 0)

      UtVec3dX refPtTop = refPoint_ft;
      UtVec3dX refPtBottom = refPoint_ft;

      if (refPoint_ft.Z() <= 0.0)
      {
         // The ref pt is for the top point
         refPtTop = refPoint_ft;

         // Now, determine location of other surface
         double currentSurfaceZ = refPtTop.Z();
         double delta = currentSurfaceZ - symmetryVerticalZ;
         double otherSurfaceZ = currentSurfaceZ - 2.0 * delta;
         refPtBottom.Set(refPtTop.X(), refPtTop.Y(), otherSurfaceZ);

         double dihedralTop_rad = dihedralAngle_rad;
         double incidenceAngleTop_rad = incidenceAngle_rad;
         bool invertVectorsTop = false;

         double dihedralBottom_rad = -dihedralTop_rad;
         double incidenceAngleBottom_rad = -incidenceAngle_rad;
         bool invertVectorsBottom = true;

         if (surfaceControlMotion == GeometrySurface::cControlMotionFixed)
         {
            AeroSurface* aeroObjTop = new AeroSurface(geometryObjectName, this);
            aeroObjTop->SetData(nameTop, refPtTop, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                sweepAngle_rad, dihedralTop_rad, incidenceAngleTop_rad, oswalds,
                                invertVectorsTop, aspectRatio, area_ft2, numAeroSections,
                                surfaceArea_ft2);

            aeroObjTop->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

            mAero->AddAeroObject(aeroObjTop);
         }
         else if (surfaceControlMotion == GeometrySurface::cControlMotionFullFlying)
         {
            AeroFullFlyingSurface* aeroObjTop = new AeroFullFlyingSurface(geometryObjectName, this);
            aeroObjTop->SetData(nameTop, refPtTop, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                sweepAngle_rad, dihedralTop_rad, incidenceAngleTop_rad, oswalds,
                                invertVectorsTop, aspectRatio, area_ft2, numAeroSections,
                                surfaceArea_ft2);

            aeroObjTop->SetAdditionalData(airfoilFilename, useExponentialAngleMapping,
                                          controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg,
                                          actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg);

            mAero->AddAeroObject(aeroObjTop);
         }
         else // if (surfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
         {
            AeroHingedSurface* aeroObjTop = new AeroHingedSurface(geometryObjectName, this);
            aeroObjTop->SetData(nameTop, refPtTop, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                sweepAngle_rad, dihedralTop_rad, incidenceAngleTop_rad, oswalds,
                                invertVectorsTop, aspectRatio, area_ft2, numAeroSections,
                                surfaceArea_ft2);

            aeroObjTop->GetControlSurface()->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                                               controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                                               actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg,
                                                               controlSurfaceChordFractionStart, controlSurfaceChordFractionEnd, 
                                                               controlSurfaceSpanFractionStart, controlSurfaceSpanFractionEnd);

            mAero->AddAeroObject(aeroObjTop);
         }

         if (surfaceControlMotion == GeometrySurface::cControlMotionFixed)
         {
            AeroSurface* aeroObjBottom = new AeroSurface(geometryObjectName, this);
            aeroObjBottom->SetData(nameBottom, refPtBottom, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                   sweepAngle_rad, dihedralBottom_rad, incidenceAngleBottom_rad, oswalds,
                                   invertVectorsBottom, aspectRatio, area_ft2, numAeroSections,
                                   surfaceArea_ft2);

            aeroObjBottom->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

            mAero->AddAeroObject(aeroObjBottom);
         }
         else if (surfaceControlMotion == GeometrySurface::cControlMotionFullFlying)
         {
            AeroFullFlyingSurface* aeroObjBottom = new AeroFullFlyingSurface(geometryObjectName, this);
            aeroObjBottom->SetData(nameBottom, refPtBottom, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                   sweepAngle_rad, dihedralBottom_rad, incidenceAngleBottom_rad, oswalds,
                                   invertVectorsBottom, aspectRatio, area_ft2, numAeroSections,
                                   surfaceArea_ft2);

            aeroObjBottom->SetAdditionalData(airfoilFilename, useExponentialAngleMapping,
                                             controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg,
                                             actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg);

            mAero->AddAeroObject(aeroObjBottom);
         }
         else // if (surfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
         {
            AeroHingedSurface* aeroObjBottom = new AeroHingedSurface(geometryObjectName, this);
            aeroObjBottom->SetData(nameBottom, refPtBottom, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                   sweepAngle_rad, dihedralBottom_rad, incidenceAngleBottom_rad, oswalds,
                                   invertVectorsBottom, aspectRatio, area_ft2, numAeroSections,
                                   surfaceArea_ft2);

            aeroObjBottom->GetControlSurface()->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                                                  controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                                                  actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg,
                                                                  controlSurfaceChordFractionStart, controlSurfaceChordFractionEnd, 
                                                                  controlSurfaceSpanFractionStart, controlSurfaceSpanFractionEnd);

            mAero->AddAeroObject(aeroObjBottom);
         }
      }
      else
      {
         // The ref pt is for the bottom point
         refPtBottom = refPoint_ft;

         // Now, determine location of other surface
         double currentSurfaceZ = refPtBottom.Z();
         double delta = currentSurfaceZ - symmetryVerticalZ;
         double otherSurfaceZ = currentSurfaceZ - 2.0 * delta;
         refPtTop.Set(refPtBottom.X(), refPtBottom.Y(), otherSurfaceZ);

         double dihedralBottom_rad = dihedralAngle_rad;
         double incidenceAngleBottom_rad = incidenceAngle_rad;
         bool invertVectorsBottom = false;

         double dihedralTop_rad = -dihedralBottom_rad;
         double incidenceAngleTop_rad = -incidenceAngle_rad;
         bool invertVectorsTop = true;

         if (surfaceControlMotion == GeometrySurface::cControlMotionFixed)
         {
            AeroSurface* aeroObjTop = new AeroSurface(geometryObjectName, this);
            aeroObjTop->SetData(nameTop, refPtTop, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                sweepAngle_rad, dihedralTop_rad, incidenceAngleTop_rad, oswalds,
                                invertVectorsTop, aspectRatio, area_ft2, numAeroSections,
                                surfaceArea_ft2);

            aeroObjTop->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

            mAero->AddAeroObject(aeroObjTop);
         }
         else if (surfaceControlMotion == GeometrySurface::cControlMotionFullFlying)
         {
            AeroFullFlyingSurface* aeroObjTop = new AeroFullFlyingSurface(geometryObjectName, this);
            aeroObjTop->SetData(nameTop, refPtTop, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                sweepAngle_rad, dihedralTop_rad, incidenceAngleTop_rad, oswalds,
                                invertVectorsTop, aspectRatio, area_ft2, numAeroSections,
                                surfaceArea_ft2);

            aeroObjTop->SetAdditionalData(airfoilFilename, useExponentialAngleMapping,
                                          controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg,
                                          actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg);

            mAero->AddAeroObject(aeroObjTop);
         }
         else if (surfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
         {
            AeroHingedSurface* aeroObjTop = new AeroHingedSurface(geometryObjectName, this);
            aeroObjTop->SetData(nameTop, refPtTop, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                sweepAngle_rad, dihedralTop_rad, incidenceAngleTop_rad, oswalds,
                                invertVectorsTop, aspectRatio, area_ft2, numAeroSections,
                                surfaceArea_ft2);

            aeroObjTop->GetControlSurface()->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                                               controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                                               actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg,
                                                               controlSurfaceChordFractionStart, controlSurfaceChordFractionEnd, 
                                                               controlSurfaceSpanFractionStart, controlSurfaceSpanFractionEnd);

            mAero->AddAeroObject(aeroObjTop);
         }

         if (surfaceControlMotion == GeometrySurface::cControlMotionFixed)
         {
            AeroSurface* aeroObjBottom = new AeroSurface(geometryObjectName, this);
            aeroObjBottom->SetData(nameBottom, refPtBottom, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                   sweepAngle_rad, dihedralBottom_rad, incidenceAngleBottom_rad, oswalds,
                                   invertVectorsBottom, aspectRatio, area_ft2, numAeroSections,
                                   surfaceArea_ft2);

            aeroObjBottom->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

            mAero->AddAeroObject(aeroObjBottom);
         }
         else if (surfaceControlMotion == GeometrySurface::cControlMotionFullFlying)
         {
            AeroFullFlyingSurface* aeroObjBottom = new AeroFullFlyingSurface(geometryObjectName, this);
            aeroObjBottom->SetData(nameBottom, refPtBottom, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                   sweepAngle_rad, dihedralBottom_rad, incidenceAngleBottom_rad, oswalds,
                                   invertVectorsBottom, aspectRatio, area_ft2, numAeroSections,
                                   surfaceArea_ft2);

            aeroObjBottom->SetAdditionalData(airfoilFilename, useExponentialAngleMapping,
                                             controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg,
                                             actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg);

            mAero->AddAeroObject(aeroObjBottom);
         }
         else // if (surfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
         {
            AeroHingedSurface* aeroObjBottom = new AeroHingedSurface(geometryObjectName, this);
            aeroObjBottom->SetData(nameBottom, refPtBottom, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                   sweepAngle_rad, dihedralBottom_rad, incidenceAngleBottom_rad, oswalds,
                                   invertVectorsBottom, aspectRatio, area_ft2, numAeroSections,
                                   surfaceArea_ft2);

            aeroObjBottom->GetControlSurface()->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                                                  controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                                                  actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg,
                                                                  controlSurfaceChordFractionStart, controlSurfaceChordFractionEnd, 
                                                                  controlSurfaceSpanFractionStart, controlSurfaceSpanFractionEnd);

            mAero->AddAeroObject(aeroObjBottom);
         }
      }
   }
   else if (symmetryString == "+ Pattern")
   {
      std::string name = geometryObjectName;

      std::string nameRgt = name + "_Right";
      nameRgt = mAero->MakeNameUnique(nameRgt);

      std::string nameTop = name + "_Top";
      nameTop = mAero->MakeNameUnique(nameTop);

      std::string nameLft = name + "_Left";
      nameLft = mAero->MakeNameUnique(nameLft);

      std::string nameBottom = name + "_Bottom";
      nameBottom = mAero->MakeNameUnique(nameBottom);

      // Right fin

      bool invertVectors = false;
      double radius      = aGeometryObject->GetFinRefRadius_ft();
      double angle_rad   = 0.0;
      double x           = 0.0;
      double y           =  radius * cos(angle_rad);
      double z           = -radius * sin(angle_rad);
      UtVec3dX refPtRgt(x, y, z);
      dihedralAngle_rad = angle_rad;

      // Add main ref pt
      refPtRgt = refPtRgt + aGeometryObject->GetRefPoint_ft();

      if (surfaceControlMotion == GeometrySurface::cControlMotionFixed)
      {
         AeroSurface* aeroObjRgt = new AeroSurface(geometryObjectName, this);
         aeroObjRgt->SetData(nameRgt, refPtRgt, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                             sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                             invertVectors, aspectRatio, area_ft2, numAeroSections,
                             surfaceArea_ft2);

         aeroObjRgt->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

         mAero->AddAeroObject(aeroObjRgt);
      }
      else if (surfaceControlMotion == GeometrySurface::cControlMotionFullFlying)
      {
         AeroFullFlyingSurface* aeroObjRgt = new AeroFullFlyingSurface(geometryObjectName, this);
         aeroObjRgt->SetData(nameRgt, refPtRgt, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                             sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                             invertVectors, aspectRatio, area_ft2, numAeroSections,
                             surfaceArea_ft2);

         aeroObjRgt->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                       controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                       actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg);

         mAero->AddAeroObject(aeroObjRgt);
      }
      else // if (surfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
      {
         AeroHingedSurface* aeroObjRgt = new AeroHingedSurface(geometryObjectName, this);
         aeroObjRgt->SetData(nameRgt, refPtRgt, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                             sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                             invertVectors, aspectRatio, area_ft2, numAeroSections,
                             surfaceArea_ft2);

         aeroObjRgt->GetControlSurface()->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                                            controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                                            actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg,
                                                            controlSurfaceChordFractionStart, controlSurfaceChordFractionEnd, 
                                                            controlSurfaceSpanFractionStart, controlSurfaceSpanFractionEnd);

         mAero->AddAeroObject(aeroObjRgt);
      }
      // Top fin

      angle_rad += UtMath::cPI_OVER_2;
      y =  radius * cos(angle_rad);
      z = -radius * sin(angle_rad);
      UtVec3dX refPtTop(x, y, z);
      dihedralAngle_rad = angle_rad;

      // Add main ref pt
      refPtTop = refPtTop + aGeometryObject->GetRefPoint_ft();

      if (surfaceControlMotion == GeometrySurface::cControlMotionFixed)
      {
         AeroSurface* aeroObjTop = new AeroSurface(geometryObjectName, this);
         aeroObjTop->SetData(nameTop, refPtTop, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                              sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                              invertVectors, aspectRatio, area_ft2, numAeroSections,
                              surfaceArea_ft2);

         aeroObjTop->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

         mAero->AddAeroObject(aeroObjTop);
      }
      else if (surfaceControlMotion == GeometrySurface::cControlMotionFullFlying)
      {
         AeroFullFlyingSurface* aeroObjTop = new AeroFullFlyingSurface(geometryObjectName, this);
         aeroObjTop->SetData(nameTop, refPtTop, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                              sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                              invertVectors, aspectRatio, area_ft2, numAeroSections,
                              surfaceArea_ft2);

         aeroObjTop->SetAdditionalData(airfoilFilename, useExponentialAngleMapping,
                                       controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg,
                                       actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg);

         mAero->AddAeroObject(aeroObjTop);
      }
      else // if (surfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
      {
         AeroHingedSurface* aeroObjTop = new AeroHingedSurface(geometryObjectName, this);
         aeroObjTop->SetData(nameTop, refPtTop, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                              sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                              invertVectors, aspectRatio, area_ft2, numAeroSections,
                              surfaceArea_ft2);

         aeroObjTop->GetControlSurface()->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                                            controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                                            actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg,
                                                            controlSurfaceChordFractionStart, controlSurfaceChordFractionEnd, 
                                                            controlSurfaceSpanFractionStart, controlSurfaceSpanFractionEnd);

         mAero->AddAeroObject(aeroObjTop);
      }
      // Left fin

      angle_rad += UtMath::cPI_OVER_2;
      y = radius * cos(angle_rad);
      z = -radius * sin(angle_rad);
      UtVec3dX refPtLft(x, y, z);
      dihedralAngle_rad = angle_rad;

      // Add main ref pt
      refPtLft = refPtLft + aGeometryObject->GetRefPoint_ft();

      if (surfaceControlMotion == GeometrySurface::cControlMotionFixed)
      {
         AeroSurface* aeroObjLft = new AeroSurface(geometryObjectName, this);
         aeroObjLft->SetData(nameLft, refPtLft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                             sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                             invertVectors, aspectRatio, area_ft2, numAeroSections,
                             surfaceArea_ft2);

         aeroObjLft->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

         mAero->AddAeroObject(aeroObjLft);
      }
      else if (surfaceControlMotion == GeometrySurface::cControlMotionFullFlying)
      {
         AeroFullFlyingSurface* aeroObjLft = new AeroFullFlyingSurface(geometryObjectName, this);
         aeroObjLft->SetData(nameLft, refPtLft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                             sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                             invertVectors, aspectRatio, area_ft2, numAeroSections,
                             surfaceArea_ft2);

         aeroObjLft->SetAdditionalData(airfoilFilename, useExponentialAngleMapping,
                                       controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg,
                                       actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg);

         mAero->AddAeroObject(aeroObjLft);
      }
      else // if (surfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
      {
         AeroHingedSurface* aeroObjLft = new AeroHingedSurface(geometryObjectName, this);
         aeroObjLft->SetData(nameLft, refPtLft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                             sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                             invertVectors, aspectRatio, area_ft2, numAeroSections,
                             surfaceArea_ft2);

         aeroObjLft->GetControlSurface()->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                                            controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                                            actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg,
                                                            controlSurfaceChordFractionStart, controlSurfaceChordFractionEnd, 
                                                            controlSurfaceSpanFractionStart, controlSurfaceSpanFractionEnd);

         mAero->AddAeroObject(aeroObjLft);
      }
      // Bottom fin

      angle_rad += UtMath::cPI_OVER_2;
      y = radius * cos(angle_rad);
      z = -radius * sin(angle_rad);
      UtVec3dX refPtBottom(x, y, z);
      dihedralAngle_rad = angle_rad;

      // Add main ref pt
      refPtBottom = refPtBottom + aGeometryObject->GetRefPoint_ft();

      if (surfaceControlMotion == GeometrySurface::cControlMotionFixed)
      {
         AeroSurface* aeroObjBottom = new AeroSurface(geometryObjectName, this);
         aeroObjBottom->SetData(nameBottom, refPtBottom, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                 sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                                 invertVectors, aspectRatio, area_ft2, numAeroSections,
                                 surfaceArea_ft2);

         aeroObjBottom->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

         mAero->AddAeroObject(aeroObjBottom);
      }
      else if (surfaceControlMotion == GeometrySurface::cControlMotionFullFlying)
      {
         AeroFullFlyingSurface* aeroObjBottom = new AeroFullFlyingSurface(geometryObjectName, this);
         aeroObjBottom->SetData(nameBottom, refPtBottom, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                 sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                                 invertVectors, aspectRatio, area_ft2, numAeroSections,
                                 surfaceArea_ft2);

         aeroObjBottom->SetAdditionalData(airfoilFilename, useExponentialAngleMapping,
                                          controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg,
                                          actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg);

         mAero->AddAeroObject(aeroObjBottom);
      }
      else // if (surfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
      {
         AeroHingedSurface* aeroObjBottom = new AeroHingedSurface(geometryObjectName, this);
         aeroObjBottom->SetData(nameBottom, refPtBottom, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                 sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                                 invertVectors, aspectRatio, area_ft2, numAeroSections,
                                 surfaceArea_ft2);

         aeroObjBottom->GetControlSurface()->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                                               controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                                               actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg,
                                                               controlSurfaceChordFractionStart, controlSurfaceChordFractionEnd, 
                                                               controlSurfaceSpanFractionStart, controlSurfaceSpanFractionEnd);

         mAero->AddAeroObject(aeroObjBottom);
      }
   }
   else if (symmetryString == "X Pattern")
   {
      std::string name = geometryObjectName;

      std::string nameTopRgt = name + "_TopRight";
      nameTopRgt = mAero->MakeNameUnique(nameTopRgt);

      std::string nameTopLft = name + "_TopLeft";
      nameTopLft = mAero->MakeNameUnique(nameTopLft);

      std::string nameBottomLft = name + "_BottomLeft";
      nameBottomLft = mAero->MakeNameUnique(nameBottomLft);

      std::string nameBottomRgt = name + "_BottomRight";
      nameBottomRgt = mAero->MakeNameUnique(nameBottomRgt);

      // Top-Right fin

      bool invertVectors = false;
      double radius      = aGeometryObject->GetFinRefRadius_ft();
      double angle_rad   = UtMath::cPI_OVER_4;
      double x           = 0.0;
      double y           = radius * cos(angle_rad);
      double z           = -radius * sin(angle_rad);
      UtVec3dX refPtTopRgt(x, y, z);
      dihedralAngle_rad = angle_rad;

      // Add main ref pt
      refPtTopRgt = refPtTopRgt + aGeometryObject->GetRefPoint_ft();

      if (surfaceControlMotion == GeometrySurface::cControlMotionFixed)
      {
         AeroSurface* aeroObjTopRight = new AeroSurface(geometryObjectName, this);
         aeroObjTopRight->SetData(nameTopRgt, refPtTopRgt, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                  sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                                  invertVectors, aspectRatio, area_ft2, numAeroSections,
                                  surfaceArea_ft2);

         aeroObjTopRight->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

         mAero->AddAeroObject(aeroObjTopRight);
      }
      else if (surfaceControlMotion == GeometrySurface::cControlMotionFullFlying)
      {
         AeroFullFlyingSurface* aeroObjTopRight = new AeroFullFlyingSurface(geometryObjectName, this);
         aeroObjTopRight->SetData(nameTopRgt, refPtTopRgt, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                  sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                                  invertVectors, aspectRatio, area_ft2, numAeroSections,
                                  surfaceArea_ft2);

         aeroObjTopRight->SetAdditionalData(airfoilFilename, useExponentialAngleMapping,
                                            controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg,
                                            actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg);

         mAero->AddAeroObject(aeroObjTopRight);
      }
      else // if (surfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
      {
         AeroHingedSurface* aeroObjTopRight = new AeroHingedSurface(geometryObjectName, this);
         aeroObjTopRight->SetData(nameTopRgt, refPtTopRgt, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                  sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                                  invertVectors, aspectRatio, area_ft2, numAeroSections,
                                  surfaceArea_ft2);

         aeroObjTopRight->GetControlSurface()->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                                                 controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                                                 actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg,
                                                                 controlSurfaceChordFractionStart, controlSurfaceChordFractionEnd, 
                                                                 controlSurfaceSpanFractionStart, controlSurfaceSpanFractionEnd);

         mAero->AddAeroObject(aeroObjTopRight);
      }

      // Top-Left fin

      angle_rad += UtMath::cPI_OVER_2;
      y = radius * cos(angle_rad);
      z = -radius * sin(angle_rad);
      UtVec3dX refPtTopLft(x, y, z);
      dihedralAngle_rad = angle_rad;

      // Add main ref pt
      refPtTopLft = refPtTopLft + aGeometryObject->GetRefPoint_ft();

      if (surfaceControlMotion == GeometrySurface::cControlMotionFixed)
      {
         AeroSurface* aeroObjTopLft = new AeroSurface(geometryObjectName, this);
         aeroObjTopLft->SetData(nameTopLft, refPtTopLft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                                invertVectors, aspectRatio, area_ft2, numAeroSections,
                                surfaceArea_ft2);

         aeroObjTopLft->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

         mAero->AddAeroObject(aeroObjTopLft);
      }
      else if (surfaceControlMotion == GeometrySurface::cControlMotionFullFlying)
      {
         AeroFullFlyingSurface* aeroObjTopLft = new AeroFullFlyingSurface(geometryObjectName, this);
         aeroObjTopLft->SetData(nameTopLft, refPtTopLft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                                invertVectors, aspectRatio, area_ft2, numAeroSections,
                                surfaceArea_ft2);

         aeroObjTopLft->SetAdditionalData(airfoilFilename, useExponentialAngleMapping,
                                          controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg,
                                          actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg);

         mAero->AddAeroObject(aeroObjTopLft);
      }
      else // if (surfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
      {
         AeroHingedSurface* aeroObjTopLft = new AeroHingedSurface(geometryObjectName, this);
         aeroObjTopLft->SetData(nameTopLft, refPtTopLft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                                invertVectors, aspectRatio, area_ft2, numAeroSections,
                                surfaceArea_ft2);

         aeroObjTopLft->GetControlSurface()->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                                               controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                                               actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg,
                                                               controlSurfaceChordFractionStart, controlSurfaceChordFractionEnd, 
                                                               controlSurfaceSpanFractionStart, controlSurfaceSpanFractionEnd);

         mAero->AddAeroObject(aeroObjTopLft);
      }
      // Bottom-Left fin

      angle_rad += UtMath::cPI_OVER_2;
      y = radius * cos(angle_rad);
      z = -radius * sin(angle_rad);
      UtVec3dX refPtBottomLft(x, y, z);
      dihedralAngle_rad = angle_rad;

      // Add main ref pt
      refPtBottomLft = refPtBottomLft + aGeometryObject->GetRefPoint_ft();

      if (surfaceControlMotion == GeometrySurface::cControlMotionFixed)
      {
         AeroSurface* aeroObjBottomLeft = new AeroSurface(geometryObjectName, this);
         aeroObjBottomLeft->SetData(nameBottomLft, refPtBottomLft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                    sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                                    invertVectors, aspectRatio, area_ft2, numAeroSections,
                                    surfaceArea_ft2);

         aeroObjBottomLeft->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

         mAero->AddAeroObject(aeroObjBottomLeft);
      }
      else if (surfaceControlMotion == GeometrySurface::cControlMotionFullFlying)
      {
         AeroFullFlyingSurface* aeroObjBottomLeft = new AeroFullFlyingSurface(geometryObjectName, this);
         aeroObjBottomLeft->SetData(nameBottomLft, refPtBottomLft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                    sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                                    invertVectors, aspectRatio, area_ft2, numAeroSections,
                                    surfaceArea_ft2);

         aeroObjBottomLeft->SetAdditionalData(airfoilFilename, useExponentialAngleMapping,
                                              controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg,
                                              actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg);

         mAero->AddAeroObject(aeroObjBottomLeft);
      }
      else // if (surfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
      {
         AeroHingedSurface* aeroObjBottomLeft = new AeroHingedSurface(geometryObjectName, this);
         aeroObjBottomLeft->SetData(nameBottomLft, refPtBottomLft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                    sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                                    invertVectors, aspectRatio, area_ft2, numAeroSections,
                                    surfaceArea_ft2);

         aeroObjBottomLeft->GetControlSurface()->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                                                   controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                                                   actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg,
                                                                   controlSurfaceChordFractionStart, controlSurfaceChordFractionEnd, 
                                                                   controlSurfaceSpanFractionStart, controlSurfaceSpanFractionEnd);

         mAero->AddAeroObject(aeroObjBottomLeft);
      }

      // Bottom-Right fin

      angle_rad += UtMath::cPI_OVER_2;
      y = radius * cos(angle_rad);
      z = -radius * sin(angle_rad);
      UtVec3dX refPtBottomRgt(x, y, z);
      dihedralAngle_rad = angle_rad;

      // Add main ref pt
      refPtBottomRgt = refPtBottomRgt + aGeometryObject->GetRefPoint_ft();

      if (surfaceControlMotion == GeometrySurface::cControlMotionFixed)
      {
         AeroSurface* aeroObjBottomRight = new AeroSurface(geometryObjectName, this);
         aeroObjBottomRight->SetData(nameBottomRgt, refPtBottomRgt, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                     sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                                     invertVectors, aspectRatio, area_ft2, numAeroSections,
                                     surfaceArea_ft2);

         aeroObjBottomRight->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

         mAero->AddAeroObject(aeroObjBottomRight);
      }
      else if (surfaceControlMotion == GeometrySurface::cControlMotionFullFlying)
      {
         AeroFullFlyingSurface* aeroObjBottomRight = new AeroFullFlyingSurface(geometryObjectName, this);
         aeroObjBottomRight->SetData(nameBottomRgt, refPtBottomRgt, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                     sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                                     invertVectors, aspectRatio, area_ft2, numAeroSections,
                                     surfaceArea_ft2);

         aeroObjBottomRight->SetAdditionalData(airfoilFilename, useExponentialAngleMapping,
                                               controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg,
                                               actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg);

         mAero->AddAeroObject(aeroObjBottomRight);
      }
      else // if (surfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
      {
         AeroHingedSurface* aeroObjBottomRight = new AeroHingedSurface(geometryObjectName, this);
         aeroObjBottomRight->SetData(nameBottomRgt, refPtBottomRgt, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                                     sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                                     invertVectors, aspectRatio, area_ft2, numAeroSections,
                                     surfaceArea_ft2);

         aeroObjBottomRight->GetControlSurface()->SetAdditionalData(airfoilFilename, useExponentialAngleMapping, 
                                                                    controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg, 
                                                                    actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg,
                                                                    controlSurfaceChordFractionStart, controlSurfaceChordFractionEnd, 
                                                                    controlSurfaceSpanFractionStart, controlSurfaceSpanFractionEnd);

         mAero->AddAeroObject(aeroObjBottomRight);
      }
   }
   else // Single
   {
      std::string name = geometryObjectName;
      name = mAero->MakeNameUnique(name);

      // Note: Surfaces use validators to ensure right-side only (y >= 0)

      bool invertVectors = false;

      if (surfaceControlMotion == GeometrySurface::cControlMotionFixed)
      {
         AeroSurface* aeroObj = new AeroSurface(geometryObjectName, this);
         aeroObj->SetData(name, refPoint_ft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                          sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                          invertVectors, aspectRatio, area_ft2, numAeroSections,
                          surfaceArea_ft2);

         aeroObj->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

         mAero->AddAeroObject(aeroObj);
      }
      else if (surfaceControlMotion == GeometrySurface::cControlMotionFullFlying)
      {
         AeroFullFlyingSurface* aeroObj = new AeroFullFlyingSurface(geometryObjectName, this);
         aeroObj->SetData(name, refPoint_ft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                          sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                          invertVectors, aspectRatio, area_ft2, numAeroSections,
                          surfaceArea_ft2);

         aeroObj->SetAdditionalData(airfoilFilename, useExponentialAngleMapping,
                                    controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg,
                                    actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg);

         mAero->AddAeroObject(aeroObj);
      }
      else //if (surfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
      {
         AeroHingedSurface* aeroObj = new AeroHingedSurface(geometryObjectName, this);
         aeroObj->SetData(name, refPoint_ft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                          sweepAngle_rad, dihedralAngle_rad, incidenceAngle_rad, oswalds,
                          invertVectors, aspectRatio, area_ft2, numAeroSections,
                          surfaceArea_ft2);

         aeroObj->GetControlSurface()->SetAdditionalData(airfoilFilename, useExponentialAngleMapping,
                                                         controlSurfaceMinAngle_deg, controlSurfaceMaxAngle_deg,
                                                         actuatorMinRate_dps, actuatorMaxRate_dps, actuatorMinAngle_deg, actuatorMaxAngle_deg,
                                                         controlSurfaceChordFractionStart, controlSurfaceChordFractionEnd,
                                                         controlSurfaceSpanFractionStart, controlSurfaceSpanFractionEnd);

         mAero->AddAeroObject(aeroObj);
      }
   }
}

void Vehicle::GenerateAeroWing(GeometryWing* aGeometryObject)
{
   if (mAero == nullptr) { return; } // If we lack the vehicle aero, simply return

   std::string geometryObjectName = aGeometryObject->GetName();

   // Replace any spaces with underscores
   std::replace(geometryObjectName.begin(), geometryObjectName.end(), ' ', '_');

   UtVec3dX refPoint_ft      = aGeometryObject->GetRefPoint_ft();
   double span_ft            = aGeometryObject->GetSpan_ft();
   double rootChord_ft       = aGeometryObject->GetRootChord_ft();
   double tipChord_ft        = aGeometryObject->GetTipChord_ft();
   double sweepAngle_rad     = aGeometryObject->GetSweepAngle_deg() * UtMath::cRAD_PER_DEG;
   double dihedralAngle_rad  = aGeometryObject->GetDihedralAngle_deg() * UtMath::cRAD_PER_DEG;
   double incidenceAngle_rad = aGeometryObject->GetIncidenceAngle_deg() * UtMath::cRAD_PER_DEG;
   double thicknessRatio     = aGeometryObject->GetThicknessRatio();
   double oswalds            = aGeometryObject->GetOswaldsEfficiency();
   double aspectRatio        = aGeometryObject->GetAspectRatio();
   double area_ft2           = aGeometryObject->GetPlanformArea_ft2();
   int    numAeroSections    = aGeometryObject->GetNumberofAeroSections();

   // Get the surface area
   double surfaceArea_ft2 = aGeometryObject->GetMassArea_ft2();

   std::string airfoilFilename = aGeometryObject->GetAirfoil().toStdString();

   // Adjust for single side
   span_ft *= 0.5;
   area_ft2 *= 0.5;

   //AeroSurface::eSurfaceControlMotion aeroSurfaceControlMotion = AeroSurface::cControlMotionFixed;
   bool    useExponentialAngleMapping = false;

   bool    aileronsIsPresent                     = aGeometryObject->GetAileronsPresent();
   double  aileronsChordFractionStart            = aGeometryObject->GetAileronsChordFractionStart();
   double  aileronsChordFractionEnd              = aGeometryObject->GetAileronsChordFractionEnd();
   double  aileronsSpanFractionStart             = aGeometryObject->GetAileronsSpanFractionStart();
   double  aileronsSpanFractionEnd               = aGeometryObject->GetAileronsSpanFractionEnd();
   bool    aileronsUseExponentialAngleMapping    = aGeometryObject->GetAileronsUseExponentialAngleMapping();
   double  aileronsControlSurfaceMinAngle_deg    = aGeometryObject->GetAileronsControlSurfaceMinAngle_deg();
   double  aileronsControlSurfaceMaxAngle_deg    = aGeometryObject->GetAileronsControlSurfaceMaxAngle_deg();
   double  aileronsActuatorMinAngle_deg          = aGeometryObject->GetAileronsActuatorMinAngle_deg();
   double  aileronsActuatorMaxAngle_deg          = aGeometryObject->GetAileronsActuatorMaxAngle_deg();
   double  aileronsActuatorMinRate_dps           = aGeometryObject->GetAileronsActuatorMinRate_dps();
   double  aileronsActuatorMaxRate_dps           = aGeometryObject->GetAileronsActuatorMaxRate_dps();

   bool    drageronsIsPresent                    = aGeometryObject->GetDrageronsPresent();
   double  drageronsChordFractionStart           = aGeometryObject->GetDrageronsChordFractionStart();
   double  drageronsChordFractionEnd             = aGeometryObject->GetDrageronsChordFractionEnd();
   double  drageronsSpanFractionStart            = aGeometryObject->GetDrageronsSpanFractionStart();
   double  drageronsSpanFractionEnd              = aGeometryObject->GetDrageronsSpanFractionEnd();
   bool    drageronsUseExponentialAngleMapping   = aGeometryObject->GetDrageronsUseExponentialAngleMapping();
   double  drageronsControlSurfaceMinAngle_deg   = aGeometryObject->GetDrageronsControlSurfaceMinAngle_deg();
   double  drageronsControlSurfaceMaxAngle_deg   = aGeometryObject->GetDrageronsControlSurfaceMaxAngle_deg();
   double  drageronsActuatorMinAngle_deg         = aGeometryObject->GetDrageronsActuatorMinAngle_deg();
   double  drageronsActuatorMaxAngle_deg         = aGeometryObject->GetDrageronsActuatorMaxAngle_deg();
   double  drageronsActuatorMinRate_dps          = aGeometryObject->GetDrageronsActuatorMinRate_dps();
   double  drageronsActuatorMaxRate_dps          = aGeometryObject->GetDrageronsActuatorMaxRate_dps();

   bool    spoileronsIsPresent                   = aGeometryObject->GetSpoileronsPresent();
   double  spoileronsChordFractionStart          = aGeometryObject->GetSpoileronsChordFractionStart();
   double  spoileronsChordFractionEnd            = aGeometryObject->GetSpoileronsChordFractionEnd();
   double  spoileronsSpanFractionStart           = aGeometryObject->GetSpoileronsSpanFractionStart();
   double  spoileronsSpanFractionEnd             = aGeometryObject->GetSpoileronsSpanFractionEnd();
   bool    spoileronsUseExponentialAngleMapping  = aGeometryObject->GetSpoileronsUseExponentialAngleMapping();
   double  spoileronsControlSurfaceMinAngle_deg  = aGeometryObject->GetSpoileronsControlSurfaceMinAngle_deg();
   double  spoileronsControlSurfaceMaxAngle_deg  = aGeometryObject->GetSpoileronsControlSurfaceMaxAngle_deg();
   double  spoileronsActuatorMinAngle_deg        = aGeometryObject->GetSpoileronsActuatorMinAngle_deg();
   double  spoileronsActuatorMaxAngle_deg        = aGeometryObject->GetSpoileronsActuatorMaxAngle_deg();
   double  spoileronsActuatorMinRate_dps         = aGeometryObject->GetSpoileronsActuatorMinRate_dps();
   double  spoileronsActuatorMaxRate_dps         = aGeometryObject->GetSpoileronsActuatorMaxRate_dps();

   bool    spoilersIsPresent                     = aGeometryObject->GetSpoilersPresent();
   double  spoilersChordFractionStart            = aGeometryObject->GetSpoilersChordFractionStart();
   double  spoilersChordFractionEnd              = aGeometryObject->GetSpoilersChordFractionEnd();
   double  spoilersSpanFractionStart             = aGeometryObject->GetSpoilersSpanFractionStart();
   double  spoilersSpanFractionEnd               = aGeometryObject->GetSpoilersSpanFractionEnd();
   bool    spoilersUseExponentialAngleMapping    = aGeometryObject->GetSpoilersUseExponentialAngleMapping();
   double  spoilersControlSurfaceMinAngle_deg    = aGeometryObject->GetSpoilersControlSurfaceMinAngle_deg();
   double  spoilersControlSurfaceMaxAngle_deg    = aGeometryObject->GetSpoilersControlSurfaceMaxAngle_deg();
   double  spoilersActuatorMinAngle_deg          = aGeometryObject->GetSpoilersActuatorMinAngle_deg();
   double  spoilersActuatorMaxAngle_deg          = aGeometryObject->GetSpoilersActuatorMaxAngle_deg();
   double  spoilersActuatorMinRate_dps           = aGeometryObject->GetSpoilersActuatorMinRate_dps();
   double  spoilersActuatorMaxRate_dps           = aGeometryObject->GetSpoilersActuatorMaxRate_dps();

   bool    elevonsIsPresent                     = aGeometryObject->GetElevonsPresent();
   double  elevonsChordFractionStart            = aGeometryObject->GetElevonsChordFractionStart();
   double  elevonsChordFractionEnd              = aGeometryObject->GetElevonsChordFractionEnd();
   double  elevonsSpanFractionStart             = aGeometryObject->GetElevonsSpanFractionStart();
   double  elevonsSpanFractionEnd               = aGeometryObject->GetElevonsSpanFractionEnd();
   bool    elevonsUseExponentialAngleMapping    = aGeometryObject->GetElevonsUseExponentialAngleMapping();
   double  elevonsControlSurfaceMinAngle_deg    = aGeometryObject->GetElevonsControlSurfaceMinAngle_deg();
   double  elevonsControlSurfaceMaxAngle_deg    = aGeometryObject->GetElevonsControlSurfaceMaxAngle_deg();
   double  elevonsActuatorMinAngle_deg          = aGeometryObject->GetElevonsActuatorMinAngle_deg();
   double  elevonsActuatorMaxAngle_deg          = aGeometryObject->GetElevonsActuatorMaxAngle_deg();
   double  elevonsActuatorMinRate_dps           = aGeometryObject->GetElevonsActuatorMinRate_dps();
   double  elevonsActuatorMaxRate_dps           = aGeometryObject->GetElevonsActuatorMaxRate_dps();

   // Wings are always horizontal symmetry

   std::string name = geometryObjectName;

   std::string nameRgt = name + "_Right";
   nameRgt = mAero->MakeNameUnique(nameRgt);

   std::string nameLft = name + "_Left";
   nameLft = mAero->MakeNameUnique(nameLft);

   // Both "sides" of the wing use the same ref point

   double dihedralRgt_rad = dihedralAngle_rad;
   double incidenceAngleRgt_rad = incidenceAngle_rad;
   bool isInvertedRgt = false;

   double dihedralLft_rad = UtMath::cPI - dihedralRgt_rad;
   double incidenceAngleLft_rad = -incidenceAngle_rad;
   bool isInvertedLft = true;

   AeroWing* aeroObjRgt = new AeroWing(geometryObjectName, this);

   aeroObjRgt->SetData(nameRgt, refPoint_ft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                       sweepAngle_rad, dihedralRgt_rad, incidenceAngleRgt_rad, oswalds,
                       isInvertedRgt, aspectRatio, area_ft2, numAeroSections,
                       surfaceArea_ft2);

   aeroObjRgt->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

   aeroObjRgt->SetAileronsData(aileronsIsPresent,
                               "Aileron_Right",
                               aileronsChordFractionStart,
                               aileronsChordFractionEnd,
                               aileronsSpanFractionStart,
                               aileronsSpanFractionEnd,
                               aileronsUseExponentialAngleMapping,
                               aileronsControlSurfaceMinAngle_deg,
                               aileronsControlSurfaceMaxAngle_deg,
                               aileronsActuatorMinAngle_deg,
                               aileronsActuatorMaxAngle_deg,
                               aileronsActuatorMinRate_dps,
                               aileronsActuatorMaxRate_dps);

   aeroObjRgt->SetDrageronsData(drageronsIsPresent,
                                "Drageron_Right",
                                drageronsChordFractionStart,
                                drageronsChordFractionEnd,
                                drageronsSpanFractionStart,
                                drageronsSpanFractionEnd,
                                drageronsUseExponentialAngleMapping,
                                drageronsControlSurfaceMinAngle_deg,
                                drageronsControlSurfaceMaxAngle_deg,
                                drageronsActuatorMinAngle_deg,
                                drageronsActuatorMaxAngle_deg,
                                drageronsActuatorMinRate_dps,
                                drageronsActuatorMaxRate_dps);

   aeroObjRgt->SetSpoileronsData(spoileronsIsPresent,
                                 "Spoileron_Right",
                                 spoileronsChordFractionStart,
                                 spoileronsChordFractionEnd,
                                 spoileronsSpanFractionStart,
                                 spoileronsSpanFractionEnd,
                                 spoileronsUseExponentialAngleMapping,
                                 spoileronsControlSurfaceMinAngle_deg,
                                 spoileronsControlSurfaceMaxAngle_deg,
                                 spoileronsActuatorMinAngle_deg,
                                 spoileronsActuatorMaxAngle_deg,
                                 spoileronsActuatorMinRate_dps,
                                 spoileronsActuatorMaxRate_dps);

   aeroObjRgt->SetSpoilersData(spoilersIsPresent,
                               "Spoiler_Right",
                               spoilersChordFractionStart,
                               spoilersChordFractionEnd,
                               spoilersSpanFractionStart,
                               spoilersSpanFractionEnd,
                               spoilersUseExponentialAngleMapping,
                               spoilersControlSurfaceMinAngle_deg,
                               spoilersControlSurfaceMaxAngle_deg,
                               spoilersActuatorMinAngle_deg,
                               spoilersActuatorMaxAngle_deg,
                               spoilersActuatorMinRate_dps,
                               spoilersActuatorMaxRate_dps);

   aeroObjRgt->SetElevonsData(elevonsIsPresent,
                              "Elevon_Right",
                              elevonsChordFractionStart,
                              elevonsChordFractionEnd,
                              elevonsSpanFractionStart,
                              elevonsSpanFractionEnd,
                              elevonsUseExponentialAngleMapping,
                              elevonsControlSurfaceMinAngle_deg,
                              elevonsControlSurfaceMaxAngle_deg,
                              elevonsActuatorMinAngle_deg,
                              elevonsActuatorMaxAngle_deg,
                              elevonsActuatorMinRate_dps,
                              elevonsActuatorMaxRate_dps);

   mAero->AddAeroObject(aeroObjRgt);

   AeroWing* aeroObjLft = new AeroWing(geometryObjectName, this);

   aeroObjLft->SetData(nameLft, refPoint_ft, span_ft, rootChord_ft, tipChord_ft, thicknessRatio,
                       sweepAngle_rad, dihedralLft_rad, incidenceAngleLft_rad, oswalds,
                       isInvertedLft, aspectRatio, area_ft2, numAeroSections,
                       surfaceArea_ft2);

   aeroObjLft->SetAdditionalData(airfoilFilename, useExponentialAngleMapping);

   aeroObjLft->SetAileronsData(aileronsIsPresent,
                               "Aileron_Left",
                               aileronsChordFractionStart,
                               aileronsChordFractionEnd,
                               aileronsSpanFractionStart,
                               aileronsSpanFractionEnd,
                               aileronsUseExponentialAngleMapping,
                               aileronsControlSurfaceMinAngle_deg,
                               aileronsControlSurfaceMaxAngle_deg,
                               aileronsActuatorMinAngle_deg,
                               aileronsActuatorMaxAngle_deg,
                               aileronsActuatorMinRate_dps,
                               aileronsActuatorMaxRate_dps);

   aeroObjLft->SetDrageronsData(drageronsIsPresent,
                                "Drageron_Left",
                                drageronsChordFractionStart,
                                drageronsChordFractionEnd,
                                drageronsSpanFractionStart,
                                drageronsSpanFractionEnd,
                                drageronsUseExponentialAngleMapping,
                                drageronsControlSurfaceMinAngle_deg,
                                drageronsControlSurfaceMaxAngle_deg,
                                drageronsActuatorMinAngle_deg,
                                drageronsActuatorMaxAngle_deg,
                                drageronsActuatorMinRate_dps,
                                drageronsActuatorMaxRate_dps);

   aeroObjLft->SetSpoileronsData(spoileronsIsPresent,
                                 "Spoileron_Left",
                                 spoileronsChordFractionStart,
                                 spoileronsChordFractionEnd,
                                 spoileronsSpanFractionStart,
                                 spoileronsSpanFractionEnd,
                                 spoileronsUseExponentialAngleMapping,
                                 spoileronsControlSurfaceMinAngle_deg,
                                 spoileronsControlSurfaceMaxAngle_deg,
                                 spoileronsActuatorMinAngle_deg,
                                 spoileronsActuatorMaxAngle_deg,
                                 spoileronsActuatorMinRate_dps,
                                 spoileronsActuatorMaxRate_dps);

   aeroObjLft->SetSpoilersData(spoilersIsPresent,
                               "Spoiler_Left",
                               spoilersChordFractionStart,
                               spoilersChordFractionEnd,
                               spoilersSpanFractionStart,
                               spoilersSpanFractionEnd,
                               spoilersUseExponentialAngleMapping,
                               spoilersControlSurfaceMinAngle_deg,
                               spoilersControlSurfaceMaxAngle_deg,
                               spoilersActuatorMinAngle_deg,
                               spoilersActuatorMaxAngle_deg,
                               spoilersActuatorMinRate_dps,
                               spoilersActuatorMaxRate_dps);

   aeroObjLft->SetElevonsData(elevonsIsPresent,
                              "Elevon_Left",
                              elevonsChordFractionStart,
                              elevonsChordFractionEnd,
                              elevonsSpanFractionStart,
                              elevonsSpanFractionEnd,
                              elevonsUseExponentialAngleMapping,
                              elevonsControlSurfaceMinAngle_deg,
                              elevonsControlSurfaceMaxAngle_deg,
                              elevonsActuatorMinAngle_deg,
                              elevonsActuatorMaxAngle_deg,
                              elevonsActuatorMinRate_dps,
                              elevonsActuatorMaxRate_dps);

   mAero->AddAeroObject(aeroObjLft);
}

void Vehicle::GenerateAeroEngine(GeometryEngine* aGeometryObject)
{
   std::string modelName          = aGeometryObject->GetEngineModel();
   std::string name               = aGeometryObject->GetName();
   UtVec3dX refPoint_ft           = aGeometryObject->GetRefPoint_ft();
   double yaw_deg                 = aGeometryObject->GetYaw_deg();
   double pitch_deg               = aGeometryObject->GetPitch_deg();
   double roll_deg                = aGeometryObject->GetRoll_deg();
   double length_ft               = aGeometryObject->GetLength_ft();
   double diameter_ft             = aGeometryObject->GetDiameter_ft();
   double mass_lbs                = aGeometryObject->GetMass_lbs();
   double thrustOffset_ft         = aGeometryObject->GetThrustOffset_ft();
   double ignitionDelay_sec       = aGeometryObject->GetIgnitionDelay_sec();
   bool   latchFuelInjection      = aGeometryObject->GetLatchFuelInjection();
   bool   useProportionalThrottle = aGeometryObject->UseProportionalThrottle();
   double minProportionalThrottle = aGeometryObject->GetMinProportionalThrottle();

   GeometryEngine::EngineType engineCategory = aGeometryObject->GetEngineType();

   if (aGeometryObject->IsSymmetrical())
   {
      bool rightEngine = false;
      if (refPoint_ft.Y() > 0.0)
      {
         rightEngine = true;
      }

      std::string postfix = "_Left";
      if (rightEngine)
      {
         postfix = "_Right";
      }

      std::string name1 = name + postfix;
      CreateAeroEngine(name1, modelName, engineCategory, refPoint_ft,
                       yaw_deg, pitch_deg, roll_deg, length_ft, diameter_ft, mass_lbs, thrustOffset_ft, 
                       ignitionDelay_sec, latchFuelInjection, useProportionalThrottle, minProportionalThrottle);

      // Adjust ref pt
      double currentBodyY = refPoint_ft.Y();
      double delta = currentBodyY - aGeometryObject->GetSymmetryHorizontalY();
      double otherBodyY = currentBodyY - 2.0 * delta;
      UtVec3dX otherEngineRefPt(refPoint_ft.X(), otherBodyY, refPoint_ft.Z());

      // Adjust yaw and roll
      yaw_deg = -yaw_deg;
      roll_deg = -roll_deg;

      // Adjust postfix
      if (rightEngine) { postfix = "_Left"; }
      else             { postfix = "_Right"; }

      std::string name2 = name + postfix;
      CreateAeroEngine(name2, modelName, engineCategory, otherEngineRefPt,
                       yaw_deg, pitch_deg, roll_deg, length_ft, diameter_ft, mass_lbs, thrustOffset_ft,
                       ignitionDelay_sec, latchFuelInjection, useProportionalThrottle, minProportionalThrottle);
   }
   else
   {
      // Single engine
      CreateAeroEngine(name, modelName, engineCategory, refPoint_ft,
                       yaw_deg, pitch_deg, roll_deg, length_ft, diameter_ft, mass_lbs, thrustOffset_ft,
                       ignitionDelay_sec, latchFuelInjection, useProportionalThrottle, minProportionalThrottle);
   }
}

void Vehicle::CreateAeroEngine(const std::string&         aEngineInstanceName,
                               const std::string&         aModelName,
                               GeometryEngine::EngineType aEngineType,
                               UtVec3dX                   aRefPoint_ft,
                               double                     aYaw_deg,
                               double                     aPitch_deg,
                               double                     aRoll_deg,
                               double                     aLength_ft,
                               double                     aDiameter_ft,
                               double                     aMass_lbs,
                               double                     aThrustOffset_ft,
                               double                     aIgnitionDelay_sec,
                               bool                       aLatchFuelInjection,
                               bool                       aUseProportionalThrottle,
                               double                     aMinProportionalThrottle)
{
   AeroJetEngine* jetEngine = nullptr;
   AeroSolidRocketEngine* solidPropellantRocketEngine = nullptr;
   AeroRamjetEngine* ramjetEngine = nullptr;
   AeroLiquidRocketEngine* liquidRocketEngine = nullptr;

   switch (aEngineType)
   {
      case GeometryEngine::EngineType::cJET:
         jetEngine = new AeroJetEngine(aModelName);
         jetEngine->SetData(aEngineInstanceName, aEngineType, aRefPoint_ft,
                            aYaw_deg, aPitch_deg, aRoll_deg, aLength_ft, aDiameter_ft, 
                            aMass_lbs, aThrustOffset_ft, aIgnitionDelay_sec);
         mPropulsion.AddEngine(jetEngine);
         break;
      case GeometryEngine::EngineType::cRAMJET:
         ramjetEngine = new AeroRamjetEngine(aModelName);
         ramjetEngine->SetData(aEngineInstanceName, aEngineType, aRefPoint_ft,
                               aYaw_deg, aPitch_deg, aRoll_deg, aLength_ft, aDiameter_ft, 
                               aMass_lbs, aThrustOffset_ft, aIgnitionDelay_sec,
                               aLatchFuelInjection, aUseProportionalThrottle, aMinProportionalThrottle);
         mPropulsion.AddEngine(ramjetEngine);
         break;
      case GeometryEngine::EngineType::cSOLIDROCKET:
         solidPropellantRocketEngine = new AeroSolidRocketEngine(aModelName);
         solidPropellantRocketEngine->SetData(aEngineInstanceName, aEngineType, aRefPoint_ft,
                                              aYaw_deg, aPitch_deg, aRoll_deg, aLength_ft, aDiameter_ft, 
                                              aMass_lbs, aThrustOffset_ft, aIgnitionDelay_sec);
         mPropulsion.AddEngine(solidPropellantRocketEngine);
         break;
      case GeometryEngine::EngineType::cLIQUIDROCKET:
         liquidRocketEngine = new AeroLiquidRocketEngine(aModelName);
         liquidRocketEngine->SetData(aEngineInstanceName, aEngineType, aRefPoint_ft,
                                     aYaw_deg, aPitch_deg, aRoll_deg, aLength_ft, aDiameter_ft, 
                                     aMass_lbs, aThrustOffset_ft, aIgnitionDelay_sec);
         mPropulsion.AddEngine(liquidRocketEngine);
         break;
   }
}

void Vehicle::CalculateVehicleAerodynamics()
{
   if (mAero == nullptr) { return; } // If we lack the vehicle aero, simply return

   GenerateAeroDataFromGeometry();
   SetTestingDataAndAeroTableLimits();
   mAero->CalculateFullVehicleAerodynamics();
}

void Vehicle::SetVehicleAlphaBeta(double aAlpha_deg, double aBeta_deg)
{
   mVehicleAlpha_deg = aAlpha_deg;
   mVehicleBeta_deg  = aBeta_deg;

   if (mAero == nullptr) { return; } // If we lack the vehicle aero, simply return

   mAero->SetVehicleAlphaBeta(aAlpha_deg, aBeta_deg);
}

void Vehicle::GetVehicleAlphaBeta(double& aAlpha_deg, double& aBeta_deg)
{
   aAlpha_deg = mVehicleAlpha_deg;
   aBeta_deg  = mVehicleBeta_deg;
}

void Vehicle::SetVehicleYawPitchRollRates(double aYawRate_dps, double aPitchRate_dps, double aRollRate_dps)
{
   mVehicleYawRate_dps   = aYawRate_dps;
   mVehiclePitchRate_dps = aPitchRate_dps;
   mVehicleRollRate_dps  = aRollRate_dps;

   if (mAero == nullptr) { return; } // If we lack the vehicle aero, simply return

   mAero->SetVehicleYawPitchRollRates(aYawRate_dps, aPitchRate_dps, aRollRate_dps);
}

void Vehicle::GetVehicleYawPitchRollRates(double& aYawRate_dps, double& aPitchRate_dps, double& aRollRate_dps)
{
   aYawRate_dps   = mVehicleYawRate_dps;
   aPitchRate_dps = mVehiclePitchRate_dps;
   aRollRate_dps  = mVehicleRollRate_dps;
}

void Vehicle::SetVehicleSpeedParameters(double aSpeed_fps,
                                        double aMach,
                                        double aKTAS,
                                        double aKCAS,
                                        double aDynamicPressure_psf,
                                        double aAirDensity_slugs_ft3)
{
   mVehicleSpeed_fps           = aSpeed_fps;
   mVehicleMach                = aMach;
   mVehicleKTAS                = aKTAS;
   mVehicleKCAS                = aKCAS;
   mVehicleDynamicPressure_psf = aDynamicPressure_psf;
   mAirDensity_slugs_ft3       = aAirDensity_slugs_ft3;

   if (mAero == nullptr) { return; } // If we lack the vehicle aero, simply return

   mAero->SetVehicleSpeedParameters(aSpeed_fps, aMach, aKTAS, aKCAS,
                                    aDynamicPressure_psf, aAirDensity_slugs_ft3);
}

void Vehicle::GetVehicleSpeedParameters(double& aSpeed_fps,
                                        double& aMach,
                                        double& aKTAS,
                                        double& aKCAS,
                                        double& aDynamicPressure_psf,
                                        double& aAirDensity_slugs_ft3)
{
   aSpeed_fps            = mVehicleSpeed_fps;
   aMach                 = mVehicleMach;
   aKTAS                 = mVehicleKTAS;
   aKCAS                 = mVehicleKCAS;
   aDynamicPressure_psf  = mVehicleDynamicPressure_psf;
   aAirDensity_slugs_ft3 = mAirDensity_slugs_ft3;
}

void Vehicle::CalculateAeroAttitudeVectors()
{
   if (mAero == nullptr) { return; } // If we lack the vehicle aero, simply return

   mAero->CalculateAeroAttitudeVectors();
}

void Vehicle::Update()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Designer::GeometryWidget* geometry = mw.GetMoverCreatorWidget()->GetGeometryWidget();
   if (geometry != nullptr)
   {
      // Update mass and inertia properties
      Designer::GeometryGLWidget* glObj = geometry->GetGeometryGLWidget();
      glObj->Update();
   }
}

void Vehicle::CalcMassAndInertia()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Designer::GeometryWidget* geometry = mw.GetMoverCreatorWidget()->GetGeometryWidget();
   if (geometry != nullptr)
   {
      GeometryMassProperties* massProperties = geometry->GetGeometryMassProperties();
      massProperties->CalcMassAndInertiaProperties();
   }
}

bool Vehicle::DebugMode()
{
   return mDebugModeActive;
}

void Vehicle::SetDebugMode(bool aDebugActive)
{
   mDebugModeActive = aDebugActive;
}

QStringList Vehicle::Audit()
{
   QStringList errors;
   errors.append(mGeometry.Audit());

   if (mAero == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle does not include a valid aero component!");
      return errors;
   }

   errors.append(mAero->Audit());

   return errors;
}

} // namespace Designer
