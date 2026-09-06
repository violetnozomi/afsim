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
#ifndef Uci__Type__POST_OffsetTrackSettingsType_h
#define Uci__Type__POST_OffsetTrackSettingsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DoubleMinMaxType_h)
# include "uci/type/DoubleMinMaxType.h"
#endif

#if !defined(Uci__Type__RatesSettingsType_h)
# include "uci/type/RatesSettingsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Allowable track offset setting parameters (Az/El/Roll and Az/El/Roll Rates) from the center of the LOS of the PO
        * sensor.
        */
      class POST_OffsetTrackSettingsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_OffsetTrackSettingsType()
         { }

         /** Returns this accessor's type constant, i.e. POST_OffsetTrackSettingsType
           *
           * @return This accessor's type constant, i.e. POST_OffsetTrackSettingsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_OffsetTrackSettingsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_OffsetTrackSettingsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AzimuthSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by AzimuthSettings.
           */
         virtual const uci::type::DoubleMinMaxType& getAzimuthSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AzimuthSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by AzimuthSettings.
           */
         virtual uci::type::DoubleMinMaxType& getAzimuthSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AzimuthSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AzimuthSettings
           */
         virtual void setAzimuthSettings(const uci::type::DoubleMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElevationSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElevationSettings.
           */
         virtual const uci::type::DoubleMinMaxType& getElevationSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElevationSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElevationSettings.
           */
         virtual uci::type::DoubleMinMaxType& getElevationSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ElevationSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ElevationSettings
           */
         virtual void setElevationSettings(const uci::type::DoubleMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RollSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by RollSettings.
           */
         virtual const uci::type::DoubleMinMaxType& getRollSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RollSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by RollSettings.
           */
         virtual uci::type::DoubleMinMaxType& getRollSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RollSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RollSettings
           */
         virtual void setRollSettings(const uci::type::DoubleMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RollSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RollSettings is emabled or not
           */
         virtual bool hasRollSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RollSettings
           *
           * @param type = uci::type::accessorType::doubleMinMaxType This Accessor's accessor type
           */
         virtual void enableRollSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::doubleMinMaxType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RollSettings */
         virtual void clearRollSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RatesSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by RatesSettings.
           */
         virtual const uci::type::RatesSettingsType& getRatesSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RatesSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by RatesSettings.
           */
         virtual uci::type::RatesSettingsType& getRatesSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RatesSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RatesSettings
           */
         virtual void setRatesSettings(const uci::type::RatesSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RatesSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RatesSettings is emabled or not
           */
         virtual bool hasRatesSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RatesSettings
           *
           * @param type = uci::type::accessorType::ratesSettingsType This Accessor's accessor type
           */
         virtual void enableRatesSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::ratesSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RatesSettings */
         virtual void clearRatesSettings()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_OffsetTrackSettingsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_OffsetTrackSettingsType to copy from
           */
         POST_OffsetTrackSettingsType(const POST_OffsetTrackSettingsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_OffsetTrackSettingsType to the contents of the
           * POST_OffsetTrackSettingsType on the right hand side (rhs) of the assignment operator.POST_OffsetTrackSettingsType
           * [only available to derived classes]
           *
           * @param rhs The POST_OffsetTrackSettingsType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::POST_OffsetTrackSettingsType
           * @return A constant reference to this POST_OffsetTrackSettingsType.
           */
         const POST_OffsetTrackSettingsType& operator=(const POST_OffsetTrackSettingsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_OffsetTrackSettingsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_OffsetTrackSettingsType_h

