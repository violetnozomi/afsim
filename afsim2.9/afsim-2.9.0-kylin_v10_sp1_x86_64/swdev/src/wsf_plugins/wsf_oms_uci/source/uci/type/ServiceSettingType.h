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
#ifndef Uci__Type__ServiceSettingType_h
#define Uci__Type__ServiceSettingType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ServiceSettingEnum_h)
# include "uci/type/ServiceSettingEnum.h"
#endif

#if !defined(Uci__Type__ServiceSettingsParameterType_h)
# include "uci/type/ServiceSettingsParameterType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EventSeverityEnum_h)
# include "uci/type/EventSeverityEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ServiceSettingType sequence accessor class */
      class ServiceSettingType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ServiceSettingType()
         { }

         /** Returns this accessor's type constant, i.e. ServiceSettingType
           *
           * @return This accessor's type constant, i.e. ServiceSettingType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::serviceSettingType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ServiceSettingType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Valid when ServiceSetting is AUDIT_LOGGING; 1..n may be valid for Audit logging. [Minimum occurrences: 0] [Maximum
           * occurrences: 4]
           */
         typedef uci::base::BoundedList<uci::type::EventSeverityEnum, uci::type::accessorType::eventSeverityEnum> EventSeverityClass;

         /** Returns the value of the enumeration that is identified by the ServiceSetting.
           *
           * @return The value of the enumeration identified by ServiceSetting.
           */
         virtual const uci::type::ServiceSettingEnum& getServiceSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ServiceSetting.
           *
           * @return The value of the enumeration identified by ServiceSetting.
           */
         virtual uci::type::ServiceSettingEnum& getServiceSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ServiceSetting.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setServiceSetting(const uci::type::ServiceSettingEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ServiceSetting.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setServiceSetting(uci::type::ServiceSettingEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ServiceSettingParameter.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceSettingParameter.
           */
         virtual const uci::type::ServiceSettingsParameterType& getServiceSettingParameter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ServiceSettingParameter.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceSettingParameter.
           */
         virtual uci::type::ServiceSettingsParameterType& getServiceSettingParameter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ServiceSettingParameter to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ServiceSettingParameter
           */
         virtual void setServiceSettingParameter(const uci::type::ServiceSettingsParameterType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ServiceSettingParameter exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ServiceSettingParameter is emabled or not
           */
         virtual bool hasServiceSettingParameter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ServiceSettingParameter
           *
           * @param type = uci::type::accessorType::serviceSettingsParameterType This Accessor's accessor type
           */
         virtual void enableServiceSettingParameter(uci::base::accessorType::AccessorType type = uci::type::accessorType::serviceSettingsParameterType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ServiceSettingParameter */
         virtual void clearServiceSettingParameter()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EventSeverityClass.
           *
           * @return The bounded list identified by EventSeverityClass.
           */
         virtual const uci::type::ServiceSettingType::EventSeverityClass& getEventSeverityClass() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EventSeverityClass.
           *
           * @return The bounded list identified by EventSeverityClass.
           */
         virtual uci::type::ServiceSettingType::EventSeverityClass& getEventSeverityClass()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EventSeverityClass.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEventSeverityClass(const uci::type::ServiceSettingType::EventSeverityClass& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ServiceSettingType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ServiceSettingType to copy from
           */
         ServiceSettingType(const ServiceSettingType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ServiceSettingType to the contents of the ServiceSettingType on
           * the right hand side (rhs) of the assignment operator.ServiceSettingType [only available to derived classes]
           *
           * @param rhs The ServiceSettingType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ServiceSettingType
           * @return A constant reference to this ServiceSettingType.
           */
         const ServiceSettingType& operator=(const ServiceSettingType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ServiceSettingType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ServiceSettingType_h

