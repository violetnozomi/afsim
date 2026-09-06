// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__AntennaPerformanceProfileType_h
#define Uci__Type__AntennaPerformanceProfileType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AnglePairType_h)
# include "uci/type/AnglePairType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AntennaPerformanceProfileType sequence accessor class */
      class AntennaPerformanceProfileType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AntennaPerformanceProfileType()
         { }

         /** Returns this accessor's type constant, i.e. AntennaPerformanceProfileType
           *
           * @return This accessor's type constant, i.e. AntennaPerformanceProfileType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::antennaPerformanceProfileType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AntennaPerformanceProfileType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElevationFieldOfRegard.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElevationFieldOfRegard.
           */
         virtual const uci::type::AnglePairType& getElevationFieldOfRegard() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElevationFieldOfRegard.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElevationFieldOfRegard.
           */
         virtual uci::type::AnglePairType& getElevationFieldOfRegard()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ElevationFieldOfRegard to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ElevationFieldOfRegard
           */
         virtual void setElevationFieldOfRegard(const uci::type::AnglePairType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AzimuthFieldOfRegard.
           *
           * @return The acecssor that provides access to the complex content that is identified by AzimuthFieldOfRegard.
           */
         virtual const uci::type::AnglePairType& getAzimuthFieldOfRegard() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AzimuthFieldOfRegard.
           *
           * @return The acecssor that provides access to the complex content that is identified by AzimuthFieldOfRegard.
           */
         virtual uci::type::AnglePairType& getAzimuthFieldOfRegard()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AzimuthFieldOfRegard to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AzimuthFieldOfRegard
           */
         virtual void setAzimuthFieldOfRegard(const uci::type::AnglePairType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DutyFactorLimit.
           *
           * @return The value of the simple primitive data type identified by DutyFactorLimit.
           */
         virtual uci::type::PercentTypeValue getDutyFactorLimit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DutyFactorLimit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDutyFactorLimit(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the BeamLimit.
           *
           * @return The value of the simple primitive data type identified by BeamLimit.
           */
         virtual xs::UnsignedInt getBeamLimit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the BeamLimit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBeamLimit(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Band.
           *
           * @return The acecssor that provides access to the complex content that is identified by Band.
           */
         virtual const uci::type::FrequencyRangeType& getBand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Band.
           *
           * @return The acecssor that provides access to the complex content that is identified by Band.
           */
         virtual uci::type::FrequencyRangeType& getBand()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Band to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Band
           */
         virtual void setBand(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AntennaPerformanceProfileType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AntennaPerformanceProfileType to copy from
           */
         AntennaPerformanceProfileType(const AntennaPerformanceProfileType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AntennaPerformanceProfileType to the contents of the
           * AntennaPerformanceProfileType on the right hand side (rhs) of the assignment operator.AntennaPerformanceProfileType
           * [only available to derived classes]
           *
           * @param rhs The AntennaPerformanceProfileType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::AntennaPerformanceProfileType
           * @return A constant reference to this AntennaPerformanceProfileType.
           */
         const AntennaPerformanceProfileType& operator=(const AntennaPerformanceProfileType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AntennaPerformanceProfileType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AntennaPerformanceProfileType_h

