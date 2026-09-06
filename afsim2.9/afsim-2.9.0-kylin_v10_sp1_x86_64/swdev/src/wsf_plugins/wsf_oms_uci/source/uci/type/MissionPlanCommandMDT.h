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
#ifndef Uci__Type__MissionPlanCommandMDT_h
#define Uci__Type__MissionPlanCommandMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MissionPlanCommandID_Type_h)
# include "uci/type/MissionPlanCommandID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__ReplanReasonType_h)
# include "uci/type/ReplanReasonType.h"
#endif

#if !defined(Uci__Type__MissionPlanCommandType_h)
# include "uci/type/MissionPlanCommandType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MissionEnvironmentConstraintType_h)
# include "uci/type/MissionEnvironmentConstraintType.h"
#endif

#if !defined(Uci__Type__RoutesType_h)
# include "uci/type/RoutesType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionPlanCommandMDT sequence accessor class */
      class MissionPlanCommandMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MissionPlanCommandMDT()
         { }

         /** Returns this accessor's type constant, i.e. MissionPlanCommandMDT
           *
           * @return This accessor's type constant, i.e. MissionPlanCommandMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionPlanCommandMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionPlanCommandMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a selective override of an item in the mission environment world state, only in the context of the sibling
           * Command. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MissionEnvironmentConstraintType, uci::type::accessorType::missionEnvironmentConstraintType> EnvironmentOverride;

         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanCommandID.
           */
         virtual const uci::type::MissionPlanCommandID_Type& getMissionPlanCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanCommandID.
           */
         virtual uci::type::MissionPlanCommandID_Type& getMissionPlanCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionPlanCommandID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionPlanCommandID
           */
         virtual void setMissionPlanCommandID(const uci::type::MissionPlanCommandID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ParentMissionPlanCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ParentMissionPlanCommandID.
           */
         virtual const uci::type::MissionPlanCommandID_Type& getParentMissionPlanCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ParentMissionPlanCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ParentMissionPlanCommandID.
           */
         virtual uci::type::MissionPlanCommandID_Type& getParentMissionPlanCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ParentMissionPlanCommandID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ParentMissionPlanCommandID
           */
         virtual void setParentMissionPlanCommandID(const uci::type::MissionPlanCommandID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ParentMissionPlanCommandID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ParentMissionPlanCommandID is emabled or not
           */
         virtual bool hasParentMissionPlanCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ParentMissionPlanCommandID
           *
           * @param type = uci::type::accessorType::missionPlanCommandID_Type This Accessor's accessor type
           */
         virtual void enableParentMissionPlanCommandID(uci::base::accessorType::AccessorType type = uci::type::accessorType::missionPlanCommandID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ParentMissionPlanCommandID */
         virtual void clearParentMissionPlanCommandID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ResultsInNativeMessage.
           *
           * @return The value of the simple primitive data type identified by ResultsInNativeMessage.
           */
         virtual xs::Boolean getResultsInNativeMessage() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ResultsInNativeMessage.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setResultsInNativeMessage(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReplanReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReplanReason.
           */
         virtual const uci::type::ReplanReasonType& getReplanReason() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReplanReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReplanReason.
           */
         virtual uci::type::ReplanReasonType& getReplanReason()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReplanReason to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReplanReason
           */
         virtual void setReplanReason(const uci::type::ReplanReasonType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReplanReason exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReplanReason is emabled or not
           */
         virtual bool hasReplanReason() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReplanReason
           *
           * @param type = uci::type::accessorType::replanReasonType This Accessor's accessor type
           */
         virtual void enableReplanReason(uci::base::accessorType::AccessorType type = uci::type::accessorType::replanReasonType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReplanReason */
         virtual void clearReplanReason()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Command.
           *
           * @return The acecssor that provides access to the complex content that is identified by Command.
           */
         virtual const uci::type::MissionPlanCommandType& getCommand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Command.
           *
           * @return The acecssor that provides access to the complex content that is identified by Command.
           */
         virtual uci::type::MissionPlanCommandType& getCommand()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Command to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Command
           */
         virtual void setCommand(const uci::type::MissionPlanCommandType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the UseCommAllocations.
           *
           * @return The value of the simple primitive data type identified by UseCommAllocations.
           */
         virtual xs::Boolean getUseCommAllocations() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the UseCommAllocations.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setUseCommAllocations(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EnvironmentOverride.
           *
           * @return The bounded list identified by EnvironmentOverride.
           */
         virtual const uci::type::MissionPlanCommandMDT::EnvironmentOverride& getEnvironmentOverride() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EnvironmentOverride.
           *
           * @return The bounded list identified by EnvironmentOverride.
           */
         virtual uci::type::MissionPlanCommandMDT::EnvironmentOverride& getEnvironmentOverride()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EnvironmentOverride.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEnvironmentOverride(const uci::type::MissionPlanCommandMDT::EnvironmentOverride& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SupportedRoutes.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportedRoutes.
           */
         virtual const uci::type::RoutesType& getSupportedRoutes() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SupportedRoutes.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportedRoutes.
           */
         virtual uci::type::RoutesType& getSupportedRoutes()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SupportedRoutes to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SupportedRoutes
           */
         virtual void setSupportedRoutes(const uci::type::RoutesType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SupportedRoutes exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SupportedRoutes is emabled or not
           */
         virtual bool hasSupportedRoutes() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SupportedRoutes
           *
           * @param type = uci::type::accessorType::routesType This Accessor's accessor type
           */
         virtual void enableSupportedRoutes(uci::base::accessorType::AccessorType type = uci::type::accessorType::routesType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SupportedRoutes */
         virtual void clearSupportedRoutes()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionPlanCommandMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionPlanCommandMDT to copy from
           */
         MissionPlanCommandMDT(const MissionPlanCommandMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionPlanCommandMDT to the contents of the MissionPlanCommandMDT
           * on the right hand side (rhs) of the assignment operator.MissionPlanCommandMDT [only available to derived classes]
           *
           * @param rhs The MissionPlanCommandMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::MissionPlanCommandMDT
           * @return A constant reference to this MissionPlanCommandMDT.
           */
         const MissionPlanCommandMDT& operator=(const MissionPlanCommandMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionPlanCommandMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionPlanCommandMDT_h

