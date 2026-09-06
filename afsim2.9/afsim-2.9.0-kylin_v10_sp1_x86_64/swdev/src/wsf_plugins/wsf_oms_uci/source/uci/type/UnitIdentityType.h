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
#ifndef Uci__Type__UnitIdentityType_h
#define Uci__Type__UnitIdentityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__UnitID_Type_h)
# include "uci/type/UnitID_Type.h"
#endif

#if !defined(Uci__Type__UnitIdentifierType_h)
# include "uci/type/UnitIdentifierType.h"
#endif

#if !defined(Uci__Type__UnitNameType_h)
# include "uci/type/UnitNameType.h"
#endif

#if !defined(Uci__Type__String20Type_h)
# include "uci/type/String20Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__OrderOfBattleEnum_h)
# include "uci/type/OrderOfBattleEnum.h"
#endif

#if !defined(Uci__Type__NameSpecialCharacterRestrictionType_h)
# include "uci/type/NameSpecialCharacterRestrictionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the UnitIdentityType sequence accessor class */
      class UnitIdentityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~UnitIdentityType()
         { }

         /** Returns this accessor's type constant, i.e. UnitIdentityType
           *
           * @return This accessor's type constant, i.e. UnitIdentityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::unitIdentityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const UnitIdentityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the type of Order-of-Battle to which a unit or equipment belongs. [Maximum occurrences: 11] */
         typedef uci::base::BoundedList<uci::type::OrderOfBattleEnum, uci::type::accessorType::orderOfBattleEnum> OB_Type;

         /** Returns the accessor that provides access to the complex content that is identified by the UnitUUID.
           *
           * @return The acecssor that provides access to the complex content that is identified by UnitUUID.
           */
         virtual const uci::type::UnitID_Type& getUnitUUID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the UnitUUID.
           *
           * @return The acecssor that provides access to the complex content that is identified by UnitUUID.
           */
         virtual uci::type::UnitID_Type& getUnitUUID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the UnitUUID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by UnitUUID
           */
         virtual void setUnitUUID(const uci::type::UnitID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ID.
           *
           * @return The value of the string data type identified by ID.
           */
         virtual const uci::type::UnitIdentifierType& getID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ID.
           *
           * @return The value of the string data type identified by ID.
           */
         virtual uci::type::UnitIdentifierType& getID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ID to the string accessed by the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setID(const uci::type::UnitIdentifierType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Name.
           *
           * @return The value of the string data type identified by Name.
           */
         virtual const uci::type::UnitNameType& getName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Name.
           *
           * @return The value of the string data type identified by Name.
           */
         virtual uci::type::UnitNameType& getName()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Name to the string accessed by the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setName(const uci::type::UnitNameType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Name to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setName(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Name to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setName(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SurrogateKey.
           *
           * @return The value of the string data type identified by SurrogateKey.
           */
         virtual const uci::type::String20Type& getSurrogateKey() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SurrogateKey.
           *
           * @return The value of the string data type identified by SurrogateKey.
           */
         virtual uci::type::String20Type& getSurrogateKey()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SurrogateKey to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSurrogateKey(const uci::type::String20Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SurrogateKey to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSurrogateKey(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SurrogateKey to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSurrogateKey(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OB_Type.
           *
           * @return The bounded list identified by OB_Type.
           */
         virtual const uci::type::UnitIdentityType::OB_Type& getOB_Type() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OB_Type.
           *
           * @return The bounded list identified by OB_Type.
           */
         virtual uci::type::UnitIdentityType::OB_Type& getOB_Type()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the OB_Type.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOB_Type(const uci::type::UnitIdentityType::OB_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SymbolCode.
           *
           * @return The value of the string data type identified by SymbolCode.
           */
         virtual const uci::type::NameSpecialCharacterRestrictionType& getSymbolCode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SymbolCode.
           *
           * @return The value of the string data type identified by SymbolCode.
           */
         virtual uci::type::NameSpecialCharacterRestrictionType& getSymbolCode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SymbolCode to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSymbolCode(const uci::type::NameSpecialCharacterRestrictionType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SymbolCode to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSymbolCode(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SymbolCode to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSymbolCode(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SymbolCode exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SymbolCode is emabled or not
           */
         virtual bool hasSymbolCode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SymbolCode
           *
           * @param type = uci::type::accessorType::nameSpecialCharacterRestrictionType This Accessor's accessor type
           */
         virtual void enableSymbolCode(uci::base::accessorType::AccessorType type = uci::type::accessorType::nameSpecialCharacterRestrictionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SymbolCode */
         virtual void clearSymbolCode()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         UnitIdentityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The UnitIdentityType to copy from
           */
         UnitIdentityType(const UnitIdentityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this UnitIdentityType to the contents of the UnitIdentityType on the
           * right hand side (rhs) of the assignment operator.UnitIdentityType [only available to derived classes]
           *
           * @param rhs The UnitIdentityType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::UnitIdentityType
           * @return A constant reference to this UnitIdentityType.
           */
         const UnitIdentityType& operator=(const UnitIdentityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // UnitIdentityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__UnitIdentityType_h

