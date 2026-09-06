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
#ifndef Uci__Type__ModRateDataType_h
#define Uci__Type__ModRateDataType_h 1

#if !defined(Uci__Type__DataRateDataType_h)
# include "uci/type/DataRateDataType.h"
#endif

#if !defined(Uci__Type__ModulationEnum_h)
# include "uci/type/ModulationEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ModRateDataType sequence accessor class */
      class ModRateDataType : public virtual uci::type::DataRateDataType {
      public:

         /** The destructor */
         virtual ~ModRateDataType()
         { }

         /** Returns this accessor's type constant, i.e. ModRateDataType
           *
           * @return This accessor's type constant, i.e. ModRateDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::modRateDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ModRateDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ModType.
           *
           * @return The value of the enumeration identified by ModType.
           */
         virtual const uci::type::ModulationEnum& getModType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ModType.
           *
           * @return The value of the enumeration identified by ModType.
           */
         virtual uci::type::ModulationEnum& getModType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ModType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setModType(const uci::type::ModulationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ModType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setModType(uci::type::ModulationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ModTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasModType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getModType will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::modulationEnum This Accessor's accessor type
           */
         virtual void enableModType(uci::base::accessorType::AccessorType type = uci::type::accessorType::modulationEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearModType()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ModRateDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ModRateDataType to copy from
           */
         ModRateDataType(const ModRateDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ModRateDataType to the contents of the ModRateDataType on the
           * right hand side (rhs) of the assignment operator.ModRateDataType [only available to derived classes]
           *
           * @param rhs The ModRateDataType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::ModRateDataType
           * @return A constant reference to this ModRateDataType.
           */
         const ModRateDataType& operator=(const ModRateDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ModRateDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ModRateDataType_h

