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
#ifndef Uci__Type__RF_DistributionType_h
#define Uci__Type__RF_DistributionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__RFD_ComponentIdentifierType_h)
# include "uci/type/RFD_ComponentIdentifierType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__RF_ThreadFeedbackType_h)
# include "uci/type/RF_ThreadFeedbackType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RF_DistributionType sequence accessor class */
      class RF_DistributionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RF_DistributionType()
         { }

         /** Returns this accessor's type constant, i.e. RF_DistributionType
           *
           * @return This accessor's type constant, i.e. RF_DistributionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rF_DistributionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RF_DistributionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Each status will provide feedback on every ThreadID. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::RF_ThreadFeedbackType, uci::type::accessorType::rF_ThreadFeedbackType> RF_Thread;

         /** Returns the accessor that provides access to the complex content that is identified by the RFD_ComponentIndex.
           *
           * @return The acecssor that provides access to the complex content that is identified by RFD_ComponentIndex.
           */
         virtual const uci::type::RFD_ComponentIdentifierType& getRFD_ComponentIndex() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RFD_ComponentIndex.
           *
           * @return The acecssor that provides access to the complex content that is identified by RFD_ComponentIndex.
           */
         virtual uci::type::RFD_ComponentIdentifierType& getRFD_ComponentIndex()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RFD_ComponentIndex to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RFD_ComponentIndex
           */
         virtual void setRFD_ComponentIndex(const uci::type::RFD_ComponentIdentifierType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_Thread.
           *
           * @return The bounded list identified by RF_Thread.
           */
         virtual const uci::type::RF_DistributionType::RF_Thread& getRF_Thread() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_Thread.
           *
           * @return The bounded list identified by RF_Thread.
           */
         virtual uci::type::RF_DistributionType::RF_Thread& getRF_Thread()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RF_Thread.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRF_Thread(const uci::type::RF_DistributionType::RF_Thread& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RF_DistributionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RF_DistributionType to copy from
           */
         RF_DistributionType(const RF_DistributionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RF_DistributionType to the contents of the RF_DistributionType on
           * the right hand side (rhs) of the assignment operator.RF_DistributionType [only available to derived classes]
           *
           * @param rhs The RF_DistributionType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::RF_DistributionType
           * @return A constant reference to this RF_DistributionType.
           */
         const RF_DistributionType& operator=(const RF_DistributionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RF_DistributionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RF_DistributionType_h

