// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__SAR_CollectionConstraintsType_h
#define Uci__Type__SAR_CollectionConstraintsType_h 1

#if !defined(Uci__Type__CollectionConstraintsType_h)
# include "uci/type/CollectionConstraintsType.h"
#endif

#if !defined(Uci__Type__SAR_CollectionConstraintsQualityType_h)
# include "uci/type/SAR_CollectionConstraintsQualityType.h"
#endif

#if !defined(Uci__Type__AngleQuarterType_h)
# include "uci/type/AngleQuarterType.h"
#endif

#if !defined(Uci__Type__AltitudeRangeType_h)
# include "uci/type/AltitudeRangeType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Type__AngleType_h)
# include "uci/type/AngleType.h"
#endif

#if !defined(Uci__Type__AnglePositiveType_h)
# include "uci/type/AnglePositiveType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

#if !defined(Uci__Type__EmconConstraintType_h)
# include "uci/type/EmconConstraintType.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Type__PositionLocationUncertaintyType_h)
# include "uci/type/PositionLocationUncertaintyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates a set of bounding requirements for the particular collection associated with this command. The subsystem
        * should attempt to satisfy all specified constraints.
        */
      class SAR_CollectionConstraintsType : public virtual uci::type::CollectionConstraintsType {
      public:

         /** The destructor */
         virtual ~SAR_CollectionConstraintsType()
         { }

         /** Returns this accessor's type constant, i.e. SAR_CollectionConstraintsType
           *
           * @return This accessor's type constant, i.e. SAR_CollectionConstraintsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sAR_CollectionConstraintsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SAR_CollectionConstraintsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Quality.
           *
           * @return The acecssor that provides access to the complex content that is identified by Quality.
           */
         virtual const uci::type::SAR_CollectionConstraintsQualityType& getQuality() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Quality.
           *
           * @return The acecssor that provides access to the complex content that is identified by Quality.
           */
         virtual uci::type::SAR_CollectionConstraintsQualityType& getQuality()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Quality to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Quality
           */
         virtual void setQuality(const uci::type::SAR_CollectionConstraintsQualityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Quality exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Quality is emabled or not
           */
         virtual bool hasQuality() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Quality
           *
           * @param type = uci::type::accessorType::sAR_CollectionConstraintsQualityType This Accessor's accessor type
           */
         virtual void enableQuality(uci::base::accessorType::AccessorType type = uci::type::accessorType::sAR_CollectionConstraintsQualityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Quality */
         virtual void clearQuality()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ContinuousSpotAngle.
           *
           * @return The value of the simple primitive data type identified by ContinuousSpotAngle.
           */
         virtual uci::type::AngleQuarterTypeValue getContinuousSpotAngle() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ContinuousSpotAngle.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setContinuousSpotAngle(uci::type::AngleQuarterTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ContinuousSpotAngle exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ContinuousSpotAngle is emabled or not
           */
         virtual bool hasContinuousSpotAngle() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ContinuousSpotAngle
           *
           * @param type = uci::type::accessorType::angleQuarterType This Accessor's accessor type
           */
         virtual void enableContinuousSpotAngle(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleQuarterType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ContinuousSpotAngle */
         virtual void clearContinuousSpotAngle()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AltitudeConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by AltitudeConstraints.
           */
         virtual const uci::type::AltitudeRangeType& getAltitudeConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AltitudeConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by AltitudeConstraints.
           */
         virtual uci::type::AltitudeRangeType& getAltitudeConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AltitudeConstraints to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AltitudeConstraints
           */
         virtual void setAltitudeConstraints(const uci::type::AltitudeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AltitudeConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AltitudeConstraints is emabled or not
           */
         virtual bool hasAltitudeConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AltitudeConstraints
           *
           * @param type = uci::type::accessorType::altitudeRangeType This Accessor's accessor type
           */
         virtual void enableAltitudeConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AltitudeConstraints */
         virtual void clearAltitudeConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaximumMappingTime.
           *
           * @return The value of the simple primitive data type identified by MaximumMappingTime.
           */
         virtual uci::type::DurationTypeValue getMaximumMappingTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaximumMappingTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaximumMappingTime(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MaximumMappingTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MaximumMappingTime is emabled or not
           */
         virtual bool hasMaximumMappingTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MaximumMappingTime
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableMaximumMappingTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MaximumMappingTime */
         virtual void clearMaximumMappingTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AzSubsetFraction.
           *
           * @return The value of the simple primitive data type identified by AzSubsetFraction.
           */
         virtual uci::type::AngleTypeValue getAzSubsetFraction() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AzSubsetFraction.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAzSubsetFraction(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AzSubsetFraction exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AzSubsetFraction is emabled or not
           */
         virtual bool hasAzSubsetFraction() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AzSubsetFraction
           *
           * @param type = uci::type::accessorType::angleType This Accessor's accessor type
           */
         virtual void enableAzSubsetFraction(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AzSubsetFraction */
         virtual void clearAzSubsetFraction()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LOSBufferAngle.
           *
           * @return The value of the simple primitive data type identified by LOSBufferAngle.
           */
         virtual uci::type::AnglePositiveTypeValue getLOSBufferAngle() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LOSBufferAngle.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLOSBufferAngle(uci::type::AnglePositiveTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LOSBufferAngle exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LOSBufferAngle is emabled or not
           */
         virtual bool hasLOSBufferAngle() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LOSBufferAngle
           *
           * @param type = uci::type::accessorType::anglePositiveType This Accessor's accessor type
           */
         virtual void enableLOSBufferAngle(uci::base::accessorType::AccessorType type = uci::type::accessorType::anglePositiveType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LOSBufferAngle */
         virtual void clearLOSBufferAngle()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NumImagesAzimuth.
           *
           * @return The value of the simple primitive data type identified by NumImagesAzimuth.
           */
         virtual xs::UnsignedInt getNumImagesAzimuth() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NumImagesAzimuth.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumImagesAzimuth(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NumImagesAzimuth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NumImagesAzimuth is emabled or not
           */
         virtual bool hasNumImagesAzimuth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NumImagesAzimuth
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableNumImagesAzimuth(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NumImagesAzimuth */
         virtual void clearNumImagesAzimuth()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NumImagesRange.
           *
           * @return The value of the simple primitive data type identified by NumImagesRange.
           */
         virtual xs::UnsignedInt getNumImagesRange() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NumImagesRange.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumImagesRange(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NumImagesRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NumImagesRange is emabled or not
           */
         virtual bool hasNumImagesRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NumImagesRange
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableNumImagesRange(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NumImagesRange */
         virtual void clearNumImagesRange()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentAzimuthCollection.
           *
           * @return The value of the simple primitive data type identified by PercentAzimuthCollection.
           */
         virtual uci::type::PercentTypeValue getPercentAzimuthCollection() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentAzimuthCollection.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentAzimuthCollection(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PercentAzimuthCollection exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PercentAzimuthCollection is emabled or not
           */
         virtual bool hasPercentAzimuthCollection() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PercentAzimuthCollection
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enablePercentAzimuthCollection(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PercentAzimuthCollection */
         virtual void clearPercentAzimuthCollection()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the HeadingDeviationTolerance.
           *
           * @return The value of the simple primitive data type identified by HeadingDeviationTolerance.
           */
         virtual uci::type::AnglePositiveTypeValue getHeadingDeviationTolerance() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HeadingDeviationTolerance.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHeadingDeviationTolerance(uci::type::AnglePositiveTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HeadingDeviationTolerance exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HeadingDeviationTolerance is emabled or not
           */
         virtual bool hasHeadingDeviationTolerance() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HeadingDeviationTolerance
           *
           * @param type = uci::type::accessorType::anglePositiveType This Accessor's accessor type
           */
         virtual void enableHeadingDeviationTolerance(uci::base::accessorType::AccessorType type = uci::type::accessorType::anglePositiveType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HeadingDeviationTolerance */
         virtual void clearHeadingDeviationTolerance()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CrossTrackDeviationTolerance.
           *
           * @return The value of the simple primitive data type identified by CrossTrackDeviationTolerance.
           */
         virtual uci::type::DistanceTypeValue getCrossTrackDeviationTolerance() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CrossTrackDeviationTolerance.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCrossTrackDeviationTolerance(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CrossTrackDeviationTolerance exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CrossTrackDeviationTolerance is emabled or not
           */
         virtual bool hasCrossTrackDeviationTolerance() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CrossTrackDeviationTolerance
           *
           * @param type = uci::type::accessorType::distanceType This Accessor's accessor type
           */
         virtual void enableCrossTrackDeviationTolerance(uci::base::accessorType::AccessorType type = uci::type::accessorType::distanceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CrossTrackDeviationTolerance */
         virtual void clearCrossTrackDeviationTolerance()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Emcon.
           *
           * @return The acecssor that provides access to the complex content that is identified by Emcon.
           */
         virtual const uci::type::EmconConstraintType& getEmcon() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Emcon.
           *
           * @return The acecssor that provides access to the complex content that is identified by Emcon.
           */
         virtual uci::type::EmconConstraintType& getEmcon()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Emcon to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Emcon
           */
         virtual void setEmcon(const uci::type::EmconConstraintType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Emcon exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Emcon is emabled or not
           */
         virtual bool hasEmcon() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Emcon
           *
           * @param type = uci::type::accessorType::emconConstraintType This Accessor's accessor type
           */
         virtual void enableEmcon(uci::base::accessorType::AccessorType type = uci::type::accessorType::emconConstraintType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Emcon */
         virtual void clearEmcon()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MapGainAdjustment.
           *
           * @return The value of the simple primitive data type identified by MapGainAdjustment.
           */
         virtual uci::type::DecibelTypeValue getMapGainAdjustment() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MapGainAdjustment.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMapGainAdjustment(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MapGainAdjustment exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MapGainAdjustment is emabled or not
           */
         virtual bool hasMapGainAdjustment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MapGainAdjustment
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableMapGainAdjustment(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MapGainAdjustment */
         virtual void clearMapGainAdjustment()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OwnshipLocationConstraint.
           *
           * @return The acecssor that provides access to the complex content that is identified by OwnshipLocationConstraint.
           */
         virtual const uci::type::PositionLocationUncertaintyType& getOwnshipLocationConstraint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OwnshipLocationConstraint.
           *
           * @return The acecssor that provides access to the complex content that is identified by OwnshipLocationConstraint.
           */
         virtual uci::type::PositionLocationUncertaintyType& getOwnshipLocationConstraint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OwnshipLocationConstraint to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OwnshipLocationConstraint
           */
         virtual void setOwnshipLocationConstraint(const uci::type::PositionLocationUncertaintyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OwnshipLocationConstraint exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OwnshipLocationConstraint is emabled or not
           */
         virtual bool hasOwnshipLocationConstraint() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OwnshipLocationConstraint
           *
           * @param type = uci::type::accessorType::positionLocationUncertaintyType This Accessor's accessor type
           */
         virtual void enableOwnshipLocationConstraint(uci::base::accessorType::AccessorType type = uci::type::accessorType::positionLocationUncertaintyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OwnshipLocationConstraint */
         virtual void clearOwnshipLocationConstraint()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SAR_CollectionConstraintsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_CollectionConstraintsType to copy from
           */
         SAR_CollectionConstraintsType(const SAR_CollectionConstraintsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_CollectionConstraintsType to the contents of the
           * SAR_CollectionConstraintsType on the right hand side (rhs) of the assignment operator.SAR_CollectionConstraintsType
           * [only available to derived classes]
           *
           * @param rhs The SAR_CollectionConstraintsType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::SAR_CollectionConstraintsType
           * @return A constant reference to this SAR_CollectionConstraintsType.
           */
         const SAR_CollectionConstraintsType& operator=(const SAR_CollectionConstraintsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_CollectionConstraintsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SAR_CollectionConstraintsType_h

