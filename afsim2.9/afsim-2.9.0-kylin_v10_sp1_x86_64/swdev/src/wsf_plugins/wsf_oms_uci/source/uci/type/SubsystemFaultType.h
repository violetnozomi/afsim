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
#ifndef Uci__Type__SubsystemFaultType_h
#define Uci__Type__SubsystemFaultType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__FaultID_Type_h)
# include "uci/type/FaultID_Type.h"
#endif

#if !defined(Uci__Type__FaultSeverityEnum_h)
# include "uci/type/FaultSeverityEnum.h"
#endif

#if !defined(Uci__Type__FaultStateEnum_h)
# include "uci/type/FaultStateEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FaultDataType_h)
# include "uci/type/FaultDataType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__SupportCapabilityID_Type_h)
# include "uci/type/SupportCapabilityID_Type.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Contains fault data for reporting by subsystems and services. */
      class SubsystemFaultType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SubsystemFaultType()
         { }

         /** Returns this accessor's type constant, i.e. SubsystemFaultType
           *
           * @return This accessor's type constant, i.e. SubsystemFaultType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::subsystemFaultType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SubsystemFaultType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Additional fault data which contians key/value pairs with format and unit indicated for the data value. This data
           * supports fault isolation and helps to minimize false alarms. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FaultDataType, uci::type::accessorType::faultDataType> FaultData;

         /** Indicates the unique ID of a Capability related to the fault. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Indicates the unique ID of a SupportCapability related to the fault. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SupportCapabilityID_Type, uci::type::accessorType::supportCapabilityID_Type> SupportCapabilityID;

         /** Indicates the unique ID of a Component related to the fault. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ComponentID_Type, uci::type::accessorType::componentID_Type> ComponentID;

         /** Returns the accessor that provides access to the complex content that is identified by the FaultID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FaultID.
           */
         virtual const uci::type::FaultID_Type& getFaultID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FaultID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FaultID.
           */
         virtual uci::type::FaultID_Type& getFaultID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FaultID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FaultID
           */
         virtual void setFaultID(const uci::type::FaultID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Severity.
           *
           * @return The value of the enumeration identified by Severity.
           */
         virtual const uci::type::FaultSeverityEnum& getSeverity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Severity.
           *
           * @return The value of the enumeration identified by Severity.
           */
         virtual uci::type::FaultSeverityEnum& getSeverity()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Severity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSeverity(const uci::type::FaultSeverityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Severity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSeverity(uci::type::FaultSeverityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Severityis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSeverity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSeverity will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::faultSeverityEnum This Accessor's accessor type
           */
         virtual void enableSeverity(uci::base::accessorType::AccessorType type = uci::type::accessorType::faultSeverityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSeverity()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the State.
           *
           * @return The value of the enumeration identified by State.
           */
         virtual const uci::type::FaultStateEnum& getState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the State.
           *
           * @return The value of the enumeration identified by State.
           */
         virtual uci::type::FaultStateEnum& getState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the State.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setState(const uci::type::FaultStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the State.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setState(uci::type::FaultStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Stateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasState() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getState will return the optional field and not throw an exception when invoked.
           *
           * @param type = uci::type::accessorType::faultStateEnum This Accessor's accessor type
           */
         virtual void enableState(uci::base::accessorType::AccessorType type = uci::type::accessorType::faultStateEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearState()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FaultData.
           *
           * @return The bounded list identified by FaultData.
           */
         virtual const uci::type::SubsystemFaultType::FaultData& getFaultData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FaultData.
           *
           * @return The bounded list identified by FaultData.
           */
         virtual uci::type::SubsystemFaultType::FaultData& getFaultData()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FaultData.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFaultData(const uci::type::SubsystemFaultType::FaultData& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DetectionTime.
           *
           * @return The value of the simple primitive data type identified by DetectionTime.
           */
         virtual uci::type::DateTimeTypeValue getDetectionTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DetectionTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDetectionTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DetectionTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DetectionTime is emabled or not
           */
         virtual bool hasDetectionTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DetectionTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableDetectionTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DetectionTime */
         virtual void clearDetectionTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FaultCode.
           *
           * @return The value of the string data type identified by FaultCode.
           */
         virtual const uci::type::VisibleString256Type& getFaultCode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FaultCode.
           *
           * @return The value of the string data type identified by FaultCode.
           */
         virtual uci::type::VisibleString256Type& getFaultCode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FaultCode to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFaultCode(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FaultCode to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFaultCode(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FaultCode to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFaultCode(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FaultDescription.
           *
           * @return The value of the string data type identified by FaultDescription.
           */
         virtual const uci::type::VisibleString256Type& getFaultDescription() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FaultDescription.
           *
           * @return The value of the string data type identified by FaultDescription.
           */
         virtual uci::type::VisibleString256Type& getFaultDescription()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FaultDescription to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFaultDescription(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FaultDescription to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFaultDescription(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FaultDescription to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFaultDescription(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FaultDescription exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FaultDescription is emabled or not
           */
         virtual bool hasFaultDescription() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FaultDescription
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableFaultDescription(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FaultDescription */
         virtual void clearFaultDescription()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual const uci::type::SubsystemID_Type& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual uci::type::SubsystemID_Type& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubsystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubsystemID
           */
         virtual void setSubsystemID(const uci::type::SubsystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SubsystemID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SubsystemID is emabled or not
           */
         virtual bool hasSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SubsystemID
           *
           * @param type = uci::type::accessorType::subsystemID_Type This Accessor's accessor type
           */
         virtual void enableSubsystemID(uci::base::accessorType::AccessorType type = uci::type::accessorType::subsystemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SubsystemID */
         virtual void clearSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceID.
           */
         virtual const uci::type::ServiceID_Type& getServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceID.
           */
         virtual uci::type::ServiceID_Type& getServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ServiceID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ServiceID
           */
         virtual void setServiceID(const uci::type::ServiceID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ServiceID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ServiceID is emabled or not
           */
         virtual bool hasServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ServiceID
           *
           * @param type = uci::type::accessorType::serviceID_Type This Accessor's accessor type
           */
         virtual void enableServiceID(uci::base::accessorType::AccessorType type = uci::type::accessorType::serviceID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ServiceID */
         virtual void clearServiceID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::SubsystemFaultType::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::SubsystemFaultType::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::SubsystemFaultType::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportCapabilityID.
           *
           * @return The bounded list identified by SupportCapabilityID.
           */
         virtual const uci::type::SubsystemFaultType::SupportCapabilityID& getSupportCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportCapabilityID.
           *
           * @return The bounded list identified by SupportCapabilityID.
           */
         virtual uci::type::SubsystemFaultType::SupportCapabilityID& getSupportCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportCapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportCapabilityID(const uci::type::SubsystemFaultType::SupportCapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentID.
           *
           * @return The bounded list identified by ComponentID.
           */
         virtual const uci::type::SubsystemFaultType::ComponentID& getComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentID.
           *
           * @return The bounded list identified by ComponentID.
           */
         virtual uci::type::SubsystemFaultType::ComponentID& getComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ComponentID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setComponentID(const uci::type::SubsystemFaultType::ComponentID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemFaultType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemFaultType to copy from
           */
         SubsystemFaultType(const SubsystemFaultType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemFaultType to the contents of the SubsystemFaultType on
           * the right hand side (rhs) of the assignment operator.SubsystemFaultType [only available to derived classes]
           *
           * @param rhs The SubsystemFaultType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::SubsystemFaultType
           * @return A constant reference to this SubsystemFaultType.
           */
         const SubsystemFaultType& operator=(const SubsystemFaultType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemFaultType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SubsystemFaultType_h

