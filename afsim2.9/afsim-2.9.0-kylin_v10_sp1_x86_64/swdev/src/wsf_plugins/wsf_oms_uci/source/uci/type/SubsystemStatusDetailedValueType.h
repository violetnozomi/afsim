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
#ifndef Uci__Type__SubsystemStatusDetailedValueType_h
#define Uci__Type__SubsystemStatusDetailedValueType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__ByteAccessor_h)
# include "uci/base/ByteAccessor.h"
#endif

#if !defined(Uci__Base__ShortAccessor_h)
# include "uci/base/ShortAccessor.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

#if !defined(Uci__Base__LongAccessor_h)
# include "uci/base/LongAccessor.h"
#endif

#if !defined(Uci__Base__FloatAccessor_h)
# include "uci/base/FloatAccessor.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The SubsystemStatusDetailedValue is used to report the value of a single Subsystem status attribute. This type is
        * used to provide status for subsystem unique attributes that cannot be reported with other types or structures.
        */
      class SubsystemStatusDetailedValueType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SubsystemStatusDetailedValueType()
         { }

         /** Returns this accessor's type constant, i.e. SubsystemStatusDetailedValueType
           *
           * @return This accessor's type constant, i.e. SubsystemStatusDetailedValueType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::subsystemStatusDetailedValueType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SubsystemStatusDetailedValueType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum SubsystemStatusDetailedValueTypeChoice {
            SUBSYSTEMSTATUSDETAILEDVALUETYPE_CHOICE_NONE,
            SUBSYSTEMSTATUSDETAILEDVALUETYPE_CHOICE_BOOLEANVALUE,
            SUBSYSTEMSTATUSDETAILEDVALUETYPE_CHOICE_BYTEVALUE,
            SUBSYSTEMSTATUSDETAILEDVALUETYPE_CHOICE_SHORTVALUE,
            SUBSYSTEMSTATUSDETAILEDVALUETYPE_CHOICE_INTVALUE,
            SUBSYSTEMSTATUSDETAILEDVALUETYPE_CHOICE_LONGVALUE,
            SUBSYSTEMSTATUSDETAILEDVALUETYPE_CHOICE_FLOATVALUE,
            SUBSYSTEMSTATUSDETAILEDVALUETYPE_CHOICE_DOUBLEVALUE,
            SUBSYSTEMSTATUSDETAILEDVALUETYPE_CHOICE_STRINGVALUE
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual SubsystemStatusDetailedValueTypeChoice getSubsystemStatusDetailedValueTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getSubsystemStatusDetailedValueTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In
           * addition, the access methods associated with the chosen element will be enabled and will provide access to the chosen
           * element.
           *
           * @param chosenElementOrdinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setSubsystemStatusDetailedValueTypeChoiceOrdinal(SubsystemStatusDetailedValueTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the BooleanValue.
           *
           * @return The value of the simple primitive data type identified by BooleanValue.
           */
         virtual xs::Boolean getBooleanValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the BooleanValue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBooleanValue(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ByteValue.
           *
           * @return The value of the simple primitive data type identified by ByteValue.
           */
         virtual xs::Byte getByteValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ByteValue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setByteValue(xs::Byte value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ShortValue.
           *
           * @return The value of the simple primitive data type identified by ShortValue.
           */
         virtual xs::Short getShortValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ShortValue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setShortValue(xs::Short value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IntValue.
           *
           * @return The value of the simple primitive data type identified by IntValue.
           */
         virtual xs::Int getIntValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IntValue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIntValue(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LongValue.
           *
           * @return The value of the simple primitive data type identified by LongValue.
           */
         virtual xs::Long getLongValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LongValue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLongValue(xs::Long value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FloatValue.
           *
           * @return The value of the simple primitive data type identified by FloatValue.
           */
         virtual xs::Float getFloatValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FloatValue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFloatValue(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DoubleValue.
           *
           * @return The value of the simple primitive data type identified by DoubleValue.
           */
         virtual xs::Double getDoubleValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DoubleValue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDoubleValue(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the StringValue.
           *
           * @return The value of the string data type identified by StringValue.
           */
         virtual const uci::type::VisibleString256Type& getStringValue() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the StringValue.
           *
           * @return The value of the string data type identified by StringValue.
           */
         virtual uci::type::VisibleString256Type& getStringValue()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the StringValue to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setStringValue(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the StringValue to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setStringValue(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the StringValue to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setStringValue(const char* value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemStatusDetailedValueType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemStatusDetailedValueType to copy from
           */
         SubsystemStatusDetailedValueType(const SubsystemStatusDetailedValueType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemStatusDetailedValueType to the contents of the
           * SubsystemStatusDetailedValueType on the right hand side (rhs) of the assignment
           * operator.SubsystemStatusDetailedValueType [only available to derived classes]
           *
           * @param rhs The SubsystemStatusDetailedValueType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::SubsystemStatusDetailedValueType
           * @return A constant reference to this SubsystemStatusDetailedValueType.
           */
         const SubsystemStatusDetailedValueType& operator=(const SubsystemStatusDetailedValueType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemStatusDetailedValueType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SubsystemStatusDetailedValueType_h

