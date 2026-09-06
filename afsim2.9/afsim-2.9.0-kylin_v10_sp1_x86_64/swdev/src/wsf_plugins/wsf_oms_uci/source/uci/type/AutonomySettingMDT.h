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
#ifndef Uci__Type__AutonomySettingMDT_h
#define Uci__Type__AutonomySettingMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AutonomySettingID_Type_h)
# include "uci/type/AutonomySettingID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__MissionPlanningAutonomySettingType_h)
# include "uci/type/MissionPlanningAutonomySettingType.h"
#endif

#if !defined(Uci__Type__MissionPlanActivationCommandEnum_h)
# include "uci/type/MissionPlanActivationCommandEnum.h"
#endif

#if !defined(Uci__Type__ContingencyPathAutonomyType_h)
# include "uci/type/ContingencyPathAutonomyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AutonomySettingMDT sequence accessor class */
      class AutonomySettingMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AutonomySettingMDT()
         { }

         /** Returns this accessor's type constant, i.e. AutonomySettingMDT
           *
           * @return This accessor's type constant, i.e. AutonomySettingMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::autonomySettingMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AutonomySettingMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique ID of a System which the autonomy settings command is applicable to. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> ApplicableSystemID;

         /** Indicates commanded settings for autonomous mission planning capabilities of the System. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MissionPlanningAutonomySettingType, uci::type::accessorType::missionPlanningAutonomySettingType> MissionPlanningAutonomy;

         /** Indicates commanded settings for autonomous mission plan activation (upload, conversion, etc.) features of the
           * System. [Minimum occurrences: 0] [Maximum occurrences: 5]
           */
         typedef uci::base::BoundedList<uci::type::MissionPlanActivationCommandEnum, uci::type::accessorType::missionPlanActivationCommandEnum> MissionPlanActivationAutonomy;

         /** Indicates commanded setting s for the autonomous generation of contingency paths along the primary path. If multiple
           * instances are given, each must be of a different path type as indicated by the child element. [Minimum occurrences:
           * 0] [Maximum occurrences: 11]
           */
         typedef uci::base::BoundedList<uci::type::ContingencyPathAutonomyType, uci::type::accessorType::contingencyPathAutonomyType> ContingencyPath;

         /** Returns the accessor that provides access to the complex content that is identified by the AutonomySettingID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AutonomySettingID.
           */
         virtual const uci::type::AutonomySettingID_Type& getAutonomySettingID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AutonomySettingID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AutonomySettingID.
           */
         virtual uci::type::AutonomySettingID_Type& getAutonomySettingID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AutonomySettingID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AutonomySettingID
           */
         virtual void setAutonomySettingID(const uci::type::AutonomySettingID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableSystemID.
           *
           * @return The bounded list identified by ApplicableSystemID.
           */
         virtual const uci::type::AutonomySettingMDT::ApplicableSystemID& getApplicableSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableSystemID.
           *
           * @return The bounded list identified by ApplicableSystemID.
           */
         virtual uci::type::AutonomySettingMDT::ApplicableSystemID& getApplicableSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ApplicableSystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setApplicableSystemID(const uci::type::AutonomySettingMDT::ApplicableSystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TransitionDroppedToFailed.
           *
           * @return The value of the simple primitive data type identified by TransitionDroppedToFailed.
           */
         virtual xs::Boolean getTransitionDroppedToFailed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TransitionDroppedToFailed.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTransitionDroppedToFailed(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanningAutonomy.
           *
           * @return The bounded list identified by MissionPlanningAutonomy.
           */
         virtual const uci::type::AutonomySettingMDT::MissionPlanningAutonomy& getMissionPlanningAutonomy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanningAutonomy.
           *
           * @return The bounded list identified by MissionPlanningAutonomy.
           */
         virtual uci::type::AutonomySettingMDT::MissionPlanningAutonomy& getMissionPlanningAutonomy()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionPlanningAutonomy.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionPlanningAutonomy(const uci::type::AutonomySettingMDT::MissionPlanningAutonomy& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanActivationAutonomy.
           *
           * @return The bounded list identified by MissionPlanActivationAutonomy.
           */
         virtual const uci::type::AutonomySettingMDT::MissionPlanActivationAutonomy& getMissionPlanActivationAutonomy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionPlanActivationAutonomy.
           *
           * @return The bounded list identified by MissionPlanActivationAutonomy.
           */
         virtual uci::type::AutonomySettingMDT::MissionPlanActivationAutonomy& getMissionPlanActivationAutonomy()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionPlanActivationAutonomy.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionPlanActivationAutonomy(const uci::type::AutonomySettingMDT::MissionPlanActivationAutonomy& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ContingencyPath.
           *
           * @return The bounded list identified by ContingencyPath.
           */
         virtual const uci::type::AutonomySettingMDT::ContingencyPath& getContingencyPath() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ContingencyPath.
           *
           * @return The bounded list identified by ContingencyPath.
           */
         virtual uci::type::AutonomySettingMDT::ContingencyPath& getContingencyPath()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ContingencyPath.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setContingencyPath(const uci::type::AutonomySettingMDT::ContingencyPath& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AutonomySettingMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AutonomySettingMDT to copy from
           */
         AutonomySettingMDT(const AutonomySettingMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AutonomySettingMDT to the contents of the AutonomySettingMDT on
           * the right hand side (rhs) of the assignment operator.AutonomySettingMDT [only available to derived classes]
           *
           * @param rhs The AutonomySettingMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::AutonomySettingMDT
           * @return A constant reference to this AutonomySettingMDT.
           */
         const AutonomySettingMDT& operator=(const AutonomySettingMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AutonomySettingMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AutonomySettingMDT_h

