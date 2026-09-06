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
#ifndef Uci__Type__VehicleIdentificationType_h
#define Uci__Type__VehicleIdentificationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__TailNumberType_h)
# include "uci/type/TailNumberType.h"
#endif

#if !defined(Uci__Type__IFF_Type_h)
# include "uci/type/IFF_Type.h"
#endif

#if !defined(Uci__Type__AIS_Type_h)
# include "uci/type/AIS_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CallSignType_h)
# include "uci/type/CallSignType.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Type__DataLinkIdentifierPET_h)
# include "uci/type/DataLinkIdentifierPET.h"
#endif

#if !defined(Uci__Type__SatelliteIdentifierType_h)
# include "uci/type/SatelliteIdentifierType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the VehicleIdentificationType sequence accessor class */
      class VehicleIdentificationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~VehicleIdentificationType()
         { }

         /** Returns this accessor's type constant, i.e. VehicleIdentificationType
           *
           * @return This accessor's type constant, i.e. VehicleIdentificationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::vehicleIdentificationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const VehicleIdentificationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a call sign for the asset. Multiple call signs are allowed for different communications systems, registries
           * or other reasons. The use of ForeignKeyType is an indication that call signs aren't universally defined and come from
           * multiple systems/sources but they are expected to be unique within the context of their source. The child Key element
           * indicates the call sign itself while the child SystemName element indicates the system that assigned it and ensures
           * it is unique. Call signs from ubiquitous systems/sources such as AIS are given in sibling elements and shouldn't be
           * repeated here. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CallSignType, uci::type::accessorType::callSignType> CallSign;

         /** The specific Radar Cross Section identification for this system. This will typically be a system-specific and
           * service-specific identifier for pre-configured Radar Cross Sectional data. Services should have coordinated this data
           * ahead of time. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> RadarCrossSection;

         /** List of datalink ID. Multiple datalink IDs can be reported for the same network type. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DataLinkIdentifierPET, uci::type::accessorType::dataLinkIdentifierPET> DataLinkID;

         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SystemID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SystemID is emabled or not
           */
         virtual bool hasSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SystemID
           *
           * @param type = uci::type::accessorType::systemID_Type This Accessor's accessor type
           */
         virtual void enableSystemID(uci::base::accessorType::AccessorType type = uci::type::accessorType::systemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SystemID */
         virtual void clearSystemID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TailNumber.
           *
           * @return The value of the string data type identified by TailNumber.
           */
         virtual const uci::type::TailNumberType& getTailNumber() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TailNumber.
           *
           * @return The value of the string data type identified by TailNumber.
           */
         virtual uci::type::TailNumberType& getTailNumber()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TailNumber to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setTailNumber(const uci::type::TailNumberType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TailNumber to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTailNumber(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TailNumber to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTailNumber(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TailNumber exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TailNumber is emabled or not
           */
         virtual bool hasTailNumber() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TailNumber
           *
           * @param type = uci::type::accessorType::tailNumberType This Accessor's accessor type
           */
         virtual void enableTailNumber(uci::base::accessorType::AccessorType type = uci::type::accessorType::tailNumberType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TailNumber */
         virtual void clearTailNumber()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the IFF.
           *
           * @return The acecssor that provides access to the complex content that is identified by IFF.
           */
         virtual const uci::type::IFF_Type& getIFF() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the IFF.
           *
           * @return The acecssor that provides access to the complex content that is identified by IFF.
           */
         virtual uci::type::IFF_Type& getIFF()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the IFF to the contents of the complex content that is accessed by the
           * specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by IFF
           */
         virtual void setIFF(const uci::type::IFF_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IFF exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IFF is emabled or not
           */
         virtual bool hasIFF() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IFF
           *
           * @param type = uci::type::accessorType::iFF_Type This Accessor's accessor type
           */
         virtual void enableIFF(uci::base::accessorType::AccessorType type = uci::type::accessorType::iFF_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IFF */
         virtual void clearIFF()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AIS.
           *
           * @return The acecssor that provides access to the complex content that is identified by AIS.
           */
         virtual const uci::type::AIS_Type& getAIS() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AIS.
           *
           * @return The acecssor that provides access to the complex content that is identified by AIS.
           */
         virtual uci::type::AIS_Type& getAIS()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AIS to the contents of the complex content that is accessed by the
           * specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AIS
           */
         virtual void setAIS(const uci::type::AIS_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AIS exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AIS is emabled or not
           */
         virtual bool hasAIS() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AIS
           *
           * @param type = uci::type::accessorType::aIS_Type This Accessor's accessor type
           */
         virtual void enableAIS(uci::base::accessorType::AccessorType type = uci::type::accessorType::aIS_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AIS */
         virtual void clearAIS()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CallSign.
           *
           * @return The bounded list identified by CallSign.
           */
         virtual const uci::type::VehicleIdentificationType::CallSign& getCallSign() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CallSign.
           *
           * @return The bounded list identified by CallSign.
           */
         virtual uci::type::VehicleIdentificationType::CallSign& getCallSign()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CallSign.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCallSign(const uci::type::VehicleIdentificationType::CallSign& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RadarCrossSection.
           *
           * @return The bounded list identified by RadarCrossSection.
           */
         virtual const uci::type::VehicleIdentificationType::RadarCrossSection& getRadarCrossSection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RadarCrossSection.
           *
           * @return The bounded list identified by RadarCrossSection.
           */
         virtual uci::type::VehicleIdentificationType::RadarCrossSection& getRadarCrossSection()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RadarCrossSection.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRadarCrossSection(const uci::type::VehicleIdentificationType::RadarCrossSection& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataLinkID.
           *
           * @return The bounded list identified by DataLinkID.
           */
         virtual const uci::type::VehicleIdentificationType::DataLinkID& getDataLinkID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataLinkID.
           *
           * @return The bounded list identified by DataLinkID.
           */
         virtual uci::type::VehicleIdentificationType::DataLinkID& getDataLinkID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DataLinkID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDataLinkID(const uci::type::VehicleIdentificationType::DataLinkID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Satellite.
           *
           * @return The acecssor that provides access to the complex content that is identified by Satellite.
           */
         virtual const uci::type::SatelliteIdentifierType& getSatellite() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Satellite.
           *
           * @return The acecssor that provides access to the complex content that is identified by Satellite.
           */
         virtual uci::type::SatelliteIdentifierType& getSatellite()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Satellite to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Satellite
           */
         virtual void setSatellite(const uci::type::SatelliteIdentifierType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Satellite exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Satellite is emabled or not
           */
         virtual bool hasSatellite() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Satellite
           *
           * @param type = uci::type::accessorType::satelliteIdentifierType This Accessor's accessor type
           */
         virtual void enableSatellite(uci::base::accessorType::AccessorType type = uci::type::accessorType::satelliteIdentifierType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Satellite */
         virtual void clearSatellite()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         VehicleIdentificationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The VehicleIdentificationType to copy from
           */
         VehicleIdentificationType(const VehicleIdentificationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this VehicleIdentificationType to the contents of the
           * VehicleIdentificationType on the right hand side (rhs) of the assignment operator.VehicleIdentificationType [only
           * available to derived classes]
           *
           * @param rhs The VehicleIdentificationType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::VehicleIdentificationType
           * @return A constant reference to this VehicleIdentificationType.
           */
         const VehicleIdentificationType& operator=(const VehicleIdentificationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // VehicleIdentificationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__VehicleIdentificationType_h

