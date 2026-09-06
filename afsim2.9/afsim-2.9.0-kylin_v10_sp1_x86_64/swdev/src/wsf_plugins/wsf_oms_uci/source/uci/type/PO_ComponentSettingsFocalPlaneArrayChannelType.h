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
#ifndef Uci__Type__PO_ComponentSettingsFocalPlaneArrayChannelType_h
#define Uci__Type__PO_ComponentSettingsFocalPlaneArrayChannelType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_FPA_ChannelID_Type_h)
# include "uci/type/PO_FPA_ChannelID_Type.h"
#endif

#if !defined(Uci__Type__ComponentControlsA_Type_h)
# include "uci/type/ComponentControlsA_Type.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsFocalPlaneArrayOpticalBandType_h)
# include "uci/type/PO_ComponentSettingsFocalPlaneArrayOpticalBandType.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsFocalPlaneArrayPixelAggregationType_h)
# include "uci/type/PO_ComponentSettingsFocalPlaneArrayPixelAggregationType.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsFocalPlaneArrayTimeDelayIntegrationType_h)
# include "uci/type/PO_ComponentSettingsFocalPlaneArrayTimeDelayIntegrationType.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsFocalPlaneArrayPixelScalingType_h)
# include "uci/type/PO_ComponentSettingsFocalPlaneArrayPixelScalingType.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsFocalPlaneArrayPixelPolarityType_h)
# include "uci/type/PO_ComponentSettingsFocalPlaneArrayPixelPolarityType.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsFocalPlaneArrayCollectionType_h)
# include "uci/type/PO_ComponentSettingsFocalPlaneArrayCollectionType.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsFocalPlaneArrayNonUniformityCorrectionType_h)
# include "uci/type/PO_ComponentSettingsFocalPlaneArrayNonUniformityCorrectionType.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsFocalPlaneArrayScanDirectionType_h)
# include "uci/type/PO_ComponentSettingsFocalPlaneArrayScanDirectionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ComponentSettingsFocalPlaneArrayChannelType sequence accessor class */
      class PO_ComponentSettingsFocalPlaneArrayChannelType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ComponentSettingsFocalPlaneArrayChannelType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ComponentSettingsFocalPlaneArrayChannelType
           *
           * @return This accessor's type constant, i.e. PO_ComponentSettingsFocalPlaneArrayChannelType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayChannelType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ComponentSettingsFocalPlaneArrayChannelType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChannelID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChannelID.
           */
         virtual const uci::type::PO_FPA_ChannelID_Type& getChannelID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChannelID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChannelID.
           */
         virtual uci::type::PO_FPA_ChannelID_Type& getChannelID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChannelID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChannelID
           */
         virtual void setChannelID(const uci::type::PO_FPA_ChannelID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChannelControls.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChannelControls.
           */
         virtual const uci::type::ComponentControlsA_Type& getChannelControls() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChannelControls.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChannelControls.
           */
         virtual uci::type::ComponentControlsA_Type& getChannelControls()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChannelControls to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChannelControls
           */
         virtual void setChannelControls(const uci::type::ComponentControlsA_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChannelControls exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChannelControls is emabled or not
           */
         virtual bool hasChannelControls() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChannelControls
           *
           * @param type = uci::type::accessorType::componentControlsA_Type This Accessor's accessor type
           */
         virtual void enableChannelControls(uci::base::accessorType::AccessorType type = uci::type::accessorType::componentControlsA_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChannelControls */
         virtual void clearChannelControls()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpticalBand.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpticalBand.
           */
         virtual const uci::type::PO_ComponentSettingsFocalPlaneArrayOpticalBandType& getOpticalBand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpticalBand.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpticalBand.
           */
         virtual uci::type::PO_ComponentSettingsFocalPlaneArrayOpticalBandType& getOpticalBand()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OpticalBand to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OpticalBand
           */
         virtual void setOpticalBand(const uci::type::PO_ComponentSettingsFocalPlaneArrayOpticalBandType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OpticalBand exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OpticalBand is emabled or not
           */
         virtual bool hasOpticalBand() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OpticalBand
           *
           * @param type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayOpticalBandType This Accessor's accessor
           *      type
           */
         virtual void enableOpticalBand(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayOpticalBandType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OpticalBand */
         virtual void clearOpticalBand()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelAggregation.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelAggregation.
           */
         virtual const uci::type::PO_ComponentSettingsFocalPlaneArrayPixelAggregationType& getPixelAggregation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelAggregation.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelAggregation.
           */
         virtual uci::type::PO_ComponentSettingsFocalPlaneArrayPixelAggregationType& getPixelAggregation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PixelAggregation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PixelAggregation
           */
         virtual void setPixelAggregation(const uci::type::PO_ComponentSettingsFocalPlaneArrayPixelAggregationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PixelAggregation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PixelAggregation is emabled or not
           */
         virtual bool hasPixelAggregation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PixelAggregation
           *
           * @param type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayPixelAggregationType This Accessor's
           *      accessor type
           */
         virtual void enablePixelAggregation(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayPixelAggregationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PixelAggregation */
         virtual void clearPixelAggregation()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimeDelayIntegration.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimeDelayIntegration.
           */
         virtual const uci::type::PO_ComponentSettingsFocalPlaneArrayTimeDelayIntegrationType& getTimeDelayIntegration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimeDelayIntegration.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimeDelayIntegration.
           */
         virtual uci::type::PO_ComponentSettingsFocalPlaneArrayTimeDelayIntegrationType& getTimeDelayIntegration()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TimeDelayIntegration to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TimeDelayIntegration
           */
         virtual void setTimeDelayIntegration(const uci::type::PO_ComponentSettingsFocalPlaneArrayTimeDelayIntegrationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimeDelayIntegration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimeDelayIntegration is emabled or not
           */
         virtual bool hasTimeDelayIntegration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimeDelayIntegration
           *
           * @param type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayTimeDelayIntegrationType This Accessor's
           *      accessor type
           */
         virtual void enableTimeDelayIntegration(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayTimeDelayIntegrationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimeDelayIntegration */
         virtual void clearTimeDelayIntegration()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelScaling.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelScaling.
           */
         virtual const uci::type::PO_ComponentSettingsFocalPlaneArrayPixelScalingType& getPixelScaling() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelScaling.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelScaling.
           */
         virtual uci::type::PO_ComponentSettingsFocalPlaneArrayPixelScalingType& getPixelScaling()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PixelScaling to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PixelScaling
           */
         virtual void setPixelScaling(const uci::type::PO_ComponentSettingsFocalPlaneArrayPixelScalingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PixelScaling exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PixelScaling is emabled or not
           */
         virtual bool hasPixelScaling() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PixelScaling
           *
           * @param type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayPixelScalingType This Accessor's accessor
           *      type
           */
         virtual void enablePixelScaling(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayPixelScalingType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PixelScaling */
         virtual void clearPixelScaling()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelPolarity.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelPolarity.
           */
         virtual const uci::type::PO_ComponentSettingsFocalPlaneArrayPixelPolarityType& getPixelPolarity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelPolarity.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelPolarity.
           */
         virtual uci::type::PO_ComponentSettingsFocalPlaneArrayPixelPolarityType& getPixelPolarity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PixelPolarity to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PixelPolarity
           */
         virtual void setPixelPolarity(const uci::type::PO_ComponentSettingsFocalPlaneArrayPixelPolarityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PixelPolarity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PixelPolarity is emabled or not
           */
         virtual bool hasPixelPolarity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PixelPolarity
           *
           * @param type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayPixelPolarityType This Accessor's accessor
           *      type
           */
         virtual void enablePixelPolarity(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayPixelPolarityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PixelPolarity */
         virtual void clearPixelPolarity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionTime.
           */
         virtual const uci::type::PO_ComponentSettingsFocalPlaneArrayCollectionType& getCollectionTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionTime.
           */
         virtual uci::type::PO_ComponentSettingsFocalPlaneArrayCollectionType& getCollectionTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionTime to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionTime
           */
         virtual void setCollectionTime(const uci::type::PO_ComponentSettingsFocalPlaneArrayCollectionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionTime is emabled or not
           */
         virtual bool hasCollectionTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionTime
           *
           * @param type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayCollectionType This Accessor's accessor
           *      type
           */
         virtual void enableCollectionTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayCollectionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CollectionTime */
         virtual void clearCollectionTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NonUniformityCorrection.
           *
           * @return The acecssor that provides access to the complex content that is identified by NonUniformityCorrection.
           */
         virtual const uci::type::PO_ComponentSettingsFocalPlaneArrayNonUniformityCorrectionType& getNonUniformityCorrection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NonUniformityCorrection.
           *
           * @return The acecssor that provides access to the complex content that is identified by NonUniformityCorrection.
           */
         virtual uci::type::PO_ComponentSettingsFocalPlaneArrayNonUniformityCorrectionType& getNonUniformityCorrection()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NonUniformityCorrection to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NonUniformityCorrection
           */
         virtual void setNonUniformityCorrection(const uci::type::PO_ComponentSettingsFocalPlaneArrayNonUniformityCorrectionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NonUniformityCorrection exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NonUniformityCorrection is emabled or not
           */
         virtual bool hasNonUniformityCorrection() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NonUniformityCorrection
           *
           * @param type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayNonUniformityCorrectionType This Accessor's
           *      accessor type
           */
         virtual void enableNonUniformityCorrection(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayNonUniformityCorrectionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NonUniformityCorrection */
         virtual void clearNonUniformityCorrection()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ScanDirection.
           *
           * @return The acecssor that provides access to the complex content that is identified by ScanDirection.
           */
         virtual const uci::type::PO_ComponentSettingsFocalPlaneArrayScanDirectionType& getScanDirection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ScanDirection.
           *
           * @return The acecssor that provides access to the complex content that is identified by ScanDirection.
           */
         virtual uci::type::PO_ComponentSettingsFocalPlaneArrayScanDirectionType& getScanDirection()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ScanDirection to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ScanDirection
           */
         virtual void setScanDirection(const uci::type::PO_ComponentSettingsFocalPlaneArrayScanDirectionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ScanDirection exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ScanDirection is emabled or not
           */
         virtual bool hasScanDirection() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ScanDirection
           *
           * @param type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayScanDirectionType This Accessor's accessor
           *      type
           */
         virtual void enableScanDirection(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayScanDirectionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ScanDirection */
         virtual void clearScanDirection()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ComponentSettingsFocalPlaneArrayChannelType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ComponentSettingsFocalPlaneArrayChannelType to copy from
           */
         PO_ComponentSettingsFocalPlaneArrayChannelType(const PO_ComponentSettingsFocalPlaneArrayChannelType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ComponentSettingsFocalPlaneArrayChannelType to the contents of
           * the PO_ComponentSettingsFocalPlaneArrayChannelType on the right hand side (rhs) of the assignment
           * operator.PO_ComponentSettingsFocalPlaneArrayChannelType [only available to derived classes]
           *
           * @param rhs The PO_ComponentSettingsFocalPlaneArrayChannelType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::PO_ComponentSettingsFocalPlaneArrayChannelType
           * @return A constant reference to this PO_ComponentSettingsFocalPlaneArrayChannelType.
           */
         const PO_ComponentSettingsFocalPlaneArrayChannelType& operator=(const PO_ComponentSettingsFocalPlaneArrayChannelType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ComponentSettingsFocalPlaneArrayChannelType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ComponentSettingsFocalPlaneArrayChannelType_h

