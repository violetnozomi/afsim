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
#ifndef Uci__Type__DMPI_TraceabilityType_h
#define Uci__Type__DMPI_TraceabilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DMPI_ID_Type_h)
# include "uci/type/DMPI_ID_Type.h"
#endif

#if !defined(Uci__Type__BasicEncyclopediaNumberType_h)
# include "uci/type/BasicEncyclopediaNumberType.h"
#endif

#if !defined(Uci__Type__GeoLocatedObjectType_h)
# include "uci/type/GeoLocatedObjectType.h"
#endif

#if !defined(Uci__Type__ATO_TraceabilityType_h)
# include "uci/type/ATO_TraceabilityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DMPI_TraceabilityType sequence accessor class */
      class DMPI_TraceabilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DMPI_TraceabilityType()
         { }

         /** Returns this accessor's type constant, i.e. DMPI_TraceabilityType
           *
           * @return This accessor's type constant, i.e. DMPI_TraceabilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dMPI_TraceabilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DMPI_TraceabilityType& accessor)
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


         /** Returns the accessor that provides access to the complex content that is identified by the BE_Number.
           *
           * @return The acecssor that provides access to the complex content that is identified by BE_Number.
           */
         virtual const uci::type::BasicEncyclopediaNumberType& getBE_Number() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BE_Number.
           *
           * @return The acecssor that provides access to the complex content that is identified by BE_Number.
           */
         virtual uci::type::BasicEncyclopediaNumberType& getBE_Number()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the BE_Number to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by BE_Number
           */
         virtual void setBE_Number(const uci::type::BasicEncyclopediaNumberType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BE_Number exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BE_Number is emabled or not
           */
         virtual bool hasBE_Number() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BE_Number
           *
           * @param type = uci::type::accessorType::basicEncyclopediaNumberType This Accessor's accessor type
           */
         virtual void enableBE_Number(uci::base::accessorType::AccessorType type = uci::type::accessorType::basicEncyclopediaNumberType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BE_Number */
         virtual void clearBE_Number()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TraceObject.
           *
           * @return The acecssor that provides access to the complex content that is identified by TraceObject.
           */
         virtual const uci::type::GeoLocatedObjectType& getTraceObject() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TraceObject.
           *
           * @return The acecssor that provides access to the complex content that is identified by TraceObject.
           */
         virtual uci::type::GeoLocatedObjectType& getTraceObject()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TraceObject to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TraceObject
           */
         virtual void setTraceObject(const uci::type::GeoLocatedObjectType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TraceObject exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TraceObject is emabled or not
           */
         virtual bool hasTraceObject() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TraceObject
           *
           * @param type = uci::type::accessorType::geoLocatedObjectType This Accessor's accessor type
           */
         virtual void enableTraceObject(uci::base::accessorType::AccessorType type = uci::type::accessorType::geoLocatedObjectType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TraceObject */
         virtual void clearTraceObject()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ATO_Traceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by ATO_Traceability.
           */
         virtual const uci::type::ATO_TraceabilityType& getATO_Traceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ATO_Traceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by ATO_Traceability.
           */
         virtual uci::type::ATO_TraceabilityType& getATO_Traceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ATO_Traceability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ATO_Traceability
           */
         virtual void setATO_Traceability(const uci::type::ATO_TraceabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ATO_Traceability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ATO_Traceability is emabled or not
           */
         virtual bool hasATO_Traceability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ATO_Traceability
           *
           * @param type = uci::type::accessorType::aTO_TraceabilityType This Accessor's accessor type
           */
         virtual void enableATO_Traceability(uci::base::accessorType::AccessorType type = uci::type::accessorType::aTO_TraceabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ATO_Traceability */
         virtual void clearATO_Traceability()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DMPI_TraceabilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DMPI_TraceabilityType to copy from
           */
         DMPI_TraceabilityType(const DMPI_TraceabilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DMPI_TraceabilityType to the contents of the DMPI_TraceabilityType
           * on the right hand side (rhs) of the assignment operator.DMPI_TraceabilityType [only available to derived classes]
           *
           * @param rhs The DMPI_TraceabilityType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::DMPI_TraceabilityType
           * @return A constant reference to this DMPI_TraceabilityType.
           */
         const DMPI_TraceabilityType& operator=(const DMPI_TraceabilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DMPI_TraceabilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DMPI_TraceabilityType_h

