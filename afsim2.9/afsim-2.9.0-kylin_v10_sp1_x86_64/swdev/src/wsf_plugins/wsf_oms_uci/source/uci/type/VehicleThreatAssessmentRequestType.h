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
#ifndef Uci__Type__VehicleThreatAssessmentRequestType_h
#define Uci__Type__VehicleThreatAssessmentRequestType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AssessmentSuppressionEnum_h)
# include "uci/type/AssessmentSuppressionEnum.h"
#endif

#if !defined(Uci__Type__ThreatCapabilityEnum_h)
# include "uci/type/ThreatCapabilityEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemPositionSpecificationType_h)
# include "uci/type/SystemPositionSpecificationType.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__DetectionEnvelopeParametersType_h)
# include "uci/type/DetectionEnvelopeParametersType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The inputs used in generating and/or requesting a Vehicle Threat Assessment. */
      class VehicleThreatAssessmentRequestType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~VehicleThreatAssessmentRequestType()
         { }

         /** Returns this accessor's type constant, i.e. VehicleThreatAssessmentRequestType
           *
           * @return This accessor's type constant, i.e. VehicleThreatAssessmentRequestType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::vehicleThreatAssessmentRequestType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const VehicleThreatAssessmentRequestType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element identifies the vehicle that should be used for threat assessment. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemPositionSpecificationType, uci::type::accessorType::systemPositionSpecificationType> SystemToAssess;

         /** This element identifies other vehicles that are to be considered for the assessment, but not assessed (ie. include a
           * vehicle with a supporting capability, but there is no need to assess the supporting vehicle, just the vehicle being
           * supported). [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemPositionSpecificationType, uci::type::accessorType::systemPositionSpecificationType> SupportingSystem;

         /** This element is used if the threat assessment should be generated in regards to only certain threats. If this element
           * is not specified then all entities are considered. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntityID_Type, uci::type::accessorType::entityID_Type> EntityID;

         /** Returns the value of the enumeration that is identified by the AssessmentSuppression.
           *
           * @return The value of the enumeration identified by AssessmentSuppression.
           */
         virtual const uci::type::AssessmentSuppressionEnum& getAssessmentSuppression() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AssessmentSuppression.
           *
           * @return The value of the enumeration identified by AssessmentSuppression.
           */
         virtual uci::type::AssessmentSuppressionEnum& getAssessmentSuppression()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AssessmentSuppression.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAssessmentSuppression(const uci::type::AssessmentSuppressionEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AssessmentSuppression.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAssessmentSuppression(uci::type::AssessmentSuppressionEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


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


         /** Returns the bounded list that is identified by the SystemToAssess.
           *
           * @return The bounded list identified by SystemToAssess.
           */
         virtual const uci::type::VehicleThreatAssessmentRequestType::SystemToAssess& getSystemToAssess() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemToAssess.
           *
           * @return The bounded list identified by SystemToAssess.
           */
         virtual uci::type::VehicleThreatAssessmentRequestType::SystemToAssess& getSystemToAssess()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemToAssess.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemToAssess(const uci::type::VehicleThreatAssessmentRequestType::SystemToAssess& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportingSystem.
           *
           * @return The bounded list identified by SupportingSystem.
           */
         virtual const uci::type::VehicleThreatAssessmentRequestType::SupportingSystem& getSupportingSystem() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportingSystem.
           *
           * @return The bounded list identified by SupportingSystem.
           */
         virtual uci::type::VehicleThreatAssessmentRequestType::SupportingSystem& getSupportingSystem()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportingSystem.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportingSystem(const uci::type::VehicleThreatAssessmentRequestType::SupportingSystem& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EntityID.
           *
           * @return The bounded list identified by EntityID.
           */
         virtual const uci::type::VehicleThreatAssessmentRequestType::EntityID& getEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EntityID.
           *
           * @return The bounded list identified by EntityID.
           */
         virtual uci::type::VehicleThreatAssessmentRequestType::EntityID& getEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EntityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEntityID(const uci::type::VehicleThreatAssessmentRequestType::EntityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GenerateDetectionEnvelope.
           *
           * @return The acecssor that provides access to the complex content that is identified by GenerateDetectionEnvelope.
           */
         virtual const uci::type::DetectionEnvelopeParametersType& getGenerateDetectionEnvelope() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GenerateDetectionEnvelope.
           *
           * @return The acecssor that provides access to the complex content that is identified by GenerateDetectionEnvelope.
           */
         virtual uci::type::DetectionEnvelopeParametersType& getGenerateDetectionEnvelope()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the GenerateDetectionEnvelope to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by GenerateDetectionEnvelope
           */
         virtual void setGenerateDetectionEnvelope(const uci::type::DetectionEnvelopeParametersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GenerateDetectionEnvelope exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GenerateDetectionEnvelope is emabled or not
           */
         virtual bool hasGenerateDetectionEnvelope() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GenerateDetectionEnvelope
           *
           * @param type = uci::type::accessorType::detectionEnvelopeParametersType This Accessor's accessor type
           */
         virtual void enableGenerateDetectionEnvelope(uci::base::accessorType::AccessorType type = uci::type::accessorType::detectionEnvelopeParametersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GenerateDetectionEnvelope */
         virtual void clearGenerateDetectionEnvelope()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         VehicleThreatAssessmentRequestType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The VehicleThreatAssessmentRequestType to copy from
           */
         VehicleThreatAssessmentRequestType(const VehicleThreatAssessmentRequestType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this VehicleThreatAssessmentRequestType to the contents of the
           * VehicleThreatAssessmentRequestType on the right hand side (rhs) of the assignment
           * operator.VehicleThreatAssessmentRequestType [only available to derived classes]
           *
           * @param rhs The VehicleThreatAssessmentRequestType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::VehicleThreatAssessmentRequestType
           * @return A constant reference to this VehicleThreatAssessmentRequestType.
           */
         const VehicleThreatAssessmentRequestType& operator=(const VehicleThreatAssessmentRequestType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // VehicleThreatAssessmentRequestType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__VehicleThreatAssessmentRequestType_h

