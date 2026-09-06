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
#ifndef Uci__Type__BeamWeightingType_h
#define Uci__Type__BeamWeightingType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__BeamWeightingEnum_h)
# include "uci/type/BeamWeightingEnum.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the BeamWeightingType sequence accessor class */
      class BeamWeightingType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~BeamWeightingType()
         { }

         /** Returns this accessor's type constant, i.e. BeamWeightingType
           *
           * @return This accessor's type constant, i.e. BeamWeightingType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::beamWeightingType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const BeamWeightingType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the BeamWeighting.
           *
           * @return The value of the enumeration identified by BeamWeighting.
           */
         virtual const uci::type::BeamWeightingEnum& getBeamWeighting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the BeamWeighting.
           *
           * @return The value of the enumeration identified by BeamWeighting.
           */
         virtual uci::type::BeamWeightingEnum& getBeamWeighting()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the BeamWeighting.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setBeamWeighting(const uci::type::BeamWeightingEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the BeamWeighting.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setBeamWeighting(uci::type::BeamWeightingEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the BeamShapeSidelobeWeighting.
           *
           * @return The value of the simple primitive data type identified by BeamShapeSidelobeWeighting.
           */
         virtual uci::type::DecibelTypeValue getBeamShapeSidelobeWeighting() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the BeamShapeSidelobeWeighting.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBeamShapeSidelobeWeighting(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BeamShapeSidelobeWeighting exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BeamShapeSidelobeWeighting is emabled or not
           */
         virtual bool hasBeamShapeSidelobeWeighting() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BeamShapeSidelobeWeighting
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableBeamShapeSidelobeWeighting(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BeamShapeSidelobeWeighting */
         virtual void clearBeamShapeSidelobeWeighting()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         BeamWeightingType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The BeamWeightingType to copy from
           */
         BeamWeightingType(const BeamWeightingType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this BeamWeightingType to the contents of the BeamWeightingType on the
           * right hand side (rhs) of the assignment operator.BeamWeightingType [only available to derived classes]
           *
           * @param rhs The BeamWeightingType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::BeamWeightingType
           * @return A constant reference to this BeamWeightingType.
           */
         const BeamWeightingType& operator=(const BeamWeightingType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // BeamWeightingType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__BeamWeightingType_h

