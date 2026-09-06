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
#ifndef Uci__Type__POST_ComponentFocalPlaneArrayOperationType_h
#define Uci__Type__POST_ComponentFocalPlaneArrayOperationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_FPA_ChannelOperationEnum_h)
# include "uci/type/PO_FPA_ChannelOperationEnum.h"
#endif

#if !defined(Uci__Type__UnsignedIntegerMinMaxType_h)
# include "uci/type/UnsignedIntegerMinMaxType.h"
#endif

#if !defined(Uci__Type__DoubleMinMaxType_h)
# include "uci/type/DoubleMinMaxType.h"
#endif

#if !defined(Uci__Type__PixelScalingSettingsType_h)
# include "uci/type/PixelScalingSettingsType.h"
#endif

#if !defined(Uci__Type__DurationRangeType_h)
# include "uci/type/DurationRangeType.h"
#endif

#if !defined(Uci__Type__ComponentControllabilityA_Type_h)
# include "uci/type/ComponentControllabilityA_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_ComponentFocalPlaneArrayOperationType sequence accessor class */
      class POST_ComponentFocalPlaneArrayOperationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_ComponentFocalPlaneArrayOperationType()
         { }

         /** Returns this accessor's type constant, i.e. POST_ComponentFocalPlaneArrayOperationType
           *
           * @return This accessor's type constant, i.e. POST_ComponentFocalPlaneArrayOperationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_ComponentFocalPlaneArrayOperationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_ComponentFocalPlaneArrayOperationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationType.
           *
           * @return The value of the enumeration identified by OperationType.
           */
         virtual const uci::type::PO_FPA_ChannelOperationEnum& getOperationType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationType.
           *
           * @return The value of the enumeration identified by OperationType.
           */
         virtual uci::type::PO_FPA_ChannelOperationEnum& getOperationType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationType(const uci::type::PO_FPA_ChannelOperationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationType(uci::type::PO_FPA_ChannelOperationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelAggregationSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelAggregationSettings.
           */
         virtual const uci::type::UnsignedIntegerMinMaxType& getPixelAggregationSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelAggregationSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelAggregationSettings.
           */
         virtual uci::type::UnsignedIntegerMinMaxType& getPixelAggregationSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PixelAggregationSettings to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PixelAggregationSettings
           */
         virtual void setPixelAggregationSettings(const uci::type::UnsignedIntegerMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PixelAggregationSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PixelAggregationSettings is emabled or not
           */
         virtual bool hasPixelAggregationSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PixelAggregationSettings
           *
           * @param type = uci::type::accessorType::unsignedIntegerMinMaxType This Accessor's accessor type
           */
         virtual void enablePixelAggregationSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::unsignedIntegerMinMaxType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PixelAggregationSettings */
         virtual void clearPixelAggregationSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * TimeDelayIntegrationSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimeDelayIntegrationSettings.
           */
         virtual const uci::type::DoubleMinMaxType& getTimeDelayIntegrationSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * TimeDelayIntegrationSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimeDelayIntegrationSettings.
           */
         virtual uci::type::DoubleMinMaxType& getTimeDelayIntegrationSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TimeDelayIntegrationSettings to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TimeDelayIntegrationSettings
           */
         virtual void setTimeDelayIntegrationSettings(const uci::type::DoubleMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimeDelayIntegrationSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimeDelayIntegrationSettings is emabled or not
           */
         virtual bool hasTimeDelayIntegrationSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimeDelayIntegrationSettings
           *
           * @param type = uci::type::accessorType::doubleMinMaxType This Accessor's accessor type
           */
         virtual void enableTimeDelayIntegrationSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::doubleMinMaxType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimeDelayIntegrationSettings */
         virtual void clearTimeDelayIntegrationSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelScalingSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelScalingSettings.
           */
         virtual const uci::type::PixelScalingSettingsType& getPixelScalingSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelScalingSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelScalingSettings.
           */
         virtual uci::type::PixelScalingSettingsType& getPixelScalingSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PixelScalingSettings to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PixelScalingSettings
           */
         virtual void setPixelScalingSettings(const uci::type::PixelScalingSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PixelScalingSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PixelScalingSettings is emabled or not
           */
         virtual bool hasPixelScalingSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PixelScalingSettings
           *
           * @param type = uci::type::accessorType::pixelScalingSettingsType This Accessor's accessor type
           */
         virtual void enablePixelScalingSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::pixelScalingSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PixelScalingSettings */
         virtual void clearPixelScalingSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionTimeSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionTimeSettings.
           */
         virtual const uci::type::DurationRangeType& getCollectionTimeSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionTimeSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionTimeSettings.
           */
         virtual uci::type::DurationRangeType& getCollectionTimeSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionTimeSettings to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionTimeSettings
           */
         virtual void setCollectionTimeSettings(const uci::type::DurationRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionTimeSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionTimeSettings is emabled or not
           */
         virtual bool hasCollectionTimeSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionTimeSettings
           *
           * @param type = uci::type::accessorType::durationRangeType This Accessor's accessor type
           */
         virtual void enableCollectionTimeSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CollectionTimeSettings */
         virtual void clearCollectionTimeSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * NonUniformityCorrectionSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      NonUniformityCorrectionSettings.
           */
         virtual const uci::type::UnsignedIntegerMinMaxType& getNonUniformityCorrectionSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * NonUniformityCorrectionSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      NonUniformityCorrectionSettings.
           */
         virtual uci::type::UnsignedIntegerMinMaxType& getNonUniformityCorrectionSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NonUniformityCorrectionSettings to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NonUniformityCorrectionSettings
           */
         virtual void setNonUniformityCorrectionSettings(const uci::type::UnsignedIntegerMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NonUniformityCorrectionSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NonUniformityCorrectionSettings is emabled or not
           */
         virtual bool hasNonUniformityCorrectionSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NonUniformityCorrectionSettings
           *
           * @param type = uci::type::accessorType::unsignedIntegerMinMaxType This Accessor's accessor type
           */
         virtual void enableNonUniformityCorrectionSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::unsignedIntegerMinMaxType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NonUniformityCorrectionSettings */
         virtual void clearNonUniformityCorrectionSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Controllability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Controllability.
           */
         virtual const uci::type::ComponentControllabilityA_Type& getControllability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Controllability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Controllability.
           */
         virtual uci::type::ComponentControllabilityA_Type& getControllability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Controllability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Controllability
           */
         virtual void setControllability(const uci::type::ComponentControllabilityA_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Controllability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Controllability is emabled or not
           */
         virtual bool hasControllability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Controllability
           *
           * @param type = uci::type::accessorType::componentControllabilityA_Type This Accessor's accessor type
           */
         virtual void enableControllability(uci::base::accessorType::AccessorType type = uci::type::accessorType::componentControllabilityA_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Controllability */
         virtual void clearControllability()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_ComponentFocalPlaneArrayOperationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_ComponentFocalPlaneArrayOperationType to copy from
           */
         POST_ComponentFocalPlaneArrayOperationType(const POST_ComponentFocalPlaneArrayOperationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_ComponentFocalPlaneArrayOperationType to the contents of the
           * POST_ComponentFocalPlaneArrayOperationType on the right hand side (rhs) of the assignment
           * operator.POST_ComponentFocalPlaneArrayOperationType [only available to derived classes]
           *
           * @param rhs The POST_ComponentFocalPlaneArrayOperationType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::POST_ComponentFocalPlaneArrayOperationType
           * @return A constant reference to this POST_ComponentFocalPlaneArrayOperationType.
           */
         const POST_ComponentFocalPlaneArrayOperationType& operator=(const POST_ComponentFocalPlaneArrayOperationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_ComponentFocalPlaneArrayOperationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_ComponentFocalPlaneArrayOperationType_h

