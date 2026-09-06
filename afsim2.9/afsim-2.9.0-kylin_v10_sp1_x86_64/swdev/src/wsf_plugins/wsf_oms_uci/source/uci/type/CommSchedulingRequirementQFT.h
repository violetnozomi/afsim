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
#ifndef Uci__Type__CommSchedulingRequirementQFT_h
#define Uci__Type__CommSchedulingRequirementQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommSchedulingRequirementID_Type_h)
# include "uci/type/CommSchedulingRequirementID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__CommSchedulingRequirementStateEnum_h)
# include "uci/type/CommSchedulingRequirementStateEnum.h"
#endif

#if !defined(Uci__Type__CommResourceType_h)
# include "uci/type/CommResourceType.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__TimeCoverageType_h)
# include "uci/type/TimeCoverageType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommSchedulingRequirementQFT sequence accessor class */
      class CommSchedulingRequirementQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~CommSchedulingRequirementQFT()
         { }

         /** Returns this accessor's type constant, i.e. CommSchedulingRequirementQFT
           *
           * @return This accessor's type constant, i.e. CommSchedulingRequirementQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commSchedulingRequirementQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommSchedulingRequirementQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CommSchedulingRequirementID_Type, uci::type::accessorType::commSchedulingRequirementID_Type> CommSchedulingRequirementID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> RequestingSystemID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 8] */
         typedef uci::base::BoundedList<uci::type::CommSchedulingRequirementStateEnum, uci::type::accessorType::commSchedulingRequirementStateEnum> RequirementState;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CommResourceType, uci::type::accessorType::commResourceType> CommResource;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DateTimeRangeType, uci::type::accessorType::dateTimeRangeType> TimeFilter;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::TimeCoverageType, uci::type::accessorType::timeCoverageType> TimeCoverage;

         /** Returns the bounded list that is identified by the CommSchedulingRequirementID.
           *
           * @return The bounded list identified by CommSchedulingRequirementID.
           */
         virtual const uci::type::CommSchedulingRequirementQFT::CommSchedulingRequirementID& getCommSchedulingRequirementID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommSchedulingRequirementID.
           *
           * @return The bounded list identified by CommSchedulingRequirementID.
           */
         virtual uci::type::CommSchedulingRequirementQFT::CommSchedulingRequirementID& getCommSchedulingRequirementID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommSchedulingRequirementID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommSchedulingRequirementID(const uci::type::CommSchedulingRequirementQFT::CommSchedulingRequirementID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RequestingSystemID.
           *
           * @return The bounded list identified by RequestingSystemID.
           */
         virtual const uci::type::CommSchedulingRequirementQFT::RequestingSystemID& getRequestingSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RequestingSystemID.
           *
           * @return The bounded list identified by RequestingSystemID.
           */
         virtual uci::type::CommSchedulingRequirementQFT::RequestingSystemID& getRequestingSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RequestingSystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRequestingSystemID(const uci::type::CommSchedulingRequirementQFT::RequestingSystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RequirementState.
           *
           * @return The bounded list identified by RequirementState.
           */
         virtual const uci::type::CommSchedulingRequirementQFT::RequirementState& getRequirementState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RequirementState.
           *
           * @return The bounded list identified by RequirementState.
           */
         virtual uci::type::CommSchedulingRequirementQFT::RequirementState& getRequirementState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RequirementState.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRequirementState(const uci::type::CommSchedulingRequirementQFT::RequirementState& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommResource.
           *
           * @return The bounded list identified by CommResource.
           */
         virtual const uci::type::CommSchedulingRequirementQFT::CommResource& getCommResource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommResource.
           *
           * @return The bounded list identified by CommResource.
           */
         virtual uci::type::CommSchedulingRequirementQFT::CommResource& getCommResource()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommResource.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommResource(const uci::type::CommSchedulingRequirementQFT::CommResource& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TimeFilter.
           *
           * @return The bounded list identified by TimeFilter.
           */
         virtual const uci::type::CommSchedulingRequirementQFT::TimeFilter& getTimeFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TimeFilter.
           *
           * @return The bounded list identified by TimeFilter.
           */
         virtual uci::type::CommSchedulingRequirementQFT::TimeFilter& getTimeFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TimeFilter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTimeFilter(const uci::type::CommSchedulingRequirementQFT::TimeFilter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TimeCoverage.
           *
           * @return The bounded list identified by TimeCoverage.
           */
         virtual const uci::type::CommSchedulingRequirementQFT::TimeCoverage& getTimeCoverage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TimeCoverage.
           *
           * @return The bounded list identified by TimeCoverage.
           */
         virtual uci::type::CommSchedulingRequirementQFT::TimeCoverage& getTimeCoverage()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TimeCoverage.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTimeCoverage(const uci::type::CommSchedulingRequirementQFT::TimeCoverage& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommSchedulingRequirementQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommSchedulingRequirementQFT to copy from
           */
         CommSchedulingRequirementQFT(const CommSchedulingRequirementQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommSchedulingRequirementQFT to the contents of the
           * CommSchedulingRequirementQFT on the right hand side (rhs) of the assignment operator.CommSchedulingRequirementQFT
           * [only available to derived classes]
           *
           * @param rhs The CommSchedulingRequirementQFT on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::CommSchedulingRequirementQFT
           * @return A constant reference to this CommSchedulingRequirementQFT.
           */
         const CommSchedulingRequirementQFT& operator=(const CommSchedulingRequirementQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommSchedulingRequirementQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommSchedulingRequirementQFT_h

