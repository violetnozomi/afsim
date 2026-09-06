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
#ifndef Uci__Type__TLE_BaseType_h
#define Uci__Type__TLE_BaseType_h 1

#if !defined(Uci__Type__COE_OrbitBaseType_h)
# include "uci/type/COE_OrbitBaseType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__ElementSetNumberType_h)
# include "uci/type/ElementSetNumberType.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

#if !defined(Uci__Type__COE_PositionType_h)
# include "uci/type/COE_PositionType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates the default or "catalog" orbital element set (also known as two line element or TLE) for the satellite. */
      class TLE_BaseType : public virtual uci::type::COE_OrbitBaseType {
      public:

         /** The destructor */
         virtual ~TLE_BaseType()
         { }

         /** Returns this accessor's type constant, i.e. TLE_BaseType
           *
           * @return This accessor's type constant, i.e. TLE_BaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::tLE_BaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TLE_BaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Epoch.
           *
           * @return The value of the simple primitive data type identified by Epoch.
           */
         virtual uci::type::DateTimeTypeValue getEpoch() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Epoch.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEpoch(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ElementSetNumber.
           *
           * @return The value of the simple primitive data type identified by ElementSetNumber.
           */
         virtual uci::type::ElementSetNumberTypeValue getElementSetNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ElementSetNumber.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setElementSetNumber(uci::type::ElementSetNumberTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ElementSetNumber exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ElementSetNumber is emabled or not
           */
         virtual bool hasElementSetNumber() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ElementSetNumber
           *
           * @param type = uci::type::accessorType::elementSetNumberType This Accessor's accessor type
           */
         virtual void enableElementSetNumber(uci::base::accessorType::AccessorType type = uci::type::accessorType::elementSetNumberType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ElementSetNumber */
         virtual void clearElementSetNumber()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FirstTimeDerivativeOfMeanMotion.
           *
           * @return The value of the simple primitive data type identified by FirstTimeDerivativeOfMeanMotion.
           */
         virtual xs::Double getFirstTimeDerivativeOfMeanMotion() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FirstTimeDerivativeOfMeanMotion.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFirstTimeDerivativeOfMeanMotion(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FirstTimeDerivativeOfMeanMotion exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FirstTimeDerivativeOfMeanMotion is emabled or not
           */
         virtual bool hasFirstTimeDerivativeOfMeanMotion() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FirstTimeDerivativeOfMeanMotion
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableFirstTimeDerivativeOfMeanMotion(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FirstTimeDerivativeOfMeanMotion */
         virtual void clearFirstTimeDerivativeOfMeanMotion()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SecondTimeDerivativeOfMeanMotion.
           *
           * @return The value of the simple primitive data type identified by SecondTimeDerivativeOfMeanMotion.
           */
         virtual xs::Double getSecondTimeDerivativeOfMeanMotion() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SecondTimeDerivativeOfMeanMotion.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSecondTimeDerivativeOfMeanMotion(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SecondTimeDerivativeOfMeanMotion exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SecondTimeDerivativeOfMeanMotion is emabled or not
           */
         virtual bool hasSecondTimeDerivativeOfMeanMotion() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SecondTimeDerivativeOfMeanMotion
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableSecondTimeDerivativeOfMeanMotion(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SecondTimeDerivativeOfMeanMotion */
         virtual void clearSecondTimeDerivativeOfMeanMotion()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the BSTAR_Drag.
           *
           * @return The value of the simple primitive data type identified by BSTAR_Drag.
           */
         virtual xs::Double getBSTAR_Drag() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the BSTAR_Drag.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBSTAR_Drag(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BSTAR_Drag exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BSTAR_Drag is emabled or not
           */
         virtual bool hasBSTAR_Drag() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BSTAR_Drag
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableBSTAR_Drag(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BSTAR_Drag */
         virtual void clearBSTAR_Drag()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual const uci::type::COE_PositionType& getPosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual uci::type::COE_PositionType& getPosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Position to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Position
           */
         virtual void setPosition(const uci::type::COE_PositionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MeanMotion.
           *
           * @return The value of the simple primitive data type identified by MeanMotion.
           */
         virtual xs::Double getMeanMotion() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MeanMotion.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMeanMotion(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RevolutionNumber.
           *
           * @return The value of the simple primitive data type identified by RevolutionNumber.
           */
         virtual xs::UnsignedInt getRevolutionNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RevolutionNumber.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRevolutionNumber(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RevolutionNumber exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RevolutionNumber is emabled or not
           */
         virtual bool hasRevolutionNumber() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RevolutionNumber
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableRevolutionNumber(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RevolutionNumber */
         virtual void clearRevolutionNumber()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TLE_BaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TLE_BaseType to copy from
           */
         TLE_BaseType(const TLE_BaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TLE_BaseType to the contents of the TLE_BaseType on the right hand
           * side (rhs) of the assignment operator.TLE_BaseType [only available to derived classes]
           *
           * @param rhs The TLE_BaseType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::TLE_BaseType
           * @return A constant reference to this TLE_BaseType.
           */
         const TLE_BaseType& operator=(const TLE_BaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TLE_BaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TLE_BaseType_h

