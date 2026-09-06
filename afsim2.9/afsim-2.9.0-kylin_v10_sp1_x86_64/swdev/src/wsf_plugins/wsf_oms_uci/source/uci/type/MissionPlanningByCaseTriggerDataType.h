// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__MissionPlanningByCaseTriggerDataType_h
#define Uci__Type__MissionPlanningByCaseTriggerDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityTriggerDataType_h)
# include "uci/type/CapabilityTriggerDataType.h"
#endif

#if !defined(Uci__Type__CommsLostTriggerDataType_h)
# include "uci/type/CommsLostTriggerDataType.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

#if !defined(Uci__Type__EnduranceType_h)
# include "uci/type/EnduranceType.h"
#endif

#if !defined(Uci__Type__ThresholdOffRouteTriggerDataType_h)
# include "uci/type/ThresholdOffRouteTriggerDataType.h"
#endif

#if !defined(Uci__Type__RouteVulnerabilityTriggerDataType_h)
# include "uci/type/RouteVulnerabilityTriggerDataType.h"
#endif

#if !defined(Uci__Type__TaskTriggerDataType_h)
# include "uci/type/TaskTriggerDataType.h"
#endif

#if !defined(Uci__Type__TaskFailedTriggerDataType_h)
# include "uci/type/TaskFailedTriggerDataType.h"
#endif

#if !defined(Uci__Type__ZoneViolationTriggerDataType_h)
# include "uci/type/ZoneViolationTriggerDataType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionPlanningByCaseTriggerDataType choice accessor class */
      class MissionPlanningByCaseTriggerDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MissionPlanningByCaseTriggerDataType()
         { }

         /** Returns this accessor's type constant, i.e. MissionPlanningByCaseTriggerDataType
           *
           * @return This accessor's type constant, i.e. MissionPlanningByCaseTriggerDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionPlanningByCaseTriggerDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionPlanningByCaseTriggerDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum MissionPlanningByCaseTriggerDataTypeChoice {
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_NONE,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_CAPABILITYADDED,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_CAPABILITYFAILURE,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_COMMSLOST,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_DMPI_OVERDESIGNATION,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_DMPI_UNDERDESIGNATION,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_ENDURANCELOW,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_OFFROUTE,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_PROXIMITYCONFLICT,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_RELEASEPOINTOUTSIDELAR,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_ROUTECONFLICT,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_ROUTEVULNERABILITY,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_SYSTEMFAILED,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_TASKADDED,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_TASKDEPENDENCYFAILED,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_TASKDROPPED,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_TASKFAILED,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_TASKMOVED,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_TASKTIMING,
            MISSIONPLANNINGBYCASETRIGGERDATATYPE_CHOICE_ZONEVIOLATION
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual MissionPlanningByCaseTriggerDataTypeChoice getMissionPlanningByCaseTriggerDataTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getMissionPlanningByCaseTriggerDataTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In
           * addition, the access methods associated with the chosen element will be enabled and will provide access to the chosen
           * element.
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
         virtual void setMissionPlanningByCaseTriggerDataTypeChoiceOrdinal(MissionPlanningByCaseTriggerDataTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityAdded.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityAdded.
           */
         virtual const uci::type::CapabilityTriggerDataType& getCapabilityAdded() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityAdded.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityAdded.
           */
         virtual uci::type::CapabilityTriggerDataType& getCapabilityAdded()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityAdded to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityAdded
           */
         virtual void setCapabilityAdded(const uci::type::CapabilityTriggerDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityFailure.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityFailure.
           */
         virtual const uci::type::CapabilityTriggerDataType& getCapabilityFailure() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityFailure.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityFailure.
           */
         virtual uci::type::CapabilityTriggerDataType& getCapabilityFailure()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityFailure to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityFailure
           */
         virtual void setCapabilityFailure(const uci::type::CapabilityTriggerDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommsLost.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommsLost.
           */
         virtual const uci::type::CommsLostTriggerDataType& getCommsLost() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommsLost.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommsLost.
           */
         virtual uci::type::CommsLostTriggerDataType& getCommsLost()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommsLost to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommsLost
           */
         virtual void setCommsLost(const uci::type::CommsLostTriggerDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DMPI_OverDesignation.
           *
           * @return The value of the string data type identified by DMPI_OverDesignation.
           */
         virtual const uci::type::EmptyType& getDMPI_OverDesignation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DMPI_OverDesignation.
           *
           * @return The value of the string data type identified by DMPI_OverDesignation.
           */
         virtual uci::type::EmptyType& getDMPI_OverDesignation()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DMPI_OverDesignation to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setDMPI_OverDesignation(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DMPI_OverDesignation to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDMPI_OverDesignation(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DMPI_OverDesignation to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDMPI_OverDesignation(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DMPI_UnderDesignation.
           *
           * @return The value of the string data type identified by DMPI_UnderDesignation.
           */
         virtual const uci::type::EmptyType& getDMPI_UnderDesignation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DMPI_UnderDesignation.
           *
           * @return The value of the string data type identified by DMPI_UnderDesignation.
           */
         virtual uci::type::EmptyType& getDMPI_UnderDesignation()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DMPI_UnderDesignation to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setDMPI_UnderDesignation(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DMPI_UnderDesignation to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDMPI_UnderDesignation(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DMPI_UnderDesignation to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDMPI_UnderDesignation(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EnduranceLow.
           *
           * @return The acecssor that provides access to the complex content that is identified by EnduranceLow.
           */
         virtual const uci::type::EnduranceType& getEnduranceLow() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EnduranceLow.
           *
           * @return The acecssor that provides access to the complex content that is identified by EnduranceLow.
           */
         virtual uci::type::EnduranceType& getEnduranceLow()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EnduranceLow to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EnduranceLow
           */
         virtual void setEnduranceLow(const uci::type::EnduranceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OffRoute.
           *
           * @return The acecssor that provides access to the complex content that is identified by OffRoute.
           */
         virtual const uci::type::ThresholdOffRouteTriggerDataType& getOffRoute() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OffRoute.
           *
           * @return The acecssor that provides access to the complex content that is identified by OffRoute.
           */
         virtual uci::type::ThresholdOffRouteTriggerDataType& getOffRoute()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OffRoute to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OffRoute
           */
         virtual void setOffRoute(const uci::type::ThresholdOffRouteTriggerDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ProximityConflict.
           *
           * @return The value of the string data type identified by ProximityConflict.
           */
         virtual const uci::type::EmptyType& getProximityConflict() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ProximityConflict.
           *
           * @return The value of the string data type identified by ProximityConflict.
           */
         virtual uci::type::EmptyType& getProximityConflict()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ProximityConflict to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setProximityConflict(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ProximityConflict to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setProximityConflict(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ProximityConflict to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setProximityConflict(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ReleasePointOutsideLAR.
           *
           * @return The value of the string data type identified by ReleasePointOutsideLAR.
           */
         virtual const uci::type::EmptyType& getReleasePointOutsideLAR() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ReleasePointOutsideLAR.
           *
           * @return The value of the string data type identified by ReleasePointOutsideLAR.
           */
         virtual uci::type::EmptyType& getReleasePointOutsideLAR()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ReleasePointOutsideLAR to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setReleasePointOutsideLAR(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ReleasePointOutsideLAR to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReleasePointOutsideLAR(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ReleasePointOutsideLAR to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReleasePointOutsideLAR(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the RouteConflict.
           *
           * @return The value of the string data type identified by RouteConflict.
           */
         virtual const uci::type::EmptyType& getRouteConflict() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the RouteConflict.
           *
           * @return The value of the string data type identified by RouteConflict.
           */
         virtual uci::type::EmptyType& getRouteConflict()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the RouteConflict to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setRouteConflict(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the RouteConflict to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRouteConflict(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the RouteConflict to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRouteConflict(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteVulnerability.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteVulnerability.
           */
         virtual const uci::type::RouteVulnerabilityTriggerDataType& getRouteVulnerability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteVulnerability.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteVulnerability.
           */
         virtual uci::type::RouteVulnerabilityTriggerDataType& getRouteVulnerability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RouteVulnerability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RouteVulnerability
           */
         virtual void setRouteVulnerability(const uci::type::RouteVulnerabilityTriggerDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SystemFailed.
           *
           * @return The value of the string data type identified by SystemFailed.
           */
         virtual const uci::type::EmptyType& getSystemFailed() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SystemFailed.
           *
           * @return The value of the string data type identified by SystemFailed.
           */
         virtual uci::type::EmptyType& getSystemFailed()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SystemFailed to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSystemFailed(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SystemFailed to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSystemFailed(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SystemFailed to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSystemFailed(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskAdded.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskAdded.
           */
         virtual const uci::type::TaskTriggerDataType& getTaskAdded() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskAdded.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskAdded.
           */
         virtual uci::type::TaskTriggerDataType& getTaskAdded()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskAdded to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskAdded
           */
         virtual void setTaskAdded(const uci::type::TaskTriggerDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TaskDependencyFailed.
           *
           * @return The value of the string data type identified by TaskDependencyFailed.
           */
         virtual const uci::type::EmptyType& getTaskDependencyFailed() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TaskDependencyFailed.
           *
           * @return The value of the string data type identified by TaskDependencyFailed.
           */
         virtual uci::type::EmptyType& getTaskDependencyFailed()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TaskDependencyFailed to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setTaskDependencyFailed(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TaskDependencyFailed to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTaskDependencyFailed(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TaskDependencyFailed to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTaskDependencyFailed(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskDropped.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskDropped.
           */
         virtual const uci::type::TaskTriggerDataType& getTaskDropped() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskDropped.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskDropped.
           */
         virtual uci::type::TaskTriggerDataType& getTaskDropped()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskDropped to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskDropped
           */
         virtual void setTaskDropped(const uci::type::TaskTriggerDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskFailed.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskFailed.
           */
         virtual const uci::type::TaskFailedTriggerDataType& getTaskFailed() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskFailed.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskFailed.
           */
         virtual uci::type::TaskFailedTriggerDataType& getTaskFailed()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskFailed to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskFailed
           */
         virtual void setTaskFailed(const uci::type::TaskFailedTriggerDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskMoved.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskMoved.
           */
         virtual const uci::type::TaskTriggerDataType& getTaskMoved() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskMoved.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskMoved.
           */
         virtual uci::type::TaskTriggerDataType& getTaskMoved()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskMoved to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskMoved
           */
         virtual void setTaskMoved(const uci::type::TaskTriggerDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TaskTiming.
           *
           * @return The value of the string data type identified by TaskTiming.
           */
         virtual const uci::type::EmptyType& getTaskTiming() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TaskTiming.
           *
           * @return The value of the string data type identified by TaskTiming.
           */
         virtual uci::type::EmptyType& getTaskTiming()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TaskTiming to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setTaskTiming(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TaskTiming to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTaskTiming(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TaskTiming to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTaskTiming(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ZoneViolation.
           *
           * @return The acecssor that provides access to the complex content that is identified by ZoneViolation.
           */
         virtual const uci::type::ZoneViolationTriggerDataType& getZoneViolation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ZoneViolation.
           *
           * @return The acecssor that provides access to the complex content that is identified by ZoneViolation.
           */
         virtual uci::type::ZoneViolationTriggerDataType& getZoneViolation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ZoneViolation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ZoneViolation
           */
         virtual void setZoneViolation(const uci::type::ZoneViolationTriggerDataType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionPlanningByCaseTriggerDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionPlanningByCaseTriggerDataType to copy from
           */
         MissionPlanningByCaseTriggerDataType(const MissionPlanningByCaseTriggerDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionPlanningByCaseTriggerDataType to the contents of the
           * MissionPlanningByCaseTriggerDataType on the right hand side (rhs) of the assignment
           * operator.MissionPlanningByCaseTriggerDataType [only available to derived classes]
           *
           * @param rhs The MissionPlanningByCaseTriggerDataType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::MissionPlanningByCaseTriggerDataType
           * @return A constant reference to this MissionPlanningByCaseTriggerDataType.
           */
         const MissionPlanningByCaseTriggerDataType& operator=(const MissionPlanningByCaseTriggerDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionPlanningByCaseTriggerDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionPlanningByCaseTriggerDataType_h

