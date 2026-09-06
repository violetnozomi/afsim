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
#ifndef Uci__Type__StrengthType_h
#define Uci__Type__StrengthType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__StrengthRangeType_h)
# include "uci/type/StrengthRangeType.h"
#endif

#if !defined(Uci__Type__PercentRangeType_h)
# include "uci/type/PercentRangeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type defines the minimum, maximum strength values and the percentage of tracked vehicles associated with an
        * Entity.
        */
      class StrengthType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~StrengthType()
         { }

         /** Returns this accessor's type constant, i.e. StrengthType
           *
           * @return This accessor's type constant, i.e. StrengthType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::strengthType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const StrengthType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the StrengthValue.
           *
           * @return The acecssor that provides access to the complex content that is identified by StrengthValue.
           */
         virtual const uci::type::StrengthRangeType& getStrengthValue() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the StrengthValue.
           *
           * @return The acecssor that provides access to the complex content that is identified by StrengthValue.
           */
         virtual uci::type::StrengthRangeType& getStrengthValue()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the StrengthValue to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by StrengthValue
           */
         virtual void setStrengthValue(const uci::type::StrengthRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by StrengthValue exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by StrengthValue is emabled or not
           */
         virtual bool hasStrengthValue() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by StrengthValue
           *
           * @param type = uci::type::accessorType::strengthRangeType This Accessor's accessor type
           */
         virtual void enableStrengthValue(uci::base::accessorType::AccessorType type = uci::type::accessorType::strengthRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by StrengthValue */
         virtual void clearStrengthValue()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VehicleCount.
           *
           * @return The acecssor that provides access to the complex content that is identified by VehicleCount.
           */
         virtual const uci::type::StrengthRangeType& getVehicleCount() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VehicleCount.
           *
           * @return The acecssor that provides access to the complex content that is identified by VehicleCount.
           */
         virtual uci::type::StrengthRangeType& getVehicleCount()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the VehicleCount to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by VehicleCount
           */
         virtual void setVehicleCount(const uci::type::StrengthRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VehicleCount exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VehicleCount is emabled or not
           */
         virtual bool hasVehicleCount() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VehicleCount
           *
           * @param type = uci::type::accessorType::strengthRangeType This Accessor's accessor type
           */
         virtual void enableVehicleCount(uci::base::accessorType::AccessorType type = uci::type::accessorType::strengthRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VehicleCount */
         virtual void clearVehicleCount()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PercentTracked.
           *
           * @return The acecssor that provides access to the complex content that is identified by PercentTracked.
           */
         virtual const uci::type::PercentRangeType& getPercentTracked() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PercentTracked.
           *
           * @return The acecssor that provides access to the complex content that is identified by PercentTracked.
           */
         virtual uci::type::PercentRangeType& getPercentTracked()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PercentTracked to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PercentTracked
           */
         virtual void setPercentTracked(const uci::type::PercentRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PercentTracked exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PercentTracked is emabled or not
           */
         virtual bool hasPercentTracked() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PercentTracked
           *
           * @param type = uci::type::accessorType::percentRangeType This Accessor's accessor type
           */
         virtual void enablePercentTracked(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PercentTracked */
         virtual void clearPercentTracked()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         StrengthType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StrengthType to copy from
           */
         StrengthType(const StrengthType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this StrengthType to the contents of the StrengthType on the right hand
           * side (rhs) of the assignment operator.StrengthType [only available to derived classes]
           *
           * @param rhs The StrengthType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::StrengthType
           * @return A constant reference to this StrengthType.
           */
         const StrengthType& operator=(const StrengthType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // StrengthType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__StrengthType_h

