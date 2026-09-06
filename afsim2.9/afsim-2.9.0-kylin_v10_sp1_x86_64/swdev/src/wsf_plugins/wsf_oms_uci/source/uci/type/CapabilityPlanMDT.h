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
#ifndef Uci__Type__CapabilityPlanMDT_h
#define Uci__Type__CapabilityPlanMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityPlanID_Type_h)
# include "uci/type/CapabilityPlanID_Type.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityPlanCommandType_h)
# include "uci/type/CapabilityPlanCommandType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CapabilityPlanMDT sequence accessor class */
      class CapabilityPlanMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CapabilityPlanMDT()
         { }

         /** Returns this accessor's type constant, i.e. CapabilityPlanMDT
           *
           * @return This accessor's type constant, i.e. CapabilityPlanMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::capabilityPlanMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CapabilityPlanMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a Capability command that is planned to occur. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityPlanCommandType, uci::type::accessorType::capabilityPlanCommandType> Command;

         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityPlanID.
           */
         virtual const uci::type::CapabilityPlanID_Type& getCapabilityPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityPlanID.
           */
         virtual uci::type::CapabilityPlanID_Type& getCapabilityPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityPlanID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityPlanID
           */
         virtual void setCapabilityPlanID(const uci::type::CapabilityPlanID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual const uci::type::MissionPlanID_Type& getMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual uci::type::MissionPlanID_Type& getMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionPlanID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionPlanID
           */
         virtual void setMissionPlanID(const uci::type::MissionPlanID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Command.
           *
           * @return The bounded list identified by Command.
           */
         virtual const uci::type::CapabilityPlanMDT::Command& getCommand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Command.
           *
           * @return The bounded list identified by Command.
           */
         virtual uci::type::CapabilityPlanMDT::Command& getCommand()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Command.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommand(const uci::type::CapabilityPlanMDT::Command& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityPlanMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityPlanMDT to copy from
           */
         CapabilityPlanMDT(const CapabilityPlanMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityPlanMDT to the contents of the CapabilityPlanMDT on the
           * right hand side (rhs) of the assignment operator.CapabilityPlanMDT [only available to derived classes]
           *
           * @param rhs The CapabilityPlanMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::CapabilityPlanMDT
           * @return A constant reference to this CapabilityPlanMDT.
           */
         const CapabilityPlanMDT& operator=(const CapabilityPlanMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityPlanMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CapabilityPlanMDT_h

