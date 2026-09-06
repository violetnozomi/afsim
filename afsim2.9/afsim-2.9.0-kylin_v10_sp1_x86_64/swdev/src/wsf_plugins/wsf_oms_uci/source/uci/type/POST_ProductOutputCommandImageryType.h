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
#ifndef Uci__Type__POST_ProductOutputCommandImageryType_h
#define Uci__Type__POST_ProductOutputCommandImageryType_h 1

#if !defined(Uci__Type__ProductOutputCommandBasicType_h)
# include "uci/type/ProductOutputCommandBasicType.h"
#endif

#if !defined(Uci__Type__PO_ProductGeneratorOutputID_Type_h)
# include "uci/type/PO_ProductGeneratorOutputID_Type.h"
#endif

#if !defined(Uci__Type__JPEG_SettingsType_h)
# include "uci/type/JPEG_SettingsType.h"
#endif

#if !defined(Uci__Type__NITF_PackingPlanType_h)
# include "uci/type/NITF_PackingPlanType.h"
#endif

#if !defined(Uci__Base__FloatAccessor_h)
# include "uci/base/FloatAccessor.h"
#endif

#if !defined(Uci__Type__MISP_PackingPlanType_h)
# include "uci/type/MISP_PackingPlanType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__ProductResolutionType_h)
# include "uci/type/ProductResolutionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_ProductOutputCommandImageryType sequence accessor class */
      class POST_ProductOutputCommandImageryType : public virtual uci::type::ProductOutputCommandBasicType {
      public:

         /** The destructor */
         virtual ~POST_ProductOutputCommandImageryType()
         { }

         /** Returns this accessor's type constant, i.e. POST_ProductOutputCommandImageryType
           *
           * @return This accessor's type constant, i.e. POST_ProductOutputCommandImageryType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_ProductOutputCommandImageryType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_ProductOutputCommandImageryType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductOutputID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductOutputID.
           */
         virtual const uci::type::PO_ProductGeneratorOutputID_Type& getProductOutputID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductOutputID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductOutputID.
           */
         virtual uci::type::PO_ProductGeneratorOutputID_Type& getProductOutputID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductOutputID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductOutputID
           */
         virtual void setProductOutputID(const uci::type::PO_ProductGeneratorOutputID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the JPEG_Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by JPEG_Settings.
           */
         virtual const uci::type::JPEG_SettingsType& getJPEG_Settings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the JPEG_Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by JPEG_Settings.
           */
         virtual uci::type::JPEG_SettingsType& getJPEG_Settings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the JPEG_Settings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by JPEG_Settings
           */
         virtual void setJPEG_Settings(const uci::type::JPEG_SettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by JPEG_Settings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by JPEG_Settings is emabled or not
           */
         virtual bool hasJPEG_Settings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by JPEG_Settings
           *
           * @param type = uci::type::accessorType::jPEG_SettingsType This Accessor's accessor type
           */
         virtual void enableJPEG_Settings(uci::base::accessorType::AccessorType type = uci::type::accessorType::jPEG_SettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by JPEG_Settings */
         virtual void clearJPEG_Settings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NITF_PackingPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by NITF_PackingPlan.
           */
         virtual const uci::type::NITF_PackingPlanType& getNITF_PackingPlan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NITF_PackingPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by NITF_PackingPlan.
           */
         virtual uci::type::NITF_PackingPlanType& getNITF_PackingPlan()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NITF_PackingPlan to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NITF_PackingPlan
           */
         virtual void setNITF_PackingPlan(const uci::type::NITF_PackingPlanType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NITF_PackingPlan exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NITF_PackingPlan is emabled or not
           */
         virtual bool hasNITF_PackingPlan() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NITF_PackingPlan
           *
           * @param type = uci::type::accessorType::nITF_PackingPlanType This Accessor's accessor type
           */
         virtual void enableNITF_PackingPlan(uci::base::accessorType::AccessorType type = uci::type::accessorType::nITF_PackingPlanType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NITF_PackingPlan */
         virtual void clearNITF_PackingPlan()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CompressionRatio.
           *
           * @return The value of the simple primitive data type identified by CompressionRatio.
           */
         virtual xs::Float getCompressionRatio() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CompressionRatio.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCompressionRatio(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CompressionRatio exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CompressionRatio is emabled or not
           */
         virtual bool hasCompressionRatio() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CompressionRatio
           *
           * @param type = uci::base::accessorType::floatAccessor This Accessor's accessor type
           */
         virtual void enableCompressionRatio(uci::base::accessorType::AccessorType type = uci::base::accessorType::floatAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CompressionRatio */
         virtual void clearCompressionRatio()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MISP_Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by MISP_Settings.
           */
         virtual const uci::type::MISP_PackingPlanType& getMISP_Settings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MISP_Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by MISP_Settings.
           */
         virtual uci::type::MISP_PackingPlanType& getMISP_Settings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MISP_Settings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MISP_Settings
           */
         virtual void setMISP_Settings(const uci::type::MISP_PackingPlanType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MISP_Settings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MISP_Settings is emabled or not
           */
         virtual bool hasMISP_Settings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MISP_Settings
           *
           * @param type = uci::type::accessorType::mISP_PackingPlanType This Accessor's accessor type
           */
         virtual void enableMISP_Settings(uci::base::accessorType::AccessorType type = uci::type::accessorType::mISP_PackingPlanType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MISP_Settings */
         virtual void clearMISP_Settings()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ProductRate.
           *
           * @return The value of the simple primitive data type identified by ProductRate.
           */
         virtual xs::UnsignedInt getProductRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ProductRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setProductRate(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProductRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProductRate is emabled or not
           */
         virtual bool hasProductRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProductRate
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableProductRate(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProductRate */
         virtual void clearProductRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Interlaced.
           *
           * @return The value of the simple primitive data type identified by Interlaced.
           */
         virtual xs::Boolean getInterlaced() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Interlaced.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInterlaced(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Interlaced exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Interlaced is emabled or not
           */
         virtual bool hasInterlaced() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Interlaced
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableInterlaced(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Interlaced */
         virtual void clearInterlaced()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ProductSize.
           *
           * @return The value of the simple primitive data type identified by ProductSize.
           */
         virtual xs::UnsignedInt getProductSize() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ProductSize.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setProductSize(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProductSize exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProductSize is emabled or not
           */
         virtual bool hasProductSize() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProductSize
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableProductSize(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProductSize */
         virtual void clearProductSize()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductResolution.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductResolution.
           */
         virtual const uci::type::ProductResolutionType& getProductResolution() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductResolution.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductResolution.
           */
         virtual uci::type::ProductResolutionType& getProductResolution()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductResolution to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductResolution
           */
         virtual void setProductResolution(const uci::type::ProductResolutionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProductResolution exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProductResolution is emabled or not
           */
         virtual bool hasProductResolution() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProductResolution
           *
           * @param type = uci::type::accessorType::productResolutionType This Accessor's accessor type
           */
         virtual void enableProductResolution(uci::base::accessorType::AccessorType type = uci::type::accessorType::productResolutionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProductResolution */
         virtual void clearProductResolution()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_ProductOutputCommandImageryType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_ProductOutputCommandImageryType to copy from
           */
         POST_ProductOutputCommandImageryType(const POST_ProductOutputCommandImageryType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_ProductOutputCommandImageryType to the contents of the
           * POST_ProductOutputCommandImageryType on the right hand side (rhs) of the assignment
           * operator.POST_ProductOutputCommandImageryType [only available to derived classes]
           *
           * @param rhs The POST_ProductOutputCommandImageryType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::POST_ProductOutputCommandImageryType
           * @return A constant reference to this POST_ProductOutputCommandImageryType.
           */
         const POST_ProductOutputCommandImageryType& operator=(const POST_ProductOutputCommandImageryType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_ProductOutputCommandImageryType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_ProductOutputCommandImageryType_h

