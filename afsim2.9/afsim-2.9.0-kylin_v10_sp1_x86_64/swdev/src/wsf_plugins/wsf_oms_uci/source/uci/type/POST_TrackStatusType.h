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
#ifndef Uci__Type__POST_TrackStatusType_h
#define Uci__Type__POST_TrackStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__TrackOptionsID_Type_h)
# include "uci/type/TrackOptionsID_Type.h"
#endif

#if !defined(Uci__Type__POST_TrackCommandEnum_h)
# include "uci/type/POST_TrackCommandEnum.h"
#endif

#if !defined(Uci__Type__IncreaseDecreaseEnum_h)
# include "uci/type/IncreaseDecreaseEnum.h"
#endif

#if !defined(Uci__Type__POST_LockPersistenceEnum_h)
# include "uci/type/POST_LockPersistenceEnum.h"
#endif

#if !defined(Uci__Type__IdentityBasicType_h)
# include "uci/type/IdentityBasicType.h"
#endif

#if !defined(Uci__Type__LOS_VariableB_Type_h)
# include "uci/type/LOS_VariableB_Type.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__POST_CoastSettingValueType_h)
# include "uci/type/POST_CoastSettingValueType.h"
#endif

#if !defined(Uci__Type__POST_AirTargetVolumeType_h)
# include "uci/type/POST_AirTargetVolumeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Status of current values used for Search and Track parameters to customize target detections. */
      class POST_TrackStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_TrackStatusType()
         { }

         /** Returns this accessor's type constant, i.e. POST_TrackStatusType
           *
           * @return This accessor's type constant, i.e. POST_TrackStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_TrackStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_TrackStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the amount of time currently being used to coast a track before dropping it based on the associated
           * environment setting. [Minimum occurrences: 0] [Maximum occurrences: 6]
           */
         typedef uci::base::BoundedList<uci::type::POST_CoastSettingValueType, uci::type::accessorType::pOST_CoastSettingValueType> CoastSetting;

         /** Returns the accessor that provides access to the complex content that is identified by the TrackOptionID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TrackOptionID.
           */
         virtual const uci::type::TrackOptionsID_Type& getTrackOptionID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TrackOptionID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TrackOptionID.
           */
         virtual uci::type::TrackOptionsID_Type& getTrackOptionID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TrackOptionID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TrackOptionID
           */
         virtual void setTrackOptionID(const uci::type::TrackOptionsID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Mode.
           *
           * @return The value of the enumeration identified by Mode.
           */
         virtual const uci::type::POST_TrackCommandEnum& getMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Mode.
           *
           * @return The value of the enumeration identified by Mode.
           */
         virtual uci::type::POST_TrackCommandEnum& getMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Mode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMode(const uci::type::POST_TrackCommandEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Mode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMode(uci::type::POST_TrackCommandEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Modeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasMode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getMode will return the optional field and not throw an exception when invoked.
           *
           * @param type = uci::type::accessorType::pOST_TrackCommandEnum This Accessor's accessor type
           */
         virtual void enableMode(uci::base::accessorType::AccessorType type = uci::type::accessorType::pOST_TrackCommandEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearMode()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TrackArea.
           *
           * @return The value of the enumeration identified by TrackArea.
           */
         virtual const uci::type::IncreaseDecreaseEnum& getTrackArea() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TrackArea.
           *
           * @return The value of the enumeration identified by TrackArea.
           */
         virtual uci::type::IncreaseDecreaseEnum& getTrackArea()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TrackArea.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTrackArea(const uci::type::IncreaseDecreaseEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TrackArea.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTrackArea(uci::type::IncreaseDecreaseEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield TrackAreais available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTrackArea() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTrackArea will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::increaseDecreaseEnum This Accessor's accessor type
           */
         virtual void enableTrackArea(uci::base::accessorType::AccessorType type = uci::type::accessorType::increaseDecreaseEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTrackArea()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LockPersistence.
           *
           * @return The value of the enumeration identified by LockPersistence.
           */
         virtual const uci::type::POST_LockPersistenceEnum& getLockPersistence() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LockPersistence.
           *
           * @return The value of the enumeration identified by LockPersistence.
           */
         virtual uci::type::POST_LockPersistenceEnum& getLockPersistence()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LockPersistence.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLockPersistence(const uci::type::POST_LockPersistenceEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LockPersistence.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLockPersistence(uci::type::POST_LockPersistenceEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield LockPersistenceis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasLockPersistence() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getLockPersistence will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::pOST_LockPersistenceEnum This Accessor's accessor type
           */
         virtual void enableLockPersistence(uci::base::accessorType::AccessorType type = uci::type::accessorType::pOST_LockPersistenceEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearLockPersistence()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ATR_FilterStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by ATR_FilterStatus.
           */
         virtual const uci::type::IdentityBasicType& getATR_FilterStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ATR_FilterStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by ATR_FilterStatus.
           */
         virtual uci::type::IdentityBasicType& getATR_FilterStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ATR_FilterStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ATR_FilterStatus
           */
         virtual void setATR_FilterStatus(const uci::type::IdentityBasicType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ATR_FilterStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ATR_FilterStatus is emabled or not
           */
         virtual bool hasATR_FilterStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ATR_FilterStatus
           *
           * @param type = uci::type::accessorType::identityBasicType This Accessor's accessor type
           */
         virtual void enableATR_FilterStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::identityBasicType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ATR_FilterStatus */
         virtual void clearATR_FilterStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OffsetTrackSetting.
           *
           * @return The acecssor that provides access to the complex content that is identified by OffsetTrackSetting.
           */
         virtual const uci::type::LOS_VariableB_Type& getOffsetTrackSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OffsetTrackSetting.
           *
           * @return The acecssor that provides access to the complex content that is identified by OffsetTrackSetting.
           */
         virtual uci::type::LOS_VariableB_Type& getOffsetTrackSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OffsetTrackSetting to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OffsetTrackSetting
           */
         virtual void setOffsetTrackSetting(const uci::type::LOS_VariableB_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OffsetTrackSetting exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OffsetTrackSetting is emabled or not
           */
         virtual bool hasOffsetTrackSetting() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OffsetTrackSetting
           *
           * @param type = uci::type::accessorType::lOS_VariableB_Type This Accessor's accessor type
           */
         virtual void enableOffsetTrackSetting(uci::base::accessorType::AccessorType type = uci::type::accessorType::lOS_VariableB_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OffsetTrackSetting */
         virtual void clearOffsetTrackSetting()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ObservedEntityThreshold.
           *
           * @return The value of the simple primitive data type identified by ObservedEntityThreshold.
           */
         virtual uci::type::DurationTypeValue getObservedEntityThreshold() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ObservedEntityThreshold.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setObservedEntityThreshold(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ObservedEntityThreshold exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ObservedEntityThreshold is emabled or not
           */
         virtual bool hasObservedEntityThreshold() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ObservedEntityThreshold
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableObservedEntityThreshold(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ObservedEntityThreshold */
         virtual void clearObservedEntityThreshold()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TrackingRange.
           *
           * @return The value of the simple primitive data type identified by TrackingRange.
           */
         virtual uci::type::DistanceTypeValue getTrackingRange() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TrackingRange.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTrackingRange(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TrackingRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TrackingRange is emabled or not
           */
         virtual bool hasTrackingRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TrackingRange
           *
           * @param type = uci::type::accessorType::distanceType This Accessor's accessor type
           */
         virtual void enableTrackingRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::distanceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TrackingRange */
         virtual void clearTrackingRange()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CoastSetting.
           *
           * @return The bounded list identified by CoastSetting.
           */
         virtual const uci::type::POST_TrackStatusType::CoastSetting& getCoastSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CoastSetting.
           *
           * @return The bounded list identified by CoastSetting.
           */
         virtual uci::type::POST_TrackStatusType::CoastSetting& getCoastSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CoastSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCoastSetting(const uci::type::POST_TrackStatusType::CoastSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BlankingVolumeStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by BlankingVolumeStatus.
           */
         virtual const uci::type::POST_AirTargetVolumeType& getBlankingVolumeStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BlankingVolumeStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by BlankingVolumeStatus.
           */
         virtual uci::type::POST_AirTargetVolumeType& getBlankingVolumeStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the BlankingVolumeStatus to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by BlankingVolumeStatus
           */
         virtual void setBlankingVolumeStatus(const uci::type::POST_AirTargetVolumeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BlankingVolumeStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BlankingVolumeStatus is emabled or not
           */
         virtual bool hasBlankingVolumeStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BlankingVolumeStatus
           *
           * @param type = uci::type::accessorType::pOST_AirTargetVolumeType This Accessor's accessor type
           */
         virtual void enableBlankingVolumeStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::pOST_AirTargetVolumeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BlankingVolumeStatus */
         virtual void clearBlankingVolumeStatus()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_TrackStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_TrackStatusType to copy from
           */
         POST_TrackStatusType(const POST_TrackStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_TrackStatusType to the contents of the POST_TrackStatusType
           * on the right hand side (rhs) of the assignment operator.POST_TrackStatusType [only available to derived classes]
           *
           * @param rhs The POST_TrackStatusType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::POST_TrackStatusType
           * @return A constant reference to this POST_TrackStatusType.
           */
         const POST_TrackStatusType& operator=(const POST_TrackStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_TrackStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_TrackStatusType_h

