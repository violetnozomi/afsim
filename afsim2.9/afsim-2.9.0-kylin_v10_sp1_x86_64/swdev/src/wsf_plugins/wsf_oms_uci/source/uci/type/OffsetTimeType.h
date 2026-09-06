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
#ifndef Uci__Type__OffsetTimeType_h
#define Uci__Type__OffsetTimeType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Represents an offset from a time, can define a tolerance around a time. */
      class OffsetTimeType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OffsetTimeType()
         { }

         /** Returns this accessor's type constant, i.e. OffsetTimeType
           *
           * @return This accessor's type constant, i.e. OffsetTimeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::offsetTimeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OffsetTimeType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Before.
           *
           * @return The value of the simple primitive data type identified by Before.
           */
         virtual uci::type::DurationTypeValue getBefore() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Before.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBefore(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Before exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Before is emabled or not
           */
         virtual bool hasBefore() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Before
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableBefore(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Before */
         virtual void clearBefore()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the After.
           *
           * @return The value of the simple primitive data type identified by After.
           */
         virtual uci::type::DurationTypeValue getAfter() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the After.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAfter(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by After exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by After is emabled or not
           */
         virtual bool hasAfter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by After
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableAfter(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by After */
         virtual void clearAfter()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OffsetTimeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OffsetTimeType to copy from
           */
         OffsetTimeType(const OffsetTimeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OffsetTimeType to the contents of the OffsetTimeType on the right
           * hand side (rhs) of the assignment operator.OffsetTimeType [only available to derived classes]
           *
           * @param rhs The OffsetTimeType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::OffsetTimeType
           * @return A constant reference to this OffsetTimeType.
           */
         const OffsetTimeType& operator=(const OffsetTimeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OffsetTimeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OffsetTimeType_h

