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
#ifndef Uci__Type__RTN_VelocityType_h
#define Uci__Type__RTN_VelocityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SpeedType_h)
# include "uci/type/SpeedType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates a velocity vector in the Radial-Transverse-Normal Reference System. */
      class RTN_VelocityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RTN_VelocityType()
         { }

         /** Returns this accessor's type constant, i.e. RTN_VelocityType
           *
           * @return This accessor's type constant, i.e. RTN_VelocityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rTN_VelocityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RTN_VelocityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Radial.
           *
           * @return The value of the simple primitive data type identified by Radial.
           */
         virtual uci::type::SpeedTypeValue getRadial() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Radial.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRadial(uci::type::SpeedTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Transverse.
           *
           * @return The value of the simple primitive data type identified by Transverse.
           */
         virtual uci::type::SpeedTypeValue getTransverse() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Transverse.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTransverse(uci::type::SpeedTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Normal.
           *
           * @return The value of the simple primitive data type identified by Normal.
           */
         virtual uci::type::SpeedTypeValue getNormal() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Normal.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNormal(uci::type::SpeedTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RTN_VelocityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RTN_VelocityType to copy from
           */
         RTN_VelocityType(const RTN_VelocityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RTN_VelocityType to the contents of the RTN_VelocityType on the
           * right hand side (rhs) of the assignment operator.RTN_VelocityType [only available to derived classes]
           *
           * @param rhs The RTN_VelocityType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::RTN_VelocityType
           * @return A constant reference to this RTN_VelocityType.
           */
         const RTN_VelocityType& operator=(const RTN_VelocityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RTN_VelocityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RTN_VelocityType_h

