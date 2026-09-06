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
#ifndef Uci__Type__POST_ActivityCommandType_h
#define Uci__Type__POST_ActivityCommandType_h 1

#if !defined(Uci__Type__ActivityCommandBaseType_h)
# include "uci/type/ActivityCommandBaseType.h"
#endif

#if !defined(Uci__Type__PointingType_h)
# include "uci/type/PointingType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__POST_SubCapabilitySettingsType_h)
# include "uci/type/POST_SubCapabilitySettingsType.h"
#endif

#if !defined(Uci__Type__POST_ComponentSettingsType_h)
# include "uci/type/POST_ComponentSettingsType.h"
#endif

#if !defined(Uci__Type__PO_CollectionConstraintsSettingsType_h)
# include "uci/type/PO_CollectionConstraintsSettingsType.h"
#endif

#if !defined(Uci__Type__POST_TrackDetailsType_h)
# include "uci/type/POST_TrackDetailsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_ActivityCommandType sequence accessor class */
      class POST_ActivityCommandType : public virtual uci::type::ActivityCommandBaseType {
      public:

         /** The destructor */
         virtual ~POST_ActivityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. POST_ActivityCommandType
           *
           * @return This accessor's type constant, i.e. POST_ActivityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_ActivityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_ActivityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This field contains the settings that will override the current subcapabiility settings in the activity being
           * adjusted. These changes, however, do not impact the default settings for the associated subcapability. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_SubCapabilitySettingsType, uci::type::accessorType::pOST_SubCapabilitySettingsType> ChangeSubCapability;

         /** This field contains the set of settings that will override the current component settings in the activity being
           * adjusted. These changes, however, do not impact the default settings for the associated component. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_ComponentSettingsType, uci::type::accessorType::pOST_ComponentSettingsType> ChangeComponentSettings;

         /** Returns the accessor that provides access to the complex content that is identified by the Pointing.
           *
           * @return The acecssor that provides access to the complex content that is identified by Pointing.
           */
         virtual const uci::type::PointingType& getPointing() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Pointing.
           *
           * @return The acecssor that provides access to the complex content that is identified by Pointing.
           */
         virtual uci::type::PointingType& getPointing()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Pointing to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Pointing
           */
         virtual void setPointing(const uci::type::PointingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Pointing exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Pointing is emabled or not
           */
         virtual bool hasPointing() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Pointing
           *
           * @param type = uci::type::accessorType::pointingType This Accessor's accessor type
           */
         virtual void enablePointing(uci::base::accessorType::AccessorType type = uci::type::accessorType::pointingType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Pointing */
         virtual void clearPointing()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ChangeSubCapability.
           *
           * @return The bounded list identified by ChangeSubCapability.
           */
         virtual const uci::type::POST_ActivityCommandType::ChangeSubCapability& getChangeSubCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ChangeSubCapability.
           *
           * @return The bounded list identified by ChangeSubCapability.
           */
         virtual uci::type::POST_ActivityCommandType::ChangeSubCapability& getChangeSubCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ChangeSubCapability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setChangeSubCapability(const uci::type::POST_ActivityCommandType::ChangeSubCapability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ChangeComponentSettings.
           *
           * @return The bounded list identified by ChangeComponentSettings.
           */
         virtual const uci::type::POST_ActivityCommandType::ChangeComponentSettings& getChangeComponentSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ChangeComponentSettings.
           *
           * @return The bounded list identified by ChangeComponentSettings.
           */
         virtual uci::type::POST_ActivityCommandType::ChangeComponentSettings& getChangeComponentSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ChangeComponentSettings.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setChangeComponentSettings(const uci::type::POST_ActivityCommandType::ChangeComponentSettings& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ChangeCollectionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeCollectionConstraints.
           */
         virtual const uci::type::PO_CollectionConstraintsSettingsType& getChangeCollectionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ChangeCollectionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeCollectionConstraints.
           */
         virtual uci::type::PO_CollectionConstraintsSettingsType& getChangeCollectionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChangeCollectionConstraints to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChangeCollectionConstraints
           */
         virtual void setChangeCollectionConstraints(const uci::type::PO_CollectionConstraintsSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChangeCollectionConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChangeCollectionConstraints is emabled or not
           */
         virtual bool hasChangeCollectionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChangeCollectionConstraints
           *
           * @param type = uci::type::accessorType::pO_CollectionConstraintsSettingsType This Accessor's accessor type
           */
         virtual void enableChangeCollectionConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_CollectionConstraintsSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChangeCollectionConstraints */
         virtual void clearChangeCollectionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeTrack.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeTrack.
           */
         virtual const uci::type::POST_TrackDetailsType& getChangeTrack() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeTrack.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeTrack.
           */
         virtual uci::type::POST_TrackDetailsType& getChangeTrack()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChangeTrack to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChangeTrack
           */
         virtual void setChangeTrack(const uci::type::POST_TrackDetailsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChangeTrack exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChangeTrack is emabled or not
           */
         virtual bool hasChangeTrack() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChangeTrack
           *
           * @param type = uci::type::accessorType::pOST_TrackDetailsType This Accessor's accessor type
           */
         virtual void enableChangeTrack(uci::base::accessorType::AccessorType type = uci::type::accessorType::pOST_TrackDetailsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChangeTrack */
         virtual void clearChangeTrack()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_ActivityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_ActivityCommandType to copy from
           */
         POST_ActivityCommandType(const POST_ActivityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_ActivityCommandType to the contents of the
           * POST_ActivityCommandType on the right hand side (rhs) of the assignment operator.POST_ActivityCommandType [only
           * available to derived classes]
           *
           * @param rhs The POST_ActivityCommandType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::POST_ActivityCommandType
           * @return A constant reference to this POST_ActivityCommandType.
           */
         const POST_ActivityCommandType& operator=(const POST_ActivityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_ActivityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_ActivityCommandType_h

