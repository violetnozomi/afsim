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
#ifndef Uci__Type__PositionVelocityCovarianceType_h
#define Uci__Type__PositionVelocityCovarianceType_h 1

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

      /** This is the PositionVelocityCovarianceType sequence accessor class */
      class PositionVelocityCovarianceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PositionVelocityCovarianceType()
         { }

         /** Returns this accessor's type constant, i.e. PositionVelocityCovarianceType
           *
           * @return This accessor's type constant, i.e. PositionVelocityCovarianceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::positionVelocityCovarianceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PositionVelocityCovarianceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PnVn.
           *
           * @return The value of the simple primitive data type identified by PnVn.
           */
         virtual xs::Double getPnVn() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PnVn.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPnVn(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PnVe.
           *
           * @return The value of the simple primitive data type identified by PnVe.
           */
         virtual xs::Double getPnVe() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PnVe.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPnVe(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PnVd.
           *
           * @return The value of the simple primitive data type identified by PnVd.
           */
         virtual xs::Double getPnVd() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PnVd.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPnVd(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PnVd exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PnVd is emabled or not
           */
         virtual bool hasPnVd() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PnVd
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enablePnVd(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PnVd */
         virtual void clearPnVd()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PeVn.
           *
           * @return The value of the simple primitive data type identified by PeVn.
           */
         virtual xs::Double getPeVn() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PeVn.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPeVn(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PeVe.
           *
           * @return The value of the simple primitive data type identified by PeVe.
           */
         virtual xs::Double getPeVe() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PeVe.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPeVe(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PeVd.
           *
           * @return The value of the simple primitive data type identified by PeVd.
           */
         virtual xs::Double getPeVd() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PeVd.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPeVd(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PeVd exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PeVd is emabled or not
           */
         virtual bool hasPeVd() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PeVd
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enablePeVd(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PeVd */
         virtual void clearPeVd()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PdVn.
           *
           * @return The value of the simple primitive data type identified by PdVn.
           */
         virtual xs::Double getPdVn() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PdVn.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPdVn(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PdVn exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PdVn is emabled or not
           */
         virtual bool hasPdVn() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PdVn
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enablePdVn(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PdVn */
         virtual void clearPdVn()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PdVe.
           *
           * @return The value of the simple primitive data type identified by PdVe.
           */
         virtual xs::Double getPdVe() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PdVe.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPdVe(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PdVe exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PdVe is emabled or not
           */
         virtual bool hasPdVe() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PdVe
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enablePdVe(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PdVe */
         virtual void clearPdVe()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PdVd.
           *
           * @return The value of the simple primitive data type identified by PdVd.
           */
         virtual xs::Double getPdVd() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PdVd.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPdVd(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PdVd exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PdVd is emabled or not
           */
         virtual bool hasPdVd() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PdVd
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enablePdVd(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PdVd */
         virtual void clearPdVd()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PositionVelocityCovarianceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PositionVelocityCovarianceType to copy from
           */
         PositionVelocityCovarianceType(const PositionVelocityCovarianceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PositionVelocityCovarianceType to the contents of the
           * PositionVelocityCovarianceType on the right hand side (rhs) of the assignment operator.PositionVelocityCovarianceType
           * [only available to derived classes]
           *
           * @param rhs The PositionVelocityCovarianceType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::PositionVelocityCovarianceType
           * @return A constant reference to this PositionVelocityCovarianceType.
           */
         const PositionVelocityCovarianceType& operator=(const PositionVelocityCovarianceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PositionVelocityCovarianceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PositionVelocityCovarianceType_h

