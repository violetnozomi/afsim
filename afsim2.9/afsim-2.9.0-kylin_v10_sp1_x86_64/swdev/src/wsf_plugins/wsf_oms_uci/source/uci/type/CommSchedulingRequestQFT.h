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
#ifndef Uci__Type__CommSchedulingRequestQFT_h
#define Uci__Type__CommSchedulingRequestQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommSchedulingRequestID_Type_h)
# include "uci/type/CommSchedulingRequestID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__CommSchedulingRequestApprovalEnum_h)
# include "uci/type/CommSchedulingRequestApprovalEnum.h"
#endif

#if !defined(Uci__Type__CommResourceType_h)
# include "uci/type/CommResourceType.h"
#endif

#if !defined(Uci__Type__TimeCoverageType_h)
# include "uci/type/TimeCoverageType.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommSchedulingRequestQFT sequence accessor class */
      class CommSchedulingRequestQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~CommSchedulingRequestQFT()
         { }

         /** Returns this accessor's type constant, i.e. CommSchedulingRequestQFT
           *
           * @return This accessor's type constant, i.e. CommSchedulingRequestQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commSchedulingRequestQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommSchedulingRequestQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CommSchedulingRequestID_Type, uci::type::accessorType::commSchedulingRequestID_Type> CommSchedulingRequestID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DateTimeRangeType, uci::type::accessorType::dateTimeRangeType> TimeFilter;

         /** [Minimum occurrences: 0] [Maximum occurrences: 6] */
         typedef uci::base::BoundedList<uci::type::CommSchedulingRequestApprovalEnum, uci::type::accessorType::commSchedulingRequestApprovalEnum> ApprovalStatus;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CommResourceType, uci::type::accessorType::commResourceType> CommResource;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::TimeCoverageType, uci::type::accessorType::timeCoverageType> TimeCoverage;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> ControllingSystemID;

         /** Returns the value of the SimplePrimitive data type that is identified by the IncludeStatus.
           *
           * @return The value of the simple primitive data type identified by IncludeStatus.
           */
         virtual xs::Boolean getIncludeStatus() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IncludeStatus.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIncludeStatus(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommSchedulingRequestID.
           *
           * @return The bounded list identified by CommSchedulingRequestID.
           */
         virtual const uci::type::CommSchedulingRequestQFT::CommSchedulingRequestID& getCommSchedulingRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommSchedulingRequestID.
           *
           * @return The bounded list identified by CommSchedulingRequestID.
           */
         virtual uci::type::CommSchedulingRequestQFT::CommSchedulingRequestID& getCommSchedulingRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommSchedulingRequestID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommSchedulingRequestID(const uci::type::CommSchedulingRequestQFT::CommSchedulingRequestID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TimeFilter.
           *
           * @return The bounded list identified by TimeFilter.
           */
         virtual const uci::type::CommSchedulingRequestQFT::TimeFilter& getTimeFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TimeFilter.
           *
           * @return The bounded list identified by TimeFilter.
           */
         virtual uci::type::CommSchedulingRequestQFT::TimeFilter& getTimeFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TimeFilter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTimeFilter(const uci::type::CommSchedulingRequestQFT::TimeFilter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApprovalStatus.
           *
           * @return The bounded list identified by ApprovalStatus.
           */
         virtual const uci::type::CommSchedulingRequestQFT::ApprovalStatus& getApprovalStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApprovalStatus.
           *
           * @return The bounded list identified by ApprovalStatus.
           */
         virtual uci::type::CommSchedulingRequestQFT::ApprovalStatus& getApprovalStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ApprovalStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setApprovalStatus(const uci::type::CommSchedulingRequestQFT::ApprovalStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommResource.
           *
           * @return The bounded list identified by CommResource.
           */
         virtual const uci::type::CommSchedulingRequestQFT::CommResource& getCommResource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommResource.
           *
           * @return The bounded list identified by CommResource.
           */
         virtual uci::type::CommSchedulingRequestQFT::CommResource& getCommResource()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommResource.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommResource(const uci::type::CommSchedulingRequestQFT::CommResource& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TimeCoverage.
           *
           * @return The bounded list identified by TimeCoverage.
           */
         virtual const uci::type::CommSchedulingRequestQFT::TimeCoverage& getTimeCoverage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TimeCoverage.
           *
           * @return The bounded list identified by TimeCoverage.
           */
         virtual uci::type::CommSchedulingRequestQFT::TimeCoverage& getTimeCoverage()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TimeCoverage.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTimeCoverage(const uci::type::CommSchedulingRequestQFT::TimeCoverage& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ControllingSystemID.
           *
           * @return The bounded list identified by ControllingSystemID.
           */
         virtual const uci::type::CommSchedulingRequestQFT::ControllingSystemID& getControllingSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ControllingSystemID.
           *
           * @return The bounded list identified by ControllingSystemID.
           */
         virtual uci::type::CommSchedulingRequestQFT::ControllingSystemID& getControllingSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ControllingSystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setControllingSystemID(const uci::type::CommSchedulingRequestQFT::ControllingSystemID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommSchedulingRequestQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommSchedulingRequestQFT to copy from
           */
         CommSchedulingRequestQFT(const CommSchedulingRequestQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommSchedulingRequestQFT to the contents of the
           * CommSchedulingRequestQFT on the right hand side (rhs) of the assignment operator.CommSchedulingRequestQFT [only
           * available to derived classes]
           *
           * @param rhs The CommSchedulingRequestQFT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommSchedulingRequestQFT
           * @return A constant reference to this CommSchedulingRequestQFT.
           */
         const CommSchedulingRequestQFT& operator=(const CommSchedulingRequestQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommSchedulingRequestQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommSchedulingRequestQFT_h

