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
#ifndef Uci__Type__SubsystemBIT_StatusMDT_h
#define Uci__Type__SubsystemBIT_StatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SubsystemActiveBIT_Type_h)
# include "uci/type/SubsystemActiveBIT_Type.h"
#endif

#if !defined(Uci__Type__SubsystemCompletedBIT_Type_h)
# include "uci/type/SubsystemCompletedBIT_Type.h"
#endif

#if !defined(Uci__Type__SubsystemFaultType_h)
# include "uci/type/SubsystemFaultType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SubsystemBIT_StatusMDT sequence accessor class */
      class SubsystemBIT_StatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SubsystemBIT_StatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. SubsystemBIT_StatusMDT
           *
           * @return This accessor's type constant, i.e. SubsystemBIT_StatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::subsystemBIT_StatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SubsystemBIT_StatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a BIT that is currently running or enqueued to run. This element can be used for BIT initiated via
           * SubsystemStateCommand, SubsystemBIT_Command and/or by the Subsystem itself (periodic BIT (PBIT), background BIT
           * (BBIT), start-up BIT (SBIT), etc.). [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SubsystemActiveBIT_Type, uci::type::accessorType::subsystemActiveBIT_Type> ActiveBIT;

         /** Indicates results of a previously completed BIT. This element can be used for results from BIT initiated via
           * SubsystemStateCommand, SubsystemBIT_Command and/or self-initiated by the Subsystem. Subsystem self-initiated BIT
           * examples include periodic BIT (PBIT), background BIT (BBIT), startup BIT (SBIT), maintenance BIT (MBIT), etc.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SubsystemCompletedBIT_Type, uci::type::accessorType::subsystemCompletedBIT_Type> CompletedBIT;

         /** Indicates a current fault in the Subsystem. When omitted, the Subsystem has no current faults. Faults aren't
           * necessarily related to or exclusively detected by BIT. This element is independent of the sibling BIT_Results so that
           * faults can be reported regardless of how they were detected or triggered. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SubsystemFaultType, uci::type::accessorType::subsystemFaultType> Fault;

         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual const uci::type::SubsystemID_Type& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual uci::type::SubsystemID_Type& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubsystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubsystemID
           */
         virtual void setSubsystemID(const uci::type::SubsystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActiveBIT.
           *
           * @return The bounded list identified by ActiveBIT.
           */
         virtual const uci::type::SubsystemBIT_StatusMDT::ActiveBIT& getActiveBIT() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActiveBIT.
           *
           * @return The bounded list identified by ActiveBIT.
           */
         virtual uci::type::SubsystemBIT_StatusMDT::ActiveBIT& getActiveBIT()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ActiveBIT.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setActiveBIT(const uci::type::SubsystemBIT_StatusMDT::ActiveBIT& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CompletedBIT.
           *
           * @return The bounded list identified by CompletedBIT.
           */
         virtual const uci::type::SubsystemBIT_StatusMDT::CompletedBIT& getCompletedBIT() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CompletedBIT.
           *
           * @return The bounded list identified by CompletedBIT.
           */
         virtual uci::type::SubsystemBIT_StatusMDT::CompletedBIT& getCompletedBIT()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CompletedBIT.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCompletedBIT(const uci::type::SubsystemBIT_StatusMDT::CompletedBIT& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Fault.
           *
           * @return The bounded list identified by Fault.
           */
         virtual const uci::type::SubsystemBIT_StatusMDT::Fault& getFault() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Fault.
           *
           * @return The bounded list identified by Fault.
           */
         virtual uci::type::SubsystemBIT_StatusMDT::Fault& getFault()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Fault.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFault(const uci::type::SubsystemBIT_StatusMDT::Fault& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemBIT_StatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemBIT_StatusMDT to copy from
           */
         SubsystemBIT_StatusMDT(const SubsystemBIT_StatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemBIT_StatusMDT to the contents of the
           * SubsystemBIT_StatusMDT on the right hand side (rhs) of the assignment operator.SubsystemBIT_StatusMDT [only available
           * to derived classes]
           *
           * @param rhs The SubsystemBIT_StatusMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::SubsystemBIT_StatusMDT
           * @return A constant reference to this SubsystemBIT_StatusMDT.
           */
         const SubsystemBIT_StatusMDT& operator=(const SubsystemBIT_StatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemBIT_StatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SubsystemBIT_StatusMDT_h

