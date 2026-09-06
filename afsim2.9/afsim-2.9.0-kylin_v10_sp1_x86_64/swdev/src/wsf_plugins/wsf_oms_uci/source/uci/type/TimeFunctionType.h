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
#ifndef Uci__Type__TimeFunctionType_h
#define Uci__Type__TimeFunctionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__TimeFunctionEnum_h)
# include "uci/type/TimeFunctionEnum.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** A time and its associated function or purpose. */
      class TimeFunctionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~TimeFunctionType()
         { }

         /** Returns this accessor's type constant, i.e. TimeFunctionType
           *
           * @return This accessor's type constant, i.e. TimeFunctionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::timeFunctionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TimeFunctionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Function.
           *
           * @return The value of the enumeration identified by Function.
           */
         virtual const uci::type::TimeFunctionEnum& getFunction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Function.
           *
           * @return The value of the enumeration identified by Function.
           */
         virtual uci::type::TimeFunctionEnum& getFunction()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Function.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFunction(const uci::type::TimeFunctionEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Function.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFunction(uci::type::TimeFunctionEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Value.
           *
           * @return The value of the simple primitive data type identified by Value.
           */
         virtual uci::type::DateTimeTypeValue getValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Value.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setValue(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TimeFunctionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TimeFunctionType to copy from
           */
         TimeFunctionType(const TimeFunctionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TimeFunctionType to the contents of the TimeFunctionType on the
           * right hand side (rhs) of the assignment operator.TimeFunctionType [only available to derived classes]
           *
           * @param rhs The TimeFunctionType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::TimeFunctionType
           * @return A constant reference to this TimeFunctionType.
           */
         const TimeFunctionType& operator=(const TimeFunctionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TimeFunctionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TimeFunctionType_h

