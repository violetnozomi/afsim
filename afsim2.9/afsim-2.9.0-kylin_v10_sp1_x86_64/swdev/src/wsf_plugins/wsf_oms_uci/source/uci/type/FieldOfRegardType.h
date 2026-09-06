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
#ifndef Uci__Type__FieldOfRegardType_h
#define Uci__Type__FieldOfRegardType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AnglePairType_h)
# include "uci/type/AnglePairType.h"
#endif

#if !defined(Uci__Type__SlantRangeConstraintsType_h)
# include "uci/type/SlantRangeConstraintsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Near Range - the MAX is the advertised range (SPEC), and the MIN is the absolute edge of the capability of the Radar.
        * Taking a SAR between the MIN and MAX indicates degraded quality. FarRange - MIN is the advertised Range (SPEC), and
        * MAX is the absolute edge of capability of the Radar. Taking a SAR between the MIN and MAX indicates degraded quality.
        */
      class FieldOfRegardType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~FieldOfRegardType()
         { }

         /** Returns this accessor's type constant, i.e. FieldOfRegardType
           *
           * @return This accessor's type constant, i.e. FieldOfRegardType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::fieldOfRegardType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FieldOfRegardType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AzimuthExtent.
           *
           * @return The acecssor that provides access to the complex content that is identified by AzimuthExtent.
           */
         virtual const uci::type::AnglePairType& getAzimuthExtent() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AzimuthExtent.
           *
           * @return The acecssor that provides access to the complex content that is identified by AzimuthExtent.
           */
         virtual uci::type::AnglePairType& getAzimuthExtent()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AzimuthExtent to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AzimuthExtent
           */
         virtual void setAzimuthExtent(const uci::type::AnglePairType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElevationExtent.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElevationExtent.
           */
         virtual const uci::type::AnglePairType& getElevationExtent() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElevationExtent.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElevationExtent.
           */
         virtual uci::type::AnglePairType& getElevationExtent()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ElevationExtent to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ElevationExtent
           */
         virtual void setElevationExtent(const uci::type::AnglePairType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ElevationExtent exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ElevationExtent is emabled or not
           */
         virtual bool hasElevationExtent() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ElevationExtent
           *
           * @param type = uci::type::accessorType::anglePairType This Accessor's accessor type
           */
         virtual void enableElevationExtent(uci::base::accessorType::AccessorType type = uci::type::accessorType::anglePairType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ElevationExtent */
         virtual void clearElevationExtent()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NearRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by NearRange.
           */
         virtual const uci::type::SlantRangeConstraintsType& getNearRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NearRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by NearRange.
           */
         virtual uci::type::SlantRangeConstraintsType& getNearRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NearRange to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NearRange
           */
         virtual void setNearRange(const uci::type::SlantRangeConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NearRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NearRange is emabled or not
           */
         virtual bool hasNearRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NearRange
           *
           * @param type = uci::type::accessorType::slantRangeConstraintsType This Accessor's accessor type
           */
         virtual void enableNearRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::slantRangeConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NearRange */
         virtual void clearNearRange()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FarRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by FarRange.
           */
         virtual const uci::type::SlantRangeConstraintsType& getFarRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FarRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by FarRange.
           */
         virtual uci::type::SlantRangeConstraintsType& getFarRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FarRange to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FarRange
           */
         virtual void setFarRange(const uci::type::SlantRangeConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FarRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FarRange is emabled or not
           */
         virtual bool hasFarRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FarRange
           *
           * @param type = uci::type::accessorType::slantRangeConstraintsType This Accessor's accessor type
           */
         virtual void enableFarRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::slantRangeConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FarRange */
         virtual void clearFarRange()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FieldOfRegardType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FieldOfRegardType to copy from
           */
         FieldOfRegardType(const FieldOfRegardType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FieldOfRegardType to the contents of the FieldOfRegardType on the
           * right hand side (rhs) of the assignment operator.FieldOfRegardType [only available to derived classes]
           *
           * @param rhs The FieldOfRegardType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::FieldOfRegardType
           * @return A constant reference to this FieldOfRegardType.
           */
         const FieldOfRegardType& operator=(const FieldOfRegardType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FieldOfRegardType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FieldOfRegardType_h

