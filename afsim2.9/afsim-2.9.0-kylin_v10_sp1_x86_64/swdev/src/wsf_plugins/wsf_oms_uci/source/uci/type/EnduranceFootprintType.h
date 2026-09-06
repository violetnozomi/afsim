// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__EnduranceFootprintType_h
#define Uci__Type__EnduranceFootprintType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__BoundaryType_h)
# include "uci/type/BoundaryType.h"
#endif

#if !defined(Uci__Type__AltitudeType_h)
# include "uci/type/AltitudeType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EnduranceFootprintType sequence accessor class */
      class EnduranceFootprintType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EnduranceFootprintType()
         { }

         /** Returns this accessor's type constant, i.e. EnduranceFootprintType
           *
           * @return This accessor's type constant, i.e. EnduranceFootprintType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::enduranceFootprintType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EnduranceFootprintType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Boundary.
           *
           * @return The acecssor that provides access to the complex content that is identified by Boundary.
           */
         virtual const uci::type::BoundaryType& getBoundary() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Boundary.
           *
           * @return The acecssor that provides access to the complex content that is identified by Boundary.
           */
         virtual uci::type::BoundaryType& getBoundary()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Boundary to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Boundary
           */
         virtual void setBoundary(const uci::type::BoundaryType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ReferenceAltitude.
           *
           * @return The value of the simple primitive data type identified by ReferenceAltitude.
           */
         virtual uci::type::AltitudeTypeValue getReferenceAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ReferenceAltitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setReferenceAltitude(uci::type::AltitudeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReferenceAltitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReferenceAltitude is emabled or not
           */
         virtual bool hasReferenceAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReferenceAltitude
           *
           * @param type = uci::type::accessorType::altitudeType This Accessor's accessor type
           */
         virtual void enableReferenceAltitude(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReferenceAltitude */
         virtual void clearReferenceAltitude()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Duration.
           *
           * @return The value of the simple primitive data type identified by Duration.
           */
         virtual uci::type::DurationTypeValue getDuration() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Duration.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDuration(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Duration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Duration is emabled or not
           */
         virtual bool hasDuration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Duration
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableDuration(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Duration */
         virtual void clearDuration()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EnduranceFootprintType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EnduranceFootprintType to copy from
           */
         EnduranceFootprintType(const EnduranceFootprintType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EnduranceFootprintType to the contents of the
           * EnduranceFootprintType on the right hand side (rhs) of the assignment operator.EnduranceFootprintType [only available
           * to derived classes]
           *
           * @param rhs The EnduranceFootprintType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::EnduranceFootprintType
           * @return A constant reference to this EnduranceFootprintType.
           */
         const EnduranceFootprintType& operator=(const EnduranceFootprintType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EnduranceFootprintType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EnduranceFootprintType_h

