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
#ifndef Uci__Type__CryptoSlotType_h
#define Uci__Type__CryptoSlotType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CryptoSlotID_Type_h)
# include "uci/type/CryptoSlotID_Type.h"
#endif

#if !defined(Uci__Type__CryptoKeyStatusEnum_h)
# include "uci/type/CryptoKeyStatusEnum.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CryptoSlotType sequence accessor class */
      class CryptoSlotType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CryptoSlotType()
         { }

         /** Returns this accessor's type constant, i.e. CryptoSlotType
           *
           * @return This accessor's type constant, i.e. CryptoSlotType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cryptoSlotType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CryptoSlotType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SlotID.
           *
           * @return The value of the string data type identified by SlotID.
           */
         virtual const uci::type::CryptoSlotID_Type& getSlotID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SlotID.
           *
           * @return The value of the string data type identified by SlotID.
           */
         virtual uci::type::CryptoSlotID_Type& getSlotID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SlotID to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSlotID(const uci::type::CryptoSlotID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SlotID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSlotID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SlotID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSlotID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SlotState.
           *
           * @return The value of the enumeration identified by SlotState.
           */
         virtual const uci::type::CryptoKeyStatusEnum& getSlotState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SlotState.
           *
           * @return The value of the enumeration identified by SlotState.
           */
         virtual uci::type::CryptoKeyStatusEnum& getSlotState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SlotState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSlotState(const uci::type::CryptoKeyStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SlotState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSlotState(uci::type::CryptoKeyStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the KeyID.
           *
           * @return The acecssor that provides access to the complex content that is identified by KeyID.
           */
         virtual const uci::type::ForeignKeyType& getKeyID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the KeyID.
           *
           * @return The acecssor that provides access to the complex content that is identified by KeyID.
           */
         virtual uci::type::ForeignKeyType& getKeyID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the KeyID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by KeyID
           */
         virtual void setKeyID(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by KeyID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by KeyID is emabled or not
           */
         virtual bool hasKeyID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by KeyID
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableKeyID(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by KeyID */
         virtual void clearKeyID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the NextSlotID.
           *
           * @return The value of the string data type identified by NextSlotID.
           */
         virtual const uci::type::CryptoSlotID_Type& getNextSlotID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the NextSlotID.
           *
           * @return The value of the string data type identified by NextSlotID.
           */
         virtual uci::type::CryptoSlotID_Type& getNextSlotID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the NextSlotID to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setNextSlotID(const uci::type::CryptoSlotID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the NextSlotID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setNextSlotID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the NextSlotID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setNextSlotID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NextSlotID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NextSlotID is emabled or not
           */
         virtual bool hasNextSlotID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NextSlotID
           *
           * @param type = uci::type::accessorType::visibleString32Type This Accessor's accessor type
           */
         virtual void enableNextSlotID(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString32Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NextSlotID */
         virtual void clearNextSlotID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CryptoSlotType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CryptoSlotType to copy from
           */
         CryptoSlotType(const CryptoSlotType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CryptoSlotType to the contents of the CryptoSlotType on the right
           * hand side (rhs) of the assignment operator.CryptoSlotType [only available to derived classes]
           *
           * @param rhs The CryptoSlotType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::CryptoSlotType
           * @return A constant reference to this CryptoSlotType.
           */
         const CryptoSlotType& operator=(const CryptoSlotType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CryptoSlotType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CryptoSlotType_h

