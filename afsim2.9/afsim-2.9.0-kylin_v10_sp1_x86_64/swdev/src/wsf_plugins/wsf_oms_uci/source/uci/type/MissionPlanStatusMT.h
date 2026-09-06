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
#ifndef Uci__Type__MissionPlanStatusMT_h
#define Uci__Type__MissionPlanStatusMT_h 1

#if !defined(Uci__Type__MessageType_h)
# include "uci/type/MessageType.h"
#endif

#if !defined(Uci__Type__MessageStateEnum_h)
# include "uci/type/MessageStateEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MissionPlanStatusMDT_h)
# include "uci/type/MissionPlanStatusMDT.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** See the annotation in the associated message for an overall description of the message and this type. */
      class MissionPlanStatusMT : public virtual uci::type::MessageType {
      public:

         /** The destructor */
         virtual ~MissionPlanStatusMT()
         { }

         /** Returns this accessor's type constant, i.e. MissionPlanStatusMT
           *
           * @return This accessor's type constant, i.e. MissionPlanStatusMT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionPlanStatusMT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionPlanStatusMT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Multiple instances allows aggregation of MissionPlanStatus messages. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::MissionPlanStatusMDT, uci::type::accessorType::missionPlanStatusMDT> MessageData;

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


         /** Returns the bounded list that is identified by the MessageData.
           *
           * @return The bounded list identified by MessageData.
           */
         virtual const uci::type::MissionPlanStatusMT::MessageData& getMessageData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MessageData.
           *
           * @return The bounded list identified by MessageData.
           */
         virtual uci::type::MissionPlanStatusMT::MessageData& getMessageData()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MessageData.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMessageData(const uci::type::MissionPlanStatusMT::MessageData& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionPlanStatusMT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionPlanStatusMT to copy from
           */
         MissionPlanStatusMT(const MissionPlanStatusMT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionPlanStatusMT to the contents of the MissionPlanStatusMT on
           * the right hand side (rhs) of the assignment operator.MissionPlanStatusMT [only available to derived classes]
           *
           * @param rhs The MissionPlanStatusMT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::MissionPlanStatusMT
           * @return A constant reference to this MissionPlanStatusMT.
           */
         const MissionPlanStatusMT& operator=(const MissionPlanStatusMT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionPlanStatusMT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionPlanStatusMT_h

