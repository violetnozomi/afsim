// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__AO_SettingsCommandMDT_h
#define Uci__Type__AO_SettingsCommandMDT_h 1

#if !defined(Uci__Type__CapabilitySettingsCommandBaseType_h)
# include "uci/type/CapabilitySettingsCommandBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AO_ComponentSettingsType_h)
# include "uci/type/AO_ComponentSettingsType.h"
#endif

#if !defined(Uci__Type__NITF_PackingPlanType_h)
# include "uci/type/NITF_PackingPlanType.h"
#endif

#if !defined(Uci__Type__JPEG_SettingsType_h)
# include "uci/type/JPEG_SettingsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AO_SettingsCommandMDT sequence accessor class */
      class AO_SettingsCommandMDT : public virtual uci::type::CapabilitySettingsCommandBaseType {
      public:

         /** The destructor */
         virtual ~AO_SettingsCommandMDT()
         { }

         /** Returns this accessor's type constant, i.e. AO_SettingsCommandMDT
           *
           * @return This accessor's type constant, i.e. AO_SettingsCommandMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aO_SettingsCommandMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AO_SettingsCommandMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Describes the AO Component settings. This is used to change the set/default settings for the component prior to an
           * AO_Command and/or AO_Activity. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::AO_ComponentSettingsType, uci::type::accessorType::aO_ComponentSettingsType> AO_ComponentSettings;

         /** Returns the bounded list that is identified by the AO_ComponentSettings.
           *
           * @return The bounded list identified by AO_ComponentSettings.
           */
         virtual const uci::type::AO_SettingsCommandMDT::AO_ComponentSettings& getAO_ComponentSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AO_ComponentSettings.
           *
           * @return The bounded list identified by AO_ComponentSettings.
           */
         virtual uci::type::AO_SettingsCommandMDT::AO_ComponentSettings& getAO_ComponentSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AO_ComponentSettings.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAO_ComponentSettings(const uci::type::AO_SettingsCommandMDT::AO_ComponentSettings& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultPackingPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultPackingPlan.
           */
         virtual const uci::type::NITF_PackingPlanType& getDefaultPackingPlan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultPackingPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultPackingPlan.
           */
         virtual uci::type::NITF_PackingPlanType& getDefaultPackingPlan()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DefaultPackingPlan to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DefaultPackingPlan
           */
         virtual void setDefaultPackingPlan(const uci::type::NITF_PackingPlanType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DefaultPackingPlan exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DefaultPackingPlan is emabled or not
           */
         virtual bool hasDefaultPackingPlan() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DefaultPackingPlan
           *
           * @param type = uci::type::accessorType::nITF_PackingPlanType This Accessor's accessor type
           */
         virtual void enableDefaultPackingPlan(uci::base::accessorType::AccessorType type = uci::type::accessorType::nITF_PackingPlanType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DefaultPackingPlan */
         virtual void clearDefaultPackingPlan()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultJPEG_Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultJPEG_Settings.
           */
         virtual const uci::type::JPEG_SettingsType& getDefaultJPEG_Settings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultJPEG_Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultJPEG_Settings.
           */
         virtual uci::type::JPEG_SettingsType& getDefaultJPEG_Settings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DefaultJPEG_Settings to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DefaultJPEG_Settings
           */
         virtual void setDefaultJPEG_Settings(const uci::type::JPEG_SettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DefaultJPEG_Settings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DefaultJPEG_Settings is emabled or not
           */
         virtual bool hasDefaultJPEG_Settings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DefaultJPEG_Settings
           *
           * @param type = uci::type::accessorType::jPEG_SettingsType This Accessor's accessor type
           */
         virtual void enableDefaultJPEG_Settings(uci::base::accessorType::AccessorType type = uci::type::accessorType::jPEG_SettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DefaultJPEG_Settings */
         virtual void clearDefaultJPEG_Settings()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AO_SettingsCommandMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AO_SettingsCommandMDT to copy from
           */
         AO_SettingsCommandMDT(const AO_SettingsCommandMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AO_SettingsCommandMDT to the contents of the AO_SettingsCommandMDT
           * on the right hand side (rhs) of the assignment operator.AO_SettingsCommandMDT [only available to derived classes]
           *
           * @param rhs The AO_SettingsCommandMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::AO_SettingsCommandMDT
           * @return A constant reference to this AO_SettingsCommandMDT.
           */
         const AO_SettingsCommandMDT& operator=(const AO_SettingsCommandMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AO_SettingsCommandMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AO_SettingsCommandMDT_h

