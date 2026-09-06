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
#ifndef Uci__Type__DamageEstimateDetailsType_h
#define Uci__Type__DamageEstimateDetailsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ObjectInstanceDamageEstimateType_h)
# include "uci/type/ObjectInstanceDamageEstimateType.h"
#endif

#if !defined(Uci__Type__ObjectTypeDamageEstimateType_h)
# include "uci/type/ObjectTypeDamageEstimateType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DamageEstimateDetailsType sequence accessor class */
      class DamageEstimateDetailsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DamageEstimateDetailsType()
         { }

         /** Returns this accessor's type constant, i.e. DamageEstimateDetailsType
           *
           * @return This accessor's type constant, i.e. DamageEstimateDetailsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::damageEstimateDetailsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DamageEstimateDetailsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates damage details summarized by object instance. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ObjectInstanceDamageEstimateType, uci::type::accessorType::objectInstanceDamageEstimateType> ByObjectInstance;

         /** Indicates damage details summarized by object type(s). Allows damage to be summarized for similar objects with
           * probability organized into zones/ranges. Allows an estimated damage template to be created that can be used to
           * re-evaluate the sibling ByObject estimate at a later time when the battlespace changes. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ObjectTypeDamageEstimateType, uci::type::accessorType::objectTypeDamageEstimateType> ByObjectType;

         /** Returns the bounded list that is identified by the ByObjectInstance.
           *
           * @return The bounded list identified by ByObjectInstance.
           */
         virtual const uci::type::DamageEstimateDetailsType::ByObjectInstance& getByObjectInstance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ByObjectInstance.
           *
           * @return The bounded list identified by ByObjectInstance.
           */
         virtual uci::type::DamageEstimateDetailsType::ByObjectInstance& getByObjectInstance()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ByObjectInstance.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setByObjectInstance(const uci::type::DamageEstimateDetailsType::ByObjectInstance& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ByObjectType.
           *
           * @return The bounded list identified by ByObjectType.
           */
         virtual const uci::type::DamageEstimateDetailsType::ByObjectType& getByObjectType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ByObjectType.
           *
           * @return The bounded list identified by ByObjectType.
           */
         virtual uci::type::DamageEstimateDetailsType::ByObjectType& getByObjectType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ByObjectType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setByObjectType(const uci::type::DamageEstimateDetailsType::ByObjectType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DamageEstimateDetailsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DamageEstimateDetailsType to copy from
           */
         DamageEstimateDetailsType(const DamageEstimateDetailsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DamageEstimateDetailsType to the contents of the
           * DamageEstimateDetailsType on the right hand side (rhs) of the assignment operator.DamageEstimateDetailsType [only
           * available to derived classes]
           *
           * @param rhs The DamageEstimateDetailsType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::DamageEstimateDetailsType
           * @return A constant reference to this DamageEstimateDetailsType.
           */
         const DamageEstimateDetailsType& operator=(const DamageEstimateDetailsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DamageEstimateDetailsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DamageEstimateDetailsType_h

