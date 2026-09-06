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

#include "P6DofAtmosphere.hpp"

#include "UtAirspeed.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtVec3dX.hpp"

P6DofAtmosphere::P6DofAtmosphere()
   : mDensityAltitudeTablePtr(nullptr)
   , mPressureAltitudeTablePtr(nullptr)
   , mSpeedOfSoundAltitudeTablePtr(nullptr)
   , mTemperatureAltitudeTablePtr(nullptr)
   , mContrailingConditionsExist(false)
   , mMinContrailingAltitude_ft(0.0)
   , mMaxContrailingAltitude_ft(0.0)
{
   ut::log::warning() << "P6DofAtmosphere constructed without an atmosphere data file.";
}

P6DofAtmosphere::P6DofAtmosphere(const std::string& aFilename)
   : mDensityAltitudeTablePtr(nullptr)
   , mPressureAltitudeTablePtr(nullptr)
   , mSpeedOfSoundAltitudeTablePtr(nullptr)
   , mTemperatureAltitudeTablePtr(nullptr)
   , mContrailingConditionsExist(false)
   , mMinContrailingAltitude_ft(0.0)
   , mMaxContrailingAltitude_ft(0.0)
{
   try
   {
      UtInput inputObj;
      inputObj.PushInput(ut::make_unique<UtInputFile>(aFilename));
      ProcessInput(inputObj);
   }
   catch (UtInput::ExceptionBase& exception)
   {
      auto out = ut::log::error() << "Caught exception while initializing P6DofAtmosphere.";
      out.AddNote() << "Message: " << exception.GetMessage();
      out.AddNote() << "Location: " << exception.GetLocation();
   }
   catch (UtInputFile::OpenError&)
   {
      auto out = ut::log::error() << "Unable to open P6Dof atmosphere input file.";
      out.AddNote() << "File: " << aFilename;
   }
   catch (std::exception& e)
   {
      auto out = ut::log::error() << "Exception in P6Dof atmosphere input file.";
      out.AddNote() << "File: " << aFilename;
      out.AddNote() << "Type: " << ut::TypeNameOf(e);
      out.AddNote() << "What: " << e.what();
   }
}

P6DofAtmosphere* P6DofAtmosphere::Clone() const
{
   return new P6DofAtmosphere(*this);
}

double P6DofAtmosphere::SpeedOfSoundAtAltitude_fps(double aAltitude_Ft) const
{
   double c = 0.0;

   if (mSpeedOfSoundAltitudeTablePtr != nullptr)
   {
      c = mSpeedOfSoundAltitudeTablePtr->Lookup(aAltitude_Ft);
   }
   else
   {
      c = UtAirspeed::GetSpeedOfSoundAtAltitude_fps(aAltitude_Ft);
   }
   return c;
}

double P6DofAtmosphere::CalcMachAtAltitude(double aAltitude_Ft, double aSpeed_fps) const
{
   double c    = SpeedOfSoundAtAltitude_fps(aAltitude_Ft);
   double mach = 0.0;

   if (c < 0.001)
   {
      mach = 0.0;
   }
   else
   {
      mach = aSpeed_fps / c;
   }

   return mach;
}

double P6DofAtmosphere::AirDensityAtAltitude(double aAltitude_Ft) const
{
   if (aAltitude_Ft < 0.0)
   {
      aAltitude_Ft = 0.0;
   }

   // if a density table was specified
   if (mDensityAltitudeTablePtr != nullptr)
   {
      return mDensityAltitudeTablePtr->Lookup(aAltitude_Ft);
   }
   else // calculate a value
   {
      double val       = pow(aAltitude_Ft, 1.15);
      double rhoFactor = exp(-7.4E-06 * val);
      double rho       = 2.3769E-03 * rhoFactor;

      // For now, we eliminate any atmosphere above 400kft
      if (aAltitude_Ft > 400000.0)
      {
         rho = 0.0;
      }

      return rho;
   }
}

double P6DofAtmosphere::StaticPressureAtAltitude(double aAltitude_ft) const
{
   if (mPressureAltitudeTablePtr != nullptr)
   {
      return mPressureAltitudeTablePtr->Lookup(aAltitude_ft);
   }
   else
   {
      return 0.0;
   }
}

double P6DofAtmosphere::TemperatureAtAltitude_F(double aAltitude_ft) const
{
   return TemperatureAtAltitude_R(aAltitude_ft) - 459.67;
}

double P6DofAtmosphere::TemperatureAtAltitude_R(double aAltitude_ft) const
{
   if (mTemperatureAltitudeTablePtr != nullptr)
   {
      return mTemperatureAltitudeTablePtr->Lookup(aAltitude_ft);
   }
   else
   {
      return 0.0;
   }
}

double P6DofAtmosphere::DensityRatioAtAltitude(double aAltitude_ft) const
{
   double seaLevel   = AirDensityAtAltitude(0.0);
   double atAltitude = AirDensityAtAltitude(aAltitude_ft);

   if (seaLevel == 0.0)
   {
      return 1.0; // assume constant atmosphere
   }
   return atAltitude / seaLevel;
}

double P6DofAtmosphere::StaticPressureRatioAtAltitude(double aAltitude_ft) const
{
   double seaLevel   = StaticPressureAtAltitude(0.0);
   double atAltitude = StaticPressureAtAltitude(aAltitude_ft);

   if (seaLevel == 0.0)
   {
      return 1.0; // assume constant atmosphere
   }
   return atAltitude / seaLevel;
}

double P6DofAtmosphere::TemperatureRatioAtAltitude(double aAltitude_ft) const
{
   double seaLevel   = TemperatureAtAltitude_F(0.0);
   double atAltitude = TemperatureAtAltitude_F(aAltitude_ft);

   if (seaLevel == 0.0)
   {
      return 1.0; // assume constant atmosphere
   }
   return atAltitude / seaLevel;
}

void P6DofAtmosphere::GetAeroParametersAtAltitude(double  aAltitude_Ft,
                                                  double  aSpeed_fps,
                                                  double& aAirDensity,
                                                  double& aDynamicPressure_psf,
                                                  double& aStaticPressure_psf,
                                                  double& aMach) const
{
   if (aAltitude_Ft < 0.0)
   {
      aAltitude_Ft = 0.0;
   }

   aAirDensity = AirDensityAtAltitude(aAltitude_Ft);

   // Calc dynamic pressure
   aDynamicPressure_psf = 0.5 * aAirDensity * (aSpeed_fps * aSpeed_fps);

   // Calc static pressure
   aStaticPressure_psf = StaticPressureAtAltitude(aAltitude_Ft);

   // Calc mach
   aMach = CalcMachAtAltitude(aAltitude_Ft, aSpeed_fps);
}

double P6DofAtmosphere::CalcKtasFromKcas(double aAltitude_ft, double aSpeed_kcas) const
{
   double speed_fps = CalcFpsFromKcas(aAltitude_ft, aSpeed_kcas);

   return CalcKtasFromFps(speed_fps);
}

double P6DofAtmosphere::CalcKtasFromMach(double aAltitude_ft, double aSpeed_mach) const
{
   double speed_fps = CalcFpsFromMach(aAltitude_ft, aSpeed_mach);

   return CalcKtasFromFps(speed_fps);
}

double P6DofAtmosphere::CalcKtasFromFps(double aSpeed_fps) const
{
   return UtMath::cNM_PER_FT * aSpeed_fps * 3600.0;
}

double P6DofAtmosphere::CalcKcasFromKtas(double aAltitude_ft, double aSpeed_ktas) const
{
   double speed_fps = CalcFpsFromKtas(aSpeed_ktas);

   return CalcKcasFromFps(aAltitude_ft, speed_fps);
}

double P6DofAtmosphere::CalcKcasFromMach(double aAltitude_ft, double aSpeed_mach) const
{
   double speed_fps = CalcFpsFromMach(aAltitude_ft, aSpeed_mach);

   return CalcKcasFromFps(aAltitude_ft, speed_fps);
}

double P6DofAtmosphere::CalcKcasFromFps(double aAltitude_ft, double aSpeed_fps) const
{
   if (aAltitude_ft < 0)
   {
      aAltitude_ft = 0;
   }
   double rho_SL   = AirDensityAtAltitude(0.0);
   double rho      = AirDensityAtAltitude(aAltitude_ft);
   double actual_q = 0.5 * rho * (aSpeed_fps * aSpeed_fps);

   // actual_q = 0.5*rho_alt*(aTrueSpeed_fps)^2
   // Indicated/calibrated speed uses the actual dynamic pressure (q) but calculates a
   // speed based on the std seal level air density
   // So, 0.5*rho_SL*(aCalSpeed_fps)^2 = actual_q

   double tempSpeed_fps = sqrt(actual_q / (0.5 * rho_SL));
   double speed_kcas    = tempSpeed_fps * (3600.0 * UtMath::cNM_PER_FT);

   return speed_kcas;
}

double P6DofAtmosphere::CalcMachFromKcas(double aAltitude_ft, double aSpeed_kcas) const
{
   double speed_fps = CalcFpsFromKcas(aAltitude_ft, aSpeed_kcas);
   return CalcMachFromFps(aAltitude_ft, speed_fps);
}

double P6DofAtmosphere::CalcMachFromKtas(double aAltitude_ft, double aSpeed_ktas) const
{
   double speed_fps = CalcFpsFromKtas(aSpeed_ktas);

   return CalcMachFromFps(aAltitude_ft, speed_fps);
}

double P6DofAtmosphere::CalcMachFromFps(double aAltitude_ft, double aSpeed_fps) const
{
   return CalcMachAtAltitude(aAltitude_ft, aSpeed_fps);
}

double P6DofAtmosphere::CalcFpsFromKcas(double aAltitude_ft, double aSpeed_kcas) const
{
   if (aAltitude_ft < 0)
   {
      aAltitude_ft = 0;
   }
   double rho_SL  = AirDensityAtAltitude(0.0);
   double rho_alt = AirDensityAtAltitude(aAltitude_ft);

   // Get calibrated speed in fps
   double calSpeed_fps = aSpeed_kcas / (3600.0 * UtMath::cNM_PER_FT);

   // Now determine the "calibrated" dynamic pressure
   double cal_q = 0.5 * rho_SL * (calSpeed_fps * calSpeed_fps);

   // Since the dynamic pressure is the same between calibrated and true,
   // we get:  0.5*rho_alt*(trueSpeed_fps^2) = cal_q

   double trueSpeed_fps = sqrt(cal_q / (0.5 * rho_alt));

   return trueSpeed_fps;
}

double P6DofAtmosphere::CalcFpsFromKtas(double aSpeed_ktas) const
{
   return UtMath::cFT_PER_NM * (aSpeed_ktas / 3600.0);
}

double P6DofAtmosphere::CalcFpsFromMach(double aAltitude_ft, double aSpeed_mach) const
{
   return aSpeed_mach * SpeedOfSoundAtAltitude_fps(aAltitude_ft);
}

double P6DofAtmosphere::CalcDynamicPressure_lbft2(double aAltitude_ft, double aSpeed_fps) const
{
   double rho = AirDensityAtAltitude(aAltitude_ft);

   double q = 0.5 * rho * (aSpeed_fps * aSpeed_fps);

   return q;
}

double P6DofAtmosphere::CalcFpsFromAltitudeDynamicPressure(double aAltitude_ft, double aPressure_lbft2) const
{
   double rho = AirDensityAtAltitude(aAltitude_ft);

   double speed_fps = sqrt(aPressure_lbft2 / (.5 * rho));

   return speed_fps;
}

bool P6DofAtmosphere::ContrailAltitudeBand_ft(double& aLowAltitude_ft, double& aHighAltitude_ft) const
{
   if (mContrailingConditionsExist)
   {
      aLowAltitude_ft  = mMinContrailingAltitude_ft;
      aHighAltitude_ft = mMaxContrailingAltitude_ft;
      return true;
   }
   return false;
}

bool P6DofAtmosphere::WithinContrailAltitudeBand(double aAltitude_ft) const
{
   if (mContrailingConditionsExist)
   {
      if ((aAltitude_ft >= mMinContrailingAltitude_ft) && (aAltitude_ft <= mMaxContrailingAltitude_ft))
      {
         return true;
      }
   }
   return false;
}

bool P6DofAtmosphere::ProcessInput(UtInput& aInput)
{
   const std::string& command = aInput.GetCommand();

   bool minContrailingAltSet   = false;
   bool maxContrailingAltSet   = false;
   mContrailingConditionsExist = false;

   if (command == "p6dof_atmosphere")
   {
      UtInputBlock block(aInput, "end_p6dof_atmosphere");

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();
         if (localCommand == "density_altitude_table")
         {
            mDensityAltitudeTablePtr = new UtTable::Curve;
            mDensityAltitudeTablePtr->ProcessInput(aInput,
                                                   UtInput::cNON_DIMENSIONAL,
                                                   "altitude_ft",
                                                   UtTable::NoCheck(),
                                                   UtInput::cNON_DIMENSIONAL,
                                                   "density_slug_ft3",
                                                   UtTable::NoCheck());
         }
         else if (localCommand == "pressure_altitude_table")
         {
            mPressureAltitudeTablePtr = new UtTable::Curve;
            mPressureAltitudeTablePtr->ProcessInput(aInput,
                                                    UtInput::cNON_DIMENSIONAL,
                                                    "altitude_ft",
                                                    UtTable::NoCheck(),
                                                    UtInput::cNON_DIMENSIONAL,
                                                    "pressure_lbs_ft2",
                                                    UtTable::NoCheck());
         }
         else if (localCommand == "temperature_altitude_table")
         {
            mTemperatureAltitudeTablePtr = new UtTable::Curve;
            mTemperatureAltitudeTablePtr->ProcessInput(aInput,
                                                       UtInput::cNON_DIMENSIONAL,
                                                       "altitude_ft",
                                                       UtTable::NoCheck(),
                                                       UtInput::cNON_DIMENSIONAL,
                                                       "rankine_deg",
                                                       UtTable::NoCheck());
         }
         else if (localCommand == "sonic_speed_altitude_table")
         {
            mSpeedOfSoundAltitudeTablePtr = new UtTable::Curve;
            mSpeedOfSoundAltitudeTablePtr->ProcessInput(aInput,
                                                        UtInput::cNON_DIMENSIONAL,
                                                        "altitude_ft",
                                                        UtTable::NoCheck(),
                                                        UtInput::cNON_DIMENSIONAL,
                                                        "sonic_speed_ft_s",
                                                        UtTable::NoCheck());
         }
         else if (localCommand == "contrail_min_altitude")
         {
            double alt_m;
            aInput.ReadValueOfType(alt_m, UtInput::cLENGTH);
            mMinContrailingAltitude_ft = alt_m * UtMath::cFT_PER_M;
            minContrailingAltSet       = true;
         }
         else if (localCommand == "contrail_max_altitude")
         {
            double alt_m;
            aInput.ReadValueOfType(alt_m, UtInput::cLENGTH);
            mMaxContrailingAltitude_ft = alt_m * UtMath::cFT_PER_M;
            maxContrailingAltSet       = true;
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofAeroCoreObject::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   // Verify correct contrail band parameters
   if (minContrailingAltSet && maxContrailingAltSet)
   {
      if (mMaxContrailingAltitude_ft > mMinContrailingAltitude_ft)
      {
         // Contrailing altitudes are set properly
         mContrailingConditionsExist = true;
      }
      else
      {
         auto out = ut::log::warning() << "Min contrailing altitude is higher than the max contrailing altitude within "
                                          "P6DofAeroCoreObject::ProcessInput().";
         out.AddNote() << "Min Alt: " << mMinContrailingAltitude_ft << " ft";
         out.AddNote() << "Max Alt: " << mMaxContrailingAltitude_ft << " ft";
         out.AddNote() << "Disabling contrailing effects.";
      }
   }
   else if (minContrailingAltSet)
   {
      auto out = ut::log::warning() << "Min contrailing altitude is set, but no max contrailing altitude is set in "
                                       "P6DofAeroCoreObject::ProcessInput().";
      out.AddNote() << "Disabling contrailing effects.";
   }
   else if (maxContrailingAltSet)
   {
      auto out = ut::log::warning() << "Max contrailing altitude is set, but no min contrailing altitude is set in "
                                       "P6DofAeroCoreObject::ProcessInput().";
      out.AddNote() << "Disabling contrailing effects.";
   }

   return true;
}
