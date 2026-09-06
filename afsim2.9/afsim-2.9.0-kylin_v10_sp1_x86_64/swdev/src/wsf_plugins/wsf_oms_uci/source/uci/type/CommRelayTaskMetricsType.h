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
#ifndef Uci__Type__CommRelayTaskMetricsType_h
#define Uci__Type__CommRelayTaskMetricsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__LinkMetricsType_h)
# include "uci/type/LinkMetricsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommRelayTaskMetricsType sequence accessor class */
      class CommRelayTaskMetricsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommRelayTaskMetricsType()
         { }

         /** Returns this accessor's type constant, i.e. CommRelayTaskMetricsType
           *
           * @return This accessor's type constant, i.e. CommRelayTaskMetricsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commRelayTaskMetricsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommRelayTaskMetricsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Qualitative assessment data for communication relay missions. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::LinkMetricsType, uci::type::accessorType::linkMetricsType> LinkMetrics;

         /** Returns the value of the SimplePrimitive data type that is identified by the TimeOnStation.
           *
           * @return The value of the simple primitive data type identified by TimeOnStation.
           */
         virtual uci::type::DurationTypeValue getTimeOnStation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TimeOnStation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimeOnStation(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NumDropOuts.
           *
           * @return The value of the simple primitive data type identified by NumDropOuts.
           */
         virtual xs::UnsignedInt getNumDropOuts() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NumDropOuts.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumDropOuts(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LinkMetrics.
           *
           * @return The bounded list identified by LinkMetrics.
           */
         virtual const uci::type::CommRelayTaskMetricsType::LinkMetrics& getLinkMetrics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LinkMetrics.
           *
           * @return The bounded list identified by LinkMetrics.
           */
         virtual uci::type::CommRelayTaskMetricsType::LinkMetrics& getLinkMetrics()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the LinkMetrics.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setLinkMetrics(const uci::type::CommRelayTaskMetricsType::LinkMetrics& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommRelayTaskMetricsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommRelayTaskMetricsType to copy from
           */
         CommRelayTaskMetricsType(const CommRelayTaskMetricsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommRelayTaskMetricsType to the contents of the
           * CommRelayTaskMetricsType on the right hand side (rhs) of the assignment operator.CommRelayTaskMetricsType [only
           * available to derived classes]
           *
           * @param rhs The CommRelayTaskMetricsType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommRelayTaskMetricsType
           * @return A constant reference to this CommRelayTaskMetricsType.
           */
         const CommRelayTaskMetricsType& operator=(const CommRelayTaskMetricsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommRelayTaskMetricsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommRelayTaskMetricsType_h

