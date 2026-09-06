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
#ifndef Uci__Type__ProductResolutionType_h
#define Uci__Type__ProductResolutionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductResolutionType sequence accessor class */
      class ProductResolutionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductResolutionType()
         { }

         /** Returns this accessor's type constant, i.e. ProductResolutionType
           *
           * @return This accessor's type constant, i.e. ProductResolutionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productResolutionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductResolutionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the number of discrete color values in each band, e.g. 8-bit color representation will require a value of
           * 256. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::base::UnsignedIntAccessor, uci::base::accessorType::unsignedIntAccessor> ColorDepth;

         /** Returns the value of the SimplePrimitive data type that is identified by the HorizontalPixelCount.
           *
           * @return The value of the simple primitive data type identified by HorizontalPixelCount.
           */
         virtual xs::UnsignedInt getHorizontalPixelCount() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HorizontalPixelCount.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHorizontalPixelCount(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VerticalPixelCount.
           *
           * @return The value of the simple primitive data type identified by VerticalPixelCount.
           */
         virtual xs::UnsignedInt getVerticalPixelCount() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VerticalPixelCount.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVerticalPixelCount(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ColorDepth.
           *
           * @return The bounded list identified by ColorDepth.
           */
         virtual const uci::type::ProductResolutionType::ColorDepth& getColorDepth() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ColorDepth.
           *
           * @return The bounded list identified by ColorDepth.
           */
         virtual uci::type::ProductResolutionType::ColorDepth& getColorDepth()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ColorDepth.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setColorDepth(const uci::type::ProductResolutionType::ColorDepth& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductResolutionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductResolutionType to copy from
           */
         ProductResolutionType(const ProductResolutionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductResolutionType to the contents of the ProductResolutionType
           * on the right hand side (rhs) of the assignment operator.ProductResolutionType [only available to derived classes]
           *
           * @param rhs The ProductResolutionType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ProductResolutionType
           * @return A constant reference to this ProductResolutionType.
           */
         const ProductResolutionType& operator=(const ProductResolutionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductResolutionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductResolutionType_h

