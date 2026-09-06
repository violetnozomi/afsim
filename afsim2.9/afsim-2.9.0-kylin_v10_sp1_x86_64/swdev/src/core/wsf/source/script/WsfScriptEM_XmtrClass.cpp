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

#include "script/WsfScriptEM_XmtrClass.hpp"

#include "UtMemory.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfScenario.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptEM_XmtrClass::WsfScriptEM_XmtrClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptEM_XmtrRcvrClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfEM_Xmtr");

   AddMethod(ut::make_unique<Power_1>("Power"));
   AddMethod(ut::make_unique<Power_2>("Power"));
   AddMethod(ut::make_unique<SetPower_1>("SetPower"));
   AddMethod(ut::make_unique<SetPower_2>("SetPower"));

   AddMethod(ut::make_unique<Power_1>("PeakPower"));
   AddMethod(ut::make_unique<Power_2>("PeakPower"));

   AddMethod(ut::make_unique<DutyCycle>());
   AddMethod(ut::make_unique<SetDutyCycle>());

   AddMethod(ut::make_unique<AlternateFrequency>());
   AddMethod(ut::make_unique<AlternateFrequencyCount>());
   AddMethod(ut::make_unique<AlternateFrequencyMap>());
   AddMethod(ut::make_unique<AlternateFrequencyIndex>());

   AddMethod(ut::make_unique<PulseRepetitionInterval_1>("PulseRepetitionInterval"));
   AddMethod(ut::make_unique<PulseRepetitionInterval_2>("PulseRepetitionInterval"));
   AddMethod(ut::make_unique<SetPulseRepetitionInterval_1>("SetPulseRepetitionInterval"));
   AddMethod(ut::make_unique<SetPulseRepetitionInterval_2>("SetPulseRepetitionInterval"));
   AddMethod(ut::make_unique<PulseRepetitionIntervalCount>());
   AddMethod(ut::make_unique<PulseRepetitionIntervals>());

   AddMethod(ut::make_unique<PulseRepetitionFrequency_1>("PulseRepetitionFrequency"));
   AddMethod(ut::make_unique<PulseRepetitionFrequency_2>("PulseRepetitionFrequency"));
   AddMethod(ut::make_unique<SetPulseRepetitionFrequency_1>("SetPulseRepetitionFrequency"));
   AddMethod(ut::make_unique<SetPulseRepetitionFrequency_2>("SetPulseRepetitionFrequency"));
   AddMethod(ut::make_unique<PulseRepetitionFrequencyCount>());
   AddMethod(ut::make_unique<PulseRepetitionFrequencies>());

   AddMethod(ut::make_unique<PulseWidth>());
   AddMethod(ut::make_unique<SetPulseWidth>());

   AddMethod(ut::make_unique<PulseCompressionRatio>());
   AddMethod(ut::make_unique<SetPulseCompressionRatio>());
   AddMethod(ut::make_unique<Index>("BeamNumber"));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, Power_1, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPower());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, Power_2, 1, "double", "double")
{
   double frequency = aVarArgs[0].GetDouble();
   aReturnVal.SetDouble(aObjectPtr->GetPower(frequency));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, SetPower_1, 1, "void", "double")
{
   double power = aVarArgs[0].GetDouble();
   aObjectPtr->SetPower(power);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, SetPower_2, 2, "void", "double, double")
{
   double power     = aVarArgs[0].GetDouble();
   double frequency = aVarArgs[1].GetDouble();
   aObjectPtr->SetPower(power, frequency);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, PeakPower_1, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPeakPower());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, PeakPower_2, 1, "double", "double")
{
   double frequency = aVarArgs[0].GetDouble();
   aReturnVal.SetDouble(aObjectPtr->GetPeakPower(frequency));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, DutyCycle, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDutyCycle());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, SetDutyCycle, 1, "void", "double")
{
   double dutyCycle = aVarArgs[0].GetDouble();
   aObjectPtr->SetDutyCycle(dutyCycle);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, AlternateFrequency, 1, "double", "int")
{
   int id = aVarArgs[0].GetInt();
   aReturnVal.SetDouble(aObjectPtr->GetAlternateFrequency(id));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, AlternateFrequencyCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetAlternateFrequencyCount());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, AlternateFrequencyMap, 0, "Map<int, double>", "")
{
   UtScriptTypes*      scriptTypesPtr       = SCENARIO->GetScriptTypes();
   static UtScriptMap& sScriptMapClass      = static_cast<UtScriptMap&>(*scriptTypesPtr->GetClass("Map<int, double>"));
   UtScriptMap::Map*   mapPtr               = new UtScriptMap::Map;
   UtScriptMap::Map&   map                  = *mapPtr;
   const WsfEM_Xmtr::FrequencyList& freqMap = aObjectPtr->GetAlternateFrequencies();
   for (auto& idfreq : freqMap)
   {
      UtScriptData idData(static_cast<int>(idfreq.first));
      UtScriptData freqData(idfreq.second);
      map[idData] = freqData;
   }
   UtScriptRef* mapRefPtr = new UtScriptRef(mapPtr, &sScriptMapClass, UtScriptRef::cMANAGE);
   aReturnVal.SetPointer(mapRefPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, AlternateFrequencyIndex, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetCurrentAlternateFrequencyId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, PulseRepetitionInterval_1, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPulseRepetitionInterval());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, PulseRepetitionInterval_2, 1, "double", "int")
{
   int index = aVarArgs[0].GetInt();
   aReturnVal.SetDouble(aObjectPtr->GetPulseRepetitionInterval(index));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, SetPulseRepetitionInterval_1, 1, "void", "double")
{
   double pri = aVarArgs[0].GetDouble();
   aObjectPtr->SetPulseRepetitionInterval(pri);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, SetPulseRepetitionInterval_2, 2, "void", "double, int")
{
   double pri = aVarArgs[0].GetDouble();
   int    num = aVarArgs[1].GetInt();
   aObjectPtr->SetPulseRepetitionInterval(pri, num);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, PulseRepetitionIntervalCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetPulseRepetitionIntervalCount());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, PulseRepetitionIntervals, 0, "Array<double>", "")
{
   std::vector<double>        pris = aObjectPtr->GetPulseRepetitionIntervals();
   std::vector<UtScriptData>* data = new std::vector<UtScriptData>(pris.size());
   for (size_t i = 0; i < pris.size(); ++i)
   {
      (*data)[i].SetDouble(pris[i]);
   }
   aReturnVal.SetPointer(new UtScriptRef(data, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, PulseRepetitionFrequency_1, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPulseRepetitionFrequency());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, PulseRepetitionFrequency_2, 1, "double", "int")
{
   int index = aVarArgs[0].GetInt();
   aReturnVal.SetDouble(aObjectPtr->GetPulseRepetitionFrequency(index));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, SetPulseRepetitionFrequency_1, 1, "void", "double")
{
   double prf = aVarArgs[0].GetDouble();
   aObjectPtr->SetPulseRepetitionFrequency(prf);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, SetPulseRepetitionFrequency_2, 2, "void", "double, int")
{
   double prf = aVarArgs[0].GetDouble();
   int    num = aVarArgs[1].GetInt();
   aObjectPtr->SetPulseRepetitionFrequency(prf, num);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, PulseRepetitionFrequencyCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetPulseRepetitionFrequencyCount());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, PulseRepetitionFrequencies, 0, "Array<double>", "")
{
   std::vector<double>        prfs = aObjectPtr->GetPulseRepetitionFrequencies();
   std::vector<UtScriptData>* data = new std::vector<UtScriptData>(prfs.size());
   for (size_t i = 0; i < prfs.size(); ++i)
   {
      (*data)[i].SetDouble(prfs[i]);
   }
   aReturnVal.SetPointer(new UtScriptRef(data, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, PulseWidth, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPulseWidth());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, SetPulseWidth, 1, "void", "double")
{
   double pw = aVarArgs[0].GetDouble();
   aObjectPtr->SetPulseWidth(pw);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, PulseCompressionRatio, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPulseCompressionRatio());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrClass, WsfEM_Xmtr, SetPulseCompressionRatio, 1, "void", "double")
{
   double pcr = aVarArgs[0].GetDouble();
   aObjectPtr->SetPulseCompressionRatio(pcr);
}
