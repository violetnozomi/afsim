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
#ifndef Uci__Type__PixelOutputDataType_h
#define Uci__Type__PixelOutputDataType_h 1

#if !defined(Uci__Type__PixelDataType_h)
# include "uci/type/PixelDataType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__DMPI_ID_Type_h)
# include "uci/type/DMPI_ID_Type.h"
#endif

#if !defined(Uci__Type__OpPointID_Type_h)
# include "uci/type/OpPointID_Type.h"
#endif

#if !defined(Uci__Type__Point2D_Type_h)
# include "uci/type/Point2D_Type.h"
#endif

#if !defined(Uci__Type__UncertaintyType_h)
# include "uci/type/UncertaintyType.h"
#endif

#if !defined(Uci__Type__PositionPositionCovarianceType_h)
# include "uci/type/PositionPositionCovarianceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PixelOutputDataType sequence accessor class */
      class PixelOutputDataType : public virtual uci::type::PixelDataType {
      public:

         /** The destructor */
         virtual ~PixelOutputDataType()
         { }

         /** Returns this accessor's type constant, i.e. PixelOutputDataType
           *
           * @return This accessor's type constant, i.e. PixelOutputDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pixelOutputDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PixelOutputDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the GeoLocationStatus.
           *
           * @return The value of the simple primitive data type identified by GeoLocationStatus.
           */
         virtual xs::Boolean getGeoLocationStatus() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the GeoLocationStatus.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGeoLocationStatus(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DMPI_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DMPI_ID.
           */
         virtual const uci::type::DMPI_ID_Type& getDMPI_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DMPI_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DMPI_ID.
           */
         virtual uci::type::DMPI_ID_Type& getDMPI_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DMPI_ID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DMPI_ID
           */
         virtual void setDMPI_ID(const uci::type::DMPI_ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DMPI_ID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DMPI_ID is emabled or not
           */
         virtual bool hasDMPI_ID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DMPI_ID
           *
           * @param type = uci::type::accessorType::dMPI_ID_Type This Accessor's accessor type
           */
         virtual void enableDMPI_ID(uci::base::accessorType::AccessorType type = uci::type::accessorType::dMPI_ID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DMPI_ID */
         virtual void clearDMPI_ID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpPointID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpPointID.
           */
         virtual const uci::type::OpPointID_Type& getOpPointID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpPointID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpPointID.
           */
         virtual uci::type::OpPointID_Type& getOpPointID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OpPointID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OpPointID
           */
         virtual void setOpPointID(const uci::type::OpPointID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OpPointID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OpPointID is emabled or not
           */
         virtual bool hasOpPointID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OpPointID
           *
           * @param type = uci::type::accessorType::opPointID_Type This Accessor's accessor type
           */
         virtual void enableOpPointID(uci::base::accessorType::AccessorType type = uci::type::accessorType::opPointID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OpPointID */
         virtual void clearOpPointID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelGeoLocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelGeoLocation.
           */
         virtual const uci::type::Point2D_Type& getPixelGeoLocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelGeoLocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelGeoLocation.
           */
         virtual uci::type::Point2D_Type& getPixelGeoLocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PixelGeoLocation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PixelGeoLocation
           */
         virtual void setPixelGeoLocation(const uci::type::Point2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PixelGeoLocation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PixelGeoLocation is emabled or not
           */
         virtual bool hasPixelGeoLocation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PixelGeoLocation
           *
           * @param type = uci::type::accessorType::point2D_Type This Accessor's accessor type
           */
         virtual void enablePixelGeoLocation(uci::base::accessorType::AccessorType type = uci::type::accessorType::point2D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PixelGeoLocation */
         virtual void clearPixelGeoLocation()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LocationUncertainty.
           *
           * @return The acecssor that provides access to the complex content that is identified by LocationUncertainty.
           */
         virtual const uci::type::UncertaintyType& getLocationUncertainty() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LocationUncertainty.
           *
           * @return The acecssor that provides access to the complex content that is identified by LocationUncertainty.
           */
         virtual uci::type::UncertaintyType& getLocationUncertainty()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LocationUncertainty to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LocationUncertainty
           */
         virtual void setLocationUncertainty(const uci::type::UncertaintyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LocationUncertainty exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LocationUncertainty is emabled or not
           */
         virtual bool hasLocationUncertainty() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LocationUncertainty
           *
           * @param type = uci::type::accessorType::uncertaintyType This Accessor's accessor type
           */
         virtual void enableLocationUncertainty(uci::base::accessorType::AccessorType type = uci::type::accessorType::uncertaintyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LocationUncertainty */
         virtual void clearLocationUncertainty()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LocationCovariance.
           *
           * @return The acecssor that provides access to the complex content that is identified by LocationCovariance.
           */
         virtual const uci::type::PositionPositionCovarianceType& getLocationCovariance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LocationCovariance.
           *
           * @return The acecssor that provides access to the complex content that is identified by LocationCovariance.
           */
         virtual uci::type::PositionPositionCovarianceType& getLocationCovariance()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LocationCovariance to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LocationCovariance
           */
         virtual void setLocationCovariance(const uci::type::PositionPositionCovarianceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LocationCovariance exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LocationCovariance is emabled or not
           */
         virtual bool hasLocationCovariance() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LocationCovariance
           *
           * @param type = uci::type::accessorType::positionPositionCovarianceType This Accessor's accessor type
           */
         virtual void enableLocationCovariance(uci::base::accessorType::AccessorType type = uci::type::accessorType::positionPositionCovarianceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LocationCovariance */
         virtual void clearLocationCovariance()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PixelOutputDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PixelOutputDataType to copy from
           */
         PixelOutputDataType(const PixelOutputDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PixelOutputDataType to the contents of the PixelOutputDataType on
           * the right hand side (rhs) of the assignment operator.PixelOutputDataType [only available to derived classes]
           *
           * @param rhs The PixelOutputDataType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::PixelOutputDataType
           * @return A constant reference to this PixelOutputDataType.
           */
         const PixelOutputDataType& operator=(const PixelOutputDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PixelOutputDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PixelOutputDataType_h

