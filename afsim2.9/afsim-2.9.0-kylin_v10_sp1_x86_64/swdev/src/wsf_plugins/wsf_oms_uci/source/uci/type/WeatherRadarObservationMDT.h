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
#ifndef Uci__Type__WeatherRadarObservationMDT_h
#define Uci__Type__WeatherRadarObservationMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__Point3D_Type_h)
# include "uci/type/Point3D_Type.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__WeatherRadarCellSizeType_h)
# include "uci/type/WeatherRadarCellSizeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__WeatherRadarCellType_h)
# include "uci/type/WeatherRadarCellType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the WeatherRadarObservationMDT sequence accessor class */
      class WeatherRadarObservationMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~WeatherRadarObservationMDT()
         { }

         /** Returns this accessor's type constant, i.e. WeatherRadarObservationMDT
           *
           * @return This accessor's type constant, i.e. WeatherRadarObservationMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::weatherRadarObservationMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const WeatherRadarObservationMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Represents the data for each weather observation cell observed by the weather radar. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::WeatherRadarCellType, uci::type::accessorType::weatherRadarCellType> Cell;

         /** Returns the accessor that provides access to the complex content that is identified by the ObservingSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ObservingSystemID.
           */
         virtual const uci::type::SystemID_Type& getObservingSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ObservingSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ObservingSystemID.
           */
         virtual uci::type::SystemID_Type& getObservingSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ObservingSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ObservingSystemID
           */
         virtual void setObservingSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by Location.
           */
         virtual const uci::type::Point3D_Type& getLocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by Location.
           */
         virtual uci::type::Point3D_Type& getLocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Location to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Location
           */
         virtual void setLocation(const uci::type::Point3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Timestamp.
           *
           * @return The value of the simple primitive data type identified by Timestamp.
           */
         virtual uci::type::DateTimeTypeValue getTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Timestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual const uci::type::CapabilityID_Type& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual uci::type::CapabilityID_Type& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityID
           */
         virtual void setCapabilityID(const uci::type::CapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CapabilityID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CapabilityID is emabled or not
           */
         virtual bool hasCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CapabilityID
           *
           * @param type = uci::type::accessorType::capabilityID_Type This Accessor's accessor type
           */
         virtual void enableCapabilityID(uci::base::accessorType::AccessorType type = uci::type::accessorType::capabilityID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CapabilityID */
         virtual void clearCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CellSize.
           *
           * @return The acecssor that provides access to the complex content that is identified by CellSize.
           */
         virtual const uci::type::WeatherRadarCellSizeType& getCellSize() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CellSize.
           *
           * @return The acecssor that provides access to the complex content that is identified by CellSize.
           */
         virtual uci::type::WeatherRadarCellSizeType& getCellSize()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CellSize to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CellSize
           */
         virtual void setCellSize(const uci::type::WeatherRadarCellSizeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Cell.
           *
           * @return The bounded list identified by Cell.
           */
         virtual const uci::type::WeatherRadarObservationMDT::Cell& getCell() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Cell.
           *
           * @return The bounded list identified by Cell.
           */
         virtual uci::type::WeatherRadarObservationMDT::Cell& getCell()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Cell.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCell(const uci::type::WeatherRadarObservationMDT::Cell& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         WeatherRadarObservationMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The WeatherRadarObservationMDT to copy from
           */
         WeatherRadarObservationMDT(const WeatherRadarObservationMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this WeatherRadarObservationMDT to the contents of the
           * WeatherRadarObservationMDT on the right hand side (rhs) of the assignment operator.WeatherRadarObservationMDT [only
           * available to derived classes]
           *
           * @param rhs The WeatherRadarObservationMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::WeatherRadarObservationMDT
           * @return A constant reference to this WeatherRadarObservationMDT.
           */
         const WeatherRadarObservationMDT& operator=(const WeatherRadarObservationMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // WeatherRadarObservationMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__WeatherRadarObservationMDT_h

