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
#ifndef Uci__Type__RxBeamType_h
#define Uci__Type__RxBeamType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__BeamID_Type_h)
# include "uci/type/BeamID_Type.h"
#endif

#if !defined(Uci__Type__BeamWidthType_h)
# include "uci/type/BeamWidthType.h"
#endif

#if !defined(Uci__Type__EA_PointingEnum_h)
# include "uci/type/EA_PointingEnum.h"
#endif

#if !defined(Uci__Type__BeamPointingReferenceType_h)
# include "uci/type/BeamPointingReferenceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type references receive beam characteristics in use by an EA to perform tracking. */
      class RxBeamType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RxBeamType()
         { }

         /** Returns this accessor's type constant, i.e. RxBeamType
           *
           * @return This accessor's type constant, i.e. RxBeamType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rxBeamType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RxBeamType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BeamID.
           *
           * @return The acecssor that provides access to the complex content that is identified by BeamID.
           */
         virtual const uci::type::BeamID_Type& getBeamID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BeamID.
           *
           * @return The acecssor that provides access to the complex content that is identified by BeamID.
           */
         virtual uci::type::BeamID_Type& getBeamID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the BeamID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by BeamID
           */
         virtual void setBeamID(const uci::type::BeamID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BeamWidths.
           *
           * @return The acecssor that provides access to the complex content that is identified by BeamWidths.
           */
         virtual const uci::type::BeamWidthType& getBeamWidths() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BeamWidths.
           *
           * @return The acecssor that provides access to the complex content that is identified by BeamWidths.
           */
         virtual uci::type::BeamWidthType& getBeamWidths()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the BeamWidths to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by BeamWidths
           */
         virtual void setBeamWidths(const uci::type::BeamWidthType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TrackingMode.
           *
           * @return The value of the enumeration identified by TrackingMode.
           */
         virtual const uci::type::EA_PointingEnum& getTrackingMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TrackingMode.
           *
           * @return The value of the enumeration identified by TrackingMode.
           */
         virtual uci::type::EA_PointingEnum& getTrackingMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TrackingMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTrackingMode(const uci::type::EA_PointingEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TrackingMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTrackingMode(uci::type::EA_PointingEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield TrackingModeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTrackingMode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTrackingMode will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::eA_PointingEnum This Accessor's accessor type
           */
         virtual void enableTrackingMode(uci::base::accessorType::AccessorType type = uci::type::accessorType::eA_PointingEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTrackingMode()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BeamPointingReference.
           *
           * @return The acecssor that provides access to the complex content that is identified by BeamPointingReference.
           */
         virtual const uci::type::BeamPointingReferenceType& getBeamPointingReference() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BeamPointingReference.
           *
           * @return The acecssor that provides access to the complex content that is identified by BeamPointingReference.
           */
         virtual uci::type::BeamPointingReferenceType& getBeamPointingReference()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the BeamPointingReference to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by BeamPointingReference
           */
         virtual void setBeamPointingReference(const uci::type::BeamPointingReferenceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BeamPointingReference exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BeamPointingReference is emabled or not
           */
         virtual bool hasBeamPointingReference() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BeamPointingReference
           *
           * @param type = uci::type::accessorType::beamPointingReferenceType This Accessor's accessor type
           */
         virtual void enableBeamPointingReference(uci::base::accessorType::AccessorType type = uci::type::accessorType::beamPointingReferenceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BeamPointingReference */
         virtual void clearBeamPointingReference()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RxBeamType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RxBeamType to copy from
           */
         RxBeamType(const RxBeamType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RxBeamType to the contents of the RxBeamType on the right hand
           * side (rhs) of the assignment operator.RxBeamType [only available to derived classes]
           *
           * @param rhs The RxBeamType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::RxBeamType
           * @return A constant reference to this RxBeamType.
           */
         const RxBeamType& operator=(const RxBeamType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RxBeamType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RxBeamType_h

