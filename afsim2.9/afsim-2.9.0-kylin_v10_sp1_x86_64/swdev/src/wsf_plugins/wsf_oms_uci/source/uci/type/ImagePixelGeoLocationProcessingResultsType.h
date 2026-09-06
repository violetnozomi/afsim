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
#ifndef Uci__Type__ImagePixelGeoLocationProcessingResultsType_h
#define Uci__Type__ImagePixelGeoLocationProcessingResultsType_h 1

#if !defined(Uci__Type__ProcessingResultsPET_h)
# include "uci/type/ProcessingResultsPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PixelOutputDataType_h)
# include "uci/type/PixelOutputDataType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base ProcessingResultsPET for data output from
        * a generic pixel geo-location algorithm.
        */
      class ImagePixelGeoLocationProcessingResultsType : public virtual uci::type::ProcessingResultsPET {
      public:

         /** The destructor */
         virtual ~ImagePixelGeoLocationProcessingResultsType()
         { }

         /** Returns this accessor's type constant, i.e. ImagePixelGeoLocationProcessingResultsType
           *
           * @return This accessor's type constant, i.e. ImagePixelGeoLocationProcessingResultsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::imagePixelGeoLocationProcessingResultsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ImagePixelGeoLocationProcessingResultsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** These are the specific pixels from the source image for which the processing algorithms should generate
           * geo-locations. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PixelOutputDataType, uci::type::accessorType::pixelOutputDataType> Pixel;

         /** Returns the bounded list that is identified by the Pixel.
           *
           * @return The bounded list identified by Pixel.
           */
         virtual const uci::type::ImagePixelGeoLocationProcessingResultsType::Pixel& getPixel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Pixel.
           *
           * @return The bounded list identified by Pixel.
           */
         virtual uci::type::ImagePixelGeoLocationProcessingResultsType::Pixel& getPixel()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Pixel.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPixel(const uci::type::ImagePixelGeoLocationProcessingResultsType::Pixel& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ImagePixelGeoLocationProcessingResultsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ImagePixelGeoLocationProcessingResultsType to copy from
           */
         ImagePixelGeoLocationProcessingResultsType(const ImagePixelGeoLocationProcessingResultsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ImagePixelGeoLocationProcessingResultsType to the contents of the
           * ImagePixelGeoLocationProcessingResultsType on the right hand side (rhs) of the assignment
           * operator.ImagePixelGeoLocationProcessingResultsType [only available to derived classes]
           *
           * @param rhs The ImagePixelGeoLocationProcessingResultsType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::ImagePixelGeoLocationProcessingResultsType
           * @return A constant reference to this ImagePixelGeoLocationProcessingResultsType.
           */
         const ImagePixelGeoLocationProcessingResultsType& operator=(const ImagePixelGeoLocationProcessingResultsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ImagePixelGeoLocationProcessingResultsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ImagePixelGeoLocationProcessingResultsType_h

