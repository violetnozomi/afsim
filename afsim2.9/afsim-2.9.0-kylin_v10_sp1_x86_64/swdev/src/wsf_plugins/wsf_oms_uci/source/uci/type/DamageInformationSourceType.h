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
#ifndef Uci__Type__DamageInformationSourceType_h
#define Uci__Type__DamageInformationSourceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductMetadataID_Type_h)
# include "uci/type/ProductMetadataID_Type.h"
#endif

#if !defined(Uci__Type__DamageObservationSourceType_h)
# include "uci/type/DamageObservationSourceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DamageInformationSourceType sequence accessor class */
      class DamageInformationSourceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DamageInformationSourceType()
         { }

         /** Returns this accessor's type constant, i.e. DamageInformationSourceType
           *
           * @return This accessor's type constant, i.e. DamageInformationSourceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::damageInformationSourceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DamageInformationSourceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates products used to make the report. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ProductMetadataID_Type, uci::type::accessorType::productMetadataID_Type> ProductMetadataID;

         /** Indicates observations used to make the report which don't have a corresponding Product for review. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DamageObservationSourceType, uci::type::accessorType::damageObservationSourceType> Observation;

         /** Returns the bounded list that is identified by the ProductMetadataID.
           *
           * @return The bounded list identified by ProductMetadataID.
           */
         virtual const uci::type::DamageInformationSourceType::ProductMetadataID& getProductMetadataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductMetadataID.
           *
           * @return The bounded list identified by ProductMetadataID.
           */
         virtual uci::type::DamageInformationSourceType::ProductMetadataID& getProductMetadataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductMetadataID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductMetadataID(const uci::type::DamageInformationSourceType::ProductMetadataID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Observation.
           *
           * @return The bounded list identified by Observation.
           */
         virtual const uci::type::DamageInformationSourceType::Observation& getObservation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Observation.
           *
           * @return The bounded list identified by Observation.
           */
         virtual uci::type::DamageInformationSourceType::Observation& getObservation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Observation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setObservation(const uci::type::DamageInformationSourceType::Observation& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DamageInformationSourceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DamageInformationSourceType to copy from
           */
         DamageInformationSourceType(const DamageInformationSourceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DamageInformationSourceType to the contents of the
           * DamageInformationSourceType on the right hand side (rhs) of the assignment operator.DamageInformationSourceType [only
           * available to derived classes]
           *
           * @param rhs The DamageInformationSourceType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::DamageInformationSourceType
           * @return A constant reference to this DamageInformationSourceType.
           */
         const DamageInformationSourceType& operator=(const DamageInformationSourceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DamageInformationSourceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DamageInformationSourceType_h

