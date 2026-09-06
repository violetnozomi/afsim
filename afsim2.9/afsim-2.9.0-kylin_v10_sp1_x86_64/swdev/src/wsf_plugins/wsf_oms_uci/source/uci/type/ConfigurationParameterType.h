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
#ifndef Uci__Type__ConfigurationParameterType_h
#define Uci__Type__ConfigurationParameterType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ParameterID_Type_h)
# include "uci/type/ParameterID_Type.h"
#endif

#if !defined(Uci__Type__VisibleString32Type_h)
# include "uci/type/VisibleString32Type.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__MessageElementDataTypeEnum_h)
# include "uci/type/MessageElementDataTypeEnum.h"
#endif

#if !defined(Uci__Type__ConfigurationParameterValueRestrictionsType_h)
# include "uci/type/ConfigurationParameterValueRestrictionsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ConfigurationParameterType sequence accessor class */
      class ConfigurationParameterType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ConfigurationParameterType()
         { }

         /** Returns this accessor's type constant, i.e. ConfigurationParameterType
           *
           * @return This accessor's type constant, i.e. ConfigurationParameterType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::configurationParameterType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ConfigurationParameterType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ParameterID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ParameterID.
           */
         virtual const uci::type::ParameterID_Type& getParameterID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ParameterID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ParameterID.
           */
         virtual uci::type::ParameterID_Type& getParameterID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ParameterID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ParameterID
           */
         virtual void setParameterID(const uci::type::ParameterID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Name.
           *
           * @return The value of the string data type identified by Name.
           */
         virtual const uci::type::VisibleString32Type& getName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Name.
           *
           * @return The value of the string data type identified by Name.
           */
         virtual uci::type::VisibleString32Type& getName()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Name to the string accessed by the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setName(const uci::type::VisibleString32Type& value)
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


         /** Returns the value of the string data type that is identified by the Value.
           *
           * @return The value of the string data type identified by Value.
           */
         virtual const uci::type::VisibleString256Type& getValue() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Value.
           *
           * @return The value of the string data type identified by Value.
           */
         virtual uci::type::VisibleString256Type& getValue()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Value to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setValue(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Value to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setValue(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Value to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setValue(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DefaultValue.
           *
           * @return The value of the string data type identified by DefaultValue.
           */
         virtual const uci::type::VisibleString256Type& getDefaultValue() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DefaultValue.
           *
           * @return The value of the string data type identified by DefaultValue.
           */
         virtual uci::type::VisibleString256Type& getDefaultValue()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DefaultValue to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setDefaultValue(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DefaultValue to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDefaultValue(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DefaultValue to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDefaultValue(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the UserModifiable.
           *
           * @return The value of the simple primitive data type identified by UserModifiable.
           */
         virtual xs::Boolean getUserModifiable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the UserModifiable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setUserModifiable(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Type.
           *
           * @return The value of the enumeration identified by Type.
           */
         virtual const uci::type::MessageElementDataTypeEnum& getType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Type.
           *
           * @return The value of the enumeration identified by Type.
           */
         virtual uci::type::MessageElementDataTypeEnum& getType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Type.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setType(const uci::type::MessageElementDataTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Type.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setType(uci::type::MessageElementDataTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Units.
           *
           * @return The value of the string data type identified by Units.
           */
         virtual const uci::type::VisibleString32Type& getUnits() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Units.
           *
           * @return The value of the string data type identified by Units.
           */
         virtual uci::type::VisibleString32Type& getUnits()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Units to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setUnits(const uci::type::VisibleString32Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Units to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setUnits(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Units to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setUnits(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Units exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Units is emabled or not
           */
         virtual bool hasUnits() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Units
           *
           * @param type = uci::type::accessorType::visibleString32Type This Accessor's accessor type
           */
         virtual void enableUnits(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString32Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Units */
         virtual void clearUnits()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TypeSpecificRestrictions.
           *
           * @return The acecssor that provides access to the complex content that is identified by TypeSpecificRestrictions.
           */
         virtual const uci::type::ConfigurationParameterValueRestrictionsType& getTypeSpecificRestrictions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TypeSpecificRestrictions.
           *
           * @return The acecssor that provides access to the complex content that is identified by TypeSpecificRestrictions.
           */
         virtual uci::type::ConfigurationParameterValueRestrictionsType& getTypeSpecificRestrictions()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TypeSpecificRestrictions to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TypeSpecificRestrictions
           */
         virtual void setTypeSpecificRestrictions(const uci::type::ConfigurationParameterValueRestrictionsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TypeSpecificRestrictions exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TypeSpecificRestrictions is emabled or not
           */
         virtual bool hasTypeSpecificRestrictions() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TypeSpecificRestrictions
           *
           * @param type = uci::type::accessorType::configurationParameterValueRestrictionsType This Accessor's accessor type
           */
         virtual void enableTypeSpecificRestrictions(uci::base::accessorType::AccessorType type = uci::type::accessorType::configurationParameterValueRestrictionsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TypeSpecificRestrictions */
         virtual void clearTypeSpecificRestrictions()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ConfigurationParameterType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ConfigurationParameterType to copy from
           */
         ConfigurationParameterType(const ConfigurationParameterType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ConfigurationParameterType to the contents of the
           * ConfigurationParameterType on the right hand side (rhs) of the assignment operator.ConfigurationParameterType [only
           * available to derived classes]
           *
           * @param rhs The ConfigurationParameterType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ConfigurationParameterType
           * @return A constant reference to this ConfigurationParameterType.
           */
         const ConfigurationParameterType& operator=(const ConfigurationParameterType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ConfigurationParameterType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ConfigurationParameterType_h

