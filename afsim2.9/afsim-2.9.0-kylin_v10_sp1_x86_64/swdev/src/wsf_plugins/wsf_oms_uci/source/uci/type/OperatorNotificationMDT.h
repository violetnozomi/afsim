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
#ifndef Uci__Type__OperatorNotificationMDT_h
#define Uci__Type__OperatorNotificationMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__OperatorNotificationID_Type_h)
# include "uci/type/OperatorNotificationID_Type.h"
#endif

#if !defined(Uci__Type__ID_Type_h)
# include "uci/type/ID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__VisibleString32Type_h)
# include "uci/type/VisibleString32Type.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

#if !defined(Uci__Type__NotificationCategoryEnum_h)
# include "uci/type/NotificationCategoryEnum.h"
#endif

#if !defined(Uci__Type__NotificationSeverityEnum_h)
# include "uci/type/NotificationSeverityEnum.h"
#endif

#if !defined(Uci__Type__Point2D_Type_h)
# include "uci/type/Point2D_Type.h"
#endif

#if !defined(Uci__Type__NotificationSourceType_h)
# include "uci/type/NotificationSourceType.h"
#endif

#if !defined(Uci__Type__OperatorNotificationActionType_h)
# include "uci/type/OperatorNotificationActionType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__AuralAlertType_h)
# include "uci/type/AuralAlertType.h"
#endif

#if !defined(Uci__Type__OperatorRoleType_h)
# include "uci/type/OperatorRoleType.h"
#endif

#if !defined(Uci__Type__AssociatedMessageType_h)
# include "uci/type/AssociatedMessageType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OperatorNotificationMDT sequence accessor class */
      class OperatorNotificationMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OperatorNotificationMDT()
         { }

         /** Returns this accessor's type constant, i.e. OperatorNotificationMDT
           *
           * @return This accessor's type constant, i.e. OperatorNotificationMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::operatorNotificationMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OperatorNotificationMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** List of parameters used as substitution arguments when applying notification templates. If a notification template
           * syntax defined a substitution token (e.g. $#) in the Title, the template parameters would be used to replace those
           * tokens. For example, given a template Title of “System $1 violated zone $2” an OperatorNotification that applies that
           * template would replace the $1 token with the first template parameter and the $2 with the second template parameter.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::VisibleString32Type, uci::type::accessorType::visibleString32Type> OperatorNotificationTemplateParameters;

         /** [Minimum inclusive: 0] [Maximum inclusive: 100] */
         typedef uci::base::UnsignedIntAccessor Priority;
         //  The type of primitive data accessed by Priority
         typedef xs::UnsignedInt PriorityValue;

         /** Indicates a message that is associated with or the cause of the operator notification. This element should be omitted
           * if an input message was not the cause of the notification. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::AssociatedMessageType, uci::type::accessorType::associatedMessageType> AssociatedMessage;

         /** Returns the accessor that provides access to the complex content that is identified by the OperatorNotificationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperatorNotificationID.
           */
         virtual const uci::type::OperatorNotificationID_Type& getOperatorNotificationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OperatorNotificationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperatorNotificationID.
           */
         virtual uci::type::OperatorNotificationID_Type& getOperatorNotificationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OperatorNotificationID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OperatorNotificationID
           */
         virtual void setOperatorNotificationID(const uci::type::OperatorNotificationID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * OperatorNotificationTemplateID.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      OperatorNotificationTemplateID.
           */
         virtual const uci::type::ID_Type& getOperatorNotificationTemplateID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * OperatorNotificationTemplateID.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      OperatorNotificationTemplateID.
           */
         virtual uci::type::ID_Type& getOperatorNotificationTemplateID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OperatorNotificationTemplateID to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OperatorNotificationTemplateID
           */
         virtual void setOperatorNotificationTemplateID(const uci::type::ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OperatorNotificationTemplateID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OperatorNotificationTemplateID is emabled or not
           */
         virtual bool hasOperatorNotificationTemplateID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OperatorNotificationTemplateID
           *
           * @param type = uci::type::accessorType::iD_Type This Accessor's accessor type
           */
         virtual void enableOperatorNotificationTemplateID(uci::base::accessorType::AccessorType type = uci::type::accessorType::iD_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OperatorNotificationTemplateID */
         virtual void clearOperatorNotificationTemplateID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OperatorNotificationTemplateParameters.
           *
           * @return The bounded list identified by OperatorNotificationTemplateParameters.
           */
         virtual const uci::type::OperatorNotificationMDT::OperatorNotificationTemplateParameters& getOperatorNotificationTemplateParameters() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OperatorNotificationTemplateParameters.
           *
           * @return The bounded list identified by OperatorNotificationTemplateParameters.
           */
         virtual uci::type::OperatorNotificationMDT::OperatorNotificationTemplateParameters& getOperatorNotificationTemplateParameters()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the OperatorNotificationTemplateParameters.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOperatorNotificationTemplateParameters(const uci::type::OperatorNotificationMDT::OperatorNotificationTemplateParameters& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Title.
           *
           * @return The value of the string data type identified by Title.
           */
         virtual const uci::type::VisibleString256Type& getTitle() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Title.
           *
           * @return The value of the string data type identified by Title.
           */
         virtual uci::type::VisibleString256Type& getTitle()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Title to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setTitle(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Title to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTitle(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Title to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTitle(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Details.
           *
           * @return The value of the string data type identified by Details.
           */
         virtual const uci::type::VisibleString1024Type& getDetails() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Details.
           *
           * @return The value of the string data type identified by Details.
           */
         virtual uci::type::VisibleString1024Type& getDetails()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Details to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setDetails(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Details to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDetails(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Details to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDetails(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Details exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Details is emabled or not
           */
         virtual bool hasDetails() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Details
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableDetails(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Details */
         virtual void clearDetails()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Category.
           *
           * @return The value of the enumeration identified by Category.
           */
         virtual const uci::type::NotificationCategoryEnum& getCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Category.
           *
           * @return The value of the enumeration identified by Category.
           */
         virtual uci::type::NotificationCategoryEnum& getCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Category.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCategory(const uci::type::NotificationCategoryEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Category.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCategory(uci::type::NotificationCategoryEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Severity.
           *
           * @return The value of the enumeration identified by Severity.
           */
         virtual const uci::type::NotificationSeverityEnum& getSeverity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Severity.
           *
           * @return The value of the enumeration identified by Severity.
           */
         virtual uci::type::NotificationSeverityEnum& getSeverity()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Severity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSeverity(const uci::type::NotificationSeverityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Severity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSeverity(uci::type::NotificationSeverityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by Location.
           */
         virtual const uci::type::Point2D_Type& getLocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by Location.
           */
         virtual uci::type::Point2D_Type& getLocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Location to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Location
           */
         virtual void setLocation(const uci::type::Point2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Location exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Location is emabled or not
           */
         virtual bool hasLocation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Location
           *
           * @param type = uci::type::accessorType::point2D_Type This Accessor's accessor type
           */
         virtual void enableLocation(uci::base::accessorType::AccessorType type = uci::type::accessorType::point2D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Location */
         virtual void clearLocation()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Source.
           *
           * @return The acecssor that provides access to the complex content that is identified by Source.
           */
         virtual const uci::type::NotificationSourceType& getSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Source.
           *
           * @return The acecssor that provides access to the complex content that is identified by Source.
           */
         virtual uci::type::NotificationSourceType& getSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Source to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Source
           */
         virtual void setSource(const uci::type::NotificationSourceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Source exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Source is emabled or not
           */
         virtual bool hasSource() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Source
           *
           * @param type = uci::type::accessorType::notificationSourceType This Accessor's accessor type
           */
         virtual void enableSource(uci::base::accessorType::AccessorType type = uci::type::accessorType::notificationSourceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Source */
         virtual void clearSource()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Actions.
           *
           * @return The acecssor that provides access to the complex content that is identified by Actions.
           */
         virtual const uci::type::OperatorNotificationActionType& getActions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Actions.
           *
           * @return The acecssor that provides access to the complex content that is identified by Actions.
           */
         virtual uci::type::OperatorNotificationActionType& getActions()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Actions to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Actions
           */
         virtual void setActions(const uci::type::OperatorNotificationActionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Actions exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Actions is emabled or not
           */
         virtual bool hasActions() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Actions
           *
           * @param type = uci::type::accessorType::operatorNotificationActionType This Accessor's accessor type
           */
         virtual void enableActions(uci::base::accessorType::AccessorType type = uci::type::accessorType::operatorNotificationActionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Actions */
         virtual void clearActions()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Count.
           *
           * @return The value of the simple primitive data type identified by Count.
           */
         virtual xs::UnsignedInt getCount() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Count.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCount(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Count exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Count is emabled or not
           */
         virtual bool hasCount() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Count
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableCount(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Count */
         virtual void clearCount()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OccurenceTimestamp.
           *
           * @return The value of the simple primitive data type identified by OccurenceTimestamp.
           */
         virtual uci::type::DateTimeTypeValue getOccurenceTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OccurenceTimestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOccurenceTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OccurenceTimestamp exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OccurenceTimestamp is emabled or not
           */
         virtual bool hasOccurenceTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OccurenceTimestamp
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableOccurenceTimestamp(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OccurenceTimestamp */
         virtual void clearOccurenceTimestamp()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Priority.
           *
           * @return The value of the simple primitive data type identified by Priority.
           */
         virtual uci::type::OperatorNotificationMDT::PriorityValue getPriority() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Priority.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPriority(uci::type::OperatorNotificationMDT::PriorityValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Priority exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Priority is emabled or not
           */
         virtual bool hasPriority() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Priority
           *
           * @param type = uci::type::operatorNotificationMDT::accessorType::priority This Accessor's accessor type
           */
         virtual void enablePriority(uci::base::accessorType::AccessorType type = uci::type::operatorNotificationMDT::accessorType::priority)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Priority */
         virtual void clearPriority()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AuralAlert.
           *
           * @return The acecssor that provides access to the complex content that is identified by AuralAlert.
           */
         virtual const uci::type::AuralAlertType& getAuralAlert() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AuralAlert.
           *
           * @return The acecssor that provides access to the complex content that is identified by AuralAlert.
           */
         virtual uci::type::AuralAlertType& getAuralAlert()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AuralAlert to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AuralAlert
           */
         virtual void setAuralAlert(const uci::type::AuralAlertType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AuralAlert exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AuralAlert is emabled or not
           */
         virtual bool hasAuralAlert() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AuralAlert
           *
           * @param type = uci::type::accessorType::auralAlertType This Accessor's accessor type
           */
         virtual void enableAuralAlert(uci::base::accessorType::AccessorType type = uci::type::accessorType::auralAlertType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AuralAlert */
         virtual void clearAuralAlert()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Destinations.
           *
           * @return The acecssor that provides access to the complex content that is identified by Destinations.
           */
         virtual const uci::type::OperatorRoleType& getDestinations() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Destinations.
           *
           * @return The acecssor that provides access to the complex content that is identified by Destinations.
           */
         virtual uci::type::OperatorRoleType& getDestinations()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Destinations to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Destinations
           */
         virtual void setDestinations(const uci::type::OperatorRoleType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Destinations exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Destinations is emabled or not
           */
         virtual bool hasDestinations() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Destinations
           *
           * @param type = uci::type::accessorType::operatorRoleType This Accessor's accessor type
           */
         virtual void enableDestinations(uci::base::accessorType::AccessorType type = uci::type::accessorType::operatorRoleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Destinations */
         virtual void clearDestinations()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedMessage.
           *
           * @return The bounded list identified by AssociatedMessage.
           */
         virtual const uci::type::OperatorNotificationMDT::AssociatedMessage& getAssociatedMessage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedMessage.
           *
           * @return The bounded list identified by AssociatedMessage.
           */
         virtual uci::type::OperatorNotificationMDT::AssociatedMessage& getAssociatedMessage()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociatedMessage.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociatedMessage(const uci::type::OperatorNotificationMDT::AssociatedMessage& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OperatorNotificationMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OperatorNotificationMDT to copy from
           */
         OperatorNotificationMDT(const OperatorNotificationMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OperatorNotificationMDT to the contents of the
           * OperatorNotificationMDT on the right hand side (rhs) of the assignment operator.OperatorNotificationMDT [only
           * available to derived classes]
           *
           * @param rhs The OperatorNotificationMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::OperatorNotificationMDT
           * @return A constant reference to this OperatorNotificationMDT.
           */
         const OperatorNotificationMDT& operator=(const OperatorNotificationMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OperatorNotificationMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OperatorNotificationMDT_h

