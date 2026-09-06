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
#ifndef Uci__Type__ThreatAssessmentRequestType_h
#define Uci__Type__ThreatAssessmentRequestType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ThreatCapabilityEnum_h)
# include "uci/type/ThreatCapabilityEnum.h"
#endif

#if !defined(Uci__Type__AssessmentSuppressionEnum_h)
# include "uci/type/AssessmentSuppressionEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Type__SystemMissionPlanSpecificationType_h)
# include "uci/type/SystemMissionPlanSpecificationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The inputs used in generating and/or requesting a Route Threat Assessment. */
      class ThreatAssessmentRequestType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ThreatAssessmentRequestType()
         { }

         /** Returns this accessor's type constant, i.e. ThreatAssessmentRequestType
           *
           * @return This accessor's type constant, i.e. ThreatAssessmentRequestType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::threatAssessmentRequestType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ThreatAssessmentRequestType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element indicates a specific threat Entity to assess. If omitted, all relevant threat Entities are assessed. If
           * any are given, only they are assessed. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntityID_Type, uci::type::accessorType::entityID_Type> ThreatEntityID;

         /** Indicates a System or Systems to assess given their flight paths (MissionPlan) and available capabilities. [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemMissionPlanSpecificationType, uci::type::accessorType::systemMissionPlanSpecificationType> SystemToAssess;

         /** Indicates additional systems to be considered for the assessment, but are not assessed. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemMissionPlanSpecificationType, uci::type::accessorType::systemMissionPlanSpecificationType> SupportingSystem;

         /** Returns the value of the enumeration that is identified by the ThreatCapability.
           *
           * @return The value of the enumeration identified by ThreatCapability.
           */
         virtual const uci::type::ThreatCapabilityEnum& getThreatCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ThreatCapability.
           *
           * @return The value of the enumeration identified by ThreatCapability.
           */
         virtual uci::type::ThreatCapabilityEnum& getThreatCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ThreatCapability.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setThreatCapability(const uci::type::ThreatCapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ThreatCapability.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setThreatCapability(uci::type::ThreatCapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ThreatSuppression.
           *
           * @return The value of the enumeration identified by ThreatSuppression.
           */
         virtual const uci::type::AssessmentSuppressionEnum& getThreatSuppression() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ThreatSuppression.
           *
           * @return The value of the enumeration identified by ThreatSuppression.
           */
         virtual uci::type::AssessmentSuppressionEnum& getThreatSuppression()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ThreatSuppression.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setThreatSuppression(const uci::type::AssessmentSuppressionEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ThreatSuppression.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setThreatSuppression(uci::type::AssessmentSuppressionEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ThreatEntityID.
           *
           * @return The bounded list identified by ThreatEntityID.
           */
         virtual const uci::type::ThreatAssessmentRequestType::ThreatEntityID& getThreatEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ThreatEntityID.
           *
           * @return The bounded list identified by ThreatEntityID.
           */
         virtual uci::type::ThreatAssessmentRequestType::ThreatEntityID& getThreatEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ThreatEntityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setThreatEntityID(const uci::type::ThreatAssessmentRequestType::ThreatEntityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AssessmentInterval.
           *
           * @return The value of the simple primitive data type identified by AssessmentInterval.
           */
         virtual uci::type::DurationTypeValue getAssessmentInterval() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AssessmentInterval.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAssessmentInterval(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AssessmentInterval exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AssessmentInterval is emabled or not
           */
         virtual bool hasAssessmentInterval() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AssessmentInterval
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableAssessmentInterval(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AssessmentInterval */
         virtual void clearAssessmentInterval()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemToAssess.
           *
           * @return The bounded list identified by SystemToAssess.
           */
         virtual const uci::type::ThreatAssessmentRequestType::SystemToAssess& getSystemToAssess() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemToAssess.
           *
           * @return The bounded list identified by SystemToAssess.
           */
         virtual uci::type::ThreatAssessmentRequestType::SystemToAssess& getSystemToAssess()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemToAssess.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemToAssess(const uci::type::ThreatAssessmentRequestType::SystemToAssess& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportingSystem.
           *
           * @return The bounded list identified by SupportingSystem.
           */
         virtual const uci::type::ThreatAssessmentRequestType::SupportingSystem& getSupportingSystem() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportingSystem.
           *
           * @return The bounded list identified by SupportingSystem.
           */
         virtual uci::type::ThreatAssessmentRequestType::SupportingSystem& getSupportingSystem()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportingSystem.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportingSystem(const uci::type::ThreatAssessmentRequestType::SupportingSystem& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ThreatAssessmentRequestType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ThreatAssessmentRequestType to copy from
           */
         ThreatAssessmentRequestType(const ThreatAssessmentRequestType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ThreatAssessmentRequestType to the contents of the
           * ThreatAssessmentRequestType on the right hand side (rhs) of the assignment operator.ThreatAssessmentRequestType [only
           * available to derived classes]
           *
           * @param rhs The ThreatAssessmentRequestType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ThreatAssessmentRequestType
           * @return A constant reference to this ThreatAssessmentRequestType.
           */
         const ThreatAssessmentRequestType& operator=(const ThreatAssessmentRequestType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ThreatAssessmentRequestType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ThreatAssessmentRequestType_h

