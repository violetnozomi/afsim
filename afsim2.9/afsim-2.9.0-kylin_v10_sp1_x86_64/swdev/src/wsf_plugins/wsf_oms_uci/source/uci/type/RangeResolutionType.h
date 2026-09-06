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
#ifndef Uci__Type__RangeResolutionType_h
#define Uci__Type__RangeResolutionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates the down range and / or cross range resolution of a collection */
      class RangeResolutionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RangeResolutionType()
         { }

         /** Returns this accessor's type constant, i.e. RangeResolutionType
           *
           * @return This accessor's type constant, i.e. RangeResolutionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rangeResolutionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RangeResolutionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Range.
           *
           * @return The value of the simple primitive data type identified by Range.
           */
         virtual uci::type::DistanceTypeValue getRange() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Range.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRange(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Range exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Range is emabled or not
           */
         virtual bool hasRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Range
           *
           * @param type = uci::type::accessorType::distanceType This Accessor's accessor type
           */
         virtual void enableRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::distanceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Range */
         virtual void clearRange()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CrossRange.
           *
           * @return The value of the simple primitive data type identified by CrossRange.
           */
         virtual uci::type::DistanceTypeValue getCrossRange() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CrossRange.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCrossRange(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CrossRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CrossRange is emabled or not
           */
         virtual bool hasCrossRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CrossRange
           *
           * @param type = uci::type::accessorType::distanceType This Accessor's accessor type
           */
         virtual void enableCrossRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::distanceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CrossRange */
         virtual void clearCrossRange()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RangeResolutionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RangeResolutionType to copy from
           */
         RangeResolutionType(const RangeResolutionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RangeResolutionType to the contents of the RangeResolutionType on
           * the right hand side (rhs) of the assignment operator.RangeResolutionType [only available to derived classes]
           *
           * @param rhs The RangeResolutionType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::RangeResolutionType
           * @return A constant reference to this RangeResolutionType.
           */
         const RangeResolutionType& operator=(const RangeResolutionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RangeResolutionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RangeResolutionType_h

