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
#ifndef Uci__Type__AMTI_CommandStatusMT_h
#define Uci__Type__AMTI_CommandStatusMT_h 1

#if !defined(Uci__Type__MessageType_h)
# include "uci/type/MessageType.h"
#endif

#if !defined(Uci__Type__MessageStateEnum_h)
# include "uci/type/MessageStateEnum.h"
#endif

#if !defined(Uci__Type__CapabilityCommandStatusMDT_h)
# include "uci/type/CapabilityCommandStatusMDT.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AMTI_CommandStatusMT sequence accessor class */
      class AMTI_CommandStatusMT : public virtual uci::type::MessageType {
      public:

         /** The destructor */
         virtual ~AMTI_CommandStatusMT()
         { }

         /** Returns this accessor's type constant, i.e. AMTI_CommandStatusMT
           *
           * @return This accessor's type constant, i.e. AMTI_CommandStatusMT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aMTI_CommandStatusMT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AMTI_CommandStatusMT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MessageState.
           *
           * @return The value of the enumeration identified by MessageState.
           */
         virtual const uci::type::MessageStateEnum& getMessageState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MessageState.
           *
           * @return The value of the enumeration identified by MessageState.
           */
         virtual uci::type::MessageStateEnum& getMessageState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MessageState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMessageState(const uci::type::MessageStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MessageState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMessageState(uci::type::MessageStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MessageData.
           *
           * @return The acecssor that provides access to the complex content that is identified by MessageData.
           */
         virtual const uci::type::CapabilityCommandStatusMDT& getMessageData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MessageData.
           *
           * @return The acecssor that provides access to the complex content that is identified by MessageData.
           */
         virtual uci::type::CapabilityCommandStatusMDT& getMessageData()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MessageData to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MessageData
           */
         virtual void setMessageData(const uci::type::CapabilityCommandStatusMDT& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AMTI_CommandStatusMT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AMTI_CommandStatusMT to copy from
           */
         AMTI_CommandStatusMT(const AMTI_CommandStatusMT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AMTI_CommandStatusMT to the contents of the AMTI_CommandStatusMT
           * on the right hand side (rhs) of the assignment operator.AMTI_CommandStatusMT [only available to derived classes]
           *
           * @param rhs The AMTI_CommandStatusMT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::AMTI_CommandStatusMT
           * @return A constant reference to this AMTI_CommandStatusMT.
           */
         const AMTI_CommandStatusMT& operator=(const AMTI_CommandStatusMT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AMTI_CommandStatusMT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AMTI_CommandStatusMT_h

