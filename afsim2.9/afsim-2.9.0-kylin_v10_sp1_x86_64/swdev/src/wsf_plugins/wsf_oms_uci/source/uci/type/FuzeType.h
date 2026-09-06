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
#ifndef Uci__Type__FuzeType_h
#define Uci__Type__FuzeType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__FuzeTriggerType_h)
# include "uci/type/FuzeTriggerType.h"
#endif

#if !defined(Uci__Type__WeaponFuzeModeEnum_h)
# include "uci/type/WeaponFuzeModeEnum.h"
#endif

#if !defined(Uci__Type__WeaponFuzePositionEnum_h)
# include "uci/type/WeaponFuzePositionEnum.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Weapon fuze mode and setting. See operational handbooks and TTP manuals for allowable fuze modes and settings. */
      class FuzeType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~FuzeType()
         { }

         /** Returns this accessor's type constant, i.e. FuzeType
           *
           * @return This accessor's type constant, i.e. FuzeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::fuzeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FuzeType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FuzeTrigger.
           *
           * @return The acecssor that provides access to the complex content that is identified by FuzeTrigger.
           */
         virtual const uci::type::FuzeTriggerType& getFuzeTrigger() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FuzeTrigger.
           *
           * @return The acecssor that provides access to the complex content that is identified by FuzeTrigger.
           */
         virtual uci::type::FuzeTriggerType& getFuzeTrigger()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FuzeTrigger to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FuzeTrigger
           */
         virtual void setFuzeTrigger(const uci::type::FuzeTriggerType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FuzeMode.
           *
           * @return The value of the enumeration identified by FuzeMode.
           */
         virtual const uci::type::WeaponFuzeModeEnum& getFuzeMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FuzeMode.
           *
           * @return The value of the enumeration identified by FuzeMode.
           */
         virtual uci::type::WeaponFuzeModeEnum& getFuzeMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FuzeMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFuzeMode(const uci::type::WeaponFuzeModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FuzeMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFuzeMode(uci::type::WeaponFuzeModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield FuzeModeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasFuzeMode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getFuzeMode will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::weaponFuzeModeEnum This Accessor's accessor type
           */
         virtual void enableFuzeMode(uci::base::accessorType::AccessorType type = uci::type::accessorType::weaponFuzeModeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearFuzeMode()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FuzePosition.
           *
           * @return The value of the enumeration identified by FuzePosition.
           */
         virtual const uci::type::WeaponFuzePositionEnum& getFuzePosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FuzePosition.
           *
           * @return The value of the enumeration identified by FuzePosition.
           */
         virtual uci::type::WeaponFuzePositionEnum& getFuzePosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FuzePosition.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFuzePosition(const uci::type::WeaponFuzePositionEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FuzePosition.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFuzePosition(uci::type::WeaponFuzePositionEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield FuzePositionis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasFuzePosition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getFuzePosition will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::weaponFuzePositionEnum This Accessor's accessor type
           */
         virtual void enableFuzePosition(uci::base::accessorType::AccessorType type = uci::type::accessorType::weaponFuzePositionEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearFuzePosition()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Mnemonic.
           *
           * @return The value of the string data type identified by Mnemonic.
           */
         virtual const uci::type::VisibleString1024Type& getMnemonic() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Mnemonic.
           *
           * @return The value of the string data type identified by Mnemonic.
           */
         virtual uci::type::VisibleString1024Type& getMnemonic()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Mnemonic to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMnemonic(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Mnemonic to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMnemonic(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Mnemonic to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMnemonic(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Mnemonic exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Mnemonic is emabled or not
           */
         virtual bool hasMnemonic() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Mnemonic
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableMnemonic(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Mnemonic */
         virtual void clearMnemonic()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FuzeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FuzeType to copy from
           */
         FuzeType(const FuzeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FuzeType to the contents of the FuzeType on the right hand side
           * (rhs) of the assignment operator.FuzeType [only available to derived classes]
           *
           * @param rhs The FuzeType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::FuzeType
           * @return A constant reference to this FuzeType.
           */
         const FuzeType& operator=(const FuzeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FuzeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FuzeType_h

