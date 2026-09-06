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
#ifndef Uci__Type__SpeedRangeType_h
#define Uci__Type__SpeedRangeType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SpeedType_h)
# include "uci/type/SpeedType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SpeedRangeType sequence accessor class */
      class SpeedRangeType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SpeedRangeType()
         { }

         /** Returns this accessor's type constant, i.e. SpeedRangeType
           *
           * @return This accessor's type constant, i.e. SpeedRangeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::speedRangeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SpeedRangeType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Min.
           *
           * @return The value of the simple primitive data type identified by Min.
           */
         virtual uci::type::SpeedTypeValue getMin() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Min.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMin(uci::type::SpeedTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Max.
           *
           * @return The value of the simple primitive data type identified by Max.
           */
         virtual uci::type::SpeedTypeValue getMax() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Max.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMax(uci::type::SpeedTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SpeedRangeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SpeedRangeType to copy from
           */
         SpeedRangeType(const SpeedRangeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SpeedRangeType to the contents of the SpeedRangeType on the right
           * hand side (rhs) of the assignment operator.SpeedRangeType [only available to derived classes]
           *
           * @param rhs The SpeedRangeType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::SpeedRangeType
           * @return A constant reference to this SpeedRangeType.
           */
         const SpeedRangeType& operator=(const SpeedRangeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SpeedRangeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SpeedRangeType_h

