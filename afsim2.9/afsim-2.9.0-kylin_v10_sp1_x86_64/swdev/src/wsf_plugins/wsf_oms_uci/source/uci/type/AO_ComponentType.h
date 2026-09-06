// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__AO_ComponentType_h
#define Uci__Type__AO_ComponentType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

#if !defined(Uci__Type__PowerType_h)
# include "uci/type/PowerType.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AO_CodeType_h)
# include "uci/type/AO_CodeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AO_ComponentType sequence accessor class */
      class AO_ComponentType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AO_ComponentType()
         { }

         /** Returns this accessor's type constant, i.e. AO_ComponentType
           *
           * @return This accessor's type constant, i.e. AO_ComponentType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aO_ComponentType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AO_ComponentType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a PRF and/or PIM code supported by the AO component. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::AO_CodeType, uci::type::accessorType::aO_CodeType> SupportedCode;

         /** Returns the accessor that provides access to the complex content that is identified by the ComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentID.
           */
         virtual const uci::type::ComponentID_Type& getComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentID.
           */
         virtual uci::type::ComponentID_Type& getComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ComponentID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ComponentID
           */
         virtual void setComponentID(const uci::type::ComponentID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MinimumPower.
           *
           * @return The value of the simple primitive data type identified by MinimumPower.
           */
         virtual uci::type::PowerTypeValue getMinimumPower() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MinimumPower.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMinimumPower(uci::type::PowerTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaximumPower.
           *
           * @return The value of the simple primitive data type identified by MaximumPower.
           */
         virtual uci::type::PowerTypeValue getMaximumPower() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaximumPower.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaximumPower(uci::type::PowerTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MinimumWavelength.
           *
           * @return The value of the simple primitive data type identified by MinimumWavelength.
           */
         virtual uci::type::DistanceTypeValue getMinimumWavelength() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MinimumWavelength.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMinimumWavelength(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaximumWavelength.
           *
           * @return The value of the simple primitive data type identified by MaximumWavelength.
           */
         virtual uci::type::DistanceTypeValue getMaximumWavelength() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaximumWavelength.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaximumWavelength(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedCode.
           *
           * @return The bounded list identified by SupportedCode.
           */
         virtual const uci::type::AO_ComponentType::SupportedCode& getSupportedCode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedCode.
           *
           * @return The bounded list identified by SupportedCode.
           */
         virtual uci::type::AO_ComponentType::SupportedCode& getSupportedCode()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportedCode.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportedCode(const uci::type::AO_ComponentType::SupportedCode& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AO_ComponentType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AO_ComponentType to copy from
           */
         AO_ComponentType(const AO_ComponentType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AO_ComponentType to the contents of the AO_ComponentType on the
           * right hand side (rhs) of the assignment operator.AO_ComponentType [only available to derived classes]
           *
           * @param rhs The AO_ComponentType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::AO_ComponentType
           * @return A constant reference to this AO_ComponentType.
           */
         const AO_ComponentType& operator=(const AO_ComponentType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AO_ComponentType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AO_ComponentType_h

