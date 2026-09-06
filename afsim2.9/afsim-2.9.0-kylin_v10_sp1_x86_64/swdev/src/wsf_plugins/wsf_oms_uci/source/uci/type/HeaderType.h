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
#ifndef Uci__Type__HeaderType_h
#define Uci__Type__HeaderType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

#if !defined(Uci__Type__MessageModeEnum_h)
# include "uci/type/MessageModeEnum.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Type__MissionID_Type_h)
# include "uci/type/MissionID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type is for the Header used by all messages in the UCI interface. */
      class HeaderType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~HeaderType()
         { }

         /** Returns this accessor's type constant, i.e. HeaderType
           *
           * @return This accessor's type constant, i.e. HeaderType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::headerType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const HeaderType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the schema version used to generate the message. UCI v67 would be indicated as "067.0", UCI v67.1 would be
           * indicated as "067.1", etc. For engineering development versions only, an optional dot and 15 alphanumerics is
           * allowed.; [Minimum length: 5] [Maximum length: 21] [Pattern: "[0-9]{3}\.[0-9]{1}(\.[0-9A-Za-z_]{1,15})?"]
           */
         typedef xs::String SchemaVersion;

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


         /** Returns the value of the string data type that is identified by the SchemaVersion.
           *
           * @return The value of the string data type identified by SchemaVersion.
           */
         virtual const uci::type::HeaderType::SchemaVersion& getSchemaVersion() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SchemaVersion.
           *
           * @return The value of the string data type identified by SchemaVersion.
           */
         virtual uci::type::HeaderType::SchemaVersion& getSchemaVersion()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SchemaVersion to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSchemaVersion(const uci::type::HeaderType::SchemaVersion& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SchemaVersion to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSchemaVersion(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SchemaVersion to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSchemaVersion(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Mode.
           *
           * @return The value of the enumeration identified by Mode.
           */
         virtual const uci::type::MessageModeEnum& getMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Mode.
           *
           * @return The value of the enumeration identified by Mode.
           */
         virtual uci::type::MessageModeEnum& getMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Mode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMode(const uci::type::MessageModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Mode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMode(uci::type::MessageModeEnum::EnumerationItem value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the MissionID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionID.
           */
         virtual const uci::type::MissionID_Type& getMissionID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionID.
           */
         virtual uci::type::MissionID_Type& getMissionID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionID
           */
         virtual void setMissionID(const uci::type::MissionID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionID is emabled or not
           */
         virtual bool hasMissionID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionID
           *
           * @param type = uci::type::accessorType::missionID_Type This Accessor's accessor type
           */
         virtual void enableMissionID(uci::base::accessorType::AccessorType type = uci::type::accessorType::missionID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionID */
         virtual void clearMissionID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         HeaderType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The HeaderType to copy from
           */
         HeaderType(const HeaderType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this HeaderType to the contents of the HeaderType on the right hand
           * side (rhs) of the assignment operator.HeaderType [only available to derived classes]
           *
           * @param rhs The HeaderType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::HeaderType
           * @return A constant reference to this HeaderType.
           */
         const HeaderType& operator=(const HeaderType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // HeaderType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__HeaderType_h

