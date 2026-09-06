// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__ThreatTargetPrioritizationListQFT_h
#define Uci__Type__ThreatTargetPrioritizationListQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ThreatTargetPrioritizationListID_Type_h)
# include "uci/type/ThreatTargetPrioritizationListID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ThreatTargetPrioritizationListQFT sequence accessor class */
      class ThreatTargetPrioritizationListQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~ThreatTargetPrioritizationListQFT()
         { }

         /** Returns this accessor's type constant, i.e. ThreatTargetPrioritizationListQFT
           *
           * @return This accessor's type constant, i.e. ThreatTargetPrioritizationListQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::threatTargetPrioritizationListQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ThreatTargetPrioritizationListQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ThreatTargetPrioritizationListID_Type, uci::type::accessorType::threatTargetPrioritizationListID_Type> ThreatTargetPrioritizationListID;

         /** Indicates a query filter on the SystemID which the TTPL applies to. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> ApplicableSystemID;

         /** Indicates a query filter on the ServiceID which the TTPL applies to. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ServiceID_Type, uci::type::accessorType::serviceID_Type> ApplicableServiceID;

         /** Returns the bounded list that is identified by the ThreatTargetPrioritizationListID.
           *
           * @return The bounded list identified by ThreatTargetPrioritizationListID.
           */
         virtual const uci::type::ThreatTargetPrioritizationListQFT::ThreatTargetPrioritizationListID& getThreatTargetPrioritizationListID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ThreatTargetPrioritizationListID.
           *
           * @return The bounded list identified by ThreatTargetPrioritizationListID.
           */
         virtual uci::type::ThreatTargetPrioritizationListQFT::ThreatTargetPrioritizationListID& getThreatTargetPrioritizationListID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ThreatTargetPrioritizationListID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setThreatTargetPrioritizationListID(const uci::type::ThreatTargetPrioritizationListQFT::ThreatTargetPrioritizationListID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableSystemID.
           *
           * @return The bounded list identified by ApplicableSystemID.
           */
         virtual const uci::type::ThreatTargetPrioritizationListQFT::ApplicableSystemID& getApplicableSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableSystemID.
           *
           * @return The bounded list identified by ApplicableSystemID.
           */
         virtual uci::type::ThreatTargetPrioritizationListQFT::ApplicableSystemID& getApplicableSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ApplicableSystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setApplicableSystemID(const uci::type::ThreatTargetPrioritizationListQFT::ApplicableSystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableServiceID.
           *
           * @return The bounded list identified by ApplicableServiceID.
           */
         virtual const uci::type::ThreatTargetPrioritizationListQFT::ApplicableServiceID& getApplicableServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableServiceID.
           *
           * @return The bounded list identified by ApplicableServiceID.
           */
         virtual uci::type::ThreatTargetPrioritizationListQFT::ApplicableServiceID& getApplicableServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ApplicableServiceID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setApplicableServiceID(const uci::type::ThreatTargetPrioritizationListQFT::ApplicableServiceID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ThreatTargetPrioritizationListQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ThreatTargetPrioritizationListQFT to copy from
           */
         ThreatTargetPrioritizationListQFT(const ThreatTargetPrioritizationListQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ThreatTargetPrioritizationListQFT to the contents of the
           * ThreatTargetPrioritizationListQFT on the right hand side (rhs) of the assignment
           * operator.ThreatTargetPrioritizationListQFT [only available to derived classes]
           *
           * @param rhs The ThreatTargetPrioritizationListQFT on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ThreatTargetPrioritizationListQFT
           * @return A constant reference to this ThreatTargetPrioritizationListQFT.
           */
         const ThreatTargetPrioritizationListQFT& operator=(const ThreatTargetPrioritizationListQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ThreatTargetPrioritizationListQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ThreatTargetPrioritizationListQFT_h

