// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__TaskType_h
#define Uci__Type__TaskType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AirSampleTaskType_h)
# include "uci/type/AirSampleTaskType.h"
#endif

#if !defined(Uci__Type__AO_TaskType_h)
# include "uci/type/AO_TaskType.h"
#endif

#if !defined(Uci__Type__AMTI_TaskType_h)
# include "uci/type/AMTI_TaskType.h"
#endif

#if !defined(Uci__Type__CAP_TaskType_h)
# include "uci/type/CAP_TaskType.h"
#endif

#if !defined(Uci__Type__CargoDeliveryTaskType_h)
# include "uci/type/CargoDeliveryTaskType.h"
#endif

#if !defined(Uci__Type__ClearAircraftTaskType_h)
# include "uci/type/ClearAircraftTaskType.h"
#endif

#if !defined(Uci__Type__COMINT_TaskType_h)
# include "uci/type/COMINT_TaskType.h"
#endif

#if !defined(Uci__Type__CommRelayTaskType_h)
# include "uci/type/CommRelayTaskType.h"
#endif

#if !defined(Uci__Type__EA_TaskType_h)
# include "uci/type/EA_TaskType.h"
#endif

#if !defined(Uci__Type__EngageTaskType_h)
# include "uci/type/EngageTaskType.h"
#endif

#if !defined(Uci__Type__ESM_TaskType_h)
# include "uci/type/ESM_TaskType.h"
#endif

#if !defined(Uci__Type__FlightTaskType_h)
# include "uci/type/FlightTaskType.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

#if !defined(Uci__Type__PO_TaskType_h)
# include "uci/type/PO_TaskType.h"
#endif

#if !defined(Uci__Type__POST_TaskType_h)
# include "uci/type/POST_TaskType.h"
#endif

#if !defined(Uci__Type__ReconnaissanceTaskType_h)
# include "uci/type/ReconnaissanceTaskType.h"
#endif

#if !defined(Uci__Type__SAR_TaskType_h)
# include "uci/type/SAR_TaskType.h"
#endif

#if !defined(Uci__Type__SearchAndRescueTaskType_h)
# include "uci/type/SearchAndRescueTaskType.h"
#endif

#if !defined(Uci__Type__SMTI_TaskType_h)
# include "uci/type/SMTI_TaskType.h"
#endif

#if !defined(Uci__Type__StrikeTaskType_h)
# include "uci/type/StrikeTaskType.h"
#endif

#if !defined(Uci__Type__WeatherRadarTaskType_h)
# include "uci/type/WeatherRadarTaskType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Identifies the type of this Task instance. Note: When modifying this complexType (whether adding or removing
        * choices), there are equavalient complexTypes that require the same modifications. Changes to TaskTriggerTaskType and
        * CapabilityTriggerDataType will likely require the same changes.
        */
      class TaskType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~TaskType()
         { }

         /** Returns this accessor's type constant, i.e. TaskType
           *
           * @return This accessor's type constant, i.e. TaskType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::taskType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TaskType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum TaskTypeChoice {
            TASKTYPE_CHOICE_NONE,
            TASKTYPE_CHOICE_AIRSAMPLE,
            TASKTYPE_CHOICE_AO,
            TASKTYPE_CHOICE_AMTI,
            TASKTYPE_CHOICE_CAP,
            TASKTYPE_CHOICE_CARGODELIVERY,
            TASKTYPE_CHOICE_CLEARAIRCRAFT,
            TASKTYPE_CHOICE_COMINT,
            TASKTYPE_CHOICE_COMMRELAY,
            TASKTYPE_CHOICE_EA,
            TASKTYPE_CHOICE_ENGAGE,
            TASKTYPE_CHOICE_ESM,
            TASKTYPE_CHOICE_FLIGHT,
            TASKTYPE_CHOICE_GOTOVOICE,
            TASKTYPE_CHOICE_PO,
            TASKTYPE_CHOICE_POST,
            TASKTYPE_CHOICE_RECONNAISSANCE,
            TASKTYPE_CHOICE_REPORTTARGETS,
            TASKTYPE_CHOICE_SAR,
            TASKTYPE_CHOICE_SEARCHANDRESCUE,
            TASKTYPE_CHOICE_SMTI,
            TASKTYPE_CHOICE_STRIKE,
            TASKTYPE_CHOICE_WEATHERRADAR
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual TaskTypeChoice getTaskTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getTaskTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the access
           * methods associated with the chosen element will be enabled and will provide access to the chosen element.
           *
           * @param chosenElementOrdinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setTaskTypeChoiceOrdinal(TaskTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AirSample.
           *
           * @return The acecssor that provides access to the complex content that is identified by AirSample.
           */
         virtual const uci::type::AirSampleTaskType& getAirSample() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AirSample.
           *
           * @return The acecssor that provides access to the complex content that is identified by AirSample.
           */
         virtual uci::type::AirSampleTaskType& getAirSample()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AirSample to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AirSample
           */
         virtual void setAirSample(const uci::type::AirSampleTaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AO.
           *
           * @return The acecssor that provides access to the complex content that is identified by AO.
           */
         virtual const uci::type::AO_TaskType& getAO() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AO.
           *
           * @return The acecssor that provides access to the complex content that is identified by AO.
           */
         virtual uci::type::AO_TaskType& getAO()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AO to the contents of the complex content that is accessed by the
           * specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AO
           */
         virtual void setAO(const uci::type::AO_TaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AMTI.
           *
           * @return The acecssor that provides access to the complex content that is identified by AMTI.
           */
         virtual const uci::type::AMTI_TaskType& getAMTI() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AMTI.
           *
           * @return The acecssor that provides access to the complex content that is identified by AMTI.
           */
         virtual uci::type::AMTI_TaskType& getAMTI()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AMTI to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AMTI
           */
         virtual void setAMTI(const uci::type::AMTI_TaskType& value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the CargoDelivery.
           *
           * @return The acecssor that provides access to the complex content that is identified by CargoDelivery.
           */
         virtual const uci::type::CargoDeliveryTaskType& getCargoDelivery() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CargoDelivery.
           *
           * @return The acecssor that provides access to the complex content that is identified by CargoDelivery.
           */
         virtual uci::type::CargoDeliveryTaskType& getCargoDelivery()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CargoDelivery to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CargoDelivery
           */
         virtual void setCargoDelivery(const uci::type::CargoDeliveryTaskType& value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the COMINT.
           *
           * @return The acecssor that provides access to the complex content that is identified by COMINT.
           */
         virtual const uci::type::COMINT_TaskType& getCOMINT() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the COMINT.
           *
           * @return The acecssor that provides access to the complex content that is identified by COMINT.
           */
         virtual uci::type::COMINT_TaskType& getCOMINT()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the COMINT to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by COMINT
           */
         virtual void setCOMINT(const uci::type::COMINT_TaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommRelay.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommRelay.
           */
         virtual const uci::type::CommRelayTaskType& getCommRelay() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommRelay.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommRelay.
           */
         virtual uci::type::CommRelayTaskType& getCommRelay()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommRelay to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommRelay
           */
         virtual void setCommRelay(const uci::type::CommRelayTaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EA.
           *
           * @return The acecssor that provides access to the complex content that is identified by EA.
           */
         virtual const uci::type::EA_TaskType& getEA() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EA.
           *
           * @return The acecssor that provides access to the complex content that is identified by EA.
           */
         virtual uci::type::EA_TaskType& getEA()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EA to the contents of the complex content that is accessed by the
           * specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EA
           */
         virtual void setEA(const uci::type::EA_TaskType& value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the ESM.
           *
           * @return The acecssor that provides access to the complex content that is identified by ESM.
           */
         virtual const uci::type::ESM_TaskType& getESM() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ESM.
           *
           * @return The acecssor that provides access to the complex content that is identified by ESM.
           */
         virtual uci::type::ESM_TaskType& getESM()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ESM to the contents of the complex content that is accessed by the
           * specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ESM
           */
         virtual void setESM(const uci::type::ESM_TaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Flight.
           *
           * @return The acecssor that provides access to the complex content that is identified by Flight.
           */
         virtual const uci::type::FlightTaskType& getFlight() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Flight.
           *
           * @return The acecssor that provides access to the complex content that is identified by Flight.
           */
         virtual uci::type::FlightTaskType& getFlight()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Flight to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Flight
           */
         virtual void setFlight(const uci::type::FlightTaskType& value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the PO.
           *
           * @return The acecssor that provides access to the complex content that is identified by PO.
           */
         virtual const uci::type::PO_TaskType& getPO() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PO.
           *
           * @return The acecssor that provides access to the complex content that is identified by PO.
           */
         virtual uci::type::PO_TaskType& getPO()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PO to the contents of the complex content that is accessed by the
           * specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PO
           */
         virtual void setPO(const uci::type::PO_TaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the POST.
           *
           * @return The acecssor that provides access to the complex content that is identified by POST.
           */
         virtual const uci::type::POST_TaskType& getPOST() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the POST.
           *
           * @return The acecssor that provides access to the complex content that is identified by POST.
           */
         virtual uci::type::POST_TaskType& getPOST()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the POST to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by POST
           */
         virtual void setPOST(const uci::type::POST_TaskType& value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the SAR.
           *
           * @return The acecssor that provides access to the complex content that is identified by SAR.
           */
         virtual const uci::type::SAR_TaskType& getSAR() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SAR.
           *
           * @return The acecssor that provides access to the complex content that is identified by SAR.
           */
         virtual uci::type::SAR_TaskType& getSAR()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SAR to the contents of the complex content that is accessed by the
           * specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SAR
           */
         virtual void setSAR(const uci::type::SAR_TaskType& value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the SMTI.
           *
           * @return The acecssor that provides access to the complex content that is identified by SMTI.
           */
         virtual const uci::type::SMTI_TaskType& getSMTI() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SMTI.
           *
           * @return The acecssor that provides access to the complex content that is identified by SMTI.
           */
         virtual uci::type::SMTI_TaskType& getSMTI()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SMTI to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SMTI
           */
         virtual void setSMTI(const uci::type::SMTI_TaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Strike.
           *
           * @return The acecssor that provides access to the complex content that is identified by Strike.
           */
         virtual const uci::type::StrikeTaskType& getStrike() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Strike.
           *
           * @return The acecssor that provides access to the complex content that is identified by Strike.
           */
         virtual uci::type::StrikeTaskType& getStrike()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Strike to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Strike
           */
         virtual void setStrike(const uci::type::StrikeTaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherRadar.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherRadar.
           */
         virtual const uci::type::WeatherRadarTaskType& getWeatherRadar() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherRadar.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherRadar.
           */
         virtual uci::type::WeatherRadarTaskType& getWeatherRadar()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WeatherRadar to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WeatherRadar
           */
         virtual void setWeatherRadar(const uci::type::WeatherRadarTaskType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TaskType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TaskType to copy from
           */
         TaskType(const TaskType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TaskType to the contents of the TaskType on the right hand side
           * (rhs) of the assignment operator.TaskType [only available to derived classes]
           *
           * @param rhs The TaskType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::TaskType
           * @return A constant reference to this TaskType.
           */
         const TaskType& operator=(const TaskType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TaskType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TaskType_h

