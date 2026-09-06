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
#ifndef Uci__Type__FaultQFT_h
#define Uci__Type__FaultQFT_h 1

#if !defined(Uci__Type__SubsystemBIT_StatusQFT_h)
# include "uci/type/SubsystemBIT_StatusQFT.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FaultID_Type_h)
# include "uci/type/FaultID_Type.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Type__FaultSeverityEnum_h)
# include "uci/type/FaultSeverityEnum.h"
#endif

#if !defined(Uci__Type__FaultStateEnum_h)
# include "uci/type/FaultStateEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Queries the current fault state in the system based on its registered properties */
      class FaultQFT : public virtual uci::type::SubsystemBIT_StatusQFT {
      public:

         /** The destructor */
         virtual ~FaultQFT()
         { }

         /** Returns this accessor's type constant, i.e. FaultQFT
           *
           * @return This accessor's type constant, i.e. FaultQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::faultQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FaultQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FaultID_Type, uci::type::accessorType::faultID_Type> FaultID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ComponentID_Type, uci::type::accessorType::componentID_Type> ComponentID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ServiceID_Type, uci::type::accessorType::serviceID_Type> ServiceID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 4] */
         typedef uci::base::BoundedList<uci::type::FaultSeverityEnum, uci::type::accessorType::faultSeverityEnum> Severity;

         /** [Minimum occurrences: 0] [Maximum occurrences: 3] */
         typedef uci::base::BoundedList<uci::type::FaultStateEnum, uci::type::accessorType::faultStateEnum> State;

         /** Returns the bounded list that is identified by the FaultID.
           *
           * @return The bounded list identified by FaultID.
           */
         virtual const uci::type::FaultQFT::FaultID& getFaultID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FaultID.
           *
           * @return The bounded list identified by FaultID.
           */
         virtual uci::type::FaultQFT::FaultID& getFaultID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FaultID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFaultID(const uci::type::FaultQFT::FaultID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentID.
           *
           * @return The bounded list identified by ComponentID.
           */
         virtual const uci::type::FaultQFT::ComponentID& getComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentID.
           *
           * @return The bounded list identified by ComponentID.
           */
         virtual uci::type::FaultQFT::ComponentID& getComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ComponentID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setComponentID(const uci::type::FaultQFT::ComponentID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ServiceID.
           *
           * @return The bounded list identified by ServiceID.
           */
         virtual const uci::type::FaultQFT::ServiceID& getServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ServiceID.
           *
           * @return The bounded list identified by ServiceID.
           */
         virtual uci::type::FaultQFT::ServiceID& getServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ServiceID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setServiceID(const uci::type::FaultQFT::ServiceID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Severity.
           *
           * @return The bounded list identified by Severity.
           */
         virtual const uci::type::FaultQFT::Severity& getSeverity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Severity.
           *
           * @return The bounded list identified by Severity.
           */
         virtual uci::type::FaultQFT::Severity& getSeverity()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Severity.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSeverity(const uci::type::FaultQFT::Severity& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the State.
           *
           * @return The bounded list identified by State.
           */
         virtual const uci::type::FaultQFT::State& getState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the State.
           *
           * @return The bounded list identified by State.
           */
         virtual uci::type::FaultQFT::State& getState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the State.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setState(const uci::type::FaultQFT::State& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FaultQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FaultQFT to copy from
           */
         FaultQFT(const FaultQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FaultQFT to the contents of the FaultQFT on the right hand side
           * (rhs) of the assignment operator.FaultQFT [only available to derived classes]
           *
           * @param rhs The FaultQFT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::FaultQFT
           * @return A constant reference to this FaultQFT.
           */
         const FaultQFT& operator=(const FaultQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FaultQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FaultQFT_h

