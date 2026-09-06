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
#ifndef Uci__Type__SecurityEventType_h
#define Uci__Type__SecurityEventType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SecurityAccessType_h)
# include "uci/type/SecurityAccessType.h"
#endif

#if !defined(Uci__Type__SecurityAuthenticationType_h)
# include "uci/type/SecurityAuthenticationType.h"
#endif

#if !defined(Uci__Type__SecurityIntegrityType_h)
# include "uci/type/SecurityIntegrityType.h"
#endif

#if !defined(Uci__Type__SecurityAuditManagementType_h)
# include "uci/type/SecurityAuditManagementType.h"
#endif

#if !defined(Uci__Type__SecurityFileManagementType_h)
# include "uci/type/SecurityFileManagementType.h"
#endif

#if !defined(Uci__Type__SecurityKeyManagementType_h)
# include "uci/type/SecurityKeyManagementType.h"
#endif

#if !defined(Uci__Type__SecurityPolicyManagementType_h)
# include "uci/type/SecurityPolicyManagementType.h"
#endif

#if !defined(Uci__Type__SecuritySystemType_h)
# include "uci/type/SecuritySystemType.h"
#endif

#if !defined(Uci__Type__SecurityIntrusionDetectionType_h)
# include "uci/type/SecurityIntrusionDetectionType.h"
#endif

#if !defined(Uci__Type__SecuritySanitizationType_h)
# include "uci/type/SecuritySanitizationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SecurityEventType choice accessor class */
      class SecurityEventType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SecurityEventType()
         { }

         /** Returns this accessor's type constant, i.e. SecurityEventType
           *
           * @return This accessor's type constant, i.e. SecurityEventType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::securityEventType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SecurityEventType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum SecurityEventTypeChoice {
            SECURITYEVENTTYPE_CHOICE_NONE,
            SECURITYEVENTTYPE_CHOICE_ACCOUNTACCESS,
            SECURITYEVENTTYPE_CHOICE_AUTHENTICATION,
            SECURITYEVENTTYPE_CHOICE_INTEGRITY,
            SECURITYEVENTTYPE_CHOICE_AUDITMANAGEMENT,
            SECURITYEVENTTYPE_CHOICE_FILEMANAGEMENT,
            SECURITYEVENTTYPE_CHOICE_KEYMANAGEMENT,
            SECURITYEVENTTYPE_CHOICE_POLICYMANAGEMENT,
            SECURITYEVENTTYPE_CHOICE_SYSTEM,
            SECURITYEVENTTYPE_CHOICE_INTRUSIONDETECTION,
            SECURITYEVENTTYPE_CHOICE_SANITIZATION
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual SecurityEventTypeChoice getSecurityEventTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getSecurityEventTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the
           * access methods associated with the chosen element will be enabled and will provide access to the chosen element.
           *
           * @param chosenElementOrdinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setSecurityEventTypeChoiceOrdinal(SecurityEventTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AccountAccess.
           *
           * @return The acecssor that provides access to the complex content that is identified by AccountAccess.
           */
         virtual const uci::type::SecurityAccessType& getAccountAccess() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AccountAccess.
           *
           * @return The acecssor that provides access to the complex content that is identified by AccountAccess.
           */
         virtual uci::type::SecurityAccessType& getAccountAccess()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AccountAccess to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AccountAccess
           */
         virtual void setAccountAccess(const uci::type::SecurityAccessType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Authentication.
           *
           * @return The acecssor that provides access to the complex content that is identified by Authentication.
           */
         virtual const uci::type::SecurityAuthenticationType& getAuthentication() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Authentication.
           *
           * @return The acecssor that provides access to the complex content that is identified by Authentication.
           */
         virtual uci::type::SecurityAuthenticationType& getAuthentication()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Authentication to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Authentication
           */
         virtual void setAuthentication(const uci::type::SecurityAuthenticationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Integrity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Integrity.
           */
         virtual const uci::type::SecurityIntegrityType& getIntegrity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Integrity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Integrity.
           */
         virtual uci::type::SecurityIntegrityType& getIntegrity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Integrity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Integrity
           */
         virtual void setIntegrity(const uci::type::SecurityIntegrityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AuditManagement.
           *
           * @return The acecssor that provides access to the complex content that is identified by AuditManagement.
           */
         virtual const uci::type::SecurityAuditManagementType& getAuditManagement() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AuditManagement.
           *
           * @return The acecssor that provides access to the complex content that is identified by AuditManagement.
           */
         virtual uci::type::SecurityAuditManagementType& getAuditManagement()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AuditManagement to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AuditManagement
           */
         virtual void setAuditManagement(const uci::type::SecurityAuditManagementType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileManagement.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileManagement.
           */
         virtual const uci::type::SecurityFileManagementType& getFileManagement() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileManagement.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileManagement.
           */
         virtual uci::type::SecurityFileManagementType& getFileManagement()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FileManagement to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FileManagement
           */
         virtual void setFileManagement(const uci::type::SecurityFileManagementType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the KeyManagement.
           *
           * @return The acecssor that provides access to the complex content that is identified by KeyManagement.
           */
         virtual const uci::type::SecurityKeyManagementType& getKeyManagement() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the KeyManagement.
           *
           * @return The acecssor that provides access to the complex content that is identified by KeyManagement.
           */
         virtual uci::type::SecurityKeyManagementType& getKeyManagement()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the KeyManagement to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by KeyManagement
           */
         virtual void setKeyManagement(const uci::type::SecurityKeyManagementType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PolicyManagement.
           *
           * @return The acecssor that provides access to the complex content that is identified by PolicyManagement.
           */
         virtual const uci::type::SecurityPolicyManagementType& getPolicyManagement() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PolicyManagement.
           *
           * @return The acecssor that provides access to the complex content that is identified by PolicyManagement.
           */
         virtual uci::type::SecurityPolicyManagementType& getPolicyManagement()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PolicyManagement to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PolicyManagement
           */
         virtual void setPolicyManagement(const uci::type::SecurityPolicyManagementType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the System.
           *
           * @return The acecssor that provides access to the complex content that is identified by System.
           */
         virtual const uci::type::SecuritySystemType& getSystem() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the System.
           *
           * @return The acecssor that provides access to the complex content that is identified by System.
           */
         virtual uci::type::SecuritySystemType& getSystem()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the System to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by System
           */
         virtual void setSystem(const uci::type::SecuritySystemType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the IntrusionDetection.
           *
           * @return The acecssor that provides access to the complex content that is identified by IntrusionDetection.
           */
         virtual const uci::type::SecurityIntrusionDetectionType& getIntrusionDetection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the IntrusionDetection.
           *
           * @return The acecssor that provides access to the complex content that is identified by IntrusionDetection.
           */
         virtual uci::type::SecurityIntrusionDetectionType& getIntrusionDetection()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the IntrusionDetection to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by IntrusionDetection
           */
         virtual void setIntrusionDetection(const uci::type::SecurityIntrusionDetectionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Sanitization.
           *
           * @return The acecssor that provides access to the complex content that is identified by Sanitization.
           */
         virtual const uci::type::SecuritySanitizationType& getSanitization() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Sanitization.
           *
           * @return The acecssor that provides access to the complex content that is identified by Sanitization.
           */
         virtual uci::type::SecuritySanitizationType& getSanitization()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Sanitization to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Sanitization
           */
         virtual void setSanitization(const uci::type::SecuritySanitizationType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SecurityEventType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SecurityEventType to copy from
           */
         SecurityEventType(const SecurityEventType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SecurityEventType to the contents of the SecurityEventType on the
           * right hand side (rhs) of the assignment operator.SecurityEventType [only available to derived classes]
           *
           * @param rhs The SecurityEventType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::SecurityEventType
           * @return A constant reference to this SecurityEventType.
           */
         const SecurityEventType& operator=(const SecurityEventType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SecurityEventType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SecurityEventType_h

