// This file was generated  on 12/5/2018 at 1:1:50 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__ProductTypeEnum_h
#define Uci__Type__ProductTypeEnum_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(StdLibMap)
#  include <map>
//  @cond
#  define StdLibMap 1
//  @endcond
#endif

#if !defined(StdLibSstream)
#  include <sstream>
//  @cond
#  define StdLibSstream 1
//  @endcond
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates the product type. */
      class ProductTypeEnum : public virtual uci::base::Accessor {
      public:

         //  The following are the enumerated items that comprise this enumeration
         enum EnumerationItem {
            //  Enumerated item that indicates that the enumeration has not been set
            enumNotSet,
            //  UCI_AMTI_AUXILIARY_DATA's ordinal value */
            UCI_AMTI_AUXILIARY_DATA,

            //  UCI_AMTI_DETECTION's ordinal value */
            UCI_AMTI_DETECTION,

            //  UCI_AMTI_WBRD's ordinal value */
            UCI_AMTI_WBRD,

            //  UCI_AUDIO's ordinal value */
            UCI_AUDIO,

            //  UCI_AUDIO_CLIP's ordinal value */
            UCI_AUDIO_CLIP,

            //  UCI_AUDIO_STREAM's ordinal value */
            UCI_AUDIO_STREAM,

            //  UCI_DEMODULATED_DATA's ordinal value */
            UCI_DEMODULATED_DATA,

            //  UCI_ESM_IQ_DATA's ordinal value */
            UCI_ESM_IQ_DATA,

            //  UCI_GEOPRODUCT_ELEVATION's ordinal value */
            UCI_GEOPRODUCT_ELEVATION,

            //  UCI_GEOPRODUCT_FEATURE's ordinal value */
            UCI_GEOPRODUCT_FEATURE,

            //  UCI_GEOPRODUCT_FEATURE_BORDER's ordinal value */
            UCI_GEOPRODUCT_FEATURE_BORDER,

            //  UCI_GEOPRODUCT_FEATURE_ROADS's ordinal value */
            UCI_GEOPRODUCT_FEATURE_ROADS,

            //  UCI_GEOPRODUCT_FEATURE_SHORELINE's ordinal value */
            UCI_GEOPRODUCT_FEATURE_SHORELINE,

            //  UCI_IQ_DATA's ordinal value */
            UCI_IQ_DATA,

            //  UCI_IQ_DATA_TIME_CORRECTED's ordinal value */
            UCI_IQ_DATA_TIME_CORRECTED,

            //  UCI_ISAR_WBRD's ordinal value */
            UCI_ISAR_WBRD,

            //  UCI_ISAR_AUXILIARY_DATA's ordinal value */
            UCI_ISAR_AUXILIARY_DATA,

            //  UCI_ISAR_MCPHD's ordinal value */
            UCI_ISAR_MCPHD,

            //  UCI_ISAR_WCMD's ordinal value */
            UCI_ISAR_WCMD,

            //  UCI_ISAR_MOVIE's ordinal value */
            UCI_ISAR_MOVIE,

            //  UCI_OPTICAL_IMAGE_VISIBLE's ordinal value */
            UCI_OPTICAL_IMAGE_VISIBLE,

            //  UCI_OPTICAL_IMAGE_INFRARED's ordinal value */
            UCI_OPTICAL_IMAGE_INFRARED,

            //  UCI_OPTICAL_IMAGE's ordinal value */
            UCI_OPTICAL_IMAGE,

            //  UCI_OPTICAL_IMAGE_HYPERSPECTRAL's ordinal value */
            UCI_OPTICAL_IMAGE_HYPERSPECTRAL,

            //  UCI_OPTICAL_VIDEO_CLIP_VISIBLE's ordinal value */
            UCI_OPTICAL_VIDEO_CLIP_VISIBLE,

            //  UCI_OPTICAL_VIDEO_CLIP_INFRARED's ordinal value */
            UCI_OPTICAL_VIDEO_CLIP_INFRARED,

            //  UCI_OPTICAL_VIDEO_CLIP's ordinal value */
            UCI_OPTICAL_VIDEO_CLIP,

            //  UCI_OPTICAL_VIDEO_CLIP_HYPERSPECTRAL's ordinal value */
            UCI_OPTICAL_VIDEO_CLIP_HYPERSPECTRAL,

            //  UCI_OPTICAL_VIDEO_STREAM_VISIBLE's ordinal value */
            UCI_OPTICAL_VIDEO_STREAM_VISIBLE,

            //  UCI_OPTICAL_VIDEO_STREAM_INFRARED's ordinal value */
            UCI_OPTICAL_VIDEO_STREAM_INFRARED,

            //  UCI_OPTICAL_VIDEO_STREAM's ordinal value */
            UCI_OPTICAL_VIDEO_STREAM,

            //  UCI_OPTICAL_VIDEO_STREAM_HYPERSPECTRAL's ordinal value */
            UCI_OPTICAL_VIDEO_STREAM_HYPERSPECTRAL,

            //  UCI_OPTICAL_IMAGE_GOOGLE_MAPPED's ordinal value */
            UCI_OPTICAL_IMAGE_GOOGLE_MAPPED,

            //  UCI_OPTICAL_IMAGE_DTD_MAPPED's ordinal value */
            UCI_OPTICAL_IMAGE_DTD_MAPPED,

            //  UCI_OPTICAL_IMAGE_FOCUS_SWEEP's ordinal value */
            UCI_OPTICAL_IMAGE_FOCUS_SWEEP,

            //  UCI_OPTICAL_REPORT_CALIBRATION's ordinal value */
            UCI_OPTICAL_REPORT_CALIBRATION,

            //  UCI_SAR_WCMD's ordinal value */
            UCI_SAR_WCMD,

            //  UCI_SAR_WCMD_TOC's ordinal value */
            UCI_SAR_WCMD_TOC,

            //  UCI_SAR_WCID's ordinal value */
            UCI_SAR_WCID,

            //  UCI_SAR_WBID's ordinal value */
            UCI_SAR_WBID,

            //  UCI_SAR_WBRD's ordinal value */
            UCI_SAR_WBRD,

            //  UCI_SAR_AUXILIARY_DATA's ordinal value */
            UCI_SAR_AUXILIARY_DATA,

            //  UCI_SAR_MCPHD's ordinal value */
            UCI_SAR_MCPHD,

            //  UCI_SHAPEFILE's ordinal value */
            UCI_SHAPEFILE,

            //  UCI_SMTI_AUXILIARY_DATA's ordinal value */
            UCI_SMTI_AUXILIARY_DATA,

            //  UCI_SMTI_1D_HRR_PROFILE's ordinal value */
            UCI_SMTI_1D_HRR_PROFILE,

            //  UCI_SMTI_2D_HRR_CHIP's ordinal value */
            UCI_SMTI_2D_HRR_CHIP,

            //  UCI_SMTI_SPARSE_HRR_CHIP's ordinal value */
            UCI_SMTI_SPARSE_HRR_CHIP,

            //  UCI_SMTI_OVERSIZED_HRR_CHIP's ordinal value */
            UCI_SMTI_OVERSIZED_HRR_CHIP,

            //  UCI_SMTI_HRR_FULL_RDM's ordinal value */
            UCI_SMTI_HRR_FULL_RDM,

            //  UCI_SMTI_HRR_PARTIAL_RDM's ordinal value */
            UCI_SMTI_HRR_PARTIAL_RDM,

            //  UCI_SMTI_HRR_RANGE_PULSE_DATA's ordinal value */
            UCI_SMTI_HRR_RANGE_PULSE_DATA,

            //  UCI_SMTI_HRR_THRESHOLD_SCATTER's ordinal value */
            UCI_SMTI_HRR_THRESHOLD_SCATTER,

            //  UCI_SMTI_DETECTION's ordinal value */
            UCI_SMTI_DETECTION,

            //  UCI_SMTI_WBRD's ordinal value */
            UCI_SMTI_WBRD,

            //  UCI_WEATHER_STREAM's ordinal value */
            UCI_WEATHER_STREAM,

            //  UCI_WEATHER_RADAR_WBRD's ordinal value */
            UCI_WEATHER_RADAR_WBRD,

            //  UCI_WEATHER_RADAR_AUXILIARY_DATA's ordinal value */
            UCI_WEATHER_RADAR_AUXILIARY_DATA,

            //  UCI_WEATHER_RADAR_IMAGERY_BASE_REFLECTIVITY's ordinal value */
            UCI_WEATHER_RADAR_IMAGERY_BASE_REFLECTIVITY,

            //  UCI_WEATHER_RADAR_IMAGERY_COMP_REFLECTIVITY's ordinal value */
            UCI_WEATHER_RADAR_IMAGERY_COMP_REFLECTIVITY,

            //  UCI_WEATHER_METAR's ordinal value */
            UCI_WEATHER_METAR,

            //  UCI_WEATHER_TAF's ordinal value */
            UCI_WEATHER_TAF,

            //  UCI_WEATHER_PIREP's ordinal value */
            UCI_WEATHER_PIREP,

            //  UCI_WEATHER_AIREP's ordinal value */
            UCI_WEATHER_AIREP,

            //  UCI_WEATHER_SATELLITE_IMAGERY_VISIBLE's ordinal value */
            UCI_WEATHER_SATELLITE_IMAGERY_VISIBLE,

            //  UCI_WEATHER_SATELLITE_IMAGERY_INFRARED's ordinal value */
            UCI_WEATHER_SATELLITE_IMAGERY_INFRARED,

            //  UCI_WEATHER_SATELLITE_IMAGERY_MSI's ordinal value */
            UCI_WEATHER_SATELLITE_IMAGERY_MSI,

            //  UCI_WEATHER_SATELLITE_IMAGERY_MICROWAVE's ordinal value */
            UCI_WEATHER_SATELLITE_IMAGERY_MICROWAVE,

            //  UCI_WEATHER_SATELLITE_IMAGERY_WATERVAPOR's ordinal value */
            UCI_WEATHER_SATELLITE_IMAGERY_WATERVAPOR,

            //  UCI_WEATHER_GRIB's ordinal value */
            UCI_WEATHER_GRIB,

            //  UCI_WEATHER_TABULAR_DATA's ordinal value */
            UCI_WEATHER_TABULAR_DATA,

            //  UCI_WEATHER_PROGNOSTIC_CHARTS's ordinal value */
            UCI_WEATHER_PROGNOSTIC_CHARTS,

            //  UCI_WEATHER_SURFACE_CHARTS's ordinal value */
            UCI_WEATHER_SURFACE_CHARTS,

            //  UCI_WEATHER_FORECAST_WIND_VELOCITY_CHART's ordinal value */
            UCI_WEATHER_FORECAST_WIND_VELOCITY_CHART,

            //  UCI_WEATHER_FORECAST_AIR_TEMPERATURE_CHART's ordinal value */
            UCI_WEATHER_FORECAST_AIR_TEMPERATURE_CHART,

            //  UCI_WEATHER_FORECAST_EAST_WIND_CHART's ordinal value */
            UCI_WEATHER_FORECAST_EAST_WIND_CHART,

            //  UCI_WEATHER_FORECAST_NORTH_WIND_CHART's ordinal value */
            UCI_WEATHER_FORECAST_NORTH_WIND_CHART,

            //  UCI_OTHER's ordinal value */
            UCI_OTHER,

            //  Enumerated item that indicates maximum enumerated item exclusive of this item
            enumMaxExclusive
         };


         /** The destructor */
         virtual ~ProductTypeEnum()
         { }

         /** Returns this accessor's type constant, i.e. productTypeEnum
           *
           * @return This accessor's type constant, i.e. productTypeEnum
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productTypeEnum;
         }


         /** The assignment operator. Sets the contents of this ProductTypeEnum to the contents of the ProductTypeEnum on the
           * right hand side (rhs) of the assignment operator.ProductTypeEnum
           *
           * @param rhs The ProductTypeEnum on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this ProductTypeEnum
           * @return A constant reference to this ProductTypeEnum.
           */
         const ProductTypeEnum& operator=(const ProductTypeEnum& rhs)
         {
            setValue(rhs.getValue());
            return *this;
         }


         /** The assignment operator. Sets the value of this ProductTypeEnum enumeration to the EnumerationItem on the right hand
           * side of the assignment operator.
           *
           * @param rhs The EnumerationItemwhose value is to be assigned to this ProductTypeEnum enumeration
           * @return A reference to this ProductTypeEnum enumeration
           */
         const ProductTypeEnum& operator=(EnumerationItem rhs)
         {
            setValue(rhs);
            return *this;
         }


         /** Sets the value of this ProductTypeEnum enumeration to the value of the specified enumeration item.
           *
           * @param item The EnumerationItemwhose value is to be used to set the value of this ProductTypeEnum enumeration
           */
         virtual void setValue(EnumerationItem item)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of this ProductTypeEnum enumeration.
           *
           * @param testForValidity=true Specifies whether this ProductTypeEnum enumeration should be validated (true) or not
           *      (false) as part of retrieving its value.
           * @return The value of this ProductTypeEnum enumeration.
           */
         virtual EnumerationItem getValue(bool testForValidity=true) const
            throw(uci::base::UCIException) = 0;


         /** Returns the number of items in this ProductTypeEnum enumeration.
           *
           * @return The number of items in this ProductTypeEnum enumeration.
           */
         int getNumberOfItems() const
         {
            return 78;
         }


         /** Returns whether this ProductTypeEnum enumeration is valid or not. A ProductTypeEnum enumeration is valid if its value
           * is one of the enumeration items other than enumNotSet and enumMaxExclusive.
           *
           * @return A flag indicating whether this ProductTypeEnum enumeration is valid (true) or not (false).
           */
         virtual bool isValid() const
         {
            return isValid(getValue(false));
         }


         /** Returns whether the specified enumeration item is valid or not. A enumeration item is valid if its value is one of
           * the enumeration items other than enumNotSet and enumMaxExclusive.
           *
           * @param item The EnumerationItem to test
           * @return A flag indicating whether the specified EnumerationItem is valid (true) or not (false).
           */
         static bool isValid(EnumerationItem item)
         {
            return (item > enumNotSet) && (item < enumMaxExclusive);
         }


         /** Returns whether the specified name item is valid or not. A name is valid if it is the name of one of the enumeration
           * items other than enumNotSet and enumMaxExclusive.
           *
           * @param name The name to test
           * @return A flag indicating whether the specified name is valid (true) or not (false).
           */
         static bool isValid(const std::string& name)
            throw(uci::base::UCIException)
         {
            initializeTables();
            FromNameTable::const_iterator iter = m_fromNameTable.find(name);
            return !(iter == m_fromNameTable.end());
         }


         /** Tests whether the value of the ProductTypeEnum enumeration on the right hand side of the == operator is equal to the
           * value as this ProductTypeEnum enumeration.
           *
           * @param rhs The ProductTypeEnum enumeration whose value is to be compared against the value of this ProductTypeEnum
           *      enumeration.
           * @return True if the value of the specified ProductTypeEnum enumeration (rhs) is equal to the value of this
           *      ProductTypeEnum enumeration, false otherwise
           */
         bool operator==(const ProductTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() == rhs.getValue();
         }


         /** Tests whether the value of the ProductTypeEnum enumeration on the right hand side of the != operator is not equal to
           * the value as this ProductTypeEnum enumeration.
           *
           * @param rhs The ProductTypeEnum enumeration whose value is to be compared against the value of this ProductTypeEnum
           *      enumeration.
           * @return True if the value of the specified ProductTypeEnum enumeration (rhs) is not equal to the value of this
           *      ProductTypeEnum enumeration, false otherwise
           */
         bool operator!=(const ProductTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() != rhs.getValue();
         }


         /** Tests whether the value of the ProductTypeEnum enumeration on the right hand side of the < operator is less than the
           * value as this ProductTypeEnum enumeration.
           *
           * @param rhs The ProductTypeEnum enumeration whose value is to be compared against the value of this ProductTypeEnum
           *      enumeration.
           * @return True if the value of the specified ProductTypeEnum enumeration (rhs) is less than the value of this
           *      ProductTypeEnum enumeration, false otherwise
           */
         bool operator<(const ProductTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() < rhs.getValue();
         }


         /** Tests whether the value of the ProductTypeEnum enumeration on the right hand side of the <= operator is less than or
           * equal to the value as this ProductTypeEnum enumeration.
           *
           * @param rhs The ProductTypeEnum enumeration whose value is to be compared against the value of this ProductTypeEnum
           *      enumeration.
           * @return True if the value of the specified ProductTypeEnum enumeration (rhs) is less than or equal to the value of
           *      this ProductTypeEnum enumeration, false otherwise
           */
         bool operator<=(const ProductTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() <= rhs.getValue();
         }


         /** Tests whether the value of the ProductTypeEnum enumeration on the right hand side of the > operator is greater than
           * the value as this ProductTypeEnum enumeration.
           *
           * @param rhs The ProductTypeEnum enumeration whose value is to be compared against the value of this ProductTypeEnum
           *      enumeration.
           * @return True if the value of the specified ProductTypeEnum enumeration (rhs) is greater than the value of this
           *      ProductTypeEnum enumeration, false otherwise
           */
         bool operator>(const ProductTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() > rhs.getValue();
         }


         /** Tests whether the value of the ProductTypeEnum enumeration on the right hand side of the >= operator is greater than
           * or equal to the value as this ProductTypeEnum enumeration.
           *
           * @param rhs The ProductTypeEnum enumeration whose value is to be compared against the value of this ProductTypeEnum
           *      enumeration.
           * @return True if the value of the specified ProductTypeEnum enumeration (rhs) is greater than or equal to the value of
           *      this ProductTypeEnum enumeration, false otherwise
           */
         bool operator>=(const ProductTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() >= rhs.getValue();
         }


         /** Tests whether the value of this ProductTypeEnum enumeration is equal to the value of the specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this ProductTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is equal to the value of this ProductTypeEnum enumeration,
           *      false otherwise
           */
         bool operator==(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() == rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is equal to the value of the specified ProductTypeEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified ProductTypeEnum
           *      enumeration.
           * @param rhs The ProductTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is equal to the value of the ProductTypeEnum enumeration (rhs),
           *      false otherwise
           */
         friend bool operator==(EnumerationItem lhs, const ProductTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs == rhs.getValue();
         }


         /** Tests whether the value of this ProductTypeEnum enumeration is not equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this ProductTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is not equal to the value of this ProductTypeEnum enumeration,
           *      false otherwise
           */
         bool operator!=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() != rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is not equal to the value of the specified ProductTypeEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified ProductTypeEnum
           *      enumeration.
           * @param rhs The ProductTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is not equal to the value of the ProductTypeEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator!=(EnumerationItem lhs, const ProductTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs != rhs.getValue();
         }


         /** Tests whether the value of this ProductTypeEnum enumeration is less than the value of the specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this ProductTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than the value of this ProductTypeEnum enumeration,
           *      false otherwise
           */
         bool operator<(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() < rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than the value of the specified ProductTypeEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified ProductTypeEnum
           *      enumeration.
           * @param rhs The ProductTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than the value of the ProductTypeEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator<(EnumerationItem lhs, const ProductTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs < rhs.getValue();
         }


         /** Tests whether the value of this ProductTypeEnum enumeration is less than or equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this ProductTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than or equal to the value of this ProductTypeEnum
           *      enumeration, false otherwise
           */
         bool operator<=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() <= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than or equal to the value of the specified
           * ProductTypeEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified ProductTypeEnum
           *      enumeration.
           * @param rhs The ProductTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than or equal to the value of the ProductTypeEnum
           *      enumeration (rhs), false otherwise
           */
         friend bool operator<=(EnumerationItem lhs, const ProductTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs <= rhs.getValue();
         }


         /** Tests whether the value of this ProductTypeEnum enumeration is greater than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this ProductTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than the value of this ProductTypeEnum enumeration,
           *      false otherwise
           */
         bool operator>(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() > rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than the value of the specified ProductTypeEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified ProductTypeEnum
           *      enumeration.
           * @param rhs The ProductTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than the value of the ProductTypeEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator>(EnumerationItem lhs, const ProductTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs > rhs.getValue();
         }


         /** Tests whether the value of this ProductTypeEnum enumeration is greater than or equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this ProductTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than or equal to the value of this ProductTypeEnum
           *      enumeration, false otherwise
           */
         bool operator>=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() >= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than or equal to the value of the specified
           * ProductTypeEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified ProductTypeEnum
           *      enumeration.
           * @param rhs The ProductTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than or equal to the value of the ProductTypeEnum
           *      enumeration (rhs), false otherwise
           */
         friend bool operator>=(EnumerationItem lhs, const ProductTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs >= rhs.getValue();
         }


         /** Returns the name of the specified enumeration item.
           *
           * @param item The enumeration item whose name is to be returned.
           * @return The enumeration item's name if valid
           */
         static const std::string& toName(EnumerationItem item)
            throw(uci::base::UCIException)
         {
            initializeTables();

            ToNameTable::const_iterator iter = m_toNameTable.find(item);
            if (iter == m_toNameTable.end()) {
               std::ostringstream oStream;
               oStream << "Failed to convert enumerated item to its name, item not found in mapping: " << item;
               throw uci::base::UCIException(oStream.str(), 0);
            }

            return iter->second;
         }


         /** Returns the enumeration item having the specified name.
           *
           * @param itemName The name of the enumeration item to be returned.
           * @return The enumeration item having the specified name
           */
         static EnumerationItem fromName(const std::string& itemName)
            throw(uci::base::UCIException)
         {
            initializeTables();

            FromNameTable::const_iterator iter = m_fromNameTable.find(itemName);
            if (iter == m_fromNameTable.end()) {
               std::ostringstream oStream;
               oStream << "Failed to return enumerated item having specified name, name not found in mapping: " << itemName;
               throw uci::base::UCIException(oStream.str(), 0);
            }

            return iter->second;
         }


         /** Return the name of the enumeration item that the value of this ProductTypeEnum enumeration is set to.
           *
           * @return The name of the enumeration item.
           */
         virtual const std::string& toName() const
            throw(uci::base::UCIException)
         {
            return toName(getValue());
         }


         /** Sets the value of this ProductTypeEnum enumeration to the value of the enumeration item having the specified name.
           *
           * @param itemName The name of the enumeration item whose value is to be used to set this ProductTypeEnum enumeration to
           */
         virtual void setValueFromName(const std::string& itemName)
            throw(uci::base::UCIException)
         {
            setValue(fromName(itemName));
         }



      protected:

         /** The default constructor. In addition to constructing a new ProductTypeEnum, this constructor also makes sure the
           * conversion tables are initialized
           */
         ProductTypeEnum()
         {
            initializeTables();
         }


         /** Tests that the specified enumeration item is valid or not, throwing an exception if the item is not valid.
           *
           * @param additionalDescription An additional description to include in the error message should an exception be thrown
           * @param item The enumeration item to test
           */
         static void testForValidItem(const char* additionalDescription, EnumerationItem item)
            throw(uci::base::UCIException)
         {
            if (!isValid(item)) {
               std::ostringstream oStream;
               oStream << additionalDescription << ": " << item << " (item must be between " << enumNotSet << " and "<< enumMaxExclusive << " exclusive)";
               throw uci::base::UCIException(oStream.str(), 0);
            }

         }


         /** Tests that the specified enumeration item is valid or not, throwing an exception if the item is not valid.
           *
           * @param item The enumeration item to test
           */
         static void testForValidItem(EnumerationItem item)
            throw(uci::base::UCIException)
         {
            testForValidItem("Invalid enumerated item specified", item);
         }



      private:

         //  The type of table that is used to lookup the name of a enumeration item given that enumeration item.
         typedef std::map<EnumerationItem, const std::string> ToNameTable;

         //  The type of table that is used to lookup an enumeration item given its name.
         typedef std::map<const std::string, EnumerationItem> FromNameTable;

         //  The table that is used to lookup the name of a enumeration item given that enumeration item.
         static ToNameTable m_toNameTable;

         //  The type of table that is used to lookup an enumeration item given its name.
         static FromNameTable m_fromNameTable;

         //  Indicates whether the tables has been initialized (true) or not (false).
         static bool m_tablesInitialized;

         //  Initializes the translation tables
         static void initializeTables();


      protected:

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductTypeEnum to copy from
           */
         ProductTypeEnum(const ProductTypeEnum& rhs)
         {
            (void)rhs;
         }

      }; // ProductTypeEnum


   } // Namespace: type
} // Namespace: uci
template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& oStream, const uci::type::ProductTypeEnum& enumeration)
{
   oStream << enumeration.toName();
   return oStream;
}

#endif // Uci__Type__ProductTypeEnum_h

