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
#ifndef Uci__Type__EntityManagementClearIdentityType_h
#define Uci__Type__EntityManagementClearIdentityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ClearIdentityValuesEnum_h)
# include "uci/type/ClearIdentityValuesEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates a request to clear part of the identity of an Entity. */
      class EntityManagementClearIdentityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntityManagementClearIdentityType()
         { }

         /** Returns this accessor's type constant, i.e. EntityManagementClearIdentityType
           *
           * @return This accessor's type constant, i.e. EntityManagementClearIdentityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityManagementClearIdentityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityManagementClearIdentityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** A list of specific identity value(s) to clear on an Entity. The associated identity field will be reset to the
           * initial state and refilled by current sensor data. [Maximum occurrences: 10]
           */
         typedef uci::base::BoundedList<uci::type::ClearIdentityValuesEnum, uci::type::accessorType::clearIdentityValuesEnum> ClearValue;

         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual const uci::type::EntityID_Type& getEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual uci::type::EntityID_Type& getEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EntityID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EntityID
           */
         virtual void setEntityID(const uci::type::EntityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ClearValue.
           *
           * @return The bounded list identified by ClearValue.
           */
         virtual const uci::type::EntityManagementClearIdentityType::ClearValue& getClearValue() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ClearValue.
           *
           * @return The bounded list identified by ClearValue.
           */
         virtual uci::type::EntityManagementClearIdentityType::ClearValue& getClearValue()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ClearValue.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setClearValue(const uci::type::EntityManagementClearIdentityType::ClearValue& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityManagementClearIdentityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityManagementClearIdentityType to copy from
           */
         EntityManagementClearIdentityType(const EntityManagementClearIdentityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityManagementClearIdentityType to the contents of the
           * EntityManagementClearIdentityType on the right hand side (rhs) of the assignment
           * operator.EntityManagementClearIdentityType [only available to derived classes]
           *
           * @param rhs The EntityManagementClearIdentityType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::EntityManagementClearIdentityType
           * @return A constant reference to this EntityManagementClearIdentityType.
           */
         const EntityManagementClearIdentityType& operator=(const EntityManagementClearIdentityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityManagementClearIdentityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityManagementClearIdentityType_h

