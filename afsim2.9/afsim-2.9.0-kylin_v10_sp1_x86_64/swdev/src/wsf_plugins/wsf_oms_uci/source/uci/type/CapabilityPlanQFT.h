// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__CapabilityPlanQFT_h
#define Uci__Type__CapabilityPlanQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityPlanID_Type_h)
# include "uci/type/CapabilityPlanID_Type.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__SupportCapabilityID_Type_h)
# include "uci/type/SupportCapabilityID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityCommandID_Type_h)
# include "uci/type/CapabilityCommandID_Type.h"
#endif

#if !defined(Uci__Type__SupportCapabilityCommandID_Type_h)
# include "uci/type/SupportCapabilityCommandID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CapabilityPlanQFT sequence accessor class */
      class CapabilityPlanQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~CapabilityPlanQFT()
         { }

         /** Returns this accessor's type constant, i.e. CapabilityPlanQFT
           *
           * @return This accessor's type constant, i.e. CapabilityPlanQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::capabilityPlanQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CapabilityPlanQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter on the ID of the CapabilityPlan. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CapabilityPlanID_Type, uci::type::accessorType::capabilityPlanID_Type> CapabilityPlanID;

         /** Filter on the ID of the MissionPlan which the CapabilityPlan is associated with. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MissionPlanID_Type, uci::type::accessorType::missionPlanID_Type> MissionPlanID;

         /** Filter on the ID of a Capability included in the CapabilityPlan. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Filter on the ID of a SupportCapability included in the CapabilityPlan. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SupportCapabilityID_Type, uci::type::accessorType::supportCapabilityID_Type> SupportCapabilityID;

         /** Filter on the ID of a [Capability]Command included in the CapabilityPlan. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityCommandID_Type, uci::type::accessorType::capabilityCommandID_Type> CapabilityCommandID;

         /** Filter on the ID of a [SupportCapability]Command included in the CapabilityPlan. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SupportCapabilityCommandID_Type, uci::type::accessorType::supportCapabilityCommandID_Type> SupportCapabilityCommandID;

         /** Returns the bounded list that is identified by the CapabilityPlanID.
           *
           * @return The bounded list identified by CapabilityPlanID.
           */
         virtual const uci::type::CapabilityPlanQFT::CapabilityPlanID& getCapabilityPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityPlanID.
           *
           * @return The bounded list identified by CapabilityPlanID.
           */
         virtual uci::type::CapabilityPlanQFT::CapabilityPlanID& getCapabilityPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityPlanID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityPlanID(const uci::type::CapabilityPlanQFT::CapabilityPlanID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanID.
           *
           * @return The bounded list identified by MissionPlanID.
           */
         virtual const uci::type::CapabilityPlanQFT::MissionPlanID& getMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanID.
           *
           * @return The bounded list identified by MissionPlanID.
           */
         virtual uci::type::CapabilityPlanQFT::MissionPlanID& getMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionPlanID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionPlanID(const uci::type::CapabilityPlanQFT::MissionPlanID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::CapabilityPlanQFT::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::CapabilityPlanQFT::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::CapabilityPlanQFT::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportCapabilityID.
           *
           * @return The bounded list identified by SupportCapabilityID.
           */
         virtual const uci::type::CapabilityPlanQFT::SupportCapabilityID& getSupportCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportCapabilityID.
           *
           * @return The bounded list identified by SupportCapabilityID.
           */
         virtual uci::type::CapabilityPlanQFT::SupportCapabilityID& getSupportCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportCapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportCapabilityID(const uci::type::CapabilityPlanQFT::SupportCapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityCommandID.
           *
           * @return The bounded list identified by CapabilityCommandID.
           */
         virtual const uci::type::CapabilityPlanQFT::CapabilityCommandID& getCapabilityCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityCommandID.
           *
           * @return The bounded list identified by CapabilityCommandID.
           */
         virtual uci::type::CapabilityPlanQFT::CapabilityCommandID& getCapabilityCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityCommandID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityCommandID(const uci::type::CapabilityPlanQFT::CapabilityCommandID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportCapabilityCommandID.
           *
           * @return The bounded list identified by SupportCapabilityCommandID.
           */
         virtual const uci::type::CapabilityPlanQFT::SupportCapabilityCommandID& getSupportCapabilityCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportCapabilityCommandID.
           *
           * @return The bounded list identified by SupportCapabilityCommandID.
           */
         virtual uci::type::CapabilityPlanQFT::SupportCapabilityCommandID& getSupportCapabilityCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportCapabilityCommandID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportCapabilityCommandID(const uci::type::CapabilityPlanQFT::SupportCapabilityCommandID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityPlanQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityPlanQFT to copy from
           */
         CapabilityPlanQFT(const CapabilityPlanQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityPlanQFT to the contents of the CapabilityPlanQFT on the
           * right hand side (rhs) of the assignment operator.CapabilityPlanQFT [only available to derived classes]
           *
           * @param rhs The CapabilityPlanQFT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::CapabilityPlanQFT
           * @return A constant reference to this CapabilityPlanQFT.
           */
         const CapabilityPlanQFT& operator=(const CapabilityPlanQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityPlanQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CapabilityPlanQFT_h

