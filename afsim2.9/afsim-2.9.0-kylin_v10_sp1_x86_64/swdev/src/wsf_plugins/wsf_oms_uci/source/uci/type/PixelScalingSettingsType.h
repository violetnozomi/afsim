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
#ifndef Uci__Type__PixelScalingSettingsType_h
#define Uci__Type__PixelScalingSettingsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DoubleMinMaxType_h)
# include "uci/type/DoubleMinMaxType.h"
#endif

#if !defined(Uci__Type__IntegerMinMaxType_h)
# include "uci/type/IntegerMinMaxType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Pixel scaling setting minimum and maximum allowable values for the subsystem. */
      class PixelScalingSettingsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PixelScalingSettingsType()
         { }

         /** Returns this accessor's type constant, i.e. PixelScalingSettingsType
           *
           * @return This accessor's type constant, i.e. PixelScalingSettingsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pixelScalingSettingsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PixelScalingSettingsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GainSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by GainSettings.
           */
         virtual const uci::type::DoubleMinMaxType& getGainSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GainSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by GainSettings.
           */
         virtual uci::type::DoubleMinMaxType& getGainSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the GainSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by GainSettings
           */
         virtual void setGainSettings(const uci::type::DoubleMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OffsetSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by OffsetSettings.
           */
         virtual const uci::type::IntegerMinMaxType& getOffsetSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OffsetSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by OffsetSettings.
           */
         virtual uci::type::IntegerMinMaxType& getOffsetSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OffsetSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OffsetSettings
           */
         virtual void setOffsetSettings(const uci::type::IntegerMinMaxType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PixelScalingSettingsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PixelScalingSettingsType to copy from
           */
         PixelScalingSettingsType(const PixelScalingSettingsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PixelScalingSettingsType to the contents of the
           * PixelScalingSettingsType on the right hand side (rhs) of the assignment operator.PixelScalingSettingsType [only
           * available to derived classes]
           *
           * @param rhs The PixelScalingSettingsType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::PixelScalingSettingsType
           * @return A constant reference to this PixelScalingSettingsType.
           */
         const PixelScalingSettingsType& operator=(const PixelScalingSettingsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PixelScalingSettingsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PixelScalingSettingsType_h

