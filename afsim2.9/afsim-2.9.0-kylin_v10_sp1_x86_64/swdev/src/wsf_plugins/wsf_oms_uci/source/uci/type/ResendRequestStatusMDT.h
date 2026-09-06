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
#ifndef Uci__Type__ResendRequestStatusMDT_h
#define Uci__Type__ResendRequestStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ResendRequestID_Type_h)
# include "uci/type/ResendRequestID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MessageType_h)
# include "uci/type/MessageType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ResendRequestStatusMDT sequence accessor class */
      class ResendRequestStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ResendRequestStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. ResendRequestStatusMDT
           *
           * @return This accessor's type constant, i.e. ResendRequestStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::resendRequestStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ResendRequestStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element contains the message(s) (derived from MessageType) that matched the corresponding ResendRequest. If
           * omitted, no matching messages were found. This is a polymorphic extension point; any UCI message (which by definition
           * is derived from MessageType) can be included here. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MessageType, uci::type::accessorType::messageType> Message;

         /** Returns the accessor that provides access to the complex content that is identified by the ResendRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ResendRequestID.
           */
         virtual const uci::type::ResendRequestID_Type& getResendRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ResendRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ResendRequestID.
           */
         virtual uci::type::ResendRequestID_Type& getResendRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ResendRequestID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ResendRequestID
           */
         virtual void setResendRequestID(const uci::type::ResendRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequestingServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequestingServiceID.
           */
         virtual const uci::type::ServiceID_Type& getRequestingServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequestingServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequestingServiceID.
           */
         virtual uci::type::ServiceID_Type& getRequestingServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RequestingServiceID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RequestingServiceID
           */
         virtual void setRequestingServiceID(const uci::type::ServiceID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DestinationServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DestinationServiceID.
           */
         virtual const uci::type::ServiceID_Type& getDestinationServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DestinationServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DestinationServiceID.
           */
         virtual uci::type::ServiceID_Type& getDestinationServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DestinationServiceID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DestinationServiceID
           */
         virtual void setDestinationServiceID(const uci::type::ServiceID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DestinationServiceID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DestinationServiceID is emabled or not
           */
         virtual bool hasDestinationServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DestinationServiceID
           *
           * @param type = uci::type::accessorType::serviceID_Type This Accessor's accessor type
           */
         virtual void enableDestinationServiceID(uci::base::accessorType::AccessorType type = uci::type::accessorType::serviceID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DestinationServiceID */
         virtual void clearDestinationServiceID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Message.
           *
           * @return The bounded list identified by Message.
           */
         virtual const uci::type::ResendRequestStatusMDT::Message& getMessage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Message.
           *
           * @return The bounded list identified by Message.
           */
         virtual uci::type::ResendRequestStatusMDT::Message& getMessage()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Message.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMessage(const uci::type::ResendRequestStatusMDT::Message& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ResendRequestStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ResendRequestStatusMDT to copy from
           */
         ResendRequestStatusMDT(const ResendRequestStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ResendRequestStatusMDT to the contents of the
           * ResendRequestStatusMDT on the right hand side (rhs) of the assignment operator.ResendRequestStatusMDT [only available
           * to derived classes]
           *
           * @param rhs The ResendRequestStatusMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ResendRequestStatusMDT
           * @return A constant reference to this ResendRequestStatusMDT.
           */
         const ResendRequestStatusMDT& operator=(const ResendRequestStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ResendRequestStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ResendRequestStatusMDT_h

