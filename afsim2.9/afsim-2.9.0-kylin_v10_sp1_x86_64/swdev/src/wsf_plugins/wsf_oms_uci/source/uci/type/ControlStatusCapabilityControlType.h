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
#ifndef Uci__Type__ControlStatusCapabilityControlType_h
#define Uci__Type__ControlStatusCapabilityControlType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__PrimaryControllerType_h)
# include "uci/type/PrimaryControllerType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SecondaryControllerType_h)
# include "uci/type/SecondaryControllerType.h"
#endif

#if !defined(Uci__Type__CapabilityManagerType_h)
# include "uci/type/CapabilityManagerType.h"
#endif

#if !defined(Uci__Type__CapabilityControlInterfacesEnum_h)
# include "uci/type/CapabilityControlInterfacesEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ControlStatusCapabilityControlType sequence accessor class */
      class ControlStatusCapabilityControlType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ControlStatusCapabilityControlType()
         { }

         /** Returns this accessor's type constant, i.e. ControlStatusCapabilityControlType
           *
           * @return This accessor's type constant, i.e. ControlStatusCapabilityControlType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::controlStatusCapabilityControlType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ControlStatusCapabilityControlType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a secondary controller, generally an operator/HCIs, of the Capability. If this field is omitted then either
           * the Capability is not operational or currently has no secondary controllers. See ControlTypeEnum annotations for
           * further details. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SecondaryControllerType, uci::type::accessorType::secondaryControllerType> SecondaryController;

         /** Indicates a Capability Manager (an automation service, also known as a Resource Manager) that is allowed to
           * autonomously command the Capability. Only the primary controller is allowed to attach Capability Managers, using the
           * ControlInterfacesCommand. See ControlTypeEnum annotations for further details. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityManagerType, uci::type::accessorType::capabilityManagerType> CapabilityManager;

         /** Enumerated list of UCI Capability control interfaces/messages this Capability will currently accept. See enumeration
           * annotations. If not present, then no commands are currently accepted. Note that the MissionPlan related enumerations
           * are closely related to the MissionControl.AcceptedControls in this message. However, this element is not an override
           * of MissionControl.AcceptedControls. This element is an indication of the controls that the primary controller
           * currently has enabled. If MissionControl.AcceptedControls=MISSION_PLAN, indicating that the System is "taskable",
           * while this element does not indicate MissionPlan related interfaces are being accepted, it is an indication that the
           * primary controller isn't currently allowing Capability control via a MissionPlan but will enable it at an appropriate
           * time in the future. [Minimum occurrences: 0] [Maximum occurrences: 7]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityControlInterfacesEnum, uci::type::accessorType::capabilityControlInterfacesEnum> AcceptedInterface;

         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual const uci::type::CapabilityID_Type& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual uci::type::CapabilityID_Type& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityID
           */
         virtual void setCapabilityID(const uci::type::CapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PrimaryController.
           *
           * @return The acecssor that provides access to the complex content that is identified by PrimaryController.
           */
         virtual const uci::type::PrimaryControllerType& getPrimaryController() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PrimaryController.
           *
           * @return The acecssor that provides access to the complex content that is identified by PrimaryController.
           */
         virtual uci::type::PrimaryControllerType& getPrimaryController()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PrimaryController to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PrimaryController
           */
         virtual void setPrimaryController(const uci::type::PrimaryControllerType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PrimaryController exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PrimaryController is emabled or not
           */
         virtual bool hasPrimaryController() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PrimaryController
           *
           * @param type = uci::type::accessorType::primaryControllerType This Accessor's accessor type
           */
         virtual void enablePrimaryController(uci::base::accessorType::AccessorType type = uci::type::accessorType::primaryControllerType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PrimaryController */
         virtual void clearPrimaryController()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SecondaryController.
           *
           * @return The bounded list identified by SecondaryController.
           */
         virtual const uci::type::ControlStatusCapabilityControlType::SecondaryController& getSecondaryController() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SecondaryController.
           *
           * @return The bounded list identified by SecondaryController.
           */
         virtual uci::type::ControlStatusCapabilityControlType::SecondaryController& getSecondaryController()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SecondaryController.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSecondaryController(const uci::type::ControlStatusCapabilityControlType::SecondaryController& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityManager.
           *
           * @return The bounded list identified by CapabilityManager.
           */
         virtual const uci::type::ControlStatusCapabilityControlType::CapabilityManager& getCapabilityManager() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityManager.
           *
           * @return The bounded list identified by CapabilityManager.
           */
         virtual uci::type::ControlStatusCapabilityControlType::CapabilityManager& getCapabilityManager()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityManager.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityManager(const uci::type::ControlStatusCapabilityControlType::CapabilityManager& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual const uci::type::ControlStatusCapabilityControlType::AcceptedInterface& getAcceptedInterface() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual uci::type::ControlStatusCapabilityControlType::AcceptedInterface& getAcceptedInterface()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AcceptedInterface.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAcceptedInterface(const uci::type::ControlStatusCapabilityControlType::AcceptedInterface& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ControlStatusCapabilityControlType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ControlStatusCapabilityControlType to copy from
           */
         ControlStatusCapabilityControlType(const ControlStatusCapabilityControlType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ControlStatusCapabilityControlType to the contents of the
           * ControlStatusCapabilityControlType on the right hand side (rhs) of the assignment
           * operator.ControlStatusCapabilityControlType [only available to derived classes]
           *
           * @param rhs The ControlStatusCapabilityControlType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ControlStatusCapabilityControlType
           * @return A constant reference to this ControlStatusCapabilityControlType.
           */
         const ControlStatusCapabilityControlType& operator=(const ControlStatusCapabilityControlType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ControlStatusCapabilityControlType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ControlStatusCapabilityControlType_h

