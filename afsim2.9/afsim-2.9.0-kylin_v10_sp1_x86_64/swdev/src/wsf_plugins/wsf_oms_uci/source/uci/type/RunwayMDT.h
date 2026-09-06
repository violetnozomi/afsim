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
#ifndef Uci__Type__RunwayMDT_h
#define Uci__Type__RunwayMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__RunwayID_Type_h)
# include "uci/type/RunwayID_Type.h"
#endif

#if !defined(Uci__Type__RunwayCoordinatesType_h)
# include "uci/type/RunwayCoordinatesType.h"
#endif

#if !defined(Uci__Type__RunwayUsageDetailsType_h)
# include "uci/type/RunwayUsageDetailsType.h"
#endif

#if !defined(Uci__Type__SafeAltitudeType_h)
# include "uci/type/SafeAltitudeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RunwayMDT sequence accessor class */
      class RunwayMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RunwayMDT()
         { }

         /** Returns this accessor's type constant, i.e. RunwayMDT
           *
           * @return This accessor's type constant, i.e. RunwayMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::runwayMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RunwayMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RunwayID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RunwayID.
           */
         virtual const uci::type::RunwayID_Type& getRunwayID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RunwayID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RunwayID.
           */
         virtual uci::type::RunwayID_Type& getRunwayID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RunwayID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RunwayID
           */
         virtual void setRunwayID(const uci::type::RunwayID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TakeoffCoordinates.
           *
           * @return The acecssor that provides access to the complex content that is identified by TakeoffCoordinates.
           */
         virtual const uci::type::RunwayCoordinatesType& getTakeoffCoordinates() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TakeoffCoordinates.
           *
           * @return The acecssor that provides access to the complex content that is identified by TakeoffCoordinates.
           */
         virtual uci::type::RunwayCoordinatesType& getTakeoffCoordinates()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TakeoffCoordinates to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TakeoffCoordinates
           */
         virtual void setTakeoffCoordinates(const uci::type::RunwayCoordinatesType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TakeoffCoordinates exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TakeoffCoordinates is emabled or not
           */
         virtual bool hasTakeoffCoordinates() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TakeoffCoordinates
           *
           * @param type = uci::type::accessorType::runwayCoordinatesType This Accessor's accessor type
           */
         virtual void enableTakeoffCoordinates(uci::base::accessorType::AccessorType type = uci::type::accessorType::runwayCoordinatesType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TakeoffCoordinates */
         virtual void clearTakeoffCoordinates()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LandingCoordinates.
           *
           * @return The acecssor that provides access to the complex content that is identified by LandingCoordinates.
           */
         virtual const uci::type::RunwayCoordinatesType& getLandingCoordinates() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LandingCoordinates.
           *
           * @return The acecssor that provides access to the complex content that is identified by LandingCoordinates.
           */
         virtual uci::type::RunwayCoordinatesType& getLandingCoordinates()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LandingCoordinates to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LandingCoordinates
           */
         virtual void setLandingCoordinates(const uci::type::RunwayCoordinatesType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LandingCoordinates exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LandingCoordinates is emabled or not
           */
         virtual bool hasLandingCoordinates() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LandingCoordinates
           *
           * @param type = uci::type::accessorType::runwayCoordinatesType This Accessor's accessor type
           */
         virtual void enableLandingCoordinates(uci::base::accessorType::AccessorType type = uci::type::accessorType::runwayCoordinatesType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LandingCoordinates */
         virtual void clearLandingCoordinates()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultRunwayUsageDetails.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultRunwayUsageDetails.
           */
         virtual const uci::type::RunwayUsageDetailsType& getDefaultRunwayUsageDetails() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultRunwayUsageDetails.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultRunwayUsageDetails.
           */
         virtual uci::type::RunwayUsageDetailsType& getDefaultRunwayUsageDetails()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DefaultRunwayUsageDetails to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DefaultRunwayUsageDetails
           */
         virtual void setDefaultRunwayUsageDetails(const uci::type::RunwayUsageDetailsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DefaultRunwayUsageDetails exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DefaultRunwayUsageDetails is emabled or not
           */
         virtual bool hasDefaultRunwayUsageDetails() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DefaultRunwayUsageDetails
           *
           * @param type = uci::type::accessorType::runwayUsageDetailsType This Accessor's accessor type
           */
         virtual void enableDefaultRunwayUsageDetails(uci::base::accessorType::AccessorType type = uci::type::accessorType::runwayUsageDetailsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DefaultRunwayUsageDetails */
         virtual void clearDefaultRunwayUsageDetails()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SafeAltitude.
           *
           * @return The acecssor that provides access to the complex content that is identified by SafeAltitude.
           */
         virtual const uci::type::SafeAltitudeType& getSafeAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SafeAltitude.
           *
           * @return The acecssor that provides access to the complex content that is identified by SafeAltitude.
           */
         virtual uci::type::SafeAltitudeType& getSafeAltitude()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SafeAltitude to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SafeAltitude
           */
         virtual void setSafeAltitude(const uci::type::SafeAltitudeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SafeAltitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SafeAltitude is emabled or not
           */
         virtual bool hasSafeAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SafeAltitude
           *
           * @param type = uci::type::accessorType::safeAltitudeType This Accessor's accessor type
           */
         virtual void enableSafeAltitude(uci::base::accessorType::AccessorType type = uci::type::accessorType::safeAltitudeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SafeAltitude */
         virtual void clearSafeAltitude()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RunwayMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RunwayMDT to copy from
           */
         RunwayMDT(const RunwayMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RunwayMDT to the contents of the RunwayMDT on the right hand side
           * (rhs) of the assignment operator.RunwayMDT [only available to derived classes]
           *
           * @param rhs The RunwayMDT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::RunwayMDT
           * @return A constant reference to this RunwayMDT.
           */
         const RunwayMDT& operator=(const RunwayMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RunwayMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RunwayMDT_h

