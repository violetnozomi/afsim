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
#ifndef Uci__Type__MissionPlanActivationStateQFT_h
#define Uci__Type__MissionPlanActivationStateQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Type__MissionPlanActivationStateEnum_h)
# include "uci/type/MissionPlanActivationStateEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionPlanActivationStateQFT sequence accessor class */
      class MissionPlanActivationStateQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~MissionPlanActivationStateQFT()
         { }

         /** Returns this accessor's type constant, i.e. MissionPlanActivationStateQFT
           *
           * @return This accessor's type constant, i.e. MissionPlanActivationStateQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionPlanActivationStateQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionPlanActivationStateQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter on the MissionPlan ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::MissionPlanID_Type, uci::type::accessorType::missionPlanID_Type> MissionPlanID;

         /** Filter on the state of the mission plan. [Minimum occurrences: 0] [Maximum occurrences: 8] */
         typedef uci::base::BoundedList<uci::type::MissionPlanActivationStateEnum, uci::type::accessorType::missionPlanActivationStateEnum> State;

         /** Returns the bounded list that is identified by the MissionPlanID.
           *
           * @return The bounded list identified by MissionPlanID.
           */
         virtual const uci::type::MissionPlanActivationStateQFT::MissionPlanID& getMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanID.
           *
           * @return The bounded list identified by MissionPlanID.
           */
         virtual uci::type::MissionPlanActivationStateQFT::MissionPlanID& getMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionPlanID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionPlanID(const uci::type::MissionPlanActivationStateQFT::MissionPlanID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the State.
           *
           * @return The bounded list identified by State.
           */
         virtual const uci::type::MissionPlanActivationStateQFT::State& getState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the State.
           *
           * @return The bounded list identified by State.
           */
         virtual uci::type::MissionPlanActivationStateQFT::State& getState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the State.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setState(const uci::type::MissionPlanActivationStateQFT::State& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionPlanActivationStateQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionPlanActivationStateQFT to copy from
           */
         MissionPlanActivationStateQFT(const MissionPlanActivationStateQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionPlanActivationStateQFT to the contents of the
           * MissionPlanActivationStateQFT on the right hand side (rhs) of the assignment operator.MissionPlanActivationStateQFT
           * [only available to derived classes]
           *
           * @param rhs The MissionPlanActivationStateQFT on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::MissionPlanActivationStateQFT
           * @return A constant reference to this MissionPlanActivationStateQFT.
           */
         const MissionPlanActivationStateQFT& operator=(const MissionPlanActivationStateQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionPlanActivationStateQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionPlanActivationStateQFT_h

