// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__AreaConstraintsType_h
#define Uci__Type__AreaConstraintsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DistanceConstraintsType_h)
# include "uci/type/DistanceConstraintsType.h"
#endif

#if !defined(Uci__Type__AltitudeConstraintsType_h)
# include "uci/type/AltitudeConstraintsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Specify the constraints of an area. The area is defined in terms of distance, altitude, or both, but neither is
        * required. If neither min nor max value is specified, then the range is all distances or all altitudes.
        */
      class AreaConstraintsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AreaConstraintsType()
         { }

         /** Returns this accessor's type constant, i.e. AreaConstraintsType
           *
           * @return This accessor's type constant, i.e. AreaConstraintsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::areaConstraintsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AreaConstraintsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DistanceContraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by DistanceContraints.
           */
         virtual const uci::type::DistanceConstraintsType& getDistanceContraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DistanceContraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by DistanceContraints.
           */
         virtual uci::type::DistanceConstraintsType& getDistanceContraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DistanceContraints to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DistanceContraints
           */
         virtual void setDistanceContraints(const uci::type::DistanceConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DistanceContraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DistanceContraints is emabled or not
           */
         virtual bool hasDistanceContraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DistanceContraints
           *
           * @param type = uci::type::accessorType::distanceConstraintsType This Accessor's accessor type
           */
         virtual void enableDistanceContraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::distanceConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DistanceContraints */
         virtual void clearDistanceContraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AltitudeConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by AltitudeConstraints.
           */
         virtual const uci::type::AltitudeConstraintsType& getAltitudeConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AltitudeConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by AltitudeConstraints.
           */
         virtual uci::type::AltitudeConstraintsType& getAltitudeConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AltitudeConstraints to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AltitudeConstraints
           */
         virtual void setAltitudeConstraints(const uci::type::AltitudeConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AltitudeConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AltitudeConstraints is emabled or not
           */
         virtual bool hasAltitudeConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AltitudeConstraints
           *
           * @param type = uci::type::accessorType::altitudeConstraintsType This Accessor's accessor type
           */
         virtual void enableAltitudeConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AltitudeConstraints */
         virtual void clearAltitudeConstraints()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AreaConstraintsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AreaConstraintsType to copy from
           */
         AreaConstraintsType(const AreaConstraintsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AreaConstraintsType to the contents of the AreaConstraintsType on
           * the right hand side (rhs) of the assignment operator.AreaConstraintsType [only available to derived classes]
           *
           * @param rhs The AreaConstraintsType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::AreaConstraintsType
           * @return A constant reference to this AreaConstraintsType.
           */
         const AreaConstraintsType& operator=(const AreaConstraintsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AreaConstraintsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AreaConstraintsType_h

