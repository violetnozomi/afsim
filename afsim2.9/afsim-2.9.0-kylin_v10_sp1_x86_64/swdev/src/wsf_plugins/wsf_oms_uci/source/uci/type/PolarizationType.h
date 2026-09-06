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
#ifndef Uci__Type__PolarizationType_h
#define Uci__Type__PolarizationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AngleType_h)
# include "uci/type/AngleType.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Type__PolarizationSenseEnum_h)
# include "uci/type/PolarizationSenseEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type defines the geometrical orientation of RF wave oscillations */
      class PolarizationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PolarizationType()
         { }

         /** Returns this accessor's type constant, i.e. PolarizationType
           *
           * @return This accessor's type constant, i.e. PolarizationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::polarizationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PolarizationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PolarizationTiltAngle.
           *
           * @return The value of the simple primitive data type identified by PolarizationTiltAngle.
           */
         virtual uci::type::AngleTypeValue getPolarizationTiltAngle() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PolarizationTiltAngle.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPolarizationTiltAngle(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PolarizationAxialRatio.
           *
           * @return The value of the simple primitive data type identified by PolarizationAxialRatio.
           */
         virtual uci::type::DecibelTypeValue getPolarizationAxialRatio() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PolarizationAxialRatio.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPolarizationAxialRatio(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PolarizationAxialRatio exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PolarizationAxialRatio is emabled or not
           */
         virtual bool hasPolarizationAxialRatio() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PolarizationAxialRatio
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enablePolarizationAxialRatio(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PolarizationAxialRatio */
         virtual void clearPolarizationAxialRatio()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PolarizationSense.
           *
           * @return The value of the enumeration identified by PolarizationSense.
           */
         virtual const uci::type::PolarizationSenseEnum& getPolarizationSense() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PolarizationSense.
           *
           * @return The value of the enumeration identified by PolarizationSense.
           */
         virtual uci::type::PolarizationSenseEnum& getPolarizationSense()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PolarizationSense.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPolarizationSense(const uci::type::PolarizationSenseEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PolarizationSense.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPolarizationSense(uci::type::PolarizationSenseEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PolarizationSenseis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPolarizationSense() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPolarizationSense will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::polarizationSenseEnum This Accessor's accessor type
           */
         virtual void enablePolarizationSense(uci::base::accessorType::AccessorType type = uci::type::accessorType::polarizationSenseEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPolarizationSense()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PolarizationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PolarizationType to copy from
           */
         PolarizationType(const PolarizationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PolarizationType to the contents of the PolarizationType on the
           * right hand side (rhs) of the assignment operator.PolarizationType [only available to derived classes]
           *
           * @param rhs The PolarizationType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::PolarizationType
           * @return A constant reference to this PolarizationType.
           */
         const PolarizationType& operator=(const PolarizationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PolarizationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PolarizationType_h

