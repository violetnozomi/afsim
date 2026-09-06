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
#ifndef Uci__Type__POST_TrackCapabilityType_h
#define Uci__Type__POST_TrackCapabilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__TrackOptionsID_Type_h)
# include "uci/type/TrackOptionsID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__POST_TrackCapabilityTypeEnum_h)
# include "uci/type/POST_TrackCapabilityTypeEnum.h"
#endif

#if !defined(Uci__Type__IdentityBasicType_h)
# include "uci/type/IdentityBasicType.h"
#endif

#if !defined(Uci__Type__POST_OffsetTrackSettingsType_h)
# include "uci/type/POST_OffsetTrackSettingsType.h"
#endif

#if !defined(Uci__Type__DurationRangeType_h)
# include "uci/type/DurationRangeType.h"
#endif

#if !defined(Uci__Type__DoubleMinMaxType_h)
# include "uci/type/DoubleMinMaxType.h"
#endif

#if !defined(Uci__Type__POST_CoastSettingOptionsType_h)
# include "uci/type/POST_CoastSettingOptionsType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Allowable range of values for Search and Track parameters that can be used to customize target detections. */
      class POST_TrackCapabilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_TrackCapabilityType()
         { }

         /** Returns this accessor's type constant, i.e. POST_TrackCapabilityType
           *
           * @return This accessor's type constant, i.e. POST_TrackCapabilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_TrackCapabilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_TrackCapabilityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** These are the additional features that the POST_Command can support and are not specifically settable (i.e., not
           * available in POST_SettingsCommand). [Minimum occurrences: 0] [Maximum occurrences: 3]
           */
         typedef uci::base::BoundedList<uci::type::POST_TrackCapabilityTypeEnum, uci::type::accessorType::pOST_TrackCapabilityTypeEnum> TrackControlOptions;

         /** This is the allowed range of coast time settings associated with an environment setting for coasting a track before
           * dropping it. Up to 6 settings are allowed - one per environment option available. [Minimum occurrences: 0] [Maximum
           * occurrences: 6]
           */
         typedef uci::base::BoundedList<uci::type::POST_CoastSettingOptionsType, uci::type::accessorType::pOST_CoastSettingOptionsType> CoastSettingOptions;

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


         /** Returns the bounded list that is identified by the TrackControlOptions.
           *
           * @return The bounded list identified by TrackControlOptions.
           */
         virtual const uci::type::POST_TrackCapabilityType::TrackControlOptions& getTrackControlOptions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TrackControlOptions.
           *
           * @return The bounded list identified by TrackControlOptions.
           */
         virtual uci::type::POST_TrackCapabilityType::TrackControlOptions& getTrackControlOptions()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TrackControlOptions.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTrackControlOptions(const uci::type::POST_TrackCapabilityType::TrackControlOptions& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ATR_Filter.
           *
           * @return The acecssor that provides access to the complex content that is identified by ATR_Filter.
           */
         virtual const uci::type::IdentityBasicType& getATR_Filter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ATR_Filter.
           *
           * @return The acecssor that provides access to the complex content that is identified by ATR_Filter.
           */
         virtual uci::type::IdentityBasicType& getATR_Filter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ATR_Filter to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ATR_Filter
           */
         virtual void setATR_Filter(const uci::type::IdentityBasicType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ATR_Filter exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ATR_Filter is emabled or not
           */
         virtual bool hasATR_Filter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ATR_Filter
           *
           * @param type = uci::type::accessorType::identityBasicType This Accessor's accessor type
           */
         virtual void enableATR_Filter(uci::base::accessorType::AccessorType type = uci::type::accessorType::identityBasicType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ATR_Filter */
         virtual void clearATR_Filter()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OffsetTrackSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by OffsetTrackSettings.
           */
         virtual const uci::type::POST_OffsetTrackSettingsType& getOffsetTrackSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OffsetTrackSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by OffsetTrackSettings.
           */
         virtual uci::type::POST_OffsetTrackSettingsType& getOffsetTrackSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OffsetTrackSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OffsetTrackSettings
           */
         virtual void setOffsetTrackSettings(const uci::type::POST_OffsetTrackSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OffsetTrackSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OffsetTrackSettings is emabled or not
           */
         virtual bool hasOffsetTrackSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OffsetTrackSettings
           *
           * @param type = uci::type::accessorType::pOST_OffsetTrackSettingsType This Accessor's accessor type
           */
         virtual void enableOffsetTrackSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::pOST_OffsetTrackSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OffsetTrackSettings */
         virtual void clearOffsetTrackSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ObservedEntityThresholdSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      ObservedEntityThresholdSettings.
           */
         virtual const uci::type::DurationRangeType& getObservedEntityThresholdSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ObservedEntityThresholdSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      ObservedEntityThresholdSettings.
           */
         virtual uci::type::DurationRangeType& getObservedEntityThresholdSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ObservedEntityThresholdSettings to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ObservedEntityThresholdSettings
           */
         virtual void setObservedEntityThresholdSettings(const uci::type::DurationRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ObservedEntityThresholdSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ObservedEntityThresholdSettings is emabled or not
           */
         virtual bool hasObservedEntityThresholdSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ObservedEntityThresholdSettings
           *
           * @param type = uci::type::accessorType::durationRangeType This Accessor's accessor type
           */
         virtual void enableObservedEntityThresholdSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ObservedEntityThresholdSettings */
         virtual void clearObservedEntityThresholdSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TrackingRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by TrackingRange.
           */
         virtual const uci::type::DoubleMinMaxType& getTrackingRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TrackingRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by TrackingRange.
           */
         virtual uci::type::DoubleMinMaxType& getTrackingRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TrackingRange to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TrackingRange
           */
         virtual void setTrackingRange(const uci::type::DoubleMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TrackingRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TrackingRange is emabled or not
           */
         virtual bool hasTrackingRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TrackingRange
           *
           * @param type = uci::type::accessorType::doubleMinMaxType This Accessor's accessor type
           */
         virtual void enableTrackingRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::doubleMinMaxType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TrackingRange */
         virtual void clearTrackingRange()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CoastSettingOptions.
           *
           * @return The bounded list identified by CoastSettingOptions.
           */
         virtual const uci::type::POST_TrackCapabilityType::CoastSettingOptions& getCoastSettingOptions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CoastSettingOptions.
           *
           * @return The bounded list identified by CoastSettingOptions.
           */
         virtual uci::type::POST_TrackCapabilityType::CoastSettingOptions& getCoastSettingOptions()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CoastSettingOptions.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCoastSettingOptions(const uci::type::POST_TrackCapabilityType::CoastSettingOptions& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the BlankingVolumeSupported.
           *
           * @return The value of the simple primitive data type identified by BlankingVolumeSupported.
           */
         virtual xs::Boolean getBlankingVolumeSupported() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the BlankingVolumeSupported.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBlankingVolumeSupported(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BlankingVolumeSupported exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BlankingVolumeSupported is emabled or not
           */
         virtual bool hasBlankingVolumeSupported() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BlankingVolumeSupported
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableBlankingVolumeSupported(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BlankingVolumeSupported */
         virtual void clearBlankingVolumeSupported()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_TrackCapabilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_TrackCapabilityType to copy from
           */
         POST_TrackCapabilityType(const POST_TrackCapabilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_TrackCapabilityType to the contents of the
           * POST_TrackCapabilityType on the right hand side (rhs) of the assignment operator.POST_TrackCapabilityType [only
           * available to derived classes]
           *
           * @param rhs The POST_TrackCapabilityType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::POST_TrackCapabilityType
           * @return A constant reference to this POST_TrackCapabilityType.
           */
         const POST_TrackCapabilityType& operator=(const POST_TrackCapabilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_TrackCapabilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_TrackCapabilityType_h

