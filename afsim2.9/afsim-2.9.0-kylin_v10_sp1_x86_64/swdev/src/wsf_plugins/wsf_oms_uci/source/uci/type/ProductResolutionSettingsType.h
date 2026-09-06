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
#ifndef Uci__Type__ProductResolutionSettingsType_h
#define Uci__Type__ProductResolutionSettingsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PixelCountSettingsType_h)
# include "uci/type/PixelCountSettingsType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ColorDepthSettingsType_h)
# include "uci/type/ColorDepthSettingsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductResolutionSettingsType sequence accessor class */
      class ProductResolutionSettingsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductResolutionSettingsType()
         { }

         /** Returns this accessor's type constant, i.e. ProductResolutionSettingsType
           *
           * @return This accessor's type constant, i.e. ProductResolutionSettingsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productResolutionSettingsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductResolutionSettingsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the maximum and minimum discrete color values in each band, e.g. 8-bit color representation will require a
           * value of 256. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ColorDepthSettingsType, uci::type::accessorType::colorDepthSettingsType> ColorDepthSettings;

         /** Returns the accessor that provides access to the complex content that is identified by the PixelHorizontalSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelHorizontalSettings.
           */
         virtual const uci::type::PixelCountSettingsType& getPixelHorizontalSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelHorizontalSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelHorizontalSettings.
           */
         virtual uci::type::PixelCountSettingsType& getPixelHorizontalSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PixelHorizontalSettings to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PixelHorizontalSettings
           */
         virtual void setPixelHorizontalSettings(const uci::type::PixelCountSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelVerticalSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelVerticalSettings.
           */
         virtual const uci::type::PixelCountSettingsType& getPixelVerticalSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelVerticalSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelVerticalSettings.
           */
         virtual uci::type::PixelCountSettingsType& getPixelVerticalSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PixelVerticalSettings to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PixelVerticalSettings
           */
         virtual void setPixelVerticalSettings(const uci::type::PixelCountSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ColorDepthSettings.
           *
           * @return The bounded list identified by ColorDepthSettings.
           */
         virtual const uci::type::ProductResolutionSettingsType::ColorDepthSettings& getColorDepthSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ColorDepthSettings.
           *
           * @return The bounded list identified by ColorDepthSettings.
           */
         virtual uci::type::ProductResolutionSettingsType::ColorDepthSettings& getColorDepthSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ColorDepthSettings.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setColorDepthSettings(const uci::type::ProductResolutionSettingsType::ColorDepthSettings& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductResolutionSettingsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductResolutionSettingsType to copy from
           */
         ProductResolutionSettingsType(const ProductResolutionSettingsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductResolutionSettingsType to the contents of the
           * ProductResolutionSettingsType on the right hand side (rhs) of the assignment operator.ProductResolutionSettingsType
           * [only available to derived classes]
           *
           * @param rhs The ProductResolutionSettingsType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ProductResolutionSettingsType
           * @return A constant reference to this ProductResolutionSettingsType.
           */
         const ProductResolutionSettingsType& operator=(const ProductResolutionSettingsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductResolutionSettingsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductResolutionSettingsType_h

