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
#ifndef Uci__Type__SubsystemConfigurationType_h
#define Uci__Type__SubsystemConfigurationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AboutType_h)
# include "uci/type/AboutType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SubsystemSupportedSettingType_h)
# include "uci/type/SubsystemSupportedSettingType.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__SupportCapabilityID_Type_h)
# include "uci/type/SupportCapabilityID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Type__InstallationDetailsType_h)
# include "uci/type/InstallationDetailsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SubsystemConfigurationType sequence accessor class */
      class SubsystemConfigurationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SubsystemConfigurationType()
         { }

         /** Returns this accessor's type constant, i.e. SubsystemConfigurationType
           *
           * @return This accessor's type constant, i.e. SubsystemConfigurationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::subsystemConfigurationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SubsystemConfigurationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** If multiple instances are given, each should be of a different setting type as indicated by the child element.
           * [Minimum occurrences: 0] [Maximum occurrences: 8]
           */
         typedef uci::base::BoundedList<uci::type::SubsystemSupportedSettingType, uci::type::accessorType::subsystemSupportedSettingType> SupportedSetting;

         /** Indicates a Capability that is controlled and/or implemented by the Subsystem. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Indicates a SupportCapability that is controlled and/or implemented by the Subsystem. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SupportCapabilityID_Type, uci::type::accessorType::supportCapabilityID_Type> SupportCapabilityID;

         /** Indicates a Service that is implemented by the Subsystem. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ServiceID_Type, uci::type::accessorType::serviceID_Type> ServiceID;

         /** Returns the accessor that provides access to the complex content that is identified by the About.
           *
           * @return The acecssor that provides access to the complex content that is identified by About.
           */
         virtual const uci::type::AboutType& getAbout() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the About.
           *
           * @return The acecssor that provides access to the complex content that is identified by About.
           */
         virtual uci::type::AboutType& getAbout()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the About to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by About
           */
         virtual void setAbout(const uci::type::AboutType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedSetting.
           *
           * @return The bounded list identified by SupportedSetting.
           */
         virtual const uci::type::SubsystemConfigurationType::SupportedSetting& getSupportedSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedSetting.
           *
           * @return The bounded list identified by SupportedSetting.
           */
         virtual uci::type::SubsystemConfigurationType::SupportedSetting& getSupportedSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportedSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportedSetting(const uci::type::SubsystemConfigurationType::SupportedSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::SubsystemConfigurationType::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::SubsystemConfigurationType::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::SubsystemConfigurationType::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportCapabilityID.
           *
           * @return The bounded list identified by SupportCapabilityID.
           */
         virtual const uci::type::SubsystemConfigurationType::SupportCapabilityID& getSupportCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportCapabilityID.
           *
           * @return The bounded list identified by SupportCapabilityID.
           */
         virtual uci::type::SubsystemConfigurationType::SupportCapabilityID& getSupportCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportCapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportCapabilityID(const uci::type::SubsystemConfigurationType::SupportCapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ServiceID.
           *
           * @return The bounded list identified by ServiceID.
           */
         virtual const uci::type::SubsystemConfigurationType::ServiceID& getServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ServiceID.
           *
           * @return The bounded list identified by ServiceID.
           */
         virtual uci::type::SubsystemConfigurationType::ServiceID& getServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ServiceID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setServiceID(const uci::type::SubsystemConfigurationType::ServiceID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InstallationDetails.
           *
           * @return The acecssor that provides access to the complex content that is identified by InstallationDetails.
           */
         virtual const uci::type::InstallationDetailsType& getInstallationDetails() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InstallationDetails.
           *
           * @return The acecssor that provides access to the complex content that is identified by InstallationDetails.
           */
         virtual uci::type::InstallationDetailsType& getInstallationDetails()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the InstallationDetails to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by InstallationDetails
           */
         virtual void setInstallationDetails(const uci::type::InstallationDetailsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InstallationDetails exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InstallationDetails is emabled or not
           */
         virtual bool hasInstallationDetails() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InstallationDetails
           *
           * @param type = uci::type::accessorType::installationDetailsType This Accessor's accessor type
           */
         virtual void enableInstallationDetails(uci::base::accessorType::AccessorType type = uci::type::accessorType::installationDetailsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InstallationDetails */
         virtual void clearInstallationDetails()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemConfigurationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemConfigurationType to copy from
           */
         SubsystemConfigurationType(const SubsystemConfigurationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemConfigurationType to the contents of the
           * SubsystemConfigurationType on the right hand side (rhs) of the assignment operator.SubsystemConfigurationType [only
           * available to derived classes]
           *
           * @param rhs The SubsystemConfigurationType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SubsystemConfigurationType
           * @return A constant reference to this SubsystemConfigurationType.
           */
         const SubsystemConfigurationType& operator=(const SubsystemConfigurationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemConfigurationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SubsystemConfigurationType_h

