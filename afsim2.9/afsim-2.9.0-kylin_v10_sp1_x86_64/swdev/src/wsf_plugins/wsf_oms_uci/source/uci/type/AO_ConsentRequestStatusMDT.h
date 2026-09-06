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
#ifndef Uci__Type__AO_ConsentRequestStatusMDT_h
#define Uci__Type__AO_ConsentRequestStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ConsentRequestID_Type_h)
# include "uci/type/ConsentRequestID_Type.h"
#endif

#if !defined(Uci__Type__ApprovalEnum_h)
# include "uci/type/ApprovalEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AO_ConsentRequestStatusMDT sequence accessor class */
      class AO_ConsentRequestStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AO_ConsentRequestStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. AO_ConsentRequestStatusMDT
           *
           * @return This accessor's type constant, i.e. AO_ConsentRequestStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aO_ConsentRequestStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AO_ConsentRequestStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConsentRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConsentRequestID.
           */
         virtual const uci::type::ConsentRequestID_Type& getConsentRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConsentRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConsentRequestID.
           */
         virtual uci::type::ConsentRequestID_Type& getConsentRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ConsentRequestID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ConsentRequestID
           */
         virtual void setConsentRequestID(const uci::type::ConsentRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the State.
           *
           * @return The value of the enumeration identified by State.
           */
         virtual const uci::type::ApprovalEnum& getState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the State.
           *
           * @return The value of the enumeration identified by State.
           */
         virtual uci::type::ApprovalEnum& getState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the State.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setState(const uci::type::ApprovalEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the State.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setState(uci::type::ApprovalEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AO_ConsentRequestStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AO_ConsentRequestStatusMDT to copy from
           */
         AO_ConsentRequestStatusMDT(const AO_ConsentRequestStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AO_ConsentRequestStatusMDT to the contents of the
           * AO_ConsentRequestStatusMDT on the right hand side (rhs) of the assignment operator.AO_ConsentRequestStatusMDT [only
           * available to derived classes]
           *
           * @param rhs The AO_ConsentRequestStatusMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::AO_ConsentRequestStatusMDT
           * @return A constant reference to this AO_ConsentRequestStatusMDT.
           */
         const AO_ConsentRequestStatusMDT& operator=(const AO_ConsentRequestStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AO_ConsentRequestStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AO_ConsentRequestStatusMDT_h

