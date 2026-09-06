// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__ImageGeoRegistrationProcessingResultsType_h
#define Uci__Type__ImageGeoRegistrationProcessingResultsType_h 1

#if !defined(Uci__Type__ProcessingResultsPET_h)
# include "uci/type/ProcessingResultsPET.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base ProcessingResultsPET for data output from
        * a generic image geo-registration algorithm.
        */
      class ImageGeoRegistrationProcessingResultsType : public virtual uci::type::ProcessingResultsPET {
      public:

         /** The destructor */
         virtual ~ImageGeoRegistrationProcessingResultsType()
         { }

         /** Returns this accessor's type constant, i.e. ImageGeoRegistrationProcessingResultsType
           *
           * @return This accessor's type constant, i.e. ImageGeoRegistrationProcessingResultsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::imageGeoRegistrationProcessingResultsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ImageGeoRegistrationProcessingResultsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The relative quality of the geo-registration process. negative = the process completed but the registration accuracy
           * was made worse. 0 = the process completed but the registration accuracy was not improved. positive = the process
           * completed with improved registration accuracy.; [Minimum inclusive: -1] [Maximum inclusive: 1]
           */
         typedef uci::base::DoubleAccessor GeoRegistrationQuality;
         //  The type of primitive data accessed by GeoRegistrationQuality
         typedef xs::Double GeoRegistrationQualityValue;

         /** Returns the value of the SimplePrimitive data type that is identified by the GeoRegistrationQuality.
           *
           * @return The value of the simple primitive data type identified by GeoRegistrationQuality.
           */
         virtual uci::type::ImageGeoRegistrationProcessingResultsType::GeoRegistrationQualityValue getGeoRegistrationQuality() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the GeoRegistrationQuality.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGeoRegistrationQuality(uci::type::ImageGeoRegistrationProcessingResultsType::GeoRegistrationQualityValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RegistrationResidual.
           *
           * @return The value of the simple primitive data type identified by RegistrationResidual.
           */
         virtual xs::Double getRegistrationResidual() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RegistrationResidual.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRegistrationResidual(xs::Double value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ImageGeoRegistrationProcessingResultsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ImageGeoRegistrationProcessingResultsType to copy from
           */
         ImageGeoRegistrationProcessingResultsType(const ImageGeoRegistrationProcessingResultsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ImageGeoRegistrationProcessingResultsType to the contents of the
           * ImageGeoRegistrationProcessingResultsType on the right hand side (rhs) of the assignment
           * operator.ImageGeoRegistrationProcessingResultsType [only available to derived classes]
           *
           * @param rhs The ImageGeoRegistrationProcessingResultsType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::ImageGeoRegistrationProcessingResultsType
           * @return A constant reference to this ImageGeoRegistrationProcessingResultsType.
           */
         const ImageGeoRegistrationProcessingResultsType& operator=(const ImageGeoRegistrationProcessingResultsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ImageGeoRegistrationProcessingResultsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ImageGeoRegistrationProcessingResultsType_h

