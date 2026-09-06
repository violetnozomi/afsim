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
#ifndef Uci__Type__CommSchedulingRequestStatusMDT_h
#define Uci__Type__CommSchedulingRequestStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommSchedulingRequestID_Type_h)
# include "uci/type/CommSchedulingRequestID_Type.h"
#endif

#if !defined(Uci__Type__CommSchedulingRequestApprovalEnum_h)
# include "uci/type/CommSchedulingRequestApprovalEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommSchedulingRequirementID_Type_h)
# include "uci/type/CommSchedulingRequirementID_Type.h"
#endif

#if !defined(Uci__Type__CommCoverageUserDataType_h)
# include "uci/type/CommCoverageUserDataType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommSchedulingRequestStatusMDT sequence accessor class */
      class CommSchedulingRequestStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommSchedulingRequestStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. CommSchedulingRequestStatusMDT
           *
           * @return This accessor's type constant, i.e. CommSchedulingRequestStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commSchedulingRequestStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommSchedulingRequestStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** A list of all of the requirements that should be accounted for in the scheduled communication element. Includes all
           * requested requirements regardless of whether or not the sibling scheduled communication element satisfies them.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommSchedulingRequirementID_Type, uci::type::accessorType::commSchedulingRequirementID_Type> CommSchedulingRequirementID;

         /** A list of allocated external communication resources and time that have been scheduled and approved. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommCoverageUserDataType, uci::type::accessorType::commCoverageUserDataType> ScheduledComm;

         /** Returns the accessor that provides access to the complex content that is identified by the CommSchedulingRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSchedulingRequestID.
           */
         virtual const uci::type::CommSchedulingRequestID_Type& getCommSchedulingRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommSchedulingRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSchedulingRequestID.
           */
         virtual uci::type::CommSchedulingRequestID_Type& getCommSchedulingRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommSchedulingRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommSchedulingRequestID
           */
         virtual void setCommSchedulingRequestID(const uci::type::CommSchedulingRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ApprovalStatus.
           *
           * @return The value of the enumeration identified by ApprovalStatus.
           */
         virtual const uci::type::CommSchedulingRequestApprovalEnum& getApprovalStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ApprovalStatus.
           *
           * @return The value of the enumeration identified by ApprovalStatus.
           */
         virtual uci::type::CommSchedulingRequestApprovalEnum& getApprovalStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ApprovalStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setApprovalStatus(const uci::type::CommSchedulingRequestApprovalEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ApprovalStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setApprovalStatus(uci::type::CommSchedulingRequestApprovalEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommSchedulingRequirementID.
           *
           * @return The bounded list identified by CommSchedulingRequirementID.
           */
         virtual const uci::type::CommSchedulingRequestStatusMDT::CommSchedulingRequirementID& getCommSchedulingRequirementID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommSchedulingRequirementID.
           *
           * @return The bounded list identified by CommSchedulingRequirementID.
           */
         virtual uci::type::CommSchedulingRequestStatusMDT::CommSchedulingRequirementID& getCommSchedulingRequirementID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommSchedulingRequirementID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommSchedulingRequirementID(const uci::type::CommSchedulingRequestStatusMDT::CommSchedulingRequirementID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ScheduledComm.
           *
           * @return The bounded list identified by ScheduledComm.
           */
         virtual const uci::type::CommSchedulingRequestStatusMDT::ScheduledComm& getScheduledComm() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ScheduledComm.
           *
           * @return The bounded list identified by ScheduledComm.
           */
         virtual uci::type::CommSchedulingRequestStatusMDT::ScheduledComm& getScheduledComm()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ScheduledComm.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setScheduledComm(const uci::type::CommSchedulingRequestStatusMDT::ScheduledComm& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommSchedulingRequestStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommSchedulingRequestStatusMDT to copy from
           */
         CommSchedulingRequestStatusMDT(const CommSchedulingRequestStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommSchedulingRequestStatusMDT to the contents of the
           * CommSchedulingRequestStatusMDT on the right hand side (rhs) of the assignment operator.CommSchedulingRequestStatusMDT
           * [only available to derived classes]
           *
           * @param rhs The CommSchedulingRequestStatusMDT on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::CommSchedulingRequestStatusMDT
           * @return A constant reference to this CommSchedulingRequestStatusMDT.
           */
         const CommSchedulingRequestStatusMDT& operator=(const CommSchedulingRequestStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommSchedulingRequestStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommSchedulingRequestStatusMDT_h

