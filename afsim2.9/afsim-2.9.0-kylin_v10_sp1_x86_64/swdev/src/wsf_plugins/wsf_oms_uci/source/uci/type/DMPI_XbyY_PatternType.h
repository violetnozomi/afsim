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
#ifndef Uci__Type__DMPI_XbyY_PatternType_h
#define Uci__Type__DMPI_XbyY_PatternType_h 1

#if !defined(Uci__Type__DMPI_PatternType_h)
# include "uci/type/DMPI_PatternType.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DMPI_XbyY_PatternType sequence accessor class */
      class DMPI_XbyY_PatternType : public virtual uci::type::DMPI_PatternType {
      public:

         /** The destructor */
         virtual ~DMPI_XbyY_PatternType()
         { }

         /** Returns this accessor's type constant, i.e. DMPI_XbyY_PatternType
           *
           * @return This accessor's type constant, i.e. DMPI_XbyY_PatternType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dMPI_XbyY_PatternType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DMPI_XbyY_PatternType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the X_Spacing.
           *
           * @return The value of the simple primitive data type identified by X_Spacing.
           */
         virtual uci::type::DistanceTypeValue getX_Spacing() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the X_Spacing.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setX_Spacing(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the XNumber.
           *
           * @return The value of the simple primitive data type identified by XNumber.
           */
         virtual xs::UnsignedInt getXNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the XNumber.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setXNumber(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Y_Spacing.
           *
           * @return The value of the simple primitive data type identified by Y_Spacing.
           */
         virtual uci::type::DistanceTypeValue getY_Spacing() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Y_Spacing.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setY_Spacing(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the YNumber.
           *
           * @return The value of the simple primitive data type identified by YNumber.
           */
         virtual xs::UnsignedInt getYNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the YNumber.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setYNumber(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DMPI_XbyY_PatternType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DMPI_XbyY_PatternType to copy from
           */
         DMPI_XbyY_PatternType(const DMPI_XbyY_PatternType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DMPI_XbyY_PatternType to the contents of the DMPI_XbyY_PatternType
           * on the right hand side (rhs) of the assignment operator.DMPI_XbyY_PatternType [only available to derived classes]
           *
           * @param rhs The DMPI_XbyY_PatternType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::DMPI_XbyY_PatternType
           * @return A constant reference to this DMPI_XbyY_PatternType.
           */
         const DMPI_XbyY_PatternType& operator=(const DMPI_XbyY_PatternType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DMPI_XbyY_PatternType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DMPI_XbyY_PatternType_h

