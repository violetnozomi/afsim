// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__MessageTransmissionListMDT_h
#define Uci__Type__MessageTransmissionListMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__MessageTransmissionListType_h)
# include "uci/type/MessageTransmissionListType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MessageTransmissionListMDT sequence accessor class */
      class MessageTransmissionListMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MessageTransmissionListMDT()
         { }

         /** Returns this accessor's type constant, i.e. MessageTransmissionListMDT
           *
           * @return This accessor's type constant, i.e. MessageTransmissionListMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::messageTransmissionListMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MessageTransmissionListMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** The system to which the transmission list applies. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> SystemID;

         /** The list of message types and associated publishing details that should be published to external UCI systems.
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MessageTransmissionListType, uci::type::accessorType::messageTransmissionListType> MessageList;

         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual const uci::type::MessageTransmissionListMDT::SystemID& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual uci::type::MessageTransmissionListMDT::SystemID& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemID(const uci::type::MessageTransmissionListMDT::SystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MessageList.
           *
           * @return The bounded list identified by MessageList.
           */
         virtual const uci::type::MessageTransmissionListMDT::MessageList& getMessageList() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MessageList.
           *
           * @return The bounded list identified by MessageList.
           */
         virtual uci::type::MessageTransmissionListMDT::MessageList& getMessageList()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MessageList.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMessageList(const uci::type::MessageTransmissionListMDT::MessageList& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MessageTransmissionListMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MessageTransmissionListMDT to copy from
           */
         MessageTransmissionListMDT(const MessageTransmissionListMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MessageTransmissionListMDT to the contents of the
           * MessageTransmissionListMDT on the right hand side (rhs) of the assignment operator.MessageTransmissionListMDT [only
           * available to derived classes]
           *
           * @param rhs The MessageTransmissionListMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::MessageTransmissionListMDT
           * @return A constant reference to this MessageTransmissionListMDT.
           */
         const MessageTransmissionListMDT& operator=(const MessageTransmissionListMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MessageTransmissionListMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MessageTransmissionListMDT_h

