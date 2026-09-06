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
#ifndef Uci__Type__LogMDT_h
#define Uci__Type__LogMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Base__DurationAccessor_h)
# include "uci/base/DurationAccessor.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__LogSeverityEnum_h)
# include "uci/type/LogSeverityEnum.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

#if !defined(Xs__Type__BinaryXmlSchemaPrimitives_h)
# include "xs/type/binaryXmlSchemaPrimitives.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This contains the Log message data. */
      class LogMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~LogMDT()
         { }

         /** Returns this accessor's type constant, i.e. LogMDT
           *
           * @return This accessor's type constant, i.e. LogMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::logMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LogMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique ID of the Component whose log message is in the message. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ComponentID_Type, uci::type::accessorType::componentID_Type> ComponentID;

         /** Indicates the unique ID of the Capability whose log message is in the message. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Returns the value of the SimplePrimitive data type that is identified by the Timestamp.
           *
           * @return The value of the simple primitive data type identified by Timestamp.
           */
         virtual uci::type::DateTimeTypeValue getTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Timestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ServiceUpTime.
           *
           * @return The value of the simple primitive data type identified by ServiceUpTime.
           */
         virtual xs::Duration getServiceUpTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ServiceUpTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setServiceUpTime(xs::Duration value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ServiceUpTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ServiceUpTime is emabled or not
           */
         virtual bool hasServiceUpTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ServiceUpTime
           *
           * @param type = uci::base::accessorType::durationAccessor This Accessor's accessor type
           */
         virtual void enableServiceUpTime(uci::base::accessorType::AccessorType type = uci::base::accessorType::durationAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ServiceUpTime */
         virtual void clearServiceUpTime()
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


         /** Returns the bounded list that is identified by the ComponentID.
           *
           * @return The bounded list identified by ComponentID.
           */
         virtual const uci::type::LogMDT::ComponentID& getComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentID.
           *
           * @return The bounded list identified by ComponentID.
           */
         virtual uci::type::LogMDT::ComponentID& getComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ComponentID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setComponentID(const uci::type::LogMDT::ComponentID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::LogMDT::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::LogMDT::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::LogMDT::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Severity.
           *
           * @return The value of the enumeration identified by Severity.
           */
         virtual const uci::type::LogSeverityEnum& getSeverity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Severity.
           *
           * @return The value of the enumeration identified by Severity.
           */
         virtual uci::type::LogSeverityEnum& getSeverity()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Severity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSeverity(const uci::type::LogSeverityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Severity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSeverity(uci::type::LogSeverityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Label.
           *
           * @return The value of the string data type identified by Label.
           */
         virtual const uci::type::VisibleString256Type& getLabel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Label.
           *
           * @return The value of the string data type identified by Label.
           */
         virtual uci::type::VisibleString256Type& getLabel()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Label to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setLabel(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Label to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setLabel(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Label to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setLabel(const char* value)
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


         /** Returns the accessor to the Data field contained in this message fragment
           *
           * @return The reference to the accessor to the Data field.
           */
         virtual const xs::HexBinary& getData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor to the Data field contained in this message fragment
           *
           * @return The reference to the accessor to the Data field.
           */
         virtual xs::HexBinary& getData()
            throw(uci::base::UCIException) = 0;


         /** Set the contents of the message fragment that this accesssor provides access to to the contents the specified
           * accessor provides access to. A deep copy will be used by this method in setting the contents
           *
           * @param value The accessor to use to set the contents of the message fragment that this accessor provides access to.
           */
         virtual void setData(const xs::HexBinary& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Datais available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasData() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getData will return the optional field and not throw an exception when invoked.
           *
           * @param type = xs::accessorType::hexBinary This Accessor's accessor type
           */
         virtual void enableData(uci::base::accessorType::AccessorType type = xs::accessorType::hexBinary)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearData()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         LogMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LogMDT to copy from
           */
         LogMDT(const LogMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LogMDT to the contents of the LogMDT on the right hand side (rhs)
           * of the assignment operator.LogMDT [only available to derived classes]
           *
           * @param rhs The LogMDT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::LogMDT
           * @return A constant reference to this LogMDT.
           */
         const LogMDT& operator=(const LogMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LogMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LogMDT_h

