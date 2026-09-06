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
#ifndef Uci__Type__RGB_ColorControlType_h
#define Uci__Type__RGB_ColorControlType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RGB_ColorControlType sequence accessor class */
      class RGB_ColorControlType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RGB_ColorControlType()
         { }

         /** Returns this accessor's type constant, i.e. RGB_ColorControlType
           *
           * @return This accessor's type constant, i.e. RGB_ColorControlType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rGB_ColorControlType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RGB_ColorControlType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Red.
           *
           * @return The value of the simple primitive data type identified by Red.
           */
         virtual uci::type::PercentTypeValue getRed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Red.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRed(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Green.
           *
           * @return The value of the simple primitive data type identified by Green.
           */
         virtual uci::type::PercentTypeValue getGreen() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Green.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGreen(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Blue.
           *
           * @return The value of the simple primitive data type identified by Blue.
           */
         virtual uci::type::PercentTypeValue getBlue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Blue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBlue(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RGB_ColorControlType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RGB_ColorControlType to copy from
           */
         RGB_ColorControlType(const RGB_ColorControlType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RGB_ColorControlType to the contents of the RGB_ColorControlType
           * on the right hand side (rhs) of the assignment operator.RGB_ColorControlType [only available to derived classes]
           *
           * @param rhs The RGB_ColorControlType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::RGB_ColorControlType
           * @return A constant reference to this RGB_ColorControlType.
           */
         const RGB_ColorControlType& operator=(const RGB_ColorControlType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RGB_ColorControlType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RGB_ColorControlType_h

