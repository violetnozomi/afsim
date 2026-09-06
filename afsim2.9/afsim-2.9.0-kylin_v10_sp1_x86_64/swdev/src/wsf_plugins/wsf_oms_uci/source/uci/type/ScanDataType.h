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
#ifndef Uci__Type__ScanDataType_h
#define Uci__Type__ScanDataType_h 1

#if !defined(Uci__Type__ScanDataBaseType_h)
# include "uci/type/ScanDataBaseType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__MeasurementStatusEnum_h)
# include "uci/type/MeasurementStatusEnum.h"
#endif

#if !defined(Uci__Type__ScanPlaneEnum_h)
# include "uci/type/ScanPlaneEnum.h"
#endif

#if !defined(Uci__Type__AngleType_h)
# include "uci/type/AngleType.h"
#endif

#if !defined(Uci__Type__ScanDirectionalityEnum_h)
# include "uci/type/ScanDirectionalityEnum.h"
#endif

#if !defined(Uci__Type__ScanMechanicsEnum_h)
# include "uci/type/ScanMechanicsEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__LobeType_h)
# include "uci/type/LobeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ScanDataType sequence accessor class */
      class ScanDataType : public virtual uci::type::ScanDataBaseType {
      public:

         /** The destructor */
         virtual ~ScanDataType()
         { }

         /** Returns this accessor's type constant, i.e. ScanDataType
           *
           * @return This accessor's type constant, i.e. ScanDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::scanDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ScanDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the measured time duration of a single scan of a scanning Signal. This element is unbounded to allow
           * reporting of a series of scan periods. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DurationType, uci::type::accessorType::durationType> ScanPeriod;

         /** Indicates a characterization of an amplitude Lobe associated with the Scan. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::LobeType, uci::type::accessorType::lobeType> Lobe;

         /** Returns the value of the SimplePrimitive data type that is identified by the ScanTimestamp.
           *
           * @return The value of the simple primitive data type identified by ScanTimestamp.
           */
         virtual uci::type::DateTimeTypeValue getScanTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ScanTimestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setScanTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ScanMeasurementStatus.
           *
           * @return The value of the enumeration identified by ScanMeasurementStatus.
           */
         virtual const uci::type::MeasurementStatusEnum& getScanMeasurementStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ScanMeasurementStatus.
           *
           * @return The value of the enumeration identified by ScanMeasurementStatus.
           */
         virtual uci::type::MeasurementStatusEnum& getScanMeasurementStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ScanMeasurementStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setScanMeasurementStatus(const uci::type::MeasurementStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ScanMeasurementStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setScanMeasurementStatus(uci::type::MeasurementStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ScanPlane.
           *
           * @return The value of the enumeration identified by ScanPlane.
           */
         virtual const uci::type::ScanPlaneEnum& getScanPlane() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ScanPlane.
           *
           * @return The value of the enumeration identified by ScanPlane.
           */
         virtual uci::type::ScanPlaneEnum& getScanPlane()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ScanPlane.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setScanPlane(const uci::type::ScanPlaneEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ScanPlane.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setScanPlane(uci::type::ScanPlaneEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ScanPlaneis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasScanPlane() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getScanPlane will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::scanPlaneEnum This Accessor's accessor type
           */
         virtual void enableScanPlane(uci::base::accessorType::AccessorType type = uci::type::accessorType::scanPlaneEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearScanPlane()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SectorWidth.
           *
           * @return The value of the simple primitive data type identified by SectorWidth.
           */
         virtual uci::type::AngleTypeValue getSectorWidth() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SectorWidth.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSectorWidth(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SectorWidth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SectorWidth is emabled or not
           */
         virtual bool hasSectorWidth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SectorWidth
           *
           * @param type = uci::type::accessorType::angleType This Accessor's accessor type
           */
         virtual void enableSectorWidth(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SectorWidth */
         virtual void clearSectorWidth()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ScanDirectionality.
           *
           * @return The value of the enumeration identified by ScanDirectionality.
           */
         virtual const uci::type::ScanDirectionalityEnum& getScanDirectionality() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ScanDirectionality.
           *
           * @return The value of the enumeration identified by ScanDirectionality.
           */
         virtual uci::type::ScanDirectionalityEnum& getScanDirectionality()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ScanDirectionality.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setScanDirectionality(const uci::type::ScanDirectionalityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ScanDirectionality.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setScanDirectionality(uci::type::ScanDirectionalityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ScanDirectionalityis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasScanDirectionality() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getScanDirectionality will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::scanDirectionalityEnum This Accessor's accessor type
           */
         virtual void enableScanDirectionality(uci::base::accessorType::AccessorType type = uci::type::accessorType::scanDirectionalityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearScanDirectionality()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ScanMechanics.
           *
           * @return The value of the enumeration identified by ScanMechanics.
           */
         virtual const uci::type::ScanMechanicsEnum& getScanMechanics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ScanMechanics.
           *
           * @return The value of the enumeration identified by ScanMechanics.
           */
         virtual uci::type::ScanMechanicsEnum& getScanMechanics()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ScanMechanics.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setScanMechanics(const uci::type::ScanMechanicsEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ScanMechanics.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setScanMechanics(uci::type::ScanMechanicsEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ScanMechanicsis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasScanMechanics() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getScanMechanics will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::scanMechanicsEnum This Accessor's accessor type
           */
         virtual void enableScanMechanics(uci::base::accessorType::AccessorType type = uci::type::accessorType::scanMechanicsEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearScanMechanics()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ScanPeriod.
           *
           * @return The bounded list identified by ScanPeriod.
           */
         virtual const uci::type::ScanDataType::ScanPeriod& getScanPeriod() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ScanPeriod.
           *
           * @return The bounded list identified by ScanPeriod.
           */
         virtual uci::type::ScanDataType::ScanPeriod& getScanPeriod()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ScanPeriod.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setScanPeriod(const uci::type::ScanDataType::ScanPeriod& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OOK_Present.
           *
           * @return The value of the simple primitive data type identified by OOK_Present.
           */
         virtual xs::Boolean getOOK_Present() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OOK_Present.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOOK_Present(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OOK_Present exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OOK_Present is emabled or not
           */
         virtual bool hasOOK_Present() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OOK_Present
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableOOK_Present(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OOK_Present */
         virtual void clearOOK_Present()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Lobe.
           *
           * @return The bounded list identified by Lobe.
           */
         virtual const uci::type::ScanDataType::Lobe& getLobe() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Lobe.
           *
           * @return The bounded list identified by Lobe.
           */
         virtual uci::type::ScanDataType::Lobe& getLobe()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Lobe.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setLobe(const uci::type::ScanDataType::Lobe& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ScanDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ScanDataType to copy from
           */
         ScanDataType(const ScanDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ScanDataType to the contents of the ScanDataType on the right hand
           * side (rhs) of the assignment operator.ScanDataType [only available to derived classes]
           *
           * @param rhs The ScanDataType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::ScanDataType
           * @return A constant reference to this ScanDataType.
           */
         const ScanDataType& operator=(const ScanDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ScanDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ScanDataType_h

