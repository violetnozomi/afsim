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
#ifndef Uci__Type__LOS_InertialB_Type_h
#define Uci__Type__LOS_InertialB_Type_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AnglePositiveType_h)
# include "uci/type/AnglePositiveType.h"
#endif

#if !defined(Uci__Type__AngleType_h)
# include "uci/type/AngleType.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Defines a line of sight ray in a world inertial frame of reference. */
      class LOS_InertialB_Type : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~LOS_InertialB_Type()
         { }

         /** Returns this accessor's type constant, i.e. LOS_InertialB_Type
           *
           * @return This accessor's type constant, i.e. LOS_InertialB_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::lOS_InertialB_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LOS_InertialB_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Azimuth.
           *
           * @return The value of the simple primitive data type identified by Azimuth.
           */
         virtual uci::type::AnglePositiveTypeValue getAzimuth() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Azimuth.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAzimuth(uci::type::AnglePositiveTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Elevation.
           *
           * @return The value of the simple primitive data type identified by Elevation.
           */
         virtual uci::type::AngleTypeValue getElevation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Elevation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setElevation(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SlantRange.
           *
           * @return The value of the simple primitive data type identified by SlantRange.
           */
         virtual uci::type::DistanceTypeValue getSlantRange() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SlantRange.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSlantRange(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SlantRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SlantRange is emabled or not
           */
         virtual bool hasSlantRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SlantRange
           *
           * @param type = uci::type::accessorType::distanceType This Accessor's accessor type
           */
         virtual void enableSlantRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::distanceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SlantRange */
         virtual void clearSlantRange()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         LOS_InertialB_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LOS_InertialB_Type to copy from
           */
         LOS_InertialB_Type(const LOS_InertialB_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LOS_InertialB_Type to the contents of the LOS_InertialB_Type on
           * the right hand side (rhs) of the assignment operator.LOS_InertialB_Type [only available to derived classes]
           *
           * @param rhs The LOS_InertialB_Type on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::LOS_InertialB_Type
           * @return A constant reference to this LOS_InertialB_Type.
           */
         const LOS_InertialB_Type& operator=(const LOS_InertialB_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LOS_InertialB_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LOS_InertialB_Type_h

