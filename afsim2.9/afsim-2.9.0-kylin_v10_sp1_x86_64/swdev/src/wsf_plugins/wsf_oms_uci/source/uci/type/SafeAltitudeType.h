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
#ifndef Uci__Type__SafeAltitudeType_h
#define Uci__Type__SafeAltitudeType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__Point2D_Type_h)
# include "uci/type/Point2D_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AltitudeRadialPairType_h)
# include "uci/type/AltitudeRadialPairType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This field specifies a set of safe minimum approach altitudes and an emergency safe altitude. This field also
        * specifies the cylindrical area, defined by a center point and radius, where the altitudes apply. This field defines
        * minimum approach altitude and emergency safe altitude consistent with standard FAA instrument approach chart
        * definitions (using UCI standard data types). This information can, for example, be used to eliminate runways or
        * extirpation points when the required altitude is not achievable during an anomaly.
        */
      class SafeAltitudeType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SafeAltitudeType()
         { }

         /** Returns this accessor's type constant, i.e. SafeAltitudeType
           *
           * @return This accessor's type constant, i.e. SafeAltitudeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::safeAltitudeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SafeAltitudeType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This field defines the minimum safe altitudes (MSA) as defined by the FAA Instrument Procedures Handbook, Chapter 4
           * "Approaches", "Minimum Safe/Sector Altitude" section. MSAs are published for emergency use on instrument approach
           * charts. MSAs provide 1,000 feet of clearance over all obstacles but do not necessarily assure acceptable navigation
           * signal coverage. Altitudes provided here are valid to a 25 nm radius range. Each array element defines the MSA
           * between the specified radial and the next element's radial. As a result elements in the array need to have a radial
           * greater than the preceding radial (i.e. in increasing radial order). If only one element is provided, its radial is
           * N/A as the MSA applies to the full circle. Radials are measured inbounds to the center point, relative to true north.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::AltitudeRadialPairType, uci::type::accessorType::altitudeRadialPairType> MinimumSafeAltitude;

         /** Returns the accessor that provides access to the complex content that is identified by the CenterPointAndESA.
           *
           * @return The acecssor that provides access to the complex content that is identified by CenterPointAndESA.
           */
         virtual const uci::type::Point2D_Type& getCenterPointAndESA() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CenterPointAndESA.
           *
           * @return The acecssor that provides access to the complex content that is identified by CenterPointAndESA.
           */
         virtual uci::type::Point2D_Type& getCenterPointAndESA()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CenterPointAndESA to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CenterPointAndESA
           */
         virtual void setCenterPointAndESA(const uci::type::Point2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MinimumSafeAltitude.
           *
           * @return The bounded list identified by MinimumSafeAltitude.
           */
         virtual const uci::type::SafeAltitudeType::MinimumSafeAltitude& getMinimumSafeAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MinimumSafeAltitude.
           *
           * @return The bounded list identified by MinimumSafeAltitude.
           */
         virtual uci::type::SafeAltitudeType::MinimumSafeAltitude& getMinimumSafeAltitude()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MinimumSafeAltitude.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMinimumSafeAltitude(const uci::type::SafeAltitudeType::MinimumSafeAltitude& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SafeAltitudeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SafeAltitudeType to copy from
           */
         SafeAltitudeType(const SafeAltitudeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SafeAltitudeType to the contents of the SafeAltitudeType on the
           * right hand side (rhs) of the assignment operator.SafeAltitudeType [only available to derived classes]
           *
           * @param rhs The SafeAltitudeType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::SafeAltitudeType
           * @return A constant reference to this SafeAltitudeType.
           */
         const SafeAltitudeType& operator=(const SafeAltitudeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SafeAltitudeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SafeAltitudeType_h

