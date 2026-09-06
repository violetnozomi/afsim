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
#ifndef Uci__Type__ReadinessItemType_h
#define Uci__Type__ReadinessItemType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemStatusMDT_h)
# include "uci/type/SystemStatusMDT.h"
#endif

#if !defined(Uci__Type__SubsystemStatusMDT_h)
# include "uci/type/SubsystemStatusMDT.h"
#endif

#if !defined(Uci__Type__CapabilityBaseType_h)
# include "uci/type/CapabilityBaseType.h"
#endif

#if !defined(Uci__Type__CapabilityStatusBaseType_h)
# include "uci/type/CapabilityStatusBaseType.h"
#endif

#if !defined(Uci__Type__StoreManagementStatusMDT_h)
# include "uci/type/StoreManagementStatusMDT.h"
#endif

#if !defined(Uci__Type__ControlStatusMDT_h)
# include "uci/type/ControlStatusMDT.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ReadinessItemType choice accessor class */
      class ReadinessItemType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ReadinessItemType()
         { }

         /** Returns this accessor's type constant, i.e. ReadinessItemType
           *
           * @return This accessor's type constant, i.e. ReadinessItemType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::readinessItemType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ReadinessItemType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum ReadinessItemTypeChoice {
            READINESSITEMTYPE_CHOICE_NONE,
            READINESSITEMTYPE_CHOICE_SYSTEMSTATUS,
            READINESSITEMTYPE_CHOICE_SUBSYSTEMSTATUS,
            READINESSITEMTYPE_CHOICE_CAPABILITY,
            READINESSITEMTYPE_CHOICE_CAPABILITYSTATUS,
            READINESSITEMTYPE_CHOICE_STOREMANAGEMENTSTATUS,
            READINESSITEMTYPE_CHOICE_CONTROLSTATUS
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual ReadinessItemTypeChoice getReadinessItemTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getReadinessItemTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the
           * access methods associated with the chosen element will be enabled and will provide access to the chosen element.
           *
           * @param chosenElementOrdinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setReadinessItemTypeChoiceOrdinal(ReadinessItemTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemStatus.
           */
         virtual const uci::type::SystemStatusMDT& getSystemStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemStatus.
           */
         virtual uci::type::SystemStatusMDT& getSystemStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemStatus
           */
         virtual void setSystemStatus(const uci::type::SystemStatusMDT& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemStatus.
           */
         virtual const uci::type::SubsystemStatusMDT& getSubsystemStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemStatus.
           */
         virtual uci::type::SubsystemStatusMDT& getSubsystemStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubsystemStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubsystemStatus
           */
         virtual void setSubsystemStatus(const uci::type::SubsystemStatusMDT& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Capability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Capability.
           */
         virtual const uci::type::CapabilityBaseType& getCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Capability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Capability.
           */
         virtual uci::type::CapabilityBaseType& getCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Capability to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Capability
           */
         virtual void setCapability(const uci::type::CapabilityBaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityStatus.
           */
         virtual const uci::type::CapabilityStatusBaseType& getCapabilityStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityStatus.
           */
         virtual uci::type::CapabilityStatusBaseType& getCapabilityStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityStatus
           */
         virtual void setCapabilityStatus(const uci::type::CapabilityStatusBaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the StoreManagementStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by StoreManagementStatus.
           */
         virtual const uci::type::StoreManagementStatusMDT& getStoreManagementStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the StoreManagementStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by StoreManagementStatus.
           */
         virtual uci::type::StoreManagementStatusMDT& getStoreManagementStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the StoreManagementStatus to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by StoreManagementStatus
           */
         virtual void setStoreManagementStatus(const uci::type::StoreManagementStatusMDT& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ControlStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControlStatus.
           */
         virtual const uci::type::ControlStatusMDT& getControlStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ControlStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControlStatus.
           */
         virtual uci::type::ControlStatusMDT& getControlStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ControlStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ControlStatus
           */
         virtual void setControlStatus(const uci::type::ControlStatusMDT& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ReadinessItemType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ReadinessItemType to copy from
           */
         ReadinessItemType(const ReadinessItemType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ReadinessItemType to the contents of the ReadinessItemType on the
           * right hand side (rhs) of the assignment operator.ReadinessItemType [only available to derived classes]
           *
           * @param rhs The ReadinessItemType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ReadinessItemType
           * @return A constant reference to this ReadinessItemType.
           */
         const ReadinessItemType& operator=(const ReadinessItemType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ReadinessItemType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ReadinessItemType_h

