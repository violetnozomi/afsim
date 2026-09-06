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
#ifndef Uci__Type__RatesSettingsType_h
#define Uci__Type__RatesSettingsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DoubleMinMaxType_h)
# include "uci/type/DoubleMinMaxType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RatesSettingsType sequence accessor class */
      class RatesSettingsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RatesSettingsType()
         { }

         /** Returns this accessor's type constant, i.e. RatesSettingsType
           *
           * @return This accessor's type constant, i.e. RatesSettingsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::ratesSettingsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RatesSettingsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AzimuthRateSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by AzimuthRateSettings.
           */
         virtual const uci::type::DoubleMinMaxType& getAzimuthRateSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AzimuthRateSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by AzimuthRateSettings.
           */
         virtual uci::type::DoubleMinMaxType& getAzimuthRateSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AzimuthRateSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AzimuthRateSettings
           */
         virtual void setAzimuthRateSettings(const uci::type::DoubleMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElevationRateSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElevationRateSettings.
           */
         virtual const uci::type::DoubleMinMaxType& getElevationRateSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElevationRateSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElevationRateSettings.
           */
         virtual uci::type::DoubleMinMaxType& getElevationRateSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ElevationRateSettings to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ElevationRateSettings
           */
         virtual void setElevationRateSettings(const uci::type::DoubleMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RollRateSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by RollRateSettings.
           */
         virtual const uci::type::DoubleMinMaxType& getRollRateSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RollRateSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by RollRateSettings.
           */
         virtual uci::type::DoubleMinMaxType& getRollRateSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RollRateSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RollRateSettings
           */
         virtual void setRollRateSettings(const uci::type::DoubleMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RollRateSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RollRateSettings is emabled or not
           */
         virtual bool hasRollRateSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RollRateSettings
           *
           * @param type = uci::type::accessorType::doubleMinMaxType This Accessor's accessor type
           */
         virtual void enableRollRateSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::doubleMinMaxType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RollRateSettings */
         virtual void clearRollRateSettings()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RatesSettingsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RatesSettingsType to copy from
           */
         RatesSettingsType(const RatesSettingsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RatesSettingsType to the contents of the RatesSettingsType on the
           * right hand side (rhs) of the assignment operator.RatesSettingsType [only available to derived classes]
           *
           * @param rhs The RatesSettingsType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::RatesSettingsType
           * @return A constant reference to this RatesSettingsType.
           */
         const RatesSettingsType& operator=(const RatesSettingsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RatesSettingsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RatesSettingsType_h

