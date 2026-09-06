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
#ifndef Uci__Type__PlannedCommActionType_h
#define Uci__Type__PlannedCommActionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommConfigurationID_Type_h)
# include "uci/type/CommConfigurationID_Type.h"
#endif

#if !defined(Uci__Type__MessageStateEnum_h)
# include "uci/type/MessageStateEnum.h"
#endif

#if !defined(Uci__Type__PlannedSubsystemCommandType_h)
# include "uci/type/PlannedSubsystemCommandType.h"
#endif

#if !defined(Uci__Type__PlannedCommCapabilityType_h)
# include "uci/type/PlannedCommCapabilityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PlannedCommActionType sequence accessor class */
      class PlannedCommActionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PlannedCommActionType()
         { }

         /** Returns this accessor's type constant, i.e. PlannedCommActionType
           *
           * @return This accessor's type constant, i.e. PlannedCommActionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::plannedCommActionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PlannedCommActionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConfigurationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConfigurationID.
           */
         virtual const uci::type::CommConfigurationID_Type& getConfigurationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConfigurationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConfigurationID.
           */
         virtual uci::type::CommConfigurationID_Type& getConfigurationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ConfigurationID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ConfigurationID
           */
         virtual void setConfigurationID(const uci::type::CommConfigurationID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConfigurationState.
           *
           * @return The value of the enumeration identified by ConfigurationState.
           */
         virtual const uci::type::MessageStateEnum& getConfigurationState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConfigurationState.
           *
           * @return The value of the enumeration identified by ConfigurationState.
           */
         virtual uci::type::MessageStateEnum& getConfigurationState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConfigurationState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConfigurationState(const uci::type::MessageStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConfigurationState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConfigurationState(uci::type::MessageStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlannedSubsystemCommand.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlannedSubsystemCommand.
           */
         virtual const uci::type::PlannedSubsystemCommandType& getPlannedSubsystemCommand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlannedSubsystemCommand.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlannedSubsystemCommand.
           */
         virtual uci::type::PlannedSubsystemCommandType& getPlannedSubsystemCommand()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PlannedSubsystemCommand to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PlannedSubsystemCommand
           */
         virtual void setPlannedSubsystemCommand(const uci::type::PlannedSubsystemCommandType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PlannedSubsystemCommand exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PlannedSubsystemCommand is emabled or not
           */
         virtual bool hasPlannedSubsystemCommand() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PlannedSubsystemCommand
           *
           * @param type = uci::type::accessorType::plannedSubsystemCommandType This Accessor's accessor type
           */
         virtual void enablePlannedSubsystemCommand(uci::base::accessorType::AccessorType type = uci::type::accessorType::plannedSubsystemCommandType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PlannedSubsystemCommand */
         virtual void clearPlannedSubsystemCommand()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlannedCommCapability.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlannedCommCapability.
           */
         virtual const uci::type::PlannedCommCapabilityType& getPlannedCommCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlannedCommCapability.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlannedCommCapability.
           */
         virtual uci::type::PlannedCommCapabilityType& getPlannedCommCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PlannedCommCapability to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PlannedCommCapability
           */
         virtual void setPlannedCommCapability(const uci::type::PlannedCommCapabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PlannedCommCapability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PlannedCommCapability is emabled or not
           */
         virtual bool hasPlannedCommCapability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PlannedCommCapability
           *
           * @param type = uci::type::accessorType::plannedCommCapabilityType This Accessor's accessor type
           */
         virtual void enablePlannedCommCapability(uci::base::accessorType::AccessorType type = uci::type::accessorType::plannedCommCapabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PlannedCommCapability */
         virtual void clearPlannedCommCapability()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PlannedCommActionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PlannedCommActionType to copy from
           */
         PlannedCommActionType(const PlannedCommActionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PlannedCommActionType to the contents of the PlannedCommActionType
           * on the right hand side (rhs) of the assignment operator.PlannedCommActionType [only available to derived classes]
           *
           * @param rhs The PlannedCommActionType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::PlannedCommActionType
           * @return A constant reference to this PlannedCommActionType.
           */
         const PlannedCommActionType& operator=(const PlannedCommActionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PlannedCommActionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PlannedCommActionType_h

