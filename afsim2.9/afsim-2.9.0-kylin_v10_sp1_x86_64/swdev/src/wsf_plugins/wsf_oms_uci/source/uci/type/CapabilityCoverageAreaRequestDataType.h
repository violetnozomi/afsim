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
#ifndef Uci__Type__CapabilityCoverageAreaRequestDataType_h
#define Uci__Type__CapabilityCoverageAreaRequestDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityCoverageAreaFactorEnum_h)
# include "uci/type/CapabilityCoverageAreaFactorEnum.h"
#endif

#if !defined(Uci__Type__Point3D_Type_h)
# include "uci/type/Point3D_Type.h"
#endif

#if !defined(Uci__Type__OrientationType_h)
# include "uci/type/OrientationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Contains the detailed capability coverage area resultant data. */
      class CapabilityCoverageAreaRequestDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CapabilityCoverageAreaRequestDataType()
         { }

         /** Returns this accessor's type constant, i.e. CapabilityCoverageAreaRequestDataType
           *
           * @return This accessor's type constant, i.e. CapabilityCoverageAreaRequestDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::capabilityCoverageAreaRequestDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CapabilityCoverageAreaRequestDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the sensor Capability instance that this CCA is associated with. Multiple instances of this element should
           * only be given when each Capability is associated with the same subsystem and multiple instances are being employed to
           * complete the sensor capability [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Indicates limiting factors used in computing the Capability Coverage Area. [Minimum occurrences: 0] [Maximum
           * occurrences: 5]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityCoverageAreaFactorEnum, uci::type::accessorType::capabilityCoverageAreaFactorEnum> CapabilityCoverageAreaFactor;

         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::CapabilityCoverageAreaRequestDataType::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::CapabilityCoverageAreaRequestDataType::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::CapabilityCoverageAreaRequestDataType::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityCoverageAreaFactor.
           *
           * @return The bounded list identified by CapabilityCoverageAreaFactor.
           */
         virtual const uci::type::CapabilityCoverageAreaRequestDataType::CapabilityCoverageAreaFactor& getCapabilityCoverageAreaFactor() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityCoverageAreaFactor.
           *
           * @return The bounded list identified by CapabilityCoverageAreaFactor.
           */
         virtual uci::type::CapabilityCoverageAreaRequestDataType::CapabilityCoverageAreaFactor& getCapabilityCoverageAreaFactor()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityCoverageAreaFactor.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityCoverageAreaFactor(const uci::type::CapabilityCoverageAreaRequestDataType::CapabilityCoverageAreaFactor& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SensorPosition.
           *
           * @return The acecssor that provides access to the complex content that is identified by SensorPosition.
           */
         virtual const uci::type::Point3D_Type& getSensorPosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SensorPosition.
           *
           * @return The acecssor that provides access to the complex content that is identified by SensorPosition.
           */
         virtual uci::type::Point3D_Type& getSensorPosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SensorPosition to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SensorPosition
           */
         virtual void setSensorPosition(const uci::type::Point3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SensorPosition exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SensorPosition is emabled or not
           */
         virtual bool hasSensorPosition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SensorPosition
           *
           * @param type = uci::type::accessorType::point3D_Type This Accessor's accessor type
           */
         virtual void enableSensorPosition(uci::base::accessorType::AccessorType type = uci::type::accessorType::point3D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SensorPosition */
         virtual void clearSensorPosition()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SensorOrientation.
           *
           * @return The acecssor that provides access to the complex content that is identified by SensorOrientation.
           */
         virtual const uci::type::OrientationType& getSensorOrientation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SensorOrientation.
           *
           * @return The acecssor that provides access to the complex content that is identified by SensorOrientation.
           */
         virtual uci::type::OrientationType& getSensorOrientation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SensorOrientation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SensorOrientation
           */
         virtual void setSensorOrientation(const uci::type::OrientationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SensorOrientation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SensorOrientation is emabled or not
           */
         virtual bool hasSensorOrientation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SensorOrientation
           *
           * @param type = uci::type::accessorType::orientationType This Accessor's accessor type
           */
         virtual void enableSensorOrientation(uci::base::accessorType::AccessorType type = uci::type::accessorType::orientationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SensorOrientation */
         virtual void clearSensorOrientation()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityCoverageAreaRequestDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityCoverageAreaRequestDataType to copy from
           */
         CapabilityCoverageAreaRequestDataType(const CapabilityCoverageAreaRequestDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityCoverageAreaRequestDataType to the contents of the
           * CapabilityCoverageAreaRequestDataType on the right hand side (rhs) of the assignment
           * operator.CapabilityCoverageAreaRequestDataType [only available to derived classes]
           *
           * @param rhs The CapabilityCoverageAreaRequestDataType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::CapabilityCoverageAreaRequestDataType
           * @return A constant reference to this CapabilityCoverageAreaRequestDataType.
           */
         const CapabilityCoverageAreaRequestDataType& operator=(const CapabilityCoverageAreaRequestDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityCoverageAreaRequestDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CapabilityCoverageAreaRequestDataType_h

