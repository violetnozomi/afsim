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
#ifndef Uci__Type__SAR_DynamicImageParametersType_h
#define Uci__Type__SAR_DynamicImageParametersType_h 1

#if !defined(Uci__Type__SAR_ProcessingParametersType_h)
# include "uci/type/SAR_ProcessingParametersType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__DynamicImageProcessingEnum_h)
# include "uci/type/DynamicImageProcessingEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type extends SAR_ProcessingParametersType to provide processing parameters needed for Dynamic Image products of
        * a SAR product.
        */
      class SAR_DynamicImageParametersType : public virtual uci::type::SAR_ProcessingParametersType {
      public:

         /** The destructor */
         virtual ~SAR_DynamicImageParametersType()
         { }

         /** Returns this accessor's type constant, i.e. SAR_DynamicImageParametersType
           *
           * @return This accessor's type constant, i.e. SAR_DynamicImageParametersType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sAR_DynamicImageParametersType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SAR_DynamicImageParametersType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The number of subimages that the Dynamic Image should be processed into in the final product.; [Minimum inclusive: 1]
           * [Maximum inclusive: 64]
           */
         typedef uci::base::UnsignedIntAccessor NumSubApertures;
         //  The type of primitive data accessed by NumSubApertures
         typedef xs::UnsignedInt NumSubAperturesValue;

         /** How much the output product should be resampled/downsampled.; [Minimum inclusive: 1.0] [Maximum inclusive: 3.0] */
         typedef uci::base::DoubleAccessor ResampleFactor;
         //  The type of primitive data accessed by ResampleFactor
         typedef xs::Double ResampleFactorValue;

         /** Returns the value of the SimplePrimitive data type that is identified by the NumSubApertures.
           *
           * @return The value of the simple primitive data type identified by NumSubApertures.
           */
         virtual uci::type::SAR_DynamicImageParametersType::NumSubAperturesValue getNumSubApertures() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NumSubApertures.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumSubApertures(uci::type::SAR_DynamicImageParametersType::NumSubAperturesValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentAperture.
           *
           * @return The value of the simple primitive data type identified by PercentAperture.
           */
         virtual uci::type::PercentTypeValue getPercentAperture() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentAperture.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentAperture(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ResampleFactor.
           *
           * @return The value of the simple primitive data type identified by ResampleFactor.
           */
         virtual uci::type::SAR_DynamicImageParametersType::ResampleFactorValue getResampleFactor() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ResampleFactor.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setResampleFactor(uci::type::SAR_DynamicImageParametersType::ResampleFactorValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EqualizeResolution.
           *
           * @return The value of the simple primitive data type identified by EqualizeResolution.
           */
         virtual xs::Boolean getEqualizeResolution() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EqualizeResolution.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEqualizeResolution(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DynamicProcessingType.
           *
           * @return The value of the enumeration identified by DynamicProcessingType.
           */
         virtual const uci::type::DynamicImageProcessingEnum& getDynamicProcessingType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DynamicProcessingType.
           *
           * @return The value of the enumeration identified by DynamicProcessingType.
           */
         virtual uci::type::DynamicImageProcessingEnum& getDynamicProcessingType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DynamicProcessingType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDynamicProcessingType(const uci::type::DynamicImageProcessingEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DynamicProcessingType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDynamicProcessingType(uci::type::DynamicImageProcessingEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SAR_DynamicImageParametersType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_DynamicImageParametersType to copy from
           */
         SAR_DynamicImageParametersType(const SAR_DynamicImageParametersType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_DynamicImageParametersType to the contents of the
           * SAR_DynamicImageParametersType on the right hand side (rhs) of the assignment operator.SAR_DynamicImageParametersType
           * [only available to derived classes]
           *
           * @param rhs The SAR_DynamicImageParametersType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::SAR_DynamicImageParametersType
           * @return A constant reference to this SAR_DynamicImageParametersType.
           */
         const SAR_DynamicImageParametersType& operator=(const SAR_DynamicImageParametersType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_DynamicImageParametersType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SAR_DynamicImageParametersType_h

