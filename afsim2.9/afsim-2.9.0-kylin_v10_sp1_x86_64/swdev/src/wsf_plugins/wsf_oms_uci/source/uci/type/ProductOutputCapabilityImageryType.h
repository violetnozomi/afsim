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
#ifndef Uci__Type__ProductOutputCapabilityImageryType_h
#define Uci__Type__ProductOutputCapabilityImageryType_h 1

#if !defined(Uci__Type__ProductOutputType_h)
# include "uci/type/ProductOutputType.h"
#endif

#if !defined(Uci__Type__PO_ProductGeneratorOutputID_Type_h)
# include "uci/type/PO_ProductGeneratorOutputID_Type.h"
#endif

#if !defined(Uci__Type__CompressionRatioSettingsType_h)
# include "uci/type/CompressionRatioSettingsType.h"
#endif

#if !defined(Uci__Type__ProductRateSettingsType_h)
# include "uci/type/ProductRateSettingsType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__ProductSizeSettingsType_h)
# include "uci/type/ProductSizeSettingsType.h"
#endif

#if !defined(Uci__Type__ProductResolutionSettingsType_h)
# include "uci/type/ProductResolutionSettingsType.h"
#endif

#if !defined(Uci__Type__ComponentControllabilityA_Type_h)
# include "uci/type/ComponentControllabilityA_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductOutputCapabilityImageryType sequence accessor class */
      class ProductOutputCapabilityImageryType : public virtual uci::type::ProductOutputType {
      public:

         /** The destructor */
         virtual ~ProductOutputCapabilityImageryType()
         { }

         /** Returns this accessor's type constant, i.e. ProductOutputCapabilityImageryType
           *
           * @return This accessor's type constant, i.e. ProductOutputCapabilityImageryType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productOutputCapabilityImageryType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductOutputCapabilityImageryType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GeneratorOutputID.
           *
           * @return The acecssor that provides access to the complex content that is identified by GeneratorOutputID.
           */
         virtual const uci::type::PO_ProductGeneratorOutputID_Type& getGeneratorOutputID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GeneratorOutputID.
           *
           * @return The acecssor that provides access to the complex content that is identified by GeneratorOutputID.
           */
         virtual uci::type::PO_ProductGeneratorOutputID_Type& getGeneratorOutputID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the GeneratorOutputID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by GeneratorOutputID
           */
         virtual void setGeneratorOutputID(const uci::type::PO_ProductGeneratorOutputID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CompressionRatioSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by CompressionRatioSettings.
           */
         virtual const uci::type::CompressionRatioSettingsType& getCompressionRatioSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CompressionRatioSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by CompressionRatioSettings.
           */
         virtual uci::type::CompressionRatioSettingsType& getCompressionRatioSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CompressionRatioSettings to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CompressionRatioSettings
           */
         virtual void setCompressionRatioSettings(const uci::type::CompressionRatioSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CompressionRatioSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CompressionRatioSettings is emabled or not
           */
         virtual bool hasCompressionRatioSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CompressionRatioSettings
           *
           * @param type = uci::type::accessorType::compressionRatioSettingsType This Accessor's accessor type
           */
         virtual void enableCompressionRatioSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::compressionRatioSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CompressionRatioSettings */
         virtual void clearCompressionRatioSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductRateSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductRateSettings.
           */
         virtual const uci::type::ProductRateSettingsType& getProductRateSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductRateSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductRateSettings.
           */
         virtual uci::type::ProductRateSettingsType& getProductRateSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductRateSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductRateSettings
           */
         virtual void setProductRateSettings(const uci::type::ProductRateSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProductRateSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProductRateSettings is emabled or not
           */
         virtual bool hasProductRateSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProductRateSettings
           *
           * @param type = uci::type::accessorType::productRateSettingsType This Accessor's accessor type
           */
         virtual void enableProductRateSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::productRateSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProductRateSettings */
         virtual void clearProductRateSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Interlaceability.
           *
           * @return The value of the simple primitive data type identified by Interlaceability.
           */
         virtual xs::Boolean getInterlaceability() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Interlaceability.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInterlaceability(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Interlaceability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Interlaceability is emabled or not
           */
         virtual bool hasInterlaceability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Interlaceability
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableInterlaceability(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Interlaceability */
         virtual void clearInterlaceability()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductSizeSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductSizeSettings.
           */
         virtual const uci::type::ProductSizeSettingsType& getProductSizeSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductSizeSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductSizeSettings.
           */
         virtual uci::type::ProductSizeSettingsType& getProductSizeSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductSizeSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductSizeSettings
           */
         virtual void setProductSizeSettings(const uci::type::ProductSizeSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProductSizeSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProductSizeSettings is emabled or not
           */
         virtual bool hasProductSizeSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProductSizeSettings
           *
           * @param type = uci::type::accessorType::productSizeSettingsType This Accessor's accessor type
           */
         virtual void enableProductSizeSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::productSizeSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProductSizeSettings */
         virtual void clearProductSizeSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductResolutionSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductResolutionSettings.
           */
         virtual const uci::type::ProductResolutionSettingsType& getProductResolutionSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductResolutionSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductResolutionSettings.
           */
         virtual uci::type::ProductResolutionSettingsType& getProductResolutionSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductResolutionSettings to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductResolutionSettings
           */
         virtual void setProductResolutionSettings(const uci::type::ProductResolutionSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProductResolutionSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProductResolutionSettings is emabled or not
           */
         virtual bool hasProductResolutionSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProductResolutionSettings
           *
           * @param type = uci::type::accessorType::productResolutionSettingsType This Accessor's accessor type
           */
         virtual void enableProductResolutionSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::productResolutionSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProductResolutionSettings */
         virtual void clearProductResolutionSettings()
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
         ProductOutputCapabilityImageryType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductOutputCapabilityImageryType to copy from
           */
         ProductOutputCapabilityImageryType(const ProductOutputCapabilityImageryType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductOutputCapabilityImageryType to the contents of the
           * ProductOutputCapabilityImageryType on the right hand side (rhs) of the assignment
           * operator.ProductOutputCapabilityImageryType [only available to derived classes]
           *
           * @param rhs The ProductOutputCapabilityImageryType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ProductOutputCapabilityImageryType
           * @return A constant reference to this ProductOutputCapabilityImageryType.
           */
         const ProductOutputCapabilityImageryType& operator=(const ProductOutputCapabilityImageryType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductOutputCapabilityImageryType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductOutputCapabilityImageryType_h

