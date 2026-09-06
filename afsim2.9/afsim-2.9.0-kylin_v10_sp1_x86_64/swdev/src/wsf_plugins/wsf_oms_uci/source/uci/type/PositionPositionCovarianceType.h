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
#ifndef Uci__Type__PositionPositionCovarianceType_h
#define Uci__Type__PositionPositionCovarianceType_h 1

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

      /** This is the PositionPositionCovarianceType sequence accessor class */
      class PositionPositionCovarianceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PositionPositionCovarianceType()
         { }

         /** Returns this accessor's type constant, i.e. PositionPositionCovarianceType
           *
           * @return This accessor's type constant, i.e. PositionPositionCovarianceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::positionPositionCovarianceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PositionPositionCovarianceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PnPn.
           *
           * @return The value of the simple primitive data type identified by PnPn.
           */
         virtual xs::Double getPnPn() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PnPn.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPnPn(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PnPe.
           *
           * @return The value of the simple primitive data type identified by PnPe.
           */
         virtual xs::Double getPnPe() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PnPe.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPnPe(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PnPd.
           *
           * @return The value of the simple primitive data type identified by PnPd.
           */
         virtual xs::Double getPnPd() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PnPd.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPnPd(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PnPd exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PnPd is emabled or not
           */
         virtual bool hasPnPd() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PnPd
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enablePnPd(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PnPd */
         virtual void clearPnPd()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PePe.
           *
           * @return The value of the simple primitive data type identified by PePe.
           */
         virtual xs::Double getPePe() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PePe.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPePe(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PePd.
           *
           * @return The value of the simple primitive data type identified by PePd.
           */
         virtual xs::Double getPePd() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PePd.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPePd(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PePd exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PePd is emabled or not
           */
         virtual bool hasPePd() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PePd
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enablePePd(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PePd */
         virtual void clearPePd()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PdPd.
           *
           * @return The value of the simple primitive data type identified by PdPd.
           */
         virtual xs::Double getPdPd() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PdPd.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPdPd(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PdPd exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PdPd is emabled or not
           */
         virtual bool hasPdPd() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PdPd
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enablePdPd(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PdPd */
         virtual void clearPdPd()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PositionPositionCovarianceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PositionPositionCovarianceType to copy from
           */
         PositionPositionCovarianceType(const PositionPositionCovarianceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PositionPositionCovarianceType to the contents of the
           * PositionPositionCovarianceType on the right hand side (rhs) of the assignment operator.PositionPositionCovarianceType
           * [only available to derived classes]
           *
           * @param rhs The PositionPositionCovarianceType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::PositionPositionCovarianceType
           * @return A constant reference to this PositionPositionCovarianceType.
           */
         const PositionPositionCovarianceType& operator=(const PositionPositionCovarianceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PositionPositionCovarianceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PositionPositionCovarianceType_h

