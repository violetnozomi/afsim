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
#ifndef Uci__Type__EntityManagementSwapType_h
#define Uci__Type__EntityManagementSwapType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__SwapEnum_h)
# include "uci/type/SwapEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntityManagementSwapType sequence accessor class */
      class EntityManagementSwapType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntityManagementSwapType()
         { }

         /** Returns this accessor's type constant, i.e. EntityManagementSwapType
           *
           * @return This accessor's type constant, i.e. EntityManagementSwapType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityManagementSwapType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityManagementSwapType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FirstEntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FirstEntityID.
           */
         virtual const uci::type::EntityID_Type& getFirstEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FirstEntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FirstEntityID.
           */
         virtual uci::type::EntityID_Type& getFirstEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FirstEntityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FirstEntityID
           */
         virtual void setFirstEntityID(const uci::type::EntityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SecondEntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecondEntityID.
           */
         virtual const uci::type::EntityID_Type& getSecondEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SecondEntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecondEntityID.
           */
         virtual uci::type::EntityID_Type& getSecondEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SecondEntityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SecondEntityID
           */
         virtual void setSecondEntityID(const uci::type::EntityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SwapCharacteristics.
           *
           * @return The value of the enumeration identified by SwapCharacteristics.
           */
         virtual const uci::type::SwapEnum& getSwapCharacteristics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SwapCharacteristics.
           *
           * @return The value of the enumeration identified by SwapCharacteristics.
           */
         virtual uci::type::SwapEnum& getSwapCharacteristics()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SwapCharacteristics.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSwapCharacteristics(const uci::type::SwapEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SwapCharacteristics.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSwapCharacteristics(uci::type::SwapEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield SwapCharacteristicsis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSwapCharacteristics() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSwapCharacteristics will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::swapEnum This Accessor's accessor type
           */
         virtual void enableSwapCharacteristics(uci::base::accessorType::AccessorType type = uci::type::accessorType::swapEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSwapCharacteristics()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityManagementSwapType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityManagementSwapType to copy from
           */
         EntityManagementSwapType(const EntityManagementSwapType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityManagementSwapType to the contents of the
           * EntityManagementSwapType on the right hand side (rhs) of the assignment operator.EntityManagementSwapType [only
           * available to derived classes]
           *
           * @param rhs The EntityManagementSwapType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EntityManagementSwapType
           * @return A constant reference to this EntityManagementSwapType.
           */
         const EntityManagementSwapType& operator=(const EntityManagementSwapType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityManagementSwapType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityManagementSwapType_h

