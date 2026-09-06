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
#ifndef Uci__Type__RF_ThreadFeedbackType_h
#define Uci__Type__RF_ThreadFeedbackType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__RF_ThreadIdentifierType_h)
# include "uci/type/RF_ThreadIdentifierType.h"
#endif

#if !defined(Uci__Type__ThreadAvailabiltyEnum_h)
# include "uci/type/ThreadAvailabiltyEnum.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Utilization feedback for each of an RFD component's RF paths */
      class RF_ThreadFeedbackType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RF_ThreadFeedbackType()
         { }

         /** Returns this accessor's type constant, i.e. RF_ThreadFeedbackType
           *
           * @return This accessor's type constant, i.e. RF_ThreadFeedbackType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rF_ThreadFeedbackType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RF_ThreadFeedbackType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RF_ThreadIndex.
           *
           * @return The acecssor that provides access to the complex content that is identified by RF_ThreadIndex.
           */
         virtual const uci::type::RF_ThreadIdentifierType& getRF_ThreadIndex() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RF_ThreadIndex.
           *
           * @return The acecssor that provides access to the complex content that is identified by RF_ThreadIndex.
           */
         virtual uci::type::RF_ThreadIdentifierType& getRF_ThreadIndex()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RF_ThreadIndex to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RF_ThreadIndex
           */
         virtual void setRF_ThreadIndex(const uci::type::RF_ThreadIdentifierType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Availability.
           *
           * @return The value of the enumeration identified by Availability.
           */
         virtual const uci::type::ThreadAvailabiltyEnum& getAvailability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Availability.
           *
           * @return The value of the enumeration identified by Availability.
           */
         virtual uci::type::ThreadAvailabiltyEnum& getAvailability()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Availability.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAvailability(const uci::type::ThreadAvailabiltyEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Availability.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAvailability(uci::type::ThreadAvailabiltyEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentAccess.
           *
           * @return The value of the simple primitive data type identified by PercentAccess.
           */
         virtual uci::type::PercentTypeValue getPercentAccess() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentAccess.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentAccess(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PercentAccess exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PercentAccess is emabled or not
           */
         virtual bool hasPercentAccess() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PercentAccess
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enablePercentAccess(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PercentAccess */
         virtual void clearPercentAccess()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RF_ThreadFeedbackType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RF_ThreadFeedbackType to copy from
           */
         RF_ThreadFeedbackType(const RF_ThreadFeedbackType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RF_ThreadFeedbackType to the contents of the RF_ThreadFeedbackType
           * on the right hand side (rhs) of the assignment operator.RF_ThreadFeedbackType [only available to derived classes]
           *
           * @param rhs The RF_ThreadFeedbackType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::RF_ThreadFeedbackType
           * @return A constant reference to this RF_ThreadFeedbackType.
           */
         const RF_ThreadFeedbackType& operator=(const RF_ThreadFeedbackType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RF_ThreadFeedbackType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RF_ThreadFeedbackType_h

