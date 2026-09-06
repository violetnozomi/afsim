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
#ifndef Uci__Type__GeoLocatedObjectType_h
#define Uci__Type__GeoLocatedObjectType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__OpPointID_Type_h)
# include "uci/type/OpPointID_Type.h"
#endif

#if !defined(Uci__Type__OpLineID_Type_h)
# include "uci/type/OpLineID_Type.h"
#endif

#if !defined(Uci__Type__OpZoneID_Type_h)
# include "uci/type/OpZoneID_Type.h"
#endif

#if !defined(Uci__Type__SignalReportID_Type_h)
# include "uci/type/SignalReportID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Container to reference the appropriate geo-located object. */
      class GeoLocatedObjectType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~GeoLocatedObjectType()
         { }

         /** Returns this accessor's type constant, i.e. GeoLocatedObjectType
           *
           * @return This accessor's type constant, i.e. GeoLocatedObjectType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::geoLocatedObjectType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GeoLocatedObjectType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum GeoLocatedObjectTypeChoice {
            GEOLOCATEDOBJECTTYPE_CHOICE_NONE,
            GEOLOCATEDOBJECTTYPE_CHOICE_ENTITYID,
            GEOLOCATEDOBJECTTYPE_CHOICE_SYSTEMID,
            GEOLOCATEDOBJECTTYPE_CHOICE_OPPOINTID,
            GEOLOCATEDOBJECTTYPE_CHOICE_OPLINEID,
            GEOLOCATEDOBJECTTYPE_CHOICE_OPZONEID,
            GEOLOCATEDOBJECTTYPE_CHOICE_SIGNALREPORTID
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual GeoLocatedObjectTypeChoice getGeoLocatedObjectTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getGeoLocatedObjectTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the
           * access methods associated with the chosen element will be enabled and will provide access to the chosen element.
           *
           * @param chosenElementOrdinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setGeoLocatedObjectTypeChoiceOrdinal(GeoLocatedObjectTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual const uci::type::EntityID_Type& getEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual uci::type::EntityID_Type& getEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EntityID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EntityID
           */
         virtual void setEntityID(const uci::type::EntityID_Type& value)
            throw(uci::base::UCIException) = 0;


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


         /** Returns the accessor that provides access to the complex content that is identified by the OpLineID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpLineID.
           */
         virtual const uci::type::OpLineID_Type& getOpLineID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpLineID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpLineID.
           */
         virtual uci::type::OpLineID_Type& getOpLineID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OpLineID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OpLineID
           */
         virtual void setOpLineID(const uci::type::OpLineID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpZoneID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpZoneID.
           */
         virtual const uci::type::OpZoneID_Type& getOpZoneID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpZoneID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpZoneID.
           */
         virtual uci::type::OpZoneID_Type& getOpZoneID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OpZoneID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OpZoneID
           */
         virtual void setOpZoneID(const uci::type::OpZoneID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalReportID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalReportID.
           */
         virtual const uci::type::SignalReportID_Type& getSignalReportID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalReportID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalReportID.
           */
         virtual uci::type::SignalReportID_Type& getSignalReportID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SignalReportID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SignalReportID
           */
         virtual void setSignalReportID(const uci::type::SignalReportID_Type& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         GeoLocatedObjectType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GeoLocatedObjectType to copy from
           */
         GeoLocatedObjectType(const GeoLocatedObjectType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GeoLocatedObjectType to the contents of the GeoLocatedObjectType
           * on the right hand side (rhs) of the assignment operator.GeoLocatedObjectType [only available to derived classes]
           *
           * @param rhs The GeoLocatedObjectType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::GeoLocatedObjectType
           * @return A constant reference to this GeoLocatedObjectType.
           */
         const GeoLocatedObjectType& operator=(const GeoLocatedObjectType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GeoLocatedObjectType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GeoLocatedObjectType_h

