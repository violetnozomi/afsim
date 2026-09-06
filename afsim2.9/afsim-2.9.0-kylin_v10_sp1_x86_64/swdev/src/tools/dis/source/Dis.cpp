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

#include "Dis.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenError.hpp"
#include "UtLog.hpp"


DisEnum8   Dis::mProtocolVersion(DisEnum::Pdu::Version::IEEE_1278_1_1995);
DisUint8   Dis::mExercise(1);
DisUint16  Dis::mApplication(1);
DisUint16  Dis::mSite(1);
DisFloat32 Dis::mHrtBeatTimer(5.0F);        // Seconds
DisFloat32 Dis::mHrtBeatMoveTimer(2.0F);    // Seconds
DisFloat32 Dis::mHrtBeatDE_FireTimer(0.5F); // Seconds
DisFloat32 Dis::mHrtBeatMplier(2.4F);
DisFloat32 Dis::mDRAOrientThrsh(3.0F);          // Degrees
DisFloat32 Dis::mDRAPosThrsh(1.0F);             // Meters
DisFloat32 Dis::mDE_PrecisionAimingThrsh(0.5F); // Meters
DisFloat32 Dis::mDE_EnergyThrsh(0.01F);         // Fractional
DisFloat32 Dis::mEE_AZ_THRSH(1.0F);             // degrees
DisFloat32 Dis::mEE_EL_THRSH(1.0F);             // degrees
DisFloat32 Dis::mEE_ERP_THRSH(1.0F);            // dBm
DisFloat32 Dis::mEE_FREQ_THRSH(1.0F);           // Hz
DisFloat32 Dis::mEE_FRNG_THRSH(1.0F);           // Hz
DisUint8   Dis::mEE_HIGH_DENSITY_THRSH(10);     // Entities/Beam
DisUint8   Dis::mEE_MAX_BEAMS_THRSH(255);       // Number of beams
DisFloat32 Dis::mEE_PRF_THRSH(1.0F);            // Hz
DisFloat32 Dis::mEE_PW_THRSH(1.0);              // microsecond
DisUint32  Dis::mMAX_PDU_SIZE_OCTETS(8192);

unsigned int       Dis::mSuppressErrorMask(0);
const unsigned int Dis::cIO_ERRORS(1 << 1);
const unsigned int Dis::cCONNECT_ERRORS(1 << 2);
const DisUint16    Dis::cALL_APPLIC(0xFFFF);
const DisUint8     Dis::cALL_BEAMS(0xFF);
const DisUint8     Dis::cALL_EMITTERS(0xFF);
const DisUint16    Dis::cALL_ENTITIES(0xFFFF);
const DisUint16    Dis::cALL_SITES(0xFFFF);
const DisUint16    Dis::cNO_APPLIC(0);
const DisUint8     Dis::cNO_BEAM(0);
const DisUint8     Dis::cNO_EMITTER(0);
const DisUint16    Dis::cNO_ENTITY(0);
const DisUint16    Dis::cNO_SITE(0);
const DisUint16    Dis::cRQST_ASSIGN_ID(0xFFFE);

const DisEntityId Dis::cD_SPOT_NO_ENTITY(Dis::cNO_SITE, Dis::cNO_APPLIC, Dis::cNO_ENTITY);
const DisEntityId Dis::cENTITY_ID_UNKNOWN(Dis::cNO_SITE, Dis::cNO_APPLIC, Dis::cNO_ENTITY);
const DisEntityId Dis::cNO_SPECIFIC_ENTITY(Dis::cNO_SITE, Dis::cNO_APPLIC, Dis::cNO_ENTITY);
const DisEntityId Dis::cTARGET_ID_UNKNOWN(Dis::cNO_SITE, Dis::cNO_APPLIC, Dis::cNO_ENTITY);

// static
void Dis::Init()
{
   char*      envStr;
   int        envVal;
   DisFloat32 floatVal;
   int        errors = 0;

   // If the error mask hasn't already been set by the user, default it
   // using the DIS_SUPPRESS_ERRORS environment variable.
   if (mSuppressErrorMask == 0)
   {
      envStr = getenv("DIS_SUPPRESS_ERRORS");
      if (envStr != nullptr)
      {
         std::string            data        = envStr;
         char                   delimiter[] = ":";
         std::string::size_type lpos        = data.find_first_not_of(delimiter, 0);
         std::string::size_type pos         = data.find_first_of(delimiter, lpos);
         std::string            token;
         while (pos != std::string::npos || lpos != std::string::npos)
         {
            token = data.substr(lpos, pos);
            lpos  = data.find_first_not_of(delimiter, pos);
            pos   = data.find_first_of(delimiter, lpos);

            if (token == "io")
            {
               SetSuppressErrorMask(GetSuppressErrorMask() | cIO_ERRORS);
            }
            else if (token == "connect")
            {
               SetSuppressErrorMask(GetSuppressErrorMask() | cCONNECT_ERRORS);
            }
         }
      }
   }

   envStr = getenv("DIS_VERSION");
   if (envStr != nullptr)
   {
      envVal = atoi(envStr);
      if ((envVal < 0) || (envVal > 6))
      {
         auto out = ut::log::error() << "Invalid value for 'DIS_VERSION'.";
         out.AddNote() << "Value: " << envStr;
         ++errors;
      }
      else
      {
         mProtocolVersion = static_cast<DisUint8>(envVal);
      }
   }

   envStr = getenv("DIS_EXERCISE");
   if (envStr != nullptr)
   {
      envVal = atoi(envStr);
      if ((envVal < 0) || (envVal > 255))
      {
         auto out = ut::log::error() << "Invalid value for 'DIS_EXERCISE'.";
         out.AddNote() << "Value: " << envStr;
         ++errors;
      }
      else
      {
         mExercise = static_cast<DisUint8>(envVal);
      }
   }

   envStr = getenv("DIS_SITE");
   if (envStr != nullptr)
   {
      envVal = atoi(envStr);
      if ((envVal < 0) || (envVal > 0xFFFF))
      {
         auto out = ut::log::error() << "Invalid value for 'DIS_SITE'.";
         out.AddNote() << "Value: " << envStr;
         ++errors;
      }
      else
      {
         mSite = static_cast<DisUint16>(envVal);
      }
   }

   envStr = getenv("DIS_APPLICATION");
   if (envStr != nullptr)
   {
      envVal = atoi(envStr);
      if ((envVal < 0) || (envVal > 0xFFFF))
      {
         auto out = ut::log::error() << "Invalid value for 'DIS_APPLICATION'.";
         out.AddNote() << "Value: " << envStr;
         ++errors;
      }
      else
      {
         mApplication = static_cast<DisUint16>(envVal);
      }
   }

   envStr = getenv("DIS_HRT_BEAT_TIMER");
   if (envStr != nullptr)
   {
      floatVal = static_cast<DisFloat32>(atof(envStr));
      if (floatVal < 0.0F)
      {
         auto out = ut::log::error() << "Invalid value for 'DIS_HRT_BEAT_TIMER'.";
         out.AddNote() << "Value: " << envStr;
         ++errors;
      }
      else
      {
         mHrtBeatTimer = floatVal;
      }
   }

   envStr = getenv("DIS_HRT_BEAT_MPLIER");
   if (envStr != nullptr)
   {
      floatVal = static_cast<DisFloat32>(atof(envStr));
      if (floatVal < 0.0F)
      {
         auto out = ut::log::error() << "Invalid value for 'DIS_HRT_BEAT_MPLIER'.";
         out.AddNote() << "Value: " << envStr;
         ++errors;
      }
      else
      {
         mHrtBeatMplier = floatVal;
      }
   }

   envStr = getenv("DIS_EE_AZ_THRSH");
   if (envStr != nullptr)
   {
      double dVal = atof(envStr);
      if (!ValidateScalar(dVal) || dVal < 0.0F || dVal >= 180.0F)
      {
         auto out = ut::log::warning() << "Invalid value for 'DIS_EE_AZ_THRSH'.";
         out.AddNote() << "Value: " << envStr;
         out.AddNote() << "Leaving DIS_EE_AZ_THRSH: " << mEE_AZ_THRSH;
      }
      else
      {
         mEE_AZ_THRSH = static_cast<DisFloat32>(dVal);
      }
   }
   envStr = getenv("DIS_EE_EL_THRSH");
   if (envStr != nullptr)
   {
      double dVal = atof(envStr);
      if (!ValidateScalar(dVal) || dVal < 0.0F || dVal >= 180.0F)
      {
         auto out = ut::log::warning() << "Invalid value for 'DIS_EE_EL_THRSH'.";
         out.AddNote() << "Value: " << envStr;
         out.AddNote() << "Leaving DIS_EE_EL_THRSH: " << mEE_EL_THRSH;
      }
      else
      {
         mEE_EL_THRSH = static_cast<DisFloat32>(dVal);
      }
   }
   envStr = getenv("DIS_EE_ERP_THRSH");
   if (envStr != nullptr)
   {
      double dVal = atof(envStr);
      if (!ValidateScalar(dVal) || dVal < 0.0F)
      {
         auto out = ut::log::warning() << "Invalid value for 'DIS_EE_ERP_THRSH'.";
         out.AddNote() << "Value: " << envStr;
         out.AddNote() << "Leaving DIS_EE_ERP_THRSH: " << mEE_ERP_THRSH;
      }
      else
      {
         mEE_ERP_THRSH = static_cast<DisFloat32>(dVal);
      }
   }
   envStr = getenv("DIS_EE_FREQ_THRSH");
   if (envStr != nullptr)
   {
      double dVal = atof(envStr);
      if (!ValidateScalar(dVal) || dVal < 0.0F)
      {
         auto out = ut::log::warning() << "Invalid value for 'DIS_EE_FREQ_THRSH'.";
         out.AddNote() << "Value: " << envStr;
         out.AddNote() << "Leaving DIS_EE_FREQ_THRSH: " << mEE_FREQ_THRSH;
      }
      else
      {
         mEE_FREQ_THRSH = static_cast<DisFloat32>(dVal);
      }
   }
   envStr = getenv("DIS_EE_FRNG_THRSH");
   if (envStr != nullptr)
   {
      double dVal = atof(envStr);
      if (!ValidateScalar(dVal) || dVal < 0.0F)
      {
         auto out = ut::log::warning() << "Invalid value for 'DIS_EE_FRNG_THRSH'.";
         out.AddNote() << "Value: " << envStr;
         out.AddNote() << "Leaving DIS_EE_FRNG_THRSH: " << mEE_FRNG_THRSH;
      }
      else
      {
         mEE_FRNG_THRSH = static_cast<DisFloat32>(dVal);
      }
   }
   envStr = getenv("DIS_TARGETS_IN_TJ_FIELD_DFLT");
   if (envStr != nullptr)
   {
      { // RAII block
         auto out = ut::log::warning() << "DIS_TARGETS_IN_TJ_FIELD_DFLT is deprecated.";
         out.AddNote() << "Please set DIS_EE_HIGH_DENSITY_THRSH instead.";
      }
      int intVal = atoi(envStr);
      if (intVal < 0 || intVal > 255)
      {
         auto out = ut::log::warning() << "Invalid value for 'DIS_EE_HIGH_DENSITY_THRSH'.";
         out.AddNote() << "Value: " << envStr;
         out.AddNote() << "Leaving DIS_EE_HIGH_DENSITY_THRSH: " << mEE_HIGH_DENSITY_THRSH;
      }
      else
      {
         mEE_HIGH_DENSITY_THRSH = static_cast<DisUint8>(intVal);
      }
   }
   envStr = getenv("DIS_EE_HIGH_DENSITY_THRSH");
   if (envStr != nullptr)
   {
      int intVal = atoi(envStr);
      if (intVal < 0 || intVal > 255)
      {
         auto out = ut::log::warning() << "Invalid value for 'DIS_EE_HIGH_DENSITY_THRSH'.";
         out.AddNote() << "Value: " << envStr;
         out.AddNote() << "Leaving DIS_EE_HIGH_DENSITY_THRSH: " << mEE_HIGH_DENSITY_THRSH;
      }
      else
      {
         mEE_HIGH_DENSITY_THRSH = static_cast<DisUint8>(intVal);
      }
   }
   envStr = getenv("DIS_EE_MAX_BEAMS_THRSH");
   if (envStr != nullptr)
   {
      int intVal = atoi(envStr);
      if (intVal < 0 || intVal > 255)
      {
         auto out = ut::log::warning() << "Invalid value for 'DIS_EE_MAX_BEAMS_THRSH'.";
         out.AddNote() << "Value: " << envStr;
         out.AddNote() << "Leaving DIS_EE_MAX_BEAMS_THRSH: " << mEE_MAX_BEAMS_THRSH;
      }
      else
      {
         mEE_MAX_BEAMS_THRSH = static_cast<DisUint8>(intVal);
      }
   }
   envStr = getenv("DIS_EE_PRF_THRSH");
   if (envStr != nullptr)
   {
      double dVal = atof(envStr);
      if (!ValidateScalar(dVal) || dVal < 0.0F)
      {
         auto out = ut::log::warning() << "Invalid value for 'DIS_EE_PRF_THRSH'.";
         out.AddNote() << "Value: " << envStr;
         out.AddNote() << "Leaving DIS_EE_PRF_THRSH: " << mEE_PRF_THRSH;
      }
      else
      {
         mEE_PRF_THRSH = static_cast<DisFloat32>(dVal);
      }
   }
   envStr = getenv("DIS_EE_PW_THRSH");
   if (envStr != nullptr)
   {
      double dVal = atof(envStr);
      if (!ValidateScalar(dVal) || dVal < 0.0F)
      {
         auto out = ut::log::warning() << "Invalid value for 'DIS_EE_PW_THRSH'.";
         out.AddNote() << "Value: " << envStr;
         out.AddNote() << "Leaving DIS_EE_PW_THRSH: " << mEE_PW_THRSH;
      }
      else
      {
         mEE_PW_THRSH = static_cast<DisFloat32>(dVal);
      }
   }
   envStr = getenv("DIS_MAX_PDU_SIZE_OCTETS");
   if (envStr != nullptr)
   {
      int intVal = atoi(envStr);
      if (intVal <= 12)
      {
         auto out = ut::log::warning() << "Invalid value for 'DIS_MAX_PDU_SIZE_OCTETS'.";
         out.AddNote() << "Value: " << envStr;
         out.AddNote() << "Leaving DIS_MAX_PDU_SIZE_OCTETS: " << mMAX_PDU_SIZE_OCTETS;
      }
      else
      {
         mMAX_PDU_SIZE_OCTETS = static_cast<DisUint32>(intVal);
      }
   }
}

// static
void Dis::Term() {}

// static
void Dis::SetSuppressErrorMask(unsigned int aSuppressErrorMask)
{
   mSuppressErrorMask = aSuppressErrorMask;
   GenError::SetSuppressErrorMask(aSuppressErrorMask);
}

// static
unsigned int Dis::GetSuppressErrorMask()
{
   return mSuppressErrorMask;
}

// static
bool Dis::ShowError(unsigned int aErrorMask)
{
   bool showError = true;

   if (mSuppressErrorMask & aErrorMask)
   {
      showError = false;
   }
   return showError;
}

bool Dis::IsValid()
{
   bool isValid = ValidateScalar(mHrtBeatTimer);
   if (isValid)
      isValid = ValidateScalar(mHrtBeatMoveTimer);
   if (isValid)
      isValid = ValidateScalar(mHrtBeatDE_FireTimer);
   if (isValid)
      isValid = ValidateScalar(mHrtBeatMplier);
   if (isValid)
      isValid = ValidateScalar(mDRAOrientThrsh);
   if (isValid)
      isValid = ValidateScalar(mDRAPosThrsh);
   if (isValid)
      isValid = ValidateScalar(mDE_PrecisionAimingThrsh);
   if (isValid)
      isValid = ValidateScalar(mDE_EnergyThrsh);
   if (isValid)
      isValid = ValidateScalar(mEE_AZ_THRSH);
   if (isValid)
      isValid = ValidateScalar(mEE_EL_THRSH);
   if (isValid)
      isValid = ValidateScalar(mEE_ERP_THRSH);
   if (isValid)
      isValid = ValidateScalar(mEE_FREQ_THRSH);
   if (isValid)
      isValid = ValidateScalar(mEE_FRNG_THRSH);
   if (isValid)
      isValid = ValidateScalar(mEE_PRF_THRSH);
   if (isValid)
      isValid = ValidateScalar(mEE_PW_THRSH);
   return isValid;
}
