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
#ifndef Uci__Type__PositionOrientationCovarianceType_h
#define Uci__Type__PositionOrientationCovarianceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PositionOrientationCovarianceType sequence accessor class */
      class PositionOrientationCovarianceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PositionOrientationCovarianceType()
         { }

         /** Returns this accessor's type constant, i.e. PositionOrientationCovarianceType
           *
           * @return This accessor's type constant, i.e. PositionOrientationCovarianceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::positionOrientationCovarianceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PositionOrientationCovarianceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PnRr.
           *
           * @return The value of the simple primitive data type identified by PnRr.
           */
         virtual xs::Double getPnRr() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PnRr.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPnRr(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PnRr exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PnRr is emabled or not
           */
         virtual bool hasPnRr() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PnRr
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enablePnRr(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PnRr */
         virtual void clearPnRr()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PnRp.
           *
           * @return The value of the simple primitive data type identified by PnRp.
           */
         virtual xs::Double getPnRp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PnRp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPnRp(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PnRp exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PnRp is emabled or not
           */
         virtual bool hasPnRp() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PnRp
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enablePnRp(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PnRp */
         virtual void clearPnRp()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PnRy.
           *
           * @return The value of the simple primitive data type identified by PnRy.
           */
         virtual xs::Double getPnRy() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PnRy.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPnRy(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PnRy exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PnRy is emabled or not
           */
         virtual bool hasPnRy() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PnRy
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enablePnRy(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PnRy */
         virtual void clearPnRy()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PeRr.
           *
           * @return The value of the simple primitive data type identified by PeRr.
           */
         virtual xs::Double getPeRr() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PeRr.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPeRr(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PeRr exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PeRr is emabled or not
           */
         virtual bool hasPeRr() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PeRr
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enablePeRr(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PeRr */
         virtual void clearPeRr()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PeRp.
           *
           * @return The value of the simple primitive data type identified by PeRp.
           */
         virtual xs::Double getPeRp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PeRp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPeRp(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PeRp exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PeRp is emabled or not
           */
         virtual bool hasPeRp() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PeRp
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enablePeRp(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PeRp */
         virtual void clearPeRp()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PeRy.
           *
           * @return The value of the simple primitive data type identified by PeRy.
           */
         virtual xs::Double getPeRy() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PeRy.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPeRy(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PeRy exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PeRy is emabled or not
           */
         virtual bool hasPeRy() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PeRy
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enablePeRy(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PeRy */
         virtual void clearPeRy()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PdRr.
           *
           * @return The value of the simple primitive data type identified by PdRr.
           */
         virtual xs::Double getPdRr() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PdRr.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPdRr(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PdRp.
           *
           * @return The value of the simple primitive data type identified by PdRp.
           */
         virtual xs::Double getPdRp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PdRp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPdRp(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PdRy.
           *
           * @return The value of the simple primitive data type identified by PdRy.
           */
         virtual xs::Double getPdRy() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PdRy.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPdRy(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PdRy exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PdRy is emabled or not
           */
         virtual bool hasPdRy() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PdRy
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enablePdRy(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PdRy */
         virtual void clearPdRy()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PositionOrientationCovarianceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PositionOrientationCovarianceType to copy from
           */
         PositionOrientationCovarianceType(const PositionOrientationCovarianceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PositionOrientationCovarianceType to the contents of the
           * PositionOrientationCovarianceType on the right hand side (rhs) of the assignment
           * operator.PositionOrientationCovarianceType [only available to derived classes]
           *
           * @param rhs The PositionOrientationCovarianceType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::PositionOrientationCovarianceType
           * @return A constant reference to this PositionOrientationCovarianceType.
           */
         const PositionOrientationCovarianceType& operator=(const PositionOrientationCovarianceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PositionOrientationCovarianceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PositionOrientationCovarianceType_h

