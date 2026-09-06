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
#ifndef Uci__Type__DMPI_RadialPatternType_h
#define Uci__Type__DMPI_RadialPatternType_h 1

#if !defined(Uci__Type__DMPI_PatternType_h)
# include "uci/type/DMPI_PatternType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DMPI_RadialPatternType sequence accessor class */
      class DMPI_RadialPatternType : public virtual uci::type::DMPI_PatternType {
      public:

         /** The destructor */
         virtual ~DMPI_RadialPatternType()
         { }

         /** Returns this accessor's type constant, i.e. DMPI_RadialPatternType
           *
           * @return This accessor's type constant, i.e. DMPI_RadialPatternType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dMPI_RadialPatternType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DMPI_RadialPatternType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NumberOfRadials.
           *
           * @return The value of the simple primitive data type identified by NumberOfRadials.
           */
         virtual xs::UnsignedInt getNumberOfRadials() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NumberOfRadials.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumberOfRadials(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RadialSpacing.
           *
           * @return The value of the simple primitive data type identified by RadialSpacing.
           */
         virtual uci::type::DistanceTypeValue getRadialSpacing() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RadialSpacing.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRadialSpacing(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DMPIsPerRadial.
           *
           * @return The value of the simple primitive data type identified by DMPIsPerRadial.
           */
         virtual xs::UnsignedInt getDMPIsPerRadial() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DMPIsPerRadial.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDMPIsPerRadial(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DMPI_RadialPatternType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DMPI_RadialPatternType to copy from
           */
         DMPI_RadialPatternType(const DMPI_RadialPatternType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DMPI_RadialPatternType to the contents of the
           * DMPI_RadialPatternType on the right hand side (rhs) of the assignment operator.DMPI_RadialPatternType [only available
           * to derived classes]
           *
           * @param rhs The DMPI_RadialPatternType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::DMPI_RadialPatternType
           * @return A constant reference to this DMPI_RadialPatternType.
           */
         const DMPI_RadialPatternType& operator=(const DMPI_RadialPatternType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DMPI_RadialPatternType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DMPI_RadialPatternType_h

