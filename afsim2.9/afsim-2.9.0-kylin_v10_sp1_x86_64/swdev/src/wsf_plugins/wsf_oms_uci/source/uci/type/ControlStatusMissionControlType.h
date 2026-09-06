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
#ifndef Uci__Type__ControlStatusMissionControlType_h
#define Uci__Type__ControlStatusMissionControlType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MissionControlInterfaceEnum_h)
# include "uci/type/MissionControlInterfaceEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__SupportedAutonomySettingsType_h)
# include "uci/type/SupportedAutonomySettingsType.h"
#endif

#if !defined(Uci__Type__SupportedMissionPlanCommandsType_h)
# include "uci/type/SupportedMissionPlanCommandsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ControlStatusMissionControlType sequence accessor class */
      class ControlStatusMissionControlType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ControlStatusMissionControlType()
         { }

         /** Returns this accessor's type constant, i.e. ControlStatusMissionControlType
           *
           * @return This accessor's type constant, i.e. ControlStatusMissionControlType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::controlStatusMissionControlType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ControlStatusMissionControlType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates which mission control interfaces are currently being accepted. See enumeration annotations. A System may
           * accept mission control interfaces/messages before being "in mission" (as indicated by the sibling InMission element).
           * See the sibling Supported* elements for details regarding use/capability of the interface for this System. When
           * omitted, a system is unavailable for mission level control. For example, the system could be temporarily down, not
           * wish to receive new tasking or performing C2 functions only. Finally, a system may only be reported by proxy, for
           * situational awareness purposes, and may not be capable of mission level control as defined by UCI. [Minimum
           * occurrences: 0] [Maximum occurrences: 6]
           */
         typedef uci::base::BoundedList<uci::type::MissionControlInterfaceEnum, uci::type::accessorType::missionControlInterfaceEnum> AcceptedInterface;

         /** Returns the accessor that provides access to the complex content that is identified by the ControllerSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControllerSystemID.
           */
         virtual const uci::type::SystemID_Type& getControllerSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ControllerSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControllerSystemID.
           */
         virtual uci::type::SystemID_Type& getControllerSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ControllerSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ControllerSystemID
           */
         virtual void setControllerSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual const uci::type::ControlStatusMissionControlType::AcceptedInterface& getAcceptedInterface() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual uci::type::ControlStatusMissionControlType::AcceptedInterface& getAcceptedInterface()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AcceptedInterface.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAcceptedInterface(const uci::type::ControlStatusMissionControlType::AcceptedInterface& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the InMission.
           *
           * @return The value of the simple primitive data type identified by InMission.
           */
         virtual xs::Boolean getInMission() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InMission.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInMission(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SupportedAutonomySettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportedAutonomySettings.
           */
         virtual const uci::type::SupportedAutonomySettingsType& getSupportedAutonomySettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SupportedAutonomySettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportedAutonomySettings.
           */
         virtual uci::type::SupportedAutonomySettingsType& getSupportedAutonomySettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SupportedAutonomySettings to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SupportedAutonomySettings
           */
         virtual void setSupportedAutonomySettings(const uci::type::SupportedAutonomySettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SupportedAutonomySettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SupportedAutonomySettings is emabled or not
           */
         virtual bool hasSupportedAutonomySettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SupportedAutonomySettings
           *
           * @param type = uci::type::accessorType::supportedAutonomySettingsType This Accessor's accessor type
           */
         virtual void enableSupportedAutonomySettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::supportedAutonomySettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SupportedAutonomySettings */
         virtual void clearSupportedAutonomySettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * SupportedMissionPlanCommands.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportedMissionPlanCommands.
           */
         virtual const uci::type::SupportedMissionPlanCommandsType& getSupportedMissionPlanCommands() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * SupportedMissionPlanCommands.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportedMissionPlanCommands.
           */
         virtual uci::type::SupportedMissionPlanCommandsType& getSupportedMissionPlanCommands()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SupportedMissionPlanCommands to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SupportedMissionPlanCommands
           */
         virtual void setSupportedMissionPlanCommands(const uci::type::SupportedMissionPlanCommandsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SupportedMissionPlanCommands exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SupportedMissionPlanCommands is emabled or not
           */
         virtual bool hasSupportedMissionPlanCommands() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SupportedMissionPlanCommands
           *
           * @param type = uci::type::accessorType::supportedMissionPlanCommandsType This Accessor's accessor type
           */
         virtual void enableSupportedMissionPlanCommands(uci::base::accessorType::AccessorType type = uci::type::accessorType::supportedMissionPlanCommandsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SupportedMissionPlanCommands */
         virtual void clearSupportedMissionPlanCommands()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ControlStatusMissionControlType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ControlStatusMissionControlType to copy from
           */
         ControlStatusMissionControlType(const ControlStatusMissionControlType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ControlStatusMissionControlType to the contents of the
           * ControlStatusMissionControlType on the right hand side (rhs) of the assignment
           * operator.ControlStatusMissionControlType [only available to derived classes]
           *
           * @param rhs The ControlStatusMissionControlType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ControlStatusMissionControlType
           * @return A constant reference to this ControlStatusMissionControlType.
           */
         const ControlStatusMissionControlType& operator=(const ControlStatusMissionControlType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ControlStatusMissionControlType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ControlStatusMissionControlType_h

