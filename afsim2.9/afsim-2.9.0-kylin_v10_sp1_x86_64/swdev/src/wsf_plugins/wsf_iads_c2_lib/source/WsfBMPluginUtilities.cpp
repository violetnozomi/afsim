// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: WsfBMPluginUtilities.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract: This is a core class implementation for a HELIOS-like C2 dissemination
*    model. It may be used or extended to meet individual IADS needs.
*
*******************************************************************************/

#include "WsfBMPluginUtilities.hpp"
#include <UtPluginManager.hpp>
#include <UtScriptTypes.hpp>
#include <script/WsfScriptManager.hpp>
#include <UtAttributeBase.hpp>
#include <UtAttributeContainer.hpp>
#include <WsfAttributeContainer.hpp>

// types to register
#include "WsfScriptBattleManagerClass.hpp"
#include "WsfBattleManager.hpp"
#include "WsfBMAssignmentMessage.hpp"
#include "WsfBMTrackMessage.hpp"
#include "WsfBMStatusMessage.hpp"
#include "WsfBMCueMessage.hpp"
#include "WsfBMEvalAssignment.hpp"
#include <WsfScriptBMAssetRecordClass.hpp>
#include <WsfScriptAssetMapClass.hpp>
#include <WsfScriptBMTrackMessageClass.hpp>
#include <WsfScriptBMUtilsClass.hpp>
#include <WsfScriptBMStatusMessageClass.hpp>
#include <WsfScriptBMAssignmentMessageClass.hpp>
#include <WsfScriptBMAssignmentStatusClass.hpp>
#include <WsfScriptBMCueMessageClass.hpp>
#include <WsfScriptBMAssignmentTrackMessageClass.hpp>
#include <WsfScriptBMWeaponInterface.hpp>
#include <WsfScriptBMEvalAssignmentClass.hpp>
#include <WsfBMInterceptCalcResult.hpp>
#include <WsfScriptBMAuxDataZoneAttachmentClass.hpp>
#include <WsfBMAuxDataZoneAttachment.hpp>
#include <WsfScriptBMAssessmentRecordClass.hpp>

#include <tuple>

void RegisterBaseTypes(WsfApplication* aApplicationPtr)
{
   auto script_types = aApplicationPtr->GetScriptTypes();

   if (!IsClassTypeRegistered(aApplicationPtr, "WsfBMAssetRecord"))
   {
      script_types->Register(ut::make_unique<WsfScriptBMAssetRecordClass>("WsfBMAssetRecord", script_types));
   }

   if (!IsClassTypeRegistered(aApplicationPtr, "WsfBMAssessmentRecord"))
   {
      script_types->Register(ut::make_unique<WsfScriptBMAssessmentRecordClass>("WsfBMAssessmentRecord", script_types));
   }

   if (!IsClassTypeRegistered(aApplicationPtr, "WsfBMAssetMap"))
   {
      script_types->Register(ut::make_unique<WsfScriptAssetMapClass>("WsfBMAssetMap", script_types));
   };

   // dependent types
   if (!IsClassTypeRegistered(aApplicationPtr, "WsfBMWeaponInterface"))
   {
      script_types->Register(ut::make_unique<WsfScriptBMWeaponInterface>("WsfBMWeaponInterface", script_types));
   }

   if (!IsClassTypeRegistered(aApplicationPtr, "WsfBMUtils"))
   {
      script_types->Register(ut::make_unique<WsfScriptBMUtilsClass>("WsfBMUtils", script_types));
   }

   // Script-referenceable Messages
   if (!IsClassTypeRegistered(aApplicationPtr, WsfScriptBMTrackMessageClass::SCRIPT_CLASS_NAME))
   {
      script_types->Register(ut::make_unique<WsfScriptBMTrackMessageClass>(WsfScriptBMTrackMessageClass::SCRIPT_CLASS_NAME, script_types));
   }

   if (!IsClassTypeRegistered(aApplicationPtr, WsfScriptBMStatusMessageClass::SCRIPT_CLASS_NAME))
   {
      script_types->Register(ut::make_unique<WsfScriptBMStatusMessageClass>(WsfScriptBMStatusMessageClass::SCRIPT_CLASS_NAME, script_types));
   }

   if (!IsClassTypeRegistered(aApplicationPtr, WsfScriptBMAssignmentMessageClass::SCRIPT_CLASS_NAME))
   {
      script_types->Register(ut::make_unique<WsfScriptBMAssignmentMessageClass>(WsfScriptBMAssignmentMessageClass::SCRIPT_CLASS_NAME, script_types));
   }

   if (!IsClassTypeRegistered(aApplicationPtr, WsfScriptBMAssignmentStatusClass::SCRIPT_CLASS_NAME))
   {
      script_types->Register(ut::make_unique<WsfScriptBMAssignmentStatusClass>(WsfScriptBMAssignmentStatusClass::SCRIPT_CLASS_NAME, script_types));
   }

   if (!IsClassTypeRegistered(aApplicationPtr, WsfScriptBMCueMessageClass::SCRIPT_CLASS_NAME))
   {
      script_types->Register(ut::make_unique<WsfScriptBMCueMessageClass>(WsfScriptBMCueMessageClass::SCRIPT_CLASS_NAME, script_types));
   }

   if (!IsClassTypeRegistered(aApplicationPtr, WsfScriptBMAssignmentTrackMessageClass::SCRIPT_CLASS_NAME))
   {
      script_types->Register(ut::make_unique<WsfScriptBMAssignmentTrackMessageClass>(WsfScriptBMAssignmentTrackMessageClass::SCRIPT_CLASS_NAME, script_types));
   }

   if (!IsClassTypeRegistered(aApplicationPtr, "WsfBMEvalAssignment"))
   {
      script_types->Register(ut::make_unique<WsfScriptBMEvalAssignmentClass>("WsfBMEvalAssignment", script_types));
   }

   if (!IsClassTypeRegistered(aApplicationPtr, "WsfBMInterceptCalcResult"))
   {
      script_types->Register(ut::make_unique<WsfScriptBMInterceptCalcResultClass>("WsfBMInterceptCalcResult", script_types));
   }

   if (!IsClassTypeRegistered(aApplicationPtr, "WsfBMAuxDataZoneAttachment"))
   {
      script_types->Register(ut::make_unique<WsfScriptBMAuxDataZoneAttachmentClass>("WsfBMAuxDataZoneAttachment", script_types));
   }

   // Register types available in aux data
   if (!WsfAttributeContainer::GetPrototype("WsfBMAuxDataZoneAttachment"))
   {
      WsfAttributeContainer::RegisterPrototype("WsfBMAuxDataZoneAttachment", ut::make_unique<UtAttribute<WsfBMAuxDataZoneAttachment>>("WsfBMAuxDataZoneAttachment"));
   }
}

bool IsClassTypeRegistered(WsfApplication* aApplicationPtr, const std::string& type_name)
{
   return aApplicationPtr->GetScriptTypes()->GetClass(type_name) != nullptr;
}

// plugin types don't necessarily register themselves through the script manager. Therefore, to check plugin type registration,
// use this call.
// Class name in this case is the registered object type and type name is the object name registered
extern "C" WSF_IADS_C2_LIB_EXPORT bool IsApplicationExtensionRegistered(WsfApplication* aApplicationPtr, const std::string& name)
{
   return aApplicationPtr->FindExtension(name) != nullptr;
}
