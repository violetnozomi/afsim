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
#ifndef Uci__Type__QueryRequestStatusMDT_h
#define Uci__Type__QueryRequestStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__QueryRequestID_Type_h)
# include "uci/type/QueryRequestID_Type.h"
#endif

#if !defined(Uci__Type__ProcessingStatusEnum_h)
# include "uci/type/ProcessingStatusEnum.h"
#endif

#if !defined(Uci__Type__CompletionStatusType_h)
# include "uci/type/CompletionStatusType.h"
#endif

#if !defined(Uci__Type__QueryResultType_h)
# include "uci/type/QueryResultType.h"
#endif

#if !defined(Uci__Type__QueryPaginationType_h)
# include "uci/type/QueryPaginationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the QueryRequestStatusMDT sequence accessor class */
      class QueryRequestStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~QueryRequestStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. QueryRequestStatusMDT
           *
           * @return This accessor's type constant, i.e. QueryRequestStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::queryRequestStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const QueryRequestStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the QueryRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by QueryRequestID.
           */
         virtual const uci::type::QueryRequestID_Type& getQueryRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the QueryRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by QueryRequestID.
           */
         virtual uci::type::QueryRequestID_Type& getQueryRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the QueryRequestID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by QueryRequestID
           */
         virtual void setQueryRequestID(const uci::type::QueryRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the QueryState.
           *
           * @return The value of the enumeration identified by QueryState.
           */
         virtual const uci::type::ProcessingStatusEnum& getQueryState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the QueryState.
           *
           * @return The value of the enumeration identified by QueryState.
           */
         virtual uci::type::ProcessingStatusEnum& getQueryState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the QueryState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setQueryState(const uci::type::ProcessingStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the QueryState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setQueryState(uci::type::ProcessingStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CompletionStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CompletionStatus.
           */
         virtual const uci::type::CompletionStatusType& getCompletionStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CompletionStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CompletionStatus.
           */
         virtual uci::type::CompletionStatusType& getCompletionStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CompletionStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CompletionStatus
           */
         virtual void setCompletionStatus(const uci::type::CompletionStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CompletionStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CompletionStatus is emabled or not
           */
         virtual bool hasCompletionStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CompletionStatus
           *
           * @param type = uci::type::accessorType::completionStatusType This Accessor's accessor type
           */
         virtual void enableCompletionStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::completionStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CompletionStatus */
         virtual void clearCompletionStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Result.
           *
           * @return The acecssor that provides access to the complex content that is identified by Result.
           */
         virtual const uci::type::QueryResultType& getResult() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Result.
           *
           * @return The acecssor that provides access to the complex content that is identified by Result.
           */
         virtual uci::type::QueryResultType& getResult()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Result to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Result
           */
         virtual void setResult(const uci::type::QueryResultType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Result exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Result is emabled or not
           */
         virtual bool hasResult() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Result
           *
           * @param type = uci::type::accessorType::queryResultType This Accessor's accessor type
           */
         virtual void enableResult(uci::base::accessorType::AccessorType type = uci::type::accessorType::queryResultType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Result */
         virtual void clearResult()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the QueryPagination.
           *
           * @return The acecssor that provides access to the complex content that is identified by QueryPagination.
           */
         virtual const uci::type::QueryPaginationType& getQueryPagination() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the QueryPagination.
           *
           * @return The acecssor that provides access to the complex content that is identified by QueryPagination.
           */
         virtual uci::type::QueryPaginationType& getQueryPagination()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the QueryPagination to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by QueryPagination
           */
         virtual void setQueryPagination(const uci::type::QueryPaginationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by QueryPagination exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by QueryPagination is emabled or not
           */
         virtual bool hasQueryPagination() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by QueryPagination
           *
           * @param type = uci::type::accessorType::queryPaginationType This Accessor's accessor type
           */
         virtual void enableQueryPagination(uci::base::accessorType::AccessorType type = uci::type::accessorType::queryPaginationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by QueryPagination */
         virtual void clearQueryPagination()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         QueryRequestStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The QueryRequestStatusMDT to copy from
           */
         QueryRequestStatusMDT(const QueryRequestStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this QueryRequestStatusMDT to the contents of the QueryRequestStatusMDT
           * on the right hand side (rhs) of the assignment operator.QueryRequestStatusMDT [only available to derived classes]
           *
           * @param rhs The QueryRequestStatusMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::QueryRequestStatusMDT
           * @return A constant reference to this QueryRequestStatusMDT.
           */
         const QueryRequestStatusMDT& operator=(const QueryRequestStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // QueryRequestStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__QueryRequestStatusMDT_h

