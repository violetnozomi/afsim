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
#ifndef Uci__Type__ScanDataBaseType_h
#define Uci__Type__ScanDataBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ScanTypeEnum_h)
# include "uci/type/ScanTypeEnum.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ScanDataBaseType sequence accessor class */
      class ScanDataBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ScanDataBaseType()
         { }

         /** Returns this accessor's type constant, i.e. ScanDataBaseType
           *
           * @return This accessor's type constant, i.e. ScanDataBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::scanDataBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ScanDataBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a type/class/category of scan being performed by the emitter of the Signal. See enumerated type annotations
           * for further details. [Maximum occurrences: 27]
           */
         typedef uci::base::BoundedList<uci::type::ScanTypeEnum, uci::type::accessorType::scanTypeEnum> ScanType;

         /** Returns the bounded list that is identified by the ScanType.
           *
           * @return The bounded list identified by ScanType.
           */
         virtual const uci::type::ScanDataBaseType::ScanType& getScanType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ScanType.
           *
           * @return The bounded list identified by ScanType.
           */
         virtual uci::type::ScanDataBaseType::ScanType& getScanType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ScanType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setScanType(const uci::type::ScanDataBaseType::ScanType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ScanPeriodMin.
           *
           * @return The value of the simple primitive data type identified by ScanPeriodMin.
           */
         virtual uci::type::DurationTypeValue getScanPeriodMin() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ScanPeriodMin.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setScanPeriodMin(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ScanPeriodMin exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ScanPeriodMin is emabled or not
           */
         virtual bool hasScanPeriodMin() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ScanPeriodMin
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableScanPeriodMin(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ScanPeriodMin */
         virtual void clearScanPeriodMin()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ScanPeriodMax.
           *
           * @return The value of the simple primitive data type identified by ScanPeriodMax.
           */
         virtual uci::type::DurationTypeValue getScanPeriodMax() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ScanPeriodMax.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setScanPeriodMax(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ScanPeriodMax exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ScanPeriodMax is emabled or not
           */
         virtual bool hasScanPeriodMax() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ScanPeriodMax
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableScanPeriodMax(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ScanPeriodMax */
         virtual void clearScanPeriodMax()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IlluminationTime.
           *
           * @return The value of the simple primitive data type identified by IlluminationTime.
           */
         virtual uci::type::DurationTypeValue getIlluminationTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IlluminationTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIlluminationTime(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IlluminationTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IlluminationTime is emabled or not
           */
         virtual bool hasIlluminationTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IlluminationTime
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableIlluminationTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IlluminationTime */
         virtual void clearIlluminationTime()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ScanDataBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ScanDataBaseType to copy from
           */
         ScanDataBaseType(const ScanDataBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ScanDataBaseType to the contents of the ScanDataBaseType on the
           * right hand side (rhs) of the assignment operator.ScanDataBaseType [only available to derived classes]
           *
           * @param rhs The ScanDataBaseType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ScanDataBaseType
           * @return A constant reference to this ScanDataBaseType.
           */
         const ScanDataBaseType& operator=(const ScanDataBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ScanDataBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ScanDataBaseType_h

