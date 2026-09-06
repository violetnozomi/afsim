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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfPM_Transmitter.hpp"

#include "UtAzElTable.hpp"
#include "UtAzElTableLoader.hpp"
#include "UtInput.hpp"
#include "WsfPProxyIndex.hpp"
#include "WsfParseNode.hpp"

WsfProxy::Power* WsfPM_Transmitter::Power() const
{
   return GetValue().GetAttr("power");
}

/* I am not really happy with this code.  Dan says that returning a double from the WsfProxy::Type should be default
units. Maybe we should just return that. double WsfPM_Transmitter::power(const std::string& aUnit) const
{
   WsfProxy::Power* pwr = power();

   UtUnitType* unitTypePtr = UtUnits::GetUnitType(UtUnits::cPOWER);

   double val1;
   std::string unit1;
   std::stringstream ss(pwr->ToString());
   if (ss >> val1 >> unit1)
   {
      int currentUnit = unitTypePtr->ReadUnit(unit1);
      int desiredUnit = unitTypePtr->ReadUnit(aUnit);
      double stdVal = unitTypePtr->ConvertToStandard(val1, currentUnit);
      return unitTypePtr->ConvertFromStandard(stdVal, desiredUnit);
   }
   return 0.0;
}*/

WsfProxy::Length* WsfPM_Transmitter::Wavelength() const
{
   return GetValue().GetAttr("wavelength");
}

/*double WsfPM_Transmitter::wavelength(const std::string& aUnit) const
{
   WsfProxy::Length* wl = wavelength();

   if (wl->IsUnset())
   {
      WsfProxy::Frequency* freq = frequency();
      if (freq->IsUnset())
      {
         return 0;
      }
      else
      {
         double freqHz = frequency("hz");
         double val1 = UtMath::cLIGHT_SPEED / freqHz;
         UtUnitType* unitTypePtr = UtUnits::GetUnitType(UtUnits::cLENGTH);
         int unitId = unitTypePtr->ReadUnit("meters");
         int desiredUnit = unitTypePtr->ReadUnit(aUnit);
         double stdVal = unitTypePtr->ConvertToStandard(val1, unitId);
         return unitTypePtr->ConvertFromStandard(stdVal, desiredUnit);
         // convert to hertz and then to wavelength meters and then to desired units
      }
   }
   else
   {
      UtUnitType* unitTypePtr = UtUnits::GetUnitType(UtUnits::cLENGTH);

      double val1;
      std::string unit1;
      std::stringstream ss(wl->ToString());
      if (ss >> val1 >> unit1)
      {
         int currentUnit = unitTypePtr->ReadUnit(unit1);
         int desiredUnit = unitTypePtr->ReadUnit(aUnit);
         double stdVal = unitTypePtr->ConvertToStandard(val1, currentUnit);
         return unitTypePtr->ConvertFromStandard(stdVal, desiredUnit);
      }
      return 0.0;
   }
}*/

WsfProxy::Frequency* WsfPM_Transmitter::Frequency() const
{
   return GetValue().GetAttr("frequency");
}

/*double WsfPM_Transmitter::frequency(const std::string& aUnit) const
{
// JON TODO, this seems to break when a default is set, it doesn't know to use the default or freq
   WsfProxy::Frequency* freq = frequency();

   if (freq->IsUnset())
   {
      WsfProxy::Length* wl = wavelength();
      if (wl->IsUnset())
      {
         return 0.0;
      }
      else
      {
         double wavelengthM = wavelength("meters");
         double val1 = UtMath::cLIGHT_SPEED / wavelengthM;
         UtUnitType* unitTypePtr = UtUnits::GetUnitType(UtUnits::cFREQUENCY);
         int unitId = unitTypePtr->ReadUnit("hz");
         int desiredUnit = unitTypePtr->ReadUnit(aUnit);
         double stdVal = unitTypePtr->ConvertToStandard(val1, unitId);
         return unitTypePtr->ConvertFromStandard(stdVal, desiredUnit);
         // convert to hertz and then to wavelength meters and then to desired units
      }
   }
   else
   {
      UtUnitType* unitTypePtr = UtUnits::GetUnitType(UtUnits::cFREQUENCY);

      double val1;
      std::string unit1;
      std::stringstream ss(freq->ToString());
      if (ss >> val1 >> unit1)
      {
         int currentUnit = unitTypePtr->ReadUnit(unit1);
         int desiredUnit = unitTypePtr->ReadUnit(aUnit);
         double stdVal = unitTypePtr->ConvertToStandard(val1, currentUnit);
         return unitTypePtr->ConvertFromStandard(stdVal, desiredUnit);
      }
      return 0.0;
   }
}*/

WsfProxy::Frequency* WsfPM_Transmitter::Bandwidth() const
{
   return GetValue().GetAttr("bandwidth");
}

/*double WsfPM_Transmitter::bandwidth(const std::string& aUnit) const
{
   WsfProxy::Frequency* bw = bandwidth();

   UtUnitType* unitTypePtr = UtUnits::GetUnitType(UtUnits::cFREQUENCY);

   double val1;
   std::string unit1;
   std::stringstream ss(bw->ToString());
   if (ss >> val1 >> unit1)
   {
      int currentUnit = unitTypePtr->ReadUnit(unit1);
      int desiredUnit = unitTypePtr->ReadUnit(aUnit);
      double stdVal = unitTypePtr->ConvertToStandard(val1, currentUnit);
      return unitTypePtr->ConvertFromStandard(stdVal, desiredUnit);
   }
   return 0.0;
}*/

std::string WsfPM_Transmitter::Polarization() const
{
   return GetValue().GetAttr("polarization").ValueToString();
}

/*int WsfPM_Transmitter::polarizationInt() const
{
   int val = -1;
   GetValue().GetAttr("polarization").GetIntegerValue(val);
   return val;
}*/

WsfProxy::Ratio* WsfPM_Transmitter::InternalLoss() const
{
   return GetValue().GetAttr("internalLoss");
}

/*double WsfPM_Transmitter::internalLoss(const std::string& aUnit) const
{
   WsfProxy::Ratio* il = internalLoss();

   UtUnitType* unitTypePtr = UtUnits::GetUnitType(UtUnits::cRATIO);

   double val1;
   std::string unit1;
   std::stringstream ss(il->ToString());
   if (ss >> val1 >> unit1)
   {
      int currentUnit = unitTypePtr->ReadUnit(unit1);
      int desiredUnit = unitTypePtr->ReadUnit(aUnit);
      double stdVal = unitTypePtr->ConvertToStandard(val1, currentUnit);
      return unitTypePtr->ConvertFromStandard(stdVal, desiredUnit);
   }
   return 0.0;
}*/

WsfProxy::Ratio* WsfPM_Transmitter::PulseCompressionRatio() const
{
   return GetValue().GetAttr("internalLoss");
}

/*double WsfPM_Transmitter::pulseCompressionRatio(const std::string& aUnit) const
{
   WsfProxy::Ratio* pcr = pulseCompressionRatio();

   UtUnitType* unitTypePtr = UtUnits::GetUnitType(UtUnits::cRATIO);

   double val1;
   std::string unit1;
   std::stringstream ss(pcr->ToString());
   if (ss >> val1 >> unit1)
   {
      int currentUnit = unitTypePtr->ReadUnit(unit1);
      int desiredUnit = unitTypePtr->ReadUnit(aUnit);
      double stdVal = unitTypePtr->ConvertToStandard(val1, currentUnit);
      return unitTypePtr->ConvertFromStandard(stdVal, desiredUnit);
   }
   return 0.0;
}*/

WsfProxy::Time* WsfPM_Transmitter::PulseWidth() const
{
   return GetValue().GetAttr("pulseWidth");
}

/*double WsfPM_Transmitter::pulseWidth(const std::string& aUnit) const
{
   WsfProxy::Time* pw = pulseWidth();

   UtUnitType* unitTypePtr = UtUnits::GetUnitType(UtUnits::cTIME);

   double val1;
   std::string unit1;
   std::stringstream ss(pw->ToString());
   if (ss >> val1 >> unit1)
   {
      int currentUnit = unitTypePtr->ReadUnit(unit1);
      int desiredUnit = unitTypePtr->ReadUnit(aUnit);
      double stdVal = unitTypePtr->ConvertToStandard(val1, currentUnit);
      return unitTypePtr->ConvertFromStandard(stdVal, desiredUnit);
   }
   return 0.0;
}*/

/*void WsfPM_Transmitter::getGainTable(WsfPProxyIndex* aProxyIndexPtr,
                                     int aPolarization,
                                     double aFrequencyHz,
                                     UtAzElTable*& aTablePtr) const
{
   // TODO handle antenna pattern tables
   aTablePtr = 0;
   std::string defaultPattern = GetValue().GetAttr("antennaPattern").ValueToString();
   WsfPProxyValuePos ap = ((WsfPProxyValuePos::FromRoot(pos().GetRoot()) + "antennaPattern") + defaultPattern);
   WsfPProxyPath path = ap.GetPath();

   if (aProxyIndexPtr)
   {
      WsfPProxyIndexNode* idxNode = aProxyIndexPtr->Find(path);
      if (idxNode)
      {
         for (size_t ei = 0; ei < idxNode->mEntries.size(); ++ei)
         {
            WsfPProxyIndexEntry& e = idxNode->mEntries[ei];
            WsfParseNode* n = e.mNodePtr->PreviousLeaf();
            if (n)
            {
               UtTextDocumentRange range = n->GetParent()->SubtreeRange();
               if (range.Valid())
               {
                  UtInput input;
                  input.PushInputString(range.Pointer());
                  std::string tableUnits("db");

                  string command(input.GetCommand());
                  if (command == "antenna_pattern")
                  {
                     input.ReadCommand(command); // consume antenna_pattern
                     input.ReadCommand(command); // consume the pattern name
                     if (UtAzElTableLoader::ProcessTable(input, aTablePtr, tableUnits) &&
                         (aTablePtr != 0))
                     {
                        UtStringUtil::ToLower(tableUnits);
                        if (tableUnits == "db")
                        {
                           // Input values are in dB
                           aTablePtr->ConvertValuesFromDB(1.0F);
                        }
                        else if (tableUnits != "absolute")
                        {
                           throw UtInput::BadValue(input, "table units must be 'db' or 'absolute'");
                        }
                        return;
                     }
                  }
               }
            }
         }
      }
   }
}*/
