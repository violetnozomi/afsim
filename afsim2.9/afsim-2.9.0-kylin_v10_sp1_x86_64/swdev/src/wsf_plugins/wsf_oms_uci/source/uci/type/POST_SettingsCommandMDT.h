// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__POST_SettingsCommandMDT_h
#define Uci__Type__POST_SettingsCommandMDT_h 1

#if !defined(Uci__Type__CapabilitySettingsCommandBaseType_h)
# include "uci/type/CapabilitySettingsCommandBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__POST_SubCapabilitySettingsType_h)
# include "uci/type/POST_SubCapabilitySettingsType.h"
#endif

#if !defined(Uci__Type__POST_TrackSettingsType_h)
# include "uci/type/POST_TrackSettingsType.h"
#endif

#if !defined(Uci__Type__POST_ComponentSettingsType_h)
# include "uci/type/POST_ComponentSettingsType.h"
#endif

#if !defined(Uci__Type__PO_CollectionConstraintsSettingsType_h)
# include "uci/type/PO_CollectionConstraintsSettingsType.h"
#endif

#if !defined(Uci__Type__FOR_Type_h)
# include "uci/type/FOR_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_SettingsCommandMDT sequence accessor class */
      class POST_SettingsCommandMDT : public virtual uci::type::CapabilitySettingsCommandBaseType {
      public:

         /** The destructor */
         virtual ~POST_SettingsCommandMDT()
         { }

         /** Returns this accessor's type constant, i.e. POST_SettingsCommandMDT
           *
           * @return This accessor's type constant, i.e. POST_SettingsCommandMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_SettingsCommandMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_SettingsCommandMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the subcapability settings to apply to this subsystem. All capabilities that are associated with the same
           * SubcapabilityID will be impacted the same in accordance to this settings command message. Any POST_Commands received
           * after this message has been processed will use these new settings unless directly overridden by settings specified in
           * the POST_Command. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_SubCapabilitySettingsType, uci::type::accessorType::pOST_SubCapabilitySettingsType> SubCapability;

         /** Indicates the settings that can be applied for track. All capabilities that are associated with the same TrackOptions
           * ID in TrackSettings will be impacted the same in accordance to this settings command message. Any POST_Commands
           * received after this message has been processed will use these new settings unless directly overridden by settings
           * specified in the POST_Command. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_TrackSettingsType, uci::type::accessorType::pOST_TrackSettingsType> TrackSettings;

         /** The component settings specified in this field are used to set the default component settings used by the sensor. All
           * capabilities that are associated with the same ComponentID in ComponentSettings will be impacted the same in
           * accordance to this settings command message. Any POST_Commands received after this message has been processed will
           * use these new settings unless directly overridden by settings specified in the POST_Command. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_ComponentSettingsType, uci::type::accessorType::pOST_ComponentSettingsType> ComponentSettings;

         /** Returns the bounded list that is identified by the SubCapability.
           *
           * @return The bounded list identified by SubCapability.
           */
         virtual const uci::type::POST_SettingsCommandMDT::SubCapability& getSubCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapability.
           *
           * @return The bounded list identified by SubCapability.
           */
         virtual uci::type::POST_SettingsCommandMDT::SubCapability& getSubCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubCapability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubCapability(const uci::type::POST_SettingsCommandMDT::SubCapability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TrackSettings.
           *
           * @return The bounded list identified by TrackSettings.
           */
         virtual const uci::type::POST_SettingsCommandMDT::TrackSettings& getTrackSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TrackSettings.
           *
           * @return The bounded list identified by TrackSettings.
           */
         virtual uci::type::POST_SettingsCommandMDT::TrackSettings& getTrackSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TrackSettings.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTrackSettings(const uci::type::POST_SettingsCommandMDT::TrackSettings& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentSettings.
           *
           * @return The bounded list identified by ComponentSettings.
           */
         virtual const uci::type::POST_SettingsCommandMDT::ComponentSettings& getComponentSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentSettings.
           *
           * @return The bounded list identified by ComponentSettings.
           */
         virtual uci::type::POST_SettingsCommandMDT::ComponentSettings& getComponentSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ComponentSettings.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setComponentSettings(const uci::type::POST_SettingsCommandMDT::ComponentSettings& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * CollectionConstraintsSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraintsSettings.
           */
         virtual const uci::type::PO_CollectionConstraintsSettingsType& getCollectionConstraintsSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * CollectionConstraintsSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraintsSettings.
           */
         virtual uci::type::PO_CollectionConstraintsSettingsType& getCollectionConstraintsSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionConstraintsSettings to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionConstraintsSettings
           */
         virtual void setCollectionConstraintsSettings(const uci::type::PO_CollectionConstraintsSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionConstraintsSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionConstraintsSettings is emabled or not
           */
         virtual bool hasCollectionConstraintsSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionConstraintsSettings
           *
           * @param type = uci::type::accessorType::pO_CollectionConstraintsSettingsType This Accessor's accessor type
           */
         virtual void enableCollectionConstraintsSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_CollectionConstraintsSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CollectionConstraintsSettings */
         virtual void clearCollectionConstraintsSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FOR_Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by FOR_Settings.
           */
         virtual const uci::type::FOR_Type& getFOR_Settings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FOR_Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by FOR_Settings.
           */
         virtual uci::type::FOR_Type& getFOR_Settings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FOR_Settings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FOR_Settings
           */
         virtual void setFOR_Settings(const uci::type::FOR_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FOR_Settings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FOR_Settings is emabled or not
           */
         virtual bool hasFOR_Settings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FOR_Settings
           *
           * @param type = uci::type::accessorType::fOR_Type This Accessor's accessor type
           */
         virtual void enableFOR_Settings(uci::base::accessorType::AccessorType type = uci::type::accessorType::fOR_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FOR_Settings */
         virtual void clearFOR_Settings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ThermalBlankingFOR_Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ThermalBlankingFOR_Settings.
           */
         virtual const uci::type::FOR_Type& getThermalBlankingFOR_Settings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ThermalBlankingFOR_Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ThermalBlankingFOR_Settings.
           */
         virtual uci::type::FOR_Type& getThermalBlankingFOR_Settings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ThermalBlankingFOR_Settings to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ThermalBlankingFOR_Settings
           */
         virtual void setThermalBlankingFOR_Settings(const uci::type::FOR_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ThermalBlankingFOR_Settings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ThermalBlankingFOR_Settings is emabled or not
           */
         virtual bool hasThermalBlankingFOR_Settings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ThermalBlankingFOR_Settings
           *
           * @param type = uci::type::accessorType::fOR_Type This Accessor's accessor type
           */
         virtual void enableThermalBlankingFOR_Settings(uci::base::accessorType::AccessorType type = uci::type::accessorType::fOR_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ThermalBlankingFOR_Settings */
         virtual void clearThermalBlankingFOR_Settings()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_SettingsCommandMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_SettingsCommandMDT to copy from
           */
         POST_SettingsCommandMDT(const POST_SettingsCommandMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_SettingsCommandMDT to the contents of the
           * POST_SettingsCommandMDT on the right hand side (rhs) of the assignment operator.POST_SettingsCommandMDT [only
           * available to derived classes]
           *
           * @param rhs The POST_SettingsCommandMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::POST_SettingsCommandMDT
           * @return A constant reference to this POST_SettingsCommandMDT.
           */
         const POST_SettingsCommandMDT& operator=(const POST_SettingsCommandMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_SettingsCommandMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_SettingsCommandMDT_h

