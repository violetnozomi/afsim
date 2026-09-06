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
#ifndef Uci__Type__TargetInformationType_h
#define Uci__Type__TargetInformationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__TargetTypeEnum_h)
# include "uci/type/TargetTypeEnum.h"
#endif

#if !defined(Uci__Type__TargetDefensesEnum_h)
# include "uci/type/TargetDefensesEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates additional information of a target. */
      class TargetInformationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~TargetInformationType()
         { }

         /** Returns this accessor's type constant, i.e. TargetInformationType
           *
           * @return This accessor's type constant, i.e. TargetInformationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::targetInformationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TargetInformationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NumberOfDMPIs.
           *
           * @return The value of the simple primitive data type identified by NumberOfDMPIs.
           */
         virtual xs::UnsignedInt getNumberOfDMPIs() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NumberOfDMPIs.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumberOfDMPIs(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NumberOfDMPIs exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NumberOfDMPIs is emabled or not
           */
         virtual bool hasNumberOfDMPIs() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NumberOfDMPIs
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableNumberOfDMPIs(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NumberOfDMPIs */
         virtual void clearNumberOfDMPIs()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TargetType.
           *
           * @return The value of the enumeration identified by TargetType.
           */
         virtual const uci::type::TargetTypeEnum& getTargetType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TargetType.
           *
           * @return The value of the enumeration identified by TargetType.
           */
         virtual uci::type::TargetTypeEnum& getTargetType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TargetType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTargetType(const uci::type::TargetTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TargetType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTargetType(uci::type::TargetTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield TargetTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTargetType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTargetType will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::targetTypeEnum This Accessor's accessor type
           */
         virtual void enableTargetType(uci::base::accessorType::AccessorType type = uci::type::accessorType::targetTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTargetType()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TargetDefenses.
           *
           * @return The value of the enumeration identified by TargetDefenses.
           */
         virtual const uci::type::TargetDefensesEnum& getTargetDefenses() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TargetDefenses.
           *
           * @return The value of the enumeration identified by TargetDefenses.
           */
         virtual uci::type::TargetDefensesEnum& getTargetDefenses()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TargetDefenses.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTargetDefenses(const uci::type::TargetDefensesEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TargetDefenses.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTargetDefenses(uci::type::TargetDefensesEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield TargetDefensesis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTargetDefenses() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTargetDefenses will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::targetDefensesEnum This Accessor's accessor type
           */
         virtual void enableTargetDefenses(uci::base::accessorType::AccessorType type = uci::type::accessorType::targetDefensesEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTargetDefenses()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TargetInformationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TargetInformationType to copy from
           */
         TargetInformationType(const TargetInformationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TargetInformationType to the contents of the TargetInformationType
           * on the right hand side (rhs) of the assignment operator.TargetInformationType [only available to derived classes]
           *
           * @param rhs The TargetInformationType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::TargetInformationType
           * @return A constant reference to this TargetInformationType.
           */
         const TargetInformationType& operator=(const TargetInformationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TargetInformationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TargetInformationType_h

