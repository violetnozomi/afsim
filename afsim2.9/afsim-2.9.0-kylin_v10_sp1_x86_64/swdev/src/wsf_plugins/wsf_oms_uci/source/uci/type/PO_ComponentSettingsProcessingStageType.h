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
#ifndef Uci__Type__PO_ComponentSettingsProcessingStageType_h
#define Uci__Type__PO_ComponentSettingsProcessingStageType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_ProcessingStageID_Type_h)
# include "uci/type/PO_ProcessingStageID_Type.h"
#endif

#if !defined(Uci__Type__ComponentControlsA_Type_h)
# include "uci/type/ComponentControlsA_Type.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsProcessingStageSettingsType_h)
# include "uci/type/PO_ComponentSettingsProcessingStageSettingsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ComponentSettingsProcessingStageType sequence accessor class */
      class PO_ComponentSettingsProcessingStageType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ComponentSettingsProcessingStageType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ComponentSettingsProcessingStageType
           *
           * @return This accessor's type constant, i.e. PO_ComponentSettingsProcessingStageType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ComponentSettingsProcessingStageType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ComponentSettingsProcessingStageType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProcessingStageID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProcessingStageID.
           */
         virtual const uci::type::PO_ProcessingStageID_Type& getProcessingStageID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProcessingStageID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProcessingStageID.
           */
         virtual uci::type::PO_ProcessingStageID_Type& getProcessingStageID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProcessingStageID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProcessingStageID
           */
         virtual void setProcessingStageID(const uci::type::PO_ProcessingStageID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProcessingStageControls.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProcessingStageControls.
           */
         virtual const uci::type::ComponentControlsA_Type& getProcessingStageControls() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProcessingStageControls.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProcessingStageControls.
           */
         virtual uci::type::ComponentControlsA_Type& getProcessingStageControls()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProcessingStageControls to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProcessingStageControls
           */
         virtual void setProcessingStageControls(const uci::type::ComponentControlsA_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProcessingStageControls exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProcessingStageControls is emabled or not
           */
         virtual bool hasProcessingStageControls() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProcessingStageControls
           *
           * @param type = uci::type::accessorType::componentControlsA_Type This Accessor's accessor type
           */
         virtual void enableProcessingStageControls(uci::base::accessorType::AccessorType type = uci::type::accessorType::componentControlsA_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProcessingStageControls */
         virtual void clearProcessingStageControls()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by Settings.
           */
         virtual const uci::type::PO_ComponentSettingsProcessingStageSettingsType& getSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by Settings.
           */
         virtual uci::type::PO_ComponentSettingsProcessingStageSettingsType& getSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Settings to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Settings
           */
         virtual void setSettings(const uci::type::PO_ComponentSettingsProcessingStageSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Settings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Settings is emabled or not
           */
         virtual bool hasSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Settings
           *
           * @param type = uci::type::accessorType::pO_ComponentSettingsProcessingStageSettingsType This Accessor's accessor type
           */
         virtual void enableSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentSettingsProcessingStageSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Settings */
         virtual void clearSettings()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ComponentSettingsProcessingStageType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ComponentSettingsProcessingStageType to copy from
           */
         PO_ComponentSettingsProcessingStageType(const PO_ComponentSettingsProcessingStageType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ComponentSettingsProcessingStageType to the contents of the
           * PO_ComponentSettingsProcessingStageType on the right hand side (rhs) of the assignment
           * operator.PO_ComponentSettingsProcessingStageType [only available to derived classes]
           *
           * @param rhs The PO_ComponentSettingsProcessingStageType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::PO_ComponentSettingsProcessingStageType
           * @return A constant reference to this PO_ComponentSettingsProcessingStageType.
           */
         const PO_ComponentSettingsProcessingStageType& operator=(const PO_ComponentSettingsProcessingStageType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ComponentSettingsProcessingStageType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ComponentSettingsProcessingStageType_h

