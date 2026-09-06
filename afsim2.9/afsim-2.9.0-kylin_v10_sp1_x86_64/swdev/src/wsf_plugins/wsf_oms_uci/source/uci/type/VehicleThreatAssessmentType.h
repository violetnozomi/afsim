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
#ifndef Uci__Type__VehicleThreatAssessmentType_h
#define Uci__Type__VehicleThreatAssessmentType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__VehicleThreatAssessmentRequestType_h)
# include "uci/type/VehicleThreatAssessmentRequestType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__VehicleThreatAssessmentDataType_h)
# include "uci/type/VehicleThreatAssessmentDataType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Base data structure containing both the inputs and outputs for a Vehicle Threat Assessment. */
      class VehicleThreatAssessmentType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~VehicleThreatAssessmentType()
         { }

         /** Returns this accessor's type constant, i.e. VehicleThreatAssessmentType
           *
           * @return This accessor's type constant, i.e. VehicleThreatAssessmentType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::vehicleThreatAssessmentType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const VehicleThreatAssessmentType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element defines the results of a vehicle threat assessment. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::VehicleThreatAssessmentDataType, uci::type::accessorType::vehicleThreatAssessmentDataType> Data;

         /** Returns the accessor that provides access to the complex content that is identified by the Parameters.
           *
           * @return The acecssor that provides access to the complex content that is identified by Parameters.
           */
         virtual const uci::type::VehicleThreatAssessmentRequestType& getParameters() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Parameters.
           *
           * @return The acecssor that provides access to the complex content that is identified by Parameters.
           */
         virtual uci::type::VehicleThreatAssessmentRequestType& getParameters()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Parameters to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Parameters
           */
         virtual void setParameters(const uci::type::VehicleThreatAssessmentRequestType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Parameters exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Parameters is emabled or not
           */
         virtual bool hasParameters() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Parameters
           *
           * @param type = uci::type::accessorType::vehicleThreatAssessmentRequestType This Accessor's accessor type
           */
         virtual void enableParameters(uci::base::accessorType::AccessorType type = uci::type::accessorType::vehicleThreatAssessmentRequestType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Parameters */
         virtual void clearParameters()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Data.
           *
           * @return The bounded list identified by Data.
           */
         virtual const uci::type::VehicleThreatAssessmentType::Data& getData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Data.
           *
           * @return The bounded list identified by Data.
           */
         virtual uci::type::VehicleThreatAssessmentType::Data& getData()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Data.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setData(const uci::type::VehicleThreatAssessmentType::Data& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         VehicleThreatAssessmentType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The VehicleThreatAssessmentType to copy from
           */
         VehicleThreatAssessmentType(const VehicleThreatAssessmentType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this VehicleThreatAssessmentType to the contents of the
           * VehicleThreatAssessmentType on the right hand side (rhs) of the assignment operator.VehicleThreatAssessmentType [only
           * available to derived classes]
           *
           * @param rhs The VehicleThreatAssessmentType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::VehicleThreatAssessmentType
           * @return A constant reference to this VehicleThreatAssessmentType.
           */
         const VehicleThreatAssessmentType& operator=(const VehicleThreatAssessmentType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // VehicleThreatAssessmentType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__VehicleThreatAssessmentType_h

