// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfEarthGravityModel.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{

// ============================================================================
//! Process the gravity_model block.
//!
//! @param aInput [input] The input stream.
//!
//! @return 'true' if the command was recognized (and processed) or 'false' if the command was not
//! one recognized by this class.
bool EarthGravityModelExtension::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if ((command == "gravity_model") || (command == "geoid_undulation"))
   {
      myCommand = true;

      mGeoidUndulationPtr = std::make_shared<GeoidUndulation>();

      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand())
      {
         if (!ProcessInputData(aInput))
         {
            throw UtInput::BadValue(aInput, "Command not understood in context of 'gravity_model' or 'geoid_undulation' block.");
         }
      }
   }
   return myCommand;
}

// ============================================================================
//! Process the gravity_model input block data.
//!
//! @param aInput [input] The input stream.
//!
//! @return 'true' if the command was recognized (and processed) or 'false' if the command was not
//! one recognized by this class.
bool EarthGravityModelExtension::ProcessInputData(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "file")
   {
      std::string egmType;
      aInput.ReadValue(egmType);
      if ((egmType == "2008") || (egmType == "EGM2008"))
      {
         mEGM_Type = cEGM2008;
      }
      else if ((egmType == "1996") || (egmType == "EGM96"))
      {
         mEGM_Type = cEGM96;
      }
      else
      {
         throw UtInput::BadValue(aInput, "egm_type not recognized!");
      }

      aInput.ReadValueQuoted(mEGM_DataFile);
      mEGM_DataFile = aInput.SubstitutePathVariables(mEGM_DataFile);
      mEGM_DataFile = aInput.LocateFile(mEGM_DataFile);
      if (mEGM_DataFile.empty())
      {
         throw UtInput::BadValue(aInput, "could not find data_file!");
      }
   }
   else if (command == "test_point")
   {
      double lat, lon;
      aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);
      mTestLatLons.emplace_back(lat, lon);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
EarthGravityModelExtension& EarthGravityModelExtension::Get(const WsfScenario& aScenario)
{
   WsfScenarioExtension* extensionPtr = aScenario.FindExtension("gravity_model");
   return static_cast<EarthGravityModelExtension&>(*extensionPtr);
}

// =================================================================================================
bool EarthGravityModelExtension::Complete()
{
   bool ok = true;
   if (!mEGM_DataFile.empty())
   {
      std::string egmType = "EGM96";
      if (mEGM_Type == cEGM2008)
      {
         egmType = "EGM2008";
      }

      if (!mGeoidUndulationPtr->ReadData(mEGM_DataFile, egmType))
      {
         auto out = ut::log::error() << "Supplied geoid_undulation data_file could not be interpreted properly.";
         out.AddNote() << "File: " << mEGM_DataFile;
         ok = false;
      }
   }
   else
   {
      mGeoidUndulationPtr.reset();
   }

   if (mGeoidUndulationPtr != nullptr)
   {
      for (auto& latlon : mTestLatLons)
      {
         float undulationheight = mGeoidUndulationPtr->Get(latlon.first, latlon.second);
         ut::log::info() << "Undulation value at ( " << latlon.first << ", " << latlon.second << " ) is "
                         << undulationheight << " m.";
      }
   }

   return ok;
}

// ============================================================================
void EarthGravityModelExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<EarthGravityModel>(this));
}

// =================================================================================================
float EarthGravityModelExtension::GetGeoidUndulation(double aLat, double aLon) const
{
   return (mGeoidUndulationPtr != nullptr) ? mGeoidUndulationPtr->Get(aLat, aLon) : 0.0f;
}

// =================================================================================================
EarthGravityModel::EarthGravityModel(EarthGravityModelExtension* aExtensionPtr)
   : mEGM_ExtensionPtr(aExtensionPtr)
{
}

// ============================================================================
EarthGravityModel* EarthGravityModel::Find(const WsfSimulation& aSimulation)
{
   return static_cast<EarthGravityModel*>(aSimulation.FindExtension("gravity_model"));
}

// =================================================================================================
float EarthGravityModel::GetGeoidUndulation(double aLat, double aLon) const
{
   return mEGM_ExtensionPtr->GetGeoidUndulation(aLat, aLon);
}

// =================================================================================================
wsf::EarthGravityModelExtension::EGMComplianceType EarthGravityModel::GetEGM_ComplianceType() const
{
   return mEGM_ExtensionPtr->GetEGM_ComplianceType();
}

// =================================================================================================
bool EarthGravityModel::IsEGM2008Compliant() const
{
   return mEGM_ExtensionPtr->IsEGM2008Compliant();
}

// =================================================================================================
bool EarthGravityModel::IsEGM96Compliant() const
{
   return mEGM_ExtensionPtr->IsEGM96Compliant();
}

// =================================================================================================
bool EarthGravityModel::IsEGM_Unknown() const
{
   return mEGM_ExtensionPtr->IsEGM_Unknown();
}
} // namespace wsf
