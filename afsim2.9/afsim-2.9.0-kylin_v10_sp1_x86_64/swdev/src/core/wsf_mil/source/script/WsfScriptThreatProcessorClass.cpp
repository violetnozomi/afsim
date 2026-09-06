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

#include "WsfScriptThreatProcessorClass.hpp"

#include <set>

#include "UtMemory.hpp"
#include "WsfThreatProcessor.hpp"

WsfScriptThreatProcessorClass::WsfScriptThreatProcessorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   AddClassName("WsfThreatProcessor");
   AddMethod(ut::make_unique<Threats>());
   AddMethod(ut::make_unique<IsThreatening>()); // bool IsThreatening(WsfTrack threat)
   AddMethod(ut::make_unique<NearestThreat>());
}


//! Returns an array of track ids for tracks that are a threat to this platform
UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatProcessorClass, WsfThreatProcessor, Threats, 0, "Array<WsfTrackId>", "")
{
   UtScriptClass*             classPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   std::vector<UtScriptData>* arrayPtr = new std::vector<UtScriptData>();
   std::set<WsfTrackId>*      threats  = aObjectPtr->GetThreatList();
   for (std::set<WsfTrackId>::iterator it = threats->begin(); it != threats->end(); ++it)
   {
      WsfTrackId* trackIdPtr = new WsfTrackId(*it);
      arrayPtr->push_back(UtScriptData(new UtScriptRef(trackIdPtr, classPtr, UtScriptRef::cMANAGE)));
   }
   aReturnVal.SetPointer(new UtScriptRef(arrayPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! Returns true or false depending on if the track is threatening to this platform
UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatProcessorClass, WsfThreatProcessor, IsThreatening, 1, "bool", "WsfTrack")
{
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(aObjectPtr->IsThreatening(trackPtr));
}


//! Returns nearest track that is threat to this platform
UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatProcessorClass, WsfThreatProcessor, NearestThreat, 0, "WsfTrack", "")
{
   WsfTrack* threat = aObjectPtr->GetNearestThreat();
   aReturnVal.SetPointer(new UtScriptRef(threat, aReturnClassPtr));
}
