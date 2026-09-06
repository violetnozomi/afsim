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
#ifndef Uci__Type__AssessmentType_h
#define Uci__Type__AssessmentType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommPointingPlanAssessmentType_h)
# include "uci/type/CommPointingPlanAssessmentType.h"
#endif

#if !defined(Uci__Type__CapabilityUtilizationAssessmentType_h)
# include "uci/type/CapabilityUtilizationAssessmentType.h"
#endif

#if !defined(Uci__Type__RouteDeconflictionAssessmentType_h)
# include "uci/type/RouteDeconflictionAssessmentType.h"
#endif

#if !defined(Uci__Type__RouteVulnerabilityMetricsAssessmentType_h)
# include "uci/type/RouteVulnerabilityMetricsAssessmentType.h"
#endif

#if !defined(Uci__Type__RouteThreatAssessmentType_h)
# include "uci/type/RouteThreatAssessmentType.h"
#endif

#if !defined(Uci__Type__RouteValidationAssessmentType_h)
# include "uci/type/RouteValidationAssessmentType.h"
#endif

#if !defined(Uci__Type__TargetMobilityAssessmentType_h)
# include "uci/type/TargetMobilityAssessmentType.h"
#endif

#if !defined(Uci__Type__TaskAllocationValidationAssessmentType_h)
# include "uci/type/TaskAllocationValidationAssessmentType.h"
#endif

#if !defined(Uci__Type__VehicleThreatAssessmentType_h)
# include "uci/type/VehicleThreatAssessmentType.h"
#endif

#if !defined(Uci__Type__ThreatNominationAssessmentType_h)
# include "uci/type/ThreatNominationAssessmentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AssessmentType choice accessor class */
      class AssessmentType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AssessmentType()
         { }

         /** Returns this accessor's type constant, i.e. AssessmentType
           *
           * @return This accessor's type constant, i.e. AssessmentType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::assessmentType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AssessmentType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum AssessmentTypeChoice {
            ASSESSMENTTYPE_CHOICE_NONE,
            ASSESSMENTTYPE_CHOICE_COMMPOINTINGPLAN,
            ASSESSMENTTYPE_CHOICE_CAPABILITYUTILIZATION,
            ASSESSMENTTYPE_CHOICE_ROUTEDECONFLICTION,
            ASSESSMENTTYPE_CHOICE_ROUTEVULNERABILITYMETRICS,
            ASSESSMENTTYPE_CHOICE_ROUTETHREATASSESSMENT,
            ASSESSMENTTYPE_CHOICE_ROUTEVALIDATION,
            ASSESSMENTTYPE_CHOICE_TARGETMOBILITY,
            ASSESSMENTTYPE_CHOICE_TASKALLOCATIONVALIDATION,
            ASSESSMENTTYPE_CHOICE_VEHICLETHREATASSESSMENT,
            ASSESSMENTTYPE_CHOICE_THREATNOMINATIONASSESSMENT
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual AssessmentTypeChoice getAssessmentTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getAssessmentTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the access
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
         virtual void setAssessmentTypeChoiceOrdinal(AssessmentTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommPointingPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommPointingPlan.
           */
         virtual const uci::type::CommPointingPlanAssessmentType& getCommPointingPlan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommPointingPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommPointingPlan.
           */
         virtual uci::type::CommPointingPlanAssessmentType& getCommPointingPlan()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommPointingPlan to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommPointingPlan
           */
         virtual void setCommPointingPlan(const uci::type::CommPointingPlanAssessmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityUtilization.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityUtilization.
           */
         virtual const uci::type::CapabilityUtilizationAssessmentType& getCapabilityUtilization() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityUtilization.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityUtilization.
           */
         virtual uci::type::CapabilityUtilizationAssessmentType& getCapabilityUtilization()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityUtilization to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityUtilization
           */
         virtual void setCapabilityUtilization(const uci::type::CapabilityUtilizationAssessmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteDeconfliction.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteDeconfliction.
           */
         virtual const uci::type::RouteDeconflictionAssessmentType& getRouteDeconfliction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteDeconfliction.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteDeconfliction.
           */
         virtual uci::type::RouteDeconflictionAssessmentType& getRouteDeconfliction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RouteDeconfliction to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RouteDeconfliction
           */
         virtual void setRouteDeconfliction(const uci::type::RouteDeconflictionAssessmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteVulnerabilityMetrics.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteVulnerabilityMetrics.
           */
         virtual const uci::type::RouteVulnerabilityMetricsAssessmentType& getRouteVulnerabilityMetrics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteVulnerabilityMetrics.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteVulnerabilityMetrics.
           */
         virtual uci::type::RouteVulnerabilityMetricsAssessmentType& getRouteVulnerabilityMetrics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RouteVulnerabilityMetrics to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RouteVulnerabilityMetrics
           */
         virtual void setRouteVulnerabilityMetrics(const uci::type::RouteVulnerabilityMetricsAssessmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteThreatAssessment.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteThreatAssessment.
           */
         virtual const uci::type::RouteThreatAssessmentType& getRouteThreatAssessment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteThreatAssessment.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteThreatAssessment.
           */
         virtual uci::type::RouteThreatAssessmentType& getRouteThreatAssessment()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RouteThreatAssessment to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RouteThreatAssessment
           */
         virtual void setRouteThreatAssessment(const uci::type::RouteThreatAssessmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteValidation.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteValidation.
           */
         virtual const uci::type::RouteValidationAssessmentType& getRouteValidation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteValidation.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteValidation.
           */
         virtual uci::type::RouteValidationAssessmentType& getRouteValidation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RouteValidation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RouteValidation
           */
         virtual void setRouteValidation(const uci::type::RouteValidationAssessmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TargetMobility.
           *
           * @return The acecssor that provides access to the complex content that is identified by TargetMobility.
           */
         virtual const uci::type::TargetMobilityAssessmentType& getTargetMobility() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TargetMobility.
           *
           * @return The acecssor that provides access to the complex content that is identified by TargetMobility.
           */
         virtual uci::type::TargetMobilityAssessmentType& getTargetMobility()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TargetMobility to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TargetMobility
           */
         virtual void setTargetMobility(const uci::type::TargetMobilityAssessmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskAllocationValidation.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskAllocationValidation.
           */
         virtual const uci::type::TaskAllocationValidationAssessmentType& getTaskAllocationValidation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskAllocationValidation.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskAllocationValidation.
           */
         virtual uci::type::TaskAllocationValidationAssessmentType& getTaskAllocationValidation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskAllocationValidation to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskAllocationValidation
           */
         virtual void setTaskAllocationValidation(const uci::type::TaskAllocationValidationAssessmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VehicleThreatAssessment.
           *
           * @return The acecssor that provides access to the complex content that is identified by VehicleThreatAssessment.
           */
         virtual const uci::type::VehicleThreatAssessmentType& getVehicleThreatAssessment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VehicleThreatAssessment.
           *
           * @return The acecssor that provides access to the complex content that is identified by VehicleThreatAssessment.
           */
         virtual uci::type::VehicleThreatAssessmentType& getVehicleThreatAssessment()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the VehicleThreatAssessment to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by VehicleThreatAssessment
           */
         virtual void setVehicleThreatAssessment(const uci::type::VehicleThreatAssessmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ThreatNominationAssessment.
           *
           * @return The acecssor that provides access to the complex content that is identified by ThreatNominationAssessment.
           */
         virtual const uci::type::ThreatNominationAssessmentType& getThreatNominationAssessment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ThreatNominationAssessment.
           *
           * @return The acecssor that provides access to the complex content that is identified by ThreatNominationAssessment.
           */
         virtual uci::type::ThreatNominationAssessmentType& getThreatNominationAssessment()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ThreatNominationAssessment to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ThreatNominationAssessment
           */
         virtual void setThreatNominationAssessment(const uci::type::ThreatNominationAssessmentType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AssessmentType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AssessmentType to copy from
           */
         AssessmentType(const AssessmentType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AssessmentType to the contents of the AssessmentType on the right
           * hand side (rhs) of the assignment operator.AssessmentType [only available to derived classes]
           *
           * @param rhs The AssessmentType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::AssessmentType
           * @return A constant reference to this AssessmentType.
           */
         const AssessmentType& operator=(const AssessmentType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AssessmentType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AssessmentType_h

