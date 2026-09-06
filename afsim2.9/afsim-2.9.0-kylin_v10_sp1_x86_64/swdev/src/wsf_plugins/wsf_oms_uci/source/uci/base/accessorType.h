// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* ‒  DISTRIBUTION STATEMENT D.  Distribution authorized to the Department
*    of Defense and U.S. DoD contractors only; Critical Technology; 17 Sep 2015.
*    Other requests shall be referred to Air Force Rapid Capabilities Office,
*    Bolling AFB, Washington DC 20032-6400. 
*
* EXPORT CONTROL:
*    WARNING - ITAR CONTROLLED - US ONLY
*    This distribution contains technical data whose export is restricted by
*    the Arms Export Control Act (Title 22, U.S.C., Sec. 2751 et seq. or the
*    Export Administration Act of 1979 as amended Title 50, U.S.C., App.
*    2401-2420 et seq.), as amended. Violation of these export laws are subject
*    to severe criminal penalties.  Disseminate in accordance with provisions of DoD
*    Directive 5230.25.
*/
#ifndef Uci__Base__AccessorType_h
#define Uci__Base__AccessorType_h 1

#if !defined(SysLibStdint_h)
#  include <stdint.h>
//  @cond
#  define SysLibStdint_h 1
//  @endcond
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all base data types are declared
   namespace base {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  The AccessorType data type. This is the data type of all Accessor Type constants.
         typedef uint32_t AccessorType;

         static const AccessorType null = 0;

         static const AccessorType uuid = 1;
         static const AccessorType booleanAccessor = 2;
         static const AccessorType integerAccessor = 3;
         static const AccessorType longAccessor = 4;
         static const AccessorType intAccessor = 5;
         static const AccessorType shortAccessor = 6;
         static const AccessorType byteAccessor = 7;
         static const AccessorType unsignedLongAccessor = 8;
         static const AccessorType unsignedIntAccessor = 9;
         static const AccessorType unsignedShortAccessor = 10;
         static const AccessorType unsignedByteAccessor = 11;
         static const AccessorType doubleAccessor = 12;
         static const AccessorType floatAccessor = 13;
         static const AccessorType durationAccessor = 14;
         static const AccessorType timeAccessor = 15;
         static const AccessorType dateTimeAccessor = 16;
         static const AccessorType dateAccessor = 17;
      } // Namespace: accessorType
   } // Namespace: base
} // Namespace: uci
//  The namespace in which all XML Schema Definition data types are declared
namespace xs {

   //  The namespace in which all accessor types are declared
   namespace accessorType {

      static const uci::base::accessorType::AccessorType anyUri = 18;
      static const uci::base::accessorType::AccessorType notation = 19;
      static const uci::base::accessorType::AccessorType qName = 20;
      static const uci::base::accessorType::AccessorType string = 21;
      static const uci::base::accessorType::AccessorType normalizedString = 22;
      static const uci::base::accessorType::AccessorType token = 23;
      static const uci::base::accessorType::AccessorType language = 24;
      static const uci::base::accessorType::AccessorType name = 25;
      static const uci::base::accessorType::AccessorType ncName = 26;
      static const uci::base::accessorType::AccessorType id = 27;
      static const uci::base::accessorType::AccessorType entity = 28;
      static const uci::base::accessorType::AccessorType idRef = 29;
      static const uci::base::accessorType::AccessorType nmToken = 30;
      static const uci::base::accessorType::AccessorType dateTimeStamp = 31;
      static const uci::base::accessorType::AccessorType dayTimeDuration = 32;
      static const uci::base::accessorType::AccessorType yearMonthDuration = 33;
      static const uci::base::accessorType::AccessorType gDay = 34;
      static const uci::base::accessorType::AccessorType gMonth = 35;
      static const uci::base::accessorType::AccessorType gMonthDay = 36;
      static const uci::base::accessorType::AccessorType gYear = 37;
      static const uci::base::accessorType::AccessorType gYearMonth = 38;
      static const uci::base::accessorType::AccessorType decimal = 39;
      static const uci::base::accessorType::AccessorType nonNegativeInteger = 40;
      static const uci::base::accessorType::AccessorType positiveInteger = 41;
      static const uci::base::accessorType::AccessorType nonPositiveInteger = 42;
      static const uci::base::accessorType::AccessorType negativeInteger = 43;
      static const uci::base::accessorType::AccessorType base64Binary = 44;
      static const uci::base::accessorType::AccessorType hexBinary = 45;
      static const uci::base::accessorType::AccessorType entities = 46;
      static const uci::base::accessorType::AccessorType idRefs = 47;
      static const uci::base::accessorType::AccessorType nmTokens = 48;
   } // Namespace: accessorType
} // Namespace: xs
//  
namespace ism25x {

   //  The namespace in which all generated data types are declared
   namespace type {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ism25x::type::CVEnumISM25XValues Enumeration
         static const uci::base::accessorType::AccessorType cVEnumISM25XValues = 49;

         //  This accessor type is used to identify the ism25x::type::CVEnumISM25X Accessor
         static const uci::base::accessorType::AccessorType cVEnumISM25X = 50;

      } // Namespace: accessorType
   } // Namespace: type
} // Namespace: ism25x
//  
namespace ismatomic {

   //  The namespace in which all generated data types are declared
   namespace type {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ismatomic::type::CVEnumISMatomicEnergyMarkingsValues Accessor
         static const uci::base::accessorType::AccessorType cVEnumISMatomicEnergyMarkingsValues = 51;

      } // Namespace: accessorType
      //  
      namespace cVEnumISMatomicEnergyMarkingsValues {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the
            //  ismatomic::type::CVEnumISMatomicEnergyMarkingsValues::CVEnumISMatomicEnergyMarkingsValuesMember1 Accessor
            static const uci::base::accessorType::AccessorType cVEnumISMatomicEnergyMarkingsValuesMember1 = 52;

            //  This accessor type is used to identify the
            //  ismatomic::type::CVEnumISMatomicEnergyMarkingsValues::CVEnumISMatomicEnergyMarkingsValuesMember2 Enumeration
            static const uci::base::accessorType::AccessorType cVEnumISMatomicEnergyMarkingsValuesMember2 = 53;

         } // Namespace: accessorType
      } // Namespace: cVEnumISMatomicEnergyMarkingsValues
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ismatomic::type::CVEnumISMatomicEnergyMarkings Accessor
         static const uci::base::accessorType::AccessorType cVEnumISMatomicEnergyMarkings = 54;

      } // Namespace: accessorType
   } // Namespace: type
} // Namespace: ismatomic
//  
namespace ismclassall {

   //  The namespace in which all generated data types are declared
   namespace type {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ismclassall::type::CVEnumISMClassificationAll Enumeration
         static const uci::base::accessorType::AccessorType cVEnumISMClassificationAll = 55;

      } // Namespace: accessorType
   } // Namespace: type
} // Namespace: ismclassall
//  
namespace ismdissem {

   //  The namespace in which all generated data types are declared
   namespace type {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ismdissem::type::CVEnumISMDissemValues Enumeration
         static const uci::base::accessorType::AccessorType cVEnumISMDissemValues = 56;

         //  This accessor type is used to identify the ismdissem::type::CVEnumISMDissem Accessor
         static const uci::base::accessorType::AccessorType cVEnumISMDissem = 57;

      } // Namespace: accessorType
   } // Namespace: type
} // Namespace: ismdissem
//  
namespace ismfgio {

   //  The namespace in which all generated data types are declared
   namespace type {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ismfgio::type::CVEnumISMFGIOpenValues Enumeration
         static const uci::base::accessorType::AccessorType cVEnumISMFGIOpenValues = 58;

         //  This accessor type is used to identify the ismfgio::type::CVEnumISMFGIOpen Accessor
         static const uci::base::accessorType::AccessorType cVEnumISMFGIOpen = 59;

      } // Namespace: accessorType
   } // Namespace: type
} // Namespace: ismfgio
//  
namespace ismfgip {

   //  The namespace in which all generated data types are declared
   namespace type {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ismfgip::type::CVEnumISMFGIProtectedValues Enumeration
         static const uci::base::accessorType::AccessorType cVEnumISMFGIProtectedValues = 60;

         //  This accessor type is used to identify the ismfgip::type::CVEnumISMFGIProtected Accessor
         static const uci::base::accessorType::AccessorType cVEnumISMFGIProtected = 61;

      } // Namespace: accessorType
   } // Namespace: type
} // Namespace: ismfgip
//  
namespace ismnonic {

   //  The namespace in which all generated data types are declared
   namespace type {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ismnonic::type::CVEnumISMNonICValues Accessor
         static const uci::base::accessorType::AccessorType cVEnumISMNonICValues = 62;

      } // Namespace: accessorType
      //  
      namespace cVEnumISMNonICValues {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the ismnonic::type::CVEnumISMNonICValues::CVEnumISMNonICValuesMember1 Accessor
            static const uci::base::accessorType::AccessorType cVEnumISMNonICValuesMember1 = 63;

            //  This accessor type is used to identify the ismnonic::type::CVEnumISMNonICValues::CVEnumISMNonICValuesMember2
            //  Enumeration
            static const uci::base::accessorType::AccessorType cVEnumISMNonICValuesMember2 = 64;

         } // Namespace: accessorType
      } // Namespace: cVEnumISMNonICValues
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ismnonic::type::CVEnumISMNonIC Accessor
         static const uci::base::accessorType::AccessorType cVEnumISMNonIC = 65;

      } // Namespace: accessorType
   } // Namespace: type
} // Namespace: ismnonic
//  
namespace ismnonus {

   //  The namespace in which all generated data types are declared
   namespace type {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ismnonus::type::CVEnumISMNonUSControlsValues Enumeration
         static const uci::base::accessorType::AccessorType cVEnumISMNonUSControlsValues = 66;

         //  This accessor type is used to identify the ismnonus::type::CVEnumISMNonUSControls Accessor
         static const uci::base::accessorType::AccessorType cVEnumISMNonUSControls = 67;

      } // Namespace: accessorType
   } // Namespace: type
} // Namespace: ismnonus
//  
namespace ismownpro {

   //  The namespace in which all generated data types are declared
   namespace type {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ismownpro::type::CVEnumISMOwnerProducerValues Enumeration
         static const uci::base::accessorType::AccessorType cVEnumISMOwnerProducerValues = 68;

         //  This accessor type is used to identify the ismownpro::type::CVEnumISMOwnerProducer Accessor
         static const uci::base::accessorType::AccessorType cVEnumISMOwnerProducer = 69;

      } // Namespace: accessorType
   } // Namespace: type
} // Namespace: ismownpro
//  
namespace ismpoctyp {

   //  The namespace in which all generated data types are declared
   namespace type {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ismpoctyp::type::CVEnumISMPocTypeValues Enumeration
         static const uci::base::accessorType::AccessorType cVEnumISMPocTypeValues = 70;

         //  This accessor type is used to identify the ismpoctyp::type::CVEnumISMPocType Accessor
         static const uci::base::accessorType::AccessorType cVEnumISMPocType = 71;

      } // Namespace: accessorType
   } // Namespace: type
} // Namespace: ismpoctyp
//  
namespace ismrelto {

   //  The namespace in which all generated data types are declared
   namespace type {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ismrelto::type::CVEnumISMRelToValues Enumeration
         static const uci::base::accessorType::AccessorType cVEnumISMRelToValues = 72;

         //  This accessor type is used to identify the ismrelto::type::CVEnumISMRelTo Accessor
         static const uci::base::accessorType::AccessorType cVEnumISMRelTo = 73;

      } // Namespace: accessorType
   } // Namespace: type
} // Namespace: ismrelto
//  
namespace ismsar {

   //  The namespace in which all generated data types are declared
   namespace type {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ismsar::type::CVEnumISMSARValues Accessor
         static const uci::base::accessorType::AccessorType cVEnumISMSARValues = 74;

         //  This accessor type is used to identify the ismsar::type::CVEnumISMSAR Accessor
         static const uci::base::accessorType::AccessorType cVEnumISMSAR = 75;

      } // Namespace: accessorType
   } // Namespace: type
} // Namespace: ismsar
//  
namespace ismscicon {

   //  The namespace in which all generated data types are declared
   namespace type {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ismscicon::type::CVEnumISMSCIControlsValues Accessor
         static const uci::base::accessorType::AccessorType cVEnumISMSCIControlsValues = 76;

      } // Namespace: accessorType
      //  
      namespace cVEnumISMSCIControlsValues {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the
            //  ismscicon::type::CVEnumISMSCIControlsValues::CVEnumISMSCIControlsValuesMember1 Accessor
            static const uci::base::accessorType::AccessorType cVEnumISMSCIControlsValuesMember1 = 77;

            //  This accessor type is used to identify the
            //  ismscicon::type::CVEnumISMSCIControlsValues::CVEnumISMSCIControlsValuesMember2 Enumeration
            static const uci::base::accessorType::AccessorType cVEnumISMSCIControlsValuesMember2 = 78;

         } // Namespace: accessorType
      } // Namespace: cVEnumISMSCIControlsValues
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ismscicon::type::CVEnumISMSCIControls Accessor
         static const uci::base::accessorType::AccessorType cVEnumISMSCIControls = 79;

      } // Namespace: accessorType
   } // Namespace: type
} // Namespace: ismscicon
//  
namespace ism {

   //  The namespace in which all generated data types are declared
   namespace type {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the ism::type::AtomicEnergyMarkings Accessor
         static const uci::base::accessorType::AccessorType atomicEnergyMarkings = 80;

         //  This accessor type is used to identify the ism::type::Classification Accessor
         static const uci::base::accessorType::AccessorType classification = 81;

         //  This accessor type is used to identify the ism::type::ClassificationReason Accessor
         static const uci::base::accessorType::AccessorType classificationReason = 82;

         //  This accessor type is used to identify the ism::type::ClassifiedBy Accessor
         static const uci::base::accessorType::AccessorType classifiedBy = 83;

         //  This accessor type is used to identify the ism::type::CompilationReason Accessor
         static const uci::base::accessorType::AccessorType compilationReason = 84;

         //  This accessor type is used to identify the ism::type::DeclassDate Accessor
         static const uci::base::accessorType::AccessorType declassDate = 85;

         //  This accessor type is used to identify the ism::type::DeclassEvent Accessor
         static const uci::base::accessorType::AccessorType declassEvent = 86;

         //  This accessor type is used to identify the ism::type::DeclassException Accessor
         static const uci::base::accessorType::AccessorType declassException = 87;

         //  This accessor type is used to identify the ism::type::DerivativelyClassifiedBy Accessor
         static const uci::base::accessorType::AccessorType derivativelyClassifiedBy = 88;

         //  This accessor type is used to identify the ism::type::DerivedFrom Accessor
         static const uci::base::accessorType::AccessorType derivedFrom = 89;

         //  This accessor type is used to identify the ism::type::DisplayOnlyTo Accessor
         static const uci::base::accessorType::AccessorType displayOnlyTo = 90;

         //  This accessor type is used to identify the ism::type::DisseminationControls Accessor
         static const uci::base::accessorType::AccessorType disseminationControls = 91;

         //  This accessor type is used to identify the ism::type::FGIsourceOpen Accessor
         static const uci::base::accessorType::AccessorType fGIsourceOpen = 92;

         //  This accessor type is used to identify the ism::type::FGIsourceProtected Accessor
         static const uci::base::accessorType::AccessorType fGIsourceProtected = 93;

         //  This accessor type is used to identify the ism::type::NonICmarkings Accessor
         static const uci::base::accessorType::AccessorType nonICmarkings = 94;

         //  This accessor type is used to identify the ism::type::NonUSControls Accessor
         static const uci::base::accessorType::AccessorType nonUSControls = 95;

         //  This accessor type is used to identify the ism::type::OwnerProducer Accessor
         static const uci::base::accessorType::AccessorType ownerProducer = 96;

         //  This accessor type is used to identify the ism::type::ReleasableTo Accessor
         static const uci::base::accessorType::AccessorType releasableTo = 97;

         //  This accessor type is used to identify the ism::type::SARIdentifier Accessor
         static const uci::base::accessorType::AccessorType sARIdentifier = 98;

         //  This accessor type is used to identify the ism::type::SCIcontrols Accessor
         static const uci::base::accessorType::AccessorType sCIcontrols = 99;

         //  This accessor type is used to identify the ism::type::SecurityAttributesGroup Accessor
         static const uci::base::accessorType::AccessorType securityAttributesGroup = 100;

      } // Namespace: accessorType
   } // Namespace: type
} // Namespace: ism
//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::AboutType Accessor
         static const uci::base::accessorType::AccessorType aboutType = 101;

         //  This accessor type is used to identify the uci::type::Acceleration3D_Type Accessor
         static const uci::base::accessorType::AccessorType acceleration3D_Type = 102;

         //  This accessor type is used to identify the uci::type::ACFTB_Type Accessor
         static const uci::base::accessorType::AccessorType aCFTB_Type = 103;

      } // Namespace: accessorType
      //  
      namespace aCFTB_Type {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::ACFTB_Type::AC_MSN_ID Accessor
            static const uci::base::accessorType::AccessorType aC_MSN_ID = 104;

            //  This accessor type is used to identify the uci::type::ACFTB_Type::AC_TAIL_NO Accessor
            static const uci::base::accessorType::AccessorType aC_TAIL_NO = 105;

            //  This accessor type is used to identify the uci::type::ACFTB_Type::AC_TO Accessor
            static const uci::base::accessorType::AccessorType aC_TO = 106;

            //  This accessor type is used to identify the uci::type::ACFTB_Type::SENSOR_ID Accessor
            static const uci::base::accessorType::AccessorType sENSOR_ID = 107;

            //  This accessor type is used to identify the uci::type::ACFTB_Type::SCENE_SOURCE Accessor
            static const uci::base::accessorType::AccessorType sCENE_SOURCE = 108;

            //  This accessor type is used to identify the uci::type::ACFTB_Type::SCNUM Accessor
            static const uci::base::accessorType::AccessorType sCNUM = 109;

            //  This accessor type is used to identify the uci::type::ACFTB_Type::IMHOSTNO Accessor
            static const uci::base::accessorType::AccessorType iMHOSTNO = 110;

            //  This accessor type is used to identify the uci::type::ACFTB_Type::IMREQID Accessor
            static const uci::base::accessorType::AccessorType iMREQID = 111;

         } // Namespace: accessorType
      } // Namespace: aCFTB_Type
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::ACTDF_CollectionPlanType Accessor
         static const uci::base::accessorType::AccessorType aCTDF_CollectionPlanType = 112;

         //  This accessor type is used to identify the uci::type::ACTDF_TraceabilityType Accessor
         static const uci::base::accessorType::AccessorType aCTDF_TraceabilityType = 113;

      } // Namespace: accessorType
      //  
      namespace aCTDF_TraceabilityType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::ACTDF_TraceabilityType::ACTDF_MissionID Accessor
            static const uci::base::accessorType::AccessorType aCTDF_MissionID = 114;

            //  This accessor type is used to identify the uci::type::ACTDF_TraceabilityType::ACTDF_TaskID Accessor
            static const uci::base::accessorType::AccessorType aCTDF_TaskID = 115;

         } // Namespace: accessorType
      } // Namespace: aCTDF_TraceabilityType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::ActionID_Type Accessor
         static const uci::base::accessorType::AccessorType actionID_Type = 116;

         //  This accessor type is used to identify the uci::type::ActionType Accessor
         static const uci::base::accessorType::AccessorType actionType = 117;

         //  This accessor type is used to identify the uci::type::ActivationPathSegmentType Accessor
         static const uci::base::accessorType::AccessorType activationPathSegmentType = 118;

         //  This accessor type is used to identify the uci::type::ActivityActorType Accessor
         static const uci::base::accessorType::AccessorType activityActorType = 119;

         //  This accessor type is used to identify the uci::type::ActivityAgainstType Accessor
         static const uci::base::accessorType::AccessorType activityAgainstType = 120;

         //  This accessor type is used to identify the uci::type::ActivityBaseType Accessor
         static const uci::base::accessorType::AccessorType activityBaseType = 121;

         //  This accessor type is used to identify the uci::type::ActivityByType Accessor
         static const uci::base::accessorType::AccessorType activityByType = 122;

         //  This accessor type is used to identify the uci::type::ActivityCommandBaseType Accessor
         static const uci::base::accessorType::AccessorType activityCommandBaseType = 123;

         //  This accessor type is used to identify the uci::type::ActivityID_Type Accessor
         static const uci::base::accessorType::AccessorType activityID_Type = 124;

         //  This accessor type is used to identify the uci::type::ActivitySourceType Accessor
         static const uci::base::accessorType::AccessorType activitySourceType = 125;

         //  This accessor type is used to identify the uci::type::AIMIDB_Type Accessor
         static const uci::base::accessorType::AccessorType aIMIDB_Type = 126;

      } // Namespace: accessorType
      //  
      namespace aIMIDB_Type {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::AIMIDB_Type::FLIGHT_NO Accessor
            static const uci::base::accessorType::AccessorType fLIGHT_NO = 127;

            //  This accessor type is used to identify the uci::type::AIMIDB_Type::MISSION_NO Accessor
            static const uci::base::accessorType::AccessorType mISSION_NO = 128;

         } // Namespace: accessorType
      } // Namespace: aIMIDB_Type
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::AirDataType Accessor
         static const uci::base::accessorType::AccessorType airDataType = 129;

         //  This accessor type is used to identify the uci::type::AirSampleActivityMDT Accessor
         static const uci::base::accessorType::AccessorType airSampleActivityMDT = 130;

         //  This accessor type is used to identify the uci::type::AirSampleActivityMT Accessor
         static const uci::base::accessorType::AccessorType airSampleActivityMT = 131;

         //  This accessor type is used to identify the uci::type::AirSampleActivityQFT Accessor
         static const uci::base::accessorType::AccessorType airSampleActivityQFT = 132;

         //  This accessor type is used to identify the uci::type::AirSampleActivityType Accessor
         static const uci::base::accessorType::AccessorType airSampleActivityType = 133;

         //  This accessor type is used to identify the uci::type::AirSampleCapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType airSampleCapabilityCommandType = 134;

         //  This accessor type is used to identify the uci::type::AirSampleCapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType airSampleCapabilityMDT = 135;

         //  This accessor type is used to identify the uci::type::AirSampleCapabilityMT Accessor
         static const uci::base::accessorType::AccessorType airSampleCapabilityMT = 136;

         //  This accessor type is used to identify the uci::type::AirSampleCapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType airSampleCapabilityQFT = 137;

         //  This accessor type is used to identify the uci::type::AirSampleCapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType airSampleCapabilityStatusMDT = 138;

         //  This accessor type is used to identify the uci::type::AirSampleCapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType airSampleCapabilityStatusMT = 139;

         //  This accessor type is used to identify the uci::type::AirSampleCapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType airSampleCapabilityStatusQFT = 140;

         //  This accessor type is used to identify the uci::type::AirSampleCapabilityType Accessor
         static const uci::base::accessorType::AccessorType airSampleCapabilityType = 141;

         //  This accessor type is used to identify the uci::type::AirSampleCommandMDT Accessor
         static const uci::base::accessorType::AccessorType airSampleCommandMDT = 142;

         //  This accessor type is used to identify the uci::type::AirSampleCommandMT Accessor
         static const uci::base::accessorType::AccessorType airSampleCommandMT = 143;

         //  This accessor type is used to identify the uci::type::AirSampleCommandQFT Accessor
         static const uci::base::accessorType::AccessorType airSampleCommandQFT = 144;

         //  This accessor type is used to identify the uci::type::AirSampleCommandResponseType Accessor
         static const uci::base::accessorType::AccessorType airSampleCommandResponseType = 145;

         //  This accessor type is used to identify the uci::type::AirSampleCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType airSampleCommandStatusMT = 146;

         //  This accessor type is used to identify the uci::type::AirSampleCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType airSampleCommandStatusQFT = 147;

         //  This accessor type is used to identify the uci::type::AirSampleCommandType Accessor
         static const uci::base::accessorType::AccessorType airSampleCommandType = 148;

         //  This accessor type is used to identify the uci::type::AirSampleOptionsType Accessor
         static const uci::base::accessorType::AccessorType airSampleOptionsType = 149;

         //  This accessor type is used to identify the uci::type::AirSampleSettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType airSampleSettingsCommandMT = 150;

         //  This accessor type is used to identify the uci::type::AirSampleSettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType airSampleSettingsCommandQFT = 151;

         //  This accessor type is used to identify the uci::type::AirSampleSettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType airSampleSettingsCommandStatusMT = 152;

         //  This accessor type is used to identify the uci::type::AirSampleSettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType airSampleSettingsCommandStatusQFT = 153;

         //  This accessor type is used to identify the uci::type::AirSampleSpecificDataType Accessor
         static const uci::base::accessorType::AccessorType airSampleSpecificDataType = 154;

         //  This accessor type is used to identify the uci::type::AirSampleTaskBaseType Accessor
         static const uci::base::accessorType::AccessorType airSampleTaskBaseType = 155;

         //  This accessor type is used to identify the uci::type::AirSampleTaskType Accessor
         static const uci::base::accessorType::AccessorType airSampleTaskType = 156;

         //  This accessor type is used to identify the uci::type::AirTargetVolumeCommandType Accessor
         static const uci::base::accessorType::AccessorType airTargetVolumeCommandType = 157;

         //  This accessor type is used to identify the uci::type::AirTargetVolumeType Accessor
         static const uci::base::accessorType::AccessorType airTargetVolumeType = 158;

         //  This accessor type is used to identify the uci::type::AirVolumeSensorReferencedType Accessor
         static const uci::base::accessorType::AccessorType airVolumeSensorReferencedType = 159;

         //  This accessor type is used to identify the uci::type::AIS_Type Accessor
         static const uci::base::accessorType::AccessorType aIS_Type = 160;

      } // Namespace: accessorType
      //  
      namespace aIS_Type {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::AIS_Type::MMSI_Number Accessor
            static const uci::base::accessorType::AccessorType mMSI_Number = 161;

            //  This accessor type is used to identify the uci::type::AIS_Type::IMO_Number Accessor
            static const uci::base::accessorType::AccessorType iMO_Number = 162;

            //  This accessor type is used to identify the uci::type::AIS_Type::VesselName Accessor
            static const uci::base::accessorType::AccessorType vesselName = 163;

            //  This accessor type is used to identify the uci::type::AIS_Type::CallSign Accessor
            static const uci::base::accessorType::AccessorType callSign = 164;

         } // Namespace: accessorType
      } // Namespace: aIS_Type
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::AllocatedTaskConstraintType Accessor
         static const uci::base::accessorType::AccessorType allocatedTaskConstraintType = 165;

         //  This accessor type is used to identify the uci::type::AllocatedTaskType Accessor
         static const uci::base::accessorType::AccessorType allocatedTaskType = 166;

         //  This accessor type is used to identify the uci::type::AltitudeConstraintsType Accessor
         static const uci::base::accessorType::AccessorType altitudeConstraintsType = 167;

         //  This accessor type is used to identify the uci::type::AltitudeRadialPairType Accessor
         static const uci::base::accessorType::AccessorType altitudeRadialPairType = 168;

         //  This accessor type is used to identify the uci::type::AltitudeRangePairType Accessor
         static const uci::base::accessorType::AccessorType altitudeRangePairType = 169;

         //  This accessor type is used to identify the uci::type::AltitudeRangeType Accessor
         static const uci::base::accessorType::AccessorType altitudeRangeType = 170;

         //  This accessor type is used to identify the uci::type::AmplifyingInformationType Accessor
         static const uci::base::accessorType::AccessorType amplifyingInformationType = 171;

         //  This accessor type is used to identify the uci::type::AMTI_ActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType aMTI_ActivityCommandType = 172;

         //  This accessor type is used to identify the uci::type::AMTI_ActivityMDT Accessor
         static const uci::base::accessorType::AccessorType aMTI_ActivityMDT = 173;

         //  This accessor type is used to identify the uci::type::AMTI_ActivityMT Accessor
         static const uci::base::accessorType::AccessorType aMTI_ActivityMT = 174;

         //  This accessor type is used to identify the uci::type::AMTI_ActivityQFT Accessor
         static const uci::base::accessorType::AccessorType aMTI_ActivityQFT = 175;

         //  This accessor type is used to identify the uci::type::AMTI_ActivityType Accessor
         static const uci::base::accessorType::AccessorType aMTI_ActivityType = 176;

         //  This accessor type is used to identify the uci::type::AMTI_CapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType aMTI_CapabilityCommandType = 177;

         //  This accessor type is used to identify the uci::type::AMTI_CapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType aMTI_CapabilityMDT = 178;

         //  This accessor type is used to identify the uci::type::AMTI_CapabilityMT Accessor
         static const uci::base::accessorType::AccessorType aMTI_CapabilityMT = 179;

         //  This accessor type is used to identify the uci::type::AMTI_CapabilityOptionsType Accessor
         static const uci::base::accessorType::AccessorType aMTI_CapabilityOptionsType = 180;

         //  This accessor type is used to identify the uci::type::AMTI_CapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType aMTI_CapabilityQFT = 181;

         //  This accessor type is used to identify the uci::type::AMTI_CapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType aMTI_CapabilityStatusMDT = 182;

         //  This accessor type is used to identify the uci::type::AMTI_CapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType aMTI_CapabilityStatusMT = 183;

         //  This accessor type is used to identify the uci::type::AMTI_CapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType aMTI_CapabilityStatusQFT = 184;

         //  This accessor type is used to identify the uci::type::AMTI_CapabilityType Accessor
         static const uci::base::accessorType::AccessorType aMTI_CapabilityType = 185;

         //  This accessor type is used to identify the uci::type::AMTI_CollectionConstraintsType Accessor
         static const uci::base::accessorType::AccessorType aMTI_CollectionConstraintsType = 186;

         //  This accessor type is used to identify the uci::type::AMTI_CommandMDT Accessor
         static const uci::base::accessorType::AccessorType aMTI_CommandMDT = 187;

         //  This accessor type is used to identify the uci::type::AMTI_CommandMT Accessor
         static const uci::base::accessorType::AccessorType aMTI_CommandMT = 188;

         //  This accessor type is used to identify the uci::type::AMTI_CommandQFT Accessor
         static const uci::base::accessorType::AccessorType aMTI_CommandQFT = 189;

         //  This accessor type is used to identify the uci::type::AMTI_CommandResponseType Accessor
         static const uci::base::accessorType::AccessorType aMTI_CommandResponseType = 190;

         //  This accessor type is used to identify the uci::type::AMTI_CommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType aMTI_CommandStatusMT = 191;

         //  This accessor type is used to identify the uci::type::AMTI_CommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType aMTI_CommandStatusQFT = 192;

         //  This accessor type is used to identify the uci::type::AMTI_CommandType Accessor
         static const uci::base::accessorType::AccessorType aMTI_CommandType = 193;

         //  This accessor type is used to identify the uci::type::AMTI_SettingsCommandMDT Accessor
         static const uci::base::accessorType::AccessorType aMTI_SettingsCommandMDT = 194;

         //  This accessor type is used to identify the uci::type::AMTI_SettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType aMTI_SettingsCommandMT = 195;

         //  This accessor type is used to identify the uci::type::AMTI_SettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType aMTI_SettingsCommandQFT = 196;

         //  This accessor type is used to identify the uci::type::AMTI_SettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType aMTI_SettingsCommandStatusMT = 197;

         //  This accessor type is used to identify the uci::type::AMTI_SettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType aMTI_SettingsCommandStatusQFT = 198;

         //  This accessor type is used to identify the uci::type::AMTI_SpecificDataType Accessor
         static const uci::base::accessorType::AccessorType aMTI_SpecificDataType = 199;

         //  This accessor type is used to identify the uci::type::AMTI_TargetType Accessor
         static const uci::base::accessorType::AccessorType aMTI_TargetType = 200;

         //  This accessor type is used to identify the uci::type::AMTI_TaskBaseType Accessor
         static const uci::base::accessorType::AccessorType aMTI_TaskBaseType = 201;

         //  This accessor type is used to identify the uci::type::AMTI_TaskType Accessor
         static const uci::base::accessorType::AccessorType aMTI_TaskType = 202;

         //  This accessor type is used to identify the uci::type::AngleHalfPairType Accessor
         static const uci::base::accessorType::AccessorType angleHalfPairType = 203;

         //  This accessor type is used to identify the uci::type::AnglePairType Accessor
         static const uci::base::accessorType::AccessorType anglePairType = 204;

         //  This accessor type is used to identify the uci::type::AngleQuarterPairType Accessor
         static const uci::base::accessorType::AccessorType angleQuarterPairType = 205;

         //  This accessor type is used to identify the uci::type::AntennaAllocationAndBeamConfigurationsType Accessor
         static const uci::base::accessorType::AccessorType antennaAllocationAndBeamConfigurationsType = 206;

         //  This accessor type is used to identify the uci::type::AntennaAndResourceInstanceType Accessor
         static const uci::base::accessorType::AccessorType antennaAndResourceInstanceType = 207;

         //  This accessor type is used to identify the uci::type::AntennaBoresightVelocityType Accessor
         static const uci::base::accessorType::AccessorType antennaBoresightVelocityType = 208;

         //  This accessor type is used to identify the uci::type::AntennaID_Type Accessor
         static const uci::base::accessorType::AccessorType antennaID_Type = 209;

         //  This accessor type is used to identify the uci::type::AntennaKinematicsType Accessor
         static const uci::base::accessorType::AccessorType antennaKinematicsType = 210;

         //  This accessor type is used to identify the uci::type::AntennaPerformanceProfileType Accessor
         static const uci::base::accessorType::AccessorType antennaPerformanceProfileType = 211;

         //  This accessor type is used to identify the uci::type::AntennaResourceAndBeamConfigurationsType Accessor
         static const uci::base::accessorType::AccessorType antennaResourceAndBeamConfigurationsType = 212;

         //  This accessor type is used to identify the uci::type::AntennaResourceChoiceType Accessor
         static const uci::base::accessorType::AccessorType antennaResourceChoiceType = 213;

         //  This accessor type is used to identify the uci::type::AntennaResourceInstanceIdentifierType Accessor
         static const uci::base::accessorType::AccessorType antennaResourceInstanceIdentifierType = 214;

         //  This accessor type is used to identify the uci::type::AntennaResourceInstanceStatusType Accessor
         static const uci::base::accessorType::AccessorType antennaResourceInstanceStatusType = 215;

         //  This accessor type is used to identify the uci::type::AntennaResourceTypeIdentifierType Accessor
         static const uci::base::accessorType::AccessorType antennaResourceTypeIdentifierType = 216;

         //  This accessor type is used to identify the uci::type::AntennaStatusMDT Accessor
         static const uci::base::accessorType::AccessorType antennaStatusMDT = 217;

         //  This accessor type is used to identify the uci::type::AntennaStatusMT Accessor
         static const uci::base::accessorType::AccessorType antennaStatusMT = 218;

         //  This accessor type is used to identify the uci::type::AntennaStatusQFT Accessor
         static const uci::base::accessorType::AccessorType antennaStatusQFT = 219;

         //  This accessor type is used to identify the uci::type::AntennaType Accessor
         static const uci::base::accessorType::AccessorType antennaType = 220;

         //  This accessor type is used to identify the uci::type::AO_ActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType aO_ActivityCommandType = 221;

         //  This accessor type is used to identify the uci::type::AO_ActivityMDT Accessor
         static const uci::base::accessorType::AccessorType aO_ActivityMDT = 222;

         //  This accessor type is used to identify the uci::type::AO_ActivityMT Accessor
         static const uci::base::accessorType::AccessorType aO_ActivityMT = 223;

         //  This accessor type is used to identify the uci::type::AO_ActivityQFT Accessor
         static const uci::base::accessorType::AccessorType aO_ActivityQFT = 224;

         //  This accessor type is used to identify the uci::type::AO_ActivityType Accessor
         static const uci::base::accessorType::AccessorType aO_ActivityType = 225;

         //  This accessor type is used to identify the uci::type::AO_CapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType aO_CapabilityCommandType = 226;

         //  This accessor type is used to identify the uci::type::AO_CapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType aO_CapabilityMDT = 227;

         //  This accessor type is used to identify the uci::type::AO_CapabilityMT Accessor
         static const uci::base::accessorType::AccessorType aO_CapabilityMT = 228;

         //  This accessor type is used to identify the uci::type::AO_CapabilityOptionsType Accessor
         static const uci::base::accessorType::AccessorType aO_CapabilityOptionsType = 229;

         //  This accessor type is used to identify the uci::type::AO_CapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType aO_CapabilityQFT = 230;

         //  This accessor type is used to identify the uci::type::AO_CapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType aO_CapabilityStatusMDT = 231;

         //  This accessor type is used to identify the uci::type::AO_CapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType aO_CapabilityStatusMT = 232;

         //  This accessor type is used to identify the uci::type::AO_CapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType aO_CapabilityStatusQFT = 233;

         //  This accessor type is used to identify the uci::type::AO_CapabilityType Accessor
         static const uci::base::accessorType::AccessorType aO_CapabilityType = 234;

         //  This accessor type is used to identify the uci::type::AO_CodeType Accessor
         static const uci::base::accessorType::AccessorType aO_CodeType = 235;

         //  This accessor type is used to identify the uci::type::AO_CommandMDT Accessor
         static const uci::base::accessorType::AccessorType aO_CommandMDT = 236;

         //  This accessor type is used to identify the uci::type::AO_CommandMT Accessor
         static const uci::base::accessorType::AccessorType aO_CommandMT = 237;

         //  This accessor type is used to identify the uci::type::AO_CommandQFT Accessor
         static const uci::base::accessorType::AccessorType aO_CommandQFT = 238;

         //  This accessor type is used to identify the uci::type::AO_CommandResponseType Accessor
         static const uci::base::accessorType::AccessorType aO_CommandResponseType = 239;

         //  This accessor type is used to identify the uci::type::AO_CommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType aO_CommandStatusMT = 240;

         //  This accessor type is used to identify the uci::type::AO_CommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType aO_CommandStatusQFT = 241;

         //  This accessor type is used to identify the uci::type::AO_CommandType Accessor
         static const uci::base::accessorType::AccessorType aO_CommandType = 242;

         //  This accessor type is used to identify the uci::type::AO_ComponentSettingsType Accessor
         static const uci::base::accessorType::AccessorType aO_ComponentSettingsType = 243;

         //  This accessor type is used to identify the uci::type::AO_ComponentType Accessor
         static const uci::base::accessorType::AccessorType aO_ComponentType = 244;

         //  This accessor type is used to identify the uci::type::AO_ConsentRequestMDT Accessor
         static const uci::base::accessorType::AccessorType aO_ConsentRequestMDT = 245;

         //  This accessor type is used to identify the uci::type::AO_ConsentRequestMT Accessor
         static const uci::base::accessorType::AccessorType aO_ConsentRequestMT = 246;

         //  This accessor type is used to identify the uci::type::AO_ConsentRequestQFT Accessor
         static const uci::base::accessorType::AccessorType aO_ConsentRequestQFT = 247;

         //  This accessor type is used to identify the uci::type::AO_ConsentRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType aO_ConsentRequestStatusMDT = 248;

         //  This accessor type is used to identify the uci::type::AO_ConsentRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType aO_ConsentRequestStatusMT = 249;

         //  This accessor type is used to identify the uci::type::AO_ConsentRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType aO_ConsentRequestStatusQFT = 250;

         //  This accessor type is used to identify the uci::type::AO_ConsentRequestType Accessor
         static const uci::base::accessorType::AccessorType aO_ConsentRequestType = 251;

         //  This accessor type is used to identify the uci::type::AO_SettingsCommandMDT Accessor
         static const uci::base::accessorType::AccessorType aO_SettingsCommandMDT = 252;

         //  This accessor type is used to identify the uci::type::AO_SettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType aO_SettingsCommandMT = 253;

         //  This accessor type is used to identify the uci::type::AO_SettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType aO_SettingsCommandQFT = 254;

         //  This accessor type is used to identify the uci::type::AO_SettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType aO_SettingsCommandStatusMT = 255;

         //  This accessor type is used to identify the uci::type::AO_SettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType aO_SettingsCommandStatusQFT = 256;

         //  This accessor type is used to identify the uci::type::AO_SpecificDataType Accessor
         static const uci::base::accessorType::AccessorType aO_SpecificDataType = 257;

         //  This accessor type is used to identify the uci::type::AO_TaskBaseType Accessor
         static const uci::base::accessorType::AccessorType aO_TaskBaseType = 258;

         //  This accessor type is used to identify the uci::type::AO_TaskType Accessor
         static const uci::base::accessorType::AccessorType aO_TaskType = 259;

         //  This accessor type is used to identify the uci::type::AOCO_TraceabilityType Accessor
         static const uci::base::accessorType::AccessorType aOCO_TraceabilityType = 260;

         //  This accessor type is used to identify the uci::type::AppliesToType Accessor
         static const uci::base::accessorType::AccessorType appliesToType = 261;

         //  This accessor type is used to identify the uci::type::ApprovalAuthorityMDT Accessor
         static const uci::base::accessorType::AccessorType approvalAuthorityMDT = 262;

         //  This accessor type is used to identify the uci::type::ApprovalAuthorityMT Accessor
         static const uci::base::accessorType::AccessorType approvalAuthorityMT = 263;

         //  This accessor type is used to identify the uci::type::ApprovalAuthorityQFT Accessor
         static const uci::base::accessorType::AccessorType approvalAuthorityQFT = 264;

         //  This accessor type is used to identify the uci::type::ApprovalAuthorityRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType approvalAuthorityRequestID_Type = 265;

         //  This accessor type is used to identify the uci::type::ApprovalAuthorityRequestMDT Accessor
         static const uci::base::accessorType::AccessorType approvalAuthorityRequestMDT = 266;

         //  This accessor type is used to identify the uci::type::ApprovalAuthorityRequestMT Accessor
         static const uci::base::accessorType::AccessorType approvalAuthorityRequestMT = 267;

         //  This accessor type is used to identify the uci::type::ApprovalAuthorityRequestQFT Accessor
         static const uci::base::accessorType::AccessorType approvalAuthorityRequestQFT = 268;

         //  This accessor type is used to identify the uci::type::ApprovalAuthorityRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType approvalAuthorityRequestStatusMDT = 269;

         //  This accessor type is used to identify the uci::type::ApprovalAuthorityRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType approvalAuthorityRequestStatusMT = 270;

         //  This accessor type is used to identify the uci::type::ApprovalAuthorityRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType approvalAuthorityRequestStatusQFT = 271;

         //  This accessor type is used to identify the uci::type::ApprovalPolicyID_Type Accessor
         static const uci::base::accessorType::AccessorType approvalPolicyID_Type = 272;

         //  This accessor type is used to identify the uci::type::ApprovalPolicyMDT Accessor
         static const uci::base::accessorType::AccessorType approvalPolicyMDT = 273;

         //  This accessor type is used to identify the uci::type::ApprovalPolicyMT Accessor
         static const uci::base::accessorType::AccessorType approvalPolicyMT = 274;

         //  This accessor type is used to identify the uci::type::ApprovalPolicyQFT Accessor
         static const uci::base::accessorType::AccessorType approvalPolicyQFT = 275;

         //  This accessor type is used to identify the uci::type::ApprovalRequestActivationItemType Accessor
         static const uci::base::accessorType::AccessorType approvalRequestActivationItemType = 276;

         //  This accessor type is used to identify the uci::type::ApprovalRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType approvalRequestID_Type = 277;

         //  This accessor type is used to identify the uci::type::ApprovalRequestItemReferenceType Accessor
         static const uci::base::accessorType::AccessorType approvalRequestItemReferenceType = 278;

         //  This accessor type is used to identify the uci::type::ApprovalRequestMDT Accessor
         static const uci::base::accessorType::AccessorType approvalRequestMDT = 279;

         //  This accessor type is used to identify the uci::type::ApprovalRequestMT Accessor
         static const uci::base::accessorType::AccessorType approvalRequestMT = 280;

         //  This accessor type is used to identify the uci::type::ApprovalRequestPolicyReferenceType Accessor
         static const uci::base::accessorType::AccessorType approvalRequestPolicyReferenceType = 281;

         //  This accessor type is used to identify the uci::type::ApprovalRequestQFT Accessor
         static const uci::base::accessorType::AccessorType approvalRequestQFT = 282;

         //  This accessor type is used to identify the uci::type::ApprovalRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType approvalRequestStatusMDT = 283;

         //  This accessor type is used to identify the uci::type::ApprovalRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType approvalRequestStatusMT = 284;

         //  This accessor type is used to identify the uci::type::ApprovalRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType approvalRequestStatusQFT = 285;

         //  This accessor type is used to identify the uci::type::ApprovalRequestTaskItemType Accessor
         static const uci::base::accessorType::AccessorType approvalRequestTaskItemType = 286;

         //  This accessor type is used to identify the uci::type::ArchiveRequestType Accessor
         static const uci::base::accessorType::AccessorType archiveRequestType = 287;

         //  This accessor type is used to identify the uci::type::AreaChoiceType Accessor
         static const uci::base::accessorType::AccessorType areaChoiceType = 288;

         //  This accessor type is used to identify the uci::type::AreaConstraintsType Accessor
         static const uci::base::accessorType::AccessorType areaConstraintsType = 289;

         //  This accessor type is used to identify the uci::type::AssessmentID_Type Accessor
         static const uci::base::accessorType::AccessorType assessmentID_Type = 290;

         //  This accessor type is used to identify the uci::type::AssessmentMDT Accessor
         static const uci::base::accessorType::AccessorType assessmentMDT = 291;

         //  This accessor type is used to identify the uci::type::AssessmentMT Accessor
         static const uci::base::accessorType::AccessorType assessmentMT = 292;

         //  This accessor type is used to identify the uci::type::AssessmentQFT Accessor
         static const uci::base::accessorType::AccessorType assessmentQFT = 293;

         //  This accessor type is used to identify the uci::type::AssessmentRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType assessmentRequestID_Type = 294;

         //  This accessor type is used to identify the uci::type::AssessmentRequestMDT Accessor
         static const uci::base::accessorType::AccessorType assessmentRequestMDT = 295;

         //  This accessor type is used to identify the uci::type::AssessmentRequestMT Accessor
         static const uci::base::accessorType::AccessorType assessmentRequestMT = 296;

         //  This accessor type is used to identify the uci::type::AssessmentRequestQFT Accessor
         static const uci::base::accessorType::AccessorType assessmentRequestQFT = 297;

         //  This accessor type is used to identify the uci::type::AssessmentRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType assessmentRequestStatusMDT = 298;

         //  This accessor type is used to identify the uci::type::AssessmentRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType assessmentRequestStatusMT = 299;

         //  This accessor type is used to identify the uci::type::AssessmentRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType assessmentRequestStatusQFT = 300;

         //  This accessor type is used to identify the uci::type::AssessmentRequestType Accessor
         static const uci::base::accessorType::AccessorType assessmentRequestType = 301;

         //  This accessor type is used to identify the uci::type::AssessmentType Accessor
         static const uci::base::accessorType::AccessorType assessmentType = 302;

         //  This accessor type is used to identify the uci::type::AssetType Accessor
         static const uci::base::accessorType::AccessorType assetType = 303;

         //  This accessor type is used to identify the uci::type::AssociatedMessageType Accessor
         static const uci::base::accessorType::AccessorType associatedMessageType = 304;

         //  This accessor type is used to identify the uci::type::AssociatedTasksType Accessor
         static const uci::base::accessorType::AccessorType associatedTasksType = 305;

         //  This accessor type is used to identify the uci::type::AtmosphericPropagationLossType Accessor
         static const uci::base::accessorType::AccessorType atmosphericPropagationLossType = 306;

         //  This accessor type is used to identify the uci::type::ATO_TraceabilityType Accessor
         static const uci::base::accessorType::AccessorType aTO_TraceabilityType = 307;

      } // Namespace: accessorType
      //  
      namespace aTO_TraceabilityType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::ATO_TraceabilityType::MissionCallSign Accessor
            static const uci::base::accessorType::AccessorType missionCallSign = 308;

            //  This accessor type is used to identify the uci::type::ATO_TraceabilityType::MissionID Accessor
            static const uci::base::accessorType::AccessorType missionID = 309;

         } // Namespace: accessorType
      } // Namespace: aTO_TraceabilityType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::AugmentationProcessingParametersType Accessor
         static const uci::base::accessorType::AccessorType augmentationProcessingParametersType = 310;

         //  This accessor type is used to identify the uci::type::AuralAlertRepetitionType Accessor
         static const uci::base::accessorType::AccessorType auralAlertRepetitionType = 311;

         //  This accessor type is used to identify the uci::type::AuralAlertType Accessor
         static const uci::base::accessorType::AccessorType auralAlertType = 312;

         //  This accessor type is used to identify the uci::type::AuthorizationID_Type Accessor
         static const uci::base::accessorType::AccessorType authorizationID_Type = 313;

         //  This accessor type is used to identify the uci::type::AuthorizationMDT Accessor
         static const uci::base::accessorType::AccessorType authorizationMDT = 314;

         //  This accessor type is used to identify the uci::type::AuthorizationMT Accessor
         static const uci::base::accessorType::AccessorType authorizationMT = 315;

         //  This accessor type is used to identify the uci::type::AuthorizationQFT Accessor
         static const uci::base::accessorType::AccessorType authorizationQFT = 316;

         //  This accessor type is used to identify the uci::type::AutonomousTriggerType Accessor
         static const uci::base::accessorType::AccessorType autonomousTriggerType = 317;

         //  This accessor type is used to identify the uci::type::AutonomySettingID_Type Accessor
         static const uci::base::accessorType::AccessorType autonomySettingID_Type = 318;

         //  This accessor type is used to identify the uci::type::AutonomySettingMDT Accessor
         static const uci::base::accessorType::AccessorType autonomySettingMDT = 319;

         //  This accessor type is used to identify the uci::type::AutonomySettingMT Accessor
         static const uci::base::accessorType::AccessorType autonomySettingMT = 320;

         //  This accessor type is used to identify the uci::type::AutonomySettingQFT Accessor
         static const uci::base::accessorType::AccessorType autonomySettingQFT = 321;

         //  This accessor type is used to identify the uci::type::AvailabilityInfoType Accessor
         static const uci::base::accessorType::AccessorType availabilityInfoType = 322;

         //  This accessor type is used to identify the uci::type::AvailableConnectionType Accessor
         static const uci::base::accessorType::AccessorType availableConnectionType = 323;

         //  This accessor type is used to identify the uci::type::AvailableFuelType Accessor
         static const uci::base::accessorType::AccessorType availableFuelType = 324;

         //  This accessor type is used to identify the uci::type::AvailableWeaponsType Accessor
         static const uci::base::accessorType::AccessorType availableWeaponsType = 325;

         //  This accessor type is used to identify the uci::type::Az_El_BeamPointingType Accessor
         static const uci::base::accessorType::AccessorType az_El_BeamPointingType = 326;

         //  This accessor type is used to identify the uci::type::Az_El_BeamPointingWithRangeType Accessor
         static const uci::base::accessorType::AccessorType az_El_BeamPointingWithRangeType = 327;

         //  This accessor type is used to identify the uci::type::BaseEOB_RecordType Accessor
         static const uci::base::accessorType::AccessorType baseEOB_RecordType = 328;

      } // Namespace: accessorType
      //  
      namespace baseEOB_RecordType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::BaseEOB_RecordType::RecordOwner Accessor
            static const uci::base::accessorType::AccessorType recordOwner = 329;

         } // Namespace: accessorType
      } // Namespace: baseEOB_RecordType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::BasicEncyclopediaNumberType Accessor
         static const uci::base::accessorType::AccessorType basicEncyclopediaNumberType = 330;

      } // Namespace: accessorType
      //  
      namespace basicEncyclopediaNumberType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::BasicEncyclopediaNumberType::WorldAreaCode Accessor
            static const uci::base::accessorType::AccessorType worldAreaCode = 331;

            //  This accessor type is used to identify the uci::type::BasicEncyclopediaNumberType::RecordOriginator Accessor
            static const uci::base::accessorType::AccessorType recordOriginator = 332;

            //  This accessor type is used to identify the uci::type::BasicEncyclopediaNumberType::OneUpNumber Accessor
            static const uci::base::accessorType::AccessorType oneUpNumber = 333;

         } // Namespace: accessorType
      } // Namespace: basicEncyclopediaNumberType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::BCRS_KinematicsType Accessor
         static const uci::base::accessorType::AccessorType bCRS_KinematicsType = 334;

         //  This accessor type is used to identify the uci::type::BDI_CollectionType Accessor
         static const uci::base::accessorType::AccessorType bDI_CollectionType = 335;

         //  This accessor type is used to identify the uci::type::BeamID_Type Accessor
         static const uci::base::accessorType::AccessorType beamID_Type = 336;

         //  This accessor type is used to identify the uci::type::BeamPointingReferenceType Accessor
         static const uci::base::accessorType::AccessorType beamPointingReferenceType = 337;

         //  This accessor type is used to identify the uci::type::BeamShapingType Accessor
         static const uci::base::accessorType::AccessorType beamShapingType = 338;

         //  This accessor type is used to identify the uci::type::BeamWeightingType Accessor
         static const uci::base::accessorType::AccessorType beamWeightingType = 339;

         //  This accessor type is used to identify the uci::type::BeamWidthType Accessor
         static const uci::base::accessorType::AccessorType beamWidthType = 340;

         //  This accessor type is used to identify the uci::type::BIT_ID_Type Accessor
         static const uci::base::accessorType::AccessorType bIT_ID_Type = 341;

         //  This accessor type is used to identify the uci::type::BlankingLimitType Accessor
         static const uci::base::accessorType::AccessorType blankingLimitType = 342;

         //  This accessor type is used to identify the uci::type::BlueVehicleType Accessor
         static const uci::base::accessorType::AccessorType blueVehicleType = 343;

         //  This accessor type is used to identify the uci::type::BoundaryType Accessor
         static const uci::base::accessorType::AccessorType boundaryType = 344;

         //  This accessor type is used to identify the uci::type::ByReferenceType Accessor
         static const uci::base::accessorType::AccessorType byReferenceType = 345;

         //  This accessor type is used to identify the uci::type::CalibrationID_Type Accessor
         static const uci::base::accessorType::AccessorType calibrationID_Type = 346;

         //  This accessor type is used to identify the uci::type::CallSignType Accessor
         static const uci::base::accessorType::AccessorType callSignType = 347;

         //  This accessor type is used to identify the uci::type::CannotComplyType Accessor
         static const uci::base::accessorType::AccessorType cannotComplyType = 348;

         //  This accessor type is used to identify the uci::type::CAP_TaskType Accessor
         static const uci::base::accessorType::AccessorType cAP_TaskType = 349;

         //  This accessor type is used to identify the uci::type::CapabilityAssessmentActionType Accessor
         static const uci::base::accessorType::AccessorType capabilityAssessmentActionType = 350;

         //  This accessor type is used to identify the uci::type::CapabilityAssessmentActivityDetailsType Accessor
         static const uci::base::accessorType::AccessorType capabilityAssessmentActivityDetailsType = 351;

         //  This accessor type is used to identify the uci::type::CapabilityAssessmentActivityType Accessor
         static const uci::base::accessorType::AccessorType capabilityAssessmentActivityType = 352;

         //  This accessor type is used to identify the uci::type::CapabilityAssessmentDataType Accessor
         static const uci::base::accessorType::AccessorType capabilityAssessmentDataType = 353;

         //  This accessor type is used to identify the uci::type::CapabilityAssessmentPointType Accessor
         static const uci::base::accessorType::AccessorType capabilityAssessmentPointType = 354;

         //  This accessor type is used to identify the uci::type::CapabilityBaseType Accessor
         static const uci::base::accessorType::AccessorType capabilityBaseType = 355;

         //  This accessor type is used to identify the uci::type::CapabilityCommandBaseType Accessor
         static const uci::base::accessorType::AccessorType capabilityCommandBaseType = 356;

         //  This accessor type is used to identify the uci::type::CapabilityCommandFrequencyType Accessor
         static const uci::base::accessorType::AccessorType capabilityCommandFrequencyType = 357;

         //  This accessor type is used to identify the uci::type::CapabilityCommandID_Type Accessor
         static const uci::base::accessorType::AccessorType capabilityCommandID_Type = 358;

         //  This accessor type is used to identify the uci::type::CapabilityCommandRankingType Accessor
         static const uci::base::accessorType::AccessorType capabilityCommandRankingType = 359;

         //  This accessor type is used to identify the uci::type::CapabilityCommandStatusMDT Accessor
         static const uci::base::accessorType::AccessorType capabilityCommandStatusMDT = 360;

         //  This accessor type is used to identify the uci::type::CapabilityCommandTemporalConstraintsType Accessor
         static const uci::base::accessorType::AccessorType capabilityCommandTemporalConstraintsType = 361;

         //  This accessor type is used to identify the uci::type::CapabilityCoverageAreaDataType Accessor
         static const uci::base::accessorType::AccessorType capabilityCoverageAreaDataType = 362;

         //  This accessor type is used to identify the uci::type::CapabilityCoverageAreaID_Type Accessor
         static const uci::base::accessorType::AccessorType capabilityCoverageAreaID_Type = 363;

         //  This accessor type is used to identify the uci::type::CapabilityCoverageAreaMDT Accessor
         static const uci::base::accessorType::AccessorType capabilityCoverageAreaMDT = 364;

         //  This accessor type is used to identify the uci::type::CapabilityCoverageAreaMT Accessor
         static const uci::base::accessorType::AccessorType capabilityCoverageAreaMT = 365;

         //  This accessor type is used to identify the uci::type::CapabilityCoverageAreaQFT Accessor
         static const uci::base::accessorType::AccessorType capabilityCoverageAreaQFT = 366;

         //  This accessor type is used to identify the uci::type::CapabilityCoverageAreaRequestDataType Accessor
         static const uci::base::accessorType::AccessorType capabilityCoverageAreaRequestDataType = 367;

         //  This accessor type is used to identify the uci::type::CapabilityCoverageAreaRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType capabilityCoverageAreaRequestID_Type = 368;

         //  This accessor type is used to identify the uci::type::CapabilityCoverageAreaRequestMDT Accessor
         static const uci::base::accessorType::AccessorType capabilityCoverageAreaRequestMDT = 369;

         //  This accessor type is used to identify the uci::type::CapabilityCoverageAreaRequestMT Accessor
         static const uci::base::accessorType::AccessorType capabilityCoverageAreaRequestMT = 370;

         //  This accessor type is used to identify the uci::type::CapabilityCoverageAreaRequestQFT Accessor
         static const uci::base::accessorType::AccessorType capabilityCoverageAreaRequestQFT = 371;

         //  This accessor type is used to identify the uci::type::CapabilityCoverageAreaRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType capabilityCoverageAreaRequestStatusMDT = 372;

         //  This accessor type is used to identify the uci::type::CapabilityCoverageAreaRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType capabilityCoverageAreaRequestStatusMT = 373;

         //  This accessor type is used to identify the uci::type::CapabilityCoverageAreaRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType capabilityCoverageAreaRequestStatusQFT = 374;

         //  This accessor type is used to identify the uci::type::CapabilityCrossReferenceType Accessor
         static const uci::base::accessorType::AccessorType capabilityCrossReferenceType = 375;

         //  This accessor type is used to identify the uci::type::CapabilityID_Type Accessor
         static const uci::base::accessorType::AccessorType capabilityID_Type = 376;

         //  This accessor type is used to identify the uci::type::CapabilityManagerType Accessor
         static const uci::base::accessorType::AccessorType capabilityManagerType = 377;

         //  This accessor type is used to identify the uci::type::CapabilityMetadataType Accessor
         static const uci::base::accessorType::AccessorType capabilityMetadataType = 378;

         //  This accessor type is used to identify the uci::type::CapabilityPlanCommandType Accessor
         static const uci::base::accessorType::AccessorType capabilityPlanCommandType = 379;

         //  This accessor type is used to identify the uci::type::CapabilityPlanID_Type Accessor
         static const uci::base::accessorType::AccessorType capabilityPlanID_Type = 380;

         //  This accessor type is used to identify the uci::type::CapabilityPlanMDT Accessor
         static const uci::base::accessorType::AccessorType capabilityPlanMDT = 381;

         //  This accessor type is used to identify the uci::type::CapabilityPlanMT Accessor
         static const uci::base::accessorType::AccessorType capabilityPlanMT = 382;

         //  This accessor type is used to identify the uci::type::CapabilityPlanQFT Accessor
         static const uci::base::accessorType::AccessorType capabilityPlanQFT = 383;

         //  This accessor type is used to identify the uci::type::CapabilityPrecedenceType Accessor
         static const uci::base::accessorType::AccessorType capabilityPrecedenceType = 384;

         //  This accessor type is used to identify the uci::type::CapabilityRF_ProfileStatusType Accessor
         static const uci::base::accessorType::AccessorType capabilityRF_ProfileStatusType = 385;

         //  This accessor type is used to identify the uci::type::CapabilitySettingsCommandBaseType Accessor
         static const uci::base::accessorType::AccessorType capabilitySettingsCommandBaseType = 386;

         //  This accessor type is used to identify the uci::type::CapabilitySettingsCommandID_Type Accessor
         static const uci::base::accessorType::AccessorType capabilitySettingsCommandID_Type = 387;

         //  This accessor type is used to identify the uci::type::CapabilitySettingsCommandStatusMDT Accessor
         static const uci::base::accessorType::AccessorType capabilitySettingsCommandStatusMDT = 388;

         //  This accessor type is used to identify the uci::type::CapabilitySpecificationType Accessor
         static const uci::base::accessorType::AccessorType capabilitySpecificationType = 389;

         //  This accessor type is used to identify the uci::type::CapabilityStateType Accessor
         static const uci::base::accessorType::AccessorType capabilityStateType = 390;

         //  This accessor type is used to identify the uci::type::CapabilityStatusBaseType Accessor
         static const uci::base::accessorType::AccessorType capabilityStatusBaseType = 391;

         //  This accessor type is used to identify the uci::type::CapabilityStatusType Accessor
         static const uci::base::accessorType::AccessorType capabilityStatusType = 392;

         //  This accessor type is used to identify the uci::type::CapabilityTriggerDataType Accessor
         static const uci::base::accessorType::AccessorType capabilityTriggerDataType = 393;

         //  This accessor type is used to identify the uci::type::CapabilityUtilizationAssessmentType Accessor
         static const uci::base::accessorType::AccessorType capabilityUtilizationAssessmentType = 394;

         //  This accessor type is used to identify the uci::type::CapabilityUtilizationRequestType Accessor
         static const uci::base::accessorType::AccessorType capabilityUtilizationRequestType = 395;

         //  This accessor type is used to identify the uci::type::CargoDeliveryCapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType cargoDeliveryCapabilityMDT = 396;

         //  This accessor type is used to identify the uci::type::CargoDeliveryCapabilityMT Accessor
         static const uci::base::accessorType::AccessorType cargoDeliveryCapabilityMT = 397;

         //  This accessor type is used to identify the uci::type::CargoDeliveryCapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType cargoDeliveryCapabilityQFT = 398;

         //  This accessor type is used to identify the uci::type::CargoDeliveryCapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType cargoDeliveryCapabilityStatusMDT = 399;

         //  This accessor type is used to identify the uci::type::CargoDeliveryCapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType cargoDeliveryCapabilityStatusMT = 400;

         //  This accessor type is used to identify the uci::type::CargoDeliveryCapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType cargoDeliveryCapabilityStatusQFT = 401;

         //  This accessor type is used to identify the uci::type::CargoDeliveryCapabilityType Accessor
         static const uci::base::accessorType::AccessorType cargoDeliveryCapabilityType = 402;

         //  This accessor type is used to identify the uci::type::CargoDeliverySpecificDataType Accessor
         static const uci::base::accessorType::AccessorType cargoDeliverySpecificDataType = 403;

         //  This accessor type is used to identify the uci::type::CargoDeliveryTaskType Accessor
         static const uci::base::accessorType::AccessorType cargoDeliveryTaskType = 404;

         //  This accessor type is used to identify the uci::type::CargoID_Type Accessor
         static const uci::base::accessorType::AccessorType cargoID_Type = 405;

         //  This accessor type is used to identify the uci::type::CargoLocationType Accessor
         static const uci::base::accessorType::AccessorType cargoLocationType = 406;

         //  This accessor type is used to identify the uci::type::CargoSizeType Accessor
         static const uci::base::accessorType::AccessorType cargoSizeType = 407;

         //  This accessor type is used to identify the uci::type::CargoStatusMDT Accessor
         static const uci::base::accessorType::AccessorType cargoStatusMDT = 408;

         //  This accessor type is used to identify the uci::type::CargoStatusMT Accessor
         static const uci::base::accessorType::AccessorType cargoStatusMT = 409;

         //  This accessor type is used to identify the uci::type::CargoStatusQFT Accessor
         static const uci::base::accessorType::AccessorType cargoStatusQFT = 410;

         //  This accessor type is used to identify the uci::type::CargoTransitionType Accessor
         static const uci::base::accessorType::AccessorType cargoTransitionType = 411;

         //  This accessor type is used to identify the uci::type::CargoType Accessor
         static const uci::base::accessorType::AccessorType cargoType = 412;

         //  This accessor type is used to identify the uci::type::CenterPointImageRegionOfInterestType Accessor
         static const uci::base::accessorType::AccessorType centerPointImageRegionOfInterestType = 413;

         //  This accessor type is used to identify the uci::type::ChannelDescriptionType Accessor
         static const uci::base::accessorType::AccessorType channelDescriptionType = 414;

         //  This accessor type is used to identify the uci::type::ChannelInstanceIdentifierType Accessor
         static const uci::base::accessorType::AccessorType channelInstanceIdentifierType = 415;

         //  This accessor type is used to identify the uci::type::ChannelTypeIdentifierType Accessor
         static const uci::base::accessorType::AccessorType channelTypeIdentifierType = 416;

         //  This accessor type is used to identify the uci::type::CircleType Accessor
         static const uci::base::accessorType::AccessorType circleType = 417;

         //  This accessor type is used to identify the uci::type::ClassificationConstrainedSubPlanType Accessor
         static const uci::base::accessorType::AccessorType classificationConstrainedSubPlanType = 418;

         //  This accessor type is used to identify the uci::type::ClassificationReportType Accessor
         static const uci::base::accessorType::AccessorType classificationReportType = 419;

         //  This accessor type is used to identify the uci::type::ClassificationRequirementsType Accessor
         static const uci::base::accessorType::AccessorType classificationRequirementsType = 420;

         //  This accessor type is used to identify the uci::type::ClearAircraftTaskType Accessor
         static const uci::base::accessorType::AccessorType clearAircraftTaskType = 421;

         //  This accessor type is used to identify the uci::type::ClimbType Accessor
         static const uci::base::accessorType::AccessorType climbType = 422;

         //  This accessor type is used to identify the uci::type::CloudsType Accessor
         static const uci::base::accessorType::AccessorType cloudsType = 423;

      } // Namespace: accessorType
      //  
      namespace cloudsType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CloudsType::CloudCoverAmplification Accessor
            static const uci::base::accessorType::AccessorType cloudCoverAmplification = 424;

         } // Namespace: accessorType
      } // Namespace: cloudsType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::COE_NonEquatorialOrientationType Accessor
         static const uci::base::accessorType::AccessorType cOE_NonEquatorialOrientationType = 425;

         //  This accessor type is used to identify the uci::type::COE_OrbitBaseType Accessor
         static const uci::base::accessorType::AccessorType cOE_OrbitBaseType = 426;

         //  This accessor type is used to identify the uci::type::COE_OrientationType Accessor
         static const uci::base::accessorType::AccessorType cOE_OrientationType = 427;

         //  This accessor type is used to identify the uci::type::COE_PositionType Accessor
         static const uci::base::accessorType::AccessorType cOE_PositionType = 428;

         //  This accessor type is used to identify the uci::type::CollectionConstraintsType Accessor
         static const uci::base::accessorType::AccessorType collectionConstraintsType = 429;

         //  This accessor type is used to identify the uci::type::CollectionDeckTraceabilityType Accessor
         static const uci::base::accessorType::AccessorType collectionDeckTraceabilityType = 430;

      } // Namespace: accessorType
      //  
      namespace collectionDeckTraceabilityType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CollectionDeckTraceabilityType::CollectionDeckMissionID
            //  Accessor
            static const uci::base::accessorType::AccessorType collectionDeckMissionID = 431;

            //  This accessor type is used to identify the uci::type::CollectionDeckTraceabilityType::CollectionDeckTaskID Accessor
            static const uci::base::accessorType::AccessorType collectionDeckTaskID = 432;

         } // Namespace: accessorType
      } // Namespace: collectionDeckTraceabilityType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CollectionFrequencyType Accessor
         static const uci::base::accessorType::AccessorType collectionFrequencyType = 433;

         //  This accessor type is used to identify the uci::type::CollectionTaskMetricsType Accessor
         static const uci::base::accessorType::AccessorType collectionTaskMetricsType = 434;

         //  This accessor type is used to identify the uci::type::ColorDepthSettingsType Accessor
         static const uci::base::accessorType::AccessorType colorDepthSettingsType = 435;

         //  This accessor type is used to identify the uci::type::COMINT_AcquisitionTargetType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_AcquisitionTargetType = 436;

         //  This accessor type is used to identify the uci::type::COMINT_ActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ActivityCommandType = 437;

         //  This accessor type is used to identify the uci::type::COMINT_ActivityMDT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ActivityMDT = 438;

         //  This accessor type is used to identify the uci::type::COMINT_ActivityMT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ActivityMT = 439;

         //  This accessor type is used to identify the uci::type::COMINT_ActivityQFT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ActivityQFT = 440;

         //  This accessor type is used to identify the uci::type::COMINT_ActivityType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ActivityType = 441;

         //  This accessor type is used to identify the uci::type::COMINT_CapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_CapabilityCommandType = 442;

         //  This accessor type is used to identify the uci::type::COMINT_CapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_CapabilityMDT = 443;

         //  This accessor type is used to identify the uci::type::COMINT_CapabilityMT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_CapabilityMT = 444;

         //  This accessor type is used to identify the uci::type::COMINT_CapabilityOptionsType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_CapabilityOptionsType = 445;

         //  This accessor type is used to identify the uci::type::COMINT_CapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_CapabilityQFT = 446;

         //  This accessor type is used to identify the uci::type::COMINT_CapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_CapabilityStatusMDT = 447;

         //  This accessor type is used to identify the uci::type::COMINT_CapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_CapabilityStatusMT = 448;

         //  This accessor type is used to identify the uci::type::COMINT_CapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_CapabilityStatusQFT = 449;

         //  This accessor type is used to identify the uci::type::COMINT_CapabilityType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_CapabilityType = 450;

         //  This accessor type is used to identify the uci::type::COMINT_ChangeDwellType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ChangeDwellType = 451;

         //  This accessor type is used to identify the uci::type::COMINT_ChangeGeolocationType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ChangeGeolocationType = 452;

         //  This accessor type is used to identify the uci::type::COMINT_CommandMDT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_CommandMDT = 453;

         //  This accessor type is used to identify the uci::type::COMINT_CommandMT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_CommandMT = 454;

         //  This accessor type is used to identify the uci::type::COMINT_CommandQFT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_CommandQFT = 455;

         //  This accessor type is used to identify the uci::type::COMINT_CommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_CommandStatusMT = 456;

         //  This accessor type is used to identify the uci::type::COMINT_CommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_CommandStatusQFT = 457;

         //  This accessor type is used to identify the uci::type::COMINT_CommandType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_CommandType = 458;

         //  This accessor type is used to identify the uci::type::COMINT_ConsentRequestMDT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ConsentRequestMDT = 459;

         //  This accessor type is used to identify the uci::type::COMINT_ConsentRequestMT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ConsentRequestMT = 460;

         //  This accessor type is used to identify the uci::type::COMINT_ConsentRequestQFT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ConsentRequestQFT = 461;

         //  This accessor type is used to identify the uci::type::COMINT_ConsentRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ConsentRequestStatusMDT = 462;

         //  This accessor type is used to identify the uci::type::COMINT_ConsentRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ConsentRequestStatusMT = 463;

         //  This accessor type is used to identify the uci::type::COMINT_ConsentRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ConsentRequestStatusQFT = 464;

         //  This accessor type is used to identify the uci::type::COMINT_ConsentRequestType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ConsentRequestType = 465;

         //  This accessor type is used to identify the uci::type::COMINT_DataCollectCommandType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_DataCollectCommandType = 466;

         //  This accessor type is used to identify the uci::type::COMINT_DirectionFindingType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_DirectionFindingType = 467;

         //  This accessor type is used to identify the uci::type::COMINT_GeolocationType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_GeolocationType = 468;

         //  This accessor type is used to identify the uci::type::COMINT_IdentificationType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_IdentificationType = 469;

         //  This accessor type is used to identify the uci::type::COMINT_InteractiveType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_InteractiveType = 470;

         //  This accessor type is used to identify the uci::type::COMINT_MeasurementResultsType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_MeasurementResultsType = 471;

         //  This accessor type is used to identify the uci::type::COMINT_MeasurementType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_MeasurementType = 472;

         //  This accessor type is used to identify the uci::type::COMINT_ProcessingParametersType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ProcessingParametersType = 473;

         //  This accessor type is used to identify the uci::type::COMINT_ReceiveDwellType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_ReceiveDwellType = 474;

         //  This accessor type is used to identify the uci::type::COMINT_SettingsCommandMDT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_SettingsCommandMDT = 475;

         //  This accessor type is used to identify the uci::type::COMINT_SettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_SettingsCommandMT = 476;

         //  This accessor type is used to identify the uci::type::COMINT_SettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_SettingsCommandQFT = 477;

         //  This accessor type is used to identify the uci::type::COMINT_SettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_SettingsCommandStatusMT = 478;

         //  This accessor type is used to identify the uci::type::COMINT_SettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType cOMINT_SettingsCommandStatusQFT = 479;

         //  This accessor type is used to identify the uci::type::COMINT_SpecificDataType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_SpecificDataType = 480;

         //  This accessor type is used to identify the uci::type::COMINT_SubcapabilityAcquisitionType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_SubcapabilityAcquisitionType = 481;

         //  This accessor type is used to identify the uci::type::COMINT_SubcapabilityChoiceType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_SubcapabilityChoiceType = 482;

         //  This accessor type is used to identify the uci::type::COMINT_SubcapabilityDataCollectType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_SubcapabilityDataCollectType = 483;

         //  This accessor type is used to identify the uci::type::COMINT_SubCapabilityDetailsType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_SubCapabilityDetailsType = 484;

         //  This accessor type is used to identify the uci::type::COMINT_SubcapabilityGeolocationType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_SubcapabilityGeolocationType = 485;

         //  This accessor type is used to identify the uci::type::COMINT_SubcapabilityIdentificationType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_SubcapabilityIdentificationType = 486;

         //  This accessor type is used to identify the uci::type::COMINT_SubcapabilityMeasurementType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_SubcapabilityMeasurementType = 487;

         //  This accessor type is used to identify the uci::type::COMINT_SubcapabilityTargetLocationDataType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_SubcapabilityTargetLocationDataType = 488;

         //  This accessor type is used to identify the uci::type::COMINT_TargetType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_TargetType = 489;

         //  This accessor type is used to identify the uci::type::COMINT_TaskBaseType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_TaskBaseType = 490;

         //  This accessor type is used to identify the uci::type::COMINT_TaskType Accessor
         static const uci::base::accessorType::AccessorType cOMINT_TaskType = 491;

         //  This accessor type is used to identify the uci::type::CommAllocationActionType Accessor
         static const uci::base::accessorType::AccessorType commAllocationActionType = 492;

         //  This accessor type is used to identify the uci::type::CommAllocationStatusType Accessor
         static const uci::base::accessorType::AccessorType commAllocationStatusType = 493;

         //  This accessor type is used to identify the uci::type::CommandActivityTraceabilityType Accessor
         static const uci::base::accessorType::AccessorType commandActivityTraceabilityType = 494;

         //  This accessor type is used to identify the uci::type::CommandRepetitionType Accessor
         static const uci::base::accessorType::AccessorType commandRepetitionType = 495;

         //  This accessor type is used to identify the uci::type::CommandResponseType Accessor
         static const uci::base::accessorType::AccessorType commandResponseType = 496;

         //  This accessor type is used to identify the uci::type::CommAntennaModeType Accessor
         static const uci::base::accessorType::AccessorType commAntennaModeType = 497;

         //  This accessor type is used to identify the uci::type::CommAssetEstimationRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType commAssetEstimationRequestID_Type = 498;

         //  This accessor type is used to identify the uci::type::CommAssetEstimationRequestMDT Accessor
         static const uci::base::accessorType::AccessorType commAssetEstimationRequestMDT = 499;

         //  This accessor type is used to identify the uci::type::CommAssetEstimationRequestMT Accessor
         static const uci::base::accessorType::AccessorType commAssetEstimationRequestMT = 500;

         //  This accessor type is used to identify the uci::type::CommAssetEstimationRequestQFT Accessor
         static const uci::base::accessorType::AccessorType commAssetEstimationRequestQFT = 501;

         //  This accessor type is used to identify the uci::type::CommAssetEstimationRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType commAssetEstimationRequestStatusMDT = 502;

         //  This accessor type is used to identify the uci::type::CommAssetEstimationRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType commAssetEstimationRequestStatusMT = 503;

         //  This accessor type is used to identify the uci::type::CommAssetEstimationRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType commAssetEstimationRequestStatusQFT = 504;

         //  This accessor type is used to identify the uci::type::CommAssetEstimationResultType Accessor
         static const uci::base::accessorType::AccessorType commAssetEstimationResultType = 505;

         //  This accessor type is used to identify the uci::type::CommAssetID_Type Accessor
         static const uci::base::accessorType::AccessorType commAssetID_Type = 506;

         //  This accessor type is used to identify the uci::type::CommAssetMDT Accessor
         static const uci::base::accessorType::AccessorType commAssetMDT = 507;

         //  This accessor type is used to identify the uci::type::CommAssetMT Accessor
         static const uci::base::accessorType::AccessorType commAssetMT = 508;

         //  This accessor type is used to identify the uci::type::CommAssetOrbitMDT Accessor
         static const uci::base::accessorType::AccessorType commAssetOrbitMDT = 509;

         //  This accessor type is used to identify the uci::type::CommAssetOrbitMT Accessor
         static const uci::base::accessorType::AccessorType commAssetOrbitMT = 510;

         //  This accessor type is used to identify the uci::type::CommAssetOrbitQFT Accessor
         static const uci::base::accessorType::AccessorType commAssetOrbitQFT = 511;

         //  This accessor type is used to identify the uci::type::CommAssetPositionReportMDT Accessor
         static const uci::base::accessorType::AccessorType commAssetPositionReportMDT = 512;

         //  This accessor type is used to identify the uci::type::CommAssetPositionReportMT Accessor
         static const uci::base::accessorType::AccessorType commAssetPositionReportMT = 513;

         //  This accessor type is used to identify the uci::type::CommAssetPositionReportQFT Accessor
         static const uci::base::accessorType::AccessorType commAssetPositionReportQFT = 514;

         //  This accessor type is used to identify the uci::type::CommAssetQFT Accessor
         static const uci::base::accessorType::AccessorType commAssetQFT = 515;

         //  This accessor type is used to identify the uci::type::CommCapabilityOptionsType Accessor
         static const uci::base::accessorType::AccessorType commCapabilityOptionsType = 516;

         //  This accessor type is used to identify the uci::type::CommCDL_CommWaveformActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType commCDL_CommWaveformActivityCommandType = 517;

         //  This accessor type is used to identify the uci::type::CommCDL_CommWaveformActivityType Accessor
         static const uci::base::accessorType::AccessorType commCDL_CommWaveformActivityType = 518;

         //  This accessor type is used to identify the uci::type::CommCDL_CommWaveformCapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType commCDL_CommWaveformCapabilityCommandType = 519;

         //  This accessor type is used to identify the uci::type::CommCDL_ControlType Accessor
         static const uci::base::accessorType::AccessorType commCDL_ControlType = 520;

      } // Namespace: accessorType
      //  
      namespace commCDL_ControlType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommCDL_ControlType::LinkChannelPriority Accessor
            static const uci::base::accessorType::AccessorType linkChannelPriority = 521;

            //  This accessor type is used to identify the uci::type::CommCDL_ControlType::UDP_MonitorPeriod Accessor
            static const uci::base::accessorType::AccessorType uDP_MonitorPeriod = 522;

         } // Namespace: accessorType
      } // Namespace: commCDL_ControlType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommConfigurationID_Type Accessor
         static const uci::base::accessorType::AccessorType commConfigurationID_Type = 523;

         //  This accessor type is used to identify the uci::type::CommConnectionType Accessor
         static const uci::base::accessorType::AccessorType commConnectionType = 524;

         //  This accessor type is used to identify the uci::type::CommControlType Accessor
         static const uci::base::accessorType::AccessorType commControlType = 525;

      } // Namespace: accessorType
      //  
      namespace commControlType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommControlType::SquelchLevel Accessor
            static const uci::base::accessorType::AccessorType squelchLevel = 526;

            //  This accessor type is used to identify the uci::type::CommControlType::VolumeLevel Accessor
            static const uci::base::accessorType::AccessorType volumeLevel = 527;

         } // Namespace: accessorType
      } // Namespace: commControlType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommCoverageDataType Accessor
         static const uci::base::accessorType::AccessorType commCoverageDataType = 528;

         //  This accessor type is used to identify the uci::type::CommCoverageUserDataType Accessor
         static const uci::base::accessorType::AccessorType commCoverageUserDataType = 529;

         //  This accessor type is used to identify the uci::type::CommDataType Accessor
         static const uci::base::accessorType::AccessorType commDataType = 530;

         //  This accessor type is used to identify the uci::type::CommEffectivityID_Type Accessor
         static const uci::base::accessorType::AccessorType commEffectivityID_Type = 531;

         //  This accessor type is used to identify the uci::type::CommEffectivityType Accessor
         static const uci::base::accessorType::AccessorType commEffectivityType = 532;

         //  This accessor type is used to identify the uci::type::CommIFDL_CommWaveformActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType commIFDL_CommWaveformActivityCommandType = 533;

         //  This accessor type is used to identify the uci::type::CommIFDL_CommWaveformActivityType Accessor
         static const uci::base::accessorType::AccessorType commIFDL_CommWaveformActivityType = 534;

      } // Namespace: accessorType
      //  
      namespace commIFDL_CommWaveformActivityType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommIFDL_CommWaveformActivityType::ChannelA_Number Accessor
            static const uci::base::accessorType::AccessorType channelA_Number = 535;

            //  This accessor type is used to identify the uci::type::CommIFDL_CommWaveformActivityType::ChannelB_Number Accessor
            static const uci::base::accessorType::AccessorType channelB_Number = 536;

            //  This accessor type is used to identify the uci::type::CommIFDL_CommWaveformActivityType::WideNetID1 Accessor
            static const uci::base::accessorType::AccessorType wideNetID1 = 537;

            //  This accessor type is used to identify the uci::type::CommIFDL_CommWaveformActivityType::WideNetID2 Accessor
            static const uci::base::accessorType::AccessorType wideNetID2 = 538;

            //  This accessor type is used to identify the uci::type::CommIFDL_CommWaveformActivityType::WideNetID3 Accessor
            static const uci::base::accessorType::AccessorType wideNetID3 = 539;

         } // Namespace: accessorType
      } // Namespace: commIFDL_CommWaveformActivityType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommIFDL_CommWaveformCapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType commIFDL_CommWaveformCapabilityCommandType = 540;

         //  This accessor type is used to identify the uci::type::CommIFDL_ControlType Accessor
         static const uci::base::accessorType::AccessorType commIFDL_ControlType = 541;

      } // Namespace: accessorType
      //  
      namespace commIFDL_ControlType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommIFDL_ControlType::FlightID Accessor
            static const uci::base::accessorType::AccessorType flightID = 542;

            //  This accessor type is used to identify the uci::type::CommIFDL_ControlType::FlightPosition Accessor
            static const uci::base::accessorType::AccessorType flightPosition = 543;

            //  This accessor type is used to identify the uci::type::CommIFDL_ControlType::NetNumber Accessor
            static const uci::base::accessorType::AccessorType netNumber = 544;

         } // Namespace: accessorType
      } // Namespace: commIFDL_ControlType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommIFDL_NetLinkStatusType Accessor
         static const uci::base::accessorType::AccessorType commIFDL_NetLinkStatusType = 545;

      } // Namespace: accessorType
      //  
      namespace commIFDL_NetLinkStatusType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommIFDL_NetLinkStatusType::PeerFlightID Accessor
            static const uci::base::accessorType::AccessorType peerFlightID = 546;

            //  This accessor type is used to identify the uci::type::CommIFDL_NetLinkStatusType::PeerFlightPosition Accessor
            static const uci::base::accessorType::AccessorType peerFlightPosition = 547;

         } // Namespace: accessorType
      } // Namespace: commIFDL_NetLinkStatusType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommLink16AdvancedSlotNotificationType Accessor
         static const uci::base::accessorType::AccessorType commLink16AdvancedSlotNotificationType = 548;

      } // Namespace: accessorType
      //  
      namespace commLink16AdvancedSlotNotificationType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommLink16AdvancedSlotNotificationType::ASN_SlotSelection
            //  Accessor
            static const uci::base::accessorType::AccessorType aSN_SlotSelection = 549;

         } // Namespace: accessorType
      } // Namespace: commLink16AdvancedSlotNotificationType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommLink16CommWaveformActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType commLink16CommWaveformActivityCommandType = 550;

         //  This accessor type is used to identify the uci::type::CommLink16CommWaveformActivityType Accessor
         static const uci::base::accessorType::AccessorType commLink16CommWaveformActivityType = 551;

      } // Namespace: accessorType
      //  
      namespace commLink16CommWaveformActivityType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommLink16CommWaveformActivityType::PPLI_MsgGenPPLI_PG Accessor
            static const uci::base::accessorType::AccessorType pPLI_MsgGenPPLI_PG = 552;

            //  This accessor type is used to identify the uci::type::CommLink16CommWaveformActivityType::PPLI_MsgGenTxRate Accessor
            static const uci::base::accessorType::AccessorType pPLI_MsgGenTxRate = 553;

         } // Namespace: accessorType
      } // Namespace: commLink16CommWaveformActivityType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommLink16CommWaveformCapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType commLink16CommWaveformCapabilityCommandType = 554;

         //  This accessor type is used to identify the uci::type::CommLink16ControlType Accessor
         static const uci::base::accessorType::AccessorType commLink16ControlType = 555;

      } // Namespace: accessorType
      //  
      namespace commLink16ControlType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommLink16ControlType::DataReductionPG_Index Accessor
            static const uci::base::accessorType::AccessorType dataReductionPG_Index = 556;

            //  This accessor type is used to identify the uci::type::CommLink16ControlType::InitializationIndexNumber Accessor
            static const uci::base::accessorType::AccessorType initializationIndexNumber = 557;

            //  This accessor type is used to identify the uci::type::CommLink16ControlType::PlatformStrength Accessor
            static const uci::base::accessorType::AccessorType platformStrength = 558;

            //  This accessor type is used to identify the uci::type::CommLink16ControlType::RouteEstPG_Index Accessor
            static const uci::base::accessorType::AccessorType routeEstPG_Index = 559;

         } // Namespace: accessorType
      } // Namespace: commLink16ControlType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommLink16DelayReductionType Accessor
         static const uci::base::accessorType::AccessorType commLink16DelayReductionType = 560;

      } // Namespace: accessorType
      //  
      namespace commLink16DelayReductionType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommLink16DelayReductionType::MessageLabel Accessor
            static const uci::base::accessorType::AccessorType messageLabel = 561;

            //  This accessor type is used to identify the uci::type::CommLink16DelayReductionType::MessageSubLabel Accessor
            static const uci::base::accessorType::AccessorType messageSubLabel = 562;

         } // Namespace: accessorType
      } // Namespace: commLink16DelayReductionType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommLink16InitialEntryMessageType Accessor
         static const uci::base::accessorType::AccessorType commLink16InitialEntryMessageType = 563;

         //  This accessor type is used to identify the uci::type::CommLink16MissionInfoWordsType Accessor
         static const uci::base::accessorType::AccessorType commLink16MissionInfoWordsType = 564;

      } // Namespace: accessorType
      //  
      namespace commLink16MissionInfoWordsType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommLink16MissionInfoWordsType::NonC2_PGA_NetNumber Accessor
            static const uci::base::accessorType::AccessorType nonC2_PGA_NetNumber = 565;

            //  This accessor type is used to identify the uci::type::CommLink16MissionInfoWordsType::NonC2_PGB_NetNumber Accessor
            static const uci::base::accessorType::AccessorType nonC2_PGB_NetNumber = 566;

            //  This accessor type is used to identify the uci::type::CommLink16MissionInfoWordsType::MissionCorrelatorID Accessor
            static const uci::base::accessorType::AccessorType missionCorrelatorID = 567;

            //  This accessor type is used to identify the uci::type::CommLink16MissionInfoWordsType::MissionCDR_TrackNumber Accessor
            static const uci::base::accessorType::AccessorType missionCDR_TrackNumber = 568;

            //  This accessor type is used to identify the uci::type::CommLink16MissionInfoWordsType::FlightLeadTrackNumber Accessor
            static const uci::base::accessorType::AccessorType flightLeadTrackNumber = 569;

         } // Namespace: accessorType
      } // Namespace: commLink16MissionInfoWordsType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommLink16NetSelectionType Accessor
         static const uci::base::accessorType::AccessorType commLink16NetSelectionType = 570;

      } // Namespace: accessorType
      //  
      namespace commLink16NetSelectionType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommLink16NetSelectionType::NetSelectionStatusNet Accessor
            static const uci::base::accessorType::AccessorType netSelectionStatusNet = 571;

            //  This accessor type is used to identify the uci::type::CommLink16NetSelectionType::NetSelectionStatusPG_Index Accessor
            static const uci::base::accessorType::AccessorType netSelectionStatusPG_Index = 572;

         } // Namespace: accessorType
      } // Namespace: commLink16NetSelectionType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommLink16ReceiptComplianceWordsType Accessor
         static const uci::base::accessorType::AccessorType commLink16ReceiptComplianceWordsType = 573;

      } // Namespace: accessorType
      //  
      namespace commLink16ReceiptComplianceWordsType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommLink16ReceiptComplianceWordsType::MessageLabel Accessor
            static const uci::base::accessorType::AccessorType messageLabel = 574;

            //  This accessor type is used to identify the uci::type::CommLink16ReceiptComplianceWordsType::MessageSubLabel Accessor
            static const uci::base::accessorType::AccessorType messageSubLabel = 575;

         } // Namespace: accessorType
      } // Namespace: commLink16ReceiptComplianceWordsType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommLink16TxPG_Type Accessor
         static const uci::base::accessorType::AccessorType commLink16TxPG_Type = 576;

      } // Namespace: accessorType
      //  
      namespace commLink16TxPG_Type {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommLink16TxPG_Type::MessagePriority Accessor
            static const uci::base::accessorType::AccessorType messagePriority = 577;

            //  This accessor type is used to identify the uci::type::CommLink16TxPG_Type::MessageTransferStorageLimit Accessor
            static const uci::base::accessorType::AccessorType messageTransferStorageLimit = 578;

            //  This accessor type is used to identify the uci::type::CommLink16TxPG_Type::PG_Index Accessor
            static const uci::base::accessorType::AccessorType pG_Index = 579;

            //  This accessor type is used to identify the uci::type::CommLink16TxPG_Type::SlotNumber Accessor
            static const uci::base::accessorType::AccessorType slotNumber = 580;

            //  This accessor type is used to identify the uci::type::CommLink16TxPG_Type::StalenessLimit Accessor
            static const uci::base::accessorType::AccessorType stalenessLimit = 581;

         } // Namespace: accessorType
      } // Namespace: commLink16TxPG_Type
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommLinkDetailsType Accessor
         static const uci::base::accessorType::AccessorType commLinkDetailsType = 582;

         //  This accessor type is used to identify the uci::type::CommLinkPortMapType Accessor
         static const uci::base::accessorType::AccessorType commLinkPortMapType = 583;

         //  This accessor type is used to identify the uci::type::CommLinkStatisticsType Accessor
         static const uci::base::accessorType::AccessorType commLinkStatisticsType = 584;

         //  This accessor type is used to identify the uci::type::CommMADL_CommWaveformActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType commMADL_CommWaveformActivityCommandType = 585;

         //  This accessor type is used to identify the uci::type::CommMADL_CommWaveformActivityType Accessor
         static const uci::base::accessorType::AccessorType commMADL_CommWaveformActivityType = 586;

      } // Namespace: accessorType
      //  
      namespace commMADL_CommWaveformActivityType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommMADL_CommWaveformActivityType::MADL_UniqueID Accessor
            static const uci::base::accessorType::AccessorType mADL_UniqueID = 587;

            //  This accessor type is used to identify the uci::type::CommMADL_CommWaveformActivityType::MADL_DayOfYearApplied
            //  Accessor
            static const uci::base::accessorType::AccessorType mADL_DayOfYearApplied = 588;

         } // Namespace: accessorType
      } // Namespace: commMADL_CommWaveformActivityType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommMADL_CommWaveformCapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType commMADL_CommWaveformCapabilityCommandType = 589;

         //  This accessor type is used to identify the uci::type::CommMADL_ControlType Accessor
         static const uci::base::accessorType::AccessorType commMADL_ControlType = 590;

      } // Namespace: accessorType
      //  
      namespace commMADL_ControlType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommMADL_ControlType::MADL_DayOfYear Accessor
            static const uci::base::accessorType::AccessorType mADL_DayOfYear = 591;

         } // Namespace: accessorType
      } // Namespace: commMADL_ControlType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommMADL_LinkCommandListType Accessor
         static const uci::base::accessorType::AccessorType commMADL_LinkCommandListType = 592;

         //  This accessor type is used to identify the uci::type::CommMADL_LinkCommandType Accessor
         static const uci::base::accessorType::AccessorType commMADL_LinkCommandType = 593;

         //  This accessor type is used to identify the uci::type::CommMADL_LinkStatusListType Accessor
         static const uci::base::accessorType::AccessorType commMADL_LinkStatusListType = 594;

         //  This accessor type is used to identify the uci::type::CommMADL_LinkStatusType Accessor
         static const uci::base::accessorType::AccessorType commMADL_LinkStatusType = 595;

      } // Namespace: accessorType
      //  
      namespace commMADL_LinkStatusType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommMADL_LinkStatusType::VoiceSource Accessor
            static const uci::base::accessorType::AccessorType voiceSource = 596;

            //  This accessor type is used to identify the uci::type::CommMADL_LinkStatusType::HopOffset Accessor
            static const uci::base::accessorType::AccessorType hopOffset = 597;

         } // Namespace: accessorType
      } // Namespace: commMADL_LinkStatusType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommMADL_NetChangeRequestType Accessor
         static const uci::base::accessorType::AccessorType commMADL_NetChangeRequestType = 598;

      } // Namespace: accessorType
      //  
      namespace commMADL_NetChangeRequestType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommMADL_NetChangeRequestType::NumSelections Accessor
            static const uci::base::accessorType::AccessorType numSelections = 599;

         } // Namespace: accessorType
      } // Namespace: commMADL_NetChangeRequestType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommMADL_NetChangeSelectionListType Accessor
         static const uci::base::accessorType::AccessorType commMADL_NetChangeSelectionListType = 600;

         //  This accessor type is used to identify the uci::type::CommMADL_NetChangeSelectionType Accessor
         static const uci::base::accessorType::AccessorType commMADL_NetChangeSelectionType = 601;

      } // Namespace: accessorType
      //  
      namespace commMADL_NetChangeSelectionType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommMADL_NetChangeSelectionType::SelectionShiftValue Accessor
            static const uci::base::accessorType::AccessorType selectionShiftValue = 602;

         } // Namespace: accessorType
      } // Namespace: commMADL_NetChangeSelectionType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommMADL_NetworkLayoutType Accessor
         static const uci::base::accessorType::AccessorType commMADL_NetworkLayoutType = 603;

         //  This accessor type is used to identify the uci::type::CommMADL_NetworkType Accessor
         static const uci::base::accessorType::AccessorType commMADL_NetworkType = 604;

      } // Namespace: accessorType
      //  
      namespace commMADL_NetworkType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommMADL_NetworkType::MADL_MissionID Accessor
            static const uci::base::accessorType::AccessorType mADL_MissionID = 605;

            //  This accessor type is used to identify the uci::type::CommMADL_NetworkType::MIDE Accessor
            static const uci::base::accessorType::AccessorType mIDE = 606;

         } // Namespace: accessorType
      } // Namespace: commMADL_NetworkType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommMADL_PeerNodeDataType Accessor
         static const uci::base::accessorType::AccessorType commMADL_PeerNodeDataType = 607;

      } // Namespace: accessorType
      //  
      namespace commMADL_PeerNodeDataType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommMADL_PeerNodeDataType::PeerMADL_UniqueID Accessor
            static const uci::base::accessorType::AccessorType peerMADL_UniqueID = 608;

         } // Namespace: accessorType
      } // Namespace: commMADL_PeerNodeDataType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommMADL_SecurityKeyType Accessor
         static const uci::base::accessorType::AccessorType commMADL_SecurityKeyType = 609;

      } // Namespace: accessorType
      //  
      namespace commMADL_SecurityKeyType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommMADL_SecurityKeyType::SecurityCategory Accessor
            static const uci::base::accessorType::AccessorType securityCategory = 610;

         } // Namespace: accessorType
      } // Namespace: commMADL_SecurityKeyType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommMADL_TopologyEntryType Accessor
         static const uci::base::accessorType::AccessorType commMADL_TopologyEntryType = 611;

      } // Namespace: accessorType
      //  
      namespace commMADL_TopologyEntryType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::CommMADL_TopologyEntryType::MemberMADL_UniqueID Accessor
            static const uci::base::accessorType::AccessorType memberMADL_UniqueID = 612;

         } // Namespace: accessorType
      } // Namespace: commMADL_TopologyEntryType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::CommModeUsageType Accessor
         static const uci::base::accessorType::AccessorType commModeUsageType = 613;

         //  This accessor type is used to identify the uci::type::CommOutageType Accessor
         static const uci::base::accessorType::AccessorType commOutageType = 614;

         //  This accessor type is used to identify the uci::type::CommPlanningTriggerType Accessor
         static const uci::base::accessorType::AccessorType commPlanningTriggerType = 615;

         //  This accessor type is used to identify the uci::type::CommPlanTriggerType Accessor
         static const uci::base::accessorType::AccessorType commPlanTriggerType = 616;

         //  This accessor type is used to identify the uci::type::CommPointingCommandID_Type Accessor
         static const uci::base::accessorType::AccessorType commPointingCommandID_Type = 617;

         //  This accessor type is used to identify the uci::type::CommPointingCommandMDT Accessor
         static const uci::base::accessorType::AccessorType commPointingCommandMDT = 618;

         //  This accessor type is used to identify the uci::type::CommPointingCommandMT Accessor
         static const uci::base::accessorType::AccessorType commPointingCommandMT = 619;

         //  This accessor type is used to identify the uci::type::CommPointingCommandQFT Accessor
         static const uci::base::accessorType::AccessorType commPointingCommandQFT = 620;

         //  This accessor type is used to identify the uci::type::CommPointingCommandStatusMDT Accessor
         static const uci::base::accessorType::AccessorType commPointingCommandStatusMDT = 621;

         //  This accessor type is used to identify the uci::type::CommPointingCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType commPointingCommandStatusMT = 622;

         //  This accessor type is used to identify the uci::type::CommPointingCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType commPointingCommandStatusQFT = 623;

         //  This accessor type is used to identify the uci::type::CommPointingID_Type Accessor
         static const uci::base::accessorType::AccessorType commPointingID_Type = 624;

         //  This accessor type is used to identify the uci::type::CommPointingMDT Accessor
         static const uci::base::accessorType::AccessorType commPointingMDT = 625;

         //  This accessor type is used to identify the uci::type::CommPointingMT Accessor
         static const uci::base::accessorType::AccessorType commPointingMT = 626;

         //  This accessor type is used to identify the uci::type::CommPointingPlanAssessmentType Accessor
         static const uci::base::accessorType::AccessorType commPointingPlanAssessmentType = 627;

         //  This accessor type is used to identify the uci::type::CommPointingPlanDataType Accessor
         static const uci::base::accessorType::AccessorType commPointingPlanDataType = 628;

         //  This accessor type is used to identify the uci::type::CommPointingPlanRequestType Accessor
         static const uci::base::accessorType::AccessorType commPointingPlanRequestType = 629;

         //  This accessor type is used to identify the uci::type::CommPointingQFT Accessor
         static const uci::base::accessorType::AccessorType commPointingQFT = 630;

         //  This accessor type is used to identify the uci::type::CommPointingRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType commPointingRequestID_Type = 631;

         //  This accessor type is used to identify the uci::type::CommPointingRequestMDT Accessor
         static const uci::base::accessorType::AccessorType commPointingRequestMDT = 632;

         //  This accessor type is used to identify the uci::type::CommPointingRequestMT Accessor
         static const uci::base::accessorType::AccessorType commPointingRequestMT = 633;

         //  This accessor type is used to identify the uci::type::CommPointingRequestQFT Accessor
         static const uci::base::accessorType::AccessorType commPointingRequestQFT = 634;

         //  This accessor type is used to identify the uci::type::CommPointingRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType commPointingRequestStatusMDT = 635;

         //  This accessor type is used to identify the uci::type::CommPointingRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType commPointingRequestStatusMT = 636;

         //  This accessor type is used to identify the uci::type::CommPointingRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType commPointingRequestStatusQFT = 637;

         //  This accessor type is used to identify the uci::type::CommPointingType Accessor
         static const uci::base::accessorType::AccessorType commPointingType = 638;

         //  This accessor type is used to identify the uci::type::CommPortSettingsType Accessor
         static const uci::base::accessorType::AccessorType commPortSettingsType = 639;

         //  This accessor type is used to identify the uci::type::CommProfileID_Type Accessor
         static const uci::base::accessorType::AccessorType commProfileID_Type = 640;

         //  This accessor type is used to identify the uci::type::CommRangeDelayChoiceType Accessor
         static const uci::base::accessorType::AccessorType commRangeDelayChoiceType = 641;

         //  This accessor type is used to identify the uci::type::CommRelayActivityMDT Accessor
         static const uci::base::accessorType::AccessorType commRelayActivityMDT = 642;

         //  This accessor type is used to identify the uci::type::CommRelayActivityMT Accessor
         static const uci::base::accessorType::AccessorType commRelayActivityMT = 643;

         //  This accessor type is used to identify the uci::type::CommRelayActivityQFT Accessor
         static const uci::base::accessorType::AccessorType commRelayActivityQFT = 644;

         //  This accessor type is used to identify the uci::type::CommRelayActivityType Accessor
         static const uci::base::accessorType::AccessorType commRelayActivityType = 645;

         //  This accessor type is used to identify the uci::type::CommRelayCapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType commRelayCapabilityCommandType = 646;

         //  This accessor type is used to identify the uci::type::CommRelayCapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType commRelayCapabilityMDT = 647;

         //  This accessor type is used to identify the uci::type::CommRelayCapabilityMT Accessor
         static const uci::base::accessorType::AccessorType commRelayCapabilityMT = 648;

         //  This accessor type is used to identify the uci::type::CommRelayCapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType commRelayCapabilityQFT = 649;

         //  This accessor type is used to identify the uci::type::CommRelayCapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType commRelayCapabilityStatusMDT = 650;

         //  This accessor type is used to identify the uci::type::CommRelayCapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType commRelayCapabilityStatusMT = 651;

         //  This accessor type is used to identify the uci::type::CommRelayCapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType commRelayCapabilityStatusQFT = 652;

         //  This accessor type is used to identify the uci::type::CommRelayCapabilityType Accessor
         static const uci::base::accessorType::AccessorType commRelayCapabilityType = 653;

         //  This accessor type is used to identify the uci::type::CommRelayCommandMDT Accessor
         static const uci::base::accessorType::AccessorType commRelayCommandMDT = 654;

         //  This accessor type is used to identify the uci::type::CommRelayCommandMT Accessor
         static const uci::base::accessorType::AccessorType commRelayCommandMT = 655;

         //  This accessor type is used to identify the uci::type::CommRelayCommandQFT Accessor
         static const uci::base::accessorType::AccessorType commRelayCommandQFT = 656;

         //  This accessor type is used to identify the uci::type::CommRelayCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType commRelayCommandStatusMT = 657;

         //  This accessor type is used to identify the uci::type::CommRelayCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType commRelayCommandStatusQFT = 658;

         //  This accessor type is used to identify the uci::type::CommRelayCommandType Accessor
         static const uci::base::accessorType::AccessorType commRelayCommandType = 659;

         //  This accessor type is used to identify the uci::type::CommRelayOptionsType Accessor
         static const uci::base::accessorType::AccessorType commRelayOptionsType = 660;

         //  This accessor type is used to identify the uci::type::CommRelaySettingsCommandMDT Accessor
         static const uci::base::accessorType::AccessorType commRelaySettingsCommandMDT = 661;

         //  This accessor type is used to identify the uci::type::CommRelaySettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType commRelaySettingsCommandMT = 662;

         //  This accessor type is used to identify the uci::type::CommRelaySettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType commRelaySettingsCommandQFT = 663;

         //  This accessor type is used to identify the uci::type::CommRelaySettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType commRelaySettingsCommandStatusMT = 664;

         //  This accessor type is used to identify the uci::type::CommRelaySettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType commRelaySettingsCommandStatusQFT = 665;

         //  This accessor type is used to identify the uci::type::CommRelaySpecificDataType Accessor
         static const uci::base::accessorType::AccessorType commRelaySpecificDataType = 666;

         //  This accessor type is used to identify the uci::type::CommRelayTaskBaseType Accessor
         static const uci::base::accessorType::AccessorType commRelayTaskBaseType = 667;

         //  This accessor type is used to identify the uci::type::CommRelayTaskMetricsType Accessor
         static const uci::base::accessorType::AccessorType commRelayTaskMetricsType = 668;

         //  This accessor type is used to identify the uci::type::CommRelayTaskType Accessor
         static const uci::base::accessorType::AccessorType commRelayTaskType = 669;

         //  This accessor type is used to identify the uci::type::CommReplanReasonType Accessor
         static const uci::base::accessorType::AccessorType commReplanReasonType = 670;

         //  This accessor type is used to identify the uci::type::CommRequirementMissionPlanReferenceType Accessor
         static const uci::base::accessorType::AccessorType commRequirementMissionPlanReferenceType = 671;

         //  This accessor type is used to identify the uci::type::CommResourceType Accessor
         static const uci::base::accessorType::AccessorType commResourceType = 672;

         //  This accessor type is used to identify the uci::type::CommScheduleAllocationID_Type Accessor
         static const uci::base::accessorType::AccessorType commScheduleAllocationID_Type = 673;

         //  This accessor type is used to identify the uci::type::CommScheduleAllocationMDT Accessor
         static const uci::base::accessorType::AccessorType commScheduleAllocationMDT = 674;

         //  This accessor type is used to identify the uci::type::CommScheduleAllocationMT Accessor
         static const uci::base::accessorType::AccessorType commScheduleAllocationMT = 675;

         //  This accessor type is used to identify the uci::type::CommScheduleAllocationQFT Accessor
         static const uci::base::accessorType::AccessorType commScheduleAllocationQFT = 676;

         //  This accessor type is used to identify the uci::type::CommScheduleAllocationUpdateRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType commScheduleAllocationUpdateRequestID_Type = 677;

         //  This accessor type is used to identify the uci::type::CommScheduleAllocationUpdateRequestMDT Accessor
         static const uci::base::accessorType::AccessorType commScheduleAllocationUpdateRequestMDT = 678;

         //  This accessor type is used to identify the uci::type::CommScheduleAllocationUpdateRequestMT Accessor
         static const uci::base::accessorType::AccessorType commScheduleAllocationUpdateRequestMT = 679;

         //  This accessor type is used to identify the uci::type::CommScheduleAllocationUpdateRequestQFT Accessor
         static const uci::base::accessorType::AccessorType commScheduleAllocationUpdateRequestQFT = 680;

         //  This accessor type is used to identify the uci::type::CommScheduleAllocationUpdateRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType commScheduleAllocationUpdateRequestStatusMDT = 681;

         //  This accessor type is used to identify the uci::type::CommScheduleAllocationUpdateRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType commScheduleAllocationUpdateRequestStatusMT = 682;

         //  This accessor type is used to identify the uci::type::CommScheduleAllocationUpdateRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType commScheduleAllocationUpdateRequestStatusQFT = 683;

         //  This accessor type is used to identify the uci::type::CommScheduleAvailabilityID_Type Accessor
         static const uci::base::accessorType::AccessorType commScheduleAvailabilityID_Type = 684;

         //  This accessor type is used to identify the uci::type::CommScheduleAvailabilityMDT Accessor
         static const uci::base::accessorType::AccessorType commScheduleAvailabilityMDT = 685;

         //  This accessor type is used to identify the uci::type::CommScheduleAvailabilityMT Accessor
         static const uci::base::accessorType::AccessorType commScheduleAvailabilityMT = 686;

         //  This accessor type is used to identify the uci::type::CommScheduleAvailabilityQFT Accessor
         static const uci::base::accessorType::AccessorType commScheduleAvailabilityQFT = 687;

         //  This accessor type is used to identify the uci::type::CommSchedulingRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType commSchedulingRequestID_Type = 688;

         //  This accessor type is used to identify the uci::type::CommSchedulingRequestMDT Accessor
         static const uci::base::accessorType::AccessorType commSchedulingRequestMDT = 689;

         //  This accessor type is used to identify the uci::type::CommSchedulingRequestMT Accessor
         static const uci::base::accessorType::AccessorType commSchedulingRequestMT = 690;

         //  This accessor type is used to identify the uci::type::CommSchedulingRequestQFT Accessor
         static const uci::base::accessorType::AccessorType commSchedulingRequestQFT = 691;

         //  This accessor type is used to identify the uci::type::CommSchedulingRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType commSchedulingRequestStatusMDT = 692;

         //  This accessor type is used to identify the uci::type::CommSchedulingRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType commSchedulingRequestStatusMT = 693;

         //  This accessor type is used to identify the uci::type::CommSchedulingRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType commSchedulingRequestStatusQFT = 694;

         //  This accessor type is used to identify the uci::type::CommSchedulingRequirementID_Type Accessor
         static const uci::base::accessorType::AccessorType commSchedulingRequirementID_Type = 695;

         //  This accessor type is used to identify the uci::type::CommSchedulingRequirementMDT Accessor
         static const uci::base::accessorType::AccessorType commSchedulingRequirementMDT = 696;

         //  This accessor type is used to identify the uci::type::CommSchedulingRequirementMT Accessor
         static const uci::base::accessorType::AccessorType commSchedulingRequirementMT = 697;

         //  This accessor type is used to identify the uci::type::CommSchedulingRequirementQFT Accessor
         static const uci::base::accessorType::AccessorType commSchedulingRequirementQFT = 698;

         //  This accessor type is used to identify the uci::type::CommSINCGARS_CommWaveformActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType commSINCGARS_CommWaveformActivityCommandType = 699;

         //  This accessor type is used to identify the uci::type::CommSINCGARS_CommWaveformActivityType Accessor
         static const uci::base::accessorType::AccessorType commSINCGARS_CommWaveformActivityType = 700;

         //  This accessor type is used to identify the uci::type::CommSINCGARS_CommWaveformCapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType commSINCGARS_CommWaveformCapabilityCommandType = 701;

         //  This accessor type is used to identify the uci::type::CommSINCGARS_ControlType Accessor
         static const uci::base::accessorType::AccessorType commSINCGARS_ControlType = 702;

         //  This accessor type is used to identify the uci::type::CommsLostTriggerDataType Accessor
         static const uci::base::accessorType::AccessorType commsLostTriggerDataType = 703;

         //  This accessor type is used to identify the uci::type::CommSpecificRequestDataType Accessor
         static const uci::base::accessorType::AccessorType commSpecificRequestDataType = 704;

         //  This accessor type is used to identify the uci::type::CommSystemID_Type Accessor
         static const uci::base::accessorType::AccessorType commSystemID_Type = 705;

         //  This accessor type is used to identify the uci::type::CommSystemMDT Accessor
         static const uci::base::accessorType::AccessorType commSystemMDT = 706;

         //  This accessor type is used to identify the uci::type::CommSystemMT Accessor
         static const uci::base::accessorType::AccessorType commSystemMT = 707;

         //  This accessor type is used to identify the uci::type::CommSystemQFT Accessor
         static const uci::base::accessorType::AccessorType commSystemQFT = 708;

         //  This accessor type is used to identify the uci::type::CommSystemUsageType Accessor
         static const uci::base::accessorType::AccessorType commSystemUsageType = 709;

         //  This accessor type is used to identify the uci::type::CommTerminalActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType commTerminalActivityCommandType = 710;

         //  This accessor type is used to identify the uci::type::CommTerminalActivityMDT Accessor
         static const uci::base::accessorType::AccessorType commTerminalActivityMDT = 711;

         //  This accessor type is used to identify the uci::type::CommTerminalActivityMT Accessor
         static const uci::base::accessorType::AccessorType commTerminalActivityMT = 712;

         //  This accessor type is used to identify the uci::type::CommTerminalActivityQFT Accessor
         static const uci::base::accessorType::AccessorType commTerminalActivityQFT = 713;

         //  This accessor type is used to identify the uci::type::CommTerminalActivityType Accessor
         static const uci::base::accessorType::AccessorType commTerminalActivityType = 714;

         //  This accessor type is used to identify the uci::type::CommTerminalCapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType commTerminalCapabilityCommandType = 715;

         //  This accessor type is used to identify the uci::type::CommTerminalCapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType commTerminalCapabilityMDT = 716;

         //  This accessor type is used to identify the uci::type::CommTerminalCapabilityMT Accessor
         static const uci::base::accessorType::AccessorType commTerminalCapabilityMT = 717;

         //  This accessor type is used to identify the uci::type::CommTerminalCapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType commTerminalCapabilityQFT = 718;

         //  This accessor type is used to identify the uci::type::CommTerminalCapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType commTerminalCapabilityStatusMDT = 719;

         //  This accessor type is used to identify the uci::type::CommTerminalCapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType commTerminalCapabilityStatusMT = 720;

         //  This accessor type is used to identify the uci::type::CommTerminalCapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType commTerminalCapabilityStatusQFT = 721;

         //  This accessor type is used to identify the uci::type::CommTerminalCapabilityStatusType Accessor
         static const uci::base::accessorType::AccessorType commTerminalCapabilityStatusType = 722;

         //  This accessor type is used to identify the uci::type::CommTerminalCapabilityType Accessor
         static const uci::base::accessorType::AccessorType commTerminalCapabilityType = 723;

         //  This accessor type is used to identify the uci::type::CommTerminalCommandMDT Accessor
         static const uci::base::accessorType::AccessorType commTerminalCommandMDT = 724;

         //  This accessor type is used to identify the uci::type::CommTerminalCommandMT Accessor
         static const uci::base::accessorType::AccessorType commTerminalCommandMT = 725;

         //  This accessor type is used to identify the uci::type::CommTerminalCommandQFT Accessor
         static const uci::base::accessorType::AccessorType commTerminalCommandQFT = 726;

         //  This accessor type is used to identify the uci::type::CommTerminalCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType commTerminalCommandStatusMT = 727;

         //  This accessor type is used to identify the uci::type::CommTerminalCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType commTerminalCommandStatusQFT = 728;

         //  This accessor type is used to identify the uci::type::CommTerminalCommandType Accessor
         static const uci::base::accessorType::AccessorType commTerminalCommandType = 729;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanActivationCommandID_Type Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanActivationCommandID_Type = 730;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanActivationCommandMDT Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanActivationCommandMDT = 731;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanActivationCommandMT Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanActivationCommandMT = 732;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanActivationCommandQFT Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanActivationCommandQFT = 733;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanActivationCommandStatusMDT Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanActivationCommandStatusMDT = 734;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanActivationCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanActivationCommandStatusMT = 735;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanActivationCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanActivationCommandStatusQFT = 736;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanActivationStateMDT Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanActivationStateMDT = 737;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanActivationStateMT Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanActivationStateMT = 738;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanActivationStateQFT Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanActivationStateQFT = 739;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanConfigurationActivationType Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanConfigurationActivationType = 740;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanConfigurationStateType Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanConfigurationStateType = 741;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanDetailsType Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanDetailsType = 742;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanEffectivityActivationType Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanEffectivityActivationType = 743;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanEffectivityStateType Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanEffectivityStateType = 744;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanID_Type Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanID_Type = 745;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanMDT Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanMDT = 746;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanMT Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanMT = 747;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanQFT Accessor
         static const uci::base::accessorType::AccessorType commTerminalPlanQFT = 748;

         //  This accessor type is used to identify the uci::type::CommTerminalSettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType commTerminalSettingsCommandMT = 749;

         //  This accessor type is used to identify the uci::type::CommTerminalSettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType commTerminalSettingsCommandQFT = 750;

         //  This accessor type is used to identify the uci::type::CommTerminalSettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType commTerminalSettingsCommandStatusMT = 751;

         //  This accessor type is used to identify the uci::type::CommTerminalSettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType commTerminalSettingsCommandStatusQFT = 752;

         //  This accessor type is used to identify the uci::type::CommType Accessor
         static const uci::base::accessorType::AccessorType commType = 753;

         //  This accessor type is used to identify the uci::type::CommUHF_SATCOM_CommWaveformActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType commUHF_SATCOM_CommWaveformActivityCommandType = 754;

         //  This accessor type is used to identify the uci::type::CommUHF_SATCOM_CommWaveformActivityType Accessor
         static const uci::base::accessorType::AccessorType commUHF_SATCOM_CommWaveformActivityType = 755;

         //  This accessor type is used to identify the uci::type::CommUHF_SATCOM_CommWaveformCapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType commUHF_SATCOM_CommWaveformCapabilityCommandType = 756;

         //  This accessor type is used to identify the uci::type::CommunicationsEmitterIdentityType Accessor
         static const uci::base::accessorType::AccessorType communicationsEmitterIdentityType = 757;

         //  This accessor type is used to identify the uci::type::CommUserIO_ID_Type Accessor
         static const uci::base::accessorType::AccessorType commUserIO_ID_Type = 758;

         //  This accessor type is used to identify the uci::type::CommUserLinkID_Type Accessor
         static const uci::base::accessorType::AccessorType commUserLinkID_Type = 759;

         //  This accessor type is used to identify the uci::type::CommUserLinkType Accessor
         static const uci::base::accessorType::AccessorType commUserLinkType = 760;

         //  This accessor type is used to identify the uci::type::CommUtilizationRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType commUtilizationRequestID_Type = 761;

         //  This accessor type is used to identify the uci::type::CommUtilizationRequestMDT Accessor
         static const uci::base::accessorType::AccessorType commUtilizationRequestMDT = 762;

         //  This accessor type is used to identify the uci::type::CommUtilizationRequestMT Accessor
         static const uci::base::accessorType::AccessorType commUtilizationRequestMT = 763;

         //  This accessor type is used to identify the uci::type::CommUtilizationRequestQFT Accessor
         static const uci::base::accessorType::AccessorType commUtilizationRequestQFT = 764;

         //  This accessor type is used to identify the uci::type::CommUtilizationRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType commUtilizationRequestStatusMDT = 765;

         //  This accessor type is used to identify the uci::type::CommUtilizationRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType commUtilizationRequestStatusMT = 766;

         //  This accessor type is used to identify the uci::type::CommUtilizationRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType commUtilizationRequestStatusQFT = 767;

         //  This accessor type is used to identify the uci::type::CommWaveformActivityCommandPET Accessor
         static const uci::base::accessorType::AccessorType commWaveformActivityCommandPET = 768;

         //  This accessor type is used to identify the uci::type::CommWaveformActivityPET Accessor
         static const uci::base::accessorType::AccessorType commWaveformActivityPET = 769;

         //  This accessor type is used to identify the uci::type::CommWaveformCapabilityCommandPET Accessor
         static const uci::base::accessorType::AccessorType commWaveformCapabilityCommandPET = 770;

         //  This accessor type is used to identify the uci::type::CompletedTaskMetricsType Accessor
         static const uci::base::accessorType::AccessorType completedTaskMetricsType = 771;

         //  This accessor type is used to identify the uci::type::CompletionStatusType Accessor
         static const uci::base::accessorType::AccessorType completionStatusType = 772;

         //  This accessor type is used to identify the uci::type::ComponentControllabilityA_Type Accessor
         static const uci::base::accessorType::AccessorType componentControllabilityA_Type = 773;

         //  This accessor type is used to identify the uci::type::ComponentControllabilityB_Type Accessor
         static const uci::base::accessorType::AccessorType componentControllabilityB_Type = 774;

         //  This accessor type is used to identify the uci::type::ComponentControlsA_Type Accessor
         static const uci::base::accessorType::AccessorType componentControlsA_Type = 775;

         //  This accessor type is used to identify the uci::type::ComponentControlsB_Type Accessor
         static const uci::base::accessorType::AccessorType componentControlsB_Type = 776;

         //  This accessor type is used to identify the uci::type::ComponentControlsStatusType Accessor
         static const uci::base::accessorType::AccessorType componentControlsStatusType = 777;

         //  This accessor type is used to identify the uci::type::ComponentFieldOfRegardType Accessor
         static const uci::base::accessorType::AccessorType componentFieldOfRegardType = 778;

         //  This accessor type is used to identify the uci::type::ComponentID_Type Accessor
         static const uci::base::accessorType::AccessorType componentID_Type = 779;

         //  This accessor type is used to identify the uci::type::ComponentLocationType Accessor
         static const uci::base::accessorType::AccessorType componentLocationType = 780;

         //  This accessor type is used to identify the uci::type::ComponentOrientationType Accessor
         static const uci::base::accessorType::AccessorType componentOrientationType = 781;

         //  This accessor type is used to identify the uci::type::CompressionRatioSettingsType Accessor
         static const uci::base::accessorType::AccessorType compressionRatioSettingsType = 782;

         //  This accessor type is used to identify the uci::type::ConcurrentCollectionDetailsType Accessor
         static const uci::base::accessorType::AccessorType concurrentCollectionDetailsType = 783;

         //  This accessor type is used to identify the uci::type::ConcurrentTaskReferenceType Accessor
         static const uci::base::accessorType::AccessorType concurrentTaskReferenceType = 784;

         //  This accessor type is used to identify the uci::type::ConditionalPathSegmentType Accessor
         static const uci::base::accessorType::AccessorType conditionalPathSegmentType = 785;

         //  This accessor type is used to identify the uci::type::ConfigurationParameterCategoryType Accessor
         static const uci::base::accessorType::AccessorType configurationParameterCategoryType = 786;

         //  This accessor type is used to identify the uci::type::ConfigurationParameterRangeType Accessor
         static const uci::base::accessorType::AccessorType configurationParameterRangeType = 787;

         //  This accessor type is used to identify the uci::type::ConfigurationParameterType Accessor
         static const uci::base::accessorType::AccessorType configurationParameterType = 788;

         //  This accessor type is used to identify the uci::type::ConfigurationParameterValueRestrictionsType Accessor
         static const uci::base::accessorType::AccessorType configurationParameterValueRestrictionsType = 789;

         //  This accessor type is used to identify the uci::type::ConfigureInteroperabilityReportType Accessor
         static const uci::base::accessorType::AccessorType configureInteroperabilityReportType = 790;

         //  This accessor type is used to identify the uci::type::ConflictedRouteSegmentType Accessor
         static const uci::base::accessorType::AccessorType conflictedRouteSegmentType = 791;

         //  This accessor type is used to identify the uci::type::ConflictLocationType Accessor
         static const uci::base::accessorType::AccessorType conflictLocationType = 792;

         //  This accessor type is used to identify the uci::type::ConflictType Accessor
         static const uci::base::accessorType::AccessorType conflictType = 793;

         //  This accessor type is used to identify the uci::type::ConsentRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType consentRequestID_Type = 794;

         //  This accessor type is used to identify the uci::type::ConstrainedEntityType Accessor
         static const uci::base::accessorType::AccessorType constrainedEntityType = 795;

         //  This accessor type is used to identify the uci::type::ConstrainedOpLineType Accessor
         static const uci::base::accessorType::AccessorType constrainedOpLineType = 796;

         //  This accessor type is used to identify the uci::type::ConstrainedOpPointType Accessor
         static const uci::base::accessorType::AccessorType constrainedOpPointType = 797;

         //  This accessor type is used to identify the uci::type::ConstrainedOpZoneType Accessor
         static const uci::base::accessorType::AccessorType constrainedOpZoneType = 798;

         //  This accessor type is used to identify the uci::type::ContactDetailsType Accessor
         static const uci::base::accessorType::AccessorType contactDetailsType = 799;

         //  This accessor type is used to identify the uci::type::ContingencyPathAutonomyType Accessor
         static const uci::base::accessorType::AccessorType contingencyPathAutonomyType = 800;

         //  This accessor type is used to identify the uci::type::ContingencyPathSpacingType Accessor
         static const uci::base::accessorType::AccessorType contingencyPathSpacingType = 801;

         //  This accessor type is used to identify the uci::type::ControleeType Accessor
         static const uci::base::accessorType::AccessorType controleeType = 802;

         //  This accessor type is used to identify the uci::type::ControlInterfacesCapabilityControlType Accessor
         static const uci::base::accessorType::AccessorType controlInterfacesCapabilityControlType = 803;

         //  This accessor type is used to identify the uci::type::ControlInterfacesCommandID_Type Accessor
         static const uci::base::accessorType::AccessorType controlInterfacesCommandID_Type = 804;

         //  This accessor type is used to identify the uci::type::ControlInterfacesCommandMDT Accessor
         static const uci::base::accessorType::AccessorType controlInterfacesCommandMDT = 805;

         //  This accessor type is used to identify the uci::type::ControlInterfacesCommandMT Accessor
         static const uci::base::accessorType::AccessorType controlInterfacesCommandMT = 806;

         //  This accessor type is used to identify the uci::type::ControlInterfacesCommandQFT Accessor
         static const uci::base::accessorType::AccessorType controlInterfacesCommandQFT = 807;

         //  This accessor type is used to identify the uci::type::ControlInterfacesCommandStatusMDT Accessor
         static const uci::base::accessorType::AccessorType controlInterfacesCommandStatusMDT = 808;

         //  This accessor type is used to identify the uci::type::ControlInterfacesCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType controlInterfacesCommandStatusMT = 809;

         //  This accessor type is used to identify the uci::type::ControlInterfacesCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType controlInterfacesCommandStatusQFT = 810;

         //  This accessor type is used to identify the uci::type::ControlInterfacesControlType Accessor
         static const uci::base::accessorType::AccessorType controlInterfacesControlType = 811;

         //  This accessor type is used to identify the uci::type::ControlInterfacesMissionControlType Accessor
         static const uci::base::accessorType::AccessorType controlInterfacesMissionControlType = 812;

         //  This accessor type is used to identify the uci::type::ControllerType Accessor
         static const uci::base::accessorType::AccessorType controllerType = 813;

         //  This accessor type is used to identify the uci::type::ControlRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType controlRequestID_Type = 814;

         //  This accessor type is used to identify the uci::type::ControlRequestMDT Accessor
         static const uci::base::accessorType::AccessorType controlRequestMDT = 815;

         //  This accessor type is used to identify the uci::type::ControlRequestMT Accessor
         static const uci::base::accessorType::AccessorType controlRequestMT = 816;

         //  This accessor type is used to identify the uci::type::ControlRequestQFT Accessor
         static const uci::base::accessorType::AccessorType controlRequestQFT = 817;

         //  This accessor type is used to identify the uci::type::ControlRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType controlRequestStatusMDT = 818;

         //  This accessor type is used to identify the uci::type::ControlRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType controlRequestStatusMT = 819;

         //  This accessor type is used to identify the uci::type::ControlRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType controlRequestStatusQFT = 820;

         //  This accessor type is used to identify the uci::type::ControlSourcesCapabilityControlType Accessor
         static const uci::base::accessorType::AccessorType controlSourcesCapabilityControlType = 821;

         //  This accessor type is used to identify the uci::type::ControlSourcesCommandID_Type Accessor
         static const uci::base::accessorType::AccessorType controlSourcesCommandID_Type = 822;

         //  This accessor type is used to identify the uci::type::ControlSourcesCommandMDT Accessor
         static const uci::base::accessorType::AccessorType controlSourcesCommandMDT = 823;

         //  This accessor type is used to identify the uci::type::ControlSourcesCommandMT Accessor
         static const uci::base::accessorType::AccessorType controlSourcesCommandMT = 824;

         //  This accessor type is used to identify the uci::type::ControlSourcesCommandQFT Accessor
         static const uci::base::accessorType::AccessorType controlSourcesCommandQFT = 825;

         //  This accessor type is used to identify the uci::type::ControlSourcesCommandStatusMDT Accessor
         static const uci::base::accessorType::AccessorType controlSourcesCommandStatusMDT = 826;

         //  This accessor type is used to identify the uci::type::ControlSourcesCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType controlSourcesCommandStatusMT = 827;

         //  This accessor type is used to identify the uci::type::ControlSourcesCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType controlSourcesCommandStatusQFT = 828;

         //  This accessor type is used to identify the uci::type::ControlSourcesControlType Accessor
         static const uci::base::accessorType::AccessorType controlSourcesControlType = 829;

         //  This accessor type is used to identify the uci::type::ControlStatusCapabilityControlType Accessor
         static const uci::base::accessorType::AccessorType controlStatusCapabilityControlType = 830;

         //  This accessor type is used to identify the uci::type::ControlStatusControlType Accessor
         static const uci::base::accessorType::AccessorType controlStatusControlType = 831;

         //  This accessor type is used to identify the uci::type::ControlStatusMDT Accessor
         static const uci::base::accessorType::AccessorType controlStatusMDT = 832;

         //  This accessor type is used to identify the uci::type::ControlStatusMissionControlType Accessor
         static const uci::base::accessorType::AccessorType controlStatusMissionControlType = 833;

         //  This accessor type is used to identify the uci::type::ControlStatusMT Accessor
         static const uci::base::accessorType::AccessorType controlStatusMT = 834;

         //  This accessor type is used to identify the uci::type::ControlStatusQFT Accessor
         static const uci::base::accessorType::AccessorType controlStatusQFT = 835;

         //  This accessor type is used to identify the uci::type::CoordinatedTaskSetID_Type Accessor
         static const uci::base::accessorType::AccessorType coordinatedTaskSetID_Type = 836;

         //  This accessor type is used to identify the uci::type::CoordinatedTaskSetMDT Accessor
         static const uci::base::accessorType::AccessorType coordinatedTaskSetMDT = 837;

         //  This accessor type is used to identify the uci::type::CoordinatedTaskSetMT Accessor
         static const uci::base::accessorType::AccessorType coordinatedTaskSetMT = 838;

         //  This accessor type is used to identify the uci::type::CoordinatedTaskSetQFT Accessor
         static const uci::base::accessorType::AccessorType coordinatedTaskSetQFT = 839;

         //  This accessor type is used to identify the uci::type::CoordinatedTaskSetStatusMDT Accessor
         static const uci::base::accessorType::AccessorType coordinatedTaskSetStatusMDT = 840;

         //  This accessor type is used to identify the uci::type::CoordinatedTaskSetStatusMT Accessor
         static const uci::base::accessorType::AccessorType coordinatedTaskSetStatusMT = 841;

         //  This accessor type is used to identify the uci::type::CoordinatedTaskSetStatusQFT Accessor
         static const uci::base::accessorType::AccessorType coordinatedTaskSetStatusQFT = 842;

         //  This accessor type is used to identify the uci::type::CoordinationConditionType Accessor
         static const uci::base::accessorType::AccessorType coordinationConditionType = 843;

         //  This accessor type is used to identify the uci::type::CornerPointImageRegionOfInterestType Accessor
         static const uci::base::accessorType::AccessorType cornerPointImageRegionOfInterestType = 844;

         //  This accessor type is used to identify the uci::type::CorrelatedEntityID_Type Accessor
         static const uci::base::accessorType::AccessorType correlatedEntityID_Type = 845;

         //  This accessor type is used to identify the uci::type::CryptoCommandMDT Accessor
         static const uci::base::accessorType::AccessorType cryptoCommandMDT = 846;

         //  This accessor type is used to identify the uci::type::CryptoCommandMT Accessor
         static const uci::base::accessorType::AccessorType cryptoCommandMT = 847;

         //  This accessor type is used to identify the uci::type::CryptoCommandQFT Accessor
         static const uci::base::accessorType::AccessorType cryptoCommandQFT = 848;

         //  This accessor type is used to identify the uci::type::CryptoCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType cryptoCommandStatusMT = 849;

         //  This accessor type is used to identify the uci::type::CryptoCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType cryptoCommandStatusQFT = 850;

         //  This accessor type is used to identify the uci::type::CryptoLocationType Accessor
         static const uci::base::accessorType::AccessorType cryptoLocationType = 851;

         //  This accessor type is used to identify the uci::type::CryptoPortID_Type Accessor
         static const uci::base::accessorType::AccessorType cryptoPortID_Type = 852;

         //  This accessor type is used to identify the uci::type::CryptoPortStatusType Accessor
         static const uci::base::accessorType::AccessorType cryptoPortStatusType = 853;

         //  This accessor type is used to identify the uci::type::CryptoRolloverCommandType Accessor
         static const uci::base::accessorType::AccessorType cryptoRolloverCommandType = 854;

         //  This accessor type is used to identify the uci::type::CryptoSlotGroupID_Type Accessor
         static const uci::base::accessorType::AccessorType cryptoSlotGroupID_Type = 855;

         //  This accessor type is used to identify the uci::type::CryptoSlotGroupType Accessor
         static const uci::base::accessorType::AccessorType cryptoSlotGroupType = 856;

         //  This accessor type is used to identify the uci::type::CryptoSlotType Accessor
         static const uci::base::accessorType::AccessorType cryptoSlotType = 857;

         //  This accessor type is used to identify the uci::type::CryptoStatusMDT Accessor
         static const uci::base::accessorType::AccessorType cryptoStatusMDT = 858;

         //  This accessor type is used to identify the uci::type::CryptoStatusMT Accessor
         static const uci::base::accessorType::AccessorType cryptoStatusMT = 859;

         //  This accessor type is used to identify the uci::type::CryptoStatusQFT Accessor
         static const uci::base::accessorType::AccessorType cryptoStatusQFT = 860;

         //  This accessor type is used to identify the uci::type::CurrentSegmentID_Type Accessor
         static const uci::base::accessorType::AccessorType currentSegmentID_Type = 861;

         //  This accessor type is used to identify the uci::type::DamageAssessmentDetailType Accessor
         static const uci::base::accessorType::AccessorType damageAssessmentDetailType = 862;

         //  This accessor type is used to identify the uci::type::DamageAssessmentID_Type Accessor
         static const uci::base::accessorType::AccessorType damageAssessmentID_Type = 863;

         //  This accessor type is used to identify the uci::type::DamageAssessmentMDT Accessor
         static const uci::base::accessorType::AccessorType damageAssessmentMDT = 864;

         //  This accessor type is used to identify the uci::type::DamageAssessmentMT Accessor
         static const uci::base::accessorType::AccessorType damageAssessmentMT = 865;

         //  This accessor type is used to identify the uci::type::DamageAssessmentQFT Accessor
         static const uci::base::accessorType::AccessorType damageAssessmentQFT = 866;

         //  This accessor type is used to identify the uci::type::DamageAssessmentRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType damageAssessmentRequestID_Type = 867;

         //  This accessor type is used to identify the uci::type::DamageAssessmentRequestMDT Accessor
         static const uci::base::accessorType::AccessorType damageAssessmentRequestMDT = 868;

         //  This accessor type is used to identify the uci::type::DamageAssessmentRequestMT Accessor
         static const uci::base::accessorType::AccessorType damageAssessmentRequestMT = 869;

         //  This accessor type is used to identify the uci::type::DamageAssessmentRequestQFT Accessor
         static const uci::base::accessorType::AccessorType damageAssessmentRequestQFT = 870;

         //  This accessor type is used to identify the uci::type::DamageAssessmentRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType damageAssessmentRequestStatusMDT = 871;

         //  This accessor type is used to identify the uci::type::DamageAssessmentRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType damageAssessmentRequestStatusMT = 872;

         //  This accessor type is used to identify the uci::type::DamageAssessmentRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType damageAssessmentRequestStatusQFT = 873;

         //  This accessor type is used to identify the uci::type::DamagedFunctionType Accessor
         static const uci::base::accessorType::AccessorType damagedFunctionType = 874;

         //  This accessor type is used to identify the uci::type::DamagedHumanType Accessor
         static const uci::base::accessorType::AccessorType damagedHumanType = 875;

         //  This accessor type is used to identify the uci::type::DamagedObjectIdentityType Accessor
         static const uci::base::accessorType::AccessorType damagedObjectIdentityType = 876;

         //  This accessor type is used to identify the uci::type::DamagedObjectNonEntityType Accessor
         static const uci::base::accessorType::AccessorType damagedObjectNonEntityType = 877;

         //  This accessor type is used to identify the uci::type::DamagedObjectType Accessor
         static const uci::base::accessorType::AccessorType damagedObjectType = 878;

         //  This accessor type is used to identify the uci::type::DamageEstimateDetailsType Accessor
         static const uci::base::accessorType::AccessorType damageEstimateDetailsType = 879;

         //  This accessor type is used to identify the uci::type::DamageEstimateID_Type Accessor
         static const uci::base::accessorType::AccessorType damageEstimateID_Type = 880;

         //  This accessor type is used to identify the uci::type::DamageEstimateMDT Accessor
         static const uci::base::accessorType::AccessorType damageEstimateMDT = 881;

         //  This accessor type is used to identify the uci::type::DamageEstimateMT Accessor
         static const uci::base::accessorType::AccessorType damageEstimateMT = 882;

         //  This accessor type is used to identify the uci::type::DamageEstimateQFT Accessor
         static const uci::base::accessorType::AccessorType damageEstimateQFT = 883;

         //  This accessor type is used to identify the uci::type::DamageEstimateRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType damageEstimateRequestID_Type = 884;

         //  This accessor type is used to identify the uci::type::DamageEstimateRequestMDT Accessor
         static const uci::base::accessorType::AccessorType damageEstimateRequestMDT = 885;

         //  This accessor type is used to identify the uci::type::DamageEstimateRequestMT Accessor
         static const uci::base::accessorType::AccessorType damageEstimateRequestMT = 886;

         //  This accessor type is used to identify the uci::type::DamageEstimateRequestQFT Accessor
         static const uci::base::accessorType::AccessorType damageEstimateRequestQFT = 887;

         //  This accessor type is used to identify the uci::type::DamageEstimateRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType damageEstimateRequestStatusMDT = 888;

         //  This accessor type is used to identify the uci::type::DamageEstimateRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType damageEstimateRequestStatusMT = 889;

         //  This accessor type is used to identify the uci::type::DamageEstimateRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType damageEstimateRequestStatusQFT = 890;

         //  This accessor type is used to identify the uci::type::DamageEstimateTargetType Accessor
         static const uci::base::accessorType::AccessorType damageEstimateTargetType = 891;

         //  This accessor type is used to identify the uci::type::DamageInformationSourceType Accessor
         static const uci::base::accessorType::AccessorType damageInformationSourceType = 892;

         //  This accessor type is used to identify the uci::type::DamageInformationWeatherSourceType Accessor
         static const uci::base::accessorType::AccessorType damageInformationWeatherSourceType = 893;

         //  This accessor type is used to identify the uci::type::DamageObjectClassType Accessor
         static const uci::base::accessorType::AccessorType damageObjectClassType = 894;

         //  This accessor type is used to identify the uci::type::DamageObservationSourceType Accessor
         static const uci::base::accessorType::AccessorType damageObservationSourceType = 895;

         //  This accessor type is used to identify the uci::type::DamageProbabilityZoneType Accessor
         static const uci::base::accessorType::AccessorType damageProbabilityZoneType = 896;

         //  This accessor type is used to identify the uci::type::DamageReportDetailType Accessor
         static const uci::base::accessorType::AccessorType damageReportDetailType = 897;

         //  This accessor type is used to identify the uci::type::DamageReportID_Type Accessor
         static const uci::base::accessorType::AccessorType damageReportID_Type = 898;

         //  This accessor type is used to identify the uci::type::DamageReportMDT Accessor
         static const uci::base::accessorType::AccessorType damageReportMDT = 899;

         //  This accessor type is used to identify the uci::type::DamageReportMT Accessor
         static const uci::base::accessorType::AccessorType damageReportMT = 900;

         //  This accessor type is used to identify the uci::type::DamageReportQFT Accessor
         static const uci::base::accessorType::AccessorType damageReportQFT = 901;

         //  This accessor type is used to identify the uci::type::DamageSubjectType Accessor
         static const uci::base::accessorType::AccessorType damageSubjectType = 902;

         //  This accessor type is used to identify the uci::type::DamageSummaryID_Type Accessor
         static const uci::base::accessorType::AccessorType damageSummaryID_Type = 903;

         //  This accessor type is used to identify the uci::type::DamageSummaryMDT Accessor
         static const uci::base::accessorType::AccessorType damageSummaryMDT = 904;

         //  This accessor type is used to identify the uci::type::DamageSummaryMT Accessor
         static const uci::base::accessorType::AccessorType damageSummaryMT = 905;

         //  This accessor type is used to identify the uci::type::DamageSummaryQFT Accessor
         static const uci::base::accessorType::AccessorType damageSummaryQFT = 906;

         //  This accessor type is used to identify the uci::type::DamageSummaryType Accessor
         static const uci::base::accessorType::AccessorType damageSummaryType = 907;

         //  This accessor type is used to identify the uci::type::DamageType Accessor
         static const uci::base::accessorType::AccessorType damageType = 908;

         //  This accessor type is used to identify the uci::type::DamageWeaponEmploymentType Accessor
         static const uci::base::accessorType::AccessorType damageWeaponEmploymentType = 909;

         //  This accessor type is used to identify the uci::type::DataDeleteChoiceType Accessor
         static const uci::base::accessorType::AccessorType dataDeleteChoiceType = 910;

         //  This accessor type is used to identify the uci::type::DataDeleteRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType dataDeleteRequestID_Type = 911;

         //  This accessor type is used to identify the uci::type::DataDeleteRequestMDT Accessor
         static const uci::base::accessorType::AccessorType dataDeleteRequestMDT = 912;

         //  This accessor type is used to identify the uci::type::DataDeleteRequestMT Accessor
         static const uci::base::accessorType::AccessorType dataDeleteRequestMT = 913;

         //  This accessor type is used to identify the uci::type::DataDeleteRequestQFT Accessor
         static const uci::base::accessorType::AccessorType dataDeleteRequestQFT = 914;

         //  This accessor type is used to identify the uci::type::DataDeleteRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType dataDeleteRequestStatusMDT = 915;

         //  This accessor type is used to identify the uci::type::DataDeleteRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType dataDeleteRequestStatusMT = 916;

         //  This accessor type is used to identify the uci::type::DataDeleteRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType dataDeleteRequestStatusQFT = 917;

         //  This accessor type is used to identify the uci::type::DataDeliveryMessageType Accessor
         static const uci::base::accessorType::AccessorType dataDeliveryMessageType = 918;

         //  This accessor type is used to identify the uci::type::DataDeliveryType Accessor
         static const uci::base::accessorType::AccessorType dataDeliveryType = 919;

         //  This accessor type is used to identify the uci::type::DataEffectivityType Accessor
         static const uci::base::accessorType::AccessorType dataEffectivityType = 920;

         //  This accessor type is used to identify the uci::type::DataLinkIdentifierPET Accessor
         static const uci::base::accessorType::AccessorType dataLinkIdentifierPET = 921;

         //  This accessor type is used to identify the uci::type::DataLinkNativeFilterPET Accessor
         static const uci::base::accessorType::AccessorType dataLinkNativeFilterPET = 922;

         //  This accessor type is used to identify the uci::type::DataLinkNativeInfoPET Accessor
         static const uci::base::accessorType::AccessorType dataLinkNativeInfoPET = 923;

         //  This accessor type is used to identify the uci::type::DataManagementCategoryType Accessor
         static const uci::base::accessorType::AccessorType dataManagementCategoryType = 924;

         //  This accessor type is used to identify the uci::type::DataManagementImportExportType Accessor
         static const uci::base::accessorType::AccessorType dataManagementImportExportType = 925;

         //  This accessor type is used to identify the uci::type::DataManagementRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType dataManagementRequestID_Type = 926;

         //  This accessor type is used to identify the uci::type::DataManagementRequestMDT Accessor
         static const uci::base::accessorType::AccessorType dataManagementRequestMDT = 927;

         //  This accessor type is used to identify the uci::type::DataManagementRequestMT Accessor
         static const uci::base::accessorType::AccessorType dataManagementRequestMT = 928;

         //  This accessor type is used to identify the uci::type::DataManagementRequestQFT Accessor
         static const uci::base::accessorType::AccessorType dataManagementRequestQFT = 929;

         //  This accessor type is used to identify the uci::type::DataManagementRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType dataManagementRequestStatusMDT = 930;

         //  This accessor type is used to identify the uci::type::DataManagementRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType dataManagementRequestStatusMT = 931;

         //  This accessor type is used to identify the uci::type::DataManagementRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType dataManagementRequestStatusQFT = 932;

         //  This accessor type is used to identify the uci::type::DataManagementSaveAsType Accessor
         static const uci::base::accessorType::AccessorType dataManagementSaveAsType = 933;

         //  This accessor type is used to identify the uci::type::DataPlanActivationCommandID_Type Accessor
         static const uci::base::accessorType::AccessorType dataPlanActivationCommandID_Type = 934;

         //  This accessor type is used to identify the uci::type::DataPlanActivationCommandMDT Accessor
         static const uci::base::accessorType::AccessorType dataPlanActivationCommandMDT = 935;

         //  This accessor type is used to identify the uci::type::DataPlanActivationCommandMT Accessor
         static const uci::base::accessorType::AccessorType dataPlanActivationCommandMT = 936;

         //  This accessor type is used to identify the uci::type::DataPlanActivationCommandQFT Accessor
         static const uci::base::accessorType::AccessorType dataPlanActivationCommandQFT = 937;

         //  This accessor type is used to identify the uci::type::DataPlanActivationCommandStatusMDT Accessor
         static const uci::base::accessorType::AccessorType dataPlanActivationCommandStatusMDT = 938;

         //  This accessor type is used to identify the uci::type::DataPlanActivationCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType dataPlanActivationCommandStatusMT = 939;

         //  This accessor type is used to identify the uci::type::DataPlanActivationCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType dataPlanActivationCommandStatusQFT = 940;

         //  This accessor type is used to identify the uci::type::DataPlanActivationStateMDT Accessor
         static const uci::base::accessorType::AccessorType dataPlanActivationStateMDT = 941;

         //  This accessor type is used to identify the uci::type::DataPlanActivationStateMT Accessor
         static const uci::base::accessorType::AccessorType dataPlanActivationStateMT = 942;

         //  This accessor type is used to identify the uci::type::DataPlanActivationStateQFT Accessor
         static const uci::base::accessorType::AccessorType dataPlanActivationStateQFT = 943;

         //  This accessor type is used to identify the uci::type::DataPlanConfigurationActivationType Accessor
         static const uci::base::accessorType::AccessorType dataPlanConfigurationActivationType = 944;

         //  This accessor type is used to identify the uci::type::DataPlanConfigurationStateType Accessor
         static const uci::base::accessorType::AccessorType dataPlanConfigurationStateType = 945;

         //  This accessor type is used to identify the uci::type::DataPlanDetailsType Accessor
         static const uci::base::accessorType::AccessorType dataPlanDetailsType = 946;

         //  This accessor type is used to identify the uci::type::DataPlanEffectivityActivationType Accessor
         static const uci::base::accessorType::AccessorType dataPlanEffectivityActivationType = 947;

         //  This accessor type is used to identify the uci::type::DataPlanEffectivityStateType Accessor
         static const uci::base::accessorType::AccessorType dataPlanEffectivityStateType = 948;

         //  This accessor type is used to identify the uci::type::DataPlanID_Type Accessor
         static const uci::base::accessorType::AccessorType dataPlanID_Type = 949;

         //  This accessor type is used to identify the uci::type::DataPlanMDT Accessor
         static const uci::base::accessorType::AccessorType dataPlanMDT = 950;

         //  This accessor type is used to identify the uci::type::DataPlanMT Accessor
         static const uci::base::accessorType::AccessorType dataPlanMT = 951;

         //  This accessor type is used to identify the uci::type::DataPlanQFT Accessor
         static const uci::base::accessorType::AccessorType dataPlanQFT = 952;

         //  This accessor type is used to identify the uci::type::DataPlanTriggerType Accessor
         static const uci::base::accessorType::AccessorType dataPlanTriggerType = 953;

         //  This accessor type is used to identify the uci::type::DataPortType Accessor
         static const uci::base::accessorType::AccessorType dataPortType = 954;

         //  This accessor type is used to identify the uci::type::DataRateDataType Accessor
         static const uci::base::accessorType::AccessorType dataRateDataType = 955;

         //  This accessor type is used to identify the uci::type::DataStorageUtilizationRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType dataStorageUtilizationRequestID_Type = 956;

         //  This accessor type is used to identify the uci::type::DataStorageUtilizationRequestMDT Accessor
         static const uci::base::accessorType::AccessorType dataStorageUtilizationRequestMDT = 957;

         //  This accessor type is used to identify the uci::type::DataStorageUtilizationRequestMT Accessor
         static const uci::base::accessorType::AccessorType dataStorageUtilizationRequestMT = 958;

         //  This accessor type is used to identify the uci::type::DataStorageUtilizationRequestQFT Accessor
         static const uci::base::accessorType::AccessorType dataStorageUtilizationRequestQFT = 959;

         //  This accessor type is used to identify the uci::type::DataStorageUtilizationRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType dataStorageUtilizationRequestStatusMDT = 960;

         //  This accessor type is used to identify the uci::type::DataStorageUtilizationRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType dataStorageUtilizationRequestStatusMT = 961;

         //  This accessor type is used to identify the uci::type::DataStorageUtilizationRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType dataStorageUtilizationRequestStatusQFT = 962;

         //  This accessor type is used to identify the uci::type::DateTimeRangeBeginType Accessor
         static const uci::base::accessorType::AccessorType dateTimeRangeBeginType = 963;

         //  This accessor type is used to identify the uci::type::DateTimeRangeType Accessor
         static const uci::base::accessorType::AccessorType dateTimeRangeType = 964;

         //  This accessor type is used to identify the uci::type::DateTimeSigmaType Accessor
         static const uci::base::accessorType::AccessorType dateTimeSigmaType = 965;

         //  This accessor type is used to identify the uci::type::DefaultResponseType Accessor
         static const uci::base::accessorType::AccessorType defaultResponseType = 966;

         //  This accessor type is used to identify the uci::type::DefensiveJammerSystemInfoType Accessor
         static const uci::base::accessorType::AccessorType defensiveJammerSystemInfoType = 967;

         //  This accessor type is used to identify the uci::type::DemodulationParameterType Accessor
         static const uci::base::accessorType::AccessorType demodulationParameterType = 968;

         //  This accessor type is used to identify the uci::type::DependentActivityType Accessor
         static const uci::base::accessorType::AccessorType dependentActivityType = 969;

         //  This accessor type is used to identify the uci::type::DependentResponseType Accessor
         static const uci::base::accessorType::AccessorType dependentResponseType = 970;

         //  This accessor type is used to identify the uci::type::DependentTaskConditionType Accessor
         static const uci::base::accessorType::AccessorType dependentTaskConditionType = 971;

         //  This accessor type is used to identify the uci::type::DependentTaskType Accessor
         static const uci::base::accessorType::AccessorType dependentTaskType = 972;

         //  This accessor type is used to identify the uci::type::DestinationID_Type Accessor
         static const uci::base::accessorType::AccessorType destinationID_Type = 973;

         //  This accessor type is used to identify the uci::type::DetailedKinematicsErrorType Accessor
         static const uci::base::accessorType::AccessorType detailedKinematicsErrorType = 974;

         //  This accessor type is used to identify the uci::type::DetailedKinematicsType Accessor
         static const uci::base::accessorType::AccessorType detailedKinematicsType = 975;

         //  This accessor type is used to identify the uci::type::DetectionEnvelopeParametersType Accessor
         static const uci::base::accessorType::AccessorType detectionEnvelopeParametersType = 976;

         //  This accessor type is used to identify the uci::type::DetectionEnvelopeType Accessor
         static const uci::base::accessorType::AccessorType detectionEnvelopeType = 977;

         //  This accessor type is used to identify the uci::type::DetectionRangeRingType Accessor
         static const uci::base::accessorType::AccessorType detectionRangeRingType = 978;

         //  This accessor type is used to identify the uci::type::DisseminationByType Accessor
         static const uci::base::accessorType::AccessorType disseminationByType = 979;

         //  This accessor type is used to identify the uci::type::DisseminationConstrainedSubplanType Accessor
         static const uci::base::accessorType::AccessorType disseminationConstrainedSubplanType = 980;

         //  This accessor type is used to identify the uci::type::DisseminationProductType Accessor
         static const uci::base::accessorType::AccessorType disseminationProductType = 981;

         //  This accessor type is used to identify the uci::type::DisseminationReportType Accessor
         static const uci::base::accessorType::AccessorType disseminationReportType = 982;

         //  This accessor type is used to identify the uci::type::DisseminationRequirementsType Accessor
         static const uci::base::accessorType::AccessorType disseminationRequirementsType = 983;

         //  This accessor type is used to identify the uci::type::DisseminationSubplanType Accessor
         static const uci::base::accessorType::AccessorType disseminationSubplanType = 984;

         //  This accessor type is used to identify the uci::type::DistanceConstraintsType Accessor
         static const uci::base::accessorType::AccessorType distanceConstraintsType = 985;

         //  This accessor type is used to identify the uci::type::DMPI_BlastPairingType Accessor
         static const uci::base::accessorType::AccessorType dMPI_BlastPairingType = 986;

         //  This accessor type is used to identify the uci::type::DMPI_CancelID_Type Accessor
         static const uci::base::accessorType::AccessorType dMPI_CancelID_Type = 987;

         //  This accessor type is used to identify the uci::type::DMPI_CancelMDT Accessor
         static const uci::base::accessorType::AccessorType dMPI_CancelMDT = 988;

         //  This accessor type is used to identify the uci::type::DMPI_CancelMT Accessor
         static const uci::base::accessorType::AccessorType dMPI_CancelMT = 989;

         //  This accessor type is used to identify the uci::type::DMPI_CancelQFT Accessor
         static const uci::base::accessorType::AccessorType dMPI_CancelQFT = 990;

         //  This accessor type is used to identify the uci::type::DMPI_CategoryType Accessor
         static const uci::base::accessorType::AccessorType dMPI_CategoryType = 991;

         //  This accessor type is used to identify the uci::type::DMPI_DamageEstimateType Accessor
         static const uci::base::accessorType::AccessorType dMPI_DamageEstimateType = 992;

         //  This accessor type is used to identify the uci::type::DMPI_DesignationID_Type Accessor
         static const uci::base::accessorType::AccessorType dMPI_DesignationID_Type = 993;

         //  This accessor type is used to identify the uci::type::DMPI_DesignationMDT Accessor
         static const uci::base::accessorType::AccessorType dMPI_DesignationMDT = 994;

         //  This accessor type is used to identify the uci::type::DMPI_DesignationMT Accessor
         static const uci::base::accessorType::AccessorType dMPI_DesignationMT = 995;

         //  This accessor type is used to identify the uci::type::DMPI_DesignationQFT Accessor
         static const uci::base::accessorType::AccessorType dMPI_DesignationQFT = 996;

         //  This accessor type is used to identify the uci::type::DMPI_DesignationRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType dMPI_DesignationRequestID_Type = 997;

         //  This accessor type is used to identify the uci::type::DMPI_DesignationRequestMDT Accessor
         static const uci::base::accessorType::AccessorType dMPI_DesignationRequestMDT = 998;

         //  This accessor type is used to identify the uci::type::DMPI_DesignationRequestMT Accessor
         static const uci::base::accessorType::AccessorType dMPI_DesignationRequestMT = 999;

         //  This accessor type is used to identify the uci::type::DMPI_DesignationRequestQFT Accessor
         static const uci::base::accessorType::AccessorType dMPI_DesignationRequestQFT = 1000;

         //  This accessor type is used to identify the uci::type::DMPI_DesignationRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType dMPI_DesignationRequestStatusMDT = 1001;

         //  This accessor type is used to identify the uci::type::DMPI_DesignationRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType dMPI_DesignationRequestStatusMT = 1002;

         //  This accessor type is used to identify the uci::type::DMPI_DesignationRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType dMPI_DesignationRequestStatusQFT = 1003;

         //  This accessor type is used to identify the uci::type::DMPI_ID_Type Accessor
         static const uci::base::accessorType::AccessorType dMPI_ID_Type = 1004;

         //  This accessor type is used to identify the uci::type::DMPI_IDsType Accessor
         static const uci::base::accessorType::AccessorType dMPI_IDsType = 1005;

         //  This accessor type is used to identify the uci::type::DMPI_LocationType Accessor
         static const uci::base::accessorType::AccessorType dMPI_LocationType = 1006;

         //  This accessor type is used to identify the uci::type::DMPI_MDT Accessor
         static const uci::base::accessorType::AccessorType dMPI_MDT = 1007;

         //  This accessor type is used to identify the uci::type::DMPI_MT Accessor
         static const uci::base::accessorType::AccessorType dMPI_MT = 1008;

         //  This accessor type is used to identify the uci::type::DMPI_PatternChoiceType Accessor
         static const uci::base::accessorType::AccessorType dMPI_PatternChoiceType = 1009;

         //  This accessor type is used to identify the uci::type::DMPI_PatternID_Type Accessor
         static const uci::base::accessorType::AccessorType dMPI_PatternID_Type = 1010;

         //  This accessor type is used to identify the uci::type::DMPI_PatternMDT Accessor
         static const uci::base::accessorType::AccessorType dMPI_PatternMDT = 1011;

         //  This accessor type is used to identify the uci::type::DMPI_PatternMT Accessor
         static const uci::base::accessorType::AccessorType dMPI_PatternMT = 1012;

         //  This accessor type is used to identify the uci::type::DMPI_PatternQFT Accessor
         static const uci::base::accessorType::AccessorType dMPI_PatternQFT = 1013;

         //  This accessor type is used to identify the uci::type::DMPI_PatternType Accessor
         static const uci::base::accessorType::AccessorType dMPI_PatternType = 1014;

         //  This accessor type is used to identify the uci::type::DMPI_QFT Accessor
         static const uci::base::accessorType::AccessorType dMPI_QFT = 1015;

         //  This accessor type is used to identify the uci::type::DMPI_RadialPatternType Accessor
         static const uci::base::accessorType::AccessorType dMPI_RadialPatternType = 1016;

         //  This accessor type is used to identify the uci::type::DMPI_StatusMDT Accessor
         static const uci::base::accessorType::AccessorType dMPI_StatusMDT = 1017;

         //  This accessor type is used to identify the uci::type::DMPI_StatusMT Accessor
         static const uci::base::accessorType::AccessorType dMPI_StatusMT = 1018;

         //  This accessor type is used to identify the uci::type::DMPI_StatusQFT Accessor
         static const uci::base::accessorType::AccessorType dMPI_StatusQFT = 1019;

         //  This accessor type is used to identify the uci::type::DMPI_StoreStatusType Accessor
         static const uci::base::accessorType::AccessorType dMPI_StoreStatusType = 1020;

         //  This accessor type is used to identify the uci::type::DMPI_TraceabilityType Accessor
         static const uci::base::accessorType::AccessorType dMPI_TraceabilityType = 1021;

         //  This accessor type is used to identify the uci::type::DMPI_ViolationChoiceType Accessor
         static const uci::base::accessorType::AccessorType dMPI_ViolationChoiceType = 1022;

         //  This accessor type is used to identify the uci::type::DMPI_ViolationType Accessor
         static const uci::base::accessorType::AccessorType dMPI_ViolationType = 1023;

         //  This accessor type is used to identify the uci::type::DMPI_XbyY_PatternType Accessor
         static const uci::base::accessorType::AccessorType dMPI_XbyY_PatternType = 1024;

         //  This accessor type is used to identify the uci::type::DoorCommandMDT Accessor
         static const uci::base::accessorType::AccessorType doorCommandMDT = 1025;

         //  This accessor type is used to identify the uci::type::DoorCommandMT Accessor
         static const uci::base::accessorType::AccessorType doorCommandMT = 1026;

         //  This accessor type is used to identify the uci::type::DoorCommandQFT Accessor
         static const uci::base::accessorType::AccessorType doorCommandQFT = 1027;

         //  This accessor type is used to identify the uci::type::DoorCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType doorCommandStatusMT = 1028;

         //  This accessor type is used to identify the uci::type::DoorCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType doorCommandStatusQFT = 1029;

         //  This accessor type is used to identify the uci::type::DoorCommandType Accessor
         static const uci::base::accessorType::AccessorType doorCommandType = 1030;

         //  This accessor type is used to identify the uci::type::DoorStatusMDT Accessor
         static const uci::base::accessorType::AccessorType doorStatusMDT = 1031;

         //  This accessor type is used to identify the uci::type::DoorStatusMT Accessor
         static const uci::base::accessorType::AccessorType doorStatusMT = 1032;

         //  This accessor type is used to identify the uci::type::DoorStatusQFT Accessor
         static const uci::base::accessorType::AccessorType doorStatusQFT = 1033;

         //  This accessor type is used to identify the uci::type::DoubleMinMaxType Accessor
         static const uci::base::accessorType::AccessorType doubleMinMaxType = 1034;

         //  This accessor type is used to identify the uci::type::DownloadConstrainedSubPlanType Accessor
         static const uci::base::accessorType::AccessorType downloadConstrainedSubPlanType = 1035;

         //  This accessor type is used to identify the uci::type::DurationRangeType Accessor
         static const uci::base::accessorType::AccessorType durationRangeType = 1036;

         //  This accessor type is used to identify the uci::type::DVR_ProcessingParametersType Accessor
         static const uci::base::accessorType::AccessorType dVR_ProcessingParametersType = 1037;

         //  This accessor type is used to identify the uci::type::DwellID_Type Accessor
         static const uci::base::accessorType::AccessorType dwellID_Type = 1038;

         //  This accessor type is used to identify the uci::type::EA_ActivityChangeBeamCommandType Accessor
         static const uci::base::accessorType::AccessorType eA_ActivityChangeBeamCommandType = 1039;

         //  This accessor type is used to identify the uci::type::EA_ActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType eA_ActivityCommandType = 1040;

         //  This accessor type is used to identify the uci::type::EA_ActivityMDT Accessor
         static const uci::base::accessorType::AccessorType eA_ActivityMDT = 1041;

         //  This accessor type is used to identify the uci::type::EA_ActivityMT Accessor
         static const uci::base::accessorType::AccessorType eA_ActivityMT = 1042;

         //  This accessor type is used to identify the uci::type::EA_ActivityQFT Accessor
         static const uci::base::accessorType::AccessorType eA_ActivityQFT = 1043;

         //  This accessor type is used to identify the uci::type::EA_ActivityType Accessor
         static const uci::base::accessorType::AccessorType eA_ActivityType = 1044;

         //  This accessor type is used to identify the uci::type::EA_BeamGroupType Accessor
         static const uci::base::accessorType::AccessorType eA_BeamGroupType = 1045;

         //  This accessor type is used to identify the uci::type::EA_CapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType eA_CapabilityCommandType = 1046;

         //  This accessor type is used to identify the uci::type::EA_CapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType eA_CapabilityMDT = 1047;

         //  This accessor type is used to identify the uci::type::EA_CapabilityMT Accessor
         static const uci::base::accessorType::AccessorType eA_CapabilityMT = 1048;

         //  This accessor type is used to identify the uci::type::EA_CapabilityOptionsType Accessor
         static const uci::base::accessorType::AccessorType eA_CapabilityOptionsType = 1049;

         //  This accessor type is used to identify the uci::type::EA_CapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType eA_CapabilityQFT = 1050;

         //  This accessor type is used to identify the uci::type::EA_CapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType eA_CapabilityStatusMDT = 1051;

         //  This accessor type is used to identify the uci::type::EA_CapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType eA_CapabilityStatusMT = 1052;

         //  This accessor type is used to identify the uci::type::EA_CapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType eA_CapabilityStatusQFT = 1053;

         //  This accessor type is used to identify the uci::type::EA_CapabilityType Accessor
         static const uci::base::accessorType::AccessorType eA_CapabilityType = 1054;

         //  This accessor type is used to identify the uci::type::EA_ChangeLocationAndPowerType Accessor
         static const uci::base::accessorType::AccessorType eA_ChangeLocationAndPowerType = 1055;

         //  This accessor type is used to identify the uci::type::EA_CommandMDT Accessor
         static const uci::base::accessorType::AccessorType eA_CommandMDT = 1056;

         //  This accessor type is used to identify the uci::type::EA_CommandMT Accessor
         static const uci::base::accessorType::AccessorType eA_CommandMT = 1057;

         //  This accessor type is used to identify the uci::type::EA_CommandQFT Accessor
         static const uci::base::accessorType::AccessorType eA_CommandQFT = 1058;

         //  This accessor type is used to identify the uci::type::EA_CommandResponseType Accessor
         static const uci::base::accessorType::AccessorType eA_CommandResponseType = 1059;

         //  This accessor type is used to identify the uci::type::EA_CommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType eA_CommandStatusMT = 1060;

         //  This accessor type is used to identify the uci::type::EA_CommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType eA_CommandStatusQFT = 1061;

         //  This accessor type is used to identify the uci::type::EA_CommandType Accessor
         static const uci::base::accessorType::AccessorType eA_CommandType = 1062;

         //  This accessor type is used to identify the uci::type::EA_ComponentType Accessor
         static const uci::base::accessorType::AccessorType eA_ComponentType = 1063;

         //  This accessor type is used to identify the uci::type::EA_ConsentRequestMDT Accessor
         static const uci::base::accessorType::AccessorType eA_ConsentRequestMDT = 1064;

         //  This accessor type is used to identify the uci::type::EA_ConsentRequestMT Accessor
         static const uci::base::accessorType::AccessorType eA_ConsentRequestMT = 1065;

         //  This accessor type is used to identify the uci::type::EA_ConsentRequestQFT Accessor
         static const uci::base::accessorType::AccessorType eA_ConsentRequestQFT = 1066;

         //  This accessor type is used to identify the uci::type::EA_ConsentRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType eA_ConsentRequestStatusMDT = 1067;

         //  This accessor type is used to identify the uci::type::EA_ConsentRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType eA_ConsentRequestStatusMT = 1068;

         //  This accessor type is used to identify the uci::type::EA_ConsentRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType eA_ConsentRequestStatusQFT = 1069;

         //  This accessor type is used to identify the uci::type::EA_ConsentRequestType Accessor
         static const uci::base::accessorType::AccessorType eA_ConsentRequestType = 1070;

         //  This accessor type is used to identify the uci::type::EA_DetailsAssignedType Accessor
         static const uci::base::accessorType::AccessorType eA_DetailsAssignedType = 1071;

         //  This accessor type is used to identify the uci::type::EA_DetailsType Accessor
         static const uci::base::accessorType::AccessorType eA_DetailsType = 1072;

         //  This accessor type is used to identify the uci::type::EA_DetailsUnassignedType Accessor
         static const uci::base::accessorType::AccessorType eA_DetailsUnassignedType = 1073;

         //  This accessor type is used to identify the uci::type::EA_EmissionType Accessor
         static const uci::base::accessorType::AccessorType eA_EmissionType = 1074;

         //  This accessor type is used to identify the uci::type::EA_EmitterDataType Accessor
         static const uci::base::accessorType::AccessorType eA_EmitterDataType = 1075;

         //  This accessor type is used to identify the uci::type::EA_EntityType Accessor
         static const uci::base::accessorType::AccessorType eA_EntityType = 1076;

         //  This accessor type is used to identify the uci::type::EA_PowerType Accessor
         static const uci::base::accessorType::AccessorType eA_PowerType = 1077;

         //  This accessor type is used to identify the uci::type::EA_ProposedType Accessor
         static const uci::base::accessorType::AccessorType eA_ProposedType = 1078;

         //  This accessor type is used to identify the uci::type::EA_ResponseType Accessor
         static const uci::base::accessorType::AccessorType eA_ResponseType = 1079;

         //  This accessor type is used to identify the uci::type::EA_SettingsCommandMDT Accessor
         static const uci::base::accessorType::AccessorType eA_SettingsCommandMDT = 1080;

         //  This accessor type is used to identify the uci::type::EA_SettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType eA_SettingsCommandMT = 1081;

         //  This accessor type is used to identify the uci::type::EA_SettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType eA_SettingsCommandQFT = 1082;

         //  This accessor type is used to identify the uci::type::EA_SettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType eA_SettingsCommandStatusMT = 1083;

         //  This accessor type is used to identify the uci::type::EA_SettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType eA_SettingsCommandStatusQFT = 1084;

         //  This accessor type is used to identify the uci::type::EA_SpecificDataType Accessor
         static const uci::base::accessorType::AccessorType eA_SpecificDataType = 1085;

         //  This accessor type is used to identify the uci::type::EA_TargetPointingType Accessor
         static const uci::base::accessorType::AccessorType eA_TargetPointingType = 1086;

         //  This accessor type is used to identify the uci::type::EA_TargetType Accessor
         static const uci::base::accessorType::AccessorType eA_TargetType = 1087;

         //  This accessor type is used to identify the uci::type::EA_TaskEscortType Accessor
         static const uci::base::accessorType::AccessorType eA_TaskEscortType = 1088;

         //  This accessor type is used to identify the uci::type::EA_TaskProtectedAssetsType Accessor
         static const uci::base::accessorType::AccessorType eA_TaskProtectedAssetsType = 1089;

         //  This accessor type is used to identify the uci::type::EA_TaskRouteRequirementsType Accessor
         static const uci::base::accessorType::AccessorType eA_TaskRouteRequirementsType = 1090;

         //  This accessor type is used to identify the uci::type::EA_TaskSuppressedEntityType Accessor
         static const uci::base::accessorType::AccessorType eA_TaskSuppressedEntityType = 1091;

         //  This accessor type is used to identify the uci::type::EA_TaskSuppressionConstraintsType Accessor
         static const uci::base::accessorType::AccessorType eA_TaskSuppressionConstraintsType = 1092;

         //  This accessor type is used to identify the uci::type::EA_TaskThreatsType Accessor
         static const uci::base::accessorType::AccessorType eA_TaskThreatsType = 1093;

         //  This accessor type is used to identify the uci::type::EA_TaskType Accessor
         static const uci::base::accessorType::AccessorType eA_TaskType = 1094;

         //  This accessor type is used to identify the uci::type::EA_TechniqueGroupIdentifierType Accessor
         static const uci::base::accessorType::AccessorType eA_TechniqueGroupIdentifierType = 1095;

      } // Namespace: accessorType
      //  
      namespace eA_TechniqueGroupIdentifierType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::EA_TechniqueGroupIdentifierType::Nickname Accessor
            static const uci::base::accessorType::AccessorType nickname = 1096;

         } // Namespace: accessorType
      } // Namespace: eA_TechniqueGroupIdentifierType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::EA_TechniqueIdentifierType Accessor
         static const uci::base::accessorType::AccessorType eA_TechniqueIdentifierType = 1097;

         //  This accessor type is used to identify the uci::type::EA_TechniqueType Accessor
         static const uci::base::accessorType::AccessorType eA_TechniqueType = 1098;

         //  This accessor type is used to identify the uci::type::ECEF_UnitVectorType Accessor
         static const uci::base::accessorType::AccessorType eCEF_UnitVectorType = 1099;

      } // Namespace: accessorType
      //  
      namespace eCEF_UnitVectorType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::ECEF_UnitVectorType::X Accessor
            static const uci::base::accessorType::AccessorType x = 1100;

            //  This accessor type is used to identify the uci::type::ECEF_UnitVectorType::Y Accessor
            static const uci::base::accessorType::AccessorType y = 1101;

            //  This accessor type is used to identify the uci::type::ECEF_UnitVectorType::Z Accessor
            static const uci::base::accessorType::AccessorType z = 1102;

         } // Namespace: accessorType
      } // Namespace: eCEF_UnitVectorType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::ECI_KinematicsType Accessor
         static const uci::base::accessorType::AccessorType eCI_KinematicsType = 1103;

         //  This accessor type is used to identify the uci::type::EEI_ID_Type Accessor
         static const uci::base::accessorType::AccessorType eEI_ID_Type = 1104;

         //  This accessor type is used to identify the uci::type::EEI_MDT Accessor
         static const uci::base::accessorType::AccessorType eEI_MDT = 1105;

         //  This accessor type is used to identify the uci::type::EEI_MT Accessor
         static const uci::base::accessorType::AccessorType eEI_MT = 1106;

         //  This accessor type is used to identify the uci::type::EEI_QFT Accessor
         static const uci::base::accessorType::AccessorType eEI_QFT = 1107;

         //  This accessor type is used to identify the uci::type::ElectronicProtectionOptionsDefaultType Accessor
         static const uci::base::accessorType::AccessorType electronicProtectionOptionsDefaultType = 1108;

         //  This accessor type is used to identify the uci::type::ElectronicProtectionOptionsEnabledType Accessor
         static const uci::base::accessorType::AccessorType electronicProtectionOptionsEnabledType = 1109;

         //  This accessor type is used to identify the uci::type::ElectronicProtectionOptionsEnableType Accessor
         static const uci::base::accessorType::AccessorType electronicProtectionOptionsEnableType = 1110;

         //  This accessor type is used to identify the uci::type::ElectronicProtectionOptionsSupportedType Accessor
         static const uci::base::accessorType::AccessorType electronicProtectionOptionsSupportedType = 1111;

         //  This accessor type is used to identify the uci::type::ElevationRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType elevationRequestID_Type = 1112;

         //  This accessor type is used to identify the uci::type::ElevationRequestMDT Accessor
         static const uci::base::accessorType::AccessorType elevationRequestMDT = 1113;

         //  This accessor type is used to identify the uci::type::ElevationRequestMT Accessor
         static const uci::base::accessorType::AccessorType elevationRequestMT = 1114;

         //  This accessor type is used to identify the uci::type::ElevationRequestPointsType Accessor
         static const uci::base::accessorType::AccessorType elevationRequestPointsType = 1115;

         //  This accessor type is used to identify the uci::type::ElevationRequestQFT Accessor
         static const uci::base::accessorType::AccessorType elevationRequestQFT = 1116;

         //  This accessor type is used to identify the uci::type::ElevationRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType elevationRequestStatusMDT = 1117;

         //  This accessor type is used to identify the uci::type::ElevationRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType elevationRequestStatusMT = 1118;

         //  This accessor type is used to identify the uci::type::ElevationRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType elevationRequestStatusQFT = 1119;

         //  This accessor type is used to identify the uci::type::ElevationVisibilityPointInfoType Accessor
         static const uci::base::accessorType::AccessorType elevationVisibilityPointInfoType = 1120;

         //  This accessor type is used to identify the uci::type::ElevationVisibilityRequestMDT Accessor
         static const uci::base::accessorType::AccessorType elevationVisibilityRequestMDT = 1121;

         //  This accessor type is used to identify the uci::type::ElevationVisibilityRequestMT Accessor
         static const uci::base::accessorType::AccessorType elevationVisibilityRequestMT = 1122;

         //  This accessor type is used to identify the uci::type::ElevationVisibilityRequestQFT Accessor
         static const uci::base::accessorType::AccessorType elevationVisibilityRequestQFT = 1123;

         //  This accessor type is used to identify the uci::type::ElevationVisibilityRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType elevationVisibilityRequestStatusMDT = 1124;

         //  This accessor type is used to identify the uci::type::ElevationVisibilityRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType elevationVisibilityRequestStatusMT = 1125;

         //  This accessor type is used to identify the uci::type::ElevationVisibilityRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType elevationVisibilityRequestStatusQFT = 1126;

         //  This accessor type is used to identify the uci::type::EllipseType Accessor
         static const uci::base::accessorType::AccessorType ellipseType = 1127;

         //  This accessor type is used to identify the uci::type::EmconConstraintType Accessor
         static const uci::base::accessorType::AccessorType emconConstraintType = 1128;

         //  This accessor type is used to identify the uci::type::EmconERP_Type Accessor
         static const uci::base::accessorType::AccessorType emconERP_Type = 1129;

         //  This accessor type is used to identify the uci::type::EmconOverrideType Accessor
         static const uci::base::accessorType::AccessorType emconOverrideType = 1130;

         //  This accessor type is used to identify the uci::type::EmconSettingType Accessor
         static const uci::base::accessorType::AccessorType emconSettingType = 1131;

         //  This accessor type is used to identify the uci::type::EmergencyReferenceOpPointCategoriesType Accessor
         static const uci::base::accessorType::AccessorType emergencyReferenceOpPointCategoriesType = 1132;

         //  This accessor type is used to identify the uci::type::EmergencyReferenceOpPointType Accessor
         static const uci::base::accessorType::AccessorType emergencyReferenceOpPointType = 1133;

         //  This accessor type is used to identify the uci::type::EmergencyReferencePointType Accessor
         static const uci::base::accessorType::AccessorType emergencyReferencePointType = 1134;

         //  This accessor type is used to identify the uci::type::EmitterEntryType Accessor
         static const uci::base::accessorType::AccessorType emitterEntryType = 1135;

         //  This accessor type is used to identify the uci::type::EmitterFunctionType Accessor
         static const uci::base::accessorType::AccessorType emitterFunctionType = 1136;

         //  This accessor type is used to identify the uci::type::EmitterIdentityConfidenceType Accessor
         static const uci::base::accessorType::AccessorType emitterIdentityConfidenceType = 1137;

         //  This accessor type is used to identify the uci::type::EmitterIdentityType Accessor
         static const uci::base::accessorType::AccessorType emitterIdentityType = 1138;

         //  This accessor type is used to identify the uci::type::EmitterMultipleType Accessor
         static const uci::base::accessorType::AccessorType emitterMultipleType = 1139;

         //  This accessor type is used to identify the uci::type::EndpointReferenceType Accessor
         static const uci::base::accessorType::AccessorType endpointReferenceType = 1140;

         //  This accessor type is used to identify the uci::type::EndPointType Accessor
         static const uci::base::accessorType::AccessorType endPointType = 1141;

         //  This accessor type is used to identify the uci::type::EnduranceBaseType Accessor
         static const uci::base::accessorType::AccessorType enduranceBaseType = 1142;

         //  This accessor type is used to identify the uci::type::EnduranceFootprintType Accessor
         static const uci::base::accessorType::AccessorType enduranceFootprintType = 1143;

         //  This accessor type is used to identify the uci::type::EnduranceRemainingType Accessor
         static const uci::base::accessorType::AccessorType enduranceRemainingType = 1144;

         //  This accessor type is used to identify the uci::type::EnduranceType Accessor
         static const uci::base::accessorType::AccessorType enduranceType = 1145;

         //  This accessor type is used to identify the uci::type::EngageTaskType Accessor
         static const uci::base::accessorType::AccessorType engageTaskType = 1146;

         //  This accessor type is used to identify the uci::type::EntityCapabilitySourceType Accessor
         static const uci::base::accessorType::AccessorType entityCapabilitySourceType = 1147;

         //  This accessor type is used to identify the uci::type::EntityCharacteristicType Accessor
         static const uci::base::accessorType::AccessorType entityCharacteristicType = 1148;

         //  This accessor type is used to identify the uci::type::EntityClauseType Accessor
         static const uci::base::accessorType::AccessorType entityClauseType = 1149;

         //  This accessor type is used to identify the uci::type::EntityComparativeType Accessor
         static const uci::base::accessorType::AccessorType entityComparativeType = 1150;

         //  This accessor type is used to identify the uci::type::EntityDataType Accessor
         static const uci::base::accessorType::AccessorType entityDataType = 1151;

         //  This accessor type is used to identify the uci::type::EntityDesignationID_Type Accessor
         static const uci::base::accessorType::AccessorType entityDesignationID_Type = 1152;

         //  This accessor type is used to identify the uci::type::EntityDesignationMDT Accessor
         static const uci::base::accessorType::AccessorType entityDesignationMDT = 1153;

         //  This accessor type is used to identify the uci::type::EntityDesignationMT Accessor
         static const uci::base::accessorType::AccessorType entityDesignationMT = 1154;

         //  This accessor type is used to identify the uci::type::EntityDesignationQFT Accessor
         static const uci::base::accessorType::AccessorType entityDesignationQFT = 1155;

         //  This accessor type is used to identify the uci::type::EntityExternalType Accessor
         static const uci::base::accessorType::AccessorType entityExternalType = 1156;

         //  This accessor type is used to identify the uci::type::EntityFusionSourceType Accessor
         static const uci::base::accessorType::AccessorType entityFusionSourceType = 1157;

         //  This accessor type is used to identify the uci::type::EntityID_Type Accessor
         static const uci::base::accessorType::AccessorType entityID_Type = 1158;

         //  This accessor type is used to identify the uci::type::EntityIdentityChoiceType Accessor
         static const uci::base::accessorType::AccessorType entityIdentityChoiceType = 1159;

         //  This accessor type is used to identify the uci::type::EntityIdentityType Accessor
         static const uci::base::accessorType::AccessorType entityIdentityType = 1160;

         //  This accessor type is used to identify the uci::type::EntityManagementClearIdentityType Accessor
         static const uci::base::accessorType::AccessorType entityManagementClearIdentityType = 1161;

         //  This accessor type is used to identify the uci::type::EntityManagementDownType Accessor
         static const uci::base::accessorType::AccessorType entityManagementDownType = 1162;

         //  This accessor type is used to identify the uci::type::EntityManagementDropProtectType Accessor
         static const uci::base::accessorType::AccessorType entityManagementDropProtectType = 1163;

         //  This accessor type is used to identify the uci::type::EntityManagementDropType Accessor
         static const uci::base::accessorType::AccessorType entityManagementDropType = 1164;

         //  This accessor type is used to identify the uci::type::EntityManagementKinematicsType Accessor
         static const uci::base::accessorType::AccessorType entityManagementKinematicsType = 1165;

         //  This accessor type is used to identify the uci::type::EntityManagementMergeType Accessor
         static const uci::base::accessorType::AccessorType entityManagementMergeType = 1166;

         //  This accessor type is used to identify the uci::type::EntityManagementProposeCorrelationType Accessor
         static const uci::base::accessorType::AccessorType entityManagementProposeCorrelationType = 1167;

         //  This accessor type is used to identify the uci::type::EntityManagementRequestEntityStatusType Accessor
         static const uci::base::accessorType::AccessorType entityManagementRequestEntityStatusType = 1168;

         //  This accessor type is used to identify the uci::type::EntityManagementRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType entityManagementRequestID_Type = 1169;

         //  This accessor type is used to identify the uci::type::EntityManagementRequestMDT Accessor
         static const uci::base::accessorType::AccessorType entityManagementRequestMDT = 1170;

         //  This accessor type is used to identify the uci::type::EntityManagementRequestMT Accessor
         static const uci::base::accessorType::AccessorType entityManagementRequestMT = 1171;

         //  This accessor type is used to identify the uci::type::EntityManagementRequestQFT Accessor
         static const uci::base::accessorType::AccessorType entityManagementRequestQFT = 1172;

         //  This accessor type is used to identify the uci::type::EntityManagementRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType entityManagementRequestStatusMDT = 1173;

         //  This accessor type is used to identify the uci::type::EntityManagementRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType entityManagementRequestStatusMT = 1174;

         //  This accessor type is used to identify the uci::type::EntityManagementRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType entityManagementRequestStatusQFT = 1175;

         //  This accessor type is used to identify the uci::type::EntityManagementRequestType Accessor
         static const uci::base::accessorType::AccessorType entityManagementRequestType = 1176;

         //  This accessor type is used to identify the uci::type::EntityManagementSetActivityByType Accessor
         static const uci::base::accessorType::AccessorType entityManagementSetActivityByType = 1177;

         //  This accessor type is used to identify the uci::type::EntityManagementSetCapabilityType Accessor
         static const uci::base::accessorType::AccessorType entityManagementSetCapabilityType = 1178;

         //  This accessor type is used to identify the uci::type::EntityManagementSetFusionEligibilityType Accessor
         static const uci::base::accessorType::AccessorType entityManagementSetFusionEligibilityType = 1179;

         //  This accessor type is used to identify the uci::type::EntityManagementSetIdentityType Accessor
         static const uci::base::accessorType::AccessorType entityManagementSetIdentityType = 1180;

         //  This accessor type is used to identify the uci::type::EntityManagementSetLink16MetadataType Accessor
         static const uci::base::accessorType::AccessorType entityManagementSetLink16MetadataType = 1181;

         //  This accessor type is used to identify the uci::type::EntityManagementSetModeType Accessor
         static const uci::base::accessorType::AccessorType entityManagementSetModeType = 1182;

         //  This accessor type is used to identify the uci::type::EntityManagementSetPlatformStatusType Accessor
         static const uci::base::accessorType::AccessorType entityManagementSetPlatformStatusType = 1183;

         //  This accessor type is used to identify the uci::type::EntityManagementSetStrengthType Accessor
         static const uci::base::accessorType::AccessorType entityManagementSetStrengthType = 1184;

         //  This accessor type is used to identify the uci::type::EntityManagementSetVoiceControlType Accessor
         static const uci::base::accessorType::AccessorType entityManagementSetVoiceControlType = 1185;

         //  This accessor type is used to identify the uci::type::EntityManagementSplitType Accessor
         static const uci::base::accessorType::AccessorType entityManagementSplitType = 1186;

         //  This accessor type is used to identify the uci::type::EntityManagementSwapType Accessor
         static const uci::base::accessorType::AccessorType entityManagementSwapType = 1187;

         //  This accessor type is used to identify the uci::type::EntityMDT Accessor
         static const uci::base::accessorType::AccessorType entityMDT = 1188;

         //  This accessor type is used to identify the uci::type::EntityMetadataMDT Accessor
         static const uci::base::accessorType::AccessorType entityMetadataMDT = 1189;

         //  This accessor type is used to identify the uci::type::EntityMetadataMT Accessor
         static const uci::base::accessorType::AccessorType entityMetadataMT = 1190;

         //  This accessor type is used to identify the uci::type::EntityMetadataPET Accessor
         static const uci::base::accessorType::AccessorType entityMetadataPET = 1191;

         //  This accessor type is used to identify the uci::type::EntityMetadataQFT Accessor
         static const uci::base::accessorType::AccessorType entityMetadataQFT = 1192;

         //  This accessor type is used to identify the uci::type::EntityMetadataType Accessor
         static const uci::base::accessorType::AccessorType entityMetadataType = 1193;

         //  This accessor type is used to identify the uci::type::EntityMT Accessor
         static const uci::base::accessorType::AccessorType entityMT = 1194;

         //  This accessor type is used to identify the uci::type::EntityNominationResultType Accessor
         static const uci::base::accessorType::AccessorType entityNominationResultType = 1195;

         //  This accessor type is used to identify the uci::type::EntityPositionType Accessor
         static const uci::base::accessorType::AccessorType entityPositionType = 1196;

         //  This accessor type is used to identify the uci::type::EntityPropagationMDT Accessor
         static const uci::base::accessorType::AccessorType entityPropagationMDT = 1197;

         //  This accessor type is used to identify the uci::type::EntityPropagationMT Accessor
         static const uci::base::accessorType::AccessorType entityPropagationMT = 1198;

         //  This accessor type is used to identify the uci::type::EntityPropagationQFT Accessor
         static const uci::base::accessorType::AccessorType entityPropagationQFT = 1199;

         //  This accessor type is used to identify the uci::type::EntityQFT Accessor
         static const uci::base::accessorType::AccessorType entityQFT = 1200;

         //  This accessor type is used to identify the uci::type::EntityRemoveInfoType Accessor
         static const uci::base::accessorType::AccessorType entityRemoveInfoType = 1201;

         //  This accessor type is used to identify the uci::type::EntitySignalSummaryType Accessor
         static const uci::base::accessorType::AccessorType entitySignalSummaryType = 1202;

         //  This accessor type is used to identify the uci::type::EntitySourceIdentifierType Accessor
         static const uci::base::accessorType::AccessorType entitySourceIdentifierType = 1203;

         //  This accessor type is used to identify the uci::type::EntitySourceSpecificDataType Accessor
         static const uci::base::accessorType::AccessorType entitySourceSpecificDataType = 1204;

      } // Namespace: accessorType
      //  
      namespace entitySourceSpecificDataType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::EntitySourceSpecificDataType::TrackQuality Accessor
            static const uci::base::accessorType::AccessorType trackQuality = 1205;

         } // Namespace: accessorType
      } // Namespace: entitySourceSpecificDataType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::EntitySourceType Accessor
         static const uci::base::accessorType::AccessorType entitySourceType = 1206;

         //  This accessor type is used to identify the uci::type::EntityStalenessThresholdsType Accessor
         static const uci::base::accessorType::AccessorType entityStalenessThresholdsType = 1207;

         //  This accessor type is used to identify the uci::type::EnvironmentIdentityConfidenceType Accessor
         static const uci::base::accessorType::AccessorType environmentIdentityConfidenceType = 1208;

         //  This accessor type is used to identify the uci::type::EOB_CodesType Accessor
         static const uci::base::accessorType::AccessorType eOB_CodesType = 1209;

         //  This accessor type is used to identify the uci::type::EOB_CorrelatedToType Accessor
         static const uci::base::accessorType::AccessorType eOB_CorrelatedToType = 1210;

         //  This accessor type is used to identify the uci::type::EOB_CorrelationInputType Accessor
         static const uci::base::accessorType::AccessorType eOB_CorrelationInputType = 1211;

         //  This accessor type is used to identify the uci::type::EOB_CorrelationRecordID_Type Accessor
         static const uci::base::accessorType::AccessorType eOB_CorrelationRecordID_Type = 1212;

         //  This accessor type is used to identify the uci::type::EOB_CorrelationRecordMDT Accessor
         static const uci::base::accessorType::AccessorType eOB_CorrelationRecordMDT = 1213;

         //  This accessor type is used to identify the uci::type::EOB_CorrelationRecordMT Accessor
         static const uci::base::accessorType::AccessorType eOB_CorrelationRecordMT = 1214;

         //  This accessor type is used to identify the uci::type::EOB_CorrelationRecordQFT Accessor
         static const uci::base::accessorType::AccessorType eOB_CorrelationRecordQFT = 1215;

         //  This accessor type is used to identify the uci::type::EOB_CorrelationRecordType Accessor
         static const uci::base::accessorType::AccessorType eOB_CorrelationRecordType = 1216;

         //  This accessor type is used to identify the uci::type::EOB_EmitterID_Type Accessor
         static const uci::base::accessorType::AccessorType eOB_EmitterID_Type = 1217;

         //  This accessor type is used to identify the uci::type::EOB_EmitterIdentityType Accessor
         static const uci::base::accessorType::AccessorType eOB_EmitterIdentityType = 1218;

         //  This accessor type is used to identify the uci::type::EOB_EmitterModeID_Type Accessor
         static const uci::base::accessorType::AccessorType eOB_EmitterModeID_Type = 1219;

         //  This accessor type is used to identify the uci::type::EOB_EmitterModeType Accessor
         static const uci::base::accessorType::AccessorType eOB_EmitterModeType = 1220;

         //  This accessor type is used to identify the uci::type::EOB_EmitterType Accessor
         static const uci::base::accessorType::AccessorType eOB_EmitterType = 1221;

         //  This accessor type is used to identify the uci::type::EOB_EquipmentIdentityType Accessor
         static const uci::base::accessorType::AccessorType eOB_EquipmentIdentityType = 1222;

         //  This accessor type is used to identify the uci::type::EOB_EquipmentRecordType Accessor
         static const uci::base::accessorType::AccessorType eOB_EquipmentRecordType = 1223;

         //  This accessor type is used to identify the uci::type::EOB_EquipmentType Accessor
         static const uci::base::accessorType::AccessorType eOB_EquipmentType = 1224;

         //  This accessor type is used to identify the uci::type::EOB_FixedPositionType Accessor
         static const uci::base::accessorType::AccessorType eOB_FixedPositionType = 1225;

      } // Namespace: accessorType
      //  
      namespace eOB_FixedPositionType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::EOB_FixedPositionType::MilitaryGrid Accessor
            static const uci::base::accessorType::AccessorType militaryGrid = 1226;

         } // Namespace: accessorType
      } // Namespace: eOB_FixedPositionType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::EOB_IdentityBaseType Accessor
         static const uci::base::accessorType::AccessorType eOB_IdentityBaseType = 1227;

         //  This accessor type is used to identify the uci::type::EOB_IdentityConfidenceType Accessor
         static const uci::base::accessorType::AccessorType eOB_IdentityConfidenceType = 1228;

         //  This accessor type is used to identify the uci::type::EOB_IdentityType Accessor
         static const uci::base::accessorType::AccessorType eOB_IdentityType = 1229;

         //  This accessor type is used to identify the uci::type::EOB_RecordID_Type Accessor
         static const uci::base::accessorType::AccessorType eOB_RecordID_Type = 1230;

         //  This accessor type is used to identify the uci::type::EOB_RecordMarkedType Accessor
         static const uci::base::accessorType::AccessorType eOB_RecordMarkedType = 1231;

         //  This accessor type is used to identify the uci::type::EOB_SignalType Accessor
         static const uci::base::accessorType::AccessorType eOB_SignalType = 1232;

         //  This accessor type is used to identify the uci::type::EOB_SiteIdentityType Accessor
         static const uci::base::accessorType::AccessorType eOB_SiteIdentityType = 1233;

      } // Namespace: accessorType
      //  
      namespace eOB_SiteIdentityType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::EOB_SiteIdentityType::Category Accessor
            static const uci::base::accessorType::AccessorType category = 1234;

         } // Namespace: accessorType
      } // Namespace: eOB_SiteIdentityType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::EOB_SitePIN_Type Accessor
         static const uci::base::accessorType::AccessorType eOB_SitePIN_Type = 1235;

         //  This accessor type is used to identify the uci::type::ESM_AcquisitionTargetType Accessor
         static const uci::base::accessorType::AccessorType eSM_AcquisitionTargetType = 1236;

         //  This accessor type is used to identify the uci::type::ESM_ActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType eSM_ActivityCommandType = 1237;

         //  This accessor type is used to identify the uci::type::ESM_ActivityMDT Accessor
         static const uci::base::accessorType::AccessorType eSM_ActivityMDT = 1238;

         //  This accessor type is used to identify the uci::type::ESM_ActivityMT Accessor
         static const uci::base::accessorType::AccessorType eSM_ActivityMT = 1239;

         //  This accessor type is used to identify the uci::type::ESM_ActivityQFT Accessor
         static const uci::base::accessorType::AccessorType eSM_ActivityQFT = 1240;

         //  This accessor type is used to identify the uci::type::ESM_ActivityType Accessor
         static const uci::base::accessorType::AccessorType eSM_ActivityType = 1241;

         //  This accessor type is used to identify the uci::type::ESM_CapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType eSM_CapabilityCommandType = 1242;

         //  This accessor type is used to identify the uci::type::ESM_CapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType eSM_CapabilityMDT = 1243;

         //  This accessor type is used to identify the uci::type::ESM_CapabilityMT Accessor
         static const uci::base::accessorType::AccessorType eSM_CapabilityMT = 1244;

         //  This accessor type is used to identify the uci::type::ESM_CapabilityOptionsType Accessor
         static const uci::base::accessorType::AccessorType eSM_CapabilityOptionsType = 1245;

         //  This accessor type is used to identify the uci::type::ESM_CapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType eSM_CapabilityQFT = 1246;

         //  This accessor type is used to identify the uci::type::ESM_CapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType eSM_CapabilityStatusMDT = 1247;

         //  This accessor type is used to identify the uci::type::ESM_CapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType eSM_CapabilityStatusMT = 1248;

         //  This accessor type is used to identify the uci::type::ESM_CapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType eSM_CapabilityStatusQFT = 1249;

         //  This accessor type is used to identify the uci::type::ESM_CapabilityType Accessor
         static const uci::base::accessorType::AccessorType eSM_CapabilityType = 1250;

         //  This accessor type is used to identify the uci::type::ESM_ChangeDwellType Accessor
         static const uci::base::accessorType::AccessorType eSM_ChangeDwellType = 1251;

         //  This accessor type is used to identify the uci::type::ESM_ChangeGeolocationType Accessor
         static const uci::base::accessorType::AccessorType eSM_ChangeGeolocationType = 1252;

         //  This accessor type is used to identify the uci::type::ESM_CommandMDT Accessor
         static const uci::base::accessorType::AccessorType eSM_CommandMDT = 1253;

         //  This accessor type is used to identify the uci::type::ESM_CommandMT Accessor
         static const uci::base::accessorType::AccessorType eSM_CommandMT = 1254;

         //  This accessor type is used to identify the uci::type::ESM_CommandQFT Accessor
         static const uci::base::accessorType::AccessorType eSM_CommandQFT = 1255;

         //  This accessor type is used to identify the uci::type::ESM_CommandResponseType Accessor
         static const uci::base::accessorType::AccessorType eSM_CommandResponseType = 1256;

         //  This accessor type is used to identify the uci::type::ESM_CommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType eSM_CommandStatusMT = 1257;

         //  This accessor type is used to identify the uci::type::ESM_CommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType eSM_CommandStatusQFT = 1258;

         //  This accessor type is used to identify the uci::type::ESM_CommandType Accessor
         static const uci::base::accessorType::AccessorType eSM_CommandType = 1259;

         //  This accessor type is used to identify the uci::type::ESM_ConsentRequestMDT Accessor
         static const uci::base::accessorType::AccessorType eSM_ConsentRequestMDT = 1260;

         //  This accessor type is used to identify the uci::type::ESM_ConsentRequestMT Accessor
         static const uci::base::accessorType::AccessorType eSM_ConsentRequestMT = 1261;

         //  This accessor type is used to identify the uci::type::ESM_ConsentRequestQFT Accessor
         static const uci::base::accessorType::AccessorType eSM_ConsentRequestQFT = 1262;

         //  This accessor type is used to identify the uci::type::ESM_ConsentRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType eSM_ConsentRequestStatusMDT = 1263;

         //  This accessor type is used to identify the uci::type::ESM_ConsentRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType eSM_ConsentRequestStatusMT = 1264;

         //  This accessor type is used to identify the uci::type::ESM_ConsentRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType eSM_ConsentRequestStatusQFT = 1265;

         //  This accessor type is used to identify the uci::type::ESM_ConsentRequestType Accessor
         static const uci::base::accessorType::AccessorType eSM_ConsentRequestType = 1266;

         //  This accessor type is used to identify the uci::type::ESM_DwellCoverageType Accessor
         static const uci::base::accessorType::AccessorType eSM_DwellCoverageType = 1267;

         //  This accessor type is used to identify the uci::type::ESM_LocationType Accessor
         static const uci::base::accessorType::AccessorType eSM_LocationType = 1268;

         //  This accessor type is used to identify the uci::type::ESM_PerformanceProfileType Accessor
         static const uci::base::accessorType::AccessorType eSM_PerformanceProfileType = 1269;

         //  This accessor type is used to identify the uci::type::ESM_ProfileEditType Accessor
         static const uci::base::accessorType::AccessorType eSM_ProfileEditType = 1270;

         //  This accessor type is used to identify the uci::type::ESM_ProfileEntryType Accessor
         static const uci::base::accessorType::AccessorType eSM_ProfileEntryType = 1271;

         //  This accessor type is used to identify the uci::type::ESM_ReceiveDwellType Accessor
         static const uci::base::accessorType::AccessorType eSM_ReceiveDwellType = 1272;

         //  This accessor type is used to identify the uci::type::ESM_SettingsCommandMDT Accessor
         static const uci::base::accessorType::AccessorType eSM_SettingsCommandMDT = 1273;

         //  This accessor type is used to identify the uci::type::ESM_SettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType eSM_SettingsCommandMT = 1274;

         //  This accessor type is used to identify the uci::type::ESM_SettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType eSM_SettingsCommandQFT = 1275;

         //  This accessor type is used to identify the uci::type::ESM_SettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType eSM_SettingsCommandStatusMT = 1276;

         //  This accessor type is used to identify the uci::type::ESM_SettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType eSM_SettingsCommandStatusQFT = 1277;

         //  This accessor type is used to identify the uci::type::ESM_SpecificDataType Accessor
         static const uci::base::accessorType::AccessorType eSM_SpecificDataType = 1278;

         //  This accessor type is used to identify the uci::type::ESM_SubcapabilityAcquisitionType Accessor
         static const uci::base::accessorType::AccessorType eSM_SubcapabilityAcquisitionType = 1279;

         //  This accessor type is used to identify the uci::type::ESM_SubcapabilityChoiceType Accessor
         static const uci::base::accessorType::AccessorType eSM_SubcapabilityChoiceType = 1280;

         //  This accessor type is used to identify the uci::type::ESM_SubcapabilityGeolocationType Accessor
         static const uci::base::accessorType::AccessorType eSM_SubcapabilityGeolocationType = 1281;

         //  This accessor type is used to identify the uci::type::ESM_SubcapabilityIdentificationType Accessor
         static const uci::base::accessorType::AccessorType eSM_SubcapabilityIdentificationType = 1282;

         //  This accessor type is used to identify the uci::type::ESM_SubcapabilityMeasurementType Accessor
         static const uci::base::accessorType::AccessorType eSM_SubcapabilityMeasurementType = 1283;

         //  This accessor type is used to identify the uci::type::ESM_SubcapabilityPulseDataCollectType Accessor
         static const uci::base::accessorType::AccessorType eSM_SubcapabilityPulseDataCollectType = 1284;

         //  This accessor type is used to identify the uci::type::ESM_SubcapabilityTargetLocationDataType Accessor
         static const uci::base::accessorType::AccessorType eSM_SubcapabilityTargetLocationDataType = 1285;

         //  This accessor type is used to identify the uci::type::ESM_TargetType Accessor
         static const uci::base::accessorType::AccessorType eSM_TargetType = 1286;

         //  This accessor type is used to identify the uci::type::ESM_TaskBaseType Accessor
         static const uci::base::accessorType::AccessorType eSM_TaskBaseType = 1287;

         //  This accessor type is used to identify the uci::type::ESM_TaskType Accessor
         static const uci::base::accessorType::AccessorType eSM_TaskType = 1288;

         //  This accessor type is used to identify the uci::type::EstimatedPerformanceType Accessor
         static const uci::base::accessorType::AccessorType estimatedPerformanceType = 1289;

         //  This accessor type is used to identify the uci::type::EstimationEndType Accessor
         static const uci::base::accessorType::AccessorType estimationEndType = 1290;

         //  This accessor type is used to identify the uci::type::EstimationResultType Accessor
         static const uci::base::accessorType::AccessorType estimationResultType = 1291;

         //  This accessor type is used to identify the uci::type::EstimationStartingSegmentID_Type Accessor
         static const uci::base::accessorType::AccessorType estimationStartingSegmentID_Type = 1292;

         //  This accessor type is used to identify the uci::type::EstimationStartType Accessor
         static const uci::base::accessorType::AccessorType estimationStartType = 1293;

         //  This accessor type is used to identify the uci::type::EthernetSettingsType Accessor
         static const uci::base::accessorType::AccessorType ethernetSettingsType = 1294;

         //  This accessor type is used to identify the uci::type::EW_ConfigurationMDT Accessor
         static const uci::base::accessorType::AccessorType eW_ConfigurationMDT = 1295;

         //  This accessor type is used to identify the uci::type::EW_ConfigurationMT Accessor
         static const uci::base::accessorType::AccessorType eW_ConfigurationMT = 1296;

         //  This accessor type is used to identify the uci::type::EW_ConfigurationQFT Accessor
         static const uci::base::accessorType::AccessorType eW_ConfigurationQFT = 1297;

         //  This accessor type is used to identify the uci::type::ExerciseIdentityType Accessor
         static const uci::base::accessorType::AccessorType exerciseIdentityType = 1298;

         //  This accessor type is used to identify the uci::type::ExpendableType Accessor
         static const uci::base::accessorType::AccessorType expendableType = 1299;

         //  This accessor type is used to identify the uci::type::EXPLTB_Type Accessor
         static const uci::base::accessorType::AccessorType eXPLTB_Type = 1300;

      } // Namespace: accessorType
      //  
      namespace eXPLTB_Type {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::EXPLTB_Type::SEQ_NUM Accessor
            static const uci::base::accessorType::AccessorType sEQ_NUM = 1301;

            //  This accessor type is used to identify the uci::type::EXPLTB_Type::PRIME_BE Accessor
            static const uci::base::accessorType::AccessorType pRIME_BE = 1302;

            //  This accessor type is used to identify the uci::type::EXPLTB_Type::PRIME_ID Accessor
            static const uci::base::accessorType::AccessorType pRIME_ID = 1303;

         } // Namespace: accessorType
      } // Namespace: eXPLTB_Type
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::ExternalDataRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType externalDataRequestID_Type = 1304;

         //  This accessor type is used to identify the uci::type::ExternalDataRequestMDT Accessor
         static const uci::base::accessorType::AccessorType externalDataRequestMDT = 1305;

         //  This accessor type is used to identify the uci::type::ExternalDataRequestMT Accessor
         static const uci::base::accessorType::AccessorType externalDataRequestMT = 1306;

         //  This accessor type is used to identify the uci::type::ExternalDataRequestQFT Accessor
         static const uci::base::accessorType::AccessorType externalDataRequestQFT = 1307;

         //  This accessor type is used to identify the uci::type::ExternalDataRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType externalDataRequestStatusMDT = 1308;

         //  This accessor type is used to identify the uci::type::ExternalDataRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType externalDataRequestStatusMT = 1309;

         //  This accessor type is used to identify the uci::type::ExternalDataRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType externalDataRequestStatusQFT = 1310;

         //  This accessor type is used to identify the uci::type::ExtrapolatedEntityType Accessor
         static const uci::base::accessorType::AccessorType extrapolatedEntityType = 1311;

         //  This accessor type is used to identify the uci::type::FacilityIdentificationType Accessor
         static const uci::base::accessorType::AccessorType facilityIdentificationType = 1312;

         //  This accessor type is used to identify the uci::type::FaultDataType Accessor
         static const uci::base::accessorType::AccessorType faultDataType = 1313;

         //  This accessor type is used to identify the uci::type::FaultID_Type Accessor
         static const uci::base::accessorType::AccessorType faultID_Type = 1314;

         //  This accessor type is used to identify the uci::type::FaultMDT Accessor
         static const uci::base::accessorType::AccessorType faultMDT = 1315;

         //  This accessor type is used to identify the uci::type::FaultMT Accessor
         static const uci::base::accessorType::AccessorType faultMT = 1316;

         //  This accessor type is used to identify the uci::type::FaultQFT Accessor
         static const uci::base::accessorType::AccessorType faultQFT = 1317;

         //  This accessor type is used to identify the uci::type::FieldOfRegardType Accessor
         static const uci::base::accessorType::AccessorType fieldOfRegardType = 1318;

         //  This accessor type is used to identify the uci::type::FileFiltersType Accessor
         static const uci::base::accessorType::AccessorType fileFiltersType = 1319;

         //  This accessor type is used to identify the uci::type::FileFormatType Accessor
         static const uci::base::accessorType::AccessorType fileFormatType = 1320;

         //  This accessor type is used to identify the uci::type::FileHeaderType Accessor
         static const uci::base::accessorType::AccessorType fileHeaderType = 1321;

      } // Namespace: accessorType
      //  
      namespace fileHeaderType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::FileHeaderType::OriginatingStationID Accessor
            static const uci::base::accessorType::AccessorType originatingStationID = 1322;

            //  This accessor type is used to identify the uci::type::FileHeaderType::FileTitle Accessor
            static const uci::base::accessorType::AccessorType fileTitle = 1323;

            //  This accessor type is used to identify the uci::type::FileHeaderType::OriginatorsName Accessor
            static const uci::base::accessorType::AccessorType originatorsName = 1324;

            //  This accessor type is used to identify the uci::type::FileHeaderType::OriginatorsPhone Accessor
            static const uci::base::accessorType::AccessorType originatorsPhone = 1325;

            //  This accessor type is used to identify the uci::type::FileHeaderType::FileClassificationSystem Accessor
            static const uci::base::accessorType::AccessorType fileClassificationSystem = 1326;

            //  This accessor type is used to identify the uci::type::FileHeaderType::FileSecurityControlNumber Accessor
            static const uci::base::accessorType::AccessorType fileSecurityControlNumber = 1327;

            //  This accessor type is used to identify the uci::type::FileHeaderType::FileControlAndHandling Accessor
            static const uci::base::accessorType::AccessorType fileControlAndHandling = 1328;

            //  This accessor type is used to identify the uci::type::FileHeaderType::FileClassificationAuthorityType Accessor
            static const uci::base::accessorType::AccessorType fileClassificationAuthorityType = 1329;

            //  This accessor type is used to identify the uci::type::FileHeaderType::FileDowngrade Accessor
            static const uci::base::accessorType::AccessorType fileDowngrade = 1330;

            //  This accessor type is used to identify the uci::type::FileHeaderType::FileSecuritySourceDate Accessor
            static const uci::base::accessorType::AccessorType fileSecuritySourceDate = 1331;

         } // Namespace: accessorType
      } // Namespace: fileHeaderType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::FileLocationID_Type Accessor
         static const uci::base::accessorType::AccessorType fileLocationID_Type = 1332;

         //  This accessor type is used to identify the uci::type::FileLocationMDT Accessor
         static const uci::base::accessorType::AccessorType fileLocationMDT = 1333;

         //  This accessor type is used to identify the uci::type::FileLocationMT Accessor
         static const uci::base::accessorType::AccessorType fileLocationMT = 1334;

         //  This accessor type is used to identify the uci::type::FileLocationQFT Accessor
         static const uci::base::accessorType::AccessorType fileLocationQFT = 1335;

         //  This accessor type is used to identify the uci::type::FileMetadataID_Type Accessor
         static const uci::base::accessorType::AccessorType fileMetadataID_Type = 1336;

         //  This accessor type is used to identify the uci::type::FileMetadataMDT Accessor
         static const uci::base::accessorType::AccessorType fileMetadataMDT = 1337;

         //  This accessor type is used to identify the uci::type::FileMetadataMT Accessor
         static const uci::base::accessorType::AccessorType fileMetadataMT = 1338;

         //  This accessor type is used to identify the uci::type::FileMetadataQFT Accessor
         static const uci::base::accessorType::AccessorType fileMetadataQFT = 1339;

         //  This accessor type is used to identify the uci::type::FileOutputType Accessor
         static const uci::base::accessorType::AccessorType fileOutputType = 1340;

         //  This accessor type is used to identify the uci::type::FixedPositionType Accessor
         static const uci::base::accessorType::AccessorType fixedPositionType = 1341;

         //  This accessor type is used to identify the uci::type::FlightTaskType Accessor
         static const uci::base::accessorType::AccessorType flightTaskType = 1342;

         //  This accessor type is used to identify the uci::type::FloatMinMaxType Accessor
         static const uci::base::accessorType::AccessorType floatMinMaxType = 1343;

         //  This accessor type is used to identify the uci::type::FOR_Type Accessor
         static const uci::base::accessorType::AccessorType fOR_Type = 1344;

         //  This accessor type is used to identify the uci::type::ForeignKeyMapID_Type Accessor
         static const uci::base::accessorType::AccessorType foreignKeyMapID_Type = 1345;

         //  This accessor type is used to identify the uci::type::ForeignKeyMapMDT Accessor
         static const uci::base::accessorType::AccessorType foreignKeyMapMDT = 1346;

         //  This accessor type is used to identify the uci::type::ForeignKeyMapMT Accessor
         static const uci::base::accessorType::AccessorType foreignKeyMapMT = 1347;

         //  This accessor type is used to identify the uci::type::ForeignKeyMappingType Accessor
         static const uci::base::accessorType::AccessorType foreignKeyMappingType = 1348;

         //  This accessor type is used to identify the uci::type::ForeignKeyMapQFT Accessor
         static const uci::base::accessorType::AccessorType foreignKeyMapQFT = 1349;

         //  This accessor type is used to identify the uci::type::ForeignKeyType Accessor
         static const uci::base::accessorType::AccessorType foreignKeyType = 1350;

         //  This accessor type is used to identify the uci::type::FOV_ActivityType Accessor
         static const uci::base::accessorType::AccessorType fOV_ActivityType = 1351;

         //  This accessor type is used to identify the uci::type::FOV_Type Accessor
         static const uci::base::accessorType::AccessorType fOV_Type = 1352;

         //  This accessor type is used to identify the uci::type::FrequencyDataBaseType Accessor
         static const uci::base::accessorType::AccessorType frequencyDataBaseType = 1353;

         //  This accessor type is used to identify the uci::type::FrequencyDataType Accessor
         static const uci::base::accessorType::AccessorType frequencyDataType = 1354;

         //  This accessor type is used to identify the uci::type::FrequencyMultiChannelType Accessor
         static const uci::base::accessorType::AccessorType frequencyMultiChannelType = 1355;

         //  This accessor type is used to identify the uci::type::FrequencyRangeType Accessor
         static const uci::base::accessorType::AccessorType frequencyRangeType = 1356;

         //  This accessor type is used to identify the uci::type::FrequencySetType Accessor
         static const uci::base::accessorType::AccessorType frequencySetType = 1357;

         //  This accessor type is used to identify the uci::type::FunctionalDamageConfidenceType Accessor
         static const uci::base::accessorType::AccessorType functionalDamageConfidenceType = 1358;

         //  This accessor type is used to identify the uci::type::FunctionalDamageProbabilityType Accessor
         static const uci::base::accessorType::AccessorType functionalDamageProbabilityType = 1359;

         //  This accessor type is used to identify the uci::type::FusionSettingsMDT Accessor
         static const uci::base::accessorType::AccessorType fusionSettingsMDT = 1360;

         //  This accessor type is used to identify the uci::type::FusionSettingsMT Accessor
         static const uci::base::accessorType::AccessorType fusionSettingsMT = 1361;

         //  This accessor type is used to identify the uci::type::FusionSettingsQFT Accessor
         static const uci::base::accessorType::AccessorType fusionSettingsQFT = 1362;

         //  This accessor type is used to identify the uci::type::FusionSettingsRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType fusionSettingsRequestID_Type = 1363;

         //  This accessor type is used to identify the uci::type::FusionSettingsRequestMDT Accessor
         static const uci::base::accessorType::AccessorType fusionSettingsRequestMDT = 1364;

         //  This accessor type is used to identify the uci::type::FusionSettingsRequestMT Accessor
         static const uci::base::accessorType::AccessorType fusionSettingsRequestMT = 1365;

         //  This accessor type is used to identify the uci::type::FusionSettingsRequestQFT Accessor
         static const uci::base::accessorType::AccessorType fusionSettingsRequestQFT = 1366;

         //  This accessor type is used to identify the uci::type::FusionSettingsRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType fusionSettingsRequestStatusMDT = 1367;

         //  This accessor type is used to identify the uci::type::FusionSettingsRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType fusionSettingsRequestStatusMT = 1368;

         //  This accessor type is used to identify the uci::type::FusionSettingsRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType fusionSettingsRequestStatusQFT = 1369;

         //  This accessor type is used to identify the uci::type::FusionSettingsRequestType Accessor
         static const uci::base::accessorType::AccessorType fusionSettingsRequestType = 1370;

         //  This accessor type is used to identify the uci::type::FusionSettingsType Accessor
         static const uci::base::accessorType::AccessorType fusionSettingsType = 1371;

         //  This accessor type is used to identify the uci::type::FusionSourcesType Accessor
         static const uci::base::accessorType::AccessorType fusionSourcesType = 1372;

         //  This accessor type is used to identify the uci::type::FusionSourceType Accessor
         static const uci::base::accessorType::AccessorType fusionSourceType = 1373;

         //  This accessor type is used to identify the uci::type::FuzeTriggerType Accessor
         static const uci::base::accessorType::AccessorType fuzeTriggerType = 1374;

         //  This accessor type is used to identify the uci::type::FuzeType Accessor
         static const uci::base::accessorType::AccessorType fuzeType = 1375;

         //  This accessor type is used to identify the uci::type::GainRangeType Accessor
         static const uci::base::accessorType::AccessorType gainRangeType = 1376;

         //  This accessor type is used to identify the uci::type::GatewayActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType gatewayActivityCommandType = 1377;

         //  This accessor type is used to identify the uci::type::GatewayActivityMDT Accessor
         static const uci::base::accessorType::AccessorType gatewayActivityMDT = 1378;

         //  This accessor type is used to identify the uci::type::GatewayActivityMT Accessor
         static const uci::base::accessorType::AccessorType gatewayActivityMT = 1379;

         //  This accessor type is used to identify the uci::type::GatewayActivityQFT Accessor
         static const uci::base::accessorType::AccessorType gatewayActivityQFT = 1380;

         //  This accessor type is used to identify the uci::type::GatewayActivityType Accessor
         static const uci::base::accessorType::AccessorType gatewayActivityType = 1381;

         //  This accessor type is used to identify the uci::type::GatewayCapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType gatewayCapabilityCommandType = 1382;

         //  This accessor type is used to identify the uci::type::GatewayCapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType gatewayCapabilityMDT = 1383;

         //  This accessor type is used to identify the uci::type::GatewayCapabilityMT Accessor
         static const uci::base::accessorType::AccessorType gatewayCapabilityMT = 1384;

         //  This accessor type is used to identify the uci::type::GatewayCapabilityOptionsType Accessor
         static const uci::base::accessorType::AccessorType gatewayCapabilityOptionsType = 1385;

         //  This accessor type is used to identify the uci::type::GatewayCapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType gatewayCapabilityQFT = 1386;

         //  This accessor type is used to identify the uci::type::GatewayCapabilitySettingsCommandBaseType Accessor
         static const uci::base::accessorType::AccessorType gatewayCapabilitySettingsCommandBaseType = 1387;

         //  This accessor type is used to identify the uci::type::GatewayCapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType gatewayCapabilityStatusMDT = 1388;

         //  This accessor type is used to identify the uci::type::GatewayCapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType gatewayCapabilityStatusMT = 1389;

         //  This accessor type is used to identify the uci::type::GatewayCapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType gatewayCapabilityStatusQFT = 1390;

         //  This accessor type is used to identify the uci::type::GatewayCapabilityStatusType Accessor
         static const uci::base::accessorType::AccessorType gatewayCapabilityStatusType = 1391;

         //  This accessor type is used to identify the uci::type::GatewayCapabilityType Accessor
         static const uci::base::accessorType::AccessorType gatewayCapabilityType = 1392;

         //  This accessor type is used to identify the uci::type::GatewayCommandMDT Accessor
         static const uci::base::accessorType::AccessorType gatewayCommandMDT = 1393;

         //  This accessor type is used to identify the uci::type::GatewayCommandMT Accessor
         static const uci::base::accessorType::AccessorType gatewayCommandMT = 1394;

         //  This accessor type is used to identify the uci::type::GatewayCommandQFT Accessor
         static const uci::base::accessorType::AccessorType gatewayCommandQFT = 1395;

         //  This accessor type is used to identify the uci::type::GatewayCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType gatewayCommandStatusMT = 1396;

         //  This accessor type is used to identify the uci::type::GatewayCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType gatewayCommandStatusQFT = 1397;

         //  This accessor type is used to identify the uci::type::GatewayCommandType Accessor
         static const uci::base::accessorType::AccessorType gatewayCommandType = 1398;

         //  This accessor type is used to identify the uci::type::GatewayConfigurationPET Accessor
         static const uci::base::accessorType::AccessorType gatewayConfigurationPET = 1399;

         //  This accessor type is used to identify the uci::type::GatewayConfigurationType Accessor
         static const uci::base::accessorType::AccessorType gatewayConfigurationType = 1400;

         //  This accessor type is used to identify the uci::type::GatewayControlType Accessor
         static const uci::base::accessorType::AccessorType gatewayControlType = 1401;

         //  This accessor type is used to identify the uci::type::GatewayLink16ConfigurationAnnulusType Accessor
         static const uci::base::accessorType::AccessorType gatewayLink16ConfigurationAnnulusType = 1402;

         //  This accessor type is used to identify the uci::type::GatewayLink16ConfigurationAreaType Accessor
         static const uci::base::accessorType::AccessorType gatewayLink16ConfigurationAreaType = 1403;

         //  This accessor type is used to identify the uci::type::GatewayLink16ConfigurationCircleType Accessor
         static const uci::base::accessorType::AccessorType gatewayLink16ConfigurationCircleType = 1404;

         //  This accessor type is used to identify the uci::type::GatewayLink16ConfigurationCircularSectorType Accessor
         static const uci::base::accessorType::AccessorType gatewayLink16ConfigurationCircularSectorType = 1405;

         //  This accessor type is used to identify the uci::type::GatewayLink16ConfigurationClosedAreaType Accessor
         static const uci::base::accessorType::AccessorType gatewayLink16ConfigurationClosedAreaType = 1406;

         //  This accessor type is used to identify the uci::type::GatewayLink16ConfigurationEnvironmentType Accessor
         static const uci::base::accessorType::AccessorType gatewayLink16ConfigurationEnvironmentType = 1407;

         //  This accessor type is used to identify the uci::type::GatewayLink16ConfigurationIdentityType Accessor
         static const uci::base::accessorType::AccessorType gatewayLink16ConfigurationIdentityType = 1408;

         //  This accessor type is used to identify the uci::type::GatewayLink16ConfigurationLineType Accessor
         static const uci::base::accessorType::AccessorType gatewayLink16ConfigurationLineType = 1409;

         //  This accessor type is used to identify the uci::type::GatewayLink16ConfigurationMotionType Accessor
         static const uci::base::accessorType::AccessorType gatewayLink16ConfigurationMotionType = 1410;

         //  This accessor type is used to identify the uci::type::GatewayLink16ConfigurationMovementVectorType Accessor
         static const uci::base::accessorType::AccessorType gatewayLink16ConfigurationMovementVectorType = 1411;

         //  This accessor type is used to identify the uci::type::GatewayLink16ConfigurationRectangleType Accessor
         static const uci::base::accessorType::AccessorType gatewayLink16ConfigurationRectangleType = 1412;

         //  This accessor type is used to identify the uci::type::GatewayLink16ConfigurationSlaveType Accessor
         static const uci::base::accessorType::AccessorType gatewayLink16ConfigurationSlaveType = 1413;

         //  This accessor type is used to identify the uci::type::GatewayLinkDestinationType Accessor
         static const uci::base::accessorType::AccessorType gatewayLinkDestinationType = 1414;

         //  This accessor type is used to identify the uci::type::GatewayLinkStatisticsType Accessor
         static const uci::base::accessorType::AccessorType gatewayLinkStatisticsType = 1415;

         //  This accessor type is used to identify the uci::type::GatewaySettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType gatewaySettingsCommandMT = 1416;

         //  This accessor type is used to identify the uci::type::GatewaySettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType gatewaySettingsCommandQFT = 1417;

         //  This accessor type is used to identify the uci::type::GatewaySettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType gatewaySettingsCommandStatusMT = 1418;

         //  This accessor type is used to identify the uci::type::GatewaySettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType gatewaySettingsCommandStatusQFT = 1419;

         //  This accessor type is used to identify the uci::type::GCRS_KinematicsType Accessor
         static const uci::base::accessorType::AccessorType gCRS_KinematicsType = 1420;

         //  This accessor type is used to identify the uci::type::GenerateOptionCommandType Accessor
         static const uci::base::accessorType::AccessorType generateOptionCommandType = 1421;

         //  This accessor type is used to identify the uci::type::GenerateOptionMetaTaskType Accessor
         static const uci::base::accessorType::AccessorType generateOptionMetaTaskType = 1422;

         //  This accessor type is used to identify the uci::type::GenerateOptionTaskType Accessor
         static const uci::base::accessorType::AccessorType generateOptionTaskType = 1423;

         //  This accessor type is used to identify the uci::type::GenerateOptionType Accessor
         static const uci::base::accessorType::AccessorType generateOptionType = 1424;

         //  This accessor type is used to identify the uci::type::GenericDataLinkNativeInfoType Accessor
         static const uci::base::accessorType::AccessorType genericDataLinkNativeInfoType = 1425;

         //  This accessor type is used to identify the uci::type::GeoFiltersType Accessor
         static const uci::base::accessorType::AccessorType geoFiltersType = 1426;

         //  This accessor type is used to identify the uci::type::GeoLocatedObjectType Accessor
         static const uci::base::accessorType::AccessorType geoLocatedObjectType = 1427;

         //  This accessor type is used to identify the uci::type::GeoLocatedStoredObjectType Accessor
         static const uci::base::accessorType::AccessorType geoLocatedStoredObjectType = 1428;

         //  This accessor type is used to identify the uci::type::HeaderType Accessor
         static const uci::base::accessorType::AccessorType headerType = 1429;

      } // Namespace: accessorType
      //  
      namespace headerType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::HeaderType::SchemaVersion Accessor
            static const uci::base::accessorType::AccessorType schemaVersion = 1430;

         } // Namespace: accessorType
      } // Namespace: headerType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::HoverType Accessor
         static const uci::base::accessorType::AccessorType hoverType = 1431;

         //  This accessor type is used to identify the uci::type::HRR_ChipSizeType Accessor
         static const uci::base::accessorType::AccessorType hRR_ChipSizeType = 1432;

         //  This accessor type is used to identify the uci::type::HRR_OptionsType Accessor
         static const uci::base::accessorType::AccessorType hRR_OptionsType = 1433;

      } // Namespace: accessorType
      //  
      namespace hRR_OptionsType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::HRR_OptionsType::HRR_PeakScatterers Accessor
            static const uci::base::accessorType::AccessorType hRR_PeakScatterers = 1434;

         } // Namespace: accessorType
      } // Namespace: hRR_OptionsType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::HSB_ColorControlType Accessor
         static const uci::base::accessorType::AccessorType hSB_ColorControlType = 1435;

         //  This accessor type is used to identify the uci::type::ID_Type Accessor
         static const uci::base::accessorType::AccessorType iD_Type = 1436;

         //  This accessor type is used to identify the uci::type::IdentityBasicType Accessor
         static const uci::base::accessorType::AccessorType identityBasicType = 1437;

         //  This accessor type is used to identify the uci::type::IdentityComparisonType Accessor
         static const uci::base::accessorType::AccessorType identityComparisonType = 1438;

         //  This accessor type is used to identify the uci::type::IdentityConfidenceType Accessor
         static const uci::base::accessorType::AccessorType identityConfidenceType = 1439;

         //  This accessor type is used to identify the uci::type::IdentityType Accessor
         static const uci::base::accessorType::AccessorType identityType = 1440;

         //  This accessor type is used to identify the uci::type::IdentityZoneType Accessor
         static const uci::base::accessorType::AccessorType identityZoneType = 1441;

         //  This accessor type is used to identify the uci::type::IFF_Mode4Type Accessor
         static const uci::base::accessorType::AccessorType iFF_Mode4Type = 1442;

         //  This accessor type is used to identify the uci::type::IFF_Mode5Type Accessor
         static const uci::base::accessorType::AccessorType iFF_Mode5Type = 1443;

         //  This accessor type is used to identify the uci::type::IFF_ModeS_Type Accessor
         static const uci::base::accessorType::AccessorType iFF_ModeS_Type = 1444;

      } // Namespace: accessorType
      //  
      namespace iFF_ModeS_Type {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::IFF_ModeS_Type::ICAO Accessor
            static const uci::base::accessorType::AccessorType iCAO = 1445;

         } // Namespace: accessorType
      } // Namespace: iFF_ModeS_Type
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::IFF_OctalModeType Accessor
         static const uci::base::accessorType::AccessorType iFF_OctalModeType = 1446;

         //  This accessor type is used to identify the uci::type::IFF_Type Accessor
         static const uci::base::accessorType::AccessorType iFF_Type = 1447;

         //  This accessor type is used to identify the uci::type::ImageAugmentationParametersType Accessor
         static const uci::base::accessorType::AccessorType imageAugmentationParametersType = 1448;

         //  This accessor type is used to identify the uci::type::ImageGeoRegistrationProcessingResultsType Accessor
         static const uci::base::accessorType::AccessorType imageGeoRegistrationProcessingResultsType = 1449;

      } // Namespace: accessorType
      //  
      namespace imageGeoRegistrationProcessingResultsType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the
            //  uci::type::ImageGeoRegistrationProcessingResultsType::GeoRegistrationQuality Accessor
            static const uci::base::accessorType::AccessorType geoRegistrationQuality = 1450;

         } // Namespace: accessorType
      } // Namespace: imageGeoRegistrationProcessingResultsType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::ImagePixelGeoLocationProcessingParametersType Accessor
         static const uci::base::accessorType::AccessorType imagePixelGeoLocationProcessingParametersType = 1451;

         //  This accessor type is used to identify the uci::type::ImagePixelGeoLocationProcessingResultsType Accessor
         static const uci::base::accessorType::AccessorType imagePixelGeoLocationProcessingResultsType = 1452;

         //  This accessor type is used to identify the uci::type::ImageRegionOfInterestPET Accessor
         static const uci::base::accessorType::AccessorType imageRegionOfInterestPET = 1453;

         //  This accessor type is used to identify the uci::type::ImageSubheaderType Accessor
         static const uci::base::accessorType::AccessorType imageSubheaderType = 1454;

      } // Namespace: accessorType
      //  
      namespace imageSubheaderType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::ImageSubheaderType::ImageSource Accessor
            static const uci::base::accessorType::AccessorType imageSource = 1455;

            //  This accessor type is used to identify the uci::type::ImageSubheaderType::PrimaryTargetID Accessor
            static const uci::base::accessorType::AccessorType primaryTargetID = 1456;

            //  This accessor type is used to identify the uci::type::ImageSubheaderType::ImageID1 Accessor
            static const uci::base::accessorType::AccessorType imageID1 = 1457;

            //  This accessor type is used to identify the uci::type::ImageSubheaderType::ImageID2 Accessor
            static const uci::base::accessorType::AccessorType imageID2 = 1458;

            //  This accessor type is used to identify the uci::type::ImageSubheaderType::ICOMn Accessor
            static const uci::base::accessorType::AccessorType iCOMn = 1459;

         } // Namespace: accessorType
      } // Namespace: imageSubheaderType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::ImpactConditionsType Accessor
         static const uci::base::accessorType::AccessorType impactConditionsType = 1460;

         //  This accessor type is used to identify the uci::type::IncompleteProcessingType Accessor
         static const uci::base::accessorType::AccessorType incompleteProcessingType = 1461;

         //  This accessor type is used to identify the uci::type::InertialStateType Accessor
         static const uci::base::accessorType::AccessorType inertialStateType = 1462;

         //  This accessor type is used to identify the uci::type::IngressEgressType Accessor
         static const uci::base::accessorType::AccessorType ingressEgressType = 1463;

         //  This accessor type is used to identify the uci::type::InputProductType Accessor
         static const uci::base::accessorType::AccessorType inputProductType = 1464;

         //  This accessor type is used to identify the uci::type::InstallationDetailsType Accessor
         static const uci::base::accessorType::AccessorType installationDetailsType = 1465;

         //  This accessor type is used to identify the uci::type::IntegerMinMaxType Accessor
         static const uci::base::accessorType::AccessorType integerMinMaxType = 1466;

         //  This accessor type is used to identify the uci::type::InternationalDesignatorType Accessor
         static const uci::base::accessorType::AccessorType internationalDesignatorType = 1467;

         //  This accessor type is used to identify the uci::type::IO_PortCommandMDT Accessor
         static const uci::base::accessorType::AccessorType iO_PortCommandMDT = 1468;

         //  This accessor type is used to identify the uci::type::IO_PortCommandMT Accessor
         static const uci::base::accessorType::AccessorType iO_PortCommandMT = 1469;

         //  This accessor type is used to identify the uci::type::IO_PortCommandQFT Accessor
         static const uci::base::accessorType::AccessorType iO_PortCommandQFT = 1470;

         //  This accessor type is used to identify the uci::type::IO_PortCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType iO_PortCommandStatusMT = 1471;

         //  This accessor type is used to identify the uci::type::IO_PortCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType iO_PortCommandStatusQFT = 1472;

         //  This accessor type is used to identify the uci::type::IO_PortCommandType Accessor
         static const uci::base::accessorType::AccessorType iO_PortCommandType = 1473;

         //  This accessor type is used to identify the uci::type::IO_PortConfigurationStatusType Accessor
         static const uci::base::accessorType::AccessorType iO_PortConfigurationStatusType = 1474;

         //  This accessor type is used to identify the uci::type::IO_PortStatusMDT Accessor
         static const uci::base::accessorType::AccessorType iO_PortStatusMDT = 1475;

         //  This accessor type is used to identify the uci::type::IO_PortStatusMT Accessor
         static const uci::base::accessorType::AccessorType iO_PortStatusMT = 1476;

         //  This accessor type is used to identify the uci::type::IO_PortStatusQFT Accessor
         static const uci::base::accessorType::AccessorType iO_PortStatusQFT = 1477;

         //  This accessor type is used to identify the uci::type::IO_PortStatusType Accessor
         static const uci::base::accessorType::AccessorType iO_PortStatusType = 1478;

         //  This accessor type is used to identify the uci::type::IPv4_DataLinkNativeInfoType Accessor
         static const uci::base::accessorType::AccessorType iPv4_DataLinkNativeInfoType = 1479;

         //  This accessor type is used to identify the uci::type::IPv4_SettingsType Accessor
         static const uci::base::accessorType::AccessorType iPv4_SettingsType = 1480;

         //  This accessor type is used to identify the uci::type::IPv6_DataLinkNativeInfoType Accessor
         static const uci::base::accessorType::AccessorType iPv6_DataLinkNativeInfoType = 1481;

         //  This accessor type is used to identify the uci::type::IPv6_SettingsType Accessor
         static const uci::base::accessorType::AccessorType iPv6_SettingsType = 1482;

         //  This accessor type is used to identify the uci::type::ISAR_SubCapabilityType Accessor
         static const uci::base::accessorType::AccessorType iSAR_SubCapabilityType = 1483;

         //  This accessor type is used to identify the uci::type::ISAR_TargetType Accessor
         static const uci::base::accessorType::AccessorType iSAR_TargetType = 1484;

         //  This accessor type is used to identify the uci::type::JammerEmitterIdentityType Accessor
         static const uci::base::accessorType::AccessorType jammerEmitterIdentityType = 1485;

         //  This accessor type is used to identify the uci::type::JammerStationType Accessor
         static const uci::base::accessorType::AccessorType jammerStationType = 1486;

         //  This accessor type is used to identify the uci::type::JPEG_SettingsType Accessor
         static const uci::base::accessorType::AccessorType jPEG_SettingsType = 1487;

         //  This accessor type is used to identify the uci::type::JPEG_WaveletTransformType Accessor
         static const uci::base::accessorType::AccessorType jPEG_WaveletTransformType = 1488;

         //  This accessor type is used to identify the uci::type::KinematicsFixedPositionType Accessor
         static const uci::base::accessorType::AccessorType kinematicsFixedPositionType = 1489;

         //  This accessor type is used to identify the uci::type::KinematicsFixedType Accessor
         static const uci::base::accessorType::AccessorType kinematicsFixedType = 1490;

         //  This accessor type is used to identify the uci::type::KinematicsType Accessor
         static const uci::base::accessorType::AccessorType kinematicsType = 1491;

         //  This accessor type is used to identify the uci::type::LAR_AnalysisType Accessor
         static const uci::base::accessorType::AccessorType lAR_AnalysisType = 1492;

         //  This accessor type is used to identify the uci::type::LAR_CalculationWindType Accessor
         static const uci::base::accessorType::AccessorType lAR_CalculationWindType = 1493;

         //  This accessor type is used to identify the uci::type::LAR_DetailsType Accessor
         static const uci::base::accessorType::AccessorType lAR_DetailsType = 1494;

         //  This accessor type is used to identify the uci::type::LAR_ID_Type Accessor
         static const uci::base::accessorType::AccessorType lAR_ID_Type = 1495;

         //  This accessor type is used to identify the uci::type::LAR_MDT Accessor
         static const uci::base::accessorType::AccessorType lAR_MDT = 1496;

         //  This accessor type is used to identify the uci::type::LAR_MT Accessor
         static const uci::base::accessorType::AccessorType lAR_MT = 1497;

         //  This accessor type is used to identify the uci::type::LAR_QFT Accessor
         static const uci::base::accessorType::AccessorType lAR_QFT = 1498;

         //  This accessor type is used to identify the uci::type::LAR_ReleaseConditionsType Accessor
         static const uci::base::accessorType::AccessorType lAR_ReleaseConditionsType = 1499;

         //  This accessor type is used to identify the uci::type::LAR_ReportMDT Accessor
         static const uci::base::accessorType::AccessorType lAR_ReportMDT = 1500;

         //  This accessor type is used to identify the uci::type::LAR_ReportMT Accessor
         static const uci::base::accessorType::AccessorType lAR_ReportMT = 1501;

         //  This accessor type is used to identify the uci::type::LAR_ReportQFT Accessor
         static const uci::base::accessorType::AccessorType lAR_ReportQFT = 1502;

         //  This accessor type is used to identify the uci::type::LAR_RequestID_Type Accessor
         static const uci::base::accessorType::AccessorType lAR_RequestID_Type = 1503;

         //  This accessor type is used to identify the uci::type::LAR_RequestMDT Accessor
         static const uci::base::accessorType::AccessorType lAR_RequestMDT = 1504;

         //  This accessor type is used to identify the uci::type::LAR_RequestMT Accessor
         static const uci::base::accessorType::AccessorType lAR_RequestMT = 1505;

         //  This accessor type is used to identify the uci::type::LAR_RequestQFT Accessor
         static const uci::base::accessorType::AccessorType lAR_RequestQFT = 1506;

         //  This accessor type is used to identify the uci::type::LAR_RequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType lAR_RequestStatusMDT = 1507;

         //  This accessor type is used to identify the uci::type::LAR_RequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType lAR_RequestStatusMT = 1508;

         //  This accessor type is used to identify the uci::type::LAR_RequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType lAR_RequestStatusQFT = 1509;

         //  This accessor type is used to identify the uci::type::LineTargetType Accessor
         static const uci::base::accessorType::AccessorType lineTargetType = 1510;

      } // Namespace: accessorType
      //  
      namespace lineTargetType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::LineTargetType::LOC_ID Accessor
            static const uci::base::accessorType::AccessorType lOC_ID = 1511;

         } // Namespace: accessorType
      } // Namespace: lineTargetType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::LineType Accessor
         static const uci::base::accessorType::AccessorType lineType = 1512;

         //  This accessor type is used to identify the uci::type::Link16DataLinkIdentifierType Accessor
         static const uci::base::accessorType::AccessorType link16DataLinkIdentifierType = 1513;

         //  This accessor type is used to identify the uci::type::Link16DataLinkNativeInfoType Accessor
         static const uci::base::accessorType::AccessorType link16DataLinkNativeInfoType = 1514;

         //  This accessor type is used to identify the uci::type::Link16EntityDataLinkNativeFilterType Accessor
         static const uci::base::accessorType::AccessorType link16EntityDataLinkNativeFilterType = 1515;

         //  This accessor type is used to identify the uci::type::Link16EntityMetadataType Accessor
         static const uci::base::accessorType::AccessorType link16EntityMetadataType = 1516;

         //  This accessor type is used to identify the uci::type::Link16MetadataType Accessor
         static const uci::base::accessorType::AccessorType link16MetadataType = 1517;

      } // Namespace: accessorType
      //  
      namespace link16MetadataType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::Link16MetadataType::ControlChannel Accessor
            static const uci::base::accessorType::AccessorType controlChannel = 1518;

         } // Namespace: accessorType
      } // Namespace: link16MetadataType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::Link16SystemMetadataType Accessor
         static const uci::base::accessorType::AccessorType link16SystemMetadataType = 1519;

         //  This accessor type is used to identify the uci::type::Link16TrackIdentifierType Accessor
         static const uci::base::accessorType::AccessorType link16TrackIdentifierType = 1520;

         //  This accessor type is used to identify the uci::type::LinkConnectionRequestType Accessor
         static const uci::base::accessorType::AccessorType linkConnectionRequestType = 1521;

         //  This accessor type is used to identify the uci::type::LinkedTaskType Accessor
         static const uci::base::accessorType::AccessorType linkedTaskType = 1522;

         //  This accessor type is used to identify the uci::type::LinkMetricsType Accessor
         static const uci::base::accessorType::AccessorType linkMetricsType = 1523;

         //  This accessor type is used to identify the uci::type::LinkRatesType Accessor
         static const uci::base::accessorType::AccessorType linkRatesType = 1524;

         //  This accessor type is used to identify the uci::type::LobeType Accessor
         static const uci::base::accessorType::AccessorType lobeType = 1525;

         //  This accessor type is used to identify the uci::type::LocatedEllipseType Accessor
         static const uci::base::accessorType::AccessorType locatedEllipseType = 1526;

         //  This accessor type is used to identify the uci::type::LocatedRectangleType Accessor
         static const uci::base::accessorType::AccessorType locatedRectangleType = 1527;

         //  This accessor type is used to identify the uci::type::LocationFilterType Accessor
         static const uci::base::accessorType::AccessorType locationFilterType = 1528;

         //  This accessor type is used to identify the uci::type::LocationType Accessor
         static const uci::base::accessorType::AccessorType locationType = 1529;

         //  This accessor type is used to identify the uci::type::LogMDT Accessor
         static const uci::base::accessorType::AccessorType logMDT = 1530;

         //  This accessor type is used to identify the uci::type::LogMT Accessor
         static const uci::base::accessorType::AccessorType logMT = 1531;

         //  This accessor type is used to identify the uci::type::LogQFT Accessor
         static const uci::base::accessorType::AccessorType logQFT = 1532;

         //  This accessor type is used to identify the uci::type::LoiterPointType Accessor
         static const uci::base::accessorType::AccessorType loiterPointType = 1533;

         //  This accessor type is used to identify the uci::type::LoiterProgressType Accessor
         static const uci::base::accessorType::AccessorType loiterProgressType = 1534;

         //  This accessor type is used to identify the uci::type::LoiterType Accessor
         static const uci::base::accessorType::AccessorType loiterType = 1535;

         //  This accessor type is used to identify the uci::type::LOS_D_Type Accessor
         static const uci::base::accessorType::AccessorType lOS_D_Type = 1536;

         //  This accessor type is used to identify the uci::type::LOS_InertialA_Type Accessor
         static const uci::base::accessorType::AccessorType lOS_InertialA_Type = 1537;

         //  This accessor type is used to identify the uci::type::LOS_InertialB_Type Accessor
         static const uci::base::accessorType::AccessorType lOS_InertialB_Type = 1538;

         //  This accessor type is used to identify the uci::type::LOS_RatesType Accessor
         static const uci::base::accessorType::AccessorType lOS_RatesType = 1539;

         //  This accessor type is used to identify the uci::type::LOS_Type Accessor
         static const uci::base::accessorType::AccessorType lOS_Type = 1540;

         //  This accessor type is used to identify the uci::type::LOS_UncertaintyType Accessor
         static const uci::base::accessorType::AccessorType lOS_UncertaintyType = 1541;

         //  This accessor type is used to identify the uci::type::LOS_VariableA_Type Accessor
         static const uci::base::accessorType::AccessorType lOS_VariableA_Type = 1542;

         //  This accessor type is used to identify the uci::type::LOS_VariableB_Type Accessor
         static const uci::base::accessorType::AccessorType lOS_VariableB_Type = 1543;

         //  This accessor type is used to identify the uci::type::LostLinkSourceType Accessor
         static const uci::base::accessorType::AccessorType lostLinkSourceType = 1544;

         //  This accessor type is used to identify the uci::type::MaintenanceStatusType Accessor
         static const uci::base::accessorType::AccessorType maintenanceStatusType = 1545;

         //  This accessor type is used to identify the uci::type::MDF_CommandSubCategoryType Accessor
         static const uci::base::accessorType::AccessorType mDF_CommandSubCategoryType = 1546;

         //  This accessor type is used to identify the uci::type::MDF_CommandType Accessor
         static const uci::base::accessorType::AccessorType mDF_CommandType = 1547;

         //  This accessor type is used to identify the uci::type::MDF_FileType Accessor
         static const uci::base::accessorType::AccessorType mDF_FileType = 1548;

         //  This accessor type is used to identify the uci::type::MDF_ID_Type Accessor
         static const uci::base::accessorType::AccessorType mDF_ID_Type = 1549;

         //  This accessor type is used to identify the uci::type::MDF_ReferenceType Accessor
         static const uci::base::accessorType::AccessorType mDF_ReferenceType = 1550;

         //  This accessor type is used to identify the uci::type::MDF_StatusType Accessor
         static const uci::base::accessorType::AccessorType mDF_StatusType = 1551;

         //  This accessor type is used to identify the uci::type::MDF_SubCategoryFileID_Type Accessor
         static const uci::base::accessorType::AccessorType mDF_SubCategoryFileID_Type = 1552;

         //  This accessor type is used to identify the uci::type::MDF_SubCategoryID_Type Accessor
         static const uci::base::accessorType::AccessorType mDF_SubCategoryID_Type = 1553;

         //  This accessor type is used to identify the uci::type::MDF_SubCategoryType Accessor
         static const uci::base::accessorType::AccessorType mDF_SubCategoryType = 1554;

         //  This accessor type is used to identify the uci::type::MDF_Type Accessor
         static const uci::base::accessorType::AccessorType mDF_Type = 1555;

         //  This accessor type is used to identify the uci::type::MessageConfigurationType Accessor
         static const uci::base::accessorType::AccessorType messageConfigurationType = 1556;

         //  This accessor type is used to identify the uci::type::MessageDestinationType Accessor
         static const uci::base::accessorType::AccessorType messageDestinationType = 1557;

         //  This accessor type is used to identify the uci::type::MessageTransmissionListMDT Accessor
         static const uci::base::accessorType::AccessorType messageTransmissionListMDT = 1558;

         //  This accessor type is used to identify the uci::type::MessageTransmissionListMT Accessor
         static const uci::base::accessorType::AccessorType messageTransmissionListMT = 1559;

         //  This accessor type is used to identify the uci::type::MessageTransmissionListQFT Accessor
         static const uci::base::accessorType::AccessorType messageTransmissionListQFT = 1560;

         //  This accessor type is used to identify the uci::type::MessageTransmissionListType Accessor
         static const uci::base::accessorType::AccessorType messageTransmissionListType = 1561;

         //  This accessor type is used to identify the uci::type::MessageType Accessor
         static const uci::base::accessorType::AccessorType messageType = 1562;

      } // Namespace: accessorType
      //  
      namespace messageType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::MessageType::SequenceGroup Accessor
            static const uci::base::accessorType::AccessorType sequenceGroup = 1563;

         } // Namespace: accessorType
      } // Namespace: messageType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::MetadataFiltersType Accessor
         static const uci::base::accessorType::AccessorType metadataFiltersType = 1564;

         //  This accessor type is used to identify the uci::type::MetadataID_Type Accessor
         static const uci::base::accessorType::AccessorType metadataID_Type = 1565;

         //  This accessor type is used to identify the uci::type::MetaTaskCancelID_Type Accessor
         static const uci::base::accessorType::AccessorType metaTaskCancelID_Type = 1566;

         //  This accessor type is used to identify the uci::type::MetaTaskCancelMDT Accessor
         static const uci::base::accessorType::AccessorType metaTaskCancelMDT = 1567;

         //  This accessor type is used to identify the uci::type::MetaTaskCancelMT Accessor
         static const uci::base::accessorType::AccessorType metaTaskCancelMT = 1568;

         //  This accessor type is used to identify the uci::type::MetaTaskCancelQFT Accessor
         static const uci::base::accessorType::AccessorType metaTaskCancelQFT = 1569;

         //  This accessor type is used to identify the uci::type::MetaTaskCommandedResponseType Accessor
         static const uci::base::accessorType::AccessorType metaTaskCommandedResponseType = 1570;

         //  This accessor type is used to identify the uci::type::MetaTaskGeneratedType Accessor
         static const uci::base::accessorType::AccessorType metaTaskGeneratedType = 1571;

         //  This accessor type is used to identify the uci::type::MetaTaskID_Type Accessor
         static const uci::base::accessorType::AccessorType metaTaskID_Type = 1572;

         //  This accessor type is used to identify the uci::type::MetaTaskMDT Accessor
         static const uci::base::accessorType::AccessorType metaTaskMDT = 1573;

         //  This accessor type is used to identify the uci::type::MetaTaskMT Accessor
         static const uci::base::accessorType::AccessorType metaTaskMT = 1574;

         //  This accessor type is used to identify the uci::type::MetaTaskQFT Accessor
         static const uci::base::accessorType::AccessorType metaTaskQFT = 1575;

         //  This accessor type is used to identify the uci::type::MetaTaskResponseType Accessor
         static const uci::base::accessorType::AccessorType metaTaskResponseType = 1576;

         //  This accessor type is used to identify the uci::type::MetaTaskStatusMDT Accessor
         static const uci::base::accessorType::AccessorType metaTaskStatusMDT = 1577;

         //  This accessor type is used to identify the uci::type::MetaTaskStatusMT Accessor
         static const uci::base::accessorType::AccessorType metaTaskStatusMT = 1578;

         //  This accessor type is used to identify the uci::type::MetaTaskStatusQFT Accessor
         static const uci::base::accessorType::AccessorType metaTaskStatusQFT = 1579;

         //  This accessor type is used to identify the uci::type::MetaTaskTargetType Accessor
         static const uci::base::accessorType::AccessorType metaTaskTargetType = 1580;

         //  This accessor type is used to identify the uci::type::MetaTaskTriggerType Accessor
         static const uci::base::accessorType::AccessorType metaTaskTriggerType = 1581;

         //  This accessor type is used to identify the uci::type::MetricsType Accessor
         static const uci::base::accessorType::AccessorType metricsType = 1582;

         //  This accessor type is used to identify the uci::type::MilliwattPowerRatioRangeType Accessor
         static const uci::base::accessorType::AccessorType milliwattPowerRatioRangeType = 1583;

         //  This accessor type is used to identify the uci::type::MISP_PackingPlanType Accessor
         static const uci::base::accessorType::AccessorType mISP_PackingPlanType = 1584;

      } // Namespace: accessorType
      //  
      namespace mISP_PackingPlanType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::MISP_PackingPlanType::PlatformDesignation Accessor
            static const uci::base::accessorType::AccessorType platformDesignation = 1585;

            //  This accessor type is used to identify the uci::type::MISP_PackingPlanType::ImageSourceSensor Accessor
            static const uci::base::accessorType::AccessorType imageSourceSensor = 1586;

            //  This accessor type is used to identify the uci::type::MISP_PackingPlanType::PlatformCallSign Accessor
            static const uci::base::accessorType::AccessorType platformCallSign = 1587;

            //  This accessor type is used to identify the uci::type::MISP_PackingPlanType::ClassificationOrMarkingSystem Accessor
            static const uci::base::accessorType::AccessorType classificationOrMarkingSystem = 1588;

            //  This accessor type is used to identify the uci::type::MISP_PackingPlanType::ClassificationComment Accessor
            static const uci::base::accessorType::AccessorType classificationComment = 1589;

            //  This accessor type is used to identify the uci::type::MISP_PackingPlanType::ItemDesignator Accessor
            static const uci::base::accessorType::AccessorType itemDesignator = 1590;

         } // Namespace: accessorType
      } // Namespace: mISP_PackingPlanType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::MissileEmitterIdentityType Accessor
         static const uci::base::accessorType::AccessorType missileEmitterIdentityType = 1591;

         //  This accessor type is used to identify the uci::type::MissionActivationRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType missionActivationRequestID_Type = 1592;

         //  This accessor type is used to identify the uci::type::MissionActivationRequestMDT Accessor
         static const uci::base::accessorType::AccessorType missionActivationRequestMDT = 1593;

         //  This accessor type is used to identify the uci::type::MissionActivationRequestMT Accessor
         static const uci::base::accessorType::AccessorType missionActivationRequestMT = 1594;

         //  This accessor type is used to identify the uci::type::MissionActivationRequestQFT Accessor
         static const uci::base::accessorType::AccessorType missionActivationRequestQFT = 1595;

         //  This accessor type is used to identify the uci::type::MissionActivationRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType missionActivationRequestStatusMDT = 1596;

         //  This accessor type is used to identify the uci::type::MissionActivationRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType missionActivationRequestStatusMT = 1597;

         //  This accessor type is used to identify the uci::type::MissionActivationRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType missionActivationRequestStatusQFT = 1598;

         //  This accessor type is used to identify the uci::type::MissionContingencyAlertID_Type Accessor
         static const uci::base::accessorType::AccessorType missionContingencyAlertID_Type = 1599;

         //  This accessor type is used to identify the uci::type::MissionContingencyAlertMDT Accessor
         static const uci::base::accessorType::AccessorType missionContingencyAlertMDT = 1600;

         //  This accessor type is used to identify the uci::type::MissionContingencyAlertMT Accessor
         static const uci::base::accessorType::AccessorType missionContingencyAlertMT = 1601;

         //  This accessor type is used to identify the uci::type::MissionContingencyAlertQFT Accessor
         static const uci::base::accessorType::AccessorType missionContingencyAlertQFT = 1602;

         //  This accessor type is used to identify the uci::type::MissionContingencyConditionType Accessor
         static const uci::base::accessorType::AccessorType missionContingencyConditionType = 1603;

         //  This accessor type is used to identify the uci::type::MissionDefinitionMDT Accessor
         static const uci::base::accessorType::AccessorType missionDefinitionMDT = 1604;

         //  This accessor type is used to identify the uci::type::MissionDefinitionMT Accessor
         static const uci::base::accessorType::AccessorType missionDefinitionMT = 1605;

         //  This accessor type is used to identify the uci::type::MissionDefinitionQFT Accessor
         static const uci::base::accessorType::AccessorType missionDefinitionQFT = 1606;

         //  This accessor type is used to identify the uci::type::MissionEnvironmentConstraintType Accessor
         static const uci::base::accessorType::AccessorType missionEnvironmentConstraintType = 1607;

         //  This accessor type is used to identify the uci::type::MissionEnvironmentOverrideID_Type Accessor
         static const uci::base::accessorType::AccessorType missionEnvironmentOverrideID_Type = 1608;

         //  This accessor type is used to identify the uci::type::MissionEnvironmentOverrideMDT Accessor
         static const uci::base::accessorType::AccessorType missionEnvironmentOverrideMDT = 1609;

         //  This accessor type is used to identify the uci::type::MissionEnvironmentOverrideMT Accessor
         static const uci::base::accessorType::AccessorType missionEnvironmentOverrideMT = 1610;

         //  This accessor type is used to identify the uci::type::MissionEnvironmentOverrideQFT Accessor
         static const uci::base::accessorType::AccessorType missionEnvironmentOverrideQFT = 1611;

         //  This accessor type is used to identify the uci::type::MissionID_Type Accessor
         static const uci::base::accessorType::AccessorType missionID_Type = 1612;

         //  This accessor type is used to identify the uci::type::MissionPlanActionReferenceType Accessor
         static const uci::base::accessorType::AccessorType missionPlanActionReferenceType = 1613;

         //  This accessor type is used to identify the uci::type::MissionPlanActivationAutonomySettingType Accessor
         static const uci::base::accessorType::AccessorType missionPlanActivationAutonomySettingType = 1614;

         //  This accessor type is used to identify the uci::type::MissionPlanActivationCommandID_Type Accessor
         static const uci::base::accessorType::AccessorType missionPlanActivationCommandID_Type = 1615;

         //  This accessor type is used to identify the uci::type::MissionPlanActivationCommandMDT Accessor
         static const uci::base::accessorType::AccessorType missionPlanActivationCommandMDT = 1616;

         //  This accessor type is used to identify the uci::type::MissionPlanActivationCommandMT Accessor
         static const uci::base::accessorType::AccessorType missionPlanActivationCommandMT = 1617;

         //  This accessor type is used to identify the uci::type::MissionPlanActivationCommandQFT Accessor
         static const uci::base::accessorType::AccessorType missionPlanActivationCommandQFT = 1618;

         //  This accessor type is used to identify the uci::type::MissionPlanActivationCommandStatusMDT Accessor
         static const uci::base::accessorType::AccessorType missionPlanActivationCommandStatusMDT = 1619;

         //  This accessor type is used to identify the uci::type::MissionPlanActivationCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType missionPlanActivationCommandStatusMT = 1620;

         //  This accessor type is used to identify the uci::type::MissionPlanActivationCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType missionPlanActivationCommandStatusQFT = 1621;

         //  This accessor type is used to identify the uci::type::MissionPlanActivationPolicyType Accessor
         static const uci::base::accessorType::AccessorType missionPlanActivationPolicyType = 1622;

         //  This accessor type is used to identify the uci::type::MissionPlanActivationStateMDT Accessor
         static const uci::base::accessorType::AccessorType missionPlanActivationStateMDT = 1623;

         //  This accessor type is used to identify the uci::type::MissionPlanActivationStateMT Accessor
         static const uci::base::accessorType::AccessorType missionPlanActivationStateMT = 1624;

         //  This accessor type is used to identify the uci::type::MissionPlanActivationStateQFT Accessor
         static const uci::base::accessorType::AccessorType missionPlanActivationStateQFT = 1625;

         //  This accessor type is used to identify the uci::type::MissionPlanCommandApprovalType Accessor
         static const uci::base::accessorType::AccessorType missionPlanCommandApprovalType = 1626;

         //  This accessor type is used to identify the uci::type::MissionPlanCommandFunctionType Accessor
         static const uci::base::accessorType::AccessorType missionPlanCommandFunctionType = 1627;

         //  This accessor type is used to identify the uci::type::MissionPlanCommandID_Type Accessor
         static const uci::base::accessorType::AccessorType missionPlanCommandID_Type = 1628;

         //  This accessor type is used to identify the uci::type::MissionPlanCommandMDT Accessor
         static const uci::base::accessorType::AccessorType missionPlanCommandMDT = 1629;

         //  This accessor type is used to identify the uci::type::MissionPlanCommandMT Accessor
         static const uci::base::accessorType::AccessorType missionPlanCommandMT = 1630;

         //  This accessor type is used to identify the uci::type::MissionPlanCommandProcessType Accessor
         static const uci::base::accessorType::AccessorType missionPlanCommandProcessType = 1631;

         //  This accessor type is used to identify the uci::type::MissionPlanCommandQFT Accessor
         static const uci::base::accessorType::AccessorType missionPlanCommandQFT = 1632;

         //  This accessor type is used to identify the uci::type::MissionPlanCommandRoutePlanningType Accessor
         static const uci::base::accessorType::AccessorType missionPlanCommandRoutePlanningType = 1633;

         //  This accessor type is used to identify the uci::type::MissionPlanCommandSimulateType Accessor
         static const uci::base::accessorType::AccessorType missionPlanCommandSimulateType = 1634;

         //  This accessor type is used to identify the uci::type::MissionPlanCommandStatusMDT Accessor
         static const uci::base::accessorType::AccessorType missionPlanCommandStatusMDT = 1635;

         //  This accessor type is used to identify the uci::type::MissionPlanCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType missionPlanCommandStatusMT = 1636;

         //  This accessor type is used to identify the uci::type::MissionPlanCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType missionPlanCommandStatusQFT = 1637;

         //  This accessor type is used to identify the uci::type::MissionPlanCommandTaskAllocationType Accessor
         static const uci::base::accessorType::AccessorType missionPlanCommandTaskAllocationType = 1638;

         //  This accessor type is used to identify the uci::type::MissionPlanCommandType Accessor
         static const uci::base::accessorType::AccessorType missionPlanCommandType = 1639;

         //  This accessor type is used to identify the uci::type::MissionPlanDetailsType Accessor
         static const uci::base::accessorType::AccessorType missionPlanDetailsType = 1640;

         //  This accessor type is used to identify the uci::type::MissionPlanID_Type Accessor
         static const uci::base::accessorType::AccessorType missionPlanID_Type = 1641;

         //  This accessor type is used to identify the uci::type::MissionPlanMDT Accessor
         static const uci::base::accessorType::AccessorType missionPlanMDT = 1642;

         //  This accessor type is used to identify the uci::type::MissionPlanMetricsReportMDT Accessor
         static const uci::base::accessorType::AccessorType missionPlanMetricsReportMDT = 1643;

         //  This accessor type is used to identify the uci::type::MissionPlanMetricsReportMT Accessor
         static const uci::base::accessorType::AccessorType missionPlanMetricsReportMT = 1644;

         //  This accessor type is used to identify the uci::type::MissionPlanMetricsReportQFT Accessor
         static const uci::base::accessorType::AccessorType missionPlanMetricsReportQFT = 1645;

         //  This accessor type is used to identify the uci::type::MissionPlanMT Accessor
         static const uci::base::accessorType::AccessorType missionPlanMT = 1646;

         //  This accessor type is used to identify the uci::type::MissionPlanNavigationType Accessor
         static const uci::base::accessorType::AccessorType missionPlanNavigationType = 1647;

         //  This accessor type is used to identify the uci::type::MissionPlanningAutonomyResponseChoiceType Accessor
         static const uci::base::accessorType::AccessorType missionPlanningAutonomyResponseChoiceType = 1648;

         //  This accessor type is used to identify the uci::type::MissionPlanningAutonomySettingByResultType Accessor
         static const uci::base::accessorType::AccessorType missionPlanningAutonomySettingByResultType = 1649;

         //  This accessor type is used to identify the uci::type::MissionPlanningAutonomySettingType Accessor
         static const uci::base::accessorType::AccessorType missionPlanningAutonomySettingType = 1650;

         //  This accessor type is used to identify the uci::type::MissionPlanningByCaseTriggerDataType Accessor
         static const uci::base::accessorType::AccessorType missionPlanningByCaseTriggerDataType = 1651;

         //  This accessor type is used to identify the uci::type::MissionPlanningByCaseTriggerType Accessor
         static const uci::base::accessorType::AccessorType missionPlanningByCaseTriggerType = 1652;

         //  This accessor type is used to identify the uci::type::MissionPlanningByResultAutonomousActionType Accessor
         static const uci::base::accessorType::AccessorType missionPlanningByResultAutonomousActionType = 1653;

         //  This accessor type is used to identify the uci::type::MissionPlanningByResultTriggerDataType Accessor
         static const uci::base::accessorType::AccessorType missionPlanningByResultTriggerDataType = 1654;

         //  This accessor type is used to identify the uci::type::MissionPlanningByResultTriggerType Accessor
         static const uci::base::accessorType::AccessorType missionPlanningByResultTriggerType = 1655;

         //  This accessor type is used to identify the uci::type::MissionPlanningTriggerType Accessor
         static const uci::base::accessorType::AccessorType missionPlanningTriggerType = 1656;

         //  This accessor type is used to identify the uci::type::MissionPlanQFT Accessor
         static const uci::base::accessorType::AccessorType missionPlanQFT = 1657;

         //  This accessor type is used to identify the uci::type::MissionPlanReferencesType Accessor
         static const uci::base::accessorType::AccessorType missionPlanReferencesType = 1658;

         //  This accessor type is used to identify the uci::type::MissionPlanReferenceType Accessor
         static const uci::base::accessorType::AccessorType missionPlanReferenceType = 1659;

         //  This accessor type is used to identify the uci::type::MissionPlanRouteType Accessor
         static const uci::base::accessorType::AccessorType missionPlanRouteType = 1660;

         //  This accessor type is used to identify the uci::type::MissionPlanStatusMDT Accessor
         static const uci::base::accessorType::AccessorType missionPlanStatusMDT = 1661;

         //  This accessor type is used to identify the uci::type::MissionPlanStatusMT Accessor
         static const uci::base::accessorType::AccessorType missionPlanStatusMT = 1662;

         //  This accessor type is used to identify the uci::type::MissionPlanStatusQFT Accessor
         static const uci::base::accessorType::AccessorType missionPlanStatusQFT = 1663;

         //  This accessor type is used to identify the uci::type::MissionResponseListID_Type Accessor
         static const uci::base::accessorType::AccessorType missionResponseListID_Type = 1664;

         //  This accessor type is used to identify the uci::type::MissionResponseListMDT Accessor
         static const uci::base::accessorType::AccessorType missionResponseListMDT = 1665;

         //  This accessor type is used to identify the uci::type::MissionResponseListMT Accessor
         static const uci::base::accessorType::AccessorType missionResponseListMT = 1666;

         //  This accessor type is used to identify the uci::type::MissionResponseListQFT Accessor
         static const uci::base::accessorType::AccessorType missionResponseListQFT = 1667;

         //  This accessor type is used to identify the uci::type::MissionResponseListType Accessor
         static const uci::base::accessorType::AccessorType missionResponseListType = 1668;

         //  This accessor type is used to identify the uci::type::MissionTaskStatusMDT Accessor
         static const uci::base::accessorType::AccessorType missionTaskStatusMDT = 1669;

         //  This accessor type is used to identify the uci::type::MissionTaskStatusMT Accessor
         static const uci::base::accessorType::AccessorType missionTaskStatusMT = 1670;

         //  This accessor type is used to identify the uci::type::MissionTaskStatusQFT Accessor
         static const uci::base::accessorType::AccessorType missionTaskStatusQFT = 1671;

         //  This accessor type is used to identify the uci::type::MissionTraceabilityType Accessor
         static const uci::base::accessorType::AccessorType missionTraceabilityType = 1672;

         //  This accessor type is used to identify the uci::type::ModifyRF_ThreadInstanceType Accessor
         static const uci::base::accessorType::AccessorType modifyRF_ThreadInstanceType = 1673;

         //  This accessor type is used to identify the uci::type::ModRateDataType Accessor
         static const uci::base::accessorType::AccessorType modRateDataType = 1674;

         //  This accessor type is used to identify the uci::type::ModulationOnPulseDataBaseType Accessor
         static const uci::base::accessorType::AccessorType modulationOnPulseDataBaseType = 1675;

         //  This accessor type is used to identify the uci::type::ModulationOnPulseDataType Accessor
         static const uci::base::accessorType::AccessorType modulationOnPulseDataType = 1676;

         //  This accessor type is used to identify the uci::type::ModulationParameterType Accessor
         static const uci::base::accessorType::AccessorType modulationParameterType = 1677;

         //  This accessor type is used to identify the uci::type::MSTGTA_Type Accessor
         static const uci::base::accessorType::AccessorType mSTGTA_Type = 1678;

      } // Namespace: accessorType
      //  
      namespace mSTGTA_Type {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::MSTGTA_Type::TGT_NUM Accessor
            static const uci::base::accessorType::AccessorType tGT_NUM = 1679;

            //  This accessor type is used to identify the uci::type::MSTGTA_Type::TGT_PRI Accessor
            static const uci::base::accessorType::AccessorType tGT_PRI = 1680;

            //  This accessor type is used to identify the uci::type::MSTGTA_Type::TGT_LTIOV Accessor
            static const uci::base::accessorType::AccessorType tGT_LTIOV = 1681;

            //  This accessor type is used to identify the uci::type::MSTGTA_Type::TGT_COLL Accessor
            static const uci::base::accessorType::AccessorType tGT_COLL = 1682;

            //  This accessor type is used to identify the uci::type::MSTGTA_Type::TGT_CAT Accessor
            static const uci::base::accessorType::AccessorType tGT_CAT = 1683;

            //  This accessor type is used to identify the uci::type::MSTGTA_Type::TGT_UTC Accessor
            static const uci::base::accessorType::AccessorType tGT_UTC = 1684;

         } // Namespace: accessorType
      } // Namespace: mSTGTA_Type
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::MTI_ActivityStatusBaseType Accessor
         static const uci::base::accessorType::AccessorType mTI_ActivityStatusBaseType = 1685;

         //  This accessor type is used to identify the uci::type::MTI_ActivityStatusDetailType Accessor
         static const uci::base::accessorType::AccessorType mTI_ActivityStatusDetailType = 1686;

         //  This accessor type is used to identify the uci::type::MTI_OpZoneFilterAreaType Accessor
         static const uci::base::accessorType::AccessorType mTI_OpZoneFilterAreaType = 1687;

         //  This accessor type is used to identify the uci::type::MTI_PredictedActivityDetailType Accessor
         static const uci::base::accessorType::AccessorType mTI_PredictedActivityDetailType = 1688;

         //  This accessor type is used to identify the uci::type::MTI_STANAG_4607_OpZoneFilterAreaType Accessor
         static const uci::base::accessorType::AccessorType mTI_STANAG_4607_OpZoneFilterAreaType = 1689;

         //  This accessor type is used to identify the uci::type::MTI_TargetClassType Accessor
         static const uci::base::accessorType::AccessorType mTI_TargetClassType = 1690;

         //  This accessor type is used to identify the uci::type::MustFlyLocationType Accessor
         static const uci::base::accessorType::AccessorType mustFlyLocationType = 1691;

         //  This accessor type is used to identify the uci::type::MustFlyType Accessor
         static const uci::base::accessorType::AccessorType mustFlyType = 1692;

         //  This accessor type is used to identify the uci::type::NavigationReportMDT Accessor
         static const uci::base::accessorType::AccessorType navigationReportMDT = 1693;

         //  This accessor type is used to identify the uci::type::NavigationReportMT Accessor
         static const uci::base::accessorType::AccessorType navigationReportMT = 1694;

         //  This accessor type is used to identify the uci::type::NavigationReportQFT Accessor
         static const uci::base::accessorType::AccessorType navigationReportQFT = 1695;

         //  This accessor type is used to identify the uci::type::NavigationSolutionCorrectionsType Accessor
         static const uci::base::accessorType::AccessorType navigationSolutionCorrectionsType = 1696;

         //  This accessor type is used to identify the uci::type::NavigationSourceType Accessor
         static const uci::base::accessorType::AccessorType navigationSourceType = 1697;

         //  This accessor type is used to identify the uci::type::NavigationType Accessor
         static const uci::base::accessorType::AccessorType navigationType = 1698;

         //  This accessor type is used to identify the uci::type::NetworkLinkID_Type Accessor
         static const uci::base::accessorType::AccessorType networkLinkID_Type = 1699;

         //  This accessor type is used to identify the uci::type::NextPathSegmentType Accessor
         static const uci::base::accessorType::AccessorType nextPathSegmentType = 1700;

         //  This accessor type is used to identify the uci::type::NITF_PackingPlanType Accessor
         static const uci::base::accessorType::AccessorType nITF_PackingPlanType = 1701;

         //  This accessor type is used to identify the uci::type::NotificationSourceType Accessor
         static const uci::base::accessorType::AccessorType notificationSourceType = 1702;

         //  This accessor type is used to identify the uci::type::ObjectCorrelationType Accessor
         static const uci::base::accessorType::AccessorType objectCorrelationType = 1703;

         //  This accessor type is used to identify the uci::type::ObjectInstanceDamageEstimateType Accessor
         static const uci::base::accessorType::AccessorType objectInstanceDamageEstimateType = 1704;

         //  This accessor type is used to identify the uci::type::ObjectTypeDamageEstimateType Accessor
         static const uci::base::accessorType::AccessorType objectTypeDamageEstimateType = 1705;

         //  This accessor type is used to identify the uci::type::ObservationCountsByHourType Accessor
         static const uci::base::accessorType::AccessorType observationCountsByHourType = 1706;

         //  This accessor type is used to identify the uci::type::ObservationReportMDT Accessor
         static const uci::base::accessorType::AccessorType observationReportMDT = 1707;

         //  This accessor type is used to identify the uci::type::ObservationReportMT Accessor
         static const uci::base::accessorType::AccessorType observationReportMT = 1708;

         //  This accessor type is used to identify the uci::type::ObservationReportQFT Accessor
         static const uci::base::accessorType::AccessorType observationReportQFT = 1709;

         //  This accessor type is used to identify the uci::type::OffsetTimeType Accessor
         static const uci::base::accessorType::AccessorType offsetTimeType = 1710;

         //  This accessor type is used to identify the uci::type::OpDescriptionType Accessor
         static const uci::base::accessorType::AccessorType opDescriptionType = 1711;

         //  This accessor type is used to identify the uci::type::OperationalScheduleType Accessor
         static const uci::base::accessorType::AccessorType operationalScheduleType = 1712;

         //  This accessor type is used to identify the uci::type::OperatorActionAutonomousType Accessor
         static const uci::base::accessorType::AccessorType operatorActionAutonomousType = 1713;

         //  This accessor type is used to identify the uci::type::OperatorActionCommandID_Type Accessor
         static const uci::base::accessorType::AccessorType operatorActionCommandID_Type = 1714;

         //  This accessor type is used to identify the uci::type::OperatorActionCommandMDT Accessor
         static const uci::base::accessorType::AccessorType operatorActionCommandMDT = 1715;

         //  This accessor type is used to identify the uci::type::OperatorActionCommandMT Accessor
         static const uci::base::accessorType::AccessorType operatorActionCommandMT = 1716;

         //  This accessor type is used to identify the uci::type::OperatorActionCommandQFT Accessor
         static const uci::base::accessorType::AccessorType operatorActionCommandQFT = 1717;

         //  This accessor type is used to identify the uci::type::OperatorActionCommandStatusMDT Accessor
         static const uci::base::accessorType::AccessorType operatorActionCommandStatusMDT = 1718;

         //  This accessor type is used to identify the uci::type::OperatorActionCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType operatorActionCommandStatusMT = 1719;

         //  This accessor type is used to identify the uci::type::OperatorActionCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType operatorActionCommandStatusQFT = 1720;

         //  This accessor type is used to identify the uci::type::OperatorActionControlledType Accessor
         static const uci::base::accessorType::AccessorType operatorActionControlledType = 1721;

         //  This accessor type is used to identify the uci::type::OperatorActionID_Type Accessor
         static const uci::base::accessorType::AccessorType operatorActionID_Type = 1722;

         //  This accessor type is used to identify the uci::type::OperatorActionType Accessor
         static const uci::base::accessorType::AccessorType operatorActionType = 1723;

         //  This accessor type is used to identify the uci::type::OperatorAnnotationID_Type Accessor
         static const uci::base::accessorType::AccessorType operatorAnnotationID_Type = 1724;

         //  This accessor type is used to identify the uci::type::OperatorAnnotationMDT Accessor
         static const uci::base::accessorType::AccessorType operatorAnnotationMDT = 1725;

         //  This accessor type is used to identify the uci::type::OperatorAnnotationMT Accessor
         static const uci::base::accessorType::AccessorType operatorAnnotationMT = 1726;

         //  This accessor type is used to identify the uci::type::OperatorAnnotationQFT Accessor
         static const uci::base::accessorType::AccessorType operatorAnnotationQFT = 1727;

         //  This accessor type is used to identify the uci::type::OperatorDefaultActionType Accessor
         static const uci::base::accessorType::AccessorType operatorDefaultActionType = 1728;

         //  This accessor type is used to identify the uci::type::OperatorID_Type Accessor
         static const uci::base::accessorType::AccessorType operatorID_Type = 1729;

         //  This accessor type is used to identify the uci::type::OperatorLocationOfInterestClauseType Accessor
         static const uci::base::accessorType::AccessorType operatorLocationOfInterestClauseType = 1730;

         //  This accessor type is used to identify the uci::type::OperatorLocationOfInterestComparativeType Accessor
         static const uci::base::accessorType::AccessorType operatorLocationOfInterestComparativeType = 1731;

         //  This accessor type is used to identify the uci::type::OperatorLocationOfInterestDesignationID_Type Accessor
         static const uci::base::accessorType::AccessorType operatorLocationOfInterestDesignationID_Type = 1732;

         //  This accessor type is used to identify the uci::type::OperatorLocationOfInterestDesignationMDT Accessor
         static const uci::base::accessorType::AccessorType operatorLocationOfInterestDesignationMDT = 1733;

         //  This accessor type is used to identify the uci::type::OperatorLocationOfInterestDesignationMT Accessor
         static const uci::base::accessorType::AccessorType operatorLocationOfInterestDesignationMT = 1734;

         //  This accessor type is used to identify the uci::type::OperatorLocationOfInterestDesignationQFT Accessor
         static const uci::base::accessorType::AccessorType operatorLocationOfInterestDesignationQFT = 1735;

         //  This accessor type is used to identify the uci::type::OperatorLocationOfInterestID_Type Accessor
         static const uci::base::accessorType::AccessorType operatorLocationOfInterestID_Type = 1736;

         //  This accessor type is used to identify the uci::type::OperatorLocationOfInterestMDT Accessor
         static const uci::base::accessorType::AccessorType operatorLocationOfInterestMDT = 1737;

         //  This accessor type is used to identify the uci::type::OperatorLocationOfInterestMT Accessor
         static const uci::base::accessorType::AccessorType operatorLocationOfInterestMT = 1738;

         //  This accessor type is used to identify the uci::type::OperatorLocationOfInterestQFT Accessor
         static const uci::base::accessorType::AccessorType operatorLocationOfInterestQFT = 1739;

         //  This accessor type is used to identify the uci::type::OperatorMDT Accessor
         static const uci::base::accessorType::AccessorType operatorMDT = 1740;

         //  This accessor type is used to identify the uci::type::OperatorMT Accessor
         static const uci::base::accessorType::AccessorType operatorMT = 1741;

         //  This accessor type is used to identify the uci::type::OperatorNameType Accessor
         static const uci::base::accessorType::AccessorType operatorNameType = 1742;

         //  This accessor type is used to identify the uci::type::OperatorNotificationAckID_Type Accessor
         static const uci::base::accessorType::AccessorType operatorNotificationAckID_Type = 1743;

         //  This accessor type is used to identify the uci::type::OperatorNotificationAckMDT Accessor
         static const uci::base::accessorType::AccessorType operatorNotificationAckMDT = 1744;

         //  This accessor type is used to identify the uci::type::OperatorNotificationAckMT Accessor
         static const uci::base::accessorType::AccessorType operatorNotificationAckMT = 1745;

         //  This accessor type is used to identify the uci::type::OperatorNotificationAckQFT Accessor
         static const uci::base::accessorType::AccessorType operatorNotificationAckQFT = 1746;

         //  This accessor type is used to identify the uci::type::OperatorNotificationActionType Accessor
         static const uci::base::accessorType::AccessorType operatorNotificationActionType = 1747;

         //  This accessor type is used to identify the uci::type::OperatorNotificationID_Type Accessor
         static const uci::base::accessorType::AccessorType operatorNotificationID_Type = 1748;

         //  This accessor type is used to identify the uci::type::OperatorNotificationMDT Accessor
         static const uci::base::accessorType::AccessorType operatorNotificationMDT = 1749;

      } // Namespace: accessorType
      //  
      namespace operatorNotificationMDT {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::OperatorNotificationMDT::Priority Accessor
            static const uci::base::accessorType::AccessorType priority = 1750;

         } // Namespace: accessorType
      } // Namespace: operatorNotificationMDT
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::OperatorNotificationMT Accessor
         static const uci::base::accessorType::AccessorType operatorNotificationMT = 1751;

         //  This accessor type is used to identify the uci::type::OperatorNotificationOverrideMDT Accessor
         static const uci::base::accessorType::AccessorType operatorNotificationOverrideMDT = 1752;

         //  This accessor type is used to identify the uci::type::OperatorNotificationOverrideMT Accessor
         static const uci::base::accessorType::AccessorType operatorNotificationOverrideMT = 1753;

         //  This accessor type is used to identify the uci::type::OperatorNotificationOverrideQFT Accessor
         static const uci::base::accessorType::AccessorType operatorNotificationOverrideQFT = 1754;

         //  This accessor type is used to identify the uci::type::OperatorNotificationQFT Accessor
         static const uci::base::accessorType::AccessorType operatorNotificationQFT = 1755;

         //  This accessor type is used to identify the uci::type::OperatorQFT Accessor
         static const uci::base::accessorType::AccessorType operatorQFT = 1756;

         //  This accessor type is used to identify the uci::type::OperatorRecommendationType Accessor
         static const uci::base::accessorType::AccessorType operatorRecommendationType = 1757;

         //  This accessor type is used to identify the uci::type::OperatorRoleType Accessor
         static const uci::base::accessorType::AccessorType operatorRoleType = 1758;

         //  This accessor type is used to identify the uci::type::OpLineID_Type Accessor
         static const uci::base::accessorType::AccessorType opLineID_Type = 1759;

         //  This accessor type is used to identify the uci::type::OpLineMDT Accessor
         static const uci::base::accessorType::AccessorType opLineMDT = 1760;

         //  This accessor type is used to identify the uci::type::OpLineMT Accessor
         static const uci::base::accessorType::AccessorType opLineMT = 1761;

         //  This accessor type is used to identify the uci::type::OpLineQFT Accessor
         static const uci::base::accessorType::AccessorType opLineQFT = 1762;

         //  This accessor type is used to identify the uci::type::OpLineType Accessor
         static const uci::base::accessorType::AccessorType opLineType = 1763;

         //  This accessor type is used to identify the uci::type::OpPointBaseType Accessor
         static const uci::base::accessorType::AccessorType opPointBaseType = 1764;

         //  This accessor type is used to identify the uci::type::OpPointCategoriesType Accessor
         static const uci::base::accessorType::AccessorType opPointCategoriesType = 1765;

         //  This accessor type is used to identify the uci::type::OpPointID_Type Accessor
         static const uci::base::accessorType::AccessorType opPointID_Type = 1766;

         //  This accessor type is used to identify the uci::type::OpPointMDT Accessor
         static const uci::base::accessorType::AccessorType opPointMDT = 1767;

         //  This accessor type is used to identify the uci::type::OpPointMT Accessor
         static const uci::base::accessorType::AccessorType opPointMT = 1768;

         //  This accessor type is used to identify the uci::type::OpPointPositionType Accessor
         static const uci::base::accessorType::AccessorType opPointPositionType = 1769;

         //  This accessor type is used to identify the uci::type::OpPointQFT Accessor
         static const uci::base::accessorType::AccessorType opPointQFT = 1770;

         //  This accessor type is used to identify the uci::type::OpPointReferenceType Accessor
         static const uci::base::accessorType::AccessorType opPointReferenceType = 1771;

         //  This accessor type is used to identify the uci::type::OpRoutingID_Type Accessor
         static const uci::base::accessorType::AccessorType opRoutingID_Type = 1772;

         //  This accessor type is used to identify the uci::type::OpRoutingMDT Accessor
         static const uci::base::accessorType::AccessorType opRoutingMDT = 1773;

         //  This accessor type is used to identify the uci::type::OpRoutingMT Accessor
         static const uci::base::accessorType::AccessorType opRoutingMT = 1774;

         //  This accessor type is used to identify the uci::type::OpRoutingQFT Accessor
         static const uci::base::accessorType::AccessorType opRoutingQFT = 1775;

         //  This accessor type is used to identify the uci::type::OpsClockDayType Accessor
         static const uci::base::accessorType::AccessorType opsClockDayType = 1776;

         //  This accessor type is used to identify the uci::type::OpticalCollectionConstraintsType Accessor
         static const uci::base::accessorType::AccessorType opticalCollectionConstraintsType = 1777;

         //  This accessor type is used to identify the uci::type::OpZoneCategoryType Accessor
         static const uci::base::accessorType::AccessorType opZoneCategoryType = 1778;

         //  This accessor type is used to identify the uci::type::OpZoneFilterAreaPET Accessor
         static const uci::base::accessorType::AccessorType opZoneFilterAreaPET = 1779;

         //  This accessor type is used to identify the uci::type::OpZoneID_Type Accessor
         static const uci::base::accessorType::AccessorType opZoneID_Type = 1780;

         //  This accessor type is used to identify the uci::type::OpZoneJammingType Accessor
         static const uci::base::accessorType::AccessorType opZoneJammingType = 1781;

         //  This accessor type is used to identify the uci::type::OpZoneMDT Accessor
         static const uci::base::accessorType::AccessorType opZoneMDT = 1782;

         //  This accessor type is used to identify the uci::type::OpZoneMT Accessor
         static const uci::base::accessorType::AccessorType opZoneMT = 1783;

         //  This accessor type is used to identify the uci::type::OpZoneNoFireType Accessor
         static const uci::base::accessorType::AccessorType opZoneNoFireType = 1784;

         //  This accessor type is used to identify the uci::type::OpZoneNoFlyType Accessor
         static const uci::base::accessorType::AccessorType opZoneNoFlyType = 1785;

         //  This accessor type is used to identify the uci::type::OpZoneQFT Accessor
         static const uci::base::accessorType::AccessorType opZoneQFT = 1786;

         //  This accessor type is used to identify the uci::type::OpZoneType Accessor
         static const uci::base::accessorType::AccessorType opZoneType = 1787;

         //  This accessor type is used to identify the uci::type::OpZoneWeaponRestrictionType Accessor
         static const uci::base::accessorType::AccessorType opZoneWeaponRestrictionType = 1788;

         //  This accessor type is used to identify the uci::type::OpZoneWeatherType Accessor
         static const uci::base::accessorType::AccessorType opZoneWeatherType = 1789;

         //  This accessor type is used to identify the uci::type::OrbitalDeltaVelocity_B_Type Accessor
         static const uci::base::accessorType::AccessorType orbitalDeltaVelocity_B_Type = 1790;

         //  This accessor type is used to identify the uci::type::OrbitalKinematicsAccelerationType Accessor
         static const uci::base::accessorType::AccessorType orbitalKinematicsAccelerationType = 1791;

         //  This accessor type is used to identify the uci::type::OrbitalKinematicsChoiceType Accessor
         static const uci::base::accessorType::AccessorType orbitalKinematicsChoiceType = 1792;

         //  This accessor type is used to identify the uci::type::OrbitalKinematicsObjectRelativeType Accessor
         static const uci::base::accessorType::AccessorType orbitalKinematicsObjectRelativeType = 1793;

         //  This accessor type is used to identify the uci::type::OrbitalKinematicsPositionType Accessor
         static const uci::base::accessorType::AccessorType orbitalKinematicsPositionType = 1794;

         //  This accessor type is used to identify the uci::type::OrbitalKinematicsStandardFrameChoiceType Accessor
         static const uci::base::accessorType::AccessorType orbitalKinematicsStandardFrameChoiceType = 1795;

         //  This accessor type is used to identify the uci::type::OrbitalKinematicsVelocityType Accessor
         static const uci::base::accessorType::AccessorType orbitalKinematicsVelocityType = 1796;

         //  This accessor type is used to identify the uci::type::OrbitalManeuverDetailsBaseType Accessor
         static const uci::base::accessorType::AccessorType orbitalManeuverDetailsBaseType = 1797;

         //  This accessor type is used to identify the uci::type::OrbitalPositionVelocityCovarianceType Accessor
         static const uci::base::accessorType::AccessorType orbitalPositionVelocityCovarianceType = 1798;

         //  This accessor type is used to identify the uci::type::OrbitalSurveillanceTargetType Accessor
         static const uci::base::accessorType::AccessorType orbitalSurveillanceTargetType = 1799;

         //  This accessor type is used to identify the uci::type::OrbitalVolumeType Accessor
         static const uci::base::accessorType::AccessorType orbitalVolumeType = 1800;

         //  This accessor type is used to identify the uci::type::OrbitDurationType Accessor
         static const uci::base::accessorType::AccessorType orbitDurationType = 1801;

         //  This accessor type is used to identify the uci::type::OrbitSurveillanceAssetVolumeType Accessor
         static const uci::base::accessorType::AccessorType orbitSurveillanceAssetVolumeType = 1802;

         //  This accessor type is used to identify the uci::type::OrbitType Accessor
         static const uci::base::accessorType::AccessorType orbitType = 1803;

         //  This accessor type is used to identify the uci::type::OrderOfBattleEmitterType Accessor
         static const uci::base::accessorType::AccessorType orderOfBattleEmitterType = 1804;

      } // Namespace: accessorType
      //  
      namespace orderOfBattleEmitterType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::OrderOfBattleEmitterType::LastCollector Accessor
            static const uci::base::accessorType::AccessorType lastCollector = 1805;

            //  This accessor type is used to identify the uci::type::OrderOfBattleEmitterType::RecordOwner Accessor
            static const uci::base::accessorType::AccessorType recordOwner = 1806;

         } // Namespace: accessorType
      } // Namespace: orderOfBattleEmitterType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::OrderOfBattleMDT Accessor
         static const uci::base::accessorType::AccessorType orderOfBattleMDT = 1807;

         //  This accessor type is used to identify the uci::type::OrderOfBattleMT Accessor
         static const uci::base::accessorType::AccessorType orderOfBattleMT = 1808;

         //  This accessor type is used to identify the uci::type::OrderOfBattleQFT Accessor
         static const uci::base::accessorType::AccessorType orderOfBattleQFT = 1809;

         //  This accessor type is used to identify the uci::type::OrderOfBattleTimestampsType Accessor
         static const uci::base::accessorType::AccessorType orderOfBattleTimestampsType = 1810;

         //  This accessor type is used to identify the uci::type::OrientationAccelerationType Accessor
         static const uci::base::accessorType::AccessorType orientationAccelerationType = 1811;

         //  This accessor type is used to identify the uci::type::OrientationCovarianceType Accessor
         static const uci::base::accessorType::AccessorType orientationCovarianceType = 1812;

         //  This accessor type is used to identify the uci::type::OrientationRateType Accessor
         static const uci::base::accessorType::AccessorType orientationRateType = 1813;

         //  This accessor type is used to identify the uci::type::OrientationType Accessor
         static const uci::base::accessorType::AccessorType orientationType = 1814;

         //  This accessor type is used to identify the uci::type::ParameterAssertType Accessor
         static const uci::base::accessorType::AccessorType parameterAssertType = 1815;

         //  This accessor type is used to identify the uci::type::ParameterID_Type Accessor
         static const uci::base::accessorType::AccessorType parameterID_Type = 1816;

         //  This accessor type is used to identify the uci::type::ParameterValueType Accessor
         static const uci::base::accessorType::AccessorType parameterValueType = 1817;

         //  This accessor type is used to identify the uci::type::PATCHB_Type Accessor
         static const uci::base::accessorType::AccessorType pATCHB_Type = 1818;

      } // Namespace: accessorType
      //  
      namespace pATCHB_Type {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::PATCHB_Type::GRAVITY Accessor
            static const uci::base::accessorType::AccessorType gRAVITY = 1819;

         } // Namespace: accessorType
      } // Namespace: pATCHB_Type
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::PathConstraintsType Accessor
         static const uci::base::accessorType::AccessorType pathConstraintsType = 1820;

         //  This accessor type is used to identify the uci::type::PathID_Type Accessor
         static const uci::base::accessorType::AccessorType pathID_Type = 1821;

         //  This accessor type is used to identify the uci::type::PathSegmentActionTriggerType Accessor
         static const uci::base::accessorType::AccessorType pathSegmentActionTriggerType = 1822;

         //  This accessor type is used to identify the uci::type::PathSegmentActionType Accessor
         static const uci::base::accessorType::AccessorType pathSegmentActionType = 1823;

         //  This accessor type is used to identify the uci::type::PathSegmentConditionType Accessor
         static const uci::base::accessorType::AccessorType pathSegmentConditionType = 1824;

         //  This accessor type is used to identify the uci::type::PathSegmentSpeedType Accessor
         static const uci::base::accessorType::AccessorType pathSegmentSpeedType = 1825;

         //  This accessor type is used to identify the uci::type::PathSegmentSpeedValueType Accessor
         static const uci::base::accessorType::AccessorType pathSegmentSpeedValueType = 1826;

         //  This accessor type is used to identify the uci::type::PathSegmentThreatAssessmentType Accessor
         static const uci::base::accessorType::AccessorType pathSegmentThreatAssessmentType = 1827;

         //  This accessor type is used to identify the uci::type::PathSegmentTriggerType Accessor
         static const uci::base::accessorType::AccessorType pathSegmentTriggerType = 1828;

         //  This accessor type is used to identify the uci::type::PathSegmentType Accessor
         static const uci::base::accessorType::AccessorType pathSegmentType = 1829;

         //  This accessor type is used to identify the uci::type::PathThreatAssessmentType Accessor
         static const uci::base::accessorType::AccessorType pathThreatAssessmentType = 1830;

         //  This accessor type is used to identify the uci::type::PathType Accessor
         static const uci::base::accessorType::AccessorType pathType = 1831;

         //  This accessor type is used to identify the uci::type::PathVulnerabilitySegmentType Accessor
         static const uci::base::accessorType::AccessorType pathVulnerabilitySegmentType = 1832;

         //  This accessor type is used to identify the uci::type::PathVulnerabilityType Accessor
         static const uci::base::accessorType::AccessorType pathVulnerabilityType = 1833;

         //  This accessor type is used to identify the uci::type::PercentRangeType Accessor
         static const uci::base::accessorType::AccessorType percentRangeType = 1834;

         //  This accessor type is used to identify the uci::type::PeriodPerformanceFeedbackType Accessor
         static const uci::base::accessorType::AccessorType periodPerformanceFeedbackType = 1835;

         //  This accessor type is used to identify the uci::type::PixelCountSettingsType Accessor
         static const uci::base::accessorType::AccessorType pixelCountSettingsType = 1836;

         //  This accessor type is used to identify the uci::type::PixelDataType Accessor
         static const uci::base::accessorType::AccessorType pixelDataType = 1837;

         //  This accessor type is used to identify the uci::type::PixelOutputDataType Accessor
         static const uci::base::accessorType::AccessorType pixelOutputDataType = 1838;

         //  This accessor type is used to identify the uci::type::PixelReferenceCoordinateType Accessor
         static const uci::base::accessorType::AccessorType pixelReferenceCoordinateType = 1839;

         //  This accessor type is used to identify the uci::type::PixelScalingSettingsType Accessor
         static const uci::base::accessorType::AccessorType pixelScalingSettingsType = 1840;

         //  This accessor type is used to identify the uci::type::PlannedCapabilityType Accessor
         static const uci::base::accessorType::AccessorType plannedCapabilityType = 1841;

         //  This accessor type is used to identify the uci::type::PlannedCommActionExtendedType Accessor
         static const uci::base::accessorType::AccessorType plannedCommActionExtendedType = 1842;

         //  This accessor type is used to identify the uci::type::PlannedCommActionType Accessor
         static const uci::base::accessorType::AccessorType plannedCommActionType = 1843;

         //  This accessor type is used to identify the uci::type::PlannedCommCapabilityType Accessor
         static const uci::base::accessorType::AccessorType plannedCommCapabilityType = 1844;

         //  This accessor type is used to identify the uci::type::PlannedGatewayConfigurationType Accessor
         static const uci::base::accessorType::AccessorType plannedGatewayConfigurationType = 1845;

         //  This accessor type is used to identify the uci::type::PlannedReleaseConditionsType Accessor
         static const uci::base::accessorType::AccessorType plannedReleaseConditionsType = 1846;

         //  This accessor type is used to identify the uci::type::PlannedSubsystemCommandType Accessor
         static const uci::base::accessorType::AccessorType plannedSubsystemCommandType = 1847;

         //  This accessor type is used to identify the uci::type::PlanningAllowedEscalationType Accessor
         static const uci::base::accessorType::AccessorType planningAllowedEscalationType = 1848;

         //  This accessor type is used to identify the uci::type::PlanningAllowedType Accessor
         static const uci::base::accessorType::AccessorType planningAllowedType = 1849;

         //  This accessor type is used to identify the uci::type::PlanningChoiceType Accessor
         static const uci::base::accessorType::AccessorType planningChoiceType = 1850;

         //  This accessor type is used to identify the uci::type::PlanningGuidelineType Accessor
         static const uci::base::accessorType::AccessorType planningGuidelineType = 1851;

         //  This accessor type is used to identify the uci::type::PlanningLocationType Accessor
         static const uci::base::accessorType::AccessorType planningLocationType = 1852;

         //  This accessor type is used to identify the uci::type::PlanningOriginType Accessor
         static const uci::base::accessorType::AccessorType planningOriginType = 1853;

         //  This accessor type is used to identify the uci::type::PlanningResultApprovalPolicyType Accessor
         static const uci::base::accessorType::AccessorType planningResultApprovalPolicyType = 1854;

         //  This accessor type is used to identify the uci::type::PlatformFunctionStatusCategoryType Accessor
         static const uci::base::accessorType::AccessorType platformFunctionStatusCategoryType = 1855;

         //  This accessor type is used to identify the uci::type::PlatformFunctionStatusType Accessor
         static const uci::base::accessorType::AccessorType platformFunctionStatusType = 1856;

         //  This accessor type is used to identify the uci::type::PlatformIdentityConfidenceType Accessor
         static const uci::base::accessorType::AccessorType platformIdentityConfidenceType = 1857;

         //  This accessor type is used to identify the uci::type::PlatformIdentityType Accessor
         static const uci::base::accessorType::AccessorType platformIdentityType = 1858;

         //  This accessor type is used to identify the uci::type::PlatformStatusSAM_Type Accessor
         static const uci::base::accessorType::AccessorType platformStatusSAM_Type = 1859;

         //  This accessor type is used to identify the uci::type::PlatformStatusType Accessor
         static const uci::base::accessorType::AccessorType platformStatusType = 1860;

         //  This accessor type is used to identify the uci::type::PMOP_SequenceType Accessor
         static const uci::base::accessorType::AccessorType pMOP_SequenceType = 1861;

         //  This accessor type is used to identify the uci::type::PO_ActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType pO_ActivityCommandType = 1862;

         //  This accessor type is used to identify the uci::type::PO_ActivityMDT Accessor
         static const uci::base::accessorType::AccessorType pO_ActivityMDT = 1863;

         //  This accessor type is used to identify the uci::type::PO_ActivityMT Accessor
         static const uci::base::accessorType::AccessorType pO_ActivityMT = 1864;

         //  This accessor type is used to identify the uci::type::PO_ActivityQFT Accessor
         static const uci::base::accessorType::AccessorType pO_ActivityQFT = 1865;

         //  This accessor type is used to identify the uci::type::PO_ActivityType Accessor
         static const uci::base::accessorType::AccessorType pO_ActivityType = 1866;

         //  This accessor type is used to identify the uci::type::PO_AngleConstraintControlsStatusType Accessor
         static const uci::base::accessorType::AccessorType pO_AngleConstraintControlsStatusType = 1867;

         //  This accessor type is used to identify the uci::type::PO_AngleConstraintControlsType Accessor
         static const uci::base::accessorType::AccessorType pO_AngleConstraintControlsType = 1868;

         //  This accessor type is used to identify the uci::type::PO_CapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType pO_CapabilityCommandType = 1869;

         //  This accessor type is used to identify the uci::type::PO_CapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType pO_CapabilityMDT = 1870;

         //  This accessor type is used to identify the uci::type::PO_CapabilityMT Accessor
         static const uci::base::accessorType::AccessorType pO_CapabilityMT = 1871;

         //  This accessor type is used to identify the uci::type::PO_CapabilityOptionsType Accessor
         static const uci::base::accessorType::AccessorType pO_CapabilityOptionsType = 1872;

         //  This accessor type is used to identify the uci::type::PO_CapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType pO_CapabilityQFT = 1873;

         //  This accessor type is used to identify the uci::type::PO_CapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType pO_CapabilityStatusMDT = 1874;

         //  This accessor type is used to identify the uci::type::PO_CapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType pO_CapabilityStatusMT = 1875;

         //  This accessor type is used to identify the uci::type::PO_CapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType pO_CapabilityStatusQFT = 1876;

         //  This accessor type is used to identify the uci::type::PO_CapabilityType Accessor
         static const uci::base::accessorType::AccessorType pO_CapabilityType = 1877;

         //  This accessor type is used to identify the uci::type::PO_CollectionConstraintsSettingsType Accessor
         static const uci::base::accessorType::AccessorType pO_CollectionConstraintsSettingsType = 1878;

         //  This accessor type is used to identify the uci::type::PO_CollectionConstraintsStatusType Accessor
         static const uci::base::accessorType::AccessorType pO_CollectionConstraintsStatusType = 1879;

         //  This accessor type is used to identify the uci::type::PO_CollectionConstraintsType Accessor
         static const uci::base::accessorType::AccessorType pO_CollectionConstraintsType = 1880;

         //  This accessor type is used to identify the uci::type::PO_CollectionPatternConstraintControlsType Accessor
         static const uci::base::accessorType::AccessorType pO_CollectionPatternConstraintControlsType = 1881;

         //  This accessor type is used to identify the uci::type::PO_ColorPatternConstraintControlsStatusType Accessor
         static const uci::base::accessorType::AccessorType pO_ColorPatternConstraintControlsStatusType = 1882;

         //  This accessor type is used to identify the uci::type::PO_CommandMDT Accessor
         static const uci::base::accessorType::AccessorType pO_CommandMDT = 1883;

         //  This accessor type is used to identify the uci::type::PO_CommandMT Accessor
         static const uci::base::accessorType::AccessorType pO_CommandMT = 1884;

         //  This accessor type is used to identify the uci::type::PO_CommandQFT Accessor
         static const uci::base::accessorType::AccessorType pO_CommandQFT = 1885;

         //  This accessor type is used to identify the uci::type::PO_CommandResponseType Accessor
         static const uci::base::accessorType::AccessorType pO_CommandResponseType = 1886;

         //  This accessor type is used to identify the uci::type::PO_CommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType pO_CommandStatusMT = 1887;

         //  This accessor type is used to identify the uci::type::PO_CommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType pO_CommandStatusQFT = 1888;

         //  This accessor type is used to identify the uci::type::PO_CommandType Accessor
         static const uci::base::accessorType::AccessorType pO_CommandType = 1889;

         //  This accessor type is used to identify the uci::type::PO_ComponentFilterType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentFilterType = 1890;

         //  This accessor type is used to identify the uci::type::PO_ComponentFocalPlaneArrayChannelType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentFocalPlaneArrayChannelType = 1891;

         //  This accessor type is used to identify the uci::type::PO_ComponentFocalPlaneArrayOperationType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentFocalPlaneArrayOperationType = 1892;

         //  This accessor type is used to identify the uci::type::PO_ComponentFocalPlaneArrayOpticalBandType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentFocalPlaneArrayOpticalBandType = 1893;

         //  This accessor type is used to identify the uci::type::PO_ComponentFocalPlaneArrayType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentFocalPlaneArrayType = 1894;

         //  This accessor type is used to identify the uci::type::PO_ComponentLensAssemblyFieldOfViewType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentLensAssemblyFieldOfViewType = 1895;

         //  This accessor type is used to identify the uci::type::PO_ComponentLensAssemblyFocusType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentLensAssemblyFocusType = 1896;

         //  This accessor type is used to identify the uci::type::PO_ComponentLensAssemblyType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentLensAssemblyType = 1897;

         //  This accessor type is used to identify the uci::type::PO_ComponentLensAssemblyZoomType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentLensAssemblyZoomType = 1898;

         //  This accessor type is used to identify the uci::type::PO_ComponentProcessingStageType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentProcessingStageType = 1899;

         //  This accessor type is used to identify the uci::type::PO_ComponentProductGeneratorOutputType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentProductGeneratorOutputType = 1900;

         //  This accessor type is used to identify the uci::type::PO_ComponentProductGeneratorType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentProductGeneratorType = 1901;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsBandpassFrequencyType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsBandpassFrequencyType = 1902;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsFilterType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsFilterType = 1903;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsFocalPlaneArrayChannelType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsFocalPlaneArrayChannelType = 1904;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsFocalPlaneArrayCollectionType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsFocalPlaneArrayCollectionType = 1905;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsFocalPlaneArrayNonUniformityCorrectionType
         //  Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsFocalPlaneArrayNonUniformityCorrectionType = 1906;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsFocalPlaneArrayOpticalBandType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsFocalPlaneArrayOpticalBandType = 1907;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsFocalPlaneArrayPixelAggregationType
         //  Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsFocalPlaneArrayPixelAggregationType = 1908;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsFocalPlaneArrayPixelPolarityType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsFocalPlaneArrayPixelPolarityType = 1909;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsFocalPlaneArrayPixelScalingSettingsType
         //  Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsFocalPlaneArrayPixelScalingSettingsType = 1910;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsFocalPlaneArrayPixelScalingType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsFocalPlaneArrayPixelScalingType = 1911;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsFocalPlaneArrayScanDirectionType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsFocalPlaneArrayScanDirectionType = 1912;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsFocalPlaneArrayTimeDelayIntegrationType
         //  Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsFocalPlaneArrayTimeDelayIntegrationType = 1913;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsFocalPlaneArrayType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsFocalPlaneArrayType = 1914;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsFocusSweepSettingsType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsFocusSweepSettingsType = 1915;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsGeneratorSettingsType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsGeneratorSettingsType = 1916;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsLensAssemblyFieldOfViewType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsLensAssemblyFieldOfViewType = 1917;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsLensAssemblyFocusType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsLensAssemblyFocusType = 1918;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsLensAssemblyType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsLensAssemblyType = 1919;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsOutputProductSettingsType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsOutputProductSettingsType = 1920;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsProcessingStagePixelScalingType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsProcessingStagePixelScalingType = 1921;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsProcessingStageProcessingSettingsType
         //  Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsProcessingStageProcessingSettingsType = 1922;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsProcessingStageSettingsType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsProcessingStageSettingsType = 1923;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsProcessingStageType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsProcessingStageType = 1924;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsProductGeneratorSettingsType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsProductGeneratorSettingsType = 1925;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsProductGeneratorType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsProductGeneratorType = 1926;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsProductSettingsType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsProductSettingsType = 1927;

         //  This accessor type is used to identify the uci::type::PO_ComponentSettingsType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentSettingsType = 1928;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusBandpassFrequencyType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusBandpassFrequencyType = 1929;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusFilterType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusFilterType = 1930;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusFocalPlaneArrayChannelType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusFocalPlaneArrayChannelType = 1931;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusFocalPlaneArrayCollectionTimeType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusFocalPlaneArrayCollectionTimeType = 1932;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusFocalPlaneArrayNonUniformityCorrectionType
         //  Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusFocalPlaneArrayNonUniformityCorrectionType = 1933;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusFocalPlaneArrayOpticalBandType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusFocalPlaneArrayOpticalBandType = 1934;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusFocalPlaneArrayPixelAggregationType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusFocalPlaneArrayPixelAggregationType = 1935;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusFocalPlaneArrayPixelPolarityType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusFocalPlaneArrayPixelPolarityType = 1936;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusFocalPlaneArrayPixelScalingType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusFocalPlaneArrayPixelScalingType = 1937;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusFocalPlaneArrayScanDirectionType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusFocalPlaneArrayScanDirectionType = 1938;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType
         //  Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType = 1939;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusFocalPlaneArrayType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusFocalPlaneArrayType = 1940;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusFocusSweepSettingsType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusFocusSweepSettingsType = 1941;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusGeneratorSettingsType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusGeneratorSettingsType = 1942;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusLensAssemblyFieldOfViewType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusLensAssemblyFieldOfViewType = 1943;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusLensAssemblyFocusType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusLensAssemblyFocusType = 1944;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusLensAssemblyType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusLensAssemblyType = 1945;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusOutputProductSettingsType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusOutputProductSettingsType = 1946;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusProcessingStagePixelScalingType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusProcessingStagePixelScalingType = 1947;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusProcessingStageProcessingSettingsType
         //  Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusProcessingStageProcessingSettingsType = 1948;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusProcessingStageSettingsType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusProcessingStageSettingsType = 1949;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusProcessingStageType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusProcessingStageType = 1950;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusProductGeneratorSettingsType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusProductGeneratorSettingsType = 1951;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusProductGeneratorType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusProductGeneratorType = 1952;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusProductSettingsType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusProductSettingsType = 1953;

         //  This accessor type is used to identify the uci::type::PO_ComponentStatusType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentStatusType = 1954;

         //  This accessor type is used to identify the uci::type::PO_ComponentType Accessor
         static const uci::base::accessorType::AccessorType pO_ComponentType = 1955;

         //  This accessor type is used to identify the uci::type::PO_ConstraintControlsType Accessor
         static const uci::base::accessorType::AccessorType pO_ConstraintControlsType = 1956;

         //  This accessor type is used to identify the uci::type::PO_FilterID_Type Accessor
         static const uci::base::accessorType::AccessorType pO_FilterID_Type = 1957;

         //  This accessor type is used to identify the uci::type::PO_FPA_ChannelID_Type Accessor
         static const uci::base::accessorType::AccessorType pO_FPA_ChannelID_Type = 1958;

         //  This accessor type is used to identify the uci::type::PO_FPA_ID_Type Accessor
         static const uci::base::accessorType::AccessorType pO_FPA_ID_Type = 1959;

         //  This accessor type is used to identify the uci::type::PO_LensID_Type Accessor
         static const uci::base::accessorType::AccessorType pO_LensID_Type = 1960;

         //  This accessor type is used to identify the uci::type::PO_ProcessingStageID_Type Accessor
         static const uci::base::accessorType::AccessorType pO_ProcessingStageID_Type = 1961;

         //  This accessor type is used to identify the uci::type::PO_ProductGeneratorID_Type Accessor
         static const uci::base::accessorType::AccessorType pO_ProductGeneratorID_Type = 1962;

         //  This accessor type is used to identify the uci::type::PO_ProductGeneratorOutputID_Type Accessor
         static const uci::base::accessorType::AccessorType pO_ProductGeneratorOutputID_Type = 1963;

         //  This accessor type is used to identify the uci::type::PO_SettingsCommandMDT Accessor
         static const uci::base::accessorType::AccessorType pO_SettingsCommandMDT = 1964;

         //  This accessor type is used to identify the uci::type::PO_SettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType pO_SettingsCommandMT = 1965;

         //  This accessor type is used to identify the uci::type::PO_SettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType pO_SettingsCommandQFT = 1966;

         //  This accessor type is used to identify the uci::type::PO_SettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType pO_SettingsCommandStatusMT = 1967;

         //  This accessor type is used to identify the uci::type::PO_SettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType pO_SettingsCommandStatusQFT = 1968;

         //  This accessor type is used to identify the uci::type::PO_SlantRangeConstraintControlsStatusType Accessor
         static const uci::base::accessorType::AccessorType pO_SlantRangeConstraintControlsStatusType = 1969;

         //  This accessor type is used to identify the uci::type::PO_SlantRangeConstraintControlsType Accessor
         static const uci::base::accessorType::AccessorType pO_SlantRangeConstraintControlsType = 1970;

         //  This accessor type is used to identify the uci::type::PO_SpecificDataType Accessor
         static const uci::base::accessorType::AccessorType pO_SpecificDataType = 1971;

         //  This accessor type is used to identify the uci::type::PO_SweepSpeedConstraintControlsStatusType Accessor
         static const uci::base::accessorType::AccessorType pO_SweepSpeedConstraintControlsStatusType = 1972;

         //  This accessor type is used to identify the uci::type::PO_SweepSpeedConstraintControlsType Accessor
         static const uci::base::accessorType::AccessorType pO_SweepSpeedConstraintControlsType = 1973;

         //  This accessor type is used to identify the uci::type::PO_TargetType Accessor
         static const uci::base::accessorType::AccessorType pO_TargetType = 1974;

         //  This accessor type is used to identify the uci::type::PO_TaskBaseType Accessor
         static const uci::base::accessorType::AccessorType pO_TaskBaseType = 1975;

         //  This accessor type is used to identify the uci::type::PO_TaskType Accessor
         static const uci::base::accessorType::AccessorType pO_TaskType = 1976;

         //  This accessor type is used to identify the uci::type::Point2D_ReferenceType Accessor
         static const uci::base::accessorType::AccessorType point2D_ReferenceType = 1977;

         //  This accessor type is used to identify the uci::type::Point2D_ReportedType Accessor
         static const uci::base::accessorType::AccessorType point2D_ReportedType = 1978;

         //  This accessor type is used to identify the uci::type::Point2D_Type Accessor
         static const uci::base::accessorType::AccessorType point2D_Type = 1979;

         //  This accessor type is used to identify the uci::type::Point3D_Type Accessor
         static const uci::base::accessorType::AccessorType point3D_Type = 1980;

         //  This accessor type is used to identify the uci::type::Point4D_Type Accessor
         static const uci::base::accessorType::AccessorType point4D_Type = 1981;

         //  This accessor type is used to identify the uci::type::PointingPlanWindowType Accessor
         static const uci::base::accessorType::AccessorType pointingPlanWindowType = 1982;

         //  This accessor type is used to identify the uci::type::PointingType Accessor
         static const uci::base::accessorType::AccessorType pointingType = 1983;

         //  This accessor type is used to identify the uci::type::PointingVectorParameterType Accessor
         static const uci::base::accessorType::AccessorType pointingVectorParameterType = 1984;

         //  This accessor type is used to identify the uci::type::PointTargetType Accessor
         static const uci::base::accessorType::AccessorType pointTargetType = 1985;

         //  This accessor type is used to identify the uci::type::PolarizationType Accessor
         static const uci::base::accessorType::AccessorType polarizationType = 1986;

         //  This accessor type is used to identify the uci::type::PolygonType Accessor
         static const uci::base::accessorType::AccessorType polygonType = 1987;

         //  This accessor type is used to identify the uci::type::PositionAndVelocityCovarianceType Accessor
         static const uci::base::accessorType::AccessorType positionAndVelocityCovarianceType = 1988;

         //  This accessor type is used to identify the uci::type::PositionLocationUncertaintyType Accessor
         static const uci::base::accessorType::AccessorType positionLocationUncertaintyType = 1989;

         //  This accessor type is used to identify the uci::type::PositionOrientationCovarianceType Accessor
         static const uci::base::accessorType::AccessorType positionOrientationCovarianceType = 1990;

         //  This accessor type is used to identify the uci::type::PositionPositionCovarianceType Accessor
         static const uci::base::accessorType::AccessorType positionPositionCovarianceType = 1991;

         //  This accessor type is used to identify the uci::type::PositionReportDetailedMDT Accessor
         static const uci::base::accessorType::AccessorType positionReportDetailedMDT = 1992;

         //  This accessor type is used to identify the uci::type::PositionReportDetailedMT Accessor
         static const uci::base::accessorType::AccessorType positionReportDetailedMT = 1993;

         //  This accessor type is used to identify the uci::type::PositionReportDetailedQFT Accessor
         static const uci::base::accessorType::AccessorType positionReportDetailedQFT = 1994;

         //  This accessor type is used to identify the uci::type::PositionReportMDT Accessor
         static const uci::base::accessorType::AccessorType positionReportMDT = 1995;

         //  This accessor type is used to identify the uci::type::PositionReportMT Accessor
         static const uci::base::accessorType::AccessorType positionReportMT = 1996;

         //  This accessor type is used to identify the uci::type::PositionReportQFT Accessor
         static const uci::base::accessorType::AccessorType positionReportQFT = 1997;

         //  This accessor type is used to identify the uci::type::PositionVelocityCovarianceType Accessor
         static const uci::base::accessorType::AccessorType positionVelocityCovarianceType = 1998;

         //  This accessor type is used to identify the uci::type::POST_ActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType pOST_ActivityCommandType = 1999;

         //  This accessor type is used to identify the uci::type::POST_ActivityMDT Accessor
         static const uci::base::accessorType::AccessorType pOST_ActivityMDT = 2000;

         //  This accessor type is used to identify the uci::type::POST_ActivityMT Accessor
         static const uci::base::accessorType::AccessorType pOST_ActivityMT = 2001;

         //  This accessor type is used to identify the uci::type::POST_ActivityQFT Accessor
         static const uci::base::accessorType::AccessorType pOST_ActivityQFT = 2002;

         //  This accessor type is used to identify the uci::type::POST_ActivityType Accessor
         static const uci::base::accessorType::AccessorType pOST_ActivityType = 2003;

         //  This accessor type is used to identify the uci::type::POST_AirTargetVolumeCommandType Accessor
         static const uci::base::accessorType::AccessorType pOST_AirTargetVolumeCommandType = 2004;

         //  This accessor type is used to identify the uci::type::POST_AirTargetVolumeType Accessor
         static const uci::base::accessorType::AccessorType pOST_AirTargetVolumeType = 2005;

         //  This accessor type is used to identify the uci::type::POST_AirVolumeSensorReferencedType Accessor
         static const uci::base::accessorType::AccessorType pOST_AirVolumeSensorReferencedType = 2006;

         //  This accessor type is used to identify the uci::type::POST_ApertureID_Type Accessor
         static const uci::base::accessorType::AccessorType pOST_ApertureID_Type = 2007;

         //  This accessor type is used to identify the uci::type::POST_CapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType pOST_CapabilityCommandType = 2008;

         //  This accessor type is used to identify the uci::type::POST_CapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType pOST_CapabilityMDT = 2009;

         //  This accessor type is used to identify the uci::type::POST_CapabilityMT Accessor
         static const uci::base::accessorType::AccessorType pOST_CapabilityMT = 2010;

         //  This accessor type is used to identify the uci::type::POST_CapabilityOptionsType Accessor
         static const uci::base::accessorType::AccessorType pOST_CapabilityOptionsType = 2011;

         //  This accessor type is used to identify the uci::type::POST_CapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType pOST_CapabilityQFT = 2012;

         //  This accessor type is used to identify the uci::type::POST_CapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType pOST_CapabilityStatusMDT = 2013;

         //  This accessor type is used to identify the uci::type::POST_CapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType pOST_CapabilityStatusMT = 2014;

         //  This accessor type is used to identify the uci::type::POST_CapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType pOST_CapabilityStatusQFT = 2015;

         //  This accessor type is used to identify the uci::type::POST_CapabilityStatusType Accessor
         static const uci::base::accessorType::AccessorType pOST_CapabilityStatusType = 2016;

         //  This accessor type is used to identify the uci::type::POST_CapabilityType Accessor
         static const uci::base::accessorType::AccessorType pOST_CapabilityType = 2017;

         //  This accessor type is used to identify the uci::type::POST_CoastSettingOptionsType Accessor
         static const uci::base::accessorType::AccessorType pOST_CoastSettingOptionsType = 2018;

         //  This accessor type is used to identify the uci::type::POST_CoastSettingValueType Accessor
         static const uci::base::accessorType::AccessorType pOST_CoastSettingValueType = 2019;

         //  This accessor type is used to identify the uci::type::POST_CommandMDT Accessor
         static const uci::base::accessorType::AccessorType pOST_CommandMDT = 2020;

         //  This accessor type is used to identify the uci::type::POST_CommandMT Accessor
         static const uci::base::accessorType::AccessorType pOST_CommandMT = 2021;

         //  This accessor type is used to identify the uci::type::POST_CommandQFT Accessor
         static const uci::base::accessorType::AccessorType pOST_CommandQFT = 2022;

         //  This accessor type is used to identify the uci::type::POST_CommandRepetitionType Accessor
         static const uci::base::accessorType::AccessorType pOST_CommandRepetitionType = 2023;

         //  This accessor type is used to identify the uci::type::POST_CommandResponseType Accessor
         static const uci::base::accessorType::AccessorType pOST_CommandResponseType = 2024;

         //  This accessor type is used to identify the uci::type::POST_CommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType pOST_CommandStatusMT = 2025;

         //  This accessor type is used to identify the uci::type::POST_CommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType pOST_CommandStatusQFT = 2026;

         //  This accessor type is used to identify the uci::type::POST_CommandType Accessor
         static const uci::base::accessorType::AccessorType pOST_CommandType = 2027;

         //  This accessor type is used to identify the uci::type::POST_ComponentApertureSettingsType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentApertureSettingsType = 2028;

         //  This accessor type is used to identify the uci::type::POST_ComponentApertureType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentApertureType = 2029;

         //  This accessor type is used to identify the uci::type::POST_ComponentF_StopSettingsType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentF_StopSettingsType = 2030;

         //  This accessor type is used to identify the uci::type::POST_ComponentF_StopType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentF_StopType = 2031;

         //  This accessor type is used to identify the uci::type::POST_ComponentF_StopVariableType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentF_StopVariableType = 2032;

         //  This accessor type is used to identify the uci::type::POST_ComponentFilterType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentFilterType = 2033;

         //  This accessor type is used to identify the uci::type::POST_ComponentFocalPlaneArrayChannelType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentFocalPlaneArrayChannelType = 2034;

         //  This accessor type is used to identify the uci::type::POST_ComponentFocalPlaneArrayOperationType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentFocalPlaneArrayOperationType = 2035;

         //  This accessor type is used to identify the uci::type::POST_ComponentFocalPlaneArrayOpticalBandType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentFocalPlaneArrayOpticalBandType = 2036;

         //  This accessor type is used to identify the uci::type::POST_ComponentFocalPlaneArrayType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentFocalPlaneArrayType = 2037;

         //  This accessor type is used to identify the uci::type::POST_ComponentProcessingStageType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentProcessingStageType = 2038;

         //  This accessor type is used to identify the uci::type::POST_ComponentProductGeneratorType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentProductGeneratorType = 2039;

         //  This accessor type is used to identify the uci::type::POST_ComponentSettingsFilterType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentSettingsFilterType = 2040;

         //  This accessor type is used to identify the uci::type::POST_ComponentSettingsFocalPlaneArrayType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentSettingsFocalPlaneArrayType = 2041;

         //  This accessor type is used to identify the uci::type::POST_ComponentSettingsGeneratorSettingsType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentSettingsGeneratorSettingsType = 2042;

         //  This accessor type is used to identify the uci::type::POST_ComponentSettingsOutputProductSettingsType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentSettingsOutputProductSettingsType = 2043;

         //  This accessor type is used to identify the uci::type::POST_ComponentSettingsProcessingStageProcessingSettingsType
         //  Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentSettingsProcessingStageProcessingSettingsType = 2044;

         //  This accessor type is used to identify the uci::type::POST_ComponentSettingsProcessingStageSettingsType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentSettingsProcessingStageSettingsType = 2045;

         //  This accessor type is used to identify the uci::type::POST_ComponentSettingsProcessingStageType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentSettingsProcessingStageType = 2046;

         //  This accessor type is used to identify the uci::type::POST_ComponentSettingsProductGeneratorSettingsType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentSettingsProductGeneratorSettingsType = 2047;

         //  This accessor type is used to identify the uci::type::POST_ComponentSettingsProductGeneratorType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentSettingsProductGeneratorType = 2048;

         //  This accessor type is used to identify the uci::type::POST_ComponentSettingsProductSettingsType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentSettingsProductSettingsType = 2049;

         //  This accessor type is used to identify the uci::type::POST_ComponentSettingsType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentSettingsType = 2050;

         //  This accessor type is used to identify the uci::type::POST_ComponentStatusApertureSettingsType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentStatusApertureSettingsType = 2051;

         //  This accessor type is used to identify the uci::type::POST_ComponentStatusFilterType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentStatusFilterType = 2052;

         //  This accessor type is used to identify the uci::type::POST_ComponentStatusFocalPlaneArrayChannelType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentStatusFocalPlaneArrayChannelType = 2053;

         //  This accessor type is used to identify the uci::type::POST_ComponentStatusFocalPlaneArrayOpticalBandType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentStatusFocalPlaneArrayOpticalBandType = 2054;

         //  This accessor type is used to identify the uci::type::POST_ComponentStatusFocalPlaneArrayType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentStatusFocalPlaneArrayType = 2055;

         //  This accessor type is used to identify the uci::type::POST_ComponentStatusOutputProductSettingsType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentStatusOutputProductSettingsType = 2056;

         //  This accessor type is used to identify the uci::type::POST_ComponentStatusProcessingStageProcessingSettingsType
         //  Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentStatusProcessingStageProcessingSettingsType = 2057;

         //  This accessor type is used to identify the uci::type::POST_ComponentStatusProcessingStageSettingsType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentStatusProcessingStageSettingsType = 2058;

         //  This accessor type is used to identify the uci::type::POST_ComponentStatusProcessingStageType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentStatusProcessingStageType = 2059;

         //  This accessor type is used to identify the uci::type::POST_ComponentStatusProductGeneratorType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentStatusProductGeneratorType = 2060;

         //  This accessor type is used to identify the uci::type::POST_ComponentStatusType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentStatusType = 2061;

         //  This accessor type is used to identify the uci::type::POST_ComponentType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentType = 2062;

         //  This accessor type is used to identify the uci::type::POST_ComponentWindowType Accessor
         static const uci::base::accessorType::AccessorType pOST_ComponentWindowType = 2063;

         //  This accessor type is used to identify the uci::type::POST_OffsetTrackSettingsType Accessor
         static const uci::base::accessorType::AccessorType pOST_OffsetTrackSettingsType = 2064;

         //  This accessor type is used to identify the uci::type::POST_ProductOutputCommandImageryType Accessor
         static const uci::base::accessorType::AccessorType pOST_ProductOutputCommandImageryType = 2065;

         //  This accessor type is used to identify the uci::type::POST_RepetitionPositionChangeType Accessor
         static const uci::base::accessorType::AccessorType pOST_RepetitionPositionChangeType = 2066;

         //  This accessor type is used to identify the uci::type::POST_SettingsCommandMDT Accessor
         static const uci::base::accessorType::AccessorType pOST_SettingsCommandMDT = 2067;

         //  This accessor type is used to identify the uci::type::POST_SettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType pOST_SettingsCommandMT = 2068;

         //  This accessor type is used to identify the uci::type::POST_SettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType pOST_SettingsCommandQFT = 2069;

         //  This accessor type is used to identify the uci::type::POST_SettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType pOST_SettingsCommandStatusMT = 2070;

         //  This accessor type is used to identify the uci::type::POST_SettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType pOST_SettingsCommandStatusQFT = 2071;

         //  This accessor type is used to identify the uci::type::POST_SpecificDataType Accessor
         static const uci::base::accessorType::AccessorType pOST_SpecificDataType = 2072;

         //  This accessor type is used to identify the uci::type::POST_SubCapabilitySettingsType Accessor
         static const uci::base::accessorType::AccessorType pOST_SubCapabilitySettingsType = 2073;

         //  This accessor type is used to identify the uci::type::POST_SubCapabilityType Accessor
         static const uci::base::accessorType::AccessorType pOST_SubCapabilityType = 2074;

         //  This accessor type is used to identify the uci::type::POST_TaskBaseType Accessor
         static const uci::base::accessorType::AccessorType pOST_TaskBaseType = 2075;

         //  This accessor type is used to identify the uci::type::POST_TaskType Accessor
         static const uci::base::accessorType::AccessorType pOST_TaskType = 2076;

         //  This accessor type is used to identify the uci::type::POST_TechniqueCapabilityType Accessor
         static const uci::base::accessorType::AccessorType pOST_TechniqueCapabilityType = 2077;

         //  This accessor type is used to identify the uci::type::POST_TechniqueSettingType Accessor
         static const uci::base::accessorType::AccessorType pOST_TechniqueSettingType = 2078;

         //  This accessor type is used to identify the uci::type::POST_TrackActivityType Accessor
         static const uci::base::accessorType::AccessorType pOST_TrackActivityType = 2079;

         //  This accessor type is used to identify the uci::type::POST_TrackCapabilityType Accessor
         static const uci::base::accessorType::AccessorType pOST_TrackCapabilityType = 2080;

         //  This accessor type is used to identify the uci::type::POST_TrackDetailsType Accessor
         static const uci::base::accessorType::AccessorType pOST_TrackDetailsType = 2081;

         //  This accessor type is used to identify the uci::type::POST_TrackSettingsType Accessor
         static const uci::base::accessorType::AccessorType pOST_TrackSettingsType = 2082;

         //  This accessor type is used to identify the uci::type::POST_TrackStatusType Accessor
         static const uci::base::accessorType::AccessorType pOST_TrackStatusType = 2083;

         //  This accessor type is used to identify the uci::type::POST_WindowID_Type Accessor
         static const uci::base::accessorType::AccessorType pOST_WindowID_Type = 2084;

         //  This accessor type is used to identify the uci::type::PredictedThreatAssessmentType Accessor
         static const uci::base::accessorType::AccessorType predictedThreatAssessmentType = 2085;

         //  This accessor type is used to identify the uci::type::PRI_DataBaseType Accessor
         static const uci::base::accessorType::AccessorType pRI_DataBaseType = 2086;

         //  This accessor type is used to identify the uci::type::PRI_DataType Accessor
         static const uci::base::accessorType::AccessorType pRI_DataType = 2087;

         //  This accessor type is used to identify the uci::type::PRI_PW_Type Accessor
         static const uci::base::accessorType::AccessorType pRI_PW_Type = 2088;

         //  This accessor type is used to identify the uci::type::PrimaryControllerType Accessor
         static const uci::base::accessorType::AccessorType primaryControllerType = 2089;

         //  This accessor type is used to identify the uci::type::ProcessingConstrainedSubPlanType Accessor
         static const uci::base::accessorType::AccessorType processingConstrainedSubPlanType = 2090;

         //  This accessor type is used to identify the uci::type::ProcessingParametersPET Accessor
         static const uci::base::accessorType::AccessorType processingParametersPET = 2091;

         //  This accessor type is used to identify the uci::type::ProcessingReportType Accessor
         static const uci::base::accessorType::AccessorType processingReportType = 2092;

         //  This accessor type is used to identify the uci::type::ProcessingRequirementsType Accessor
         static const uci::base::accessorType::AccessorType processingRequirementsType = 2093;

         //  This accessor type is used to identify the uci::type::ProcessingResultsPET Accessor
         static const uci::base::accessorType::AccessorType processingResultsPET = 2094;

         //  This accessor type is used to identify the uci::type::ProductClassificationPlanMDT Accessor
         static const uci::base::accessorType::AccessorType productClassificationPlanMDT = 2095;

         //  This accessor type is used to identify the uci::type::ProductClassificationPlanMT Accessor
         static const uci::base::accessorType::AccessorType productClassificationPlanMT = 2096;

         //  This accessor type is used to identify the uci::type::ProductClassificationPlanQFT Accessor
         static const uci::base::accessorType::AccessorType productClassificationPlanQFT = 2097;

         //  This accessor type is used to identify the uci::type::ProductClassificationPlanType Accessor
         static const uci::base::accessorType::AccessorType productClassificationPlanType = 2098;

         //  This accessor type is used to identify the uci::type::ProductClassificationReportMDT Accessor
         static const uci::base::accessorType::AccessorType productClassificationReportMDT = 2099;

         //  This accessor type is used to identify the uci::type::ProductClassificationReportMT Accessor
         static const uci::base::accessorType::AccessorType productClassificationReportMT = 2100;

         //  This accessor type is used to identify the uci::type::ProductClassificationReportQFT Accessor
         static const uci::base::accessorType::AccessorType productClassificationReportQFT = 2101;

         //  This accessor type is used to identify the uci::type::ProductClassificationRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType productClassificationRequestID_Type = 2102;

         //  This accessor type is used to identify the uci::type::ProductClassificationRequestMDT Accessor
         static const uci::base::accessorType::AccessorType productClassificationRequestMDT = 2103;

         //  This accessor type is used to identify the uci::type::ProductClassificationRequestMT Accessor
         static const uci::base::accessorType::AccessorType productClassificationRequestMT = 2104;

         //  This accessor type is used to identify the uci::type::ProductClassificationRequestQFT Accessor
         static const uci::base::accessorType::AccessorType productClassificationRequestQFT = 2105;

         //  This accessor type is used to identify the uci::type::ProductClassificationRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType productClassificationRequestStatusMDT = 2106;

         //  This accessor type is used to identify the uci::type::ProductClassificationRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType productClassificationRequestStatusMT = 2107;

         //  This accessor type is used to identify the uci::type::ProductClassificationRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType productClassificationRequestStatusQFT = 2108;

         //  This accessor type is used to identify the uci::type::ProductClassificationStatusLabelType Accessor
         static const uci::base::accessorType::AccessorType productClassificationStatusLabelType = 2109;

         //  This accessor type is used to identify the uci::type::ProductClassificationStatusType Accessor
         static const uci::base::accessorType::AccessorType productClassificationStatusType = 2110;

         //  This accessor type is used to identify the uci::type::ProductClassificationTaskMDT Accessor
         static const uci::base::accessorType::AccessorType productClassificationTaskMDT = 2111;

         //  This accessor type is used to identify the uci::type::ProductClassificationTaskMT Accessor
         static const uci::base::accessorType::AccessorType productClassificationTaskMT = 2112;

         //  This accessor type is used to identify the uci::type::ProductClassificationTaskQFT Accessor
         static const uci::base::accessorType::AccessorType productClassificationTaskQFT = 2113;

         //  This accessor type is used to identify the uci::type::ProductClassificationTaskStatusMDT Accessor
         static const uci::base::accessorType::AccessorType productClassificationTaskStatusMDT = 2114;

         //  This accessor type is used to identify the uci::type::ProductClassificationTaskStatusMT Accessor
         static const uci::base::accessorType::AccessorType productClassificationTaskStatusMT = 2115;

         //  This accessor type is used to identify the uci::type::ProductClassificationTaskStatusQFT Accessor
         static const uci::base::accessorType::AccessorType productClassificationTaskStatusQFT = 2116;

         //  This accessor type is used to identify the uci::type::ProductCollectionLocationType Accessor
         static const uci::base::accessorType::AccessorType productCollectionLocationType = 2117;

         //  This accessor type is used to identify the uci::type::ProductCollectionPathType Accessor
         static const uci::base::accessorType::AccessorType productCollectionPathType = 2118;

      } // Namespace: accessorType
      //  
      namespace productCollectionPathType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::ProductCollectionPathType::PointIndex Accessor
            static const uci::base::accessorType::AccessorType pointIndex = 2119;

         } // Namespace: accessorType
      } // Namespace: productCollectionPathType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::ProductDisseminationConfigurationMDT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationConfigurationMDT = 2120;

         //  This accessor type is used to identify the uci::type::ProductDisseminationConfigurationMT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationConfigurationMT = 2121;

         //  This accessor type is used to identify the uci::type::ProductDisseminationConfigurationQFT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationConfigurationQFT = 2122;

         //  This accessor type is used to identify the uci::type::ProductDisseminationDestinationMDT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationDestinationMDT = 2123;

         //  This accessor type is used to identify the uci::type::ProductDisseminationDestinationMT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationDestinationMT = 2124;

         //  This accessor type is used to identify the uci::type::ProductDisseminationDestinationQFT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationDestinationQFT = 2125;

         //  This accessor type is used to identify the uci::type::ProductDisseminationGatewayType Accessor
         static const uci::base::accessorType::AccessorType productDisseminationGatewayType = 2126;

         //  This accessor type is used to identify the uci::type::ProductDisseminationPlanMDT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationPlanMDT = 2127;

         //  This accessor type is used to identify the uci::type::ProductDisseminationPlanMT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationPlanMT = 2128;

         //  This accessor type is used to identify the uci::type::ProductDisseminationPlanQFT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationPlanQFT = 2129;

         //  This accessor type is used to identify the uci::type::ProductDisseminationPlanType Accessor
         static const uci::base::accessorType::AccessorType productDisseminationPlanType = 2130;

         //  This accessor type is used to identify the uci::type::ProductDisseminationReportMDT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationReportMDT = 2131;

         //  This accessor type is used to identify the uci::type::ProductDisseminationReportMT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationReportMT = 2132;

         //  This accessor type is used to identify the uci::type::ProductDisseminationReportQFT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationReportQFT = 2133;

         //  This accessor type is used to identify the uci::type::ProductDisseminationRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType productDisseminationRequestID_Type = 2134;

         //  This accessor type is used to identify the uci::type::ProductDisseminationRequestMDT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationRequestMDT = 2135;

         //  This accessor type is used to identify the uci::type::ProductDisseminationRequestMT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationRequestMT = 2136;

         //  This accessor type is used to identify the uci::type::ProductDisseminationRequestQFT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationRequestQFT = 2137;

         //  This accessor type is used to identify the uci::type::ProductDisseminationRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationRequestStatusMDT = 2138;

         //  This accessor type is used to identify the uci::type::ProductDisseminationRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationRequestStatusMT = 2139;

         //  This accessor type is used to identify the uci::type::ProductDisseminationRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationRequestStatusQFT = 2140;

         //  This accessor type is used to identify the uci::type::ProductDisseminationStatusType Accessor
         static const uci::base::accessorType::AccessorType productDisseminationStatusType = 2141;

         //  This accessor type is used to identify the uci::type::ProductDisseminationTaskMDT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationTaskMDT = 2142;

         //  This accessor type is used to identify the uci::type::ProductDisseminationTaskMT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationTaskMT = 2143;

         //  This accessor type is used to identify the uci::type::ProductDisseminationTaskQFT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationTaskQFT = 2144;

         //  This accessor type is used to identify the uci::type::ProductDisseminationTaskStatusMDT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationTaskStatusMDT = 2145;

         //  This accessor type is used to identify the uci::type::ProductDisseminationTaskStatusMT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationTaskStatusMT = 2146;

         //  This accessor type is used to identify the uci::type::ProductDisseminationTaskStatusQFT Accessor
         static const uci::base::accessorType::AccessorType productDisseminationTaskStatusQFT = 2147;

         //  This accessor type is used to identify the uci::type::ProductDownloadPlanMDT Accessor
         static const uci::base::accessorType::AccessorType productDownloadPlanMDT = 2148;

         //  This accessor type is used to identify the uci::type::ProductDownloadPlanMT Accessor
         static const uci::base::accessorType::AccessorType productDownloadPlanMT = 2149;

         //  This accessor type is used to identify the uci::type::ProductDownloadPlanQFT Accessor
         static const uci::base::accessorType::AccessorType productDownloadPlanQFT = 2150;

         //  This accessor type is used to identify the uci::type::ProductDownloadPlanType Accessor
         static const uci::base::accessorType::AccessorType productDownloadPlanType = 2151;

         //  This accessor type is used to identify the uci::type::ProductDownloadReportMDT Accessor
         static const uci::base::accessorType::AccessorType productDownloadReportMDT = 2152;

         //  This accessor type is used to identify the uci::type::ProductDownloadReportMT Accessor
         static const uci::base::accessorType::AccessorType productDownloadReportMT = 2153;

         //  This accessor type is used to identify the uci::type::ProductDownloadReportQFT Accessor
         static const uci::base::accessorType::AccessorType productDownloadReportQFT = 2154;

         //  This accessor type is used to identify the uci::type::ProductDownloadRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType productDownloadRequestID_Type = 2155;

         //  This accessor type is used to identify the uci::type::ProductDownloadRequestMDT Accessor
         static const uci::base::accessorType::AccessorType productDownloadRequestMDT = 2156;

         //  This accessor type is used to identify the uci::type::ProductDownloadRequestMT Accessor
         static const uci::base::accessorType::AccessorType productDownloadRequestMT = 2157;

         //  This accessor type is used to identify the uci::type::ProductDownloadRequestQFT Accessor
         static const uci::base::accessorType::AccessorType productDownloadRequestQFT = 2158;

         //  This accessor type is used to identify the uci::type::ProductDownloadRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType productDownloadRequestStatusMDT = 2159;

         //  This accessor type is used to identify the uci::type::ProductDownloadRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType productDownloadRequestStatusMT = 2160;

         //  This accessor type is used to identify the uci::type::ProductDownloadRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType productDownloadRequestStatusQFT = 2161;

         //  This accessor type is used to identify the uci::type::ProductDownloadStatusType Accessor
         static const uci::base::accessorType::AccessorType productDownloadStatusType = 2162;

         //  This accessor type is used to identify the uci::type::ProductDownloadTaskMDT Accessor
         static const uci::base::accessorType::AccessorType productDownloadTaskMDT = 2163;

         //  This accessor type is used to identify the uci::type::ProductDownloadTaskMT Accessor
         static const uci::base::accessorType::AccessorType productDownloadTaskMT = 2164;

         //  This accessor type is used to identify the uci::type::ProductDownloadTaskQFT Accessor
         static const uci::base::accessorType::AccessorType productDownloadTaskQFT = 2165;

         //  This accessor type is used to identify the uci::type::ProductDownloadTaskStatusMDT Accessor
         static const uci::base::accessorType::AccessorType productDownloadTaskStatusMDT = 2166;

         //  This accessor type is used to identify the uci::type::ProductDownloadTaskStatusMT Accessor
         static const uci::base::accessorType::AccessorType productDownloadTaskStatusMT = 2167;

         //  This accessor type is used to identify the uci::type::ProductDownloadTaskStatusQFT Accessor
         static const uci::base::accessorType::AccessorType productDownloadTaskStatusQFT = 2168;

         //  This accessor type is used to identify the uci::type::ProductFilterType Accessor
         static const uci::base::accessorType::AccessorType productFilterType = 2169;

         //  This accessor type is used to identify the uci::type::ProductGeospatialLocationType Accessor
         static const uci::base::accessorType::AccessorType productGeospatialLocationType = 2170;

         //  This accessor type is used to identify the uci::type::ProductLocationID_Type Accessor
         static const uci::base::accessorType::AccessorType productLocationID_Type = 2171;

         //  This accessor type is used to identify the uci::type::ProductLocationMDT Accessor
         static const uci::base::accessorType::AccessorType productLocationMDT = 2172;

         //  This accessor type is used to identify the uci::type::ProductLocationMT Accessor
         static const uci::base::accessorType::AccessorType productLocationMT = 2173;

         //  This accessor type is used to identify the uci::type::ProductLocationQFT Accessor
         static const uci::base::accessorType::AccessorType productLocationQFT = 2174;

         //  This accessor type is used to identify the uci::type::ProductLocationType Accessor
         static const uci::base::accessorType::AccessorType productLocationType = 2175;

         //  This accessor type is used to identify the uci::type::ProductManagementTaskDataType Accessor
         static const uci::base::accessorType::AccessorType productManagementTaskDataType = 2176;

         //  This accessor type is used to identify the uci::type::ProductManagementTaskStatusDataType Accessor
         static const uci::base::accessorType::AccessorType productManagementTaskStatusDataType = 2177;

         //  This accessor type is used to identify the uci::type::ProductMetadataID_Type Accessor
         static const uci::base::accessorType::AccessorType productMetadataID_Type = 2178;

         //  This accessor type is used to identify the uci::type::ProductMetadataMDT Accessor
         static const uci::base::accessorType::AccessorType productMetadataMDT = 2179;

         //  This accessor type is used to identify the uci::type::ProductMetadataMT Accessor
         static const uci::base::accessorType::AccessorType productMetadataMT = 2180;

         //  This accessor type is used to identify the uci::type::ProductMetadataQFT Accessor
         static const uci::base::accessorType::AccessorType productMetadataQFT = 2181;

         //  This accessor type is used to identify the uci::type::ProductOutputCapabilityImageryType Accessor
         static const uci::base::accessorType::AccessorType productOutputCapabilityImageryType = 2182;

         //  This accessor type is used to identify the uci::type::ProductOutputCapabilityStatusImageryType Accessor
         static const uci::base::accessorType::AccessorType productOutputCapabilityStatusImageryType = 2183;

         //  This accessor type is used to identify the uci::type::ProductOutputCommandBasicType Accessor
         static const uci::base::accessorType::AccessorType productOutputCommandBasicType = 2184;

         //  This accessor type is used to identify the uci::type::ProductOutputCommandImageryType Accessor
         static const uci::base::accessorType::AccessorType productOutputCommandImageryType = 2185;

         //  This accessor type is used to identify the uci::type::ProductOutputCommandSMTI_Type Accessor
         static const uci::base::accessorType::AccessorType productOutputCommandSMTI_Type = 2186;

         //  This accessor type is used to identify the uci::type::ProductOutputComponentStatusImageryType Accessor
         static const uci::base::accessorType::AccessorType productOutputComponentStatusImageryType = 2187;

         //  This accessor type is used to identify the uci::type::ProductOutputStatusType Accessor
         static const uci::base::accessorType::AccessorType productOutputStatusType = 2188;

         //  This accessor type is used to identify the uci::type::ProductOutputType Accessor
         static const uci::base::accessorType::AccessorType productOutputType = 2189;

         //  This accessor type is used to identify the uci::type::ProductParentType Accessor
         static const uci::base::accessorType::AccessorType productParentType = 2190;

         //  This accessor type is used to identify the uci::type::ProductProcessingClassificationType Accessor
         static const uci::base::accessorType::AccessorType productProcessingClassificationType = 2191;

         //  This accessor type is used to identify the uci::type::ProductProcessingPlanMDT Accessor
         static const uci::base::accessorType::AccessorType productProcessingPlanMDT = 2192;

         //  This accessor type is used to identify the uci::type::ProductProcessingPlanMT Accessor
         static const uci::base::accessorType::AccessorType productProcessingPlanMT = 2193;

         //  This accessor type is used to identify the uci::type::ProductProcessingPlanQFT Accessor
         static const uci::base::accessorType::AccessorType productProcessingPlanQFT = 2194;

         //  This accessor type is used to identify the uci::type::ProductProcessingPlanType Accessor
         static const uci::base::accessorType::AccessorType productProcessingPlanType = 2195;

         //  This accessor type is used to identify the uci::type::ProductProcessingReportMDT Accessor
         static const uci::base::accessorType::AccessorType productProcessingReportMDT = 2196;

         //  This accessor type is used to identify the uci::type::ProductProcessingReportMT Accessor
         static const uci::base::accessorType::AccessorType productProcessingReportMT = 2197;

         //  This accessor type is used to identify the uci::type::ProductProcessingReportQFT Accessor
         static const uci::base::accessorType::AccessorType productProcessingReportQFT = 2198;

         //  This accessor type is used to identify the uci::type::ProductProcessingRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType productProcessingRequestID_Type = 2199;

         //  This accessor type is used to identify the uci::type::ProductProcessingRequestMDT Accessor
         static const uci::base::accessorType::AccessorType productProcessingRequestMDT = 2200;

         //  This accessor type is used to identify the uci::type::ProductProcessingRequestMT Accessor
         static const uci::base::accessorType::AccessorType productProcessingRequestMT = 2201;

         //  This accessor type is used to identify the uci::type::ProductProcessingRequestQFT Accessor
         static const uci::base::accessorType::AccessorType productProcessingRequestQFT = 2202;

         //  This accessor type is used to identify the uci::type::ProductProcessingRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType productProcessingRequestStatusMDT = 2203;

         //  This accessor type is used to identify the uci::type::ProductProcessingRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType productProcessingRequestStatusMT = 2204;

         //  This accessor type is used to identify the uci::type::ProductProcessingRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType productProcessingRequestStatusQFT = 2205;

         //  This accessor type is used to identify the uci::type::ProductProcessingStatusType Accessor
         static const uci::base::accessorType::AccessorType productProcessingStatusType = 2206;

         //  This accessor type is used to identify the uci::type::ProductProcessingTaskMDT Accessor
         static const uci::base::accessorType::AccessorType productProcessingTaskMDT = 2207;

         //  This accessor type is used to identify the uci::type::ProductProcessingTaskMT Accessor
         static const uci::base::accessorType::AccessorType productProcessingTaskMT = 2208;

         //  This accessor type is used to identify the uci::type::ProductProcessingTaskQFT Accessor
         static const uci::base::accessorType::AccessorType productProcessingTaskQFT = 2209;

         //  This accessor type is used to identify the uci::type::ProductProcessingTaskStatusMDT Accessor
         static const uci::base::accessorType::AccessorType productProcessingTaskStatusMDT = 2210;

         //  This accessor type is used to identify the uci::type::ProductProcessingTaskStatusMT Accessor
         static const uci::base::accessorType::AccessorType productProcessingTaskStatusMT = 2211;

         //  This accessor type is used to identify the uci::type::ProductProcessingTaskStatusQFT Accessor
         static const uci::base::accessorType::AccessorType productProcessingTaskStatusQFT = 2212;

         //  This accessor type is used to identify the uci::type::ProductProcessingType Accessor
         static const uci::base::accessorType::AccessorType productProcessingType = 2213;

         //  This accessor type is used to identify the uci::type::ProductRateSettingsType Accessor
         static const uci::base::accessorType::AccessorType productRateSettingsType = 2214;

         //  This accessor type is used to identify the uci::type::ProductReferenceByParentType Accessor
         static const uci::base::accessorType::AccessorType productReferenceByParentType = 2215;

         //  This accessor type is used to identify the uci::type::ProductReferenceType Accessor
         static const uci::base::accessorType::AccessorType productReferenceType = 2216;

         //  This accessor type is used to identify the uci::type::ProductResolutionSettingsType Accessor
         static const uci::base::accessorType::AccessorType productResolutionSettingsType = 2217;

         //  This accessor type is used to identify the uci::type::ProductResolutionType Accessor
         static const uci::base::accessorType::AccessorType productResolutionType = 2218;

         //  This accessor type is used to identify the uci::type::ProductSizeSettingsType Accessor
         static const uci::base::accessorType::AccessorType productSizeSettingsType = 2219;

         //  This accessor type is used to identify the uci::type::ProductSystemLocationType Accessor
         static const uci::base::accessorType::AccessorType productSystemLocationType = 2220;

         //  This accessor type is used to identify the uci::type::ProductTypeFilterType Accessor
         static const uci::base::accessorType::AccessorType productTypeFilterType = 2221;

         //  This accessor type is used to identify the uci::type::ProtectedAssetReturnPowerType Accessor
         static const uci::base::accessorType::AccessorType protectedAssetReturnPowerType = 2222;

         //  This accessor type is used to identify the uci::type::ProtectedAssetType Accessor
         static const uci::base::accessorType::AccessorType protectedAssetType = 2223;

         //  This accessor type is used to identify the uci::type::PulseDataCollectCommandType Accessor
         static const uci::base::accessorType::AccessorType pulseDataCollectCommandType = 2224;

         //  This accessor type is used to identify the uci::type::PulseDataID_Type Accessor
         static const uci::base::accessorType::AccessorType pulseDataID_Type = 2225;

         //  This accessor type is used to identify the uci::type::PulseDataMDT Accessor
         static const uci::base::accessorType::AccessorType pulseDataMDT = 2226;

         //  This accessor type is used to identify the uci::type::PulseDataMT Accessor
         static const uci::base::accessorType::AccessorType pulseDataMT = 2227;

         //  This accessor type is used to identify the uci::type::PulseDataQFT Accessor
         static const uci::base::accessorType::AccessorType pulseDataQFT = 2228;

         //  This accessor type is used to identify the uci::type::PulseDataSourceType Accessor
         static const uci::base::accessorType::AccessorType pulseDataSourceType = 2229;

         //  This accessor type is used to identify the uci::type::PulseType Accessor
         static const uci::base::accessorType::AccessorType pulseType = 2230;

         //  This accessor type is used to identify the uci::type::QualifyingTagsType Accessor
         static const uci::base::accessorType::AccessorType qualifyingTagsType = 2231;

      } // Namespace: accessorType
      //  
      namespace qualifyingTagsType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::QualifyingTagsType::QualifiedSpecial Accessor
            static const uci::base::accessorType::AccessorType qualifiedSpecial = 2232;

         } // Namespace: accessorType
      } // Namespace: qualifyingTagsType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::QuaternionType Accessor
         static const uci::base::accessorType::AccessorType quaternionType = 2233;

         //  This accessor type is used to identify the uci::type::QueryFilterPET Accessor
         static const uci::base::accessorType::AccessorType queryFilterPET = 2234;

         //  This accessor type is used to identify the uci::type::QueryPaginationType Accessor
         static const uci::base::accessorType::AccessorType queryPaginationType = 2235;

         //  This accessor type is used to identify the uci::type::QueryRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType queryRequestID_Type = 2236;

         //  This accessor type is used to identify the uci::type::QueryRequestMDT Accessor
         static const uci::base::accessorType::AccessorType queryRequestMDT = 2237;

         //  This accessor type is used to identify the uci::type::QueryRequestMT Accessor
         static const uci::base::accessorType::AccessorType queryRequestMT = 2238;

         //  This accessor type is used to identify the uci::type::QueryRequestQFT Accessor
         static const uci::base::accessorType::AccessorType queryRequestQFT = 2239;

         //  This accessor type is used to identify the uci::type::QueryRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType queryRequestStatusMDT = 2240;

         //  This accessor type is used to identify the uci::type::QueryRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType queryRequestStatusMT = 2241;

         //  This accessor type is used to identify the uci::type::QueryRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType queryRequestStatusQFT = 2242;

         //  This accessor type is used to identify the uci::type::QueryResultType Accessor
         static const uci::base::accessorType::AccessorType queryResultType = 2243;

         //  This accessor type is used to identify the uci::type::RadarActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType radarActivityCommandType = 2244;

         //  This accessor type is used to identify the uci::type::RadarCollectionOptionsType Accessor
         static const uci::base::accessorType::AccessorType radarCollectionOptionsType = 2245;

         //  This accessor type is used to identify the uci::type::RadarEmitterIdentityType Accessor
         static const uci::base::accessorType::AccessorType radarEmitterIdentityType = 2246;

         //  This accessor type is used to identify the uci::type::RadarPointingTargetType Accessor
         static const uci::base::accessorType::AccessorType radarPointingTargetType = 2247;

         //  This accessor type is used to identify the uci::type::RadarSpoilTaperType Accessor
         static const uci::base::accessorType::AccessorType radarSpoilTaperType = 2248;

         //  This accessor type is used to identify the uci::type::RadarSpoilType Accessor
         static const uci::base::accessorType::AccessorType radarSpoilType = 2249;

      } // Namespace: accessorType
      //  
      namespace radarSpoilType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::RadarSpoilType::SpoilFactor Accessor
            static const uci::base::accessorType::AccessorType spoilFactor = 2250;

         } // Namespace: accessorType
      } // Namespace: radarSpoilType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::RadarTaperType Accessor
         static const uci::base::accessorType::AccessorType radarTaperType = 2251;

      } // Namespace: accessorType
      //  
      namespace radarTaperType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::RadarTaperType::SidelobeLevel Accessor
            static const uci::base::accessorType::AccessorType sidelobeLevel = 2252;

         } // Namespace: accessorType
      } // Namespace: radarTaperType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::RadarTaperWeightingFunctionType Accessor
         static const uci::base::accessorType::AccessorType radarTaperWeightingFunctionType = 2253;

         //  This accessor type is used to identify the uci::type::RangeElevationExtentChoiceType Accessor
         static const uci::base::accessorType::AccessorType rangeElevationExtentChoiceType = 2254;

         //  This accessor type is used to identify the uci::type::RangeExtentType Accessor
         static const uci::base::accessorType::AccessorType rangeExtentType = 2255;

         //  This accessor type is used to identify the uci::type::RangeResolutionType Accessor
         static const uci::base::accessorType::AccessorType rangeResolutionType = 2256;

         //  This accessor type is used to identify the uci::type::RankingType Accessor
         static const uci::base::accessorType::AccessorType rankingType = 2257;

         //  This accessor type is used to identify the uci::type::RatesSettingsType Accessor
         static const uci::base::accessorType::AccessorType ratesSettingsType = 2258;

         //  This accessor type is used to identify the uci::type::ReadinessItemType Accessor
         static const uci::base::accessorType::AccessorType readinessItemType = 2259;

         //  This accessor type is used to identify the uci::type::ReadinessStatusMDT Accessor
         static const uci::base::accessorType::AccessorType readinessStatusMDT = 2260;

         //  This accessor type is used to identify the uci::type::ReadinessStatusMT Accessor
         static const uci::base::accessorType::AccessorType readinessStatusMT = 2261;

         //  This accessor type is used to identify the uci::type::ReadinessStatusQFT Accessor
         static const uci::base::accessorType::AccessorType readinessStatusQFT = 2262;

         //  This accessor type is used to identify the uci::type::RecommendedWeaponsType Accessor
         static const uci::base::accessorType::AccessorType recommendedWeaponsType = 2263;

         //  This accessor type is used to identify the uci::type::ReconnaissanceStatusType Accessor
         static const uci::base::accessorType::AccessorType reconnaissanceStatusType = 2264;

         //  This accessor type is used to identify the uci::type::ReconnaissanceTaskType Accessor
         static const uci::base::accessorType::AccessorType reconnaissanceTaskType = 2265;

         //  This accessor type is used to identify the uci::type::ReferenceObjectType Accessor
         static const uci::base::accessorType::AccessorType referenceObjectType = 2266;

         //  This accessor type is used to identify the uci::type::RefuelActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType refuelActivityCommandType = 2267;

         //  This accessor type is used to identify the uci::type::RefuelActivityDrogueCommandType Accessor
         static const uci::base::accessorType::AccessorType refuelActivityDrogueCommandType = 2268;

         //  This accessor type is used to identify the uci::type::RefuelActivityDrogueStatusType Accessor
         static const uci::base::accessorType::AccessorType refuelActivityDrogueStatusType = 2269;

         //  This accessor type is used to identify the uci::type::RefuelActivityMDT Accessor
         static const uci::base::accessorType::AccessorType refuelActivityMDT = 2270;

         //  This accessor type is used to identify the uci::type::RefuelActivityMT Accessor
         static const uci::base::accessorType::AccessorType refuelActivityMT = 2271;

         //  This accessor type is used to identify the uci::type::RefuelActivityQFT Accessor
         static const uci::base::accessorType::AccessorType refuelActivityQFT = 2272;

         //  This accessor type is used to identify the uci::type::RefuelActivityStatusType Accessor
         static const uci::base::accessorType::AccessorType refuelActivityStatusType = 2273;

         //  This accessor type is used to identify the uci::type::RefuelActivityType Accessor
         static const uci::base::accessorType::AccessorType refuelActivityType = 2274;

         //  This accessor type is used to identify the uci::type::RefuelCapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType refuelCapabilityCommandType = 2275;

         //  This accessor type is used to identify the uci::type::RefuelCapabilityDrogueCommandType Accessor
         static const uci::base::accessorType::AccessorType refuelCapabilityDrogueCommandType = 2276;

         //  This accessor type is used to identify the uci::type::RefuelCapabilityDrogueCommonType Accessor
         static const uci::base::accessorType::AccessorType refuelCapabilityDrogueCommonType = 2277;

         //  This accessor type is used to identify the uci::type::RefuelCapabilityDrogueStatusType Accessor
         static const uci::base::accessorType::AccessorType refuelCapabilityDrogueStatusType = 2278;

         //  This accessor type is used to identify the uci::type::RefuelCapabilityExtendCommandType Accessor
         static const uci::base::accessorType::AccessorType refuelCapabilityExtendCommandType = 2279;

         //  This accessor type is used to identify the uci::type::RefuelCapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType refuelCapabilityMDT = 2280;

         //  This accessor type is used to identify the uci::type::RefuelCapabilityMT Accessor
         static const uci::base::accessorType::AccessorType refuelCapabilityMT = 2281;

         //  This accessor type is used to identify the uci::type::RefuelCapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType refuelCapabilityQFT = 2282;

         //  This accessor type is used to identify the uci::type::RefuelCapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType refuelCapabilityStatusMDT = 2283;

         //  This accessor type is used to identify the uci::type::RefuelCapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType refuelCapabilityStatusMT = 2284;

         //  This accessor type is used to identify the uci::type::RefuelCapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType refuelCapabilityStatusQFT = 2285;

         //  This accessor type is used to identify the uci::type::RefuelCapabilityStatusType Accessor
         static const uci::base::accessorType::AccessorType refuelCapabilityStatusType = 2286;

         //  This accessor type is used to identify the uci::type::RefuelCapabilityType Accessor
         static const uci::base::accessorType::AccessorType refuelCapabilityType = 2287;

         //  This accessor type is used to identify the uci::type::RefuelCommandMDT Accessor
         static const uci::base::accessorType::AccessorType refuelCommandMDT = 2288;

         //  This accessor type is used to identify the uci::type::RefuelCommandMT Accessor
         static const uci::base::accessorType::AccessorType refuelCommandMT = 2289;

         //  This accessor type is used to identify the uci::type::RefuelCommandQFT Accessor
         static const uci::base::accessorType::AccessorType refuelCommandQFT = 2290;

         //  This accessor type is used to identify the uci::type::RefuelCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType refuelCommandStatusMT = 2291;

         //  This accessor type is used to identify the uci::type::RefuelCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType refuelCommandStatusQFT = 2292;

         //  This accessor type is used to identify the uci::type::RefuelCommandType Accessor
         static const uci::base::accessorType::AccessorType refuelCommandType = 2293;

         //  This accessor type is used to identify the uci::type::RefuelConnectionType Accessor
         static const uci::base::accessorType::AccessorType refuelConnectionType = 2294;

         //  This accessor type is used to identify the uci::type::RefuelSettingsCommandMDT Accessor
         static const uci::base::accessorType::AccessorType refuelSettingsCommandMDT = 2295;

         //  This accessor type is used to identify the uci::type::RefuelSettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType refuelSettingsCommandMT = 2296;

         //  This accessor type is used to identify the uci::type::RefuelSettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType refuelSettingsCommandQFT = 2297;

         //  This accessor type is used to identify the uci::type::RefuelSettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType refuelSettingsCommandStatusMT = 2298;

         //  This accessor type is used to identify the uci::type::RefuelSettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType refuelSettingsCommandStatusQFT = 2299;

         //  This accessor type is used to identify the uci::type::RelationshipDesignationID_Type Accessor
         static const uci::base::accessorType::AccessorType relationshipDesignationID_Type = 2300;

         //  This accessor type is used to identify the uci::type::RelationshipDesignationMDT Accessor
         static const uci::base::accessorType::AccessorType relationshipDesignationMDT = 2301;

         //  This accessor type is used to identify the uci::type::RelationshipDesignationMT Accessor
         static const uci::base::accessorType::AccessorType relationshipDesignationMT = 2302;

         //  This accessor type is used to identify the uci::type::RelationshipDesignationQFT Accessor
         static const uci::base::accessorType::AccessorType relationshipDesignationQFT = 2303;

         //  This accessor type is used to identify the uci::type::RelationshipThreatType Accessor
         static const uci::base::accessorType::AccessorType relationshipThreatType = 2304;

         //  This accessor type is used to identify the uci::type::RelationshipType Accessor
         static const uci::base::accessorType::AccessorType relationshipType = 2305;

         //  This accessor type is used to identify the uci::type::RelativeNavigationType Accessor
         static const uci::base::accessorType::AccessorType relativeNavigationType = 2306;

         //  This accessor type is used to identify the uci::type::RelativePositionType Accessor
         static const uci::base::accessorType::AccessorType relativePositionType = 2307;

         //  This accessor type is used to identify the uci::type::ReleaseConsentType Accessor
         static const uci::base::accessorType::AccessorType releaseConsentType = 2308;

         //  This accessor type is used to identify the uci::type::RemoteDestinationType Accessor
         static const uci::base::accessorType::AccessorType remoteDestinationType = 2309;

         //  This accessor type is used to identify the uci::type::RemoveRF_ThreadInstanceType Accessor
         static const uci::base::accessorType::AccessorType removeRF_ThreadInstanceType = 2310;

         //  This accessor type is used to identify the uci::type::RepetitionContinuousType Accessor
         static const uci::base::accessorType::AccessorType repetitionContinuousType = 2311;

         //  This accessor type is used to identify the uci::type::RepetitionFiniteType Accessor
         static const uci::base::accessorType::AccessorType repetitionFiniteType = 2312;

         //  This accessor type is used to identify the uci::type::RepetitionPeriodicType Accessor
         static const uci::base::accessorType::AccessorType repetitionPeriodicType = 2313;

         //  This accessor type is used to identify the uci::type::RepetitionPositionChangeType Accessor
         static const uci::base::accessorType::AccessorType repetitionPositionChangeType = 2314;

         //  This accessor type is used to identify the uci::type::ReplanReasonType Accessor
         static const uci::base::accessorType::AccessorType replanReasonType = 2315;

         //  This accessor type is used to identify the uci::type::RequestingFunctionType Accessor
         static const uci::base::accessorType::AccessorType requestingFunctionType = 2316;

         //  This accessor type is used to identify the uci::type::ResendRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType resendRequestID_Type = 2317;

         //  This accessor type is used to identify the uci::type::ResendRequestMDT Accessor
         static const uci::base::accessorType::AccessorType resendRequestMDT = 2318;

         //  This accessor type is used to identify the uci::type::ResendRequestMT Accessor
         static const uci::base::accessorType::AccessorType resendRequestMT = 2319;

         //  This accessor type is used to identify the uci::type::ResendRequestQFT Accessor
         static const uci::base::accessorType::AccessorType resendRequestQFT = 2320;

         //  This accessor type is used to identify the uci::type::ResendRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType resendRequestStatusMDT = 2321;

         //  This accessor type is used to identify the uci::type::ResendRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType resendRequestStatusMT = 2322;

         //  This accessor type is used to identify the uci::type::ResendRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType resendRequestStatusQFT = 2323;

         //  This accessor type is used to identify the uci::type::ResourceAllocationPeriodType Accessor
         static const uci::base::accessorType::AccessorType resourceAllocationPeriodType = 2324;

         //  This accessor type is used to identify the uci::type::ResourceDefinitionType Accessor
         static const uci::base::accessorType::AccessorType resourceDefinitionType = 2325;

         //  This accessor type is used to identify the uci::type::ResourceRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType resourceRequestID_Type = 2326;

         //  This accessor type is used to identify the uci::type::ResourceRequestOptionType Accessor
         static const uci::base::accessorType::AccessorType resourceRequestOptionType = 2327;

         //  This accessor type is used to identify the uci::type::ResourceRequestPeriodType Accessor
         static const uci::base::accessorType::AccessorType resourceRequestPeriodType = 2328;

         //  This accessor type is used to identify the uci::type::ResourceRequestType Accessor
         static const uci::base::accessorType::AccessorType resourceRequestType = 2329;

         //  This accessor type is used to identify the uci::type::ResourceSelectionType Accessor
         static const uci::base::accessorType::AccessorType resourceSelectionType = 2330;

         //  This accessor type is used to identify the uci::type::ResourceType Accessor
         static const uci::base::accessorType::AccessorType resourceType = 2331;

         //  This accessor type is used to identify the uci::type::ResponseListID_Type Accessor
         static const uci::base::accessorType::AccessorType responseListID_Type = 2332;

         //  This accessor type is used to identify the uci::type::ResponseListType Accessor
         static const uci::base::accessorType::AccessorType responseListType = 2333;

         //  This accessor type is used to identify the uci::type::ResponseOptionType Accessor
         static const uci::base::accessorType::AccessorType responseOptionType = 2334;

         //  This accessor type is used to identify the uci::type::ResultingActivityType Accessor
         static const uci::base::accessorType::AccessorType resultingActivityType = 2335;

         //  This accessor type is used to identify the uci::type::RF_BandType Accessor
         static const uci::base::accessorType::AccessorType rF_BandType = 2336;

         //  This accessor type is used to identify the uci::type::RF_ControlCommandID_Type Accessor
         static const uci::base::accessorType::AccessorType rF_ControlCommandID_Type = 2337;

         //  This accessor type is used to identify the uci::type::RF_ControlCommandMDT Accessor
         static const uci::base::accessorType::AccessorType rF_ControlCommandMDT = 2338;

         //  This accessor type is used to identify the uci::type::RF_ControlCommandMT Accessor
         static const uci::base::accessorType::AccessorType rF_ControlCommandMT = 2339;

         //  This accessor type is used to identify the uci::type::RF_ControlCommandQFT Accessor
         static const uci::base::accessorType::AccessorType rF_ControlCommandQFT = 2340;

         //  This accessor type is used to identify the uci::type::RF_ControlCommandStatusMDT Accessor
         static const uci::base::accessorType::AccessorType rF_ControlCommandStatusMDT = 2341;

         //  This accessor type is used to identify the uci::type::RF_ControlCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType rF_ControlCommandStatusMT = 2342;

         //  This accessor type is used to identify the uci::type::RF_ControlCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType rF_ControlCommandStatusQFT = 2343;

         //  This accessor type is used to identify the uci::type::RF_DistributionType Accessor
         static const uci::base::accessorType::AccessorType rF_DistributionType = 2344;

         //  This accessor type is used to identify the uci::type::RF_GainType Accessor
         static const uci::base::accessorType::AccessorType rF_GainType = 2345;

         //  This accessor type is used to identify the uci::type::RF_OperationType Accessor
         static const uci::base::accessorType::AccessorType rF_OperationType = 2346;

         //  This accessor type is used to identify the uci::type::RF_ProfileBandType Accessor
         static const uci::base::accessorType::AccessorType rF_ProfileBandType = 2347;

         //  This accessor type is used to identify the uci::type::RF_ProfileID_Type Accessor
         static const uci::base::accessorType::AccessorType rF_ProfileID_Type = 2348;

         //  This accessor type is used to identify the uci::type::RF_ProfileMDT Accessor
         static const uci::base::accessorType::AccessorType rF_ProfileMDT = 2349;

         //  This accessor type is used to identify the uci::type::RF_ProfileMT Accessor
         static const uci::base::accessorType::AccessorType rF_ProfileMT = 2350;

         //  This accessor type is used to identify the uci::type::RF_ProfileQFT Accessor
         static const uci::base::accessorType::AccessorType rF_ProfileQFT = 2351;

         //  This accessor type is used to identify the uci::type::RF_ProfileType Accessor
         static const uci::base::accessorType::AccessorType rF_ProfileType = 2352;

         //  This accessor type is used to identify the uci::type::RF_ReportID_Type Accessor
         static const uci::base::accessorType::AccessorType rF_ReportID_Type = 2353;

         //  This accessor type is used to identify the uci::type::RF_ReportMDT Accessor
         static const uci::base::accessorType::AccessorType rF_ReportMDT = 2354;

         //  This accessor type is used to identify the uci::type::RF_ReportMT Accessor
         static const uci::base::accessorType::AccessorType rF_ReportMT = 2355;

         //  This accessor type is used to identify the uci::type::RF_ReportQFT Accessor
         static const uci::base::accessorType::AccessorType rF_ReportQFT = 2356;

         //  This accessor type is used to identify the uci::type::RF_ResourceAllocationMDT Accessor
         static const uci::base::accessorType::AccessorType rF_ResourceAllocationMDT = 2357;

         //  This accessor type is used to identify the uci::type::RF_ResourceAllocationMT Accessor
         static const uci::base::accessorType::AccessorType rF_ResourceAllocationMT = 2358;

         //  This accessor type is used to identify the uci::type::RF_ResourceAllocationQFT Accessor
         static const uci::base::accessorType::AccessorType rF_ResourceAllocationQFT = 2359;

         //  This accessor type is used to identify the uci::type::RF_ResourceRequestMDT Accessor
         static const uci::base::accessorType::AccessorType rF_ResourceRequestMDT = 2360;

         //  This accessor type is used to identify the uci::type::RF_ResourceRequestMT Accessor
         static const uci::base::accessorType::AccessorType rF_ResourceRequestMT = 2361;

         //  This accessor type is used to identify the uci::type::RF_ResourceRequestQFT Accessor
         static const uci::base::accessorType::AccessorType rF_ResourceRequestQFT = 2362;

         //  This accessor type is used to identify the uci::type::RF_ResourceRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType rF_ResourceRequestStatusMDT = 2363;

         //  This accessor type is used to identify the uci::type::RF_ResourceRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType rF_ResourceRequestStatusMT = 2364;

         //  This accessor type is used to identify the uci::type::RF_ResourceRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType rF_ResourceRequestStatusQFT = 2365;

         //  This accessor type is used to identify the uci::type::RF_ThreadFeedbackType Accessor
         static const uci::base::accessorType::AccessorType rF_ThreadFeedbackType = 2366;

         //  This accessor type is used to identify the uci::type::RF_ThreadIdentifierType Accessor
         static const uci::base::accessorType::AccessorType rF_ThreadIdentifierType = 2367;

         //  This accessor type is used to identify the uci::type::RF_ThreadInstanceCommandType Accessor
         static const uci::base::accessorType::AccessorType rF_ThreadInstanceCommandType = 2368;

         //  This accessor type is used to identify the uci::type::RF_ThreadInstanceSetupCommandMDT Accessor
         static const uci::base::accessorType::AccessorType rF_ThreadInstanceSetupCommandMDT = 2369;

         //  This accessor type is used to identify the uci::type::RF_ThreadInstanceSetupCommandMT Accessor
         static const uci::base::accessorType::AccessorType rF_ThreadInstanceSetupCommandMT = 2370;

         //  This accessor type is used to identify the uci::type::RF_ThreadInstanceSetupCommandQFT Accessor
         static const uci::base::accessorType::AccessorType rF_ThreadInstanceSetupCommandQFT = 2371;

         //  This accessor type is used to identify the uci::type::RF_ThreadInstanceSetupCommandStatusMDT Accessor
         static const uci::base::accessorType::AccessorType rF_ThreadInstanceSetupCommandStatusMDT = 2372;

         //  This accessor type is used to identify the uci::type::RF_ThreadInstanceSetupCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType rF_ThreadInstanceSetupCommandStatusMT = 2373;

         //  This accessor type is used to identify the uci::type::RF_ThreadInstanceSetupCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType rF_ThreadInstanceSetupCommandStatusQFT = 2374;

         //  This accessor type is used to identify the uci::type::RF_TransmissionType Accessor
         static const uci::base::accessorType::AccessorType rF_TransmissionType = 2375;

         //  This accessor type is used to identify the uci::type::RFD_ComponentIdentifierType Accessor
         static const uci::base::accessorType::AccessorType rFD_ComponentIdentifierType = 2376;

         //  This accessor type is used to identify the uci::type::RFD_GainSettingType Accessor
         static const uci::base::accessorType::AccessorType rFD_GainSettingType = 2377;

         //  This accessor type is used to identify the uci::type::RFDC_StatusMDT Accessor
         static const uci::base::accessorType::AccessorType rFDC_StatusMDT = 2378;

         //  This accessor type is used to identify the uci::type::RFDC_StatusMT Accessor
         static const uci::base::accessorType::AccessorType rFDC_StatusMT = 2379;

         //  This accessor type is used to identify the uci::type::RFDC_StatusQFT Accessor
         static const uci::base::accessorType::AccessorType rFDC_StatusQFT = 2380;

         //  This accessor type is used to identify the uci::type::RGB_ColorControlType Accessor
         static const uci::base::accessorType::AccessorType rGB_ColorControlType = 2381;

         //  This accessor type is used to identify the uci::type::RiskSettingType Accessor
         static const uci::base::accessorType::AccessorType riskSettingType = 2382;

         //  This accessor type is used to identify the uci::type::RouteChoiceType Accessor
         static const uci::base::accessorType::AccessorType routeChoiceType = 2383;

         //  This accessor type is used to identify the uci::type::RouteConflictsType Accessor
         static const uci::base::accessorType::AccessorType routeConflictsType = 2384;

         //  This accessor type is used to identify the uci::type::RouteDeconflictionAssessmentDataType Accessor
         static const uci::base::accessorType::AccessorType routeDeconflictionAssessmentDataType = 2385;

         //  This accessor type is used to identify the uci::type::RouteDeconflictionAssessmentType Accessor
         static const uci::base::accessorType::AccessorType routeDeconflictionAssessmentType = 2386;

         //  This accessor type is used to identify the uci::type::RouteDeconflictionMissionPlanType Accessor
         static const uci::base::accessorType::AccessorType routeDeconflictionMissionPlanType = 2387;

         //  This accessor type is used to identify the uci::type::RouteDeconflictionRequestType Accessor
         static const uci::base::accessorType::AccessorType routeDeconflictionRequestType = 2388;

         //  This accessor type is used to identify the uci::type::RouteID_Type Accessor
         static const uci::base::accessorType::AccessorType routeID_Type = 2389;

         //  This accessor type is used to identify the uci::type::RouteMDT Accessor
         static const uci::base::accessorType::AccessorType routeMDT = 2390;

         //  This accessor type is used to identify the uci::type::RouteMT Accessor
         static const uci::base::accessorType::AccessorType routeMT = 2391;

         //  This accessor type is used to identify the uci::type::RoutePathDestinationType Accessor
         static const uci::base::accessorType::AccessorType routePathDestinationType = 2392;

         //  This accessor type is used to identify the uci::type::RoutePathEndPointStateType Accessor
         static const uci::base::accessorType::AccessorType routePathEndPointStateType = 2393;

         //  This accessor type is used to identify the uci::type::RoutePathEndPointType Accessor
         static const uci::base::accessorType::AccessorType routePathEndPointType = 2394;

         //  This accessor type is used to identify the uci::type::RoutePathType Accessor
         static const uci::base::accessorType::AccessorType routePathType = 2395;

         //  This accessor type is used to identify the uci::type::RoutePlanningApprovalPolicyType Accessor
         static const uci::base::accessorType::AccessorType routePlanningApprovalPolicyType = 2396;

         //  This accessor type is used to identify the uci::type::RoutePlanningAutonomousRerouteType Accessor
         static const uci::base::accessorType::AccessorType routePlanningAutonomousRerouteType = 2397;

         //  This accessor type is used to identify the uci::type::RoutePlanningGuidelineType Accessor
         static const uci::base::accessorType::AccessorType routePlanningGuidelineType = 2398;

         //  This accessor type is used to identify the uci::type::RoutePlanningPolicyType Accessor
         static const uci::base::accessorType::AccessorType routePlanningPolicyType = 2399;

         //  This accessor type is used to identify the uci::type::RoutePlanningResultCaseType Accessor
         static const uci::base::accessorType::AccessorType routePlanningResultCaseType = 2400;

         //  This accessor type is used to identify the uci::type::RoutePlanningTriggerApprovalCaseType Accessor
         static const uci::base::accessorType::AccessorType routePlanningTriggerApprovalCaseType = 2401;

         //  This accessor type is used to identify the uci::type::RouteQFT Accessor
         static const uci::base::accessorType::AccessorType routeQFT = 2402;

         //  This accessor type is used to identify the uci::type::RoutesType Accessor
         static const uci::base::accessorType::AccessorType routesType = 2403;

         //  This accessor type is used to identify the uci::type::RouteThreatAssessmentDataType Accessor
         static const uci::base::accessorType::AccessorType routeThreatAssessmentDataType = 2404;

         //  This accessor type is used to identify the uci::type::RouteThreatAssessmentType Accessor
         static const uci::base::accessorType::AccessorType routeThreatAssessmentType = 2405;

         //  This accessor type is used to identify the uci::type::RouteType Accessor
         static const uci::base::accessorType::AccessorType routeType = 2406;

         //  This accessor type is used to identify the uci::type::RouteValidationAssessmentDataType Accessor
         static const uci::base::accessorType::AccessorType routeValidationAssessmentDataType = 2407;

         //  This accessor type is used to identify the uci::type::RouteValidationAssessmentType Accessor
         static const uci::base::accessorType::AccessorType routeValidationAssessmentType = 2408;

         //  This accessor type is used to identify the uci::type::RouteValidationInvalidPathType Accessor
         static const uci::base::accessorType::AccessorType routeValidationInvalidPathType = 2409;

         //  This accessor type is used to identify the uci::type::RouteValidationInvalidSegmentType Accessor
         static const uci::base::accessorType::AccessorType routeValidationInvalidSegmentType = 2410;

         //  This accessor type is used to identify the uci::type::RouteValidationRequestType Accessor
         static const uci::base::accessorType::AccessorType routeValidationRequestType = 2411;

         //  This accessor type is used to identify the uci::type::RouteVulnerabilityMetricsAssessmentDataType Accessor
         static const uci::base::accessorType::AccessorType routeVulnerabilityMetricsAssessmentDataType = 2412;

         //  This accessor type is used to identify the uci::type::RouteVulnerabilityMetricsAssessmentType Accessor
         static const uci::base::accessorType::AccessorType routeVulnerabilityMetricsAssessmentType = 2413;

         //  This accessor type is used to identify the uci::type::RouteVulnerabilityMetricsRequestType Accessor
         static const uci::base::accessorType::AccessorType routeVulnerabilityMetricsRequestType = 2414;

         //  This accessor type is used to identify the uci::type::RouteVulnerabilityTriggerDataType Accessor
         static const uci::base::accessorType::AccessorType routeVulnerabilityTriggerDataType = 2415;

         //  This accessor type is used to identify the uci::type::RouteVulnerabilityType Accessor
         static const uci::base::accessorType::AccessorType routeVulnerabilityType = 2416;

         //  This accessor type is used to identify the uci::type::RTN_AccelerationType Accessor
         static const uci::base::accessorType::AccessorType rTN_AccelerationType = 2417;

         //  This accessor type is used to identify the uci::type::RTN_KinematicsType Accessor
         static const uci::base::accessorType::AccessorType rTN_KinematicsType = 2418;

         //  This accessor type is used to identify the uci::type::RTN_PositionType Accessor
         static const uci::base::accessorType::AccessorType rTN_PositionType = 2419;

         //  This accessor type is used to identify the uci::type::RTN_VelocityType Accessor
         static const uci::base::accessorType::AccessorType rTN_VelocityType = 2420;

         //  This accessor type is used to identify the uci::type::RuleResponseType Accessor
         static const uci::base::accessorType::AccessorType ruleResponseType = 2421;

         //  This accessor type is used to identify the uci::type::RuleTriggerMetaTaskType Accessor
         static const uci::base::accessorType::AccessorType ruleTriggerMetaTaskType = 2422;

         //  This accessor type is used to identify the uci::type::RuleTriggerTaskTargetType Accessor
         static const uci::base::accessorType::AccessorType ruleTriggerTaskTargetType = 2423;

         //  This accessor type is used to identify the uci::type::RuleTriggerTaskType Accessor
         static const uci::base::accessorType::AccessorType ruleTriggerTaskType = 2424;

         //  This accessor type is used to identify the uci::type::RuleTriggerType Accessor
         static const uci::base::accessorType::AccessorType ruleTriggerType = 2425;

         //  This accessor type is used to identify the uci::type::RuleType Accessor
         static const uci::base::accessorType::AccessorType ruleType = 2426;

         //  This accessor type is used to identify the uci::type::RunwayCoordinatesType Accessor
         static const uci::base::accessorType::AccessorType runwayCoordinatesType = 2427;

         //  This accessor type is used to identify the uci::type::RunwayID_Type Accessor
         static const uci::base::accessorType::AccessorType runwayID_Type = 2428;

         //  This accessor type is used to identify the uci::type::RunwayMDT Accessor
         static const uci::base::accessorType::AccessorType runwayMDT = 2429;

         //  This accessor type is used to identify the uci::type::RunwayMT Accessor
         static const uci::base::accessorType::AccessorType runwayMT = 2430;

         //  This accessor type is used to identify the uci::type::RunwayQFT Accessor
         static const uci::base::accessorType::AccessorType runwayQFT = 2431;

         //  This accessor type is used to identify the uci::type::RunwayUsageDetailsType Accessor
         static const uci::base::accessorType::AccessorType runwayUsageDetailsType = 2432;

         //  This accessor type is used to identify the uci::type::RxBeamType Accessor
         static const uci::base::accessorType::AccessorType rxBeamType = 2433;

         //  This accessor type is used to identify the uci::type::SafeAltitudeType Accessor
         static const uci::base::accessorType::AccessorType safeAltitudeType = 2434;

         //  This accessor type is used to identify the uci::type::SAR_ActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType sAR_ActivityCommandType = 2435;

         //  This accessor type is used to identify the uci::type::SAR_ActivityMDT Accessor
         static const uci::base::accessorType::AccessorType sAR_ActivityMDT = 2436;

         //  This accessor type is used to identify the uci::type::SAR_ActivityMT Accessor
         static const uci::base::accessorType::AccessorType sAR_ActivityMT = 2437;

         //  This accessor type is used to identify the uci::type::SAR_ActivityQFT Accessor
         static const uci::base::accessorType::AccessorType sAR_ActivityQFT = 2438;

         //  This accessor type is used to identify the uci::type::SAR_ActivityStatusDetailType Accessor
         static const uci::base::accessorType::AccessorType sAR_ActivityStatusDetailType = 2439;

      } // Namespace: accessorType
      //  
      namespace sAR_ActivityStatusDetailType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::SAR_ActivityStatusDetailType::AchievableEllipticity Accessor
            static const uci::base::accessorType::AccessorType achievableEllipticity = 2440;

         } // Namespace: accessorType
      } // Namespace: sAR_ActivityStatusDetailType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::SAR_ActivityType Accessor
         static const uci::base::accessorType::AccessorType sAR_ActivityType = 2441;

         //  This accessor type is used to identify the uci::type::SAR_AutomaticTargetDetectionParametersType Accessor
         static const uci::base::accessorType::AccessorType sAR_AutomaticTargetDetectionParametersType = 2442;

         //  This accessor type is used to identify the uci::type::SAR_CapabilityCommandSubCapabilityType Accessor
         static const uci::base::accessorType::AccessorType sAR_CapabilityCommandSubCapabilityType = 2443;

         //  This accessor type is used to identify the uci::type::SAR_CapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType sAR_CapabilityCommandType = 2444;

         //  This accessor type is used to identify the uci::type::SAR_CapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType sAR_CapabilityMDT = 2445;

         //  This accessor type is used to identify the uci::type::SAR_CapabilityMT Accessor
         static const uci::base::accessorType::AccessorType sAR_CapabilityMT = 2446;

         //  This accessor type is used to identify the uci::type::SAR_CapabilityOptionsType Accessor
         static const uci::base::accessorType::AccessorType sAR_CapabilityOptionsType = 2447;

         //  This accessor type is used to identify the uci::type::SAR_CapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType sAR_CapabilityQFT = 2448;

         //  This accessor type is used to identify the uci::type::SAR_CapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType sAR_CapabilityStatusMDT = 2449;

         //  This accessor type is used to identify the uci::type::SAR_CapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType sAR_CapabilityStatusMT = 2450;

         //  This accessor type is used to identify the uci::type::SAR_CapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType sAR_CapabilityStatusQFT = 2451;

         //  This accessor type is used to identify the uci::type::SAR_CapabilityStatusType Accessor
         static const uci::base::accessorType::AccessorType sAR_CapabilityStatusType = 2452;

         //  This accessor type is used to identify the uci::type::SAR_CapabilityType Accessor
         static const uci::base::accessorType::AccessorType sAR_CapabilityType = 2453;

         //  This accessor type is used to identify the uci::type::SAR_CoherentChangeDetectionParametersType Accessor
         static const uci::base::accessorType::AccessorType sAR_CoherentChangeDetectionParametersType = 2454;

         //  This accessor type is used to identify the uci::type::SAR_CollectionConstraintsQualityType Accessor
         static const uci::base::accessorType::AccessorType sAR_CollectionConstraintsQualityType = 2455;

         //  This accessor type is used to identify the uci::type::SAR_CollectionConstraintsType Accessor
         static const uci::base::accessorType::AccessorType sAR_CollectionConstraintsType = 2456;

         //  This accessor type is used to identify the uci::type::SAR_CollectionOptionsType Accessor
         static const uci::base::accessorType::AccessorType sAR_CollectionOptionsType = 2457;

         //  This accessor type is used to identify the uci::type::SAR_CommandMDT Accessor
         static const uci::base::accessorType::AccessorType sAR_CommandMDT = 2458;

         //  This accessor type is used to identify the uci::type::SAR_CommandMT Accessor
         static const uci::base::accessorType::AccessorType sAR_CommandMT = 2459;

         //  This accessor type is used to identify the uci::type::SAR_CommandQFT Accessor
         static const uci::base::accessorType::AccessorType sAR_CommandQFT = 2460;

         //  This accessor type is used to identify the uci::type::SAR_CommandResponseType Accessor
         static const uci::base::accessorType::AccessorType sAR_CommandResponseType = 2461;

         //  This accessor type is used to identify the uci::type::SAR_CommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType sAR_CommandStatusMT = 2462;

         //  This accessor type is used to identify the uci::type::SAR_CommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType sAR_CommandStatusQFT = 2463;

         //  This accessor type is used to identify the uci::type::SAR_CommandType Accessor
         static const uci::base::accessorType::AccessorType sAR_CommandType = 2464;

         //  This accessor type is used to identify the uci::type::SAR_DesiredWaveformType Accessor
         static const uci::base::accessorType::AccessorType sAR_DesiredWaveformType = 2465;

         //  This accessor type is used to identify the uci::type::SAR_DynamicImageParametersType Accessor
         static const uci::base::accessorType::AccessorType sAR_DynamicImageParametersType = 2466;

      } // Namespace: accessorType
      //  
      namespace sAR_DynamicImageParametersType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::SAR_DynamicImageParametersType::NumSubApertures Accessor
            static const uci::base::accessorType::AccessorType numSubApertures = 2467;

            //  This accessor type is used to identify the uci::type::SAR_DynamicImageParametersType::ResampleFactor Accessor
            static const uci::base::accessorType::AccessorType resampleFactor = 2468;

         } // Namespace: accessorType
      } // Namespace: sAR_DynamicImageParametersType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::SAR_PredictedActivityDetailType Accessor
         static const uci::base::accessorType::AccessorType sAR_PredictedActivityDetailType = 2469;

      } // Namespace: accessorType
      //  
      namespace sAR_PredictedActivityDetailType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::SAR_PredictedActivityDetailType::EstimatedAchievableEllipticity
            //  Accessor
            static const uci::base::accessorType::AccessorType estimatedAchievableEllipticity = 2470;

         } // Namespace: accessorType
      } // Namespace: sAR_PredictedActivityDetailType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::SAR_PredictedCollectionsType Accessor
         static const uci::base::accessorType::AccessorType sAR_PredictedCollectionsType = 2471;

         //  This accessor type is used to identify the uci::type::SAR_ProcessingParametersType Accessor
         static const uci::base::accessorType::AccessorType sAR_ProcessingParametersType = 2472;

         //  This accessor type is used to identify the uci::type::SAR_SettingsCommandMDT Accessor
         static const uci::base::accessorType::AccessorType sAR_SettingsCommandMDT = 2473;

         //  This accessor type is used to identify the uci::type::SAR_SettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType sAR_SettingsCommandMT = 2474;

         //  This accessor type is used to identify the uci::type::SAR_SettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType sAR_SettingsCommandQFT = 2475;

         //  This accessor type is used to identify the uci::type::SAR_SettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType sAR_SettingsCommandStatusMT = 2476;

         //  This accessor type is used to identify the uci::type::SAR_SettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType sAR_SettingsCommandStatusQFT = 2477;

         //  This accessor type is used to identify the uci::type::SAR_SpecificDataType Accessor
         static const uci::base::accessorType::AccessorType sAR_SpecificDataType = 2478;

         //  This accessor type is used to identify the uci::type::SAR_SubCapabilityClutterParametersType Accessor
         static const uci::base::accessorType::AccessorType sAR_SubCapabilityClutterParametersType = 2479;

         //  This accessor type is used to identify the uci::type::SAR_SubCapabilityNparDetailsType Accessor
         static const uci::base::accessorType::AccessorType sAR_SubCapabilityNparDetailsType = 2480;

         //  This accessor type is used to identify the uci::type::SAR_SubCapabilityType Accessor
         static const uci::base::accessorType::AccessorType sAR_SubCapabilityType = 2481;

         //  This accessor type is used to identify the uci::type::SAR_TargetType Accessor
         static const uci::base::accessorType::AccessorType sAR_TargetType = 2482;

         //  This accessor type is used to identify the uci::type::SAR_TaskBaseType Accessor
         static const uci::base::accessorType::AccessorType sAR_TaskBaseType = 2483;

         //  This accessor type is used to identify the uci::type::SAR_TaskTargetType Accessor
         static const uci::base::accessorType::AccessorType sAR_TaskTargetType = 2484;

         //  This accessor type is used to identify the uci::type::SAR_TaskType Accessor
         static const uci::base::accessorType::AccessorType sAR_TaskType = 2485;

         //  This accessor type is used to identify the uci::type::SAR_TwoColorMultiViewParametersType Accessor
         static const uci::base::accessorType::AccessorType sAR_TwoColorMultiViewParametersType = 2486;

         //  This accessor type is used to identify the uci::type::SAR_WaveformType Accessor
         static const uci::base::accessorType::AccessorType sAR_WaveformType = 2487;

         //  This accessor type is used to identify the uci::type::SatelliteC2_IdentityType Accessor
         static const uci::base::accessorType::AccessorType satelliteC2_IdentityType = 2488;

         //  This accessor type is used to identify the uci::type::SatelliteEnduranceType Accessor
         static const uci::base::accessorType::AccessorType satelliteEnduranceType = 2489;

         //  This accessor type is used to identify the uci::type::SatelliteIdentifierType Accessor
         static const uci::base::accessorType::AccessorType satelliteIdentifierType = 2490;

         //  This accessor type is used to identify the uci::type::SatelliteIdentityType Accessor
         static const uci::base::accessorType::AccessorType satelliteIdentityType = 2491;

         //  This accessor type is used to identify the uci::type::ScanDataBaseType Accessor
         static const uci::base::accessorType::AccessorType scanDataBaseType = 2492;

         //  This accessor type is used to identify the uci::type::ScanDataType Accessor
         static const uci::base::accessorType::AccessorType scanDataType = 2493;

         //  This accessor type is used to identify the uci::type::ScheduleType Accessor
         static const uci::base::accessorType::AccessorType scheduleType = 2494;

         //  This accessor type is used to identify the uci::type::SearchAndRescueTaskType Accessor
         static const uci::base::accessorType::AccessorType searchAndRescueTaskType = 2495;

         //  This accessor type is used to identify the uci::type::SearchTrackActivityDetailsType Accessor
         static const uci::base::accessorType::AccessorType searchTrackActivityDetailsType = 2496;

         //  This accessor type is used to identify the uci::type::SecondaryControllerType Accessor
         static const uci::base::accessorType::AccessorType secondaryControllerType = 2497;

         //  This accessor type is used to identify the uci::type::SECTGA_Type Accessor
         static const uci::base::accessorType::AccessorType sECTGA_Type = 2498;

      } // Namespace: accessorType
      //  
      namespace sECTGA_Type {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::SECTGA_Type::SEC_ID Accessor
            static const uci::base::accessorType::AccessorType sEC_ID = 2499;

            //  This accessor type is used to identify the uci::type::SECTGA_Type::SEC_BE Accessor
            static const uci::base::accessorType::AccessorType sEC_BE = 2500;

         } // Namespace: accessorType
      } // Namespace: sECTGA_Type
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::SectorType Accessor
         static const uci::base::accessorType::AccessorType sectorType = 2501;

         //  This accessor type is used to identify the uci::type::SecurityAccessType Accessor
         static const uci::base::accessorType::AccessorType securityAccessType = 2502;

         //  This accessor type is used to identify the uci::type::SecurityAuditManagementType Accessor
         static const uci::base::accessorType::AccessorType securityAuditManagementType = 2503;

         //  This accessor type is used to identify the uci::type::SecurityAuditRecordMDT Accessor
         static const uci::base::accessorType::AccessorType securityAuditRecordMDT = 2504;

         //  This accessor type is used to identify the uci::type::SecurityAuditRecordMT Accessor
         static const uci::base::accessorType::AccessorType securityAuditRecordMT = 2505;

         //  This accessor type is used to identify the uci::type::SecurityAuditRecordQFT Accessor
         static const uci::base::accessorType::AccessorType securityAuditRecordQFT = 2506;

         //  This accessor type is used to identify the uci::type::SecurityAuthenticationType Accessor
         static const uci::base::accessorType::AccessorType securityAuthenticationType = 2507;

         //  This accessor type is used to identify the uci::type::SecurityEventID_Type Accessor
         static const uci::base::accessorType::AccessorType securityEventID_Type = 2508;

         //  This accessor type is used to identify the uci::type::SecurityEventType Accessor
         static const uci::base::accessorType::AccessorType securityEventType = 2509;

         //  This accessor type is used to identify the uci::type::SecurityFileManagementType Accessor
         static const uci::base::accessorType::AccessorType securityFileManagementType = 2510;

         //  This accessor type is used to identify the uci::type::SecurityInformationFilterRangeType Accessor
         static const uci::base::accessorType::AccessorType securityInformationFilterRangeType = 2511;

         //  This accessor type is used to identify the uci::type::SecurityInformationType Accessor
         static const uci::base::accessorType::AccessorType securityInformationType = 2512;

         //  This accessor type is used to identify the uci::type::SecurityIntegrityType Accessor
         static const uci::base::accessorType::AccessorType securityIntegrityType = 2513;

         //  This accessor type is used to identify the uci::type::SecurityIntrusionDetectionType Accessor
         static const uci::base::accessorType::AccessorType securityIntrusionDetectionType = 2514;

         //  This accessor type is used to identify the uci::type::SecurityKeyManagementType Accessor
         static const uci::base::accessorType::AccessorType securityKeyManagementType = 2515;

         //  This accessor type is used to identify the uci::type::SecurityPolicyManagementType Accessor
         static const uci::base::accessorType::AccessorType securityPolicyManagementType = 2516;

         //  This accessor type is used to identify the uci::type::SecuritySanitizationType Accessor
         static const uci::base::accessorType::AccessorType securitySanitizationType = 2517;

         //  This accessor type is used to identify the uci::type::SecuritySystemType Accessor
         static const uci::base::accessorType::AccessorType securitySystemType = 2518;

         //  This accessor type is used to identify the uci::type::SegmentAssociationType Accessor
         static const uci::base::accessorType::AccessorType segmentAssociationType = 2519;

         //  This accessor type is used to identify the uci::type::SegmentCaptureType Accessor
         static const uci::base::accessorType::AccessorType segmentCaptureType = 2520;

         //  This accessor type is used to identify the uci::type::SegmentCompletedType Accessor
         static const uci::base::accessorType::AccessorType segmentCompletedType = 2521;

         //  This accessor type is used to identify the uci::type::SegmentConflictsType Accessor
         static const uci::base::accessorType::AccessorType segmentConflictsType = 2522;

         //  This accessor type is used to identify the uci::type::SegmentEstimateType Accessor
         static const uci::base::accessorType::AccessorType segmentEstimateType = 2523;

         //  This accessor type is used to identify the uci::type::SegmentID_Type Accessor
         static const uci::base::accessorType::AccessorType segmentID_Type = 2524;

         //  This accessor type is used to identify the uci::type::SegmentRangeType Accessor
         static const uci::base::accessorType::AccessorType segmentRangeType = 2525;

         //  This accessor type is used to identify the uci::type::SelectAntennaType Accessor
         static const uci::base::accessorType::AccessorType selectAntennaType = 2526;

         //  This accessor type is used to identify the uci::type::SelectComponentResourceType Accessor
         static const uci::base::accessorType::AccessorType selectComponentResourceType = 2527;

         //  This accessor type is used to identify the uci::type::SelectPayloadResourceType Accessor
         static const uci::base::accessorType::AccessorType selectPayloadResourceType = 2528;

         //  This accessor type is used to identify the uci::type::SelfDefenseStatusMDT Accessor
         static const uci::base::accessorType::AccessorType selfDefenseStatusMDT = 2529;

         //  This accessor type is used to identify the uci::type::SelfDefenseStatusMT Accessor
         static const uci::base::accessorType::AccessorType selfDefenseStatusMT = 2530;

         //  This accessor type is used to identify the uci::type::SelfDefenseStatusQFT Accessor
         static const uci::base::accessorType::AccessorType selfDefenseStatusQFT = 2531;

         //  This accessor type is used to identify the uci::type::SensorConeType Accessor
         static const uci::base::accessorType::AccessorType sensorConeType = 2532;

         //  This accessor type is used to identify the uci::type::SensorLOS_Type Accessor
         static const uci::base::accessorType::AccessorType sensorLOS_Type = 2533;

         //  This accessor type is used to identify the uci::type::SensorReferencedCoverageAreaType Accessor
         static const uci::base::accessorType::AccessorType sensorReferencedCoverageAreaType = 2534;

         //  This accessor type is used to identify the uci::type::SeparationParametersType Accessor
         static const uci::base::accessorType::AccessorType separationParametersType = 2535;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationChangeRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationChangeRequestID_Type = 2536;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationChangeRequestMDT Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationChangeRequestMDT = 2537;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationChangeRequestMT Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationChangeRequestMT = 2538;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationChangeRequestQFT Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationChangeRequestQFT = 2539;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationChangeRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationChangeRequestStatusMDT = 2540;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationChangeRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationChangeRequestStatusMT = 2541;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationChangeRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationChangeRequestStatusQFT = 2542;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationChangeType Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationChangeType = 2543;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationParamsMDT Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationParamsMDT = 2544;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationParamsMT Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationParamsMT = 2545;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationParamsQFT Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationParamsQFT = 2546;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationParamsRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationParamsRequestID_Type = 2547;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationParamsRequestMDT Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationParamsRequestMDT = 2548;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationParamsRequestMT Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationParamsRequestMT = 2549;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationParamsRequestQFT Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationParamsRequestQFT = 2550;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationParamsRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationParamsRequestStatusMDT = 2551;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationParamsRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationParamsRequestStatusMT = 2552;

         //  This accessor type is used to identify the uci::type::ServiceConfigurationParamsRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType serviceConfigurationParamsRequestStatusQFT = 2553;

         //  This accessor type is used to identify the uci::type::ServiceErrorReportID_Type Accessor
         static const uci::base::accessorType::AccessorType serviceErrorReportID_Type = 2554;

         //  This accessor type is used to identify the uci::type::ServiceErrorReportMDT Accessor
         static const uci::base::accessorType::AccessorType serviceErrorReportMDT = 2555;

         //  This accessor type is used to identify the uci::type::ServiceErrorReportMT Accessor
         static const uci::base::accessorType::AccessorType serviceErrorReportMT = 2556;

         //  This accessor type is used to identify the uci::type::ServiceErrorReportQFT Accessor
         static const uci::base::accessorType::AccessorType serviceErrorReportQFT = 2557;

         //  This accessor type is used to identify the uci::type::ServiceID_Type Accessor
         static const uci::base::accessorType::AccessorType serviceID_Type = 2558;

         //  This accessor type is used to identify the uci::type::ServiceLifecycleRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType serviceLifecycleRequestID_Type = 2559;

         //  This accessor type is used to identify the uci::type::ServiceLifecycleRequestMDT Accessor
         static const uci::base::accessorType::AccessorType serviceLifecycleRequestMDT = 2560;

         //  This accessor type is used to identify the uci::type::ServiceLifecycleRequestMT Accessor
         static const uci::base::accessorType::AccessorType serviceLifecycleRequestMT = 2561;

         //  This accessor type is used to identify the uci::type::ServiceLifecycleRequestQFT Accessor
         static const uci::base::accessorType::AccessorType serviceLifecycleRequestQFT = 2562;

         //  This accessor type is used to identify the uci::type::ServiceLifecycleRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType serviceLifecycleRequestStatusMDT = 2563;

         //  This accessor type is used to identify the uci::type::ServiceLifecycleRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType serviceLifecycleRequestStatusMT = 2564;

         //  This accessor type is used to identify the uci::type::ServiceLifecycleRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType serviceLifecycleRequestStatusQFT = 2565;

         //  This accessor type is used to identify the uci::type::ServiceSettingsCommandID_Type Accessor
         static const uci::base::accessorType::AccessorType serviceSettingsCommandID_Type = 2566;

         //  This accessor type is used to identify the uci::type::ServiceSettingsCommandMDT Accessor
         static const uci::base::accessorType::AccessorType serviceSettingsCommandMDT = 2567;

         //  This accessor type is used to identify the uci::type::ServiceSettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType serviceSettingsCommandMT = 2568;

         //  This accessor type is used to identify the uci::type::ServiceSettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType serviceSettingsCommandQFT = 2569;

         //  This accessor type is used to identify the uci::type::ServiceSettingsCommandStatusMDT Accessor
         static const uci::base::accessorType::AccessorType serviceSettingsCommandStatusMDT = 2570;

         //  This accessor type is used to identify the uci::type::ServiceSettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType serviceSettingsCommandStatusMT = 2571;

         //  This accessor type is used to identify the uci::type::ServiceSettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType serviceSettingsCommandStatusQFT = 2572;

         //  This accessor type is used to identify the uci::type::ServiceSettingsParameterType Accessor
         static const uci::base::accessorType::AccessorType serviceSettingsParameterType = 2573;

         //  This accessor type is used to identify the uci::type::ServiceSettingType Accessor
         static const uci::base::accessorType::AccessorType serviceSettingType = 2574;

         //  This accessor type is used to identify the uci::type::ServiceStatusMDT Accessor
         static const uci::base::accessorType::AccessorType serviceStatusMDT = 2575;

         //  This accessor type is used to identify the uci::type::ServiceStatusMT Accessor
         static const uci::base::accessorType::AccessorType serviceStatusMT = 2576;

         //  This accessor type is used to identify the uci::type::ServiceStatusQFT Accessor
         static const uci::base::accessorType::AccessorType serviceStatusQFT = 2577;

         //  This accessor type is used to identify the uci::type::ServiceStatusRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType serviceStatusRequestID_Type = 2578;

         //  This accessor type is used to identify the uci::type::ServiceStatusRequestMDT Accessor
         static const uci::base::accessorType::AccessorType serviceStatusRequestMDT = 2579;

         //  This accessor type is used to identify the uci::type::ServiceStatusRequestMT Accessor
         static const uci::base::accessorType::AccessorType serviceStatusRequestMT = 2580;

         //  This accessor type is used to identify the uci::type::ServiceStatusRequestQFT Accessor
         static const uci::base::accessorType::AccessorType serviceStatusRequestQFT = 2581;

         //  This accessor type is used to identify the uci::type::ServiceStatusRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType serviceStatusRequestStatusMDT = 2582;

         //  This accessor type is used to identify the uci::type::ServiceStatusRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType serviceStatusRequestStatusMT = 2583;

         //  This accessor type is used to identify the uci::type::ServiceStatusRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType serviceStatusRequestStatusQFT = 2584;

         //  This accessor type is used to identify the uci::type::SetupRF_ThreadInstanceType Accessor
         static const uci::base::accessorType::AccessorType setupRF_ThreadInstanceType = 2585;

         //  This accessor type is used to identify the uci::type::SharedRF_ApertureActivityBaseType Accessor
         static const uci::base::accessorType::AccessorType sharedRF_ApertureActivityBaseType = 2586;

         //  This accessor type is used to identify the uci::type::SignalAmplitudeType Accessor
         static const uci::base::accessorType::AccessorType signalAmplitudeType = 2587;

         //  This accessor type is used to identify the uci::type::SignalEmitterKinematicsFixedGeolocationStatusType Accessor
         static const uci::base::accessorType::AccessorType signalEmitterKinematicsFixedGeolocationStatusType = 2588;

         //  This accessor type is used to identify the uci::type::SignalEmitterKinematicsFixedType Accessor
         static const uci::base::accessorType::AccessorType signalEmitterKinematicsFixedType = 2589;

         //  This accessor type is used to identify the uci::type::SignalEmitterKinematicsPositionType Accessor
         static const uci::base::accessorType::AccessorType signalEmitterKinematicsPositionType = 2590;

         //  This accessor type is used to identify the uci::type::SignalEmitterKinematicsType Accessor
         static const uci::base::accessorType::AccessorType signalEmitterKinematicsType = 2591;

         //  This accessor type is used to identify the uci::type::SignalID_Type Accessor
         static const uci::base::accessorType::AccessorType signalID_Type = 2592;

         //  This accessor type is used to identify the uci::type::SignalIdentityQueryType Accessor
         static const uci::base::accessorType::AccessorType signalIdentityQueryType = 2593;

      } // Namespace: accessorType
      //  
      namespace signalIdentityQueryType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::SignalIdentityQueryType::NotationWildcard Accessor
            static const uci::base::accessorType::AccessorType notationWildcard = 2594;

         } // Namespace: accessorType
      } // Namespace: signalIdentityQueryType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::SignalIdentityType Accessor
         static const uci::base::accessorType::AccessorType signalIdentityType = 2595;

         //  This accessor type is used to identify the uci::type::SignalLOB_Type Accessor
         static const uci::base::accessorType::AccessorType signalLOB_Type = 2596;

         //  This accessor type is used to identify the uci::type::SignalNavDataType Accessor
         static const uci::base::accessorType::AccessorType signalNavDataType = 2597;

         //  This accessor type is used to identify the uci::type::SignalParametricQueryType Accessor
         static const uci::base::accessorType::AccessorType signalParametricQueryType = 2598;

         //  This accessor type is used to identify the uci::type::SignalPulseWidthBaseType Accessor
         static const uci::base::accessorType::AccessorType signalPulseWidthBaseType = 2599;

         //  This accessor type is used to identify the uci::type::SignalPulseWidthType Accessor
         static const uci::base::accessorType::AccessorType signalPulseWidthType = 2600;

         //  This accessor type is used to identify the uci::type::SignalRemoveInfoType Accessor
         static const uci::base::accessorType::AccessorType signalRemoveInfoType = 2601;

         //  This accessor type is used to identify the uci::type::SignalReportID_Type Accessor
         static const uci::base::accessorType::AccessorType signalReportID_Type = 2602;

         //  This accessor type is used to identify the uci::type::SignalReportMDT Accessor
         static const uci::base::accessorType::AccessorType signalReportMDT = 2603;

         //  This accessor type is used to identify the uci::type::SignalReportMT Accessor
         static const uci::base::accessorType::AccessorType signalReportMT = 2604;

         //  This accessor type is used to identify the uci::type::SignalReportQFT Accessor
         static const uci::base::accessorType::AccessorType signalReportQFT = 2605;

         //  This accessor type is used to identify the uci::type::SignalSourceType Accessor
         static const uci::base::accessorType::AccessorType signalSourceType = 2606;

         //  This accessor type is used to identify the uci::type::SignalSummaryType Accessor
         static const uci::base::accessorType::AccessorType signalSummaryType = 2607;

         //  This accessor type is used to identify the uci::type::SignalType Accessor
         static const uci::base::accessorType::AccessorType signalType = 2608;

         //  This accessor type is used to identify the uci::type::SlantRangeConstraintsType Accessor
         static const uci::base::accessorType::AccessorType slantRangeConstraintsType = 2609;

         //  This accessor type is used to identify the uci::type::SlavedNavigationType Accessor
         static const uci::base::accessorType::AccessorType slavedNavigationType = 2610;

         //  This accessor type is used to identify the uci::type::SMTI_ActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType sMTI_ActivityCommandType = 2611;

         //  This accessor type is used to identify the uci::type::SMTI_ActivityMDT Accessor
         static const uci::base::accessorType::AccessorType sMTI_ActivityMDT = 2612;

         //  This accessor type is used to identify the uci::type::SMTI_ActivityMT Accessor
         static const uci::base::accessorType::AccessorType sMTI_ActivityMT = 2613;

         //  This accessor type is used to identify the uci::type::SMTI_ActivityQFT Accessor
         static const uci::base::accessorType::AccessorType sMTI_ActivityQFT = 2614;

         //  This accessor type is used to identify the uci::type::SMTI_ActivityType Accessor
         static const uci::base::accessorType::AccessorType sMTI_ActivityType = 2615;

         //  This accessor type is used to identify the uci::type::SMTI_CapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType sMTI_CapabilityCommandType = 2616;

         //  This accessor type is used to identify the uci::type::SMTI_CapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType sMTI_CapabilityMDT = 2617;

         //  This accessor type is used to identify the uci::type::SMTI_CapabilityMT Accessor
         static const uci::base::accessorType::AccessorType sMTI_CapabilityMT = 2618;

         //  This accessor type is used to identify the uci::type::SMTI_CapabilityOptionsType Accessor
         static const uci::base::accessorType::AccessorType sMTI_CapabilityOptionsType = 2619;

         //  This accessor type is used to identify the uci::type::SMTI_CapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType sMTI_CapabilityQFT = 2620;

         //  This accessor type is used to identify the uci::type::SMTI_CapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType sMTI_CapabilityStatusMDT = 2621;

         //  This accessor type is used to identify the uci::type::SMTI_CapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType sMTI_CapabilityStatusMT = 2622;

         //  This accessor type is used to identify the uci::type::SMTI_CapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType sMTI_CapabilityStatusQFT = 2623;

         //  This accessor type is used to identify the uci::type::SMTI_CapabilityType Accessor
         static const uci::base::accessorType::AccessorType sMTI_CapabilityType = 2624;

         //  This accessor type is used to identify the uci::type::SMTI_CollectionConstraintsQualityType Accessor
         static const uci::base::accessorType::AccessorType sMTI_CollectionConstraintsQualityType = 2625;

         //  This accessor type is used to identify the uci::type::SMTI_CollectionConstraintsType Accessor
         static const uci::base::accessorType::AccessorType sMTI_CollectionConstraintsType = 2626;

         //  This accessor type is used to identify the uci::type::SMTI_CollectionOptionsType Accessor
         static const uci::base::accessorType::AccessorType sMTI_CollectionOptionsType = 2627;

         //  This accessor type is used to identify the uci::type::SMTI_CommandMDT Accessor
         static const uci::base::accessorType::AccessorType sMTI_CommandMDT = 2628;

         //  This accessor type is used to identify the uci::type::SMTI_CommandMT Accessor
         static const uci::base::accessorType::AccessorType sMTI_CommandMT = 2629;

         //  This accessor type is used to identify the uci::type::SMTI_CommandQFT Accessor
         static const uci::base::accessorType::AccessorType sMTI_CommandQFT = 2630;

         //  This accessor type is used to identify the uci::type::SMTI_CommandResponseType Accessor
         static const uci::base::accessorType::AccessorType sMTI_CommandResponseType = 2631;

         //  This accessor type is used to identify the uci::type::SMTI_CommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType sMTI_CommandStatusMT = 2632;

         //  This accessor type is used to identify the uci::type::SMTI_CommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType sMTI_CommandStatusQFT = 2633;

         //  This accessor type is used to identify the uci::type::SMTI_CommandType Accessor
         static const uci::base::accessorType::AccessorType sMTI_CommandType = 2634;

         //  This accessor type is used to identify the uci::type::SMTI_FalseAlarmType Accessor
         static const uci::base::accessorType::AccessorType sMTI_FalseAlarmType = 2635;

         //  This accessor type is used to identify the uci::type::SMTI_PredictedCollectionsType Accessor
         static const uci::base::accessorType::AccessorType sMTI_PredictedCollectionsType = 2636;

         //  This accessor type is used to identify the uci::type::SMTI_ResolutionType Accessor
         static const uci::base::accessorType::AccessorType sMTI_ResolutionType = 2637;

         //  This accessor type is used to identify the uci::type::SMTI_SettingsCommandMDT Accessor
         static const uci::base::accessorType::AccessorType sMTI_SettingsCommandMDT = 2638;

         //  This accessor type is used to identify the uci::type::SMTI_SettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType sMTI_SettingsCommandMT = 2639;

         //  This accessor type is used to identify the uci::type::SMTI_SettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType sMTI_SettingsCommandQFT = 2640;

         //  This accessor type is used to identify the uci::type::SMTI_SettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType sMTI_SettingsCommandStatusMT = 2641;

         //  This accessor type is used to identify the uci::type::SMTI_SettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType sMTI_SettingsCommandStatusQFT = 2642;

         //  This accessor type is used to identify the uci::type::SMTI_SpecificDataType Accessor
         static const uci::base::accessorType::AccessorType sMTI_SpecificDataType = 2643;

         //  This accessor type is used to identify the uci::type::SMTI_TaskBaseType Accessor
         static const uci::base::accessorType::AccessorType sMTI_TaskBaseType = 2644;

         //  This accessor type is used to identify the uci::type::SMTI_TaskType Accessor
         static const uci::base::accessorType::AccessorType sMTI_TaskType = 2645;

         //  This accessor type is used to identify the uci::type::SOB_C2_RecordID_Type Accessor
         static const uci::base::accessorType::AccessorType sOB_C2_RecordID_Type = 2646;

         //  This accessor type is used to identify the uci::type::SOB_C2_RecordType Accessor
         static const uci::base::accessorType::AccessorType sOB_C2_RecordType = 2647;

         //  This accessor type is used to identify the uci::type::SOB_SatelliteRecordID_Type Accessor
         static const uci::base::accessorType::AccessorType sOB_SatelliteRecordID_Type = 2648;

         //  This accessor type is used to identify the uci::type::SOB_SatelliteRecordType Accessor
         static const uci::base::accessorType::AccessorType sOB_SatelliteRecordType = 2649;

         //  This accessor type is used to identify the uci::type::SourceCommandType Accessor
         static const uci::base::accessorType::AccessorType sourceCommandType = 2650;

         //  This accessor type is used to identify the uci::type::SourceFiltersType Accessor
         static const uci::base::accessorType::AccessorType sourceFiltersType = 2651;

         //  This accessor type is used to identify the uci::type::SourceFilterType Accessor
         static const uci::base::accessorType::AccessorType sourceFilterType = 2652;

         //  This accessor type is used to identify the uci::type::SourceLocationFilterType Accessor
         static const uci::base::accessorType::AccessorType sourceLocationFilterType = 2653;

         //  This accessor type is used to identify the uci::type::SpecialCodeDataLinkIdentifierType Accessor
         static const uci::base::accessorType::AccessorType specialCodeDataLinkIdentifierType = 2654;

         //  This accessor type is used to identify the uci::type::SpecificBlueSeparationType Accessor
         static const uci::base::accessorType::AccessorType specificBlueSeparationType = 2655;

         //  This accessor type is used to identify the uci::type::SpecificEmitterIdentityConfidenceType Accessor
         static const uci::base::accessorType::AccessorType specificEmitterIdentityConfidenceType = 2656;

         //  This accessor type is used to identify the uci::type::SpecificEmitterIdentityType Accessor
         static const uci::base::accessorType::AccessorType specificEmitterIdentityType = 2657;

         //  This accessor type is used to identify the uci::type::SpecificEmitterMultipleType Accessor
         static const uci::base::accessorType::AccessorType specificEmitterMultipleType = 2658;

         //  This accessor type is used to identify the uci::type::SpecificFacilityIdentityConfidenceType Accessor
         static const uci::base::accessorType::AccessorType specificFacilityIdentityConfidenceType = 2659;

         //  This accessor type is used to identify the uci::type::SpecificIdentityConfidenceType Accessor
         static const uci::base::accessorType::AccessorType specificIdentityConfidenceType = 2660;

         //  This accessor type is used to identify the uci::type::SpecificIdentityType Accessor
         static const uci::base::accessorType::AccessorType specificIdentityType = 2661;

         //  This accessor type is used to identify the uci::type::SpecificRedSeparationType Accessor
         static const uci::base::accessorType::AccessorType specificRedSeparationType = 2662;

         //  This accessor type is used to identify the uci::type::SpecificVehicleIdentityConfidenceType Accessor
         static const uci::base::accessorType::AccessorType specificVehicleIdentityConfidenceType = 2663;

         //  This accessor type is used to identify the uci::type::SpectralBandType Accessor
         static const uci::base::accessorType::AccessorType spectralBandType = 2664;

         //  This accessor type is used to identify the uci::type::SpectralDensityReportMDT Accessor
         static const uci::base::accessorType::AccessorType spectralDensityReportMDT = 2665;

         //  This accessor type is used to identify the uci::type::SpectralDensityReportMT Accessor
         static const uci::base::accessorType::AccessorType spectralDensityReportMT = 2666;

         //  This accessor type is used to identify the uci::type::SpectralDensityReportQFT Accessor
         static const uci::base::accessorType::AccessorType spectralDensityReportQFT = 2667;

         //  This accessor type is used to identify the uci::type::SpeedRangeType Accessor
         static const uci::base::accessorType::AccessorType speedRangeType = 2668;

         //  This accessor type is used to identify the uci::type::StalenessDomainMobilityType Accessor
         static const uci::base::accessorType::AccessorType stalenessDomainMobilityType = 2669;

         //  This accessor type is used to identify the uci::type::STANAG_4607_MissionType Accessor
         static const uci::base::accessorType::AccessorType sTANAG_4607_MissionType = 2670;

         //  This accessor type is used to identify the uci::type::STANAG_4607_PackingPlanType Accessor
         static const uci::base::accessorType::AccessorType sTANAG_4607_PackingPlanType = 2671;

      } // Namespace: accessorType
      //  
      namespace sTANAG_4607_PackingPlanType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::STANAG_4607_PackingPlanType::ClassificationSystem Accessor
            static const uci::base::accessorType::AccessorType classificationSystem = 2672;

            //  This accessor type is used to identify the uci::type::STANAG_4607_PackingPlanType::OriginatorIdentifier Accessor
            static const uci::base::accessorType::AccessorType originatorIdentifier = 2673;

            //  This accessor type is used to identify the uci::type::STANAG_4607_PackingPlanType::MissionPlan Accessor
            static const uci::base::accessorType::AccessorType missionPlan = 2674;

            //  This accessor type is used to identify the uci::type::STANAG_4607_PackingPlanType::Recipient Accessor
            static const uci::base::accessorType::AccessorType recipient = 2675;

            //  This accessor type is used to identify the uci::type::STANAG_4607_PackingPlanType::FreeText Accessor
            static const uci::base::accessorType::AccessorType freeText = 2676;

         } // Namespace: accessorType
      } // Namespace: sTANAG_4607_PackingPlanType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::StandardIdentityConfidenceType Accessor
         static const uci::base::accessorType::AccessorType standardIdentityConfidenceType = 2677;

         //  This accessor type is used to identify the uci::type::StandardIdentityType Accessor
         static const uci::base::accessorType::AccessorType standardIdentityType = 2678;

         //  This accessor type is used to identify the uci::type::StorageDetailType Accessor
         static const uci::base::accessorType::AccessorType storageDetailType = 2679;

         //  This accessor type is used to identify the uci::type::StoreCarriageCapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType storeCarriageCapabilityStatusMDT = 2680;

         //  This accessor type is used to identify the uci::type::StoreCarriageCapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType storeCarriageCapabilityStatusMT = 2681;

         //  This accessor type is used to identify the uci::type::StoreCarriageCapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType storeCarriageCapabilityStatusQFT = 2682;

         //  This accessor type is used to identify the uci::type::StoreCarriageCapabilityStatusType Accessor
         static const uci::base::accessorType::AccessorType storeCarriageCapabilityStatusType = 2683;

         //  This accessor type is used to identify the uci::type::StoreCarriageCapabilityType Accessor
         static const uci::base::accessorType::AccessorType storeCarriageCapabilityType = 2684;

         //  This accessor type is used to identify the uci::type::StoreCarriageStatusType Accessor
         static const uci::base::accessorType::AccessorType storeCarriageStatusType = 2685;

         //  This accessor type is used to identify the uci::type::StoreConfidenceType Accessor
         static const uci::base::accessorType::AccessorType storeConfidenceType = 2686;

         //  This accessor type is used to identify the uci::type::StoreInformationBaseType Accessor
         static const uci::base::accessorType::AccessorType storeInformationBaseType = 2687;

         //  This accessor type is used to identify the uci::type::StoreInformationType Accessor
         static const uci::base::accessorType::AccessorType storeInformationType = 2688;

         //  This accessor type is used to identify the uci::type::StoreManagementCommandMDT Accessor
         static const uci::base::accessorType::AccessorType storeManagementCommandMDT = 2689;

         //  This accessor type is used to identify the uci::type::StoreManagementCommandMT Accessor
         static const uci::base::accessorType::AccessorType storeManagementCommandMT = 2690;

         //  This accessor type is used to identify the uci::type::StoreManagementCommandQFT Accessor
         static const uci::base::accessorType::AccessorType storeManagementCommandQFT = 2691;

         //  This accessor type is used to identify the uci::type::StoreManagementCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType storeManagementCommandStatusMT = 2692;

         //  This accessor type is used to identify the uci::type::StoreManagementCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType storeManagementCommandStatusQFT = 2693;

         //  This accessor type is used to identify the uci::type::StoreManagementCommandType Accessor
         static const uci::base::accessorType::AccessorType storeManagementCommandType = 2694;

         //  This accessor type is used to identify the uci::type::StoreManagementReportMDT Accessor
         static const uci::base::accessorType::AccessorType storeManagementReportMDT = 2695;

         //  This accessor type is used to identify the uci::type::StoreManagementReportMT Accessor
         static const uci::base::accessorType::AccessorType storeManagementReportMT = 2696;

         //  This accessor type is used to identify the uci::type::StoreManagementReportQFT Accessor
         static const uci::base::accessorType::AccessorType storeManagementReportQFT = 2697;

         //  This accessor type is used to identify the uci::type::StoreManagementStatusMDT Accessor
         static const uci::base::accessorType::AccessorType storeManagementStatusMDT = 2698;

         //  This accessor type is used to identify the uci::type::StoreManagementStatusMT Accessor
         static const uci::base::accessorType::AccessorType storeManagementStatusMT = 2699;

         //  This accessor type is used to identify the uci::type::StoreManagementStatusQFT Accessor
         static const uci::base::accessorType::AccessorType storeManagementStatusQFT = 2700;

         //  This accessor type is used to identify the uci::type::StoreStationType Accessor
         static const uci::base::accessorType::AccessorType storeStationType = 2701;

         //  This accessor type is used to identify the uci::type::StoreStatusBaseType Accessor
         static const uci::base::accessorType::AccessorType storeStatusBaseType = 2702;

         //  This accessor type is used to identify the uci::type::StoreStatusDetailsType Accessor
         static const uci::base::accessorType::AccessorType storeStatusDetailsType = 2703;

         //  This accessor type is used to identify the uci::type::StoreStatusType Accessor
         static const uci::base::accessorType::AccessorType storeStatusType = 2704;

         //  This accessor type is used to identify the uci::type::StoreType Accessor
         static const uci::base::accessorType::AccessorType storeType = 2705;

         //  This accessor type is used to identify the uci::type::StoreVerificationStatusType Accessor
         static const uci::base::accessorType::AccessorType storeVerificationStatusType = 2706;

         //  This accessor type is used to identify the uci::type::StrengthRangeType Accessor
         static const uci::base::accessorType::AccessorType strengthRangeType = 2707;

         //  This accessor type is used to identify the uci::type::StrengthType Accessor
         static const uci::base::accessorType::AccessorType strengthType = 2708;

         //  This accessor type is used to identify the uci::type::StrikeActivityCommandType Accessor
         static const uci::base::accessorType::AccessorType strikeActivityCommandType = 2709;

         //  This accessor type is used to identify the uci::type::StrikeActivityMDT Accessor
         static const uci::base::accessorType::AccessorType strikeActivityMDT = 2710;

         //  This accessor type is used to identify the uci::type::StrikeActivityMT Accessor
         static const uci::base::accessorType::AccessorType strikeActivityMT = 2711;

         //  This accessor type is used to identify the uci::type::StrikeActivityQFT Accessor
         static const uci::base::accessorType::AccessorType strikeActivityQFT = 2712;

         //  This accessor type is used to identify the uci::type::StrikeActivityType Accessor
         static const uci::base::accessorType::AccessorType strikeActivityType = 2713;

         //  This accessor type is used to identify the uci::type::StrikeCapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType strikeCapabilityCommandType = 2714;

         //  This accessor type is used to identify the uci::type::StrikeCapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType strikeCapabilityMDT = 2715;

         //  This accessor type is used to identify the uci::type::StrikeCapabilityMT Accessor
         static const uci::base::accessorType::AccessorType strikeCapabilityMT = 2716;

         //  This accessor type is used to identify the uci::type::StrikeCapabilityOptionsType Accessor
         static const uci::base::accessorType::AccessorType strikeCapabilityOptionsType = 2717;

         //  This accessor type is used to identify the uci::type::StrikeCapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType strikeCapabilityQFT = 2718;

         //  This accessor type is used to identify the uci::type::StrikeCapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType strikeCapabilityStatusMDT = 2719;

         //  This accessor type is used to identify the uci::type::StrikeCapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType strikeCapabilityStatusMT = 2720;

         //  This accessor type is used to identify the uci::type::StrikeCapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType strikeCapabilityStatusQFT = 2721;

         //  This accessor type is used to identify the uci::type::StrikeCapabilityStatusType Accessor
         static const uci::base::accessorType::AccessorType strikeCapabilityStatusType = 2722;

         //  This accessor type is used to identify the uci::type::StrikeCapabilityType Accessor
         static const uci::base::accessorType::AccessorType strikeCapabilityType = 2723;

         //  This accessor type is used to identify the uci::type::StrikeCommandMDT Accessor
         static const uci::base::accessorType::AccessorType strikeCommandMDT = 2724;

         //  This accessor type is used to identify the uci::type::StrikeCommandMT Accessor
         static const uci::base::accessorType::AccessorType strikeCommandMT = 2725;

         //  This accessor type is used to identify the uci::type::StrikeCommandQFT Accessor
         static const uci::base::accessorType::AccessorType strikeCommandQFT = 2726;

         //  This accessor type is used to identify the uci::type::StrikeCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType strikeCommandStatusMT = 2727;

         //  This accessor type is used to identify the uci::type::StrikeCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType strikeCommandStatusQFT = 2728;

         //  This accessor type is used to identify the uci::type::StrikeCommandType Accessor
         static const uci::base::accessorType::AccessorType strikeCommandType = 2729;

         //  This accessor type is used to identify the uci::type::StrikeConsentRequestMDT Accessor
         static const uci::base::accessorType::AccessorType strikeConsentRequestMDT = 2730;

         //  This accessor type is used to identify the uci::type::StrikeConsentRequestMT Accessor
         static const uci::base::accessorType::AccessorType strikeConsentRequestMT = 2731;

         //  This accessor type is used to identify the uci::type::StrikeConsentRequestQFT Accessor
         static const uci::base::accessorType::AccessorType strikeConsentRequestQFT = 2732;

         //  This accessor type is used to identify the uci::type::StrikeConsentRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType strikeConsentRequestStatusMDT = 2733;

         //  This accessor type is used to identify the uci::type::StrikeConsentRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType strikeConsentRequestStatusMT = 2734;

         //  This accessor type is used to identify the uci::type::StrikeConsentRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType strikeConsentRequestStatusQFT = 2735;

         //  This accessor type is used to identify the uci::type::StrikeConsentRequestType Accessor
         static const uci::base::accessorType::AccessorType strikeConsentRequestType = 2736;

         //  This accessor type is used to identify the uci::type::StrikeSettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType strikeSettingsCommandMT = 2737;

         //  This accessor type is used to identify the uci::type::StrikeSettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType strikeSettingsCommandQFT = 2738;

         //  This accessor type is used to identify the uci::type::StrikeSettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType strikeSettingsCommandStatusMT = 2739;

         //  This accessor type is used to identify the uci::type::StrikeSettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType strikeSettingsCommandStatusQFT = 2740;

         //  This accessor type is used to identify the uci::type::StrikeSpecificDataType Accessor
         static const uci::base::accessorType::AccessorType strikeSpecificDataType = 2741;

         //  This accessor type is used to identify the uci::type::StrikeTaskMetricsTargetingType Accessor
         static const uci::base::accessorType::AccessorType strikeTaskMetricsTargetingType = 2742;

         //  This accessor type is used to identify the uci::type::StrikeTaskMetricsType Accessor
         static const uci::base::accessorType::AccessorType strikeTaskMetricsType = 2743;

         //  This accessor type is used to identify the uci::type::StrikeTaskReleaseConstraintsType Accessor
         static const uci::base::accessorType::AccessorType strikeTaskReleaseConstraintsType = 2744;

         //  This accessor type is used to identify the uci::type::StrikeTaskType Accessor
         static const uci::base::accessorType::AccessorType strikeTaskType = 2745;

         //  This accessor type is used to identify the uci::type::StrikeTaskWeaponListType Accessor
         static const uci::base::accessorType::AccessorType strikeTaskWeaponListType = 2746;

         //  This accessor type is used to identify the uci::type::StrikeTaskWeaponType Accessor
         static const uci::base::accessorType::AccessorType strikeTaskWeaponType = 2747;

         //  This accessor type is used to identify the uci::type::StrikeWeaponCommandType Accessor
         static const uci::base::accessorType::AccessorType strikeWeaponCommandType = 2748;

         //  This accessor type is used to identify the uci::type::SubCapabilityDetailsType Accessor
         static const uci::base::accessorType::AccessorType subCapabilityDetailsType = 2749;

         //  This accessor type is used to identify the uci::type::SubCapabilityID_Type Accessor
         static const uci::base::accessorType::AccessorType subCapabilityID_Type = 2750;

         //  This accessor type is used to identify the uci::type::SubCapabilitySelectionType Accessor
         static const uci::base::accessorType::AccessorType subCapabilitySelectionType = 2751;

         //  This accessor type is used to identify the uci::type::SubsystemActiveBIT_Type Accessor
         static const uci::base::accessorType::AccessorType subsystemActiveBIT_Type = 2752;

         //  This accessor type is used to identify the uci::type::SubsystemBIT_CommandMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemBIT_CommandMDT = 2753;

         //  This accessor type is used to identify the uci::type::SubsystemBIT_CommandMT Accessor
         static const uci::base::accessorType::AccessorType subsystemBIT_CommandMT = 2754;

         //  This accessor type is used to identify the uci::type::SubsystemBIT_CommandQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemBIT_CommandQFT = 2755;

         //  This accessor type is used to identify the uci::type::SubsystemBIT_CommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType subsystemBIT_CommandStatusMT = 2756;

         //  This accessor type is used to identify the uci::type::SubsystemBIT_CommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemBIT_CommandStatusQFT = 2757;

         //  This accessor type is used to identify the uci::type::SubsystemBIT_CommandType Accessor
         static const uci::base::accessorType::AccessorType subsystemBIT_CommandType = 2758;

         //  This accessor type is used to identify the uci::type::SubsystemBIT_ConfigurationMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemBIT_ConfigurationMDT = 2759;

         //  This accessor type is used to identify the uci::type::SubsystemBIT_ConfigurationMT Accessor
         static const uci::base::accessorType::AccessorType subsystemBIT_ConfigurationMT = 2760;

         //  This accessor type is used to identify the uci::type::SubsystemBIT_ConfigurationQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemBIT_ConfigurationQFT = 2761;

         //  This accessor type is used to identify the uci::type::SubsystemBIT_StatusMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemBIT_StatusMDT = 2762;

         //  This accessor type is used to identify the uci::type::SubsystemBIT_StatusMT Accessor
         static const uci::base::accessorType::AccessorType subsystemBIT_StatusMT = 2763;

         //  This accessor type is used to identify the uci::type::SubsystemBIT_StatusQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemBIT_StatusQFT = 2764;

         //  This accessor type is used to identify the uci::type::SubsystemBIT_Type Accessor
         static const uci::base::accessorType::AccessorType subsystemBIT_Type = 2765;

         //  This accessor type is used to identify the uci::type::SubsystemCalibrationCommandMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemCalibrationCommandMDT = 2766;

         //  This accessor type is used to identify the uci::type::SubsystemCalibrationCommandMT Accessor
         static const uci::base::accessorType::AccessorType subsystemCalibrationCommandMT = 2767;

         //  This accessor type is used to identify the uci::type::SubsystemCalibrationCommandQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemCalibrationCommandQFT = 2768;

         //  This accessor type is used to identify the uci::type::SubsystemCalibrationCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType subsystemCalibrationCommandStatusMT = 2769;

         //  This accessor type is used to identify the uci::type::SubsystemCalibrationCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemCalibrationCommandStatusQFT = 2770;

         //  This accessor type is used to identify the uci::type::SubsystemCalibrationCommandType Accessor
         static const uci::base::accessorType::AccessorType subsystemCalibrationCommandType = 2771;

         //  This accessor type is used to identify the uci::type::SubsystemCalibrationConfigurationMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemCalibrationConfigurationMDT = 2772;

         //  This accessor type is used to identify the uci::type::SubsystemCalibrationConfigurationMT Accessor
         static const uci::base::accessorType::AccessorType subsystemCalibrationConfigurationMT = 2773;

         //  This accessor type is used to identify the uci::type::SubsystemCalibrationConfigurationQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemCalibrationConfigurationQFT = 2774;

         //  This accessor type is used to identify the uci::type::SubsystemCalibrationStatusMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemCalibrationStatusMDT = 2775;

         //  This accessor type is used to identify the uci::type::SubsystemCalibrationStatusMT Accessor
         static const uci::base::accessorType::AccessorType subsystemCalibrationStatusMT = 2776;

         //  This accessor type is used to identify the uci::type::SubsystemCalibrationStatusQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemCalibrationStatusQFT = 2777;

         //  This accessor type is used to identify the uci::type::SubsystemCalibrationType Accessor
         static const uci::base::accessorType::AccessorType subsystemCalibrationType = 2778;

         //  This accessor type is used to identify the uci::type::SubsystemCommandID_Type Accessor
         static const uci::base::accessorType::AccessorType subsystemCommandID_Type = 2779;

         //  This accessor type is used to identify the uci::type::SubsystemCommandStatusMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemCommandStatusMDT = 2780;

         //  This accessor type is used to identify the uci::type::SubsystemCompletedBIT_ItemType Accessor
         static const uci::base::accessorType::AccessorType subsystemCompletedBIT_ItemType = 2781;

         //  This accessor type is used to identify the uci::type::SubsystemCompletedBIT_Type Accessor
         static const uci::base::accessorType::AccessorType subsystemCompletedBIT_Type = 2782;

         //  This accessor type is used to identify the uci::type::SubsystemCompletedCalibrationItemType Accessor
         static const uci::base::accessorType::AccessorType subsystemCompletedCalibrationItemType = 2783;

         //  This accessor type is used to identify the uci::type::SubsystemCompletedCalibrationType Accessor
         static const uci::base::accessorType::AccessorType subsystemCompletedCalibrationType = 2784;

         //  This accessor type is used to identify the uci::type::SubsystemComponentType Accessor
         static const uci::base::accessorType::AccessorType subsystemComponentType = 2785;

         //  This accessor type is used to identify the uci::type::SubsystemConfigurationType Accessor
         static const uci::base::accessorType::AccessorType subsystemConfigurationType = 2786;

         //  This accessor type is used to identify the uci::type::SubsystemEnabledSettingType Accessor
         static const uci::base::accessorType::AccessorType subsystemEnabledSettingType = 2787;

         //  This accessor type is used to identify the uci::type::SubsystemEraseCommandMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemEraseCommandMDT = 2788;

         //  This accessor type is used to identify the uci::type::SubsystemEraseCommandMT Accessor
         static const uci::base::accessorType::AccessorType subsystemEraseCommandMT = 2789;

         //  This accessor type is used to identify the uci::type::SubsystemEraseCommandQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemEraseCommandQFT = 2790;

         //  This accessor type is used to identify the uci::type::SubsystemEraseCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType subsystemEraseCommandStatusMT = 2791;

         //  This accessor type is used to identify the uci::type::SubsystemEraseCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemEraseCommandStatusQFT = 2792;

         //  This accessor type is used to identify the uci::type::SubsystemFaultType Accessor
         static const uci::base::accessorType::AccessorType subsystemFaultType = 2793;

         //  This accessor type is used to identify the uci::type::SubsystemID_Type Accessor
         static const uci::base::accessorType::AccessorType subsystemID_Type = 2794;

         //  This accessor type is used to identify the uci::type::SubsystemSettingParameterType Accessor
         static const uci::base::accessorType::AccessorType subsystemSettingParameterType = 2795;

         //  This accessor type is used to identify the uci::type::SubsystemSettingsCommandMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemSettingsCommandMDT = 2796;

         //  This accessor type is used to identify the uci::type::SubsystemSettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType subsystemSettingsCommandMT = 2797;

         //  This accessor type is used to identify the uci::type::SubsystemSettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemSettingsCommandQFT = 2798;

         //  This accessor type is used to identify the uci::type::SubsystemSettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType subsystemSettingsCommandStatusMT = 2799;

         //  This accessor type is used to identify the uci::type::SubsystemSettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemSettingsCommandStatusQFT = 2800;

         //  This accessor type is used to identify the uci::type::SubsystemSpecificationType Accessor
         static const uci::base::accessorType::AccessorType subsystemSpecificationType = 2801;

         //  This accessor type is used to identify the uci::type::SubsystemStateCommandMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemStateCommandMDT = 2802;

         //  This accessor type is used to identify the uci::type::SubsystemStateCommandMT Accessor
         static const uci::base::accessorType::AccessorType subsystemStateCommandMT = 2803;

         //  This accessor type is used to identify the uci::type::SubsystemStateCommandQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemStateCommandQFT = 2804;

         //  This accessor type is used to identify the uci::type::SubsystemStateCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType subsystemStateCommandStatusMT = 2805;

         //  This accessor type is used to identify the uci::type::SubsystemStateCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemStateCommandStatusQFT = 2806;

         //  This accessor type is used to identify the uci::type::SubsystemStatusDetailedMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusDetailedMDT = 2807;

         //  This accessor type is used to identify the uci::type::SubsystemStatusDetailedMT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusDetailedMT = 2808;

         //  This accessor type is used to identify the uci::type::SubsystemStatusDetailedNameValuePairType Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusDetailedNameValuePairType = 2809;

         //  This accessor type is used to identify the uci::type::SubsystemStatusDetailedQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusDetailedQFT = 2810;

         //  This accessor type is used to identify the uci::type::SubsystemStatusDetailedRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusDetailedRequestID_Type = 2811;

         //  This accessor type is used to identify the uci::type::SubsystemStatusDetailedRequestMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusDetailedRequestMDT = 2812;

         //  This accessor type is used to identify the uci::type::SubsystemStatusDetailedRequestMT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusDetailedRequestMT = 2813;

         //  This accessor type is used to identify the uci::type::SubsystemStatusDetailedRequestQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusDetailedRequestQFT = 2814;

         //  This accessor type is used to identify the uci::type::SubsystemStatusDetailedRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusDetailedRequestStatusMDT = 2815;

         //  This accessor type is used to identify the uci::type::SubsystemStatusDetailedRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusDetailedRequestStatusMT = 2816;

         //  This accessor type is used to identify the uci::type::SubsystemStatusDetailedRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusDetailedRequestStatusQFT = 2817;

         //  This accessor type is used to identify the uci::type::SubsystemStatusDetailedValueType Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusDetailedValueType = 2818;

         //  This accessor type is used to identify the uci::type::SubsystemStatusMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusMDT = 2819;

         //  This accessor type is used to identify the uci::type::SubsystemStatusMT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusMT = 2820;

         //  This accessor type is used to identify the uci::type::SubsystemStatusQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusQFT = 2821;

         //  This accessor type is used to identify the uci::type::SubsystemStatusRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusRequestID_Type = 2822;

         //  This accessor type is used to identify the uci::type::SubsystemStatusRequestMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusRequestMDT = 2823;

         //  This accessor type is used to identify the uci::type::SubsystemStatusRequestMT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusRequestMT = 2824;

         //  This accessor type is used to identify the uci::type::SubsystemStatusRequestQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusRequestQFT = 2825;

         //  This accessor type is used to identify the uci::type::SubsystemStatusRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusRequestStatusMDT = 2826;

         //  This accessor type is used to identify the uci::type::SubsystemStatusRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusRequestStatusMT = 2827;

         //  This accessor type is used to identify the uci::type::SubsystemStatusRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemStatusRequestStatusQFT = 2828;

         //  This accessor type is used to identify the uci::type::SubsystemStreamMDT Accessor
         static const uci::base::accessorType::AccessorType subsystemStreamMDT = 2829;

         //  This accessor type is used to identify the uci::type::SubsystemStreamMT Accessor
         static const uci::base::accessorType::AccessorType subsystemStreamMT = 2830;

         //  This accessor type is used to identify the uci::type::SubsystemStreamQFT Accessor
         static const uci::base::accessorType::AccessorType subsystemStreamQFT = 2831;

         //  This accessor type is used to identify the uci::type::SubsystemSupportedSettingType Accessor
         static const uci::base::accessorType::AccessorType subsystemSupportedSettingType = 2832;

         //  This accessor type is used to identify the uci::type::SupportCapabilityCommandBaseType Accessor
         static const uci::base::accessorType::AccessorType supportCapabilityCommandBaseType = 2833;

         //  This accessor type is used to identify the uci::type::SupportCapabilityCommandID_Type Accessor
         static const uci::base::accessorType::AccessorType supportCapabilityCommandID_Type = 2834;

         //  This accessor type is used to identify the uci::type::SupportCapabilityCommandStatusBaseType Accessor
         static const uci::base::accessorType::AccessorType supportCapabilityCommandStatusBaseType = 2835;

         //  This accessor type is used to identify the uci::type::SupportCapabilityID_Type Accessor
         static const uci::base::accessorType::AccessorType supportCapabilityID_Type = 2836;

         //  This accessor type is used to identify the uci::type::SupportCapabilityStatusBaseType Accessor
         static const uci::base::accessorType::AccessorType supportCapabilityStatusBaseType = 2837;

         //  This accessor type is used to identify the uci::type::SupportedAutonomySettingsMissionPlanningType Accessor
         static const uci::base::accessorType::AccessorType supportedAutonomySettingsMissionPlanningType = 2838;

         //  This accessor type is used to identify the uci::type::SupportedAutonomySettingsType Accessor
         static const uci::base::accessorType::AccessorType supportedAutonomySettingsType = 2839;

         //  This accessor type is used to identify the uci::type::SupportedMissionPlanCommandsRoutePlanningFunctionType Accessor
         static const uci::base::accessorType::AccessorType supportedMissionPlanCommandsRoutePlanningFunctionType = 2840;

         //  This accessor type is used to identify the
         //  uci::type::SupportedMissionPlanCommandsRoutePlanningMissionPlanProcessFunctionType Accessor
         static const uci::base::accessorType::AccessorType supportedMissionPlanCommandsRoutePlanningMissionPlanProcessFunctionType = 2841;

         //  This accessor type is used to identify the uci::type::SupportedMissionPlanCommandsRoutePlanningMissionPlanProcessType
         //  Accessor
         static const uci::base::accessorType::AccessorType supportedMissionPlanCommandsRoutePlanningMissionPlanProcessType = 2842;

         //  This accessor type is used to identify the uci::type::SupportedMissionPlanCommandsRoutePlanningType Accessor
         static const uci::base::accessorType::AccessorType supportedMissionPlanCommandsRoutePlanningType = 2843;

         //  This accessor type is used to identify the uci::type::SupportedMissionPlanCommandsTaskAllocationFunctionType Accessor
         static const uci::base::accessorType::AccessorType supportedMissionPlanCommandsTaskAllocationFunctionType = 2844;

         //  This accessor type is used to identify the
         //  uci::type::SupportedMissionPlanCommandsTaskAllocationMissionPlanProcessType Accessor
         static const uci::base::accessorType::AccessorType supportedMissionPlanCommandsTaskAllocationMissionPlanProcessType = 2845;

         //  This accessor type is used to identify the uci::type::SupportedMissionPlanCommandsTaskAllocationType Accessor
         static const uci::base::accessorType::AccessorType supportedMissionPlanCommandsTaskAllocationType = 2846;

         //  This accessor type is used to identify the uci::type::SupportedMissionPlanCommandsType Accessor
         static const uci::base::accessorType::AccessorType supportedMissionPlanCommandsType = 2847;

         //  This accessor type is used to identify the uci::type::SupportedModeType Accessor
         static const uci::base::accessorType::AccessorType supportedModeType = 2848;

         //  This accessor type is used to identify the uci::type::SupportedResolutionAvailabilityType Accessor
         static const uci::base::accessorType::AccessorType supportedResolutionAvailabilityType = 2849;

         //  This accessor type is used to identify the uci::type::SupportedResolutionID_Type Accessor
         static const uci::base::accessorType::AccessorType supportedResolutionID_Type = 2850;

         //  This accessor type is used to identify the uci::type::SupportedResolutionType Accessor
         static const uci::base::accessorType::AccessorType supportedResolutionType = 2851;

         //  This accessor type is used to identify the uci::type::SurfaceRecoveryType Accessor
         static const uci::base::accessorType::AccessorType surfaceRecoveryType = 2852;

         //  This accessor type is used to identify the uci::type::SurvivabilityRiskLevelID_Type Accessor
         static const uci::base::accessorType::AccessorType survivabilityRiskLevelID_Type = 2853;

         //  This accessor type is used to identify the uci::type::SurvivabilityRiskLevelMDT Accessor
         static const uci::base::accessorType::AccessorType survivabilityRiskLevelMDT = 2854;

         //  This accessor type is used to identify the uci::type::SurvivabilityRiskLevelMT Accessor
         static const uci::base::accessorType::AccessorType survivabilityRiskLevelMT = 2855;

         //  This accessor type is used to identify the uci::type::SurvivabilityRiskLevelQFT Accessor
         static const uci::base::accessorType::AccessorType survivabilityRiskLevelQFT = 2856;

         //  This accessor type is used to identify the uci::type::SurvivabilityRiskSettingType Accessor
         static const uci::base::accessorType::AccessorType survivabilityRiskSettingType = 2857;

         //  This accessor type is used to identify the uci::type::SystemApprovalPolicyType Accessor
         static const uci::base::accessorType::AccessorType systemApprovalPolicyType = 2858;

         //  This accessor type is used to identify the uci::type::SystemApproverType Accessor
         static const uci::base::accessorType::AccessorType systemApproverType = 2859;

         //  This accessor type is used to identify the uci::type::SystemCapabilitySpecificationType Accessor
         static const uci::base::accessorType::AccessorType systemCapabilitySpecificationType = 2860;

         //  This accessor type is used to identify the uci::type::SystemCommunicationsType Accessor
         static const uci::base::accessorType::AccessorType systemCommunicationsType = 2861;

         //  This accessor type is used to identify the uci::type::SystemConfigurationType Accessor
         static const uci::base::accessorType::AccessorType systemConfigurationType = 2862;

         //  This accessor type is used to identify the uci::type::SystemDataType Accessor
         static const uci::base::accessorType::AccessorType systemDataType = 2863;

         //  This accessor type is used to identify the uci::type::SystemEstimationRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType systemEstimationRequestID_Type = 2864;

         //  This accessor type is used to identify the uci::type::SystemEstimationRequestMDT Accessor
         static const uci::base::accessorType::AccessorType systemEstimationRequestMDT = 2865;

         //  This accessor type is used to identify the uci::type::SystemEstimationRequestMT Accessor
         static const uci::base::accessorType::AccessorType systemEstimationRequestMT = 2866;

         //  This accessor type is used to identify the uci::type::SystemEstimationRequestQFT Accessor
         static const uci::base::accessorType::AccessorType systemEstimationRequestQFT = 2867;

         //  This accessor type is used to identify the uci::type::SystemEstimationRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType systemEstimationRequestStatusMDT = 2868;

         //  This accessor type is used to identify the uci::type::SystemEstimationRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType systemEstimationRequestStatusMT = 2869;

         //  This accessor type is used to identify the uci::type::SystemEstimationRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType systemEstimationRequestStatusQFT = 2870;

         //  This accessor type is used to identify the uci::type::SystemID_Type Accessor
         static const uci::base::accessorType::AccessorType systemID_Type = 2871;

         //  This accessor type is used to identify the uci::type::SystemIdentityType Accessor
         static const uci::base::accessorType::AccessorType systemIdentityType = 2872;

         //  This accessor type is used to identify the uci::type::SystemManagementRequestID_Type Accessor
         static const uci::base::accessorType::AccessorType systemManagementRequestID_Type = 2873;

         //  This accessor type is used to identify the uci::type::SystemManagementRequestMDT Accessor
         static const uci::base::accessorType::AccessorType systemManagementRequestMDT = 2874;

         //  This accessor type is used to identify the uci::type::SystemManagementRequestMT Accessor
         static const uci::base::accessorType::AccessorType systemManagementRequestMT = 2875;

         //  This accessor type is used to identify the uci::type::SystemManagementRequestQFT Accessor
         static const uci::base::accessorType::AccessorType systemManagementRequestQFT = 2876;

         //  This accessor type is used to identify the uci::type::SystemManagementRequestStatusMDT Accessor
         static const uci::base::accessorType::AccessorType systemManagementRequestStatusMDT = 2877;

         //  This accessor type is used to identify the uci::type::SystemManagementRequestStatusMT Accessor
         static const uci::base::accessorType::AccessorType systemManagementRequestStatusMT = 2878;

         //  This accessor type is used to identify the uci::type::SystemManagementRequestStatusQFT Accessor
         static const uci::base::accessorType::AccessorType systemManagementRequestStatusQFT = 2879;

         //  This accessor type is used to identify the uci::type::SystemManagementRequestType Accessor
         static const uci::base::accessorType::AccessorType systemManagementRequestType = 2880;

         //  This accessor type is used to identify the uci::type::SystemMessageIdentifierType Accessor
         static const uci::base::accessorType::AccessorType systemMessageIdentifierType = 2881;

         //  This accessor type is used to identify the uci::type::SystemMetadataMDT Accessor
         static const uci::base::accessorType::AccessorType systemMetadataMDT = 2882;

         //  This accessor type is used to identify the uci::type::SystemMetadataMT Accessor
         static const uci::base::accessorType::AccessorType systemMetadataMT = 2883;

         //  This accessor type is used to identify the uci::type::SystemMetadataPET Accessor
         static const uci::base::accessorType::AccessorType systemMetadataPET = 2884;

         //  This accessor type is used to identify the uci::type::SystemMetadataQFT Accessor
         static const uci::base::accessorType::AccessorType systemMetadataQFT = 2885;

         //  This accessor type is used to identify the uci::type::SystemMissionPlanSpecificationType Accessor
         static const uci::base::accessorType::AccessorType systemMissionPlanSpecificationType = 2886;

         //  This accessor type is used to identify the uci::type::SystemPositionSpecificationType Accessor
         static const uci::base::accessorType::AccessorType systemPositionSpecificationType = 2887;

         //  This accessor type is used to identify the uci::type::SystemStatusMDT Accessor
         static const uci::base::accessorType::AccessorType systemStatusMDT = 2888;

         //  This accessor type is used to identify the uci::type::SystemStatusMT Accessor
         static const uci::base::accessorType::AccessorType systemStatusMT = 2889;

         //  This accessor type is used to identify the uci::type::SystemStatusQFT Accessor
         static const uci::base::accessorType::AccessorType systemStatusQFT = 2890;

         //  This accessor type is used to identify the uci::type::SystemThreadIdentifierType Accessor
         static const uci::base::accessorType::AccessorType systemThreadIdentifierType = 2891;

         //  This accessor type is used to identify the uci::type::SystemTimeAtReferenceMDT Accessor
         static const uci::base::accessorType::AccessorType systemTimeAtReferenceMDT = 2892;

         //  This accessor type is used to identify the uci::type::SystemTimeAtReferenceMT Accessor
         static const uci::base::accessorType::AccessorType systemTimeAtReferenceMT = 2893;

         //  This accessor type is used to identify the uci::type::SystemTimeAtReferenceQFT Accessor
         static const uci::base::accessorType::AccessorType systemTimeAtReferenceQFT = 2894;

         //  This accessor type is used to identify the uci::type::TargetInformationType Accessor
         static const uci::base::accessorType::AccessorType targetInformationType = 2895;

         //  This accessor type is used to identify the uci::type::TargetMobilityAssessmentDataType Accessor
         static const uci::base::accessorType::AccessorType targetMobilityAssessmentDataType = 2896;

         //  This accessor type is used to identify the uci::type::TargetMobilityAssessmentType Accessor
         static const uci::base::accessorType::AccessorType targetMobilityAssessmentType = 2897;

         //  This accessor type is used to identify the uci::type::TargetMobilityRequestType Accessor
         static const uci::base::accessorType::AccessorType targetMobilityRequestType = 2898;

         //  This accessor type is used to identify the uci::type::TargetType Accessor
         static const uci::base::accessorType::AccessorType targetType = 2899;

         //  This accessor type is used to identify the uci::type::TaskAllocationApprovalPolicyType Accessor
         static const uci::base::accessorType::AccessorType taskAllocationApprovalPolicyType = 2900;

         //  This accessor type is used to identify the uci::type::TaskAllocationCandidateType Accessor
         static const uci::base::accessorType::AccessorType taskAllocationCandidateType = 2901;

         //  This accessor type is used to identify the uci::type::TaskAllocationConstraintType Accessor
         static const uci::base::accessorType::AccessorType taskAllocationConstraintType = 2902;

         //  This accessor type is used to identify the uci::type::TaskAllocationParametersType Accessor
         static const uci::base::accessorType::AccessorType taskAllocationParametersType = 2903;

         //  This accessor type is used to identify the uci::type::TaskAllocationPolicyType Accessor
         static const uci::base::accessorType::AccessorType taskAllocationPolicyType = 2904;

         //  This accessor type is used to identify the uci::type::TaskAllocationResultCaseType Accessor
         static const uci::base::accessorType::AccessorType taskAllocationResultCaseType = 2905;

         //  This accessor type is used to identify the uci::type::TaskAllocationTriggerApprovalCaseType Accessor
         static const uci::base::accessorType::AccessorType taskAllocationTriggerApprovalCaseType = 2906;

         //  This accessor type is used to identify the uci::type::TaskAllocationValidationAssessmentDataType Accessor
         static const uci::base::accessorType::AccessorType taskAllocationValidationAssessmentDataType = 2907;

         //  This accessor type is used to identify the uci::type::TaskAllocationValidationAssessmentType Accessor
         static const uci::base::accessorType::AccessorType taskAllocationValidationAssessmentType = 2908;

         //  This accessor type is used to identify the uci::type::TaskAllocationValidationRequestType Accessor
         static const uci::base::accessorType::AccessorType taskAllocationValidationRequestType = 2909;

         //  This accessor type is used to identify the uci::type::TaskAssociationConstraintType Accessor
         static const uci::base::accessorType::AccessorType taskAssociationConstraintType = 2910;

         //  This accessor type is used to identify the uci::type::TaskCancelID_Type Accessor
         static const uci::base::accessorType::AccessorType taskCancelID_Type = 2911;

         //  This accessor type is used to identify the uci::type::TaskCancelMDT Accessor
         static const uci::base::accessorType::AccessorType taskCancelMDT = 2912;

         //  This accessor type is used to identify the uci::type::TaskCancelMT Accessor
         static const uci::base::accessorType::AccessorType taskCancelMT = 2913;

         //  This accessor type is used to identify the uci::type::TaskCancelQFT Accessor
         static const uci::base::accessorType::AccessorType taskCancelQFT = 2914;

         //  This accessor type is used to identify the uci::type::TaskDataBaseType Accessor
         static const uci::base::accessorType::AccessorType taskDataBaseType = 2915;

         //  This accessor type is used to identify the uci::type::TaskExecutionPolicyByTaskType Accessor
         static const uci::base::accessorType::AccessorType taskExecutionPolicyByTaskType = 2916;

         //  This accessor type is used to identify the uci::type::TaskExecutionPolicyType Accessor
         static const uci::base::accessorType::AccessorType taskExecutionPolicyType = 2917;

         //  This accessor type is used to identify the uci::type::TaskFailedTriggerDataType Accessor
         static const uci::base::accessorType::AccessorType taskFailedTriggerDataType = 2918;

         //  This accessor type is used to identify the uci::type::TaskID_InclusionType Accessor
         static const uci::base::accessorType::AccessorType taskID_InclusionType = 2919;

         //  This accessor type is used to identify the uci::type::TaskID_Type Accessor
         static const uci::base::accessorType::AccessorType taskID_Type = 2920;

         //  This accessor type is used to identify the uci::type::TaskMDT Accessor
         static const uci::base::accessorType::AccessorType taskMDT = 2921;

         //  This accessor type is used to identify the uci::type::TaskMetadataType Accessor
         static const uci::base::accessorType::AccessorType taskMetadataType = 2922;

         //  This accessor type is used to identify the uci::type::TaskMetricsCategoryType Accessor
         static const uci::base::accessorType::AccessorType taskMetricsCategoryType = 2923;

         //  This accessor type is used to identify the uci::type::TaskMetricsType Accessor
         static const uci::base::accessorType::AccessorType taskMetricsType = 2924;

         //  This accessor type is used to identify the uci::type::TaskMT Accessor
         static const uci::base::accessorType::AccessorType taskMT = 2925;

         //  This accessor type is used to identify the uci::type::TaskQFT Accessor
         static const uci::base::accessorType::AccessorType taskQFT = 2926;

         //  This accessor type is used to identify the uci::type::TaskResponseType Accessor
         static const uci::base::accessorType::AccessorType taskResponseType = 2927;

         //  This accessor type is used to identify the uci::type::TaskRiskAdjustmentType Accessor
         static const uci::base::accessorType::AccessorType taskRiskAdjustmentType = 2928;

         //  This accessor type is used to identify the uci::type::TaskSetID_Type Accessor
         static const uci::base::accessorType::AccessorType taskSetID_Type = 2929;

         //  This accessor type is used to identify the uci::type::TaskSetMDT Accessor
         static const uci::base::accessorType::AccessorType taskSetMDT = 2930;

         //  This accessor type is used to identify the uci::type::TaskSetMT Accessor
         static const uci::base::accessorType::AccessorType taskSetMT = 2931;

         //  This accessor type is used to identify the uci::type::TaskSetQFT Accessor
         static const uci::base::accessorType::AccessorType taskSetQFT = 2932;

         //  This accessor type is used to identify the uci::type::TaskStatusDataBaseType Accessor
         static const uci::base::accessorType::AccessorType taskStatusDataBaseType = 2933;

         //  This accessor type is used to identify the uci::type::TaskStatusMDT Accessor
         static const uci::base::accessorType::AccessorType taskStatusMDT = 2934;

         //  This accessor type is used to identify the uci::type::TaskStatusMT Accessor
         static const uci::base::accessorType::AccessorType taskStatusMT = 2935;

         //  This accessor type is used to identify the uci::type::TaskStatusQFT Accessor
         static const uci::base::accessorType::AccessorType taskStatusQFT = 2936;

         //  This accessor type is used to identify the uci::type::TaskTransitionActionType Accessor
         static const uci::base::accessorType::AccessorType taskTransitionActionType = 2937;

         //  This accessor type is used to identify the uci::type::TaskTriggerDataType Accessor
         static const uci::base::accessorType::AccessorType taskTriggerDataType = 2938;

         //  This accessor type is used to identify the uci::type::TaskTriggerPrecedenceType Accessor
         static const uci::base::accessorType::AccessorType taskTriggerPrecedenceType = 2939;

         //  This accessor type is used to identify the uci::type::TaskTriggerTaskType Accessor
         static const uci::base::accessorType::AccessorType taskTriggerTaskType = 2940;

         //  This accessor type is used to identify the uci::type::TaskType Accessor
         static const uci::base::accessorType::AccessorType taskType = 2941;

         //  This accessor type is used to identify the uci::type::TaskTypeRiskAdjustmentType Accessor
         static const uci::base::accessorType::AccessorType taskTypeRiskAdjustmentType = 2942;

         //  This accessor type is used to identify the uci::type::TaskUnallocatedTriggerType Accessor
         static const uci::base::accessorType::AccessorType taskUnallocatedTriggerType = 2943;

         //  This accessor type is used to identify the uci::type::TemperatureStatusType Accessor
         static const uci::base::accessorType::AccessorType temperatureStatusType = 2944;

         //  This accessor type is used to identify the uci::type::ThreadConfigurationType Accessor
         static const uci::base::accessorType::AccessorType threadConfigurationType = 2945;

         //  This accessor type is used to identify the uci::type::ThreatAssessmentMetricsType Accessor
         static const uci::base::accessorType::AccessorType threatAssessmentMetricsType = 2946;

         //  This accessor type is used to identify the uci::type::ThreatAssessmentProbabilityType Accessor
         static const uci::base::accessorType::AccessorType threatAssessmentProbabilityType = 2947;

         //  This accessor type is used to identify the uci::type::ThreatAssessmentRequestType Accessor
         static const uci::base::accessorType::AccessorType threatAssessmentRequestType = 2948;

         //  This accessor type is used to identify the uci::type::ThreatAssessmentSystemType Accessor
         static const uci::base::accessorType::AccessorType threatAssessmentSystemType = 2949;

         //  This accessor type is used to identify the uci::type::ThreatExposureProbabilityType Accessor
         static const uci::base::accessorType::AccessorType threatExposureProbabilityType = 2950;

         //  This accessor type is used to identify the uci::type::ThreatExposureType Accessor
         static const uci::base::accessorType::AccessorType threatExposureType = 2951;

         //  This accessor type is used to identify the uci::type::ThreatNominationAssessmentDataType Accessor
         static const uci::base::accessorType::AccessorType threatNominationAssessmentDataType = 2952;

         //  This accessor type is used to identify the uci::type::ThreatNominationAssessmentRequestType Accessor
         static const uci::base::accessorType::AccessorType threatNominationAssessmentRequestType = 2953;

         //  This accessor type is used to identify the uci::type::ThreatNominationAssessmentType Accessor
         static const uci::base::accessorType::AccessorType threatNominationAssessmentType = 2954;

         //  This accessor type is used to identify the uci::type::ThreatNominationResultType Accessor
         static const uci::base::accessorType::AccessorType threatNominationResultType = 2955;

         //  This accessor type is used to identify the uci::type::ThreatTargetPrioritizationItemType Accessor
         static const uci::base::accessorType::AccessorType threatTargetPrioritizationItemType = 2956;

         //  This accessor type is used to identify the uci::type::ThreatTargetPrioritizationListID_Type Accessor
         static const uci::base::accessorType::AccessorType threatTargetPrioritizationListID_Type = 2957;

         //  This accessor type is used to identify the uci::type::ThreatTargetPrioritizationListMDT Accessor
         static const uci::base::accessorType::AccessorType threatTargetPrioritizationListMDT = 2958;

         //  This accessor type is used to identify the uci::type::ThreatTargetPrioritizationListMT Accessor
         static const uci::base::accessorType::AccessorType threatTargetPrioritizationListMT = 2959;

         //  This accessor type is used to identify the uci::type::ThreatTargetPrioritizationListQFT Accessor
         static const uci::base::accessorType::AccessorType threatTargetPrioritizationListQFT = 2960;

         //  This accessor type is used to identify the uci::type::ThreatTargetPrioritizationPriorityType Accessor
         static const uci::base::accessorType::AccessorType threatTargetPrioritizationPriorityType = 2961;

         //  This accessor type is used to identify the uci::type::ThreatTargetPrioritizationType Accessor
         static const uci::base::accessorType::AccessorType threatTargetPrioritizationType = 2962;

         //  This accessor type is used to identify the uci::type::ThresholdOffRouteTriggerDataType Accessor
         static const uci::base::accessorType::AccessorType thresholdOffRouteTriggerDataType = 2963;

         //  This accessor type is used to identify the uci::type::ThresholdVulnerabilityType Accessor
         static const uci::base::accessorType::AccessorType thresholdVulnerabilityType = 2964;

         //  This accessor type is used to identify the uci::type::ThumbnailType Accessor
         static const uci::base::accessorType::AccessorType thumbnailType = 2965;

         //  This accessor type is used to identify the uci::type::TimeCoverageType Accessor
         static const uci::base::accessorType::AccessorType timeCoverageType = 2966;

         //  This accessor type is used to identify the uci::type::TimedZoneType Accessor
         static const uci::base::accessorType::AccessorType timedZoneType = 2967;

         //  This accessor type is used to identify the uci::type::TimeErrorType Accessor
         static const uci::base::accessorType::AccessorType timeErrorType = 2968;

         //  This accessor type is used to identify the uci::type::TimeFunctionType Accessor
         static const uci::base::accessorType::AccessorType timeFunctionType = 2969;

         //  This accessor type is used to identify the uci::type::TimeWindowType Accessor
         static const uci::base::accessorType::AccessorType timeWindowType = 2970;

         //  This accessor type is used to identify the uci::type::TimingConstraintsType Accessor
         static const uci::base::accessorType::AccessorType timingConstraintsType = 2971;

         //  This accessor type is used to identify the uci::type::TLE_BaseType Accessor
         static const uci::base::accessorType::AccessorType tLE_BaseType = 2972;

         //  This accessor type is used to identify the uci::type::TLE_Type Accessor
         static const uci::base::accessorType::AccessorType tLE_Type = 2973;

         //  This accessor type is used to identify the uci::type::TraceabilityType Accessor
         static const uci::base::accessorType::AccessorType traceabilityType = 2974;

         //  This accessor type is used to identify the uci::type::TrackNumberOrEntityType Accessor
         static const uci::base::accessorType::AccessorType trackNumberOrEntityType = 2975;

         //  This accessor type is used to identify the uci::type::TrackOptionsID_Type Accessor
         static const uci::base::accessorType::AccessorType trackOptionsID_Type = 2976;

         //  This accessor type is used to identify the uci::type::TransitionSegmentType Accessor
         static const uci::base::accessorType::AccessorType transitionSegmentType = 2977;

         //  This accessor type is used to identify the uci::type::TurnPointType Accessor
         static const uci::base::accessorType::AccessorType turnPointType = 2978;

         //  This accessor type is used to identify the uci::type::TurretCommandMDT Accessor
         static const uci::base::accessorType::AccessorType turretCommandMDT = 2979;

         //  This accessor type is used to identify the uci::type::TurretCommandMT Accessor
         static const uci::base::accessorType::AccessorType turretCommandMT = 2980;

         //  This accessor type is used to identify the uci::type::TurretCommandPositionType Accessor
         static const uci::base::accessorType::AccessorType turretCommandPositionType = 2981;

         //  This accessor type is used to identify the uci::type::TurretCommandQFT Accessor
         static const uci::base::accessorType::AccessorType turretCommandQFT = 2982;

         //  This accessor type is used to identify the uci::type::TurretCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType turretCommandStatusMT = 2983;

         //  This accessor type is used to identify the uci::type::TurretCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType turretCommandStatusQFT = 2984;

         //  This accessor type is used to identify the uci::type::TurretCommandType Accessor
         static const uci::base::accessorType::AccessorType turretCommandType = 2985;

         //  This accessor type is used to identify the uci::type::TurretReportDynamicPointingType Accessor
         static const uci::base::accessorType::AccessorType turretReportDynamicPointingType = 2986;

         //  This accessor type is used to identify the uci::type::TurretReportMDT Accessor
         static const uci::base::accessorType::AccessorType turretReportMDT = 2987;

         //  This accessor type is used to identify the uci::type::TurretReportMT Accessor
         static const uci::base::accessorType::AccessorType turretReportMT = 2988;

         //  This accessor type is used to identify the uci::type::TurretReportPointingType Accessor
         static const uci::base::accessorType::AccessorType turretReportPointingType = 2989;

         //  This accessor type is used to identify the uci::type::TurretReportQFT Accessor
         static const uci::base::accessorType::AccessorType turretReportQFT = 2990;

         //  This accessor type is used to identify the uci::type::TurretStatusMDT Accessor
         static const uci::base::accessorType::AccessorType turretStatusMDT = 2991;

         //  This accessor type is used to identify the uci::type::TurretStatusMT Accessor
         static const uci::base::accessorType::AccessorType turretStatusMT = 2992;

         //  This accessor type is used to identify the uci::type::TurretStatusQFT Accessor
         static const uci::base::accessorType::AccessorType turretStatusQFT = 2993;

         //  This accessor type is used to identify the uci::type::TxBeamType Accessor
         static const uci::base::accessorType::AccessorType txBeamType = 2994;

         //  This accessor type is used to identify the uci::type::UMID_AudioID_Type Accessor
         static const uci::base::accessorType::AccessorType uMID_AudioID_Type = 2995;

         //  This accessor type is used to identify the uci::type::UMID_DataID_Type Accessor
         static const uci::base::accessorType::AccessorType uMID_DataID_Type = 2996;

         //  This accessor type is used to identify the uci::type::UMID_SystemID_Type Accessor
         static const uci::base::accessorType::AccessorType uMID_SystemID_Type = 2997;

         //  This accessor type is used to identify the uci::type::UMID_VideoID_Type Accessor
         static const uci::base::accessorType::AccessorType uMID_VideoID_Type = 2998;

         //  This accessor type is used to identify the uci::type::UnallocatedReasonType Accessor
         static const uci::base::accessorType::AccessorType unallocatedReasonType = 2999;

         //  This accessor type is used to identify the uci::type::UncertaintyType Accessor
         static const uci::base::accessorType::AccessorType uncertaintyType = 3000;

         //  This accessor type is used to identify the uci::type::UnitID_Type Accessor
         static const uci::base::accessorType::AccessorType unitID_Type = 3001;

         //  This accessor type is used to identify the uci::type::UnitIdentityType Accessor
         static const uci::base::accessorType::AccessorType unitIdentityType = 3002;

         //  This accessor type is used to identify the uci::type::UnitType Accessor
         static const uci::base::accessorType::AccessorType unitType = 3003;

      } // Namespace: accessorType
      //  
      namespace unitType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::UnitType::Evaluation Accessor
            static const uci::base::accessorType::AccessorType evaluation = 3004;

            //  This accessor type is used to identify the uci::type::UnitType::EchelonTier Accessor
            static const uci::base::accessorType::AccessorType echelonTier = 3005;

         } // Namespace: accessorType
      } // Namespace: unitType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::UnsignedIntegerMinMaxType Accessor
         static const uci::base::accessorType::AccessorType unsignedIntegerMinMaxType = 3006;

         //  This accessor type is used to identify the uci::type::UserLinkStatusType Accessor
         static const uci::base::accessorType::AccessorType userLinkStatusType = 3007;

         //  This accessor type is used to identify the uci::type::VehicleCommandDataType Accessor
         static const uci::base::accessorType::AccessorType vehicleCommandDataType = 3008;

         //  This accessor type is used to identify the uci::type::VehicleConfigurationType Accessor
         static const uci::base::accessorType::AccessorType vehicleConfigurationType = 3009;

         //  This accessor type is used to identify the uci::type::VehicleIdentificationType Accessor
         static const uci::base::accessorType::AccessorType vehicleIdentificationType = 3010;

         //  This accessor type is used to identify the uci::type::VehicleStatusType Accessor
         static const uci::base::accessorType::AccessorType vehicleStatusType = 3011;

         //  This accessor type is used to identify the uci::type::VehicleThreatAssessmentDataType Accessor
         static const uci::base::accessorType::AccessorType vehicleThreatAssessmentDataType = 3012;

         //  This accessor type is used to identify the uci::type::VehicleThreatAssessmentRequestType Accessor
         static const uci::base::accessorType::AccessorType vehicleThreatAssessmentRequestType = 3013;

         //  This accessor type is used to identify the uci::type::VehicleThreatAssessmentType Accessor
         static const uci::base::accessorType::AccessorType vehicleThreatAssessmentType = 3014;

         //  This accessor type is used to identify the uci::type::Velocity2D_Type Accessor
         static const uci::base::accessorType::AccessorType velocity2D_Type = 3015;

         //  This accessor type is used to identify the uci::type::Velocity2D_UncertaintyType Accessor
         static const uci::base::accessorType::AccessorType velocity2D_UncertaintyType = 3016;

         //  This accessor type is used to identify the uci::type::Velocity3D_Type Accessor
         static const uci::base::accessorType::AccessorType velocity3D_Type = 3017;

         //  This accessor type is used to identify the uci::type::VelocityOrientationCovarianceType Accessor
         static const uci::base::accessorType::AccessorType velocityOrientationCovarianceType = 3018;

         //  This accessor type is used to identify the uci::type::VelocityVelocityCovarianceCartesianType Accessor
         static const uci::base::accessorType::AccessorType velocityVelocityCovarianceCartesianType = 3019;

         //  This accessor type is used to identify the uci::type::VelocityVelocityCovarianceType Accessor
         static const uci::base::accessorType::AccessorType velocityVelocityCovarianceType = 3020;

         //  This accessor type is used to identify the uci::type::VerboseStatusType Accessor
         static const uci::base::accessorType::AccessorType verboseStatusType = 3021;

         //  This accessor type is used to identify the uci::type::VideoAugmentationParametersType Accessor
         static const uci::base::accessorType::AccessorType videoAugmentationParametersType = 3022;

         //  This accessor type is used to identify the uci::type::VoiceControlType Accessor
         static const uci::base::accessorType::AccessorType voiceControlType = 3023;

      } // Namespace: accessorType
      //  
      namespace voiceControlType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::VoiceControlType::VoiceChannelA Accessor
            static const uci::base::accessorType::AccessorType voiceChannelA = 3024;

            //  This accessor type is used to identify the uci::type::VoiceControlType::VoiceChannelB Accessor
            static const uci::base::accessorType::AccessorType voiceChannelB = 3025;

         } // Namespace: accessorType
      } // Namespace: voiceControlType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::VulnerabilityLevelsCombinedType Accessor
         static const uci::base::accessorType::AccessorType vulnerabilityLevelsCombinedType = 3026;

         //  This accessor type is used to identify the uci::type::VulnerabilityLevelsType Accessor
         static const uci::base::accessorType::AccessorType vulnerabilityLevelsType = 3027;

         //  This accessor type is used to identify the uci::type::VulnerabilityMetricsType Accessor
         static const uci::base::accessorType::AccessorType vulnerabilityMetricsType = 3028;

         //  This accessor type is used to identify the uci::type::WayPointType Accessor
         static const uci::base::accessorType::AccessorType wayPointType = 3029;

         //  This accessor type is used to identify the uci::type::WeaponeeringLocationType Accessor
         static const uci::base::accessorType::AccessorType weaponeeringLocationType = 3030;

         //  This accessor type is used to identify the uci::type::WeaponeeringType Accessor
         static const uci::base::accessorType::AccessorType weaponeeringType = 3031;

         //  This accessor type is used to identify the uci::type::WeaponRestrictionType Accessor
         static const uci::base::accessorType::AccessorType weaponRestrictionType = 3032;

         //  This accessor type is used to identify the uci::type::WeaponStoreType Accessor
         static const uci::base::accessorType::AccessorType weaponStoreType = 3033;

         //  This accessor type is used to identify the uci::type::WeaponTargetPairingChoiceType Accessor
         static const uci::base::accessorType::AccessorType weaponTargetPairingChoiceType = 3034;

         //  This accessor type is used to identify the uci::type::WeatherAreaDataType Accessor
         static const uci::base::accessorType::AccessorType weatherAreaDataType = 3035;

         //  This accessor type is used to identify the uci::type::WeatherAugmentationParametersType Accessor
         static const uci::base::accessorType::AccessorType weatherAugmentationParametersType = 3036;

         //  This accessor type is used to identify the uci::type::WeatherDatasetID_Type Accessor
         static const uci::base::accessorType::AccessorType weatherDatasetID_Type = 3037;

         //  This accessor type is used to identify the uci::type::WeatherDatasetMDT Accessor
         static const uci::base::accessorType::AccessorType weatherDatasetMDT = 3038;

         //  This accessor type is used to identify the uci::type::WeatherDatasetMT Accessor
         static const uci::base::accessorType::AccessorType weatherDatasetMT = 3039;

         //  This accessor type is used to identify the uci::type::WeatherDatasetQFT Accessor
         static const uci::base::accessorType::AccessorType weatherDatasetQFT = 3040;

         //  This accessor type is used to identify the uci::type::WeatherDataType Accessor
         static const uci::base::accessorType::AccessorType weatherDataType = 3041;

         //  This accessor type is used to identify the uci::type::WeatherEffectsType Accessor
         static const uci::base::accessorType::AccessorType weatherEffectsType = 3042;

         //  This accessor type is used to identify the uci::type::WeatherFiltersType Accessor
         static const uci::base::accessorType::AccessorType weatherFiltersType = 3043;

         //  This accessor type is used to identify the uci::type::WeatherMDT Accessor
         static const uci::base::accessorType::AccessorType weatherMDT = 3044;

         //  This accessor type is used to identify the uci::type::WeatherMT Accessor
         static const uci::base::accessorType::AccessorType weatherMT = 3045;

         //  This accessor type is used to identify the uci::type::WeatherObservationMDT Accessor
         static const uci::base::accessorType::AccessorType weatherObservationMDT = 3046;

         //  This accessor type is used to identify the uci::type::WeatherObservationMT Accessor
         static const uci::base::accessorType::AccessorType weatherObservationMT = 3047;

         //  This accessor type is used to identify the uci::type::WeatherObservationQFT Accessor
         static const uci::base::accessorType::AccessorType weatherObservationQFT = 3048;

         //  This accessor type is used to identify the uci::type::WeatherQFT Accessor
         static const uci::base::accessorType::AccessorType weatherQFT = 3049;

         //  This accessor type is used to identify the uci::type::WeatherRadarActivityMDT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarActivityMDT = 3050;

         //  This accessor type is used to identify the uci::type::WeatherRadarActivityMT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarActivityMT = 3051;

         //  This accessor type is used to identify the uci::type::WeatherRadarActivityQFT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarActivityQFT = 3052;

         //  This accessor type is used to identify the uci::type::WeatherRadarActivityStatusDetailType Accessor
         static const uci::base::accessorType::AccessorType weatherRadarActivityStatusDetailType = 3053;

         //  This accessor type is used to identify the uci::type::WeatherRadarActivityType Accessor
         static const uci::base::accessorType::AccessorType weatherRadarActivityType = 3054;

         //  This accessor type is used to identify the uci::type::WeatherRadarCapabilityCommandType Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCapabilityCommandType = 3055;

         //  This accessor type is used to identify the uci::type::WeatherRadarCapabilityMDT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCapabilityMDT = 3056;

         //  This accessor type is used to identify the uci::type::WeatherRadarCapabilityMT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCapabilityMT = 3057;

         //  This accessor type is used to identify the uci::type::WeatherRadarCapabilityQFT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCapabilityQFT = 3058;

         //  This accessor type is used to identify the uci::type::WeatherRadarCapabilityStatusMDT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCapabilityStatusMDT = 3059;

         //  This accessor type is used to identify the uci::type::WeatherRadarCapabilityStatusMT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCapabilityStatusMT = 3060;

         //  This accessor type is used to identify the uci::type::WeatherRadarCapabilityStatusQFT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCapabilityStatusQFT = 3061;

         //  This accessor type is used to identify the uci::type::WeatherRadarCapabilityType Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCapabilityType = 3062;

         //  This accessor type is used to identify the uci::type::WeatherRadarCellLocationType Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCellLocationType = 3063;

         //  This accessor type is used to identify the uci::type::WeatherRadarCellSizeType Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCellSizeType = 3064;

         //  This accessor type is used to identify the uci::type::WeatherRadarCellType Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCellType = 3065;

         //  This accessor type is used to identify the uci::type::WeatherRadarCommandMDT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCommandMDT = 3066;

         //  This accessor type is used to identify the uci::type::WeatherRadarCommandMT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCommandMT = 3067;

         //  This accessor type is used to identify the uci::type::WeatherRadarCommandQFT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCommandQFT = 3068;

         //  This accessor type is used to identify the uci::type::WeatherRadarCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCommandStatusMT = 3069;

         //  This accessor type is used to identify the uci::type::WeatherRadarCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCommandStatusQFT = 3070;

         //  This accessor type is used to identify the uci::type::WeatherRadarCommandType Accessor
         static const uci::base::accessorType::AccessorType weatherRadarCommandType = 3071;

         //  This accessor type is used to identify the uci::type::WeatherRadarObservationMDT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarObservationMDT = 3072;

         //  This accessor type is used to identify the uci::type::WeatherRadarObservationMT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarObservationMT = 3073;

         //  This accessor type is used to identify the uci::type::WeatherRadarObservationQFT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarObservationQFT = 3074;

         //  This accessor type is used to identify the uci::type::WeatherRadarOptionsType Accessor
         static const uci::base::accessorType::AccessorType weatherRadarOptionsType = 3075;

         //  This accessor type is used to identify the uci::type::WeatherRadarSettingsCommandMDT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarSettingsCommandMDT = 3076;

         //  This accessor type is used to identify the uci::type::WeatherRadarSettingsCommandMT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarSettingsCommandMT = 3077;

         //  This accessor type is used to identify the uci::type::WeatherRadarSettingsCommandQFT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarSettingsCommandQFT = 3078;

         //  This accessor type is used to identify the uci::type::WeatherRadarSettingsCommandStatusMT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarSettingsCommandStatusMT = 3079;

         //  This accessor type is used to identify the uci::type::WeatherRadarSettingsCommandStatusQFT Accessor
         static const uci::base::accessorType::AccessorType weatherRadarSettingsCommandStatusQFT = 3080;

         //  This accessor type is used to identify the uci::type::WeatherRadarSpecificDataType Accessor
         static const uci::base::accessorType::AccessorType weatherRadarSpecificDataType = 3081;

         //  This accessor type is used to identify the uci::type::WeatherRadarTaskType Accessor
         static const uci::base::accessorType::AccessorType weatherRadarTaskType = 3082;

         //  This accessor type is used to identify the uci::type::WeatherReportDataType Accessor
         static const uci::base::accessorType::AccessorType weatherReportDataType = 3083;

         //  This accessor type is used to identify the uci::type::WeatherReportGridDataType Accessor
         static const uci::base::accessorType::AccessorType weatherReportGridDataType = 3084;

         //  This accessor type is used to identify the uci::type::WeatherReportType Accessor
         static const uci::base::accessorType::AccessorType weatherReportType = 3085;

         //  This accessor type is used to identify the uci::type::WeatherWarningDataType Accessor
         static const uci::base::accessorType::AccessorType weatherWarningDataType = 3086;

         //  This accessor type is used to identify the uci::type::WeekdayIntervalType Accessor
         static const uci::base::accessorType::AccessorType weekdayIntervalType = 3087;

         //  This accessor type is used to identify the uci::type::WorkingEOB_ID_Type Accessor
         static const uci::base::accessorType::AccessorType workingEOB_ID_Type = 3088;

         //  This accessor type is used to identify the uci::type::WorkingEOB_MDT Accessor
         static const uci::base::accessorType::AccessorType workingEOB_MDT = 3089;

         //  This accessor type is used to identify the uci::type::WorkingEOB_MT Accessor
         static const uci::base::accessorType::AccessorType workingEOB_MT = 3090;

         //  This accessor type is used to identify the uci::type::WorkingEOB_QFT Accessor
         static const uci::base::accessorType::AccessorType workingEOB_QFT = 3091;

         //  This accessor type is used to identify the uci::type::WorkingSOB_ID_Type Accessor
         static const uci::base::accessorType::AccessorType workingSOB_ID_Type = 3092;

         //  This accessor type is used to identify the uci::type::WorkingSOB_MDT Accessor
         static const uci::base::accessorType::AccessorType workingSOB_MDT = 3093;

         //  This accessor type is used to identify the uci::type::WorkingSOB_MT Accessor
         static const uci::base::accessorType::AccessorType workingSOB_MT = 3094;

         //  This accessor type is used to identify the uci::type::WorkingSOB_QFT Accessor
         static const uci::base::accessorType::AccessorType workingSOB_QFT = 3095;

         //  This accessor type is used to identify the uci::type::ZoneChoiceType Accessor
         static const uci::base::accessorType::AccessorType zoneChoiceType = 3096;

         //  This accessor type is used to identify the uci::type::ZoneExternalType Accessor
         static const uci::base::accessorType::AccessorType zoneExternalType = 3097;

      } // Namespace: accessorType
      //  
      namespace zoneExternalType {

         //  The namespace in which all accessor types are declared
         namespace accessorType {

            //  This accessor type is used to identify the uci::type::ZoneExternalType::DSA_ID Accessor
            static const uci::base::accessorType::AccessorType dSA_ID = 3098;

         } // Namespace: accessorType
      } // Namespace: zoneExternalType
      //  The namespace in which all accessor types are declared
      namespace accessorType {

         //  This accessor type is used to identify the uci::type::ZoneInclusionType Accessor
         static const uci::base::accessorType::AccessorType zoneInclusionType = 3099;

         //  This accessor type is used to identify the uci::type::ZoneType Accessor
         static const uci::base::accessorType::AccessorType zoneType = 3100;

         //  This accessor type is used to identify the uci::type::ZoneViolationTriggerDataType Accessor
         static const uci::base::accessorType::AccessorType zoneViolationTriggerDataType = 3101;

         //  This accessor type is used to identify the uci::type::AccelerationType Accessor
         static const uci::base::accessorType::AccessorType accelerationType = 3102;

         //  This accessor type is used to identify the uci::type::ActivityRequestStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType activityRequestStatusEnum = 3103;

         //  This accessor type is used to identify the uci::type::ActivityStateEnum Enumeration
         static const uci::base::accessorType::AccessorType activityStateEnum = 3104;

         //  This accessor type is used to identify the uci::type::AFSPC_IdentityType Accessor
         static const uci::base::accessorType::AccessorType aFSPC_IdentityType = 3105;

         //  This accessor type is used to identify the uci::type::AircraftIdentifierType Accessor
         static const uci::base::accessorType::AccessorType aircraftIdentifierType = 3106;

         //  This accessor type is used to identify the uci::type::AirDefenseAreaType Accessor
         static const uci::base::accessorType::AccessorType airDefenseAreaType = 3107;

         //  This accessor type is used to identify the uci::type::AirSampleCapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType airSampleCapabilityEnum = 3108;

         //  This accessor type is used to identify the uci::type::AirSampleMessageOutputsEnum Enumeration
         static const uci::base::accessorType::AccessorType airSampleMessageOutputsEnum = 3109;

         //  This accessor type is used to identify the uci::type::AltitudeAdjustmentType Accessor
         static const uci::base::accessorType::AccessorType altitudeAdjustmentType = 3110;

         //  This accessor type is used to identify the uci::type::AltitudeSourceEnum Enumeration
         static const uci::base::accessorType::AccessorType altitudeSourceEnum = 3111;

         //  This accessor type is used to identify the uci::type::AltitudeType Accessor
         static const uci::base::accessorType::AccessorType altitudeType = 3112;

         //  This accessor type is used to identify the uci::type::AmbiguityOrderingEnum Enumeration
         static const uci::base::accessorType::AccessorType ambiguityOrderingEnum = 3113;

         //  This accessor type is used to identify the uci::type::AMTI_CapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType aMTI_CapabilityEnum = 3114;

         //  This accessor type is used to identify the uci::type::AMTI_MessageOutputsEnum Enumeration
         static const uci::base::accessorType::AccessorType aMTI_MessageOutputsEnum = 3115;

         //  This accessor type is used to identify the uci::type::AMTI_SubCapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType aMTI_SubCapabilityEnum = 3116;

         //  This accessor type is used to identify the uci::type::AngleHalfPositiveType Accessor
         static const uci::base::accessorType::AccessorType angleHalfPositiveType = 3117;

         //  This accessor type is used to identify the uci::type::AngleHalfType Accessor
         static const uci::base::accessorType::AccessorType angleHalfType = 3118;

         //  This accessor type is used to identify the uci::type::AnglePositiveType Accessor
         static const uci::base::accessorType::AccessorType anglePositiveType = 3119;

         //  This accessor type is used to identify the uci::type::AngleQuarterType Accessor
         static const uci::base::accessorType::AccessorType angleQuarterType = 3120;

         //  This accessor type is used to identify the uci::type::AngleRateType Accessor
         static const uci::base::accessorType::AccessorType angleRateType = 3121;

         //  This accessor type is used to identify the uci::type::AngleType Accessor
         static const uci::base::accessorType::AccessorType angleType = 3122;

         //  This accessor type is used to identify the uci::type::AntennaTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType antennaTypeEnum = 3123;

         //  This accessor type is used to identify the uci::type::AO_CapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType aO_CapabilityEnum = 3124;

         //  This accessor type is used to identify the uci::type::AO_MessageOutputsEnum Enumeration
         static const uci::base::accessorType::AccessorType aO_MessageOutputsEnum = 3125;

         //  This accessor type is used to identify the uci::type::AO_PIM_CodeType Accessor
         static const uci::base::accessorType::AccessorType aO_PIM_CodeType = 3126;

         //  This accessor type is used to identify the uci::type::AO_PRF_CodeType Accessor
         static const uci::base::accessorType::AccessorType aO_PRF_CodeType = 3127;

         //  This accessor type is used to identify the uci::type::ApproachConditionStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType approachConditionStatusEnum = 3128;

         //  This accessor type is used to identify the uci::type::ApprovalEnum Enumeration
         static const uci::base::accessorType::AccessorType approvalEnum = 3129;

         //  This accessor type is used to identify the uci::type::ApprovalRequirementEnum Enumeration
         static const uci::base::accessorType::AccessorType approvalRequirementEnum = 3130;

         //  This accessor type is used to identify the uci::type::ApprovalStateEnum Enumeration
         static const uci::base::accessorType::AccessorType approvalStateEnum = 3131;

         //  This accessor type is used to identify the uci::type::AssessmentSuppressionEnum Enumeration
         static const uci::base::accessorType::AccessorType assessmentSuppressionEnum = 3132;

         //  This accessor type is used to identify the uci::type::AttributedURI_Type Accessor
         static const uci::base::accessorType::AccessorType attributedURI_Type = 3133;

         //  This accessor type is used to identify the uci::type::AutonomousActionStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType autonomousActionStatusEnum = 3134;

         //  This accessor type is used to identify the uci::type::AutopilotModeEnum Enumeration
         static const uci::base::accessorType::AccessorType autopilotModeEnum = 3135;

         //  This accessor type is used to identify the uci::type::BaroAltitudeType Accessor
         static const uci::base::accessorType::AccessorType baroAltitudeType = 3136;

         //  This accessor type is used to identify the uci::type::BeamPowerModeEnum Enumeration
         static const uci::base::accessorType::AccessorType beamPowerModeEnum = 3137;

         //  This accessor type is used to identify the uci::type::BeamWeightingEnum Enumeration
         static const uci::base::accessorType::AccessorType beamWeightingEnum = 3138;

         //  This accessor type is used to identify the uci::type::BehavioralChangeEnum Enumeration
         static const uci::base::accessorType::AccessorType behavioralChangeEnum = 3139;

         //  This accessor type is used to identify the uci::type::CannotComplyEnum Enumeration
         static const uci::base::accessorType::AccessorType cannotComplyEnum = 3140;

         //  This accessor type is used to identify the uci::type::CapabilityAvailabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType capabilityAvailabilityEnum = 3141;

         //  This accessor type is used to identify the uci::type::CapabilityControlInterfacesEnum Enumeration
         static const uci::base::accessorType::AccessorType capabilityControlInterfacesEnum = 3142;

         //  This accessor type is used to identify the uci::type::CapabilityCoverageAreaBasisEnum Enumeration
         static const uci::base::accessorType::AccessorType capabilityCoverageAreaBasisEnum = 3143;

         //  This accessor type is used to identify the uci::type::CapabilityCoverageAreaFactorEnum Enumeration
         static const uci::base::accessorType::AccessorType capabilityCoverageAreaFactorEnum = 3144;

         //  This accessor type is used to identify the uci::type::CapabilityFalseAlarmLevelEnum Enumeration
         static const uci::base::accessorType::AccessorType capabilityFalseAlarmLevelEnum = 3145;

         //  This accessor type is used to identify the uci::type::CapabilityInitiationEnum Enumeration
         static const uci::base::accessorType::AccessorType capabilityInitiationEnum = 3146;

         //  This accessor type is used to identify the uci::type::CapabilityStateCommandEnum Enumeration
         static const uci::base::accessorType::AccessorType capabilityStateCommandEnum = 3147;

         //  This accessor type is used to identify the uci::type::CapabilityTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType capabilityTypeEnum = 3148;

         //  This accessor type is used to identify the uci::type::CargoDeliveryCapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType cargoDeliveryCapabilityEnum = 3149;

         //  This accessor type is used to identify the uci::type::CargoDeliverySubCapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType cargoDeliverySubCapabilityEnum = 3150;

         //  This accessor type is used to identify the uci::type::CargoPackagingEnum Enumeration
         static const uci::base::accessorType::AccessorType cargoPackagingEnum = 3151;

         //  This accessor type is used to identify the uci::type::CED_RestrictedNameType Accessor
         static const uci::base::accessorType::AccessorType cED_RestrictedNameType = 3152;

         //  This accessor type is used to identify the uci::type::ClearIdentityValuesEnum Enumeration
         static const uci::base::accessorType::AccessorType clearIdentityValuesEnum = 3153;

         //  This accessor type is used to identify the uci::type::ClimbOptimizationEnum Enumeration
         static const uci::base::accessorType::AccessorType climbOptimizationEnum = 3154;

         //  This accessor type is used to identify the uci::type::CollectionObjectiveEnum Enumeration
         static const uci::base::accessorType::AccessorType collectionObjectiveEnum = 3155;

         //  This accessor type is used to identify the uci::type::CollectionPatternEnum Enumeration
         static const uci::base::accessorType::AccessorType collectionPatternEnum = 3156;

         //  This accessor type is used to identify the uci::type::CollectionPolicyEnum Enumeration
         static const uci::base::accessorType::AccessorType collectionPolicyEnum = 3157;

         //  This accessor type is used to identify the uci::type::COMINT_DwellStateEnum Enumeration
         static const uci::base::accessorType::AccessorType cOMINT_DwellStateEnum = 3158;

         //  This accessor type is used to identify the uci::type::COMINT_DwellTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType cOMINT_DwellTypeEnum = 3159;

         //  This accessor type is used to identify the uci::type::COMINT_SubCapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType cOMINT_SubCapabilityEnum = 3160;

         //  This accessor type is used to identify the uci::type::CommAllocationTransitionEnum Enumeration
         static const uci::base::accessorType::AccessorType commAllocationTransitionEnum = 3161;

         //  This accessor type is used to identify the uci::type::CommandStateEnum Enumeration
         static const uci::base::accessorType::AccessorType commandStateEnum = 3162;

         //  This accessor type is used to identify the uci::type::CommAssetCategoryEnum Enumeration
         static const uci::base::accessorType::AccessorType commAssetCategoryEnum = 3163;

         //  This accessor type is used to identify the uci::type::CommAssetEstimationMessageEnum Enumeration
         static const uci::base::accessorType::AccessorType commAssetEstimationMessageEnum = 3164;

         //  This accessor type is used to identify the uci::type::CommCapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType commCapabilityEnum = 3165;

         //  This accessor type is used to identify the uci::type::CommCDL_LinkFallbackStateEnum Enumeration
         static const uci::base::accessorType::AccessorType commCDL_LinkFallbackStateEnum = 3166;

         //  This accessor type is used to identify the uci::type::CommCDL_RemoteMessageProcessingEnum Enumeration
         static const uci::base::accessorType::AccessorType commCDL_RemoteMessageProcessingEnum = 3167;

         //  This accessor type is used to identify the uci::type::CommConnectionCommandEnum Enumeration
         static const uci::base::accessorType::AccessorType commConnectionCommandEnum = 3168;

         //  This accessor type is used to identify the uci::type::CommConnectionStateEnum Enumeration
         static const uci::base::accessorType::AccessorType commConnectionStateEnum = 3169;

         //  This accessor type is used to identify the uci::type::CommCountermeasureSetEnum Enumeration
         static const uci::base::accessorType::AccessorType commCountermeasureSetEnum = 3170;

         //  This accessor type is used to identify the uci::type::CommDataDirectionFiltrationEnum Enumeration
         static const uci::base::accessorType::AccessorType commDataDirectionFiltrationEnum = 3171;

         //  This accessor type is used to identify the uci::type::CommDataFormatEnum Enumeration
         static const uci::base::accessorType::AccessorType commDataFormatEnum = 3172;

         //  This accessor type is used to identify the uci::type::CommIFDL_AcqControlEnum Enumeration
         static const uci::base::accessorType::AccessorType commIFDL_AcqControlEnum = 3173;

         //  This accessor type is used to identify the uci::type::CommIFDL_CommModeEnum Enumeration
         static const uci::base::accessorType::AccessorType commIFDL_CommModeEnum = 3174;

         //  This accessor type is used to identify the uci::type::CommIFDL_FlightSizeEnum Enumeration
         static const uci::base::accessorType::AccessorType commIFDL_FlightSizeEnum = 3175;

         //  This accessor type is used to identify the uci::type::CommIFDL_TxRateEnum Enumeration
         static const uci::base::accessorType::AccessorType commIFDL_TxRateEnum = 3176;

         //  This accessor type is used to identify the uci::type::CommInitialEntryWordsType Accessor
         static const uci::base::accessorType::AccessorType commInitialEntryWordsType = 3177;

         //  This accessor type is used to identify the uci::type::CommLink16_AdvanceSlotNotificationModeEnum Enumeration
         static const uci::base::accessorType::AccessorType commLink16_AdvanceSlotNotificationModeEnum = 3178;

         //  This accessor type is used to identify the uci::type::CommLink16_PPLI_PoolEnum Enumeration
         static const uci::base::accessorType::AccessorType commLink16_PPLI_PoolEnum = 3179;

         //  This accessor type is used to identify the uci::type::CommLink16EnhancedThroughputCodeRateEnum Enumeration
         static const uci::base::accessorType::AccessorType commLink16EnhancedThroughputCodeRateEnum = 3180;

         //  This accessor type is used to identify the uci::type::CommLink16InitialEntryGroupEnum Enumeration
         static const uci::base::accessorType::AccessorType commLink16InitialEntryGroupEnum = 3181;

         //  This accessor type is used to identify the uci::type::CommLink16InterferenceProtModeEnum Enumeration
         static const uci::base::accessorType::AccessorType commLink16InterferenceProtModeEnum = 3182;

         //  This accessor type is used to identify the uci::type::CommLink16NPGType Accessor
         static const uci::base::accessorType::AccessorType commLink16NPGType = 3183;

         //  This accessor type is used to identify the uci::type::CommLink16RangeModeEnum Enumeration
         static const uci::base::accessorType::AccessorType commLink16RangeModeEnum = 3184;

         //  This accessor type is used to identify the uci::type::CommLink16SecondarySourceTrackNumberType Accessor
         static const uci::base::accessorType::AccessorType commLink16SecondarySourceTrackNumberType = 3185;

         //  This accessor type is used to identify the uci::type::CommLink16TransmitAntennaEnum Enumeration
         static const uci::base::accessorType::AccessorType commLink16TransmitAntennaEnum = 3186;

         //  This accessor type is used to identify the uci::type::CommLink16TransmitModeEnum Enumeration
         static const uci::base::accessorType::AccessorType commLink16TransmitModeEnum = 3187;

         //  This accessor type is used to identify the uci::type::CommLink16TxPackingLimitEnum Enumeration
         static const uci::base::accessorType::AccessorType commLink16TxPackingLimitEnum = 3188;

         //  This accessor type is used to identify the uci::type::CommLink16TxPowerSelectionEnum Enumeration
         static const uci::base::accessorType::AccessorType commLink16TxPowerSelectionEnum = 3189;

         //  This accessor type is used to identify the uci::type::CommMADL_AircraftTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType commMADL_AircraftTypeEnum = 3190;

         //  This accessor type is used to identify the uci::type::CommMADL_DataRateTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType commMADL_DataRateTypeEnum = 3191;

         //  This accessor type is used to identify the uci::type::CommMADL_FlightGroupType Accessor
         static const uci::base::accessorType::AccessorType commMADL_FlightGroupType = 3192;

         //  This accessor type is used to identify the uci::type::CommMADL_LinkStateEnum Enumeration
         static const uci::base::accessorType::AccessorType commMADL_LinkStateEnum = 3193;

         //  This accessor type is used to identify the uci::type::CommMADL_NetChangeOpEnum Enumeration
         static const uci::base::accessorType::AccessorType commMADL_NetChangeOpEnum = 3194;

         //  This accessor type is used to identify the uci::type::CommMADL_NetChangeStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType commMADL_NetChangeStatusEnum = 3195;

         //  This accessor type is used to identify the uci::type::CommMADL_NetLinkOpStateEnum Enumeration
         static const uci::base::accessorType::AccessorType commMADL_NetLinkOpStateEnum = 3196;

         //  This accessor type is used to identify the uci::type::CommMADL_NetLinkSearchModeEnum Enumeration
         static const uci::base::accessorType::AccessorType commMADL_NetLinkSearchModeEnum = 3197;

         //  This accessor type is used to identify the uci::type::CommMADL_NodeDataValidityEnum Enumeration
         static const uci::base::accessorType::AccessorType commMADL_NodeDataValidityEnum = 3198;

         //  This accessor type is used to identify the uci::type::CommMADL_NodeRangeType Accessor
         static const uci::base::accessorType::AccessorType commMADL_NodeRangeType = 3199;

         //  This accessor type is used to identify the uci::type::CommMADL_SearchTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType commMADL_SearchTypeEnum = 3200;

         //  This accessor type is used to identify the uci::type::CommMADL_TimeSlotOE_PhaseTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType commMADL_TimeSlotOE_PhaseTypeEnum = 3201;

         //  This accessor type is used to identify the uci::type::CommMADL_TimeSlotTxPhaseTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType commMADL_TimeSlotTxPhaseTypeEnum = 3202;

         //  This accessor type is used to identify the uci::type::CommMADL_TimeSourceEnum Enumeration
         static const uci::base::accessorType::AccessorType commMADL_TimeSourceEnum = 3203;

         //  This accessor type is used to identify the uci::type::CommMADL_VoiceRxIndicatorTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType commMADL_VoiceRxIndicatorTypeEnum = 3204;

         //  This accessor type is used to identify the uci::type::CommModeEnum Enumeration
         static const uci::base::accessorType::AccessorType commModeEnum = 3205;

         //  This accessor type is used to identify the uci::type::CommModulationTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType commModulationTypeEnum = 3206;

         //  This accessor type is used to identify the uci::type::CommPlanActivationStateEnum Enumeration
         static const uci::base::accessorType::AccessorType commPlanActivationStateEnum = 3207;

         //  This accessor type is used to identify the uci::type::CommPlanApprovalEnum Enumeration
         static const uci::base::accessorType::AccessorType commPlanApprovalEnum = 3208;

         //  This accessor type is used to identify the uci::type::CommPointingAssessmentEnum Enumeration
         static const uci::base::accessorType::AccessorType commPointingAssessmentEnum = 3209;

         //  This accessor type is used to identify the uci::type::CommPointingRequestEnum Enumeration
         static const uci::base::accessorType::AccessorType commPointingRequestEnum = 3210;

         //  This accessor type is used to identify the uci::type::CommRangeModeTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType commRangeModeTypeEnum = 3211;

         //  This accessor type is used to identify the uci::type::CommReceiverStateEnum Enumeration
         static const uci::base::accessorType::AccessorType commReceiverStateEnum = 3212;

         //  This accessor type is used to identify the uci::type::CommRelayMessageOutputsEnum Enumeration
         static const uci::base::accessorType::AccessorType commRelayMessageOutputsEnum = 3213;

         //  This accessor type is used to identify the uci::type::CommRelayStateEnum Enumeration
         static const uci::base::accessorType::AccessorType commRelayStateEnum = 3214;

         //  This accessor type is used to identify the uci::type::CommRequestActionEnum Enumeration
         static const uci::base::accessorType::AccessorType commRequestActionEnum = 3215;

         //  This accessor type is used to identify the uci::type::CommSATCOM_ServiceTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType commSATCOM_ServiceTypeEnum = 3216;

         //  This accessor type is used to identify the uci::type::CommSchedulingRequestApprovalEnum Enumeration
         static const uci::base::accessorType::AccessorType commSchedulingRequestApprovalEnum = 3217;

         //  This accessor type is used to identify the uci::type::CommSchedulingRequirementStateEnum Enumeration
         static const uci::base::accessorType::AccessorType commSchedulingRequirementStateEnum = 3218;

         //  This accessor type is used to identify the uci::type::CommSchedulingScopeEnum Enumeration
         static const uci::base::accessorType::AccessorType commSchedulingScopeEnum = 3219;

         //  This accessor type is used to identify the uci::type::CommSINCGARS_FreqModeEnum Enumeration
         static const uci::base::accessorType::AccessorType commSINCGARS_FreqModeEnum = 3220;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanActivationCommandEnum Enumeration
         static const uci::base::accessorType::AccessorType commTerminalPlanActivationCommandEnum = 3221;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanActivationCommandStateEnum Enumeration
         static const uci::base::accessorType::AccessorType commTerminalPlanActivationCommandStateEnum = 3222;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanConfigActivationCommandEnum Enumeration
         static const uci::base::accessorType::AccessorType commTerminalPlanConfigActivationCommandEnum = 3223;

         //  This accessor type is used to identify the uci::type::CommTerminalPlanConfigStateEnum Enumeration
         static const uci::base::accessorType::AccessorType commTerminalPlanConfigStateEnum = 3224;

         //  This accessor type is used to identify the uci::type::CommTransmitterStateEnum Enumeration
         static const uci::base::accessorType::AccessorType commTransmitterStateEnum = 3225;

         //  This accessor type is used to identify the uci::type::CommunicationsImpairmentEnum Enumeration
         static const uci::base::accessorType::AccessorType communicationsImpairmentEnum = 3226;

         //  This accessor type is used to identify the uci::type::ComponentControlInterfacesEnum Enumeration
         static const uci::base::accessorType::AccessorType componentControlInterfacesEnum = 3227;

         //  This accessor type is used to identify the uci::type::ComponentStateEnum Enumeration
         static const uci::base::accessorType::AccessorType componentStateEnum = 3228;

         //  This accessor type is used to identify the uci::type::ConflictStateEnum Enumeration
         static const uci::base::accessorType::AccessorType conflictStateEnum = 3229;

         //  This accessor type is used to identify the uci::type::ConnectionStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType connectionStatusEnum = 3230;

         //  This accessor type is used to identify the uci::type::ConsentEnum Enumeration
         static const uci::base::accessorType::AccessorType consentEnum = 3231;

         //  This accessor type is used to identify the uci::type::ControlRequestEnum Enumeration
         static const uci::base::accessorType::AccessorType controlRequestEnum = 3232;

         //  This accessor type is used to identify the uci::type::ControlTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType controlTypeEnum = 3233;

         //  This accessor type is used to identify the uci::type::CoordinationRiskEnum Enumeration
         static const uci::base::accessorType::AccessorType coordinationRiskEnum = 3234;

         //  This accessor type is used to identify the uci::type::CorrelationIndicatorEnum Enumeration
         static const uci::base::accessorType::AccessorType correlationIndicatorEnum = 3235;

         //  This accessor type is used to identify the uci::type::CountryCodeType Accessor
         static const uci::base::accessorType::AccessorType countryCodeType = 3236;

         //  This accessor type is used to identify the uci::type::CreationSourceEnum Enumeration
         static const uci::base::accessorType::AccessorType creationSourceEnum = 3237;

         //  This accessor type is used to identify the uci::type::CryptoKeyStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType cryptoKeyStatusEnum = 3238;

         //  This accessor type is used to identify the uci::type::CryptoRolloverMethodEnum Enumeration
         static const uci::base::accessorType::AccessorType cryptoRolloverMethodEnum = 3239;

         //  This accessor type is used to identify the uci::type::CryptoSlotID_Type Accessor
         static const uci::base::accessorType::AccessorType cryptoSlotID_Type = 3240;

         //  This accessor type is used to identify the uci::type::CryptoTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType cryptoTypeEnum = 3241;

         //  This accessor type is used to identify the uci::type::DamageCategoryEnum Enumeration
         static const uci::base::accessorType::AccessorType damageCategoryEnum = 3242;

         //  This accessor type is used to identify the uci::type::DamageCauseEnum Enumeration
         static const uci::base::accessorType::AccessorType damageCauseEnum = 3243;

         //  This accessor type is used to identify the uci::type::DamageConfidenceEnum Enumeration
         static const uci::base::accessorType::AccessorType damageConfidenceEnum = 3244;

         //  This accessor type is used to identify the uci::type::DamagedFunctionEnum Enumeration
         static const uci::base::accessorType::AccessorType damagedFunctionEnum = 3245;

         //  This accessor type is used to identify the uci::type::DamageEstimateContentsEnum Enumeration
         static const uci::base::accessorType::AccessorType damageEstimateContentsEnum = 3246;

         //  This accessor type is used to identify the uci::type::DamageLevelEnum Enumeration
         static const uci::base::accessorType::AccessorType damageLevelEnum = 3247;

         //  This accessor type is used to identify the uci::type::DamageObservationEnum Enumeration
         static const uci::base::accessorType::AccessorType damageObservationEnum = 3248;

         //  This accessor type is used to identify the uci::type::DamageSignificanceEnum Enumeration
         static const uci::base::accessorType::AccessorType damageSignificanceEnum = 3249;

         //  This accessor type is used to identify the uci::type::DataPlanActivationCommandEnum Enumeration
         static const uci::base::accessorType::AccessorType dataPlanActivationCommandEnum = 3250;

         //  This accessor type is used to identify the uci::type::DataPlanActivationCommandStateEnum Enumeration
         static const uci::base::accessorType::AccessorType dataPlanActivationCommandStateEnum = 3251;

         //  This accessor type is used to identify the uci::type::DataPlanActivationStateEnum Enumeration
         static const uci::base::accessorType::AccessorType dataPlanActivationStateEnum = 3252;

         //  This accessor type is used to identify the uci::type::DataPlanApprovalEnum Enumeration
         static const uci::base::accessorType::AccessorType dataPlanApprovalEnum = 3253;

         //  This accessor type is used to identify the uci::type::DataPlanConfigActivationCommandEnum Enumeration
         static const uci::base::accessorType::AccessorType dataPlanConfigActivationCommandEnum = 3254;

         //  This accessor type is used to identify the uci::type::DataPlanConfigStateEnum Enumeration
         static const uci::base::accessorType::AccessorType dataPlanConfigStateEnum = 3255;

         //  This accessor type is used to identify the uci::type::DataRateType Accessor
         static const uci::base::accessorType::AccessorType dataRateType = 3256;

         //  This accessor type is used to identify the uci::type::DateTimeType Accessor
         static const uci::base::accessorType::AccessorType dateTimeType = 3257;

         //  This accessor type is used to identify the uci::type::DecibelType Accessor
         static const uci::base::accessorType::AccessorType decibelType = 3258;

         //  This accessor type is used to identify the uci::type::DefensiveJammerSystemStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType defensiveJammerSystemStatusEnum = 3259;

         //  This accessor type is used to identify the uci::type::DefensiveJammerSystemTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType defensiveJammerSystemTypeEnum = 3260;

         //  This accessor type is used to identify the uci::type::DesignationEnum Enumeration
         static const uci::base::accessorType::AccessorType designationEnum = 3261;

         //  This accessor type is used to identify the uci::type::DestinationCategoryEnum Enumeration
         static const uci::base::accessorType::AccessorType destinationCategoryEnum = 3262;

         //  This accessor type is used to identify the uci::type::DeviationType Accessor
         static const uci::base::accessorType::AccessorType deviationType = 3263;

         //  This accessor type is used to identify the uci::type::DF_ModeEnum Enumeration
         static const uci::base::accessorType::AccessorType dF_ModeEnum = 3264;

         //  This accessor type is used to identify the uci::type::DisseminationStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType disseminationStatusEnum = 3265;

         //  This accessor type is used to identify the uci::type::DistanceType Accessor
         static const uci::base::accessorType::AccessorType distanceType = 3266;

         //  This accessor type is used to identify the uci::type::DMPI_StoreStateEnum Enumeration
         static const uci::base::accessorType::AccessorType dMPI_StoreStateEnum = 3267;

         //  This accessor type is used to identify the uci::type::DoorCommandEnum Enumeration
         static const uci::base::accessorType::AccessorType doorCommandEnum = 3268;

         //  This accessor type is used to identify the uci::type::DoorStateEnum Enumeration
         static const uci::base::accessorType::AccessorType doorStateEnum = 3269;

         //  This accessor type is used to identify the uci::type::DownloadPriorityEnum Enumeration
         static const uci::base::accessorType::AccessorType downloadPriorityEnum = 3270;

         //  This accessor type is used to identify the uci::type::DrogueCommandEnum Enumeration
         static const uci::base::accessorType::AccessorType drogueCommandEnum = 3271;

         //  This accessor type is used to identify the uci::type::DurationType Accessor
         static const uci::base::accessorType::AccessorType durationType = 3272;

         //  This accessor type is used to identify the uci::type::DVR_OperationEnum Enumeration
         static const uci::base::accessorType::AccessorType dVR_OperationEnum = 3273;

         //  This accessor type is used to identify the uci::type::DynamicImageProcessingEnum Enumeration
         static const uci::base::accessorType::AccessorType dynamicImageProcessingEnum = 3274;

         //  This accessor type is used to identify the uci::type::EA_ActivationEnum Enumeration
         static const uci::base::accessorType::AccessorType eA_ActivationEnum = 3275;

         //  This accessor type is used to identify the uci::type::EA_IndicatorEnum Enumeration
         static const uci::base::accessorType::AccessorType eA_IndicatorEnum = 3276;

         //  This accessor type is used to identify the uci::type::EA_MessageOutputsEnum Enumeration
         static const uci::base::accessorType::AccessorType eA_MessageOutputsEnum = 3277;

         //  This accessor type is used to identify the uci::type::EA_ModeEnum Enumeration
         static const uci::base::accessorType::AccessorType eA_ModeEnum = 3278;

         //  This accessor type is used to identify the uci::type::EA_PointingEnum Enumeration
         static const uci::base::accessorType::AccessorType eA_PointingEnum = 3279;

         //  This accessor type is used to identify the uci::type::EccentricityType Accessor
         static const uci::base::accessorType::AccessorType eccentricityType = 3280;

         //  This accessor type is used to identify the uci::type::ElementSetNumberType Accessor
         static const uci::base::accessorType::AccessorType elementSetNumberType = 3281;

         //  This accessor type is used to identify the uci::type::ElevationScanStabilizationEnum Enumeration
         static const uci::base::accessorType::AccessorType elevationScanStabilizationEnum = 3282;

         //  This accessor type is used to identify the uci::type::ElevationVisibilityEnum Enumeration
         static const uci::base::accessorType::AccessorType elevationVisibilityEnum = 3283;

         //  This accessor type is used to identify the uci::type::EmconLevelEnum Enumeration
         static const uci::base::accessorType::AccessorType emconLevelEnum = 3284;

         //  This accessor type is used to identify the uci::type::EmissionBalanceEnum Enumeration
         static const uci::base::accessorType::AccessorType emissionBalanceEnum = 3285;

         //  This accessor type is used to identify the uci::type::EmitterUrgencyEnum Enumeration
         static const uci::base::accessorType::AccessorType emitterUrgencyEnum = 3286;

         //  This accessor type is used to identify the uci::type::EmptyType Accessor
         static const uci::base::accessorType::AccessorType emptyType = 3287;

         //  This accessor type is used to identify the uci::type::EntityDropPolicyEnum Enumeration
         static const uci::base::accessorType::AccessorType entityDropPolicyEnum = 3288;

         //  This accessor type is used to identify the uci::type::EntityFusionEligibilityEnum Enumeration
         static const uci::base::accessorType::AccessorType entityFusionEligibilityEnum = 3289;

         //  This accessor type is used to identify the uci::type::EntityFusionSimpleEligibilityEnum Enumeration
         static const uci::base::accessorType::AccessorType entityFusionSimpleEligibilityEnum = 3290;

         //  This accessor type is used to identify the uci::type::EntityManagementRequestAuthorityEnum Enumeration
         static const uci::base::accessorType::AccessorType entityManagementRequestAuthorityEnum = 3291;

         //  This accessor type is used to identify the uci::type::EntityRemoveReasonEnum Enumeration
         static const uci::base::accessorType::AccessorType entityRemoveReasonEnum = 3292;

         //  This accessor type is used to identify the uci::type::EntitySourceEnum Enumeration
         static const uci::base::accessorType::AccessorType entitySourceEnum = 3293;

         //  This accessor type is used to identify the uci::type::EntityStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType entityStatusEnum = 3294;

         //  This accessor type is used to identify the uci::type::EnvironmentEnum Enumeration
         static const uci::base::accessorType::AccessorType environmentEnum = 3295;

         //  This accessor type is used to identify the uci::type::EOB_ActivityCodeType Accessor
         static const uci::base::accessorType::AccessorType eOB_ActivityCodeType = 3296;

         //  This accessor type is used to identify the uci::type::EOB_CED_NameType Accessor
         static const uci::base::accessorType::AccessorType eOB_CED_NameType = 3297;

         //  This accessor type is used to identify the uci::type::EOB_CodeWordType Accessor
         static const uci::base::accessorType::AccessorType eOB_CodeWordType = 3298;

         //  This accessor type is used to identify the uci::type::EOB_CorrelationTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType eOB_CorrelationTypeEnum = 3299;

         //  This accessor type is used to identify the uci::type::EOB_EmitterSurrogateKeyType Accessor
         static const uci::base::accessorType::AccessorType eOB_EmitterSurrogateKeyType = 3300;

         //  This accessor type is used to identify the uci::type::EOB_EvaluationCodeType Accessor
         static const uci::base::accessorType::AccessorType eOB_EvaluationCodeType = 3301;

         //  This accessor type is used to identify the uci::type::EOB_O_SuffixType Accessor
         static const uci::base::accessorType::AccessorType eOB_O_SuffixType = 3302;

         //  This accessor type is used to identify the uci::type::EOB_OperationalStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType eOB_OperationalStatusEnum = 3303;

         //  This accessor type is used to identify the uci::type::EOB_SiteNameType Accessor
         static const uci::base::accessorType::AccessorType eOB_SiteNameType = 3304;

         //  This accessor type is used to identify the uci::type::EOB_WeaponSystemType Accessor
         static const uci::base::accessorType::AccessorType eOB_WeaponSystemType = 3305;

         //  This accessor type is used to identify the uci::type::EP_CommandedStateEnum Enumeration
         static const uci::base::accessorType::AccessorType eP_CommandedStateEnum = 3306;

         //  This accessor type is used to identify the uci::type::EP_StateEnum Enumeration
         static const uci::base::accessorType::AccessorType eP_StateEnum = 3307;

         //  This accessor type is used to identify the uci::type::EP_StatusEnum Enumeration
         static const uci::base::accessorType::AccessorType eP_StatusEnum = 3308;

         //  This accessor type is used to identify the uci::type::EqualityExpressionEnum Enumeration
         static const uci::base::accessorType::AccessorType equalityExpressionEnum = 3309;

         //  This accessor type is used to identify the uci::type::ESM_ActivationEnum Enumeration
         static const uci::base::accessorType::AccessorType eSM_ActivationEnum = 3310;

         //  This accessor type is used to identify the uci::type::ESM_DwellStateEnum Enumeration
         static const uci::base::accessorType::AccessorType eSM_DwellStateEnum = 3311;

         //  This accessor type is used to identify the uci::type::ESM_DwellTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType eSM_DwellTypeEnum = 3312;

         //  This accessor type is used to identify the uci::type::ESM_MessageOutputsEnum Enumeration
         static const uci::base::accessorType::AccessorType eSM_MessageOutputsEnum = 3313;

         //  This accessor type is used to identify the uci::type::ESM_SubCapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType eSM_SubCapabilityEnum = 3314;

         //  This accessor type is used to identify the uci::type::EstimationMessageEnum Enumeration
         static const uci::base::accessorType::AccessorType estimationMessageEnum = 3315;

         //  This accessor type is used to identify the uci::type::EventSeverityEnum Enumeration
         static const uci::base::accessorType::AccessorType eventSeverityEnum = 3316;

         //  This accessor type is used to identify the uci::type::EW_BearingEnum Enumeration
         static const uci::base::accessorType::AccessorType eW_BearingEnum = 3317;

         //  This accessor type is used to identify the uci::type::EW_FixEnum Enumeration
         static const uci::base::accessorType::AccessorType eW_FixEnum = 3318;

         //  This accessor type is used to identify the uci::type::EW_ParameterSourceEnum Enumeration
         static const uci::base::accessorType::AccessorType eW_ParameterSourceEnum = 3319;

         //  This accessor type is used to identify the uci::type::ExerciseIdentityEnum Enumeration
         static const uci::base::accessorType::AccessorType exerciseIdentityEnum = 3320;

         //  This accessor type is used to identify the uci::type::ExternalCommandExecutionStateEnum Enumeration
         static const uci::base::accessorType::AccessorType externalCommandExecutionStateEnum = 3321;

         //  This accessor type is used to identify the uci::type::ExternalDataRequestEnum Enumeration
         static const uci::base::accessorType::AccessorType externalDataRequestEnum = 3322;

         //  This accessor type is used to identify the uci::type::FaultSeverityEnum Enumeration
         static const uci::base::accessorType::AccessorType faultSeverityEnum = 3323;

         //  This accessor type is used to identify the uci::type::FaultStateEnum Enumeration
         static const uci::base::accessorType::AccessorType faultStateEnum = 3324;

         //  This accessor type is used to identify the uci::type::FileDeclassificationEnum Enumeration
         static const uci::base::accessorType::AccessorType fileDeclassificationEnum = 3325;

         //  This accessor type is used to identify the uci::type::FileNameType Accessor
         static const uci::base::accessorType::AccessorType fileNameType = 3326;

         //  This accessor type is used to identify the uci::type::FileTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType fileTypeEnum = 3327;

         //  This accessor type is used to identify the uci::type::FixedPointingEnum Enumeration
         static const uci::base::accessorType::AccessorType fixedPointingEnum = 3328;

         //  This accessor type is used to identify the uci::type::FlightDeckStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType flightDeckStatusEnum = 3329;

         //  This accessor type is used to identify the uci::type::FMOP_Enum Enumeration
         static const uci::base::accessorType::AccessorType fMOP_Enum = 3330;

         //  This accessor type is used to identify the uci::type::FOR_Enum Enumeration
         static const uci::base::accessorType::AccessorType fOR_Enum = 3331;

         //  This accessor type is used to identify the uci::type::FrequencyAgilityPatternEnum Enumeration
         static const uci::base::accessorType::AccessorType frequencyAgilityPatternEnum = 3332;

         //  This accessor type is used to identify the uci::type::FrequencyModulationVariationEnum Enumeration
         static const uci::base::accessorType::AccessorType frequencyModulationVariationEnum = 3333;

         //  This accessor type is used to identify the uci::type::FrequencyPatternPeriodicityEnum Enumeration
         static const uci::base::accessorType::AccessorType frequencyPatternPeriodicityEnum = 3334;

         //  This accessor type is used to identify the uci::type::FrequencySwitchingEnum Enumeration
         static const uci::base::accessorType::AccessorType frequencySwitchingEnum = 3335;

         //  This accessor type is used to identify the uci::type::FrequencySwitchingPatternEnum Enumeration
         static const uci::base::accessorType::AccessorType frequencySwitchingPatternEnum = 3336;

         //  This accessor type is used to identify the uci::type::FrequencyType Accessor
         static const uci::base::accessorType::AccessorType frequencyType = 3337;

         //  This accessor type is used to identify the uci::type::FuelFlowModeEnum Enumeration
         static const uci::base::accessorType::AccessorType fuelFlowModeEnum = 3338;

         //  This accessor type is used to identify the uci::type::FusionUpdatesEnum Enumeration
         static const uci::base::accessorType::AccessorType fusionUpdatesEnum = 3339;

         //  This accessor type is used to identify the uci::type::GainControlMethodEnum Enumeration
         static const uci::base::accessorType::AccessorType gainControlMethodEnum = 3340;

         //  This accessor type is used to identify the uci::type::GatewayLink16ConfigurationClassEnum Enumeration
         static const uci::base::accessorType::AccessorType gatewayLink16ConfigurationClassEnum = 3341;

         //  This accessor type is used to identify the uci::type::GatewayLink16ConfigurationCompassQuadrantEnum Enumeration
         static const uci::base::accessorType::AccessorType gatewayLink16ConfigurationCompassQuadrantEnum = 3342;

         //  This accessor type is used to identify the uci::type::GatewayLink16ConfigurationSideEnum Enumeration
         static const uci::base::accessorType::AccessorType gatewayLink16ConfigurationSideEnum = 3343;

         //  This accessor type is used to identify the uci::type::GenericUnitTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType genericUnitTypeEnum = 3344;

         //  This accessor type is used to identify the uci::type::HoseCutCommandEnum Enumeration
         static const uci::base::accessorType::AccessorType hoseCutCommandEnum = 3345;

         //  This accessor type is used to identify the uci::type::HoseCutStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType hoseCutStatusEnum = 3346;

         //  This accessor type is used to identify the uci::type::IEEE_FrequencyBandEnum Enumeration
         static const uci::base::accessorType::AccessorType iEEE_FrequencyBandEnum = 3347;

         //  This accessor type is used to identify the uci::type::ImagePixelDepthEnum Enumeration
         static const uci::base::accessorType::AccessorType imagePixelDepthEnum = 3348;

         //  This accessor type is used to identify the uci::type::ImageRemapEnum Enumeration
         static const uci::base::accessorType::AccessorType imageRemapEnum = 3349;

         //  This accessor type is used to identify the uci::type::ImageWeightEnum Enumeration
         static const uci::base::accessorType::AccessorType imageWeightEnum = 3350;

         //  This accessor type is used to identify the uci::type::IMU_AlignmentEnum Enumeration
         static const uci::base::accessorType::AccessorType iMU_AlignmentEnum = 3351;

         //  This accessor type is used to identify the uci::type::InclusionEnum Enumeration
         static const uci::base::accessorType::AccessorType inclusionEnum = 3352;

         //  This accessor type is used to identify the uci::type::IncreaseDecreaseEnum Enumeration
         static const uci::base::accessorType::AccessorType increaseDecreaseEnum = 3353;

         //  This accessor type is used to identify the uci::type::InterrogationStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType interrogationStatusEnum = 3354;

         //  This accessor type is used to identify the uci::type::IO_PortOpenCloseStateEnum Enumeration
         static const uci::base::accessorType::AccessorType iO_PortOpenCloseStateEnum = 3355;

         //  This accessor type is used to identify the uci::type::IO_PortTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType iO_PortTypeEnum = 3356;

         //  This accessor type is used to identify the uci::type::IPv6_AddressType Accessor
         static const uci::base::accessorType::AccessorType iPv6_AddressType = 3357;

         //  This accessor type is used to identify the uci::type::JammerStationEnum Enumeration
         static const uci::base::accessorType::AccessorType jammerStationEnum = 3358;

         //  This accessor type is used to identify the uci::type::JPEG_WaveletTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType jPEG_WaveletTypeEnum = 3359;

         //  This accessor type is used to identify the uci::type::LAR_BasisEnum Enumeration
         static const uci::base::accessorType::AccessorType lAR_BasisEnum = 3360;

         //  This accessor type is used to identify the uci::type::LAR_RelationEnum Enumeration
         static const uci::base::accessorType::AccessorType lAR_RelationEnum = 3361;

         //  This accessor type is used to identify the uci::type::LAR_WindSourceEnum Enumeration
         static const uci::base::accessorType::AccessorType lAR_WindSourceEnum = 3362;

         //  This accessor type is used to identify the uci::type::LaunchNumberType Accessor
         static const uci::base::accessorType::AccessorType launchNumberType = 3363;

         //  This accessor type is used to identify the uci::type::LaunchPieceType Accessor
         static const uci::base::accessorType::AccessorType launchPieceType = 3364;

         //  This accessor type is used to identify the uci::type::LineProjectionEnum Enumeration
         static const uci::base::accessorType::AccessorType lineProjectionEnum = 3365;

         //  This accessor type is used to identify the uci::type::LocalDiscreteIdentifierType Accessor
         static const uci::base::accessorType::AccessorType localDiscreteIdentifierType = 3366;

         //  This accessor type is used to identify the uci::type::LockIndicatorEnum Enumeration
         static const uci::base::accessorType::AccessorType lockIndicatorEnum = 3367;

         //  This accessor type is used to identify the uci::type::LogSeverityEnum Enumeration
         static const uci::base::accessorType::AccessorType logSeverityEnum = 3368;

         //  This accessor type is used to identify the uci::type::LOS_ReferenceEnum Enumeration
         static const uci::base::accessorType::AccessorType lOS_ReferenceEnum = 3369;

         //  This accessor type is used to identify the uci::type::LossParameterType Accessor
         static const uci::base::accessorType::AccessorType lossParameterType = 3370;

         //  This accessor type is used to identify the uci::type::MassType Accessor
         static const uci::base::accessorType::AccessorType massType = 3371;

         //  This accessor type is used to identify the uci::type::MDF_StatusEnum Enumeration
         static const uci::base::accessorType::AccessorType mDF_StatusEnum = 3372;

         //  This accessor type is used to identify the uci::type::MeasurementQualityEnum Enumeration
         static const uci::base::accessorType::AccessorType measurementQualityEnum = 3373;

         //  This accessor type is used to identify the uci::type::MeasurementStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType measurementStatusEnum = 3374;

         //  This accessor type is used to identify the uci::type::MediaLocationStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType mediaLocationStatusEnum = 3375;

         //  This accessor type is used to identify the uci::type::MergeEnum Enumeration
         static const uci::base::accessorType::AccessorType mergeEnum = 3376;

         //  This accessor type is used to identify the uci::type::MessageDataVerificationEnum Enumeration
         static const uci::base::accessorType::AccessorType messageDataVerificationEnum = 3377;

         //  This accessor type is used to identify the uci::type::MessageElementDataTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType messageElementDataTypeEnum = 3378;

         //  This accessor type is used to identify the uci::type::MessageModeEnum Enumeration
         static const uci::base::accessorType::AccessorType messageModeEnum = 3379;

         //  This accessor type is used to identify the uci::type::MessageStateEnum Enumeration
         static const uci::base::accessorType::AccessorType messageStateEnum = 3380;

         //  This accessor type is used to identify the uci::type::MessageTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType messageTypeEnum = 3381;

         //  This accessor type is used to identify the uci::type::MetaTaskStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType metaTaskStatusEnum = 3382;

         //  This accessor type is used to identify the uci::type::MetaTaskTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType metaTaskTypeEnum = 3383;

         //  This accessor type is used to identify the uci::type::MilliwattPowerRatioType Accessor
         static const uci::base::accessorType::AccessorType milliwattPowerRatioType = 3384;

         //  This accessor type is used to identify the uci::type::MIME_Type Accessor
         static const uci::base::accessorType::AccessorType mIME_Type = 3385;

         //  This accessor type is used to identify the uci::type::MISP_OperationalModeEnum Enumeration
         static const uci::base::accessorType::AccessorType mISP_OperationalModeEnum = 3386;

         //  This accessor type is used to identify the uci::type::MissileChannelType Accessor
         static const uci::base::accessorType::AccessorType missileChannelType = 3387;

         //  This accessor type is used to identify the uci::type::MissileEmitterModeEnum Enumeration
         static const uci::base::accessorType::AccessorType missileEmitterModeEnum = 3388;

         //  This accessor type is used to identify the uci::type::MissionCommunicationsStateEnum Enumeration
         static const uci::base::accessorType::AccessorType missionCommunicationsStateEnum = 3389;

         //  This accessor type is used to identify the uci::type::MissionControlInterfaceEnum Enumeration
         static const uci::base::accessorType::AccessorType missionControlInterfaceEnum = 3390;

         //  This accessor type is used to identify the uci::type::MissionPlanActivationCommandEnum Enumeration
         static const uci::base::accessorType::AccessorType missionPlanActivationCommandEnum = 3391;

         //  This accessor type is used to identify the uci::type::MissionPlanActivationCommandStateEnum Enumeration
         static const uci::base::accessorType::AccessorType missionPlanActivationCommandStateEnum = 3392;

         //  This accessor type is used to identify the uci::type::MissionPlanActivationStateEnum Enumeration
         static const uci::base::accessorType::AccessorType missionPlanActivationStateEnum = 3393;

         //  This accessor type is used to identify the uci::type::MissionPlanApprovalEnum Enumeration
         static const uci::base::accessorType::AccessorType missionPlanApprovalEnum = 3394;

         //  This accessor type is used to identify the uci::type::MissionPlanCommandSystemsEnum Enumeration
         static const uci::base::accessorType::AccessorType missionPlanCommandSystemsEnum = 3395;

         //  This accessor type is used to identify the uci::type::MissionPlanningByCaseTriggerEnum Enumeration
         static const uci::base::accessorType::AccessorType missionPlanningByCaseTriggerEnum = 3396;

         //  This accessor type is used to identify the uci::type::MissionStateEnum Enumeration
         static const uci::base::accessorType::AccessorType missionStateEnum = 3397;

         //  This accessor type is used to identify the uci::type::MissionTraceabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType missionTraceabilityEnum = 3398;

         //  This accessor type is used to identify the uci::type::MissionTypeCategory Accessor
         static const uci::base::accessorType::AccessorType missionTypeCategory = 3399;

         //  This accessor type is used to identify the uci::type::MobilityEnum Enumeration
         static const uci::base::accessorType::AccessorType mobilityEnum = 3400;

         //  This accessor type is used to identify the uci::type::Mode1CodeType Accessor
         static const uci::base::accessorType::AccessorType mode1CodeType = 3401;

         //  This accessor type is used to identify the uci::type::Mode4CodeEnum Enumeration
         static const uci::base::accessorType::AccessorType mode4CodeEnum = 3402;

         //  This accessor type is used to identify the uci::type::ModulationEnum Enumeration
         static const uci::base::accessorType::AccessorType modulationEnum = 3403;

         //  This accessor type is used to identify the uci::type::MTI_TargetClassEnum Enumeration
         static const uci::base::accessorType::AccessorType mTI_TargetClassEnum = 3404;

         //  This accessor type is used to identify the uci::type::NameSpecialCharacterRestrictionType Accessor
         static const uci::base::accessorType::AccessorType nameSpecialCharacterRestrictionType = 3405;

         //  This accessor type is used to identify the uci::type::NAV_FigureOfMeritType Accessor
         static const uci::base::accessorType::AccessorType nAV_FigureOfMeritType = 3406;

         //  This accessor type is used to identify the uci::type::NavigationSolutionStateEnum Enumeration
         static const uci::base::accessorType::AccessorType navigationSolutionStateEnum = 3407;

         //  This accessor type is used to identify the uci::type::NetworkLinkActionTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType networkLinkActionTypeEnum = 3408;

         //  This accessor type is used to identify the uci::type::NIIRS_Type Accessor
         static const uci::base::accessorType::AccessorType nIIRS_Type = 3409;

         //  This accessor type is used to identify the uci::type::NoiseAdaptiveThresholdStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType noiseAdaptiveThresholdStatusEnum = 3410;

         //  This accessor type is used to identify the uci::type::NotationType Accessor
         static const uci::base::accessorType::AccessorType notationType = 3411;

         //  This accessor type is used to identify the uci::type::NotificationCategoryEnum Enumeration
         static const uci::base::accessorType::AccessorType notificationCategoryEnum = 3412;

         //  This accessor type is used to identify the uci::type::NotificationSeverityEnum Enumeration
         static const uci::base::accessorType::AccessorType notificationSeverityEnum = 3413;

         //  This accessor type is used to identify the uci::type::NPSI_Enum Enumeration
         static const uci::base::accessorType::AccessorType nPSI_Enum = 3414;

         //  This accessor type is used to identify the uci::type::ObservedOperationalStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType observedOperationalStatusEnum = 3415;

         //  This accessor type is used to identify the uci::type::OctalValueType Accessor
         static const uci::base::accessorType::AccessorType octalValueType = 3416;

         //  This accessor type is used to identify the uci::type::OperationalCapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType operationalCapabilityEnum = 3417;

         //  This accessor type is used to identify the uci::type::OperationalImpairmentEnum Enumeration
         static const uci::base::accessorType::AccessorType operationalImpairmentEnum = 3418;

         //  This accessor type is used to identify the uci::type::OperationalStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType operationalStatusEnum = 3419;

         //  This accessor type is used to identify the uci::type::OperatorContingencyActionEnum Enumeration
         static const uci::base::accessorType::AccessorType operatorContingencyActionEnum = 3420;

         //  This accessor type is used to identify the uci::type::OperatorPhoneNumberType Accessor
         static const uci::base::accessorType::AccessorType operatorPhoneNumberType = 3421;

         //  This accessor type is used to identify the uci::type::OperatorStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType operatorStatusEnum = 3422;

         //  This accessor type is used to identify the uci::type::OpLineCategoryEnum Enumeration
         static const uci::base::accessorType::AccessorType opLineCategoryEnum = 3423;

         //  This accessor type is used to identify the uci::type::OpPointCategoryEnum Enumeration
         static const uci::base::accessorType::AccessorType opPointCategoryEnum = 3424;

         //  This accessor type is used to identify the uci::type::OpPointEmergencyEnum Enumeration
         static const uci::base::accessorType::AccessorType opPointEmergencyEnum = 3425;

         //  This accessor type is used to identify the uci::type::OpPointHazardEnum Enumeration
         static const uci::base::accessorType::AccessorType opPointHazardEnum = 3426;

         //  This accessor type is used to identify the uci::type::OpPointReferenceEnum Enumeration
         static const uci::base::accessorType::AccessorType opPointReferenceEnum = 3427;

         //  This accessor type is used to identify the uci::type::OpPointStationEnum Enumeration
         static const uci::base::accessorType::AccessorType opPointStationEnum = 3428;

         //  This accessor type is used to identify the uci::type::OpZoneFilterEnum Enumeration
         static const uci::base::accessorType::AccessorType opZoneFilterEnum = 3429;

         //  This accessor type is used to identify the uci::type::OpZoneTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType opZoneTypeEnum = 3430;

         //  This accessor type is used to identify the uci::type::OrbitalKinematicsSourceEnum Enumeration
         static const uci::base::accessorType::AccessorType orbitalKinematicsSourceEnum = 3431;

         //  This accessor type is used to identify the uci::type::OrbitAltitudeEnum Enumeration
         static const uci::base::accessorType::AccessorType orbitAltitudeEnum = 3432;

         //  This accessor type is used to identify the uci::type::OrbitEnum Enumeration
         static const uci::base::accessorType::AccessorType orbitEnum = 3433;

         //  This accessor type is used to identify the uci::type::OrderOfBattleEnum Enumeration
         static const uci::base::accessorType::AccessorType orderOfBattleEnum = 3434;

         //  This accessor type is used to identify the uci::type::PairIdentifierEnum Enumeration
         static const uci::base::accessorType::AccessorType pairIdentifierEnum = 3435;

         //  This accessor type is used to identify the uci::type::PairingRelationshipEnum Enumeration
         static const uci::base::accessorType::AccessorType pairingRelationshipEnum = 3436;

         //  This accessor type is used to identify the uci::type::ParityTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType parityTypeEnum = 3437;

         //  This accessor type is used to identify the uci::type::PassiveActiveEnum Enumeration
         static const uci::base::accessorType::AccessorType passiveActiveEnum = 3438;

         //  This accessor type is used to identify the uci::type::PathSegmentSourceEnum Enumeration
         static const uci::base::accessorType::AccessorType pathSegmentSourceEnum = 3439;

         //  This accessor type is used to identify the uci::type::PathTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType pathTypeEnum = 3440;

         //  This accessor type is used to identify the uci::type::PercentType Accessor
         static const uci::base::accessorType::AccessorType percentType = 3441;

         //  This accessor type is used to identify the uci::type::PlatformFunctionAirEnum Enumeration
         static const uci::base::accessorType::AccessorType platformFunctionAirEnum = 3442;

         //  This accessor type is used to identify the uci::type::PlatformFunctionEW_Enum Enumeration
         static const uci::base::accessorType::AccessorType platformFunctionEW_Enum = 3443;

         //  This accessor type is used to identify the uci::type::PlatformFunctionGroundEnum Enumeration
         static const uci::base::accessorType::AccessorType platformFunctionGroundEnum = 3444;

         //  This accessor type is used to identify the uci::type::PlatformFunctionSeaSurfaceEnum Enumeration
         static const uci::base::accessorType::AccessorType platformFunctionSeaSurfaceEnum = 3445;

         //  This accessor type is used to identify the uci::type::PMOP_Enum Enumeration
         static const uci::base::accessorType::AccessorType pMOP_Enum = 3446;

         //  This accessor type is used to identify the uci::type::PMOP_SequenceEnum Enumeration
         static const uci::base::accessorType::AccessorType pMOP_SequenceEnum = 3447;

         //  This accessor type is used to identify the uci::type::PO_CapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType pO_CapabilityEnum = 3448;

         //  This accessor type is used to identify the uci::type::PO_CollectionConstraintsEnum Enumeration
         static const uci::base::accessorType::AccessorType pO_CollectionConstraintsEnum = 3449;

         //  This accessor type is used to identify the uci::type::PO_FilterEnum Enumeration
         static const uci::base::accessorType::AccessorType pO_FilterEnum = 3450;

         //  This accessor type is used to identify the uci::type::PO_FPA_ChannelOperationEnum Enumeration
         static const uci::base::accessorType::AccessorType pO_FPA_ChannelOperationEnum = 3451;

         //  This accessor type is used to identify the uci::type::PO_MessageOutputsEnum Enumeration
         static const uci::base::accessorType::AccessorType pO_MessageOutputsEnum = 3452;

         //  This accessor type is used to identify the uci::type::PO_ProcessingStageTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType pO_ProcessingStageTypeEnum = 3453;

         //  This accessor type is used to identify the uci::type::PointingTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType pointingTypeEnum = 3454;

         //  This accessor type is used to identify the uci::type::PointingVectorCommandTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType pointingVectorCommandTypeEnum = 3455;

         //  This accessor type is used to identify the uci::type::PointingVectorStateEnum Enumeration
         static const uci::base::accessorType::AccessorType pointingVectorStateEnum = 3456;

         //  This accessor type is used to identify the uci::type::PointingVectorStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType pointingVectorStatusEnum = 3457;

         //  This accessor type is used to identify the uci::type::PointTrackEnum Enumeration
         static const uci::base::accessorType::AccessorType pointTrackEnum = 3458;

         //  This accessor type is used to identify the uci::type::PolarizationEnum Enumeration
         static const uci::base::accessorType::AccessorType polarizationEnum = 3459;

         //  This accessor type is used to identify the uci::type::PolarizationSenseEnum Enumeration
         static const uci::base::accessorType::AccessorType polarizationSenseEnum = 3460;

         //  This accessor type is used to identify the uci::type::PositionTimeQualityEnum Enumeration
         static const uci::base::accessorType::AccessorType positionTimeQualityEnum = 3461;

         //  This accessor type is used to identify the uci::type::POST_CapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType pOST_CapabilityEnum = 3462;

         //  This accessor type is used to identify the uci::type::POST_EnvironmentEnum Enumeration
         static const uci::base::accessorType::AccessorType pOST_EnvironmentEnum = 3463;

         //  This accessor type is used to identify the uci::type::POST_LockPersistenceEnum Enumeration
         static const uci::base::accessorType::AccessorType pOST_LockPersistenceEnum = 3464;

         //  This accessor type is used to identify the uci::type::POST_MessageOutputsEnum Enumeration
         static const uci::base::accessorType::AccessorType pOST_MessageOutputsEnum = 3465;

         //  This accessor type is used to identify the uci::type::POST_ProcessingStageTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType pOST_ProcessingStageTypeEnum = 3466;

         //  This accessor type is used to identify the uci::type::POST_SubCapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType pOST_SubCapabilityEnum = 3467;

         //  This accessor type is used to identify the uci::type::POST_TechniqueEnum Enumeration
         static const uci::base::accessorType::AccessorType pOST_TechniqueEnum = 3468;

         //  This accessor type is used to identify the uci::type::POST_TrackCapabilityTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType pOST_TrackCapabilityTypeEnum = 3469;

         //  This accessor type is used to identify the uci::type::POST_TrackCommandEnum Enumeration
         static const uci::base::accessorType::AccessorType pOST_TrackCommandEnum = 3470;

         //  This accessor type is used to identify the uci::type::POST_TrackModeEnum Enumeration
         static const uci::base::accessorType::AccessorType pOST_TrackModeEnum = 3471;

         //  This accessor type is used to identify the uci::type::PowerType Accessor
         static const uci::base::accessorType::AccessorType powerType = 3472;

         //  This accessor type is used to identify the uci::type::PRI_AgilePatternEnum Enumeration
         static const uci::base::accessorType::AccessorType pRI_AgilePatternEnum = 3473;

         //  This accessor type is used to identify the uci::type::PRI_MeasurementEnum Enumeration
         static const uci::base::accessorType::AccessorType pRI_MeasurementEnum = 3474;

         //  This accessor type is used to identify the uci::type::PRI_SwitchingPatternEnum Enumeration
         static const uci::base::accessorType::AccessorType pRI_SwitchingPatternEnum = 3475;

         //  This accessor type is used to identify the uci::type::PRI_TypeEnum Enumeration
         static const uci::base::accessorType::AccessorType pRI_TypeEnum = 3476;

         //  This accessor type is used to identify the uci::type::ProcessingStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType processingStatusEnum = 3477;

         //  This accessor type is used to identify the uci::type::ProcessingTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType processingTypeEnum = 3478;

         //  This accessor type is used to identify the uci::type::ProductClassificationRequestEnum Enumeration
         static const uci::base::accessorType::AccessorType productClassificationRequestEnum = 3479;

         //  This accessor type is used to identify the uci::type::ProductManagementTaskStateEnum Enumeration
         static const uci::base::accessorType::AccessorType productManagementTaskStateEnum = 3480;

         //  This accessor type is used to identify the uci::type::ProductTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType productTypeEnum = 3481;

         //  This accessor type is used to identify the uci::type::PushContentsEnum Enumeration
         static const uci::base::accessorType::AccessorType pushContentsEnum = 3482;

         //  This accessor type is used to identify the uci::type::QuaternionValue Accessor
         static const uci::base::accessorType::AccessorType quaternionValue = 3483;

         //  This accessor type is used to identify the uci::type::RadarChannelIndicatorEnum Enumeration
         static const uci::base::accessorType::AccessorType radarChannelIndicatorEnum = 3484;

         //  This accessor type is used to identify the uci::type::RadarChannelType Accessor
         static const uci::base::accessorType::AccessorType radarChannelType = 3485;

         //  This accessor type is used to identify the uci::type::RadarModeEnum Enumeration
         static const uci::base::accessorType::AccessorType radarModeEnum = 3486;

         //  This accessor type is used to identify the uci::type::RadarSpoilTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType radarSpoilTypeEnum = 3487;

         //  This accessor type is used to identify the uci::type::RadarWeightingFunctionsEnum Enumeration
         static const uci::base::accessorType::AccessorType radarWeightingFunctionsEnum = 3488;

         //  This accessor type is used to identify the uci::type::RECCE_SystemEnum Enumeration
         static const uci::base::accessorType::AccessorType rECCE_SystemEnum = 3489;

         //  This accessor type is used to identify the uci::type::RefuelCapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType refuelCapabilityEnum = 3490;

         //  This accessor type is used to identify the uci::type::RefuelLocationEnum Enumeration
         static const uci::base::accessorType::AccessorType refuelLocationEnum = 3491;

         //  This accessor type is used to identify the uci::type::RelativeDirectionEnum Enumeration
         static const uci::base::accessorType::AccessorType relativeDirectionEnum = 3492;

         //  This accessor type is used to identify the uci::type::RepeatEnum Enumeration
         static const uci::base::accessorType::AccessorType repeatEnum = 3493;

         //  This accessor type is used to identify the uci::type::RequestSourceEnum Enumeration
         static const uci::base::accessorType::AccessorType requestSourceEnum = 3494;

         //  This accessor type is used to identify the uci::type::ResourceAllocationStateEnum Enumeration
         static const uci::base::accessorType::AccessorType resourceAllocationStateEnum = 3495;

         //  This accessor type is used to identify the uci::type::RF_SignalContentEnum Enumeration
         static const uci::base::accessorType::AccessorType rF_SignalContentEnum = 3496;

         //  This accessor type is used to identify the uci::type::RF_TypeEnum Enumeration
         static const uci::base::accessorType::AccessorType rF_TypeEnum = 3497;

         //  This accessor type is used to identify the uci::type::RiskPrioritizationEnum Enumeration
         static const uci::base::accessorType::AccessorType riskPrioritizationEnum = 3498;

         //  This accessor type is used to identify the uci::type::RoutePlanningTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType routePlanningTypeEnum = 3499;

         //  This accessor type is used to identify the uci::type::SAM_ModeEnum Enumeration
         static const uci::base::accessorType::AccessorType sAM_ModeEnum = 3500;

         //  This accessor type is used to identify the uci::type::SAR_CapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType sAR_CapabilityEnum = 3501;

         //  This accessor type is used to identify the uci::type::SAR_CollectionReasonEnum Enumeration
         static const uci::base::accessorType::AccessorType sAR_CollectionReasonEnum = 3502;

         //  This accessor type is used to identify the uci::type::SAR_EllipticityPolicyEnum Enumeration
         static const uci::base::accessorType::AccessorType sAR_EllipticityPolicyEnum = 3503;

         //  This accessor type is used to identify the uci::type::SAR_ImagingPlaneEnum Enumeration
         static const uci::base::accessorType::AccessorType sAR_ImagingPlaneEnum = 3504;

         //  This accessor type is used to identify the uci::type::SAR_MessageOutputsEnum Enumeration
         static const uci::base::accessorType::AccessorType sAR_MessageOutputsEnum = 3505;

         //  This accessor type is used to identify the uci::type::SAR_SubCapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType sAR_SubCapabilityEnum = 3506;

         //  This accessor type is used to identify the uci::type::SAR_WaveformSelectionEnum Enumeration
         static const uci::base::accessorType::AccessorType sAR_WaveformSelectionEnum = 3507;

         //  This accessor type is used to identify the uci::type::SatelliteNameType Accessor
         static const uci::base::accessorType::AccessorType satelliteNameType = 3508;

         //  This accessor type is used to identify the uci::type::SatelliteNumberType Accessor
         static const uci::base::accessorType::AccessorType satelliteNumberType = 3509;

         //  This accessor type is used to identify the uci::type::ScalarDistanceType Accessor
         static const uci::base::accessorType::AccessorType scalarDistanceType = 3510;

         //  This accessor type is used to identify the uci::type::ScanDirectionalityEnum Enumeration
         static const uci::base::accessorType::AccessorType scanDirectionalityEnum = 3511;

         //  This accessor type is used to identify the uci::type::ScanMechanicsEnum Enumeration
         static const uci::base::accessorType::AccessorType scanMechanicsEnum = 3512;

         //  This accessor type is used to identify the uci::type::ScanPlaneEnum Enumeration
         static const uci::base::accessorType::AccessorType scanPlaneEnum = 3513;

         //  This accessor type is used to identify the uci::type::ScanTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType scanTypeEnum = 3514;

         //  This accessor type is used to identify the uci::type::SchedulingCriticalityEnum Enumeration
         static const uci::base::accessorType::AccessorType schedulingCriticalityEnum = 3515;

         //  This accessor type is used to identify the uci::type::SecurityAccessEnum Enumeration
         static const uci::base::accessorType::AccessorType securityAccessEnum = 3516;

         //  This accessor type is used to identify the uci::type::SecurityAuditManagementEnum Enumeration
         static const uci::base::accessorType::AccessorType securityAuditManagementEnum = 3517;

         //  This accessor type is used to identify the uci::type::SecurityAuthenticationEnum Enumeration
         static const uci::base::accessorType::AccessorType securityAuthenticationEnum = 3518;

         //  This accessor type is used to identify the uci::type::SecurityEventOutcomeEnum Enumeration
         static const uci::base::accessorType::AccessorType securityEventOutcomeEnum = 3519;

         //  This accessor type is used to identify the uci::type::SecurityFileManagementEnum Enumeration
         static const uci::base::accessorType::AccessorType securityFileManagementEnum = 3520;

         //  This accessor type is used to identify the uci::type::SecurityIntegrityEnum Enumeration
         static const uci::base::accessorType::AccessorType securityIntegrityEnum = 3521;

         //  This accessor type is used to identify the uci::type::SecurityIntrusionDetectionEnum Enumeration
         static const uci::base::accessorType::AccessorType securityIntrusionDetectionEnum = 3522;

         //  This accessor type is used to identify the uci::type::SecurityKeyManagementEnum Enumeration
         static const uci::base::accessorType::AccessorType securityKeyManagementEnum = 3523;

         //  This accessor type is used to identify the uci::type::SecurityPolicyManagementEnum Enumeration
         static const uci::base::accessorType::AccessorType securityPolicyManagementEnum = 3524;

         //  This accessor type is used to identify the uci::type::SecuritySanitizationEnum Enumeration
         static const uci::base::accessorType::AccessorType securitySanitizationEnum = 3525;

         //  This accessor type is used to identify the uci::type::SecuritySystemEnum Enumeration
         static const uci::base::accessorType::AccessorType securitySystemEnum = 3526;

         //  This accessor type is used to identify the uci::type::SensorReferencedCoverageAreaReferenceEnum Enumeration
         static const uci::base::accessorType::AccessorType sensorReferencedCoverageAreaReferenceEnum = 3527;

         //  This accessor type is used to identify the uci::type::ServiceLifecycleRequestStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType serviceLifecycleRequestStatusEnum = 3528;

         //  This accessor type is used to identify the uci::type::ServiceLifecycleStateEnum Enumeration
         static const uci::base::accessorType::AccessorType serviceLifecycleStateEnum = 3529;

         //  This accessor type is used to identify the uci::type::ServiceSettingEnum Enumeration
         static const uci::base::accessorType::AccessorType serviceSettingEnum = 3530;

         //  This accessor type is used to identify the uci::type::ServiceStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType serviceStatusEnum = 3531;

         //  This accessor type is used to identify the uci::type::SHA_2_256_HashType Accessor
         static const uci::base::accessorType::AccessorType sHA_2_256_HashType = 3532;

         //  This accessor type is used to identify the uci::type::SignalGeolocationControlEnum Enumeration
         static const uci::base::accessorType::AccessorType signalGeolocationControlEnum = 3533;

         //  This accessor type is used to identify the uci::type::SignalGeolocationMethodEnum Enumeration
         static const uci::base::accessorType::AccessorType signalGeolocationMethodEnum = 3534;

         //  This accessor type is used to identify the uci::type::SignalLOB_FlagEnum Enumeration
         static const uci::base::accessorType::AccessorType signalLOB_FlagEnum = 3535;

         //  This accessor type is used to identify the uci::type::SignalLOB_MethodEnum Enumeration
         static const uci::base::accessorType::AccessorType signalLOB_MethodEnum = 3536;

         //  This accessor type is used to identify the uci::type::SignalLocationEnum Enumeration
         static const uci::base::accessorType::AccessorType signalLocationEnum = 3537;

         //  This accessor type is used to identify the uci::type::SignalTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType signalTypeEnum = 3538;

         //  This accessor type is used to identify the uci::type::SimulateCommandTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType simulateCommandTypeEnum = 3539;

         //  This accessor type is used to identify the uci::type::SiteEnum Enumeration
         static const uci::base::accessorType::AccessorType siteEnum = 3540;

         //  This accessor type is used to identify the uci::type::SiteTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType siteTypeEnum = 3541;

         //  This accessor type is used to identify the uci::type::SMTI_CapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType sMTI_CapabilityEnum = 3542;

         //  This accessor type is used to identify the uci::type::SMTI_MessageOutputsEnum Enumeration
         static const uci::base::accessorType::AccessorType sMTI_MessageOutputsEnum = 3543;

         //  This accessor type is used to identify the uci::type::SMTI_SubCapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType sMTI_SubCapabilityEnum = 3544;

         //  This accessor type is used to identify the uci::type::SOB_MissionTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType sOB_MissionTypeEnum = 3545;

         //  This accessor type is used to identify the uci::type::SpecialCode2Type Accessor
         static const uci::base::accessorType::AccessorType specialCode2Type = 3546;

         //  This accessor type is used to identify the uci::type::SpecificTypeModelType Accessor
         static const uci::base::accessorType::AccessorType specificTypeModelType = 3547;

         //  This accessor type is used to identify the uci::type::SpeedOptimizationEnum Enumeration
         static const uci::base::accessorType::AccessorType speedOptimizationEnum = 3548;

         //  This accessor type is used to identify the uci::type::SpeedReferenceEnum Enumeration
         static const uci::base::accessorType::AccessorType speedReferenceEnum = 3549;

         //  This accessor type is used to identify the uci::type::SpeedType Accessor
         static const uci::base::accessorType::AccessorType speedType = 3550;

         //  This accessor type is used to identify the uci::type::SquareCircleSwitchEnum Enumeration
         static const uci::base::accessorType::AccessorType squareCircleSwitchEnum = 3551;

         //  This accessor type is used to identify the uci::type::STANAG_4607_NationalityType Accessor
         static const uci::base::accessorType::AccessorType sTANAG_4607_NationalityType = 3552;

         //  This accessor type is used to identify the uci::type::STANAG_4607_PlatformIdentifierType Accessor
         static const uci::base::accessorType::AccessorType sTANAG_4607_PlatformIdentifierType = 3553;

         //  This accessor type is used to identify the uci::type::STANAG_4607_PlatformTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType sTANAG_4607_PlatformTypeEnum = 3554;

         //  This accessor type is used to identify the uci::type::STANAG_4607_TargetClassificationEnum Enumeration
         static const uci::base::accessorType::AccessorType sTANAG_4607_TargetClassificationEnum = 3555;

         //  This accessor type is used to identify the uci::type::StandardIdentityEnum Enumeration
         static const uci::base::accessorType::AccessorType standardIdentityEnum = 3556;

         //  This accessor type is used to identify the uci::type::StateTransitionStatusEnum Enumeration
         static const uci::base::accessorType::AccessorType stateTransitionStatusEnum = 3557;

         //  This accessor type is used to identify the uci::type::StoreCategoryEnum Enumeration
         static const uci::base::accessorType::AccessorType storeCategoryEnum = 3558;

         //  This accessor type is used to identify the uci::type::StoreJettisonOptionsEnum Enumeration
         static const uci::base::accessorType::AccessorType storeJettisonOptionsEnum = 3559;

         //  This accessor type is used to identify the uci::type::StoreStateEnum Enumeration
         static const uci::base::accessorType::AccessorType storeStateEnum = 3560;

         //  This accessor type is used to identify the uci::type::StrikeMessageOutputsEnum Enumeration
         static const uci::base::accessorType::AccessorType strikeMessageOutputsEnum = 3561;

         //  This accessor type is used to identify the uci::type::String20Type Accessor
         static const uci::base::accessorType::AccessorType string20Type = 3562;

         //  This accessor type is used to identify the uci::type::String4Type Accessor
         static const uci::base::accessorType::AccessorType string4Type = 3563;

         //  This accessor type is used to identify the uci::type::SubsystemBIT_ControlInterfacesEnum Enumeration
         static const uci::base::accessorType::AccessorType subsystemBIT_ControlInterfacesEnum = 3564;

         //  This accessor type is used to identify the uci::type::SubsystemBIT_ResultEnum Enumeration
         static const uci::base::accessorType::AccessorType subsystemBIT_ResultEnum = 3565;

         //  This accessor type is used to identify the uci::type::SubsystemCalibrationControlInterfacesEnum Enumeration
         static const uci::base::accessorType::AccessorType subsystemCalibrationControlInterfacesEnum = 3566;

         //  This accessor type is used to identify the uci::type::SubsystemCalibrationResultEnum Enumeration
         static const uci::base::accessorType::AccessorType subsystemCalibrationResultEnum = 3567;

         //  This accessor type is used to identify the uci::type::SubsystemEraseCommandEnum Enumeration
         static const uci::base::accessorType::AccessorType subsystemEraseCommandEnum = 3568;

         //  This accessor type is used to identify the uci::type::SubsystemSettingEnum Enumeration
         static const uci::base::accessorType::AccessorType subsystemSettingEnum = 3569;

         //  This accessor type is used to identify the uci::type::SubsystemStateEnum Enumeration
         static const uci::base::accessorType::AccessorType subsystemStateEnum = 3570;

         //  This accessor type is used to identify the uci::type::SupportCapabilityTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType supportCapabilityTypeEnum = 3571;

         //  This accessor type is used to identify the uci::type::SwapEnum Enumeration
         static const uci::base::accessorType::AccessorType swapEnum = 3572;

         //  This accessor type is used to identify the uci::type::SystemContingencyLevelEnum Enumeration
         static const uci::base::accessorType::AccessorType systemContingencyLevelEnum = 3573;

         //  This accessor type is used to identify the uci::type::SystemSourceEnum Enumeration
         static const uci::base::accessorType::AccessorType systemSourceEnum = 3574;

         //  This accessor type is used to identify the uci::type::SystemStateEnum Enumeration
         static const uci::base::accessorType::AccessorType systemStateEnum = 3575;

         //  This accessor type is used to identify the uci::type::TailConeLightEnum Enumeration
         static const uci::base::accessorType::AccessorType tailConeLightEnum = 3576;

         //  This accessor type is used to identify the uci::type::TailNumberType Accessor
         static const uci::base::accessorType::AccessorType tailNumberType = 3577;

         //  This accessor type is used to identify the uci::type::TargetDefensesEnum Enumeration
         static const uci::base::accessorType::AccessorType targetDefensesEnum = 3578;

         //  This accessor type is used to identify the uci::type::TargetHardnessEnum Enumeration
         static const uci::base::accessorType::AccessorType targetHardnessEnum = 3579;

         //  This accessor type is used to identify the uci::type::TargetTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType targetTypeEnum = 3580;

         //  This accessor type is used to identify the uci::type::TaskAllocationTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType taskAllocationTypeEnum = 3581;

         //  This accessor type is used to identify the uci::type::TaskStateEnum Enumeration
         static const uci::base::accessorType::AccessorType taskStateEnum = 3582;

         //  This accessor type is used to identify the uci::type::TaskTransitionEnum Enumeration
         static const uci::base::accessorType::AccessorType taskTransitionEnum = 3583;

         //  This accessor type is used to identify the uci::type::TaskTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType taskTypeEnum = 3584;

         //  This accessor type is used to identify the uci::type::TemperatureStateEnum Enumeration
         static const uci::base::accessorType::AccessorType temperatureStateEnum = 3585;

         //  This accessor type is used to identify the uci::type::TemperatureType Accessor
         static const uci::base::accessorType::AccessorType temperatureType = 3586;

         //  This accessor type is used to identify the uci::type::ThreadAvailabiltyEnum Enumeration
         static const uci::base::accessorType::AccessorType threadAvailabiltyEnum = 3587;

         //  This accessor type is used to identify the uci::type::ThreatCapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType threatCapabilityEnum = 3588;

         //  This accessor type is used to identify the uci::type::ThreatEnum Enumeration
         static const uci::base::accessorType::AccessorType threatEnum = 3589;

         //  This accessor type is used to identify the uci::type::ThreatPostureEnum Enumeration
         static const uci::base::accessorType::AccessorType threatPostureEnum = 3590;

         //  This accessor type is used to identify the uci::type::ThreatTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType threatTypeEnum = 3591;

         //  This accessor type is used to identify the uci::type::ThreatWeaponEnum Enumeration
         static const uci::base::accessorType::AccessorType threatWeaponEnum = 3592;

         //  This accessor type is used to identify the uci::type::TimeFunctionEnum Enumeration
         static const uci::base::accessorType::AccessorType timeFunctionEnum = 3593;

         //  This accessor type is used to identify the uci::type::TimeType Accessor
         static const uci::base::accessorType::AccessorType timeType = 3594;

         //  This accessor type is used to identify the uci::type::TrackModeEnum Enumeration
         static const uci::base::accessorType::AccessorType trackModeEnum = 3595;

         //  This accessor type is used to identify the uci::type::TrackNumberType Accessor
         static const uci::base::accessorType::AccessorType trackNumberType = 3596;

         //  This accessor type is used to identify the uci::type::TurnPointTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType turnPointTypeEnum = 3597;

         //  This accessor type is used to identify the uci::type::TypeOfDependencyEnum Enumeration
         static const uci::base::accessorType::AccessorType typeOfDependencyEnum = 3598;

         //  This accessor type is used to identify the uci::type::UnitIdentifierType Accessor
         static const uci::base::accessorType::AccessorType unitIdentifierType = 3599;

         //  This accessor type is used to identify the uci::type::UnitNameType Accessor
         static const uci::base::accessorType::AccessorType unitNameType = 3600;

         //  This accessor type is used to identify the uci::type::UnitTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType unitTypeEnum = 3601;

         //  This accessor type is used to identify the uci::type::UserID_Type Accessor
         static const uci::base::accessorType::AccessorType userID_Type = 3602;

         //  This accessor type is used to identify the uci::type::VehicleActionEnum Enumeration
         static const uci::base::accessorType::AccessorType vehicleActionEnum = 3603;

         //  This accessor type is used to identify the uci::type::VehicleControlStrategyEnum Enumeration
         static const uci::base::accessorType::AccessorType vehicleControlStrategyEnum = 3604;

         //  This accessor type is used to identify the uci::type::VehicleLossOfLinkProcessingEnum Enumeration
         static const uci::base::accessorType::AccessorType vehicleLossOfLinkProcessingEnum = 3605;

         //  This accessor type is used to identify the uci::type::VehicleSurvivabilityModeEnum Enumeration
         static const uci::base::accessorType::AccessorType vehicleSurvivabilityModeEnum = 3606;

         //  This accessor type is used to identify the uci::type::VisibleString1024Type Accessor
         static const uci::base::accessorType::AccessorType visibleString1024Type = 3607;

         //  This accessor type is used to identify the uci::type::VisibleString256Type Accessor
         static const uci::base::accessorType::AccessorType visibleString256Type = 3608;

         //  This accessor type is used to identify the uci::type::VisibleString32Type Accessor
         static const uci::base::accessorType::AccessorType visibleString32Type = 3609;

         //  This accessor type is used to identify the uci::type::VisibleString512Type Accessor
         static const uci::base::accessorType::AccessorType visibleString512Type = 3610;

         //  This accessor type is used to identify the uci::type::VisibleString64Type Accessor
         static const uci::base::accessorType::AccessorType visibleString64Type = 3611;

         //  This accessor type is used to identify the uci::type::VoiceCallSignType Accessor
         static const uci::base::accessorType::AccessorType voiceCallSignType = 3612;

         //  This accessor type is used to identify the uci::type::VulnerabilityTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType vulnerabilityTypeEnum = 3613;

         //  This accessor type is used to identify the uci::type::WaypointTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType waypointTypeEnum = 3614;

         //  This accessor type is used to identify the uci::type::WeaponFuzeModeEnum Enumeration
         static const uci::base::accessorType::AccessorType weaponFuzeModeEnum = 3615;

         //  This accessor type is used to identify the uci::type::WeaponFuzePositionEnum Enumeration
         static const uci::base::accessorType::AccessorType weaponFuzePositionEnum = 3616;

         //  This accessor type is used to identify the uci::type::WeatherCloudCoverEnum Enumeration
         static const uci::base::accessorType::AccessorType weatherCloudCoverEnum = 3617;

         //  This accessor type is used to identify the uci::type::WeatherDataTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType weatherDataTypeEnum = 3618;

         //  This accessor type is used to identify the uci::type::WeatherDataTypeSimpleEnum Enumeration
         static const uci::base::accessorType::AccessorType weatherDataTypeSimpleEnum = 3619;

         //  This accessor type is used to identify the uci::type::WeatherPrecipitationAmplificationEnum Enumeration
         static const uci::base::accessorType::AccessorType weatherPrecipitationAmplificationEnum = 3620;

         //  This accessor type is used to identify the uci::type::WeatherPrecipitationEnum Enumeration
         static const uci::base::accessorType::AccessorType weatherPrecipitationEnum = 3621;

         //  This accessor type is used to identify the uci::type::WeatherRadarCapabilityEnum Enumeration
         static const uci::base::accessorType::AccessorType weatherRadarCapabilityEnum = 3622;

         //  This accessor type is used to identify the uci::type::WeatherRadarMessageOutputsEnum Enumeration
         static const uci::base::accessorType::AccessorType weatherRadarMessageOutputsEnum = 3623;

         //  This accessor type is used to identify the uci::type::WeatherReportTypeEnum Enumeration
         static const uci::base::accessorType::AccessorType weatherReportTypeEnum = 3624;

         //  This accessor type is used to identify the uci::type::WeatherRoadStateEnum Enumeration
         static const uci::base::accessorType::AccessorType weatherRoadStateEnum = 3625;

         //  This accessor type is used to identify the uci::type::WeatherSeaStateAmplificationEnum Enumeration
         static const uci::base::accessorType::AccessorType weatherSeaStateAmplificationEnum = 3626;

         //  This accessor type is used to identify the uci::type::WeatherSeaStateEnum Enumeration
         static const uci::base::accessorType::AccessorType weatherSeaStateEnum = 3627;

         //  This accessor type is used to identify the uci::type::WeatherSeverityEnum Enumeration
         static const uci::base::accessorType::AccessorType weatherSeverityEnum = 3628;

         //  This accessor type is used to identify the uci::type::WeatherTerrainStateEnum Enumeration
         static const uci::base::accessorType::AccessorType weatherTerrainStateEnum = 3629;

         //  This accessor type is used to identify the uci::type::WeatherWarningEnum Enumeration
         static const uci::base::accessorType::AccessorType weatherWarningEnum = 3630;

         //  This accessor type is used to identify the uci::type::WeekdayEnum Enumeration
         static const uci::base::accessorType::AccessorType weekdayEnum = 3631;

         //  This accessor type is used to identify the uci::type::YearType Accessor
         static const uci::base::accessorType::AccessorType yearType = 3632;

         //  This accessor type is used to identify the uci::type::ZoneRelationEnum Enumeration
         static const uci::base::accessorType::AccessorType zoneRelationEnum = 3633;

      } // Namespace: accessorType
   } // Namespace: type
} // Namespace: uci

#endif // Uci__Base__AccessorType_h

