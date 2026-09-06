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
#ifndef Uci__Type__SatelliteC2_IdentityType_h
#define Uci__Type__SatelliteC2_IdentityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__StandardIdentityType_h)
# include "uci/type/StandardIdentityType.h"
#endif

#if !defined(Uci__Type__FacilityIdentificationType_h)
# include "uci/type/FacilityIdentificationType.h"
#endif

#if !defined(Uci__Type__EOB_IdentityType_h)
# include "uci/type/EOB_IdentityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates the identity of an asset. */
      class SatelliteC2_IdentityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SatelliteC2_IdentityType()
         { }

         /** Returns this accessor's type constant, i.e. SatelliteC2_IdentityType
           *
           * @return This accessor's type constant, i.e. SatelliteC2_IdentityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::satelliteC2_IdentityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SatelliteC2_IdentityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Standard.
           *
           * @return The acecssor that provides access to the complex content that is identified by Standard.
           */
         virtual const uci::type::StandardIdentityType& getStandard() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Standard.
           *
           * @return The acecssor that provides access to the complex content that is identified by Standard.
           */
         virtual uci::type::StandardIdentityType& getStandard()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Standard to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Standard
           */
         virtual void setStandard(const uci::type::StandardIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Standard exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Standard is emabled or not
           */
         virtual bool hasStandard() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Standard
           *
           * @param type = uci::type::accessorType::standardIdentityType This Accessor's accessor type
           */
         virtual void enableStandard(uci::base::accessorType::AccessorType type = uci::type::accessorType::standardIdentityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Standard */
         virtual void clearStandard()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SpecificFacility.
           *
           * @return The acecssor that provides access to the complex content that is identified by SpecificFacility.
           */
         virtual const uci::type::FacilityIdentificationType& getSpecificFacility() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SpecificFacility.
           *
           * @return The acecssor that provides access to the complex content that is identified by SpecificFacility.
           */
         virtual uci::type::FacilityIdentificationType& getSpecificFacility()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SpecificFacility to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SpecificFacility
           */
         virtual void setSpecificFacility(const uci::type::FacilityIdentificationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SpecificFacility exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SpecificFacility is emabled or not
           */
         virtual bool hasSpecificFacility() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SpecificFacility
           *
           * @param type = uci::type::accessorType::facilityIdentificationType This Accessor's accessor type
           */
         virtual void enableSpecificFacility(uci::base::accessorType::AccessorType type = uci::type::accessorType::facilityIdentificationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SpecificFacility */
         virtual void clearSpecificFacility()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EOB.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB.
           */
         virtual const uci::type::EOB_IdentityType& getEOB() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EOB.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB.
           */
         virtual uci::type::EOB_IdentityType& getEOB()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EOB to the contents of the complex content that is accessed by the
           * specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EOB
           */
         virtual void setEOB(const uci::type::EOB_IdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EOB exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EOB is emabled or not
           */
         virtual bool hasEOB() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EOB
           *
           * @param type = uci::type::accessorType::eOB_IdentityType This Accessor's accessor type
           */
         virtual void enableEOB(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_IdentityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EOB */
         virtual void clearEOB()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SatelliteC2_IdentityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SatelliteC2_IdentityType to copy from
           */
         SatelliteC2_IdentityType(const SatelliteC2_IdentityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SatelliteC2_IdentityType to the contents of the
           * SatelliteC2_IdentityType on the right hand side (rhs) of the assignment operator.SatelliteC2_IdentityType [only
           * available to derived classes]
           *
           * @param rhs The SatelliteC2_IdentityType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SatelliteC2_IdentityType
           * @return A constant reference to this SatelliteC2_IdentityType.
           */
         const SatelliteC2_IdentityType& operator=(const SatelliteC2_IdentityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SatelliteC2_IdentityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SatelliteC2_IdentityType_h

