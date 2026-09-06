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
#ifndef Uci__Type__SecurityAuditRecordMDT_h
#define Uci__Type__SecurityAuditRecordMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SecurityEventID_Type_h)
# include "uci/type/SecurityEventID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Type__SecurityEventType_h)
# include "uci/type/SecurityEventType.h"
#endif

#if !defined(Uci__Type__SecurityEventOutcomeEnum_h)
# include "uci/type/SecurityEventOutcomeEnum.h"
#endif

#if !defined(Uci__Type__EventSeverityEnum_h)
# include "uci/type/EventSeverityEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SecurityAuditRecordMDT sequence accessor class */
      class SecurityAuditRecordMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SecurityAuditRecordMDT()
         { }

         /** Returns this accessor's type constant, i.e. SecurityAuditRecordMDT
           *
           * @return This accessor's type constant, i.e. SecurityAuditRecordMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::securityAuditRecordMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SecurityAuditRecordMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SecurityEventID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecurityEventID.
           */
         virtual const uci::type::SecurityEventID_Type& getSecurityEventID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SecurityEventID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecurityEventID.
           */
         virtual uci::type::SecurityEventID_Type& getSecurityEventID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SecurityEventID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SecurityEventID
           */
         virtual void setSecurityEventID(const uci::type::SecurityEventID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EventTimestamp.
           *
           * @return The value of the simple primitive data type identified by EventTimestamp.
           */
         virtual uci::type::DateTimeTypeValue getEventTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EventTimestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEventTimestamp(uci::type::DateTimeTypeValue value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the EventType.
           *
           * @return The acecssor that provides access to the complex content that is identified by EventType.
           */
         virtual const uci::type::SecurityEventType& getEventType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EventType.
           *
           * @return The acecssor that provides access to the complex content that is identified by EventType.
           */
         virtual uci::type::SecurityEventType& getEventType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EventType to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EventType
           */
         virtual void setEventType(const uci::type::SecurityEventType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EventOutcome.
           *
           * @return The value of the enumeration identified by EventOutcome.
           */
         virtual const uci::type::SecurityEventOutcomeEnum& getEventOutcome() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EventOutcome.
           *
           * @return The value of the enumeration identified by EventOutcome.
           */
         virtual uci::type::SecurityEventOutcomeEnum& getEventOutcome()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EventOutcome.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEventOutcome(const uci::type::SecurityEventOutcomeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EventOutcome.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEventOutcome(uci::type::SecurityEventOutcomeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EventSeverity.
           *
           * @return The value of the enumeration identified by EventSeverity.
           */
         virtual const uci::type::EventSeverityEnum& getEventSeverity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EventSeverity.
           *
           * @return The value of the enumeration identified by EventSeverity.
           */
         virtual uci::type::EventSeverityEnum& getEventSeverity()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EventSeverity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEventSeverity(const uci::type::EventSeverityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EventSeverity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEventSeverity(uci::type::EventSeverityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SecurityAuditRecordMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SecurityAuditRecordMDT to copy from
           */
         SecurityAuditRecordMDT(const SecurityAuditRecordMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SecurityAuditRecordMDT to the contents of the
           * SecurityAuditRecordMDT on the right hand side (rhs) of the assignment operator.SecurityAuditRecordMDT [only available
           * to derived classes]
           *
           * @param rhs The SecurityAuditRecordMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::SecurityAuditRecordMDT
           * @return A constant reference to this SecurityAuditRecordMDT.
           */
         const SecurityAuditRecordMDT& operator=(const SecurityAuditRecordMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SecurityAuditRecordMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SecurityAuditRecordMDT_h

