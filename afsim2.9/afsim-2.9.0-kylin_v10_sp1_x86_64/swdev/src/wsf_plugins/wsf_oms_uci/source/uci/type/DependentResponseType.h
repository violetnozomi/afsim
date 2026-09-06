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
#ifndef Uci__Type__DependentResponseType_h
#define Uci__Type__DependentResponseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DependentResponseType sequence accessor class */
      class DependentResponseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DependentResponseType()
         { }

         /** Returns this accessor's type constant, i.e. DependentResponseType
           *
           * @return This accessor's type constant, i.e. DependentResponseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dependentResponseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DependentResponseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ResponseIndex.
           *
           * @return The value of the simple primitive data type identified by ResponseIndex.
           */
         virtual xs::UnsignedInt getResponseIndex() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ResponseIndex.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setResponseIndex(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MinRelativeTime.
           *
           * @return The value of the simple primitive data type identified by MinRelativeTime.
           */
         virtual uci::type::DurationTypeValue getMinRelativeTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MinRelativeTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMinRelativeTime(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MinRelativeTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MinRelativeTime is emabled or not
           */
         virtual bool hasMinRelativeTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MinRelativeTime
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableMinRelativeTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MinRelativeTime */
         virtual void clearMinRelativeTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaxRelativeTime.
           *
           * @return The value of the simple primitive data type identified by MaxRelativeTime.
           */
         virtual uci::type::DurationTypeValue getMaxRelativeTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaxRelativeTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaxRelativeTime(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MaxRelativeTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MaxRelativeTime is emabled or not
           */
         virtual bool hasMaxRelativeTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MaxRelativeTime
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableMaxRelativeTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MaxRelativeTime */
         virtual void clearMaxRelativeTime()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DependentResponseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DependentResponseType to copy from
           */
         DependentResponseType(const DependentResponseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DependentResponseType to the contents of the DependentResponseType
           * on the right hand side (rhs) of the assignment operator.DependentResponseType [only available to derived classes]
           *
           * @param rhs The DependentResponseType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::DependentResponseType
           * @return A constant reference to this DependentResponseType.
           */
         const DependentResponseType& operator=(const DependentResponseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DependentResponseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DependentResponseType_h

