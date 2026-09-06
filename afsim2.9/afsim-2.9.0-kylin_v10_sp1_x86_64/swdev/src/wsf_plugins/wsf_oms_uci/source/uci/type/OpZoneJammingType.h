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
#ifndef Uci__Type__OpZoneJammingType_h
#define Uci__Type__OpZoneJammingType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OpZoneJammingType sequence accessor class */
      class OpZoneJammingType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OpZoneJammingType()
         { }

         /** Returns this accessor's type constant, i.e. OpZoneJammingType
           *
           * @return This accessor's type constant, i.e. OpZoneJammingType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::opZoneJammingType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OpZoneJammingType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a specific frequency band where jamming is not allowed. This element is only applicable when the sibling
           * Allowed element indicates jamming is allowed. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> RestrictedFrequencyBand;

         /** Returns the value of the SimplePrimitive data type that is identified by the Allowed.
           *
           * @return The value of the simple primitive data type identified by Allowed.
           */
         virtual xs::Boolean getAllowed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Allowed.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAllowed(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RestrictedFrequencyBand.
           *
           * @return The bounded list identified by RestrictedFrequencyBand.
           */
         virtual const uci::type::OpZoneJammingType::RestrictedFrequencyBand& getRestrictedFrequencyBand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RestrictedFrequencyBand.
           *
           * @return The bounded list identified by RestrictedFrequencyBand.
           */
         virtual uci::type::OpZoneJammingType::RestrictedFrequencyBand& getRestrictedFrequencyBand()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RestrictedFrequencyBand.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRestrictedFrequencyBand(const uci::type::OpZoneJammingType::RestrictedFrequencyBand& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OpZoneJammingType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OpZoneJammingType to copy from
           */
         OpZoneJammingType(const OpZoneJammingType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OpZoneJammingType to the contents of the OpZoneJammingType on the
           * right hand side (rhs) of the assignment operator.OpZoneJammingType [only available to derived classes]
           *
           * @param rhs The OpZoneJammingType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::OpZoneJammingType
           * @return A constant reference to this OpZoneJammingType.
           */
         const OpZoneJammingType& operator=(const OpZoneJammingType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OpZoneJammingType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OpZoneJammingType_h

