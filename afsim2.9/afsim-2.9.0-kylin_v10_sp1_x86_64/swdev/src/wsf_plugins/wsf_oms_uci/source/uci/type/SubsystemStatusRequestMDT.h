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
#ifndef Uci__Type__SubsystemStatusRequestMDT_h
#define Uci__Type__SubsystemStatusRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemStatusRequestID_Type_h)
# include "uci/type/SubsystemStatusRequestID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SubsystemStatusRequestMDT sequence accessor class */
      class SubsystemStatusRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SubsystemStatusRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. SubsystemStatusRequestMDT
           *
           * @return This accessor's type constant, i.e. SubsystemStatusRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::subsystemStatusRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SubsystemStatusRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** ID of the Subsystem(s) that should respond to this request. If omitted, it applies to all Subsystems within the
           * specified system (s). [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SubsystemID_Type, uci::type::accessorType::subsystemID_Type> SubsystemID;

         /** ID of the system that should respond to this request. If omitted, it applies to all systems. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> SystemID;

         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemStatusRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemStatusRequestID.
           */
         virtual const uci::type::SubsystemStatusRequestID_Type& getSubsystemStatusRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemStatusRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemStatusRequestID.
           */
         virtual uci::type::SubsystemStatusRequestID_Type& getSubsystemStatusRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubsystemStatusRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubsystemStatusRequestID
           */
         virtual void setSubsystemStatusRequestID(const uci::type::SubsystemStatusRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubsystemID.
           *
           * @return The bounded list identified by SubsystemID.
           */
         virtual const uci::type::SubsystemStatusRequestMDT::SubsystemID& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubsystemID.
           *
           * @return The bounded list identified by SubsystemID.
           */
         virtual uci::type::SubsystemStatusRequestMDT::SubsystemID& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubsystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubsystemID(const uci::type::SubsystemStatusRequestMDT::SubsystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual const uci::type::SubsystemStatusRequestMDT::SystemID& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual uci::type::SubsystemStatusRequestMDT::SystemID& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemID(const uci::type::SubsystemStatusRequestMDT::SystemID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemStatusRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemStatusRequestMDT to copy from
           */
         SubsystemStatusRequestMDT(const SubsystemStatusRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemStatusRequestMDT to the contents of the
           * SubsystemStatusRequestMDT on the right hand side (rhs) of the assignment operator.SubsystemStatusRequestMDT [only
           * available to derived classes]
           *
           * @param rhs The SubsystemStatusRequestMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SubsystemStatusRequestMDT
           * @return A constant reference to this SubsystemStatusRequestMDT.
           */
         const SubsystemStatusRequestMDT& operator=(const SubsystemStatusRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemStatusRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SubsystemStatusRequestMDT_h

