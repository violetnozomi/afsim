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
#ifndef Uci__Type__TurretReportDynamicPointingType_h
#define Uci__Type__TurretReportDynamicPointingType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AngleType_h)
# include "uci/type/AngleType.h"
#endif

#if !defined(Uci__Type__AngleRateType_h)
# include "uci/type/AngleRateType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the TurretReportDynamicPointingType sequence accessor class */
      class TurretReportDynamicPointingType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~TurretReportDynamicPointingType()
         { }

         /** Returns this accessor's type constant, i.e. TurretReportDynamicPointingType
           *
           * @return This accessor's type constant, i.e. TurretReportDynamicPointingType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::turretReportDynamicPointingType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TurretReportDynamicPointingType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Heading.
           *
           * @return The value of the simple primitive data type identified by Heading.
           */
         virtual uci::type::AngleTypeValue getHeading() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Heading.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHeading(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Pitch.
           *
           * @return The value of the simple primitive data type identified by Pitch.
           */
         virtual uci::type::AngleTypeValue getPitch() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Pitch.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPitch(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the HeadingRate.
           *
           * @return The value of the simple primitive data type identified by HeadingRate.
           */
         virtual uci::type::AngleRateTypeValue getHeadingRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HeadingRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHeadingRate(uci::type::AngleRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HeadingRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HeadingRate is emabled or not
           */
         virtual bool hasHeadingRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HeadingRate
           *
           * @param type = uci::type::accessorType::angleRateType This Accessor's accessor type
           */
         virtual void enableHeadingRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleRateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HeadingRate */
         virtual void clearHeadingRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PitchRate.
           *
           * @return The value of the simple primitive data type identified by PitchRate.
           */
         virtual uci::type::AngleRateTypeValue getPitchRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PitchRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPitchRate(uci::type::AngleRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PitchRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PitchRate is emabled or not
           */
         virtual bool hasPitchRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PitchRate
           *
           * @param type = uci::type::accessorType::angleRateType This Accessor's accessor type
           */
         virtual void enablePitchRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleRateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PitchRate */
         virtual void clearPitchRate()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TurretReportDynamicPointingType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TurretReportDynamicPointingType to copy from
           */
         TurretReportDynamicPointingType(const TurretReportDynamicPointingType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TurretReportDynamicPointingType to the contents of the
           * TurretReportDynamicPointingType on the right hand side (rhs) of the assignment
           * operator.TurretReportDynamicPointingType [only available to derived classes]
           *
           * @param rhs The TurretReportDynamicPointingType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::TurretReportDynamicPointingType
           * @return A constant reference to this TurretReportDynamicPointingType.
           */
         const TurretReportDynamicPointingType& operator=(const TurretReportDynamicPointingType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TurretReportDynamicPointingType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TurretReportDynamicPointingType_h

