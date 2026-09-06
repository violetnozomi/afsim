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
#ifndef Uci__Type__ApprovalRequestStatusMDT_h
#define Uci__Type__ApprovalRequestStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ApprovalRequestID_Type_h)
# include "uci/type/ApprovalRequestID_Type.h"
#endif

#if !defined(Uci__Type__ApprovalEnum_h)
# include "uci/type/ApprovalEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DMPI_ID_Type_h)
# include "uci/type/DMPI_ID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ApprovalRequestStatusMDT sequence accessor class */
      class ApprovalRequestStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ApprovalRequestStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. ApprovalRequestStatusMDT
           *
           * @return This accessor's type constant, i.e. ApprovalRequestStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::approvalRequestStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ApprovalRequestStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** If the ApprovalRequest was for DMPI approval then this element contains the list of approved DMPIs. If any DMPIs are
           * approved, the sibling ApprovalState will indicate that the request is approved. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DMPI_ID_Type, uci::type::accessorType::dMPI_ID_Type> ApprovedDMPI_ID;

         /** Returns the accessor that provides access to the complex content that is identified by the ApprovalRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApprovalRequestID.
           */
         virtual const uci::type::ApprovalRequestID_Type& getApprovalRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApprovalRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApprovalRequestID.
           */
         virtual uci::type::ApprovalRequestID_Type& getApprovalRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ApprovalRequestID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ApprovalRequestID
           */
         virtual void setApprovalRequestID(const uci::type::ApprovalRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ApprovalState.
           *
           * @return The value of the enumeration identified by ApprovalState.
           */
         virtual const uci::type::ApprovalEnum& getApprovalState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ApprovalState.
           *
           * @return The value of the enumeration identified by ApprovalState.
           */
         virtual uci::type::ApprovalEnum& getApprovalState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ApprovalState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setApprovalState(const uci::type::ApprovalEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ApprovalState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setApprovalState(uci::type::ApprovalEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApprovedDMPI_ID.
           *
           * @return The bounded list identified by ApprovedDMPI_ID.
           */
         virtual const uci::type::ApprovalRequestStatusMDT::ApprovedDMPI_ID& getApprovedDMPI_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApprovedDMPI_ID.
           *
           * @return The bounded list identified by ApprovedDMPI_ID.
           */
         virtual uci::type::ApprovalRequestStatusMDT::ApprovedDMPI_ID& getApprovedDMPI_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ApprovedDMPI_ID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setApprovedDMPI_ID(const uci::type::ApprovalRequestStatusMDT::ApprovedDMPI_ID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ApprovalRequestStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ApprovalRequestStatusMDT to copy from
           */
         ApprovalRequestStatusMDT(const ApprovalRequestStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ApprovalRequestStatusMDT to the contents of the
           * ApprovalRequestStatusMDT on the right hand side (rhs) of the assignment operator.ApprovalRequestStatusMDT [only
           * available to derived classes]
           *
           * @param rhs The ApprovalRequestStatusMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ApprovalRequestStatusMDT
           * @return A constant reference to this ApprovalRequestStatusMDT.
           */
         const ApprovalRequestStatusMDT& operator=(const ApprovalRequestStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ApprovalRequestStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ApprovalRequestStatusMDT_h

