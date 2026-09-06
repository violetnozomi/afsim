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
#ifndef Uci__Type__ApprovalAuthorityRequestMDT_h
#define Uci__Type__ApprovalAuthorityRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ApprovalAuthorityRequestID_Type_h)
# include "uci/type/ApprovalAuthorityRequestID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ApprovalPolicyID_Type_h)
# include "uci/type/ApprovalPolicyID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ApprovalAuthorityRequestMDT sequence accessor class */
      class ApprovalAuthorityRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ApprovalAuthorityRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. ApprovalAuthorityRequestMDT
           *
           * @return This accessor's type constant, i.e. ApprovalAuthorityRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::approvalAuthorityRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ApprovalAuthorityRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique ID of a System over which the requested approval authority would apply. If omitted, the request
           * is applicable to all Systems. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> ApproveeSystemID;

         /** Returns the accessor that provides access to the complex content that is identified by the
           * ApprovalAuthorityRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApprovalAuthorityRequestID.
           */
         virtual const uci::type::ApprovalAuthorityRequestID_Type& getApprovalAuthorityRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ApprovalAuthorityRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApprovalAuthorityRequestID.
           */
         virtual uci::type::ApprovalAuthorityRequestID_Type& getApprovalAuthorityRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ApprovalAuthorityRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ApprovalAuthorityRequestID
           */
         virtual void setApprovalAuthorityRequestID(const uci::type::ApprovalAuthorityRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApproverSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApproverSystemID.
           */
         virtual const uci::type::SystemID_Type& getApproverSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApproverSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApproverSystemID.
           */
         virtual uci::type::SystemID_Type& getApproverSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ApproverSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ApproverSystemID
           */
         virtual void setApproverSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApproverServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApproverServiceID.
           */
         virtual const uci::type::ServiceID_Type& getApproverServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApproverServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApproverServiceID.
           */
         virtual uci::type::ServiceID_Type& getApproverServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ApproverServiceID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ApproverServiceID
           */
         virtual void setApproverServiceID(const uci::type::ServiceID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ApproverServiceID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ApproverServiceID is emabled or not
           */
         virtual bool hasApproverServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ApproverServiceID
           *
           * @param type = uci::type::accessorType::serviceID_Type This Accessor's accessor type
           */
         virtual void enableApproverServiceID(uci::base::accessorType::AccessorType type = uci::type::accessorType::serviceID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ApproverServiceID */
         virtual void clearApproverServiceID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApproveeSystemID.
           *
           * @return The bounded list identified by ApproveeSystemID.
           */
         virtual const uci::type::ApprovalAuthorityRequestMDT::ApproveeSystemID& getApproveeSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApproveeSystemID.
           *
           * @return The bounded list identified by ApproveeSystemID.
           */
         virtual uci::type::ApprovalAuthorityRequestMDT::ApproveeSystemID& getApproveeSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ApproveeSystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setApproveeSystemID(const uci::type::ApprovalAuthorityRequestMDT::ApproveeSystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApprovalPolicyID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApprovalPolicyID.
           */
         virtual const uci::type::ApprovalPolicyID_Type& getApprovalPolicyID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApprovalPolicyID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApprovalPolicyID.
           */
         virtual uci::type::ApprovalPolicyID_Type& getApprovalPolicyID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ApprovalPolicyID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ApprovalPolicyID
           */
         virtual void setApprovalPolicyID(const uci::type::ApprovalPolicyID_Type& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ApprovalAuthorityRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ApprovalAuthorityRequestMDT to copy from
           */
         ApprovalAuthorityRequestMDT(const ApprovalAuthorityRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ApprovalAuthorityRequestMDT to the contents of the
           * ApprovalAuthorityRequestMDT on the right hand side (rhs) of the assignment operator.ApprovalAuthorityRequestMDT [only
           * available to derived classes]
           *
           * @param rhs The ApprovalAuthorityRequestMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ApprovalAuthorityRequestMDT
           * @return A constant reference to this ApprovalAuthorityRequestMDT.
           */
         const ApprovalAuthorityRequestMDT& operator=(const ApprovalAuthorityRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ApprovalAuthorityRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ApprovalAuthorityRequestMDT_h

