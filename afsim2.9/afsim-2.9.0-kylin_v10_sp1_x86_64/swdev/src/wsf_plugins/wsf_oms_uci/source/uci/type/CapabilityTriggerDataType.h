// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__CapabilityTriggerDataType_h
#define Uci__Type__CapabilityTriggerDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AirSampleSpecificDataType_h)
# include "uci/type/AirSampleSpecificDataType.h"
#endif

#if !defined(Uci__Type__AMTI_SpecificDataType_h)
# include "uci/type/AMTI_SpecificDataType.h"
#endif

#if !defined(Uci__Type__AO_SpecificDataType_h)
# include "uci/type/AO_SpecificDataType.h"
#endif

#if !defined(Uci__Type__CAP_TaskType_h)
# include "uci/type/CAP_TaskType.h"
#endif

#if !defined(Uci__Type__CargoDeliverySpecificDataType_h)
# include "uci/type/CargoDeliverySpecificDataType.h"
#endif

#if !defined(Uci__Type__ClearAircraftTaskType_h)
# include "uci/type/ClearAircraftTaskType.h"
#endif

#if !defined(Uci__Type__COMINT_SpecificDataType_h)
# include "uci/type/COMINT_SpecificDataType.h"
#endif

#if !defined(Uci__Type__CommRelaySpecificDataType_h)
# include "uci/type/CommRelaySpecificDataType.h"
#endif

#if !defined(Uci__Type__EA_SpecificDataType_h)
# include "uci/type/EA_SpecificDataType.h"
#endif

#if !defined(Uci__Type__EngageTaskType_h)
# include "uci/type/EngageTaskType.h"
#endif

#if !defined(Uci__Type__ESM_SpecificDataType_h)
# include "uci/type/ESM_SpecificDataType.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

#if !defined(Uci__Type__PO_SpecificDataType_h)
# include "uci/type/PO_SpecificDataType.h"
#endif

#if !defined(Uci__Type__ReconnaissanceTaskType_h)
# include "uci/type/ReconnaissanceTaskType.h"
#endif

#if !defined(Uci__Type__POST_SpecificDataType_h)
# include "uci/type/POST_SpecificDataType.h"
#endif

#if !defined(Uci__Type__SAR_SpecificDataType_h)
# include "uci/type/SAR_SpecificDataType.h"
#endif

#if !defined(Uci__Type__SearchAndRescueTaskType_h)
# include "uci/type/SearchAndRescueTaskType.h"
#endif

#if !defined(Uci__Type__SMTI_SpecificDataType_h)
# include "uci/type/SMTI_SpecificDataType.h"
#endif

#if !defined(Uci__Type__StrikeSpecificDataType_h)
# include "uci/type/StrikeSpecificDataType.h"
#endif

#if !defined(Uci__Type__WeatherRadarSpecificDataType_h)
# include "uci/type/WeatherRadarSpecificDataType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CapabilityTriggerDataType sequence accessor class */
      class CapabilityTriggerDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CapabilityTriggerDataType()
         { }

         /** Returns this accessor's type constant, i.e. CapabilityTriggerDataType
           *
           * @return This accessor's type constant, i.e. CapabilityTriggerDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::capabilityTriggerDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CapabilityTriggerDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Air Sample Capability Specific Data [Minimum occurrences: 0] [Maximum occurrences: 2] */
         typedef uci::base::BoundedList<uci::type::AirSampleSpecificDataType, uci::type::accessorType::airSampleSpecificDataType> AirSample;

         /** AMTI Capability Specific Data [Minimum occurrences: 0] [Maximum occurrences: 6] */
         typedef uci::base::BoundedList<uci::type::AMTI_SpecificDataType, uci::type::accessorType::aMTI_SpecificDataType> AMTI;

         /** AO Capability Specific Data [Minimum occurrences: 0] [Maximum occurrences: 5] */
         typedef uci::base::BoundedList<uci::type::AO_SpecificDataType, uci::type::accessorType::aO_SpecificDataType> AO;

         /** Cargo Delivery Capability Specific Data [Minimum occurrences: 0] [Maximum occurrences: 4] */
         typedef uci::base::BoundedList<uci::type::CargoDeliverySpecificDataType, uci::type::accessorType::cargoDeliverySpecificDataType> CargoDelivery;

         /** COMINT Specific Data [Minimum occurrences: 0] [Maximum occurrences: 26] */
         typedef uci::base::BoundedList<uci::type::COMINT_SpecificDataType, uci::type::accessorType::cOMINT_SpecificDataType> COMINT;

         /** CommRelay Capability Specific Data [Minimum occurrences: 0] [Maximum occurrences: 29] */
         typedef uci::base::BoundedList<uci::type::CommRelaySpecificDataType, uci::type::accessorType::commRelaySpecificDataType> CommRelay;

         /** EA Capability Specific Data [Minimum occurrences: 0] [Maximum occurrences: 2] */
         typedef uci::base::BoundedList<uci::type::EA_SpecificDataType, uci::type::accessorType::eA_SpecificDataType> EA;

         /** ESM Capability Specific Data [Minimum occurrences: 0] [Maximum occurrences: 22] */
         typedef uci::base::BoundedList<uci::type::ESM_SpecificDataType, uci::type::accessorType::eSM_SpecificDataType> ESM;

         /** PO Capability Specific Data [Minimum occurrences: 0] [Maximum occurrences: 12] */
         typedef uci::base::BoundedList<uci::type::PO_SpecificDataType, uci::type::accessorType::pO_SpecificDataType> PO;

         /** POST Capability Specific Data [Minimum occurrences: 0] [Maximum occurrences: 10] */
         typedef uci::base::BoundedList<uci::type::POST_SpecificDataType, uci::type::accessorType::pOST_SpecificDataType> POST;

         /** SAR Capability Specific Data [Minimum occurrences: 0] [Maximum occurrences: 6] */
         typedef uci::base::BoundedList<uci::type::SAR_SpecificDataType, uci::type::accessorType::sAR_SpecificDataType> SAR;

         /** SMTI Capability Specific Data [Minimum occurrences: 0] [Maximum occurrences: 15] */
         typedef uci::base::BoundedList<uci::type::SMTI_SpecificDataType, uci::type::accessorType::sMTI_SpecificDataType> SMTI;

         /** Strike Capability Specific Data [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::StrikeSpecificDataType, uci::type::accessorType::strikeSpecificDataType> Strike;

         /** Returns the bounded list that is identified by the AirSample.
           *
           * @return The bounded list identified by AirSample.
           */
         virtual const uci::type::CapabilityTriggerDataType::AirSample& getAirSample() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AirSample.
           *
           * @return The bounded list identified by AirSample.
           */
         virtual uci::type::CapabilityTriggerDataType::AirSample& getAirSample()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AirSample.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAirSample(const uci::type::CapabilityTriggerDataType::AirSample& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AMTI.
           *
           * @return The bounded list identified by AMTI.
           */
         virtual const uci::type::CapabilityTriggerDataType::AMTI& getAMTI() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AMTI.
           *
           * @return The bounded list identified by AMTI.
           */
         virtual uci::type::CapabilityTriggerDataType::AMTI& getAMTI()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AMTI.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAMTI(const uci::type::CapabilityTriggerDataType::AMTI& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AO.
           *
           * @return The bounded list identified by AO.
           */
         virtual const uci::type::CapabilityTriggerDataType::AO& getAO() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AO.
           *
           * @return The bounded list identified by AO.
           */
         virtual uci::type::CapabilityTriggerDataType::AO& getAO()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AO.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAO(const uci::type::CapabilityTriggerDataType::AO& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CAP.
           *
           * @return The acecssor that provides access to the complex content that is identified by CAP.
           */
         virtual const uci::type::CAP_TaskType& getCAP() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CAP.
           *
           * @return The acecssor that provides access to the complex content that is identified by CAP.
           */
         virtual uci::type::CAP_TaskType& getCAP()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CAP to the contents of the complex content that is accessed by the
           * specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CAP
           */
         virtual void setCAP(const uci::type::CAP_TaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CAP exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CAP is emabled or not
           */
         virtual bool hasCAP() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CAP
           *
           * @param type = uci::type::accessorType::cAP_TaskType This Accessor's accessor type
           */
         virtual void enableCAP(uci::base::accessorType::AccessorType type = uci::type::accessorType::cAP_TaskType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CAP */
         virtual void clearCAP()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CargoDelivery.
           *
           * @return The bounded list identified by CargoDelivery.
           */
         virtual const uci::type::CapabilityTriggerDataType::CargoDelivery& getCargoDelivery() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CargoDelivery.
           *
           * @return The bounded list identified by CargoDelivery.
           */
         virtual uci::type::CapabilityTriggerDataType::CargoDelivery& getCargoDelivery()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CargoDelivery.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCargoDelivery(const uci::type::CapabilityTriggerDataType::CargoDelivery& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ClearAircraft.
           *
           * @return The acecssor that provides access to the complex content that is identified by ClearAircraft.
           */
         virtual const uci::type::ClearAircraftTaskType& getClearAircraft() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ClearAircraft.
           *
           * @return The acecssor that provides access to the complex content that is identified by ClearAircraft.
           */
         virtual uci::type::ClearAircraftTaskType& getClearAircraft()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ClearAircraft to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ClearAircraft
           */
         virtual void setClearAircraft(const uci::type::ClearAircraftTaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ClearAircraft exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ClearAircraft is emabled or not
           */
         virtual bool hasClearAircraft() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ClearAircraft
           *
           * @param type = uci::type::accessorType::clearAircraftTaskType This Accessor's accessor type
           */
         virtual void enableClearAircraft(uci::base::accessorType::AccessorType type = uci::type::accessorType::clearAircraftTaskType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ClearAircraft */
         virtual void clearClearAircraft()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the COMINT.
           *
           * @return The bounded list identified by COMINT.
           */
         virtual const uci::type::CapabilityTriggerDataType::COMINT& getCOMINT() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the COMINT.
           *
           * @return The bounded list identified by COMINT.
           */
         virtual uci::type::CapabilityTriggerDataType::COMINT& getCOMINT()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the COMINT.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCOMINT(const uci::type::CapabilityTriggerDataType::COMINT& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommRelay.
           *
           * @return The bounded list identified by CommRelay.
           */
         virtual const uci::type::CapabilityTriggerDataType::CommRelay& getCommRelay() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommRelay.
           *
           * @return The bounded list identified by CommRelay.
           */
         virtual uci::type::CapabilityTriggerDataType::CommRelay& getCommRelay()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommRelay.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommRelay(const uci::type::CapabilityTriggerDataType::CommRelay& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EA.
           *
           * @return The bounded list identified by EA.
           */
         virtual const uci::type::CapabilityTriggerDataType::EA& getEA() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EA.
           *
           * @return The bounded list identified by EA.
           */
         virtual uci::type::CapabilityTriggerDataType::EA& getEA()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EA.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEA(const uci::type::CapabilityTriggerDataType::EA& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Engage.
           *
           * @return The acecssor that provides access to the complex content that is identified by Engage.
           */
         virtual const uci::type::EngageTaskType& getEngage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Engage.
           *
           * @return The acecssor that provides access to the complex content that is identified by Engage.
           */
         virtual uci::type::EngageTaskType& getEngage()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Engage to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Engage
           */
         virtual void setEngage(const uci::type::EngageTaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Engage exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Engage is emabled or not
           */
         virtual bool hasEngage() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Engage
           *
           * @param type = uci::type::accessorType::engageTaskType This Accessor's accessor type
           */
         virtual void enableEngage(uci::base::accessorType::AccessorType type = uci::type::accessorType::engageTaskType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Engage */
         virtual void clearEngage()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ESM.
           *
           * @return The bounded list identified by ESM.
           */
         virtual const uci::type::CapabilityTriggerDataType::ESM& getESM() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ESM.
           *
           * @return The bounded list identified by ESM.
           */
         virtual uci::type::CapabilityTriggerDataType::ESM& getESM()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ESM.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setESM(const uci::type::CapabilityTriggerDataType::ESM& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the GoToVoice.
           *
           * @return The value of the string data type identified by GoToVoice.
           */
         virtual const uci::type::EmptyType& getGoToVoice() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the GoToVoice.
           *
           * @return The value of the string data type identified by GoToVoice.
           */
         virtual uci::type::EmptyType& getGoToVoice()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the GoToVoice to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setGoToVoice(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the GoToVoice to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setGoToVoice(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the GoToVoice to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setGoToVoice(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GoToVoice exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GoToVoice is emabled or not
           */
         virtual bool hasGoToVoice() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GoToVoice
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enableGoToVoice(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GoToVoice */
         virtual void clearGoToVoice()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PO.
           *
           * @return The bounded list identified by PO.
           */
         virtual const uci::type::CapabilityTriggerDataType::PO& getPO() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PO.
           *
           * @return The bounded list identified by PO.
           */
         virtual uci::type::CapabilityTriggerDataType::PO& getPO()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PO.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPO(const uci::type::CapabilityTriggerDataType::PO& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Reconnaissance.
           *
           * @return The acecssor that provides access to the complex content that is identified by Reconnaissance.
           */
         virtual const uci::type::ReconnaissanceTaskType& getReconnaissance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Reconnaissance.
           *
           * @return The acecssor that provides access to the complex content that is identified by Reconnaissance.
           */
         virtual uci::type::ReconnaissanceTaskType& getReconnaissance()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Reconnaissance to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Reconnaissance
           */
         virtual void setReconnaissance(const uci::type::ReconnaissanceTaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Reconnaissance exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Reconnaissance is emabled or not
           */
         virtual bool hasReconnaissance() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Reconnaissance
           *
           * @param type = uci::type::accessorType::reconnaissanceTaskType This Accessor's accessor type
           */
         virtual void enableReconnaissance(uci::base::accessorType::AccessorType type = uci::type::accessorType::reconnaissanceTaskType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Reconnaissance */
         virtual void clearReconnaissance()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ReportTargets.
           *
           * @return The value of the string data type identified by ReportTargets.
           */
         virtual const uci::type::EmptyType& getReportTargets() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ReportTargets.
           *
           * @return The value of the string data type identified by ReportTargets.
           */
         virtual uci::type::EmptyType& getReportTargets()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ReportTargets to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setReportTargets(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ReportTargets to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReportTargets(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ReportTargets to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReportTargets(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReportTargets exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReportTargets is emabled or not
           */
         virtual bool hasReportTargets() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReportTargets
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enableReportTargets(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReportTargets */
         virtual void clearReportTargets()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the POST.
           *
           * @return The bounded list identified by POST.
           */
         virtual const uci::type::CapabilityTriggerDataType::POST& getPOST() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the POST.
           *
           * @return The bounded list identified by POST.
           */
         virtual uci::type::CapabilityTriggerDataType::POST& getPOST()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the POST.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPOST(const uci::type::CapabilityTriggerDataType::POST& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SAR.
           *
           * @return The bounded list identified by SAR.
           */
         virtual const uci::type::CapabilityTriggerDataType::SAR& getSAR() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SAR.
           *
           * @return The bounded list identified by SAR.
           */
         virtual uci::type::CapabilityTriggerDataType::SAR& getSAR()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SAR.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSAR(const uci::type::CapabilityTriggerDataType::SAR& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SearchAndRescue.
           *
           * @return The acecssor that provides access to the complex content that is identified by SearchAndRescue.
           */
         virtual const uci::type::SearchAndRescueTaskType& getSearchAndRescue() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SearchAndRescue.
           *
           * @return The acecssor that provides access to the complex content that is identified by SearchAndRescue.
           */
         virtual uci::type::SearchAndRescueTaskType& getSearchAndRescue()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SearchAndRescue to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SearchAndRescue
           */
         virtual void setSearchAndRescue(const uci::type::SearchAndRescueTaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SearchAndRescue exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SearchAndRescue is emabled or not
           */
         virtual bool hasSearchAndRescue() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SearchAndRescue
           *
           * @param type = uci::type::accessorType::searchAndRescueTaskType This Accessor's accessor type
           */
         virtual void enableSearchAndRescue(uci::base::accessorType::AccessorType type = uci::type::accessorType::searchAndRescueTaskType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SearchAndRescue */
         virtual void clearSearchAndRescue()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SMTI.
           *
           * @return The bounded list identified by SMTI.
           */
         virtual const uci::type::CapabilityTriggerDataType::SMTI& getSMTI() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SMTI.
           *
           * @return The bounded list identified by SMTI.
           */
         virtual uci::type::CapabilityTriggerDataType::SMTI& getSMTI()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SMTI.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSMTI(const uci::type::CapabilityTriggerDataType::SMTI& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Strike.
           *
           * @return The bounded list identified by Strike.
           */
         virtual const uci::type::CapabilityTriggerDataType::Strike& getStrike() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Strike.
           *
           * @return The bounded list identified by Strike.
           */
         virtual uci::type::CapabilityTriggerDataType::Strike& getStrike()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Strike.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setStrike(const uci::type::CapabilityTriggerDataType::Strike& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherRadar.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherRadar.
           */
         virtual const uci::type::WeatherRadarSpecificDataType& getWeatherRadar() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherRadar.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherRadar.
           */
         virtual uci::type::WeatherRadarSpecificDataType& getWeatherRadar()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WeatherRadar to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WeatherRadar
           */
         virtual void setWeatherRadar(const uci::type::WeatherRadarSpecificDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WeatherRadar exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WeatherRadar is emabled or not
           */
         virtual bool hasWeatherRadar() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WeatherRadar
           *
           * @param type = uci::type::accessorType::weatherRadarSpecificDataType This Accessor's accessor type
           */
         virtual void enableWeatherRadar(uci::base::accessorType::AccessorType type = uci::type::accessorType::weatherRadarSpecificDataType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WeatherRadar */
         virtual void clearWeatherRadar()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityTriggerDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityTriggerDataType to copy from
           */
         CapabilityTriggerDataType(const CapabilityTriggerDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityTriggerDataType to the contents of the
           * CapabilityTriggerDataType on the right hand side (rhs) of the assignment operator.CapabilityTriggerDataType [only
           * available to derived classes]
           *
           * @param rhs The CapabilityTriggerDataType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CapabilityTriggerDataType
           * @return A constant reference to this CapabilityTriggerDataType.
           */
         const CapabilityTriggerDataType& operator=(const CapabilityTriggerDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityTriggerDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CapabilityTriggerDataType_h

