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
#ifndef Uci__Type__ElectronicProtectionOptionsSupportedType_h
#define Uci__Type__ElectronicProtectionOptionsSupportedType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EP_StateEnum_h)
# include "uci/type/EP_StateEnum.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ElectronicProtectionOptionsSupportedType sequence accessor class */
      class ElectronicProtectionOptionsSupportedType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ElectronicProtectionOptionsSupportedType()
         { }

         /** Returns this accessor's type constant, i.e. ElectronicProtectionOptionsSupportedType
           *
           * @return This accessor's type constant, i.e. ElectronicProtectionOptionsSupportedType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::electronicProtectionOptionsSupportedType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ElectronicProtectionOptionsSupportedType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ElectronicProtectionOptionState.
           *
           * @return The value of the enumeration identified by ElectronicProtectionOptionState.
           */
         virtual const uci::type::EP_StateEnum& getElectronicProtectionOptionState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ElectronicProtectionOptionState.
           *
           * @return The value of the enumeration identified by ElectronicProtectionOptionState.
           */
         virtual uci::type::EP_StateEnum& getElectronicProtectionOptionState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ElectronicProtectionOptionState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setElectronicProtectionOptionState(const uci::type::EP_StateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ElectronicProtectionOptionState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setElectronicProtectionOptionState(uci::type::EP_StateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Reference.
           *
           * @return The acecssor that provides access to the complex content that is identified by Reference.
           */
         virtual const uci::type::ForeignKeyType& getReference() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Reference.
           *
           * @return The acecssor that provides access to the complex content that is identified by Reference.
           */
         virtual uci::type::ForeignKeyType& getReference()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Reference to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Reference
           */
         virtual void setReference(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ElectronicProtectionOptionsSupportedType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ElectronicProtectionOptionsSupportedType to copy from
           */
         ElectronicProtectionOptionsSupportedType(const ElectronicProtectionOptionsSupportedType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ElectronicProtectionOptionsSupportedType to the contents of the
           * ElectronicProtectionOptionsSupportedType on the right hand side (rhs) of the assignment
           * operator.ElectronicProtectionOptionsSupportedType [only available to derived classes]
           *
           * @param rhs The ElectronicProtectionOptionsSupportedType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ElectronicProtectionOptionsSupportedType
           * @return A constant reference to this ElectronicProtectionOptionsSupportedType.
           */
         const ElectronicProtectionOptionsSupportedType& operator=(const ElectronicProtectionOptionsSupportedType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ElectronicProtectionOptionsSupportedType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ElectronicProtectionOptionsSupportedType_h

