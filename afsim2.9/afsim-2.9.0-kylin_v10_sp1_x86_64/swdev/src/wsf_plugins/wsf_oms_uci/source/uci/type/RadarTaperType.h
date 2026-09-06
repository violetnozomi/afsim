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
#ifndef Uci__Type__RadarTaperType_h
#define Uci__Type__RadarTaperType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__RadarTaperWeightingFunctionType_h)
# include "uci/type/RadarTaperWeightingFunctionType.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RadarTaperType sequence accessor class */
      class RadarTaperType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RadarTaperType()
         { }

         /** Returns this accessor's type constant, i.e. RadarTaperType
           *
           * @return This accessor's type constant, i.e. RadarTaperType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::radarTaperType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RadarTaperType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum inclusive: 0] */
         typedef uci::type::DecibelType SidelobeLevel;
         typedef uci::type::DecibelTypeValue SidelobeLevelValue;

         /** Returns the accessor that provides access to the complex content that is identified by the WeightingFunction.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeightingFunction.
           */
         virtual const uci::type::RadarTaperWeightingFunctionType& getWeightingFunction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeightingFunction.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeightingFunction.
           */
         virtual uci::type::RadarTaperWeightingFunctionType& getWeightingFunction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WeightingFunction to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WeightingFunction
           */
         virtual void setWeightingFunction(const uci::type::RadarTaperWeightingFunctionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SidelobeLevel.
           *
           * @return The value of the simple primitive data type identified by SidelobeLevel.
           */
         virtual uci::type::RadarTaperType::SidelobeLevelValue getSidelobeLevel() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SidelobeLevel.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSidelobeLevel(uci::type::RadarTaperType::SidelobeLevelValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SidelobeLevel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SidelobeLevel is emabled or not
           */
         virtual bool hasSidelobeLevel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SidelobeLevel
           *
           * @param type = uci::type::radarTaperType::accessorType::sidelobeLevel This Accessor's accessor type
           */
         virtual void enableSidelobeLevel(uci::base::accessorType::AccessorType type = uci::type::radarTaperType::accessorType::sidelobeLevel)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SidelobeLevel */
         virtual void clearSidelobeLevel()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RadarTaperType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RadarTaperType to copy from
           */
         RadarTaperType(const RadarTaperType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RadarTaperType to the contents of the RadarTaperType on the right
           * hand side (rhs) of the assignment operator.RadarTaperType [only available to derived classes]
           *
           * @param rhs The RadarTaperType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::RadarTaperType
           * @return A constant reference to this RadarTaperType.
           */
         const RadarTaperType& operator=(const RadarTaperType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RadarTaperType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RadarTaperType_h

