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
#ifndef Uci__Type__PeriodPerformanceFeedbackType_h
#define Uci__Type__PeriodPerformanceFeedbackType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Performance feedback characteristics for each access period */
      class PeriodPerformanceFeedbackType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PeriodPerformanceFeedbackType()
         { }

         /** Returns this accessor's type constant, i.e. PeriodPerformanceFeedbackType
           *
           * @return This accessor's type constant, i.e. PeriodPerformanceFeedbackType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::periodPerformanceFeedbackType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PeriodPerformanceFeedbackType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AccessPeriodID.
           *
           * @return The value of the simple primitive data type identified by AccessPeriodID.
           */
         virtual xs::UnsignedInt getAccessPeriodID() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AccessPeriodID.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAccessPeriodID(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentAccessAllocated.
           *
           * @return The value of the simple primitive data type identified by PercentAccessAllocated.
           */
         virtual uci::type::PercentTypeValue getPercentAccessAllocated() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentAccessAllocated.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentAccessAllocated(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentAccessRequested.
           *
           * @return The value of the simple primitive data type identified by PercentAccessRequested.
           */
         virtual uci::type::PercentTypeValue getPercentAccessRequested() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentAccessRequested.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentAccessRequested(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentAccessAchieved.
           *
           * @return The value of the simple primitive data type identified by PercentAccessAchieved.
           */
         virtual uci::type::PercentTypeValue getPercentAccessAchieved() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentAccessAchieved.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentAccessAchieved(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentInterruption.
           *
           * @return The value of the simple primitive data type identified by PercentInterruption.
           */
         virtual uci::type::PercentTypeValue getPercentInterruption() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentInterruption.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentInterruption(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LongestUninterruptedTime.
           *
           * @return The value of the simple primitive data type identified by LongestUninterruptedTime.
           */
         virtual uci::type::DurationTypeValue getLongestUninterruptedTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LongestUninterruptedTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLongestUninterruptedTime(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PeriodPerformanceFeedbackType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PeriodPerformanceFeedbackType to copy from
           */
         PeriodPerformanceFeedbackType(const PeriodPerformanceFeedbackType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PeriodPerformanceFeedbackType to the contents of the
           * PeriodPerformanceFeedbackType on the right hand side (rhs) of the assignment operator.PeriodPerformanceFeedbackType
           * [only available to derived classes]
           *
           * @param rhs The PeriodPerformanceFeedbackType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::PeriodPerformanceFeedbackType
           * @return A constant reference to this PeriodPerformanceFeedbackType.
           */
         const PeriodPerformanceFeedbackType& operator=(const PeriodPerformanceFeedbackType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PeriodPerformanceFeedbackType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PeriodPerformanceFeedbackType_h

