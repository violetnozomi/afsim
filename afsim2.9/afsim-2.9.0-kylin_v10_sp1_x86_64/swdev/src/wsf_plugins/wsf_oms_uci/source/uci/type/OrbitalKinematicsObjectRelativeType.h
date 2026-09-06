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
#ifndef Uci__Type__OrbitalKinematicsObjectRelativeType_h
#define Uci__Type__OrbitalKinematicsObjectRelativeType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__RTN_KinematicsType_h)
# include "uci/type/RTN_KinematicsType.h"
#endif

#if !defined(Uci__Type__OrbitalKinematicsStandardFrameChoiceType_h)
# include "uci/type/OrbitalKinematicsStandardFrameChoiceType.h"
#endif

#if !defined(Uci__Type__AssetType_h)
# include "uci/type/AssetType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates kinematics for an orbital object, expressed in the Radial-Transverse-Normal Reference System, relative to
        * another orbital object whose kinematics are relative to a celestial object.
        */
      class OrbitalKinematicsObjectRelativeType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OrbitalKinematicsObjectRelativeType()
         { }

         /** Returns this accessor's type constant, i.e. OrbitalKinematicsObjectRelativeType
           *
           * @return This accessor's type constant, i.e. OrbitalKinematicsObjectRelativeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::orbitalKinematicsObjectRelativeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OrbitalKinematicsObjectRelativeType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RelativeObjectKinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by RelativeObjectKinematics.
           */
         virtual const uci::type::RTN_KinematicsType& getRelativeObjectKinematics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RelativeObjectKinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by RelativeObjectKinematics.
           */
         virtual uci::type::RTN_KinematicsType& getRelativeObjectKinematics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RelativeObjectKinematics to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RelativeObjectKinematics
           */
         virtual void setRelativeObjectKinematics(const uci::type::RTN_KinematicsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReferenceObjectKinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReferenceObjectKinematics.
           */
         virtual const uci::type::OrbitalKinematicsStandardFrameChoiceType& getReferenceObjectKinematics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReferenceObjectKinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReferenceObjectKinematics.
           */
         virtual uci::type::OrbitalKinematicsStandardFrameChoiceType& getReferenceObjectKinematics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReferenceObjectKinematics to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReferenceObjectKinematics
           */
         virtual void setReferenceObjectKinematics(const uci::type::OrbitalKinematicsStandardFrameChoiceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReferenceAsset.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReferenceAsset.
           */
         virtual const uci::type::AssetType& getReferenceAsset() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReferenceAsset.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReferenceAsset.
           */
         virtual uci::type::AssetType& getReferenceAsset()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReferenceAsset to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReferenceAsset
           */
         virtual void setReferenceAsset(const uci::type::AssetType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReferenceAsset exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReferenceAsset is emabled or not
           */
         virtual bool hasReferenceAsset() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReferenceAsset
           *
           * @param type = uci::type::accessorType::assetType This Accessor's accessor type
           */
         virtual void enableReferenceAsset(uci::base::accessorType::AccessorType type = uci::type::accessorType::assetType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReferenceAsset */
         virtual void clearReferenceAsset()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OrbitalKinematicsObjectRelativeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OrbitalKinematicsObjectRelativeType to copy from
           */
         OrbitalKinematicsObjectRelativeType(const OrbitalKinematicsObjectRelativeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OrbitalKinematicsObjectRelativeType to the contents of the
           * OrbitalKinematicsObjectRelativeType on the right hand side (rhs) of the assignment
           * operator.OrbitalKinematicsObjectRelativeType [only available to derived classes]
           *
           * @param rhs The OrbitalKinematicsObjectRelativeType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::OrbitalKinematicsObjectRelativeType
           * @return A constant reference to this OrbitalKinematicsObjectRelativeType.
           */
         const OrbitalKinematicsObjectRelativeType& operator=(const OrbitalKinematicsObjectRelativeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OrbitalKinematicsObjectRelativeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OrbitalKinematicsObjectRelativeType_h

