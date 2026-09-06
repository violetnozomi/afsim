// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__PRI_DataBaseType_h
#define Uci__Type__PRI_DataBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PRI_TypeEnum_h)
# include "uci/type/PRI_TypeEnum.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PRI_DataBaseType sequence accessor class */
      class PRI_DataBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PRI_DataBaseType()
         { }

         /** Returns this accessor's type constant, i.e. PRI_DataBaseType
           *
           * @return This accessor's type constant, i.e. PRI_DataBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pRI_DataBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PRI_DataBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PRI_Type.
           *
           * @return The value of the enumeration identified by PRI_Type.
           */
         virtual const uci::type::PRI_TypeEnum& getPRI_Type() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PRI_Type.
           *
           * @return The value of the enumeration identified by PRI_Type.
           */
         virtual uci::type::PRI_TypeEnum& getPRI_Type()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PRI_Type.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPRI_Type(const uci::type::PRI_TypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PRI_Type.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPRI_Type(uci::type::PRI_TypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PRI_Typeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPRI_Type() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPRI_Type will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::pRI_TypeEnum This Accessor's accessor type
           */
         virtual void enablePRI_Type(uci::base::accessorType::AccessorType type = uci::type::accessorType::pRI_TypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPRI_Type()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_Average.
           *
           * @return The value of the simple primitive data type identified by PRI_Average.
           */
         virtual uci::type::DurationTypeValue getPRI_Average() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_Average.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_Average(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_Average exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_Average is emabled or not
           */
         virtual bool hasPRI_Average() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_Average
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePRI_Average(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_Average */
         virtual void clearPRI_Average()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_Min.
           *
           * @return The value of the simple primitive data type identified by PRI_Min.
           */
         virtual uci::type::DurationTypeValue getPRI_Min() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_Min.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_Min(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_Min exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_Min is emabled or not
           */
         virtual bool hasPRI_Min() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_Min
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePRI_Min(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_Min */
         virtual void clearPRI_Min()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_Max.
           *
           * @return The value of the simple primitive data type identified by PRI_Max.
           */
         virtual uci::type::DurationTypeValue getPRI_Max() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_Max.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_Max(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_Max exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_Max is emabled or not
           */
         virtual bool hasPRI_Max() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_Max
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePRI_Max(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_Max */
         virtual void clearPRI_Max()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_Last.
           *
           * @return The value of the simple primitive data type identified by PRI_Last.
           */
         virtual uci::type::DurationTypeValue getPRI_Last() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_Last.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_Last(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_Last exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_Last is emabled or not
           */
         virtual bool hasPRI_Last() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_Last
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePRI_Last(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_Last */
         virtual void clearPRI_Last()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_Third.
           *
           * @return The value of the simple primitive data type identified by PRI_Third.
           */
         virtual uci::type::DurationTypeValue getPRI_Third() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_Third.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_Third(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_Third exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_Third is emabled or not
           */
         virtual bool hasPRI_Third() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_Third
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePRI_Third(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_Third */
         virtual void clearPRI_Third()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PRI_DataBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PRI_DataBaseType to copy from
           */
         PRI_DataBaseType(const PRI_DataBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PRI_DataBaseType to the contents of the PRI_DataBaseType on the
           * right hand side (rhs) of the assignment operator.PRI_DataBaseType [only available to derived classes]
           *
           * @param rhs The PRI_DataBaseType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::PRI_DataBaseType
           * @return A constant reference to this PRI_DataBaseType.
           */
         const PRI_DataBaseType& operator=(const PRI_DataBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PRI_DataBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PRI_DataBaseType_h

