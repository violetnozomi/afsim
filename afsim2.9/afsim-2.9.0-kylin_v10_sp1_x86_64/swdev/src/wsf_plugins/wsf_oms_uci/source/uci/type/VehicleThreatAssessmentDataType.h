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
#ifndef Uci__Type__VehicleThreatAssessmentDataType_h
#define Uci__Type__VehicleThreatAssessmentDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PredictedThreatAssessmentType_h)
# include "uci/type/PredictedThreatAssessmentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The results of an assessment that determines vulnerability of a system at a given point. */
      class VehicleThreatAssessmentDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~VehicleThreatAssessmentDataType()
         { }

         /** Returns this accessor's type constant, i.e. VehicleThreatAssessmentDataType
           *
           * @return This accessor's type constant, i.e. VehicleThreatAssessmentDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::vehicleThreatAssessmentDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const VehicleThreatAssessmentDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Optional array of detection probabilities for each track (if any) with a detection probability greater than 0 for the
           * vehicle at the position specified within the AssessmentRequest message. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PredictedThreatAssessmentType, uci::type::accessorType::predictedThreatAssessmentType> PredictedThreatAssessment;

         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PredictedThreatAssessment.
           *
           * @return The bounded list identified by PredictedThreatAssessment.
           */
         virtual const uci::type::VehicleThreatAssessmentDataType::PredictedThreatAssessment& getPredictedThreatAssessment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PredictedThreatAssessment.
           *
           * @return The bounded list identified by PredictedThreatAssessment.
           */
         virtual uci::type::VehicleThreatAssessmentDataType::PredictedThreatAssessment& getPredictedThreatAssessment()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PredictedThreatAssessment.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPredictedThreatAssessment(const uci::type::VehicleThreatAssessmentDataType::PredictedThreatAssessment& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         VehicleThreatAssessmentDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The VehicleThreatAssessmentDataType to copy from
           */
         VehicleThreatAssessmentDataType(const VehicleThreatAssessmentDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this VehicleThreatAssessmentDataType to the contents of the
           * VehicleThreatAssessmentDataType on the right hand side (rhs) of the assignment
           * operator.VehicleThreatAssessmentDataType [only available to derived classes]
           *
           * @param rhs The VehicleThreatAssessmentDataType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::VehicleThreatAssessmentDataType
           * @return A constant reference to this VehicleThreatAssessmentDataType.
           */
         const VehicleThreatAssessmentDataType& operator=(const VehicleThreatAssessmentDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // VehicleThreatAssessmentDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__VehicleThreatAssessmentDataType_h

