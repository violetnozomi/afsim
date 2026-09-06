// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__ApprovalRequestStatusQFT_h
#define Uci__Type__ApprovalRequestStatusQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ApprovalRequestID_Type_h)
# include "uci/type/ApprovalRequestID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ApprovalRequestStatusQFT sequence accessor class */
      class ApprovalRequestStatusQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~ApprovalRequestStatusQFT()
         { }

         /** Returns this accessor's type constant, i.e. ApprovalRequestStatusQFT
           *
           * @return This accessor's type constant, i.e. ApprovalRequestStatusQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::approvalRequestStatusQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ApprovalRequestStatusQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ApprovalRequestID_Type, uci::type::accessorType::approvalRequestID_Type> ApprovalRequestID;

         /** Returns the value of the SimplePrimitive data type that is identified by the IncludeRequest.
           *
           * @return The value of the simple primitive data type identified by IncludeRequest.
           */
         virtual xs::Boolean getIncludeRequest() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IncludeRequest.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIncludeRequest(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApprovalRequestID.
           *
           * @return The bounded list identified by ApprovalRequestID.
           */
         virtual const uci::type::ApprovalRequestStatusQFT::ApprovalRequestID& getApprovalRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApprovalRequestID.
           *
           * @return The bounded list identified by ApprovalRequestID.
           */
         virtual uci::type::ApprovalRequestStatusQFT::ApprovalRequestID& getApprovalRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ApprovalRequestID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setApprovalRequestID(const uci::type::ApprovalRequestStatusQFT::ApprovalRequestID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ApprovalRequestStatusQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ApprovalRequestStatusQFT to copy from
           */
         ApprovalRequestStatusQFT(const ApprovalRequestStatusQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ApprovalRequestStatusQFT to the contents of the
           * ApprovalRequestStatusQFT on the right hand side (rhs) of the assignment operator.ApprovalRequestStatusQFT [only
           * available to derived classes]
           *
           * @param rhs The ApprovalRequestStatusQFT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ApprovalRequestStatusQFT
           * @return A constant reference to this ApprovalRequestStatusQFT.
           */
         const ApprovalRequestStatusQFT& operator=(const ApprovalRequestStatusQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ApprovalRequestStatusQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ApprovalRequestStatusQFT_h

