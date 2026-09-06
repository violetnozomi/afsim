// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__CargoType_h
#define Uci__Type__CargoType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CargoPackagingEnum_h)
# include "uci/type/CargoPackagingEnum.h"
#endif

#if !defined(Uci__Type__CargoSizeType_h)
# include "uci/type/CargoSizeType.h"
#endif

#if !defined(Uci__Type__MassType_h)
# include "uci/type/MassType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CargoType sequence accessor class */
      class CargoType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CargoType()
         { }

         /** Returns this accessor's type constant, i.e. CargoType
           *
           * @return This accessor's type constant, i.e. CargoType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cargoType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CargoType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CargoPackaging.
           *
           * @return The value of the enumeration identified by CargoPackaging.
           */
         virtual const uci::type::CargoPackagingEnum& getCargoPackaging() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CargoPackaging.
           *
           * @return The value of the enumeration identified by CargoPackaging.
           */
         virtual uci::type::CargoPackagingEnum& getCargoPackaging()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CargoPackaging.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCargoPackaging(const uci::type::CargoPackagingEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CargoPackaging.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCargoPackaging(uci::type::CargoPackagingEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CargoSize.
           *
           * @return The acecssor that provides access to the complex content that is identified by CargoSize.
           */
         virtual const uci::type::CargoSizeType& getCargoSize() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CargoSize.
           *
           * @return The acecssor that provides access to the complex content that is identified by CargoSize.
           */
         virtual uci::type::CargoSizeType& getCargoSize()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CargoSize to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CargoSize
           */
         virtual void setCargoSize(const uci::type::CargoSizeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CargoSize exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CargoSize is emabled or not
           */
         virtual bool hasCargoSize() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CargoSize
           *
           * @param type = uci::type::accessorType::cargoSizeType This Accessor's accessor type
           */
         virtual void enableCargoSize(uci::base::accessorType::AccessorType type = uci::type::accessorType::cargoSizeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CargoSize */
         virtual void clearCargoSize()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CargoWeight.
           *
           * @return The value of the simple primitive data type identified by CargoWeight.
           */
         virtual uci::type::MassTypeValue getCargoWeight() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CargoWeight.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCargoWeight(uci::type::MassTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CargoWeight exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CargoWeight is emabled or not
           */
         virtual bool hasCargoWeight() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CargoWeight
           *
           * @param type = uci::type::accessorType::massType This Accessor's accessor type
           */
         virtual void enableCargoWeight(uci::base::accessorType::AccessorType type = uci::type::accessorType::massType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CargoWeight */
         virtual void clearCargoWeight()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CargoType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CargoType to copy from
           */
         CargoType(const CargoType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CargoType to the contents of the CargoType on the right hand side
           * (rhs) of the assignment operator.CargoType [only available to derived classes]
           *
           * @param rhs The CargoType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::CargoType
           * @return A constant reference to this CargoType.
           */
         const CargoType& operator=(const CargoType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CargoType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CargoType_h

