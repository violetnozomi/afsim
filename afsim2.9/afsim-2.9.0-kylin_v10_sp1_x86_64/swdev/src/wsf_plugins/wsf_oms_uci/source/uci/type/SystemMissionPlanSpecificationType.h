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
#ifndef Uci__Type__SystemMissionPlanSpecificationType_h
#define Uci__Type__SystemMissionPlanSpecificationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__MissionPlanReferencesType_h)
# include "uci/type/MissionPlanReferencesType.h"
#endif

#if !defined(Uci__Type__SystemCapabilitySpecificationType_h)
# include "uci/type/SystemCapabilitySpecificationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** A type that specifies information about a system including its position represented as a Mission Plan. */
      class SystemMissionPlanSpecificationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SystemMissionPlanSpecificationType()
         { }

         /** Returns this accessor's type constant, i.e. SystemMissionPlanSpecificationType
           *
           * @return This accessor's type constant, i.e. SystemMissionPlanSpecificationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::systemMissionPlanSpecificationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SystemMissionPlanSpecificationType& accessor)
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


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlan.
           */
         virtual const uci::type::MissionPlanReferencesType& getMissionPlan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlan.
           */
         virtual uci::type::MissionPlanReferencesType& getMissionPlan()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionPlan to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionPlan
           */
         virtual void setMissionPlan(const uci::type::MissionPlanReferencesType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityConstraints.
           */
         virtual const uci::type::SystemCapabilitySpecificationType& getCapabilityConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityConstraints.
           */
         virtual uci::type::SystemCapabilitySpecificationType& getCapabilityConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityConstraints to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityConstraints
           */
         virtual void setCapabilityConstraints(const uci::type::SystemCapabilitySpecificationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CapabilityConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CapabilityConstraints is emabled or not
           */
         virtual bool hasCapabilityConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CapabilityConstraints
           *
           * @param type = uci::type::accessorType::systemCapabilitySpecificationType This Accessor's accessor type
           */
         virtual void enableCapabilityConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::systemCapabilitySpecificationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CapabilityConstraints */
         virtual void clearCapabilityConstraints()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SystemMissionPlanSpecificationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SystemMissionPlanSpecificationType to copy from
           */
         SystemMissionPlanSpecificationType(const SystemMissionPlanSpecificationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SystemMissionPlanSpecificationType to the contents of the
           * SystemMissionPlanSpecificationType on the right hand side (rhs) of the assignment
           * operator.SystemMissionPlanSpecificationType [only available to derived classes]
           *
           * @param rhs The SystemMissionPlanSpecificationType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::SystemMissionPlanSpecificationType
           * @return A constant reference to this SystemMissionPlanSpecificationType.
           */
         const SystemMissionPlanSpecificationType& operator=(const SystemMissionPlanSpecificationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SystemMissionPlanSpecificationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SystemMissionPlanSpecificationType_h

