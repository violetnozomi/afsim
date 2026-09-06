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
#ifndef Uci__Type__CommScheduleAvailabilityMDT_h
#define Uci__Type__CommScheduleAvailabilityMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommScheduleAvailabilityID_Type_h)
# include "uci/type/CommScheduleAvailabilityID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommCoverageDataType_h)
# include "uci/type/CommCoverageDataType.h"
#endif

#if !defined(Uci__Type__CommOutageType_h)
# include "uci/type/CommOutageType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommScheduleAvailabilityMDT sequence accessor class */
      class CommScheduleAvailabilityMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommScheduleAvailabilityMDT()
         { }

         /** Returns this accessor's type constant, i.e. CommScheduleAvailabilityMDT
           *
           * @return This accessor's type constant, i.e. CommScheduleAvailabilityMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commScheduleAvailabilityMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommScheduleAvailabilityMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Represents the external communication resources available to be scheduled [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommCoverageDataType, uci::type::accessorType::commCoverageDataType> AvailableComm;

         /** Represents a known outage for an external communication resource such that it is or will be unavailable for use.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommOutageType, uci::type::accessorType::commOutageType> CommOutage;

         /** Returns the accessor that provides access to the complex content that is identified by the
           * CommScheduleAvailabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommScheduleAvailabilityID.
           */
         virtual const uci::type::CommScheduleAvailabilityID_Type& getCommScheduleAvailabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * CommScheduleAvailabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommScheduleAvailabilityID.
           */
         virtual uci::type::CommScheduleAvailabilityID_Type& getCommScheduleAvailabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommScheduleAvailabilityID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommScheduleAvailabilityID
           */
         virtual void setCommScheduleAvailabilityID(const uci::type::CommScheduleAvailabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AvailableComm.
           *
           * @return The bounded list identified by AvailableComm.
           */
         virtual const uci::type::CommScheduleAvailabilityMDT::AvailableComm& getAvailableComm() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AvailableComm.
           *
           * @return The bounded list identified by AvailableComm.
           */
         virtual uci::type::CommScheduleAvailabilityMDT::AvailableComm& getAvailableComm()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AvailableComm.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAvailableComm(const uci::type::CommScheduleAvailabilityMDT::AvailableComm& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommOutage.
           *
           * @return The bounded list identified by CommOutage.
           */
         virtual const uci::type::CommScheduleAvailabilityMDT::CommOutage& getCommOutage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommOutage.
           *
           * @return The bounded list identified by CommOutage.
           */
         virtual uci::type::CommScheduleAvailabilityMDT::CommOutage& getCommOutage()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommOutage.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommOutage(const uci::type::CommScheduleAvailabilityMDT::CommOutage& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommScheduleAvailabilityMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommScheduleAvailabilityMDT to copy from
           */
         CommScheduleAvailabilityMDT(const CommScheduleAvailabilityMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommScheduleAvailabilityMDT to the contents of the
           * CommScheduleAvailabilityMDT on the right hand side (rhs) of the assignment operator.CommScheduleAvailabilityMDT [only
           * available to derived classes]
           *
           * @param rhs The CommScheduleAvailabilityMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommScheduleAvailabilityMDT
           * @return A constant reference to this CommScheduleAvailabilityMDT.
           */
         const CommScheduleAvailabilityMDT& operator=(const CommScheduleAvailabilityMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommScheduleAvailabilityMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommScheduleAvailabilityMDT_h

