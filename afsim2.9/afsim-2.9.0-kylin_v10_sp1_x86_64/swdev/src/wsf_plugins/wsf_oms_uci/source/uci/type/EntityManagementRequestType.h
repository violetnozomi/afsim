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
#ifndef Uci__Type__EntityManagementRequestType_h
#define Uci__Type__EntityManagementRequestType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityManagementSplitType_h)
# include "uci/type/EntityManagementSplitType.h"
#endif

#if !defined(Uci__Type__EntityManagementMergeType_h)
# include "uci/type/EntityManagementMergeType.h"
#endif

#if !defined(Uci__Type__EntityManagementDropType_h)
# include "uci/type/EntityManagementDropType.h"
#endif

#if !defined(Uci__Type__EntityManagementDropProtectType_h)
# include "uci/type/EntityManagementDropProtectType.h"
#endif

#if !defined(Uci__Type__EntityManagementSwapType_h)
# include "uci/type/EntityManagementSwapType.h"
#endif

#if !defined(Uci__Type__EntityManagementSetFusionEligibilityType_h)
# include "uci/type/EntityManagementSetFusionEligibilityType.h"
#endif

#if !defined(Uci__Type__EntityManagementSetIdentityType_h)
# include "uci/type/EntityManagementSetIdentityType.h"
#endif

#if !defined(Uci__Type__EntityManagementClearIdentityType_h)
# include "uci/type/EntityManagementClearIdentityType.h"
#endif

#if !defined(Uci__Type__EntityManagementDownType_h)
# include "uci/type/EntityManagementDownType.h"
#endif

#if !defined(Uci__Type__EntityManagementKinematicsType_h)
# include "uci/type/EntityManagementKinematicsType.h"
#endif

#if !defined(Uci__Type__EntityManagementSetModeType_h)
# include "uci/type/EntityManagementSetModeType.h"
#endif

#if !defined(Uci__Type__EntityManagementSetActivityByType_h)
# include "uci/type/EntityManagementSetActivityByType.h"
#endif

#if !defined(Uci__Type__EntityManagementSetStrengthType_h)
# include "uci/type/EntityManagementSetStrengthType.h"
#endif

#if !defined(Uci__Type__EntityManagementSetPlatformStatusType_h)
# include "uci/type/EntityManagementSetPlatformStatusType.h"
#endif

#if !defined(Uci__Type__EntityManagementSetLink16MetadataType_h)
# include "uci/type/EntityManagementSetLink16MetadataType.h"
#endif

#if !defined(Uci__Type__EntityManagementProposeCorrelationType_h)
# include "uci/type/EntityManagementProposeCorrelationType.h"
#endif

#if !defined(Uci__Type__EntityManagementSetVoiceControlType_h)
# include "uci/type/EntityManagementSetVoiceControlType.h"
#endif

#if !defined(Uci__Type__EntityManagementSetCapabilityType_h)
# include "uci/type/EntityManagementSetCapabilityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntityManagementRequestType choice accessor class */
      class EntityManagementRequestType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntityManagementRequestType()
         { }

         /** Returns this accessor's type constant, i.e. EntityManagementRequestType
           *
           * @return This accessor's type constant, i.e. EntityManagementRequestType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityManagementRequestType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityManagementRequestType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum EntityManagementRequestTypeChoice {
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_NONE,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_SPLIT,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_MERGE,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_DROP,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_DROPPROTECT,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_SWAP,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_SETFUSIONELIGIBILITY,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_SETIDENTITY,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_CLEARIFF,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_SETDOWNLOCATION,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_SETKINEMATICS,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_SETMODE,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_SETACTIVITYBY,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_SETSTRENGTH,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_SETPLATFORMSTATUS,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_SETLINK16METADATA,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_PROPOSECORRELATION,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_SETVOICECONTROL,
            ENTITYMANAGEMENTREQUESTTYPE_CHOICE_SETCAPABILITY
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual EntityManagementRequestTypeChoice getEntityManagementRequestTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getEntityManagementRequestTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In
           * addition, the access methods associated with the chosen element will be enabled and will provide access to the chosen
           * element.
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
         virtual void setEntityManagementRequestTypeChoiceOrdinal(EntityManagementRequestTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Split.
           *
           * @return The acecssor that provides access to the complex content that is identified by Split.
           */
         virtual const uci::type::EntityManagementSplitType& getSplit() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Split.
           *
           * @return The acecssor that provides access to the complex content that is identified by Split.
           */
         virtual uci::type::EntityManagementSplitType& getSplit()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Split to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Split
           */
         virtual void setSplit(const uci::type::EntityManagementSplitType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Merge.
           *
           * @return The acecssor that provides access to the complex content that is identified by Merge.
           */
         virtual const uci::type::EntityManagementMergeType& getMerge() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Merge.
           *
           * @return The acecssor that provides access to the complex content that is identified by Merge.
           */
         virtual uci::type::EntityManagementMergeType& getMerge()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Merge to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Merge
           */
         virtual void setMerge(const uci::type::EntityManagementMergeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Drop.
           *
           * @return The acecssor that provides access to the complex content that is identified by Drop.
           */
         virtual const uci::type::EntityManagementDropType& getDrop() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Drop.
           *
           * @return The acecssor that provides access to the complex content that is identified by Drop.
           */
         virtual uci::type::EntityManagementDropType& getDrop()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Drop to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Drop
           */
         virtual void setDrop(const uci::type::EntityManagementDropType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DropProtect.
           *
           * @return The acecssor that provides access to the complex content that is identified by DropProtect.
           */
         virtual const uci::type::EntityManagementDropProtectType& getDropProtect() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DropProtect.
           *
           * @return The acecssor that provides access to the complex content that is identified by DropProtect.
           */
         virtual uci::type::EntityManagementDropProtectType& getDropProtect()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DropProtect to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DropProtect
           */
         virtual void setDropProtect(const uci::type::EntityManagementDropProtectType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Swap.
           *
           * @return The acecssor that provides access to the complex content that is identified by Swap.
           */
         virtual const uci::type::EntityManagementSwapType& getSwap() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Swap.
           *
           * @return The acecssor that provides access to the complex content that is identified by Swap.
           */
         virtual uci::type::EntityManagementSwapType& getSwap()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Swap to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Swap
           */
         virtual void setSwap(const uci::type::EntityManagementSwapType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetFusionEligibility.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetFusionEligibility.
           */
         virtual const uci::type::EntityManagementSetFusionEligibilityType& getSetFusionEligibility() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetFusionEligibility.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetFusionEligibility.
           */
         virtual uci::type::EntityManagementSetFusionEligibilityType& getSetFusionEligibility()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SetFusionEligibility to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SetFusionEligibility
           */
         virtual void setSetFusionEligibility(const uci::type::EntityManagementSetFusionEligibilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetIdentity.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetIdentity.
           */
         virtual const uci::type::EntityManagementSetIdentityType& getSetIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetIdentity.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetIdentity.
           */
         virtual uci::type::EntityManagementSetIdentityType& getSetIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SetIdentity to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SetIdentity
           */
         virtual void setSetIdentity(const uci::type::EntityManagementSetIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ClearIFF.
           *
           * @return The acecssor that provides access to the complex content that is identified by ClearIFF.
           */
         virtual const uci::type::EntityManagementClearIdentityType& getClearIFF() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ClearIFF.
           *
           * @return The acecssor that provides access to the complex content that is identified by ClearIFF.
           */
         virtual uci::type::EntityManagementClearIdentityType& getClearIFF()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ClearIFF to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ClearIFF
           */
         virtual void setClearIFF(const uci::type::EntityManagementClearIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetDownLocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetDownLocation.
           */
         virtual const uci::type::EntityManagementDownType& getSetDownLocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetDownLocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetDownLocation.
           */
         virtual uci::type::EntityManagementDownType& getSetDownLocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SetDownLocation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SetDownLocation
           */
         virtual void setSetDownLocation(const uci::type::EntityManagementDownType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetKinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetKinematics.
           */
         virtual const uci::type::EntityManagementKinematicsType& getSetKinematics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetKinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetKinematics.
           */
         virtual uci::type::EntityManagementKinematicsType& getSetKinematics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SetKinematics to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SetKinematics
           */
         virtual void setSetKinematics(const uci::type::EntityManagementKinematicsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetMode.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetMode.
           */
         virtual const uci::type::EntityManagementSetModeType& getSetMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetMode.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetMode.
           */
         virtual uci::type::EntityManagementSetModeType& getSetMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SetMode to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SetMode
           */
         virtual void setSetMode(const uci::type::EntityManagementSetModeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetActivityBy.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetActivityBy.
           */
         virtual const uci::type::EntityManagementSetActivityByType& getSetActivityBy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetActivityBy.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetActivityBy.
           */
         virtual uci::type::EntityManagementSetActivityByType& getSetActivityBy()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SetActivityBy to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SetActivityBy
           */
         virtual void setSetActivityBy(const uci::type::EntityManagementSetActivityByType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetStrength.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetStrength.
           */
         virtual const uci::type::EntityManagementSetStrengthType& getSetStrength() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetStrength.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetStrength.
           */
         virtual uci::type::EntityManagementSetStrengthType& getSetStrength()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SetStrength to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SetStrength
           */
         virtual void setSetStrength(const uci::type::EntityManagementSetStrengthType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetPlatformStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetPlatformStatus.
           */
         virtual const uci::type::EntityManagementSetPlatformStatusType& getSetPlatformStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetPlatformStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetPlatformStatus.
           */
         virtual uci::type::EntityManagementSetPlatformStatusType& getSetPlatformStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SetPlatformStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SetPlatformStatus
           */
         virtual void setSetPlatformStatus(const uci::type::EntityManagementSetPlatformStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetLink16Metadata.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetLink16Metadata.
           */
         virtual const uci::type::EntityManagementSetLink16MetadataType& getSetLink16Metadata() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetLink16Metadata.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetLink16Metadata.
           */
         virtual uci::type::EntityManagementSetLink16MetadataType& getSetLink16Metadata()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SetLink16Metadata to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SetLink16Metadata
           */
         virtual void setSetLink16Metadata(const uci::type::EntityManagementSetLink16MetadataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProposeCorrelation.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProposeCorrelation.
           */
         virtual const uci::type::EntityManagementProposeCorrelationType& getProposeCorrelation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProposeCorrelation.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProposeCorrelation.
           */
         virtual uci::type::EntityManagementProposeCorrelationType& getProposeCorrelation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProposeCorrelation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProposeCorrelation
           */
         virtual void setProposeCorrelation(const uci::type::EntityManagementProposeCorrelationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetVoiceControl.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetVoiceControl.
           */
         virtual const uci::type::EntityManagementSetVoiceControlType& getSetVoiceControl() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetVoiceControl.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetVoiceControl.
           */
         virtual uci::type::EntityManagementSetVoiceControlType& getSetVoiceControl()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SetVoiceControl to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SetVoiceControl
           */
         virtual void setSetVoiceControl(const uci::type::EntityManagementSetVoiceControlType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetCapability.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetCapability.
           */
         virtual const uci::type::EntityManagementSetCapabilityType& getSetCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SetCapability.
           *
           * @return The acecssor that provides access to the complex content that is identified by SetCapability.
           */
         virtual uci::type::EntityManagementSetCapabilityType& getSetCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SetCapability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SetCapability
           */
         virtual void setSetCapability(const uci::type::EntityManagementSetCapabilityType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityManagementRequestType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityManagementRequestType to copy from
           */
         EntityManagementRequestType(const EntityManagementRequestType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityManagementRequestType to the contents of the
           * EntityManagementRequestType on the right hand side (rhs) of the assignment operator.EntityManagementRequestType [only
           * available to derived classes]
           *
           * @param rhs The EntityManagementRequestType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EntityManagementRequestType
           * @return A constant reference to this EntityManagementRequestType.
           */
         const EntityManagementRequestType& operator=(const EntityManagementRequestType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityManagementRequestType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityManagementRequestType_h

