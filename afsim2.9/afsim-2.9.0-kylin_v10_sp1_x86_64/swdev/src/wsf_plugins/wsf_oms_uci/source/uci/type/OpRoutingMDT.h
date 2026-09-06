// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__OpRoutingMDT_h
#define Uci__Type__OpRoutingMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__OpRoutingID_Type_h)
# include "uci/type/OpRoutingID_Type.h"
#endif

#if !defined(Uci__Type__SeparationParametersType_h)
# include "uci/type/SeparationParametersType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SpecificBlueSeparationType_h)
# include "uci/type/SpecificBlueSeparationType.h"
#endif

#if !defined(Uci__Type__SpecificRedSeparationType_h)
# include "uci/type/SpecificRedSeparationType.h"
#endif

#if !defined(Uci__Type__OpDescriptionType_h)
# include "uci/type/OpDescriptionType.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ZoneType_h)
# include "uci/type/ZoneType.h"
#endif

#if !defined(Uci__Type__ScheduleType_h)
# include "uci/type/ScheduleType.h"
#endif

#if !defined(Uci__Type__MissionTraceabilityType_h)
# include "uci/type/MissionTraceabilityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OpRoutingMDT sequence accessor class */
      class OpRoutingMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OpRoutingMDT()
         { }

         /** Returns this accessor's type constant, i.e. OpRoutingMDT
           *
           * @return This accessor's type constant, i.e. OpRoutingMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::opRoutingMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OpRoutingMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Defines parameters to use when deconflicting blue player routes or generating routes to ensure proper separation from
           * other blue players. These parameters apply at the vehicle type level. For example, these parameters would be used to
           * define how close a mission planned vehicle could get to an F-16. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SpecificBlueSeparationType, uci::type::accessorType::specificBlueSeparationType> SpecificBlueSeparation;

         /** Defines parameters to use when deconflicting blue player routes or generating routes to ensure proper separation from
           * other red players. These parameters apply at the vehicle type level. For example, these parameters would be used to
           * define how close a mission planned vehicle could get to an F-16. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SpecificRedSeparationType, uci::type::accessorType::specificRedSeparationType> SpecificRedSeparation;

         /** One or more Systems to which the routing constraints apply. If omitted, the constraints apply to all Systems.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> SystemID;

         /** Returns the accessor that provides access to the complex content that is identified by the OpRoutingID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpRoutingID.
           */
         virtual const uci::type::OpRoutingID_Type& getOpRoutingID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpRoutingID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpRoutingID.
           */
         virtual uci::type::OpRoutingID_Type& getOpRoutingID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OpRoutingID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OpRoutingID
           */
         virtual void setOpRoutingID(const uci::type::OpRoutingID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultBlueSeparation.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultBlueSeparation.
           */
         virtual const uci::type::SeparationParametersType& getDefaultBlueSeparation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultBlueSeparation.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultBlueSeparation.
           */
         virtual uci::type::SeparationParametersType& getDefaultBlueSeparation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DefaultBlueSeparation to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DefaultBlueSeparation
           */
         virtual void setDefaultBlueSeparation(const uci::type::SeparationParametersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DefaultBlueSeparation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DefaultBlueSeparation is emabled or not
           */
         virtual bool hasDefaultBlueSeparation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DefaultBlueSeparation
           *
           * @param type = uci::type::accessorType::separationParametersType This Accessor's accessor type
           */
         virtual void enableDefaultBlueSeparation(uci::base::accessorType::AccessorType type = uci::type::accessorType::separationParametersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DefaultBlueSeparation */
         virtual void clearDefaultBlueSeparation()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultRedSeparation.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultRedSeparation.
           */
         virtual const uci::type::SeparationParametersType& getDefaultRedSeparation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultRedSeparation.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultRedSeparation.
           */
         virtual uci::type::SeparationParametersType& getDefaultRedSeparation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DefaultRedSeparation to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DefaultRedSeparation
           */
         virtual void setDefaultRedSeparation(const uci::type::SeparationParametersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DefaultRedSeparation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DefaultRedSeparation is emabled or not
           */
         virtual bool hasDefaultRedSeparation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DefaultRedSeparation
           *
           * @param type = uci::type::accessorType::separationParametersType This Accessor's accessor type
           */
         virtual void enableDefaultRedSeparation(uci::base::accessorType::AccessorType type = uci::type::accessorType::separationParametersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DefaultRedSeparation */
         virtual void clearDefaultRedSeparation()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificBlueSeparation.
           *
           * @return The bounded list identified by SpecificBlueSeparation.
           */
         virtual const uci::type::OpRoutingMDT::SpecificBlueSeparation& getSpecificBlueSeparation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificBlueSeparation.
           *
           * @return The bounded list identified by SpecificBlueSeparation.
           */
         virtual uci::type::OpRoutingMDT::SpecificBlueSeparation& getSpecificBlueSeparation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SpecificBlueSeparation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSpecificBlueSeparation(const uci::type::OpRoutingMDT::SpecificBlueSeparation& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificRedSeparation.
           *
           * @return The bounded list identified by SpecificRedSeparation.
           */
         virtual const uci::type::OpRoutingMDT::SpecificRedSeparation& getSpecificRedSeparation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificRedSeparation.
           *
           * @return The bounded list identified by SpecificRedSeparation.
           */
         virtual uci::type::OpRoutingMDT::SpecificRedSeparation& getSpecificRedSeparation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SpecificRedSeparation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSpecificRedSeparation(const uci::type::OpRoutingMDT::SpecificRedSeparation& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpDescription.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpDescription.
           */
         virtual const uci::type::OpDescriptionType& getOpDescription() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpDescription.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpDescription.
           */
         virtual uci::type::OpDescriptionType& getOpDescription()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OpDescription to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OpDescription
           */
         virtual void setOpDescription(const uci::type::OpDescriptionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OpDescription exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OpDescription is emabled or not
           */
         virtual bool hasOpDescription() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OpDescription
           *
           * @param type = uci::type::accessorType::opDescriptionType This Accessor's accessor type
           */
         virtual void enableOpDescription(uci::base::accessorType::AccessorType type = uci::type::accessorType::opDescriptionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OpDescription */
         virtual void clearOpDescription()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual const uci::type::OpRoutingMDT::SystemID& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual uci::type::OpRoutingMDT::SystemID& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemID(const uci::type::OpRoutingMDT::SystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApplicableZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApplicableZone.
           */
         virtual const uci::type::ZoneType& getApplicableZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApplicableZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApplicableZone.
           */
         virtual uci::type::ZoneType& getApplicableZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ApplicableZone to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ApplicableZone
           */
         virtual void setApplicableZone(const uci::type::ZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ApplicableZone exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ApplicableZone is emabled or not
           */
         virtual bool hasApplicableZone() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ApplicableZone
           *
           * @param type = uci::type::accessorType::zoneType This Accessor's accessor type
           */
         virtual void enableApplicableZone(uci::base::accessorType::AccessorType type = uci::type::accessorType::zoneType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ApplicableZone */
         virtual void clearApplicableZone()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Schedule.
           *
           * @return The acecssor that provides access to the complex content that is identified by Schedule.
           */
         virtual const uci::type::ScheduleType& getSchedule() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Schedule.
           *
           * @return The acecssor that provides access to the complex content that is identified by Schedule.
           */
         virtual uci::type::ScheduleType& getSchedule()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Schedule to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Schedule
           */
         virtual void setSchedule(const uci::type::ScheduleType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Schedule exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Schedule is emabled or not
           */
         virtual bool hasSchedule() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Schedule
           *
           * @param type = uci::type::accessorType::scheduleType This Accessor's accessor type
           */
         virtual void enableSchedule(uci::base::accessorType::AccessorType type = uci::type::accessorType::scheduleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Schedule */
         virtual void clearSchedule()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionTraceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionTraceability.
           */
         virtual const uci::type::MissionTraceabilityType& getMissionTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionTraceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionTraceability.
           */
         virtual uci::type::MissionTraceabilityType& getMissionTraceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionTraceability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionTraceability
           */
         virtual void setMissionTraceability(const uci::type::MissionTraceabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionTraceability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionTraceability is emabled or not
           */
         virtual bool hasMissionTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionTraceability
           *
           * @param type = uci::type::accessorType::missionTraceabilityType This Accessor's accessor type
           */
         virtual void enableMissionTraceability(uci::base::accessorType::AccessorType type = uci::type::accessorType::missionTraceabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionTraceability */
         virtual void clearMissionTraceability()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OpRoutingMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OpRoutingMDT to copy from
           */
         OpRoutingMDT(const OpRoutingMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OpRoutingMDT to the contents of the OpRoutingMDT on the right hand
           * side (rhs) of the assignment operator.OpRoutingMDT [only available to derived classes]
           *
           * @param rhs The OpRoutingMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::OpRoutingMDT
           * @return A constant reference to this OpRoutingMDT.
           */
         const OpRoutingMDT& operator=(const OpRoutingMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OpRoutingMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OpRoutingMDT_h

