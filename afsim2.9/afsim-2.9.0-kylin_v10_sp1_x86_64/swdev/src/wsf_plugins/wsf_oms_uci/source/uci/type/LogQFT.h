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
#ifndef Uci__Type__LogQFT_h
#define Uci__Type__LogQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
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

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Queries the log data in the system based on its registered properties */
      class LogQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~LogQFT()
         { }

         /** Returns this accessor's type constant, i.e. LogQFT
           *
           * @return This accessor's type constant, i.e. LogQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::logQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LogQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique ID of the Component whose log message is in the message. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ComponentID_Type, uci::type::accessorType::componentID_Type> ComponentID;

         /** Indicates the unique ID of the Capability whose log message is in the message. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Indicates the severity level of the log message. Service contracts should indicate which enums are supported.
           * [Minimum occurrences: 0] [Maximum occurrences: 6]
           */
         typedef uci::base::BoundedList<uci::type::LogSeverityEnum, uci::type::accessorType::logSeverityEnum> Severity;

         /** Indicates a system specific label such as method/function, CSCI, etc where the log message originated. Used for
           * differentiation and filtering of log messages. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::VisibleString256Type, uci::type::accessorType::visibleString256Type> Label;

         /** Contains the actual human readable message from the service/subsystem/component/capability that is to be logged.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::VisibleString1024Type, uci::type::accessorType::visibleString1024Type> Details;

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


         /** Returns the bounded list that is identified by the ComponentID.
           *
           * @return The bounded list identified by ComponentID.
           */
         virtual const uci::type::LogQFT::ComponentID& getComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentID.
           *
           * @return The bounded list identified by ComponentID.
           */
         virtual uci::type::LogQFT::ComponentID& getComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ComponentID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setComponentID(const uci::type::LogQFT::ComponentID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::LogQFT::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::LogQFT::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::LogQFT::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Severity.
           *
           * @return The bounded list identified by Severity.
           */
         virtual const uci::type::LogQFT::Severity& getSeverity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Severity.
           *
           * @return The bounded list identified by Severity.
           */
         virtual uci::type::LogQFT::Severity& getSeverity()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Severity.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSeverity(const uci::type::LogQFT::Severity& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Label.
           *
           * @return The bounded list identified by Label.
           */
         virtual const uci::type::LogQFT::Label& getLabel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Label.
           *
           * @return The bounded list identified by Label.
           */
         virtual uci::type::LogQFT::Label& getLabel()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Label.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setLabel(const uci::type::LogQFT::Label& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Details.
           *
           * @return The bounded list identified by Details.
           */
         virtual const uci::type::LogQFT::Details& getDetails() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Details.
           *
           * @return The bounded list identified by Details.
           */
         virtual uci::type::LogQFT::Details& getDetails()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Details.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDetails(const uci::type::LogQFT::Details& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         LogQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LogQFT to copy from
           */
         LogQFT(const LogQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LogQFT to the contents of the LogQFT on the right hand side (rhs)
           * of the assignment operator.LogQFT [only available to derived classes]
           *
           * @param rhs The LogQFT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::LogQFT
           * @return A constant reference to this LogQFT.
           */
         const LogQFT& operator=(const LogQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LogQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LogQFT_h

