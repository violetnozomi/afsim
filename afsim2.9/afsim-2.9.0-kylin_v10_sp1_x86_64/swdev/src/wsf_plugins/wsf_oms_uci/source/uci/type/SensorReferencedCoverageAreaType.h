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
#ifndef Uci__Type__SensorReferencedCoverageAreaType_h
#define Uci__Type__SensorReferencedCoverageAreaType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SensorReferencedCoverageAreaReferenceEnum_h)
# include "uci/type/SensorReferencedCoverageAreaReferenceEnum.h"
#endif

#if !defined(Uci__Type__AnglePairType_h)
# include "uci/type/AnglePairType.h"
#endif

#if !defined(Uci__Type__RangeElevationExtentChoiceType_h)
# include "uci/type/RangeElevationExtentChoiceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Definition of a Sensor Reference Coverage Area (SRCA). Allows a Subsystem to include a sensor relative target
        * definition used for air to ground collections
        */
      class SensorReferencedCoverageAreaType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SensorReferencedCoverageAreaType()
         { }

         /** Returns this accessor's type constant, i.e. SensorReferencedCoverageAreaType
           *
           * @return This accessor's type constant, i.e. SensorReferencedCoverageAreaType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sensorReferencedCoverageAreaType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SensorReferencedCoverageAreaType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Reference.
           *
           * @return The value of the enumeration identified by Reference.
           */
         virtual const uci::type::SensorReferencedCoverageAreaReferenceEnum& getReference() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Reference.
           *
           * @return The value of the enumeration identified by Reference.
           */
         virtual uci::type::SensorReferencedCoverageAreaReferenceEnum& getReference()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Reference.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setReference(const uci::type::SensorReferencedCoverageAreaReferenceEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Reference.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setReference(uci::type::SensorReferencedCoverageAreaReferenceEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AzimuthExtent.
           *
           * @return The acecssor that provides access to the complex content that is identified by AzimuthExtent.
           */
         virtual const uci::type::AnglePairType& getAzimuthExtent() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AzimuthExtent.
           *
           * @return The acecssor that provides access to the complex content that is identified by AzimuthExtent.
           */
         virtual uci::type::AnglePairType& getAzimuthExtent()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AzimuthExtent to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AzimuthExtent
           */
         virtual void setAzimuthExtent(const uci::type::AnglePairType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RangeElevationExtent.
           *
           * @return The acecssor that provides access to the complex content that is identified by RangeElevationExtent.
           */
         virtual const uci::type::RangeElevationExtentChoiceType& getRangeElevationExtent() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RangeElevationExtent.
           *
           * @return The acecssor that provides access to the complex content that is identified by RangeElevationExtent.
           */
         virtual uci::type::RangeElevationExtentChoiceType& getRangeElevationExtent()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RangeElevationExtent to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RangeElevationExtent
           */
         virtual void setRangeElevationExtent(const uci::type::RangeElevationExtentChoiceType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SensorReferencedCoverageAreaType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SensorReferencedCoverageAreaType to copy from
           */
         SensorReferencedCoverageAreaType(const SensorReferencedCoverageAreaType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SensorReferencedCoverageAreaType to the contents of the
           * SensorReferencedCoverageAreaType on the right hand side (rhs) of the assignment
           * operator.SensorReferencedCoverageAreaType [only available to derived classes]
           *
           * @param rhs The SensorReferencedCoverageAreaType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::SensorReferencedCoverageAreaType
           * @return A constant reference to this SensorReferencedCoverageAreaType.
           */
         const SensorReferencedCoverageAreaType& operator=(const SensorReferencedCoverageAreaType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SensorReferencedCoverageAreaType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SensorReferencedCoverageAreaType_h

