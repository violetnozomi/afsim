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
#ifndef Uci__Type__POST_ActivityType_h
#define Uci__Type__POST_ActivityType_h 1

#if !defined(Uci__Type__ActivityBaseType_h)
# include "uci/type/ActivityBaseType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__FOV_ActivityType_h)
# include "uci/type/FOV_ActivityType.h"
#endif

#if !defined(Uci__Type__PointingType_h)
# include "uci/type/PointingType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__POST_SubCapabilitySettingsType_h)
# include "uci/type/POST_SubCapabilitySettingsType.h"
#endif

#if !defined(Uci__Type__POST_ComponentStatusType_h)
# include "uci/type/POST_ComponentStatusType.h"
#endif

#if !defined(Uci__Type__PO_CollectionConstraintsStatusType_h)
# include "uci/type/PO_CollectionConstraintsStatusType.h"
#endif

#if !defined(Uci__Type__POST_TrackDetailsType_h)
# include "uci/type/POST_TrackDetailsType.h"
#endif

#if !defined(Uci__Type__ProductOutputCapabilityStatusImageryType_h)
# include "uci/type/ProductOutputCapabilityStatusImageryType.h"
#endif

#if !defined(Uci__Type__SearchTrackActivityDetailsType_h)
# include "uci/type/SearchTrackActivityDetailsType.h"
#endif

#if !defined(Uci__Type__POST_TrackActivityType_h)
# include "uci/type/POST_TrackActivityType.h"
#endif

#if !defined(Uci__Type__FOR_Enum_h)
# include "uci/type/FOR_Enum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_ActivityType sequence accessor class */
      class POST_ActivityType : public virtual uci::type::ActivityBaseType {
      public:

         /** The destructor */
         virtual ~POST_ActivityType()
         { }

         /** Returns this accessor's type constant, i.e. POST_ActivityType
           *
           * @return This accessor's type constant, i.e. POST_ActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_ActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_ActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates detailed POST sensor Subcapabilities settings that will be used for this Activity. If the Activity is
           * interactive and the Component controls and settings support manual interaction, these settings can be changed via
           * POST_Command. Activity to optimize the captured imagery and tracks. If omitted, the Activity is subject to the
           * default/current settings when the Activity starts. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_SubCapabilitySettingsType, uci::type::accessorType::pOST_SubCapabilitySettingsType> SubCapabilityStatus;

         /** Indicates detailed POST sensor control states and settings that will be used for this Activity. If the Activity is
           * interactive and the Component controls and settings support manual interaction, these settings can be changed via
           * POST_Command. Activity to optimize the captured imagery. If omitted, the Activity is subject to the default/current
           * Component state when the Activity starts. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_ComponentStatusType, uci::type::accessorType::pOST_ComponentStatusType> ComponentStatus;

         /** Indicates the current status of the product output associated with this Activity. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductOutputCapabilityStatusImageryType, uci::type::accessorType::productOutputCapabilityStatusImageryType> ProductOutputStatus;

         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedStartTime.
           *
           * @return The value of the simple primitive data type identified by EstimatedStartTime.
           */
         virtual uci::type::DateTimeTypeValue getEstimatedStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedStartTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedStartTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedStartTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedStartTime is emabled or not
           */
         virtual bool hasEstimatedStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedStartTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableEstimatedStartTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedStartTime */
         virtual void clearEstimatedStartTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ActualStartTime.
           *
           * @return The value of the simple primitive data type identified by ActualStartTime.
           */
         virtual uci::type::DateTimeTypeValue getActualStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ActualStartTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setActualStartTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActualStartTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActualStartTime is emabled or not
           */
         virtual bool hasActualStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActualStartTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableActualStartTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActualStartTime */
         virtual void clearActualStartTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ActualCompletionTime.
           *
           * @return The value of the simple primitive data type identified by ActualCompletionTime.
           */
         virtual uci::type::DateTimeTypeValue getActualCompletionTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ActualCompletionTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setActualCompletionTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActualCompletionTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActualCompletionTime is emabled or not
           */
         virtual bool hasActualCompletionTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActualCompletionTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableActualCompletionTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActualCompletionTime */
         virtual void clearActualCompletionTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AllProductsAndMessagesProduced.
           *
           * @return The value of the simple primitive data type identified by AllProductsAndMessagesProduced.
           */
         virtual xs::Boolean getAllProductsAndMessagesProduced() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AllProductsAndMessagesProduced.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAllProductsAndMessagesProduced(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AllProductsAndMessagesProduced exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AllProductsAndMessagesProduced is emabled or not
           */
         virtual bool hasAllProductsAndMessagesProduced() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AllProductsAndMessagesProduced
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableAllProductsAndMessagesProduced(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AllProductsAndMessagesProduced */
         virtual void clearAllProductsAndMessagesProduced()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FOV.
           *
           * @return The acecssor that provides access to the complex content that is identified by FOV.
           */
         virtual const uci::type::FOV_ActivityType& getFOV() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FOV.
           *
           * @return The acecssor that provides access to the complex content that is identified by FOV.
           */
         virtual uci::type::FOV_ActivityType& getFOV()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FOV to the contents of the complex content that is accessed by the
           * specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FOV
           */
         virtual void setFOV(const uci::type::FOV_ActivityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FOV exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FOV is emabled or not
           */
         virtual bool hasFOV() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FOV
           *
           * @param type = uci::type::accessorType::fOV_ActivityType This Accessor's accessor type
           */
         virtual void enableFOV(uci::base::accessorType::AccessorType type = uci::type::accessorType::fOV_ActivityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FOV */
         virtual void clearFOV()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PointingActivityReport.
           *
           * @return The acecssor that provides access to the complex content that is identified by PointingActivityReport.
           */
         virtual const uci::type::PointingType& getPointingActivityReport() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PointingActivityReport.
           *
           * @return The acecssor that provides access to the complex content that is identified by PointingActivityReport.
           */
         virtual uci::type::PointingType& getPointingActivityReport()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PointingActivityReport to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PointingActivityReport
           */
         virtual void setPointingActivityReport(const uci::type::PointingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PointingActivityReport exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PointingActivityReport is emabled or not
           */
         virtual bool hasPointingActivityReport() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PointingActivityReport
           *
           * @param type = uci::type::accessorType::pointingType This Accessor's accessor type
           */
         virtual void enablePointingActivityReport(uci::base::accessorType::AccessorType type = uci::type::accessorType::pointingType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PointingActivityReport */
         virtual void clearPointingActivityReport()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapabilityStatus.
           *
           * @return The bounded list identified by SubCapabilityStatus.
           */
         virtual const uci::type::POST_ActivityType::SubCapabilityStatus& getSubCapabilityStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapabilityStatus.
           *
           * @return The bounded list identified by SubCapabilityStatus.
           */
         virtual uci::type::POST_ActivityType::SubCapabilityStatus& getSubCapabilityStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubCapabilityStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubCapabilityStatus(const uci::type::POST_ActivityType::SubCapabilityStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentStatus.
           *
           * @return The bounded list identified by ComponentStatus.
           */
         virtual const uci::type::POST_ActivityType::ComponentStatus& getComponentStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentStatus.
           *
           * @return The bounded list identified by ComponentStatus.
           */
         virtual uci::type::POST_ActivityType::ComponentStatus& getComponentStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ComponentStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setComponentStatus(const uci::type::POST_ActivityType::ComponentStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * CollectionConstraintsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraintsStatus.
           */
         virtual const uci::type::PO_CollectionConstraintsStatusType& getCollectionConstraintsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * CollectionConstraintsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraintsStatus.
           */
         virtual uci::type::PO_CollectionConstraintsStatusType& getCollectionConstraintsStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionConstraintsStatus to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionConstraintsStatus
           */
         virtual void setCollectionConstraintsStatus(const uci::type::PO_CollectionConstraintsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionConstraintsStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionConstraintsStatus is emabled or not
           */
         virtual bool hasCollectionConstraintsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionConstraintsStatus
           *
           * @param type = uci::type::accessorType::pO_CollectionConstraintsStatusType This Accessor's accessor type
           */
         virtual void enableCollectionConstraintsStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_CollectionConstraintsStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CollectionConstraintsStatus */
         virtual void clearCollectionConstraintsStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TrackOptionsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by TrackOptionsStatus.
           */
         virtual const uci::type::POST_TrackDetailsType& getTrackOptionsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TrackOptionsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by TrackOptionsStatus.
           */
         virtual uci::type::POST_TrackDetailsType& getTrackOptionsStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TrackOptionsStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TrackOptionsStatus
           */
         virtual void setTrackOptionsStatus(const uci::type::POST_TrackDetailsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TrackOptionsStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TrackOptionsStatus is emabled or not
           */
         virtual bool hasTrackOptionsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TrackOptionsStatus
           *
           * @param type = uci::type::accessorType::pOST_TrackDetailsType This Accessor's accessor type
           */
         virtual void enableTrackOptionsStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::pOST_TrackDetailsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TrackOptionsStatus */
         virtual void clearTrackOptionsStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutputStatus.
           *
           * @return The bounded list identified by ProductOutputStatus.
           */
         virtual const uci::type::POST_ActivityType::ProductOutputStatus& getProductOutputStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutputStatus.
           *
           * @return The bounded list identified by ProductOutputStatus.
           */
         virtual uci::type::POST_ActivityType::ProductOutputStatus& getProductOutputStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductOutputStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductOutputStatus(const uci::type::POST_ActivityType::ProductOutputStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SearchActivity.
           *
           * @return The acecssor that provides access to the complex content that is identified by SearchActivity.
           */
         virtual const uci::type::SearchTrackActivityDetailsType& getSearchActivity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SearchActivity.
           *
           * @return The acecssor that provides access to the complex content that is identified by SearchActivity.
           */
         virtual uci::type::SearchTrackActivityDetailsType& getSearchActivity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SearchActivity to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SearchActivity
           */
         virtual void setSearchActivity(const uci::type::SearchTrackActivityDetailsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SearchActivity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SearchActivity is emabled or not
           */
         virtual bool hasSearchActivity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SearchActivity
           *
           * @param type = uci::type::accessorType::searchTrackActivityDetailsType This Accessor's accessor type
           */
         virtual void enableSearchActivity(uci::base::accessorType::AccessorType type = uci::type::accessorType::searchTrackActivityDetailsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SearchActivity */
         virtual void clearSearchActivity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TrackActivity.
           *
           * @return The acecssor that provides access to the complex content that is identified by TrackActivity.
           */
         virtual const uci::type::POST_TrackActivityType& getTrackActivity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TrackActivity.
           *
           * @return The acecssor that provides access to the complex content that is identified by TrackActivity.
           */
         virtual uci::type::POST_TrackActivityType& getTrackActivity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TrackActivity to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TrackActivity
           */
         virtual void setTrackActivity(const uci::type::POST_TrackActivityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TrackActivity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TrackActivity is emabled or not
           */
         virtual bool hasTrackActivity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TrackActivity
           *
           * @param type = uci::type::accessorType::pOST_TrackActivityType This Accessor's accessor type
           */
         virtual void enableTrackActivity(uci::base::accessorType::AccessorType type = uci::type::accessorType::pOST_TrackActivityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TrackActivity */
         virtual void clearTrackActivity()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FOR_State.
           *
           * @return The value of the enumeration identified by FOR_State.
           */
         virtual const uci::type::FOR_Enum& getFOR_State() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FOR_State.
           *
           * @return The value of the enumeration identified by FOR_State.
           */
         virtual uci::type::FOR_Enum& getFOR_State()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FOR_State.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFOR_State(const uci::type::FOR_Enum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FOR_State.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFOR_State(uci::type::FOR_Enum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield FOR_Stateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasFOR_State() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getFOR_State will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::fOR_Enum This Accessor's accessor type
           */
         virtual void enableFOR_State(uci::base::accessorType::AccessorType type = uci::type::accessorType::fOR_Enum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearFOR_State()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ThermalBlankingFOR_State.
           *
           * @return The value of the enumeration identified by ThermalBlankingFOR_State.
           */
         virtual const uci::type::FOR_Enum& getThermalBlankingFOR_State() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ThermalBlankingFOR_State.
           *
           * @return The value of the enumeration identified by ThermalBlankingFOR_State.
           */
         virtual uci::type::FOR_Enum& getThermalBlankingFOR_State()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ThermalBlankingFOR_State.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setThermalBlankingFOR_State(const uci::type::FOR_Enum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ThermalBlankingFOR_State.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setThermalBlankingFOR_State(uci::type::FOR_Enum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ThermalBlankingFOR_Stateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasThermalBlankingFOR_State() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getThermalBlankingFOR_State will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::fOR_Enum This Accessor's accessor type
           */
         virtual void enableThermalBlankingFOR_State(uci::base::accessorType::AccessorType type = uci::type::accessorType::fOR_Enum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearThermalBlankingFOR_State()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_ActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_ActivityType to copy from
           */
         POST_ActivityType(const POST_ActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_ActivityType to the contents of the POST_ActivityType on the
           * right hand side (rhs) of the assignment operator.POST_ActivityType [only available to derived classes]
           *
           * @param rhs The POST_ActivityType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::POST_ActivityType
           * @return A constant reference to this POST_ActivityType.
           */
         const POST_ActivityType& operator=(const POST_ActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_ActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_ActivityType_h

