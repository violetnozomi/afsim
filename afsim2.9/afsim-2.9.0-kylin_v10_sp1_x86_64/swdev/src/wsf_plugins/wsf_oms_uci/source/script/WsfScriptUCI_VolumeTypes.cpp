// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

//
#include "WsfScriptUCI_VolumeTypes.hpp"
//
//#include <string>
//
//#include "WsfUCI_Interface.hpp"
//#include "WsfUCI_Types.hpp"
//
//#include "UtMemory.hpp"
//#include "UtScriptEnvironment.hpp"
//#include "script/WsfScriptContext.hpp"
//#include "script/WsfScriptDefs.hpp"
//#include "WsfAntennaPattern.hpp"
//#include "WsfEM_Antenna.hpp"
//#include "WsfEM_Rcvr.hpp"
//#include "WsfSensor.hpp"
//
//namespace wsf
//{
//namespace script
//{
//
//UCI_POST_AirTargetVolumeCommandTypeClass::UCI_POST_AirTargetVolumeCommandTypeClass(const std::string& aClassName,
//                                                                                   UtScriptTypes*     aTypesPtr)
//   : UtScriptClass(aClassName, aTypesPtr)
//{
//   SetClassName(aClassName);  
//
//  /* AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
//   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));*/
//}
//
////==========================================================================
////!
////UT_DEFINE_SCRIPT_METHOD(UCI_POST_AirTargetVolumeCommandTypeClass, UCI_POST_AirTargetVolumeCommandTypeClass, Construct_1, 1,
////   "UCI_POST_AirTargetVolumeCommand", "UCI_POST_AirTargetVolume")
////{
////   auto volumeCommandPtr = ut::make_unique<UCI_POST_AirTargetVolumeCommandType>(WsfScriptContext::GetPLATFORM(aContext));
////   auto airTargetVolumePtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_POST_AirTargetVolumeType>();
////   volumeCommandPtr->Get().setPOST_AirTargetVolumeCommandTypeChoiceOrdinal(uci::type::POST_AirTargetVolumeCommandType::POST_AIRTARGETVOLUMECOMMANDTYPE_CHOICE_AIRVOLUME);
////   volumeCommandPtr->Get().setAirVolume(airTargetVolumePtr->Get());
////   aReturnVal.SetPointer(new UtScriptRef(volumeCommandPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
////}
////
////UT_DEFINE_SCRIPT_METHOD(UCI_POST_AirTargetVolumeCommandTypeClass, UCI_POST_AirTargetVolumeCommandTypeClass, Construct_2, 1,
////   "UCI_POST_AirTargetVolumeCommand", "UCI_EntityId")
////{
////   auto volumeCommandPtr = ut::make_unique<UCI_POST_AirTargetVolumeCommandType>(WsfScriptContext::GetPLATFORM(aContext));
////   auto entityIdPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_EntityID_Type>();
////   volumeCommandPtr->Get().setPOST_AirTargetVolumeCommandTypeChoiceOrdinal(uci::type::POST_AirTargetVolumeCommandType::POST_AIRTARGETVOLUMECOMMANDTYPE_CHOICE_ENTITYID);
////   volumeCommandPtr->Get().setEntityID(entityIdPtr->Get());
////   aReturnVal.SetPointer(new UtScriptRef(volumeCommandPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
////}
//
////=============================================================================================================================================
//UCI_POST_AirTargetVolumeTypeClass::UCI_POST_AirTargetVolumeTypeClass(const std::string& aClassName,
//                                                                     UtScriptTypes*     aScriptTypesPtr)
//   : UtScriptClass(aClassName, aScriptTypesPtr)
//{
//   SetClassName(aClassName);
//
//   AddStaticMethod(ut::make_unique<Construct>());
//}
//
//UT_DEFINE_SCRIPT_METHOD(UCI_POST_AirTargetVolumeTypeClass, UCI_POST_AirTargetVolumeTypeClass, Construct, 1,
//   "UCI_POST_AirTargetVolume", "UCI_POST_AirVolumeSensorReferenced")
//{
//   auto volumePtr = ut::make_unique<UCI_POST_AirTargetVolumeType>(WsfScriptContext::GetPLATFORM(aContext));
//   auto sensorReferencedPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_POST_AirVolumeSensorReferencedType>();
//   auto ordinal = uci::type::POST_AirTargetVolumeType::POST_AIRTARGETVOLUMETYPE_CHOICE_AIRVOLUMESENSORREFERENCED;
//   volumePtr->Get().setPOST_AirTargetVolumeTypeChoiceOrdinal(ordinal);
//   volumePtr->Get().setAirVolumeSensorReferenced(sensorReferencedPtr->Get());
//   aReturnVal.SetPointer(new UtScriptRef(volumePtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
//}
//
////=============================================================================================================================================
//UCI_POST_AirVolumeSensorReferencedTypeClass::UCI_POST_AirVolumeSensorReferencedTypeClass(const std::string& aClassName,
//                                                                                         UtScriptTypes*     aScriptTypesPtr)
//   : UtScriptClass(aClassName, aScriptTypesPtr)
//{
//   SetClassName(aClassName);
//
//   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
//   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));
//}
//
//UT_DEFINE_SCRIPT_METHOD(UCI_POST_AirVolumeSensorReferencedTypeClass, UCI_POST_AirVolumeSensorReferencedType, Construct_1, 2, "UCI_POST_AirVolumeSensorReferenced", "WsfSensor, int")
//{
//   auto sensorPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfSensor>();
//   auto index = aVarArgs[1].GetInt();
//   auto sensorReferencedPtr = ut::make_unique<UCI_POST_AirVolumeSensorReferencedType>(WsfScriptContext::GetPLATFORM(aContext));
//   if (sensorPtr != nullptr &&
//       index >= 0 &&
//       index < sensorPtr->GetEM_RcvrCount())
//   {
//      auto rcvr = sensorPtr->GetEM_Rcvr(index);
//      auto& sensorReferenced = sensorReferencedPtr->Get();
//      sensorReferenced.setAzimuthScanWidth(rcvr.GetAzimuthBeamwidth());
//      sensorReferenced.setElevationScanWidth(rcvr.GetElevationBeamwidth());
//      auto stabilization = rcvr.GetAntenna()->GetScanStabilization();
//      sensorReferenced.setRollStabilized(stabilization == WsfEM_Antenna::ScanStabilization::cSS_BOTH ||
//                                         stabilization == WsfEM_Antenna::ScanStabilization::cSS_ROLL);
//      sensorReferenced.getElevationScanStabilization().setValue(uci::type::ElevationScanStabilizationEnum::UCI_BODY);
//      sensorReferenced.getAzimuthScanStabilization().setValue(uci::type::LOS_ReferenceEnum::UCI_BODY);
//      sensorReferenced.setMinRangeOfInterest(rcvr.GetAntenna()->GetMinimumRange());
//      sensorReferenced.setMaxRangeOfInterest(rcvr.GetAntenna()->GetMaximumRange());
//      sensorReferenced.setAzimuthScanCenter(sensorPtr->GetPitch());
//      sensorReferenced.setElevationScanCenter(sensorPtr->GetYaw());
//      aReturnVal.SetPointer(new UtScriptRef(sensorReferencedPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
//   }
//}
//
////Should only be used for debugging purposes
//UT_DEFINE_SCRIPT_METHOD(UCI_POST_AirVolumeSensorReferencedTypeClass, UCI_POST_AirVolumeSensorReferencedType, Construct_2, 7, "UCI_POST_AirVolumeSensorReferenced",
//   "double, double, double, double, double, double, bool")
//{
//   auto sensorReferencedPtr = ut::make_unique<UCI_POST_AirVolumeSensorReferencedType>(WsfScriptContext::GetPLATFORM(aContext));
//   auto azScanWidth = aVarArgs[0].GetDouble();
//   auto azScanCenter = aVarArgs[1].GetDouble();
//   auto elScanWidth = aVarArgs[2].GetDouble();
//   auto elScanCenter = aVarArgs[3].GetDouble();
//   auto maxROI = aVarArgs[4].GetDouble();
//   auto minROI = aVarArgs[5].GetDouble();
//   auto stabilized = aVarArgs[6].GetBool();
//
//   auto& sensorReferenced = sensorReferencedPtr->Get();
//   sensorReferenced.setAzimuthScanWidth(azScanWidth);
//   sensorReferenced.setElevationScanWidth(elScanWidth);
//   sensorReferenced.setRollStabilized(stabilized);
//   sensorReferenced.getElevationScanStabilization().setValue(uci::type::ElevationScanStabilizationEnum::UCI_BODY);
//   sensorReferenced.getAzimuthScanStabilization().setValue(uci::type::LOS_ReferenceEnum::UCI_BODY);
//   sensorReferenced.setMinRangeOfInterest(minROI);
//   sensorReferenced.setMaxRangeOfInterest(maxROI);
//   sensorReferenced.setAzimuthScanCenter(azScanCenter);
//   sensorReferenced.setElevationScanCenter(elScanCenter);
//   aReturnVal.SetPointer(new UtScriptRef(sensorReferencedPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
//}
//
//} // script
//} // wsf
