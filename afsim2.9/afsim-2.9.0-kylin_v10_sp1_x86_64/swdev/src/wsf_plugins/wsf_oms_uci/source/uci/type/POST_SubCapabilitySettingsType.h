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
#ifndef Uci__Type__POST_SubCapabilitySettingsType_h
#define Uci__Type__POST_SubCapabilitySettingsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubCapabilityID_Type_h)
# include "uci/type/SubCapabilityID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__POST_TechniqueSettingType_h)
# include "uci/type/POST_TechniqueSettingType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates an installed POST high-level SubCapability which adds an additional level of granularity to complete the
        * mission using POST commands and tasks.
        */
      class POST_SubCapabilitySettingsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_SubCapabilitySettingsType()
         { }

         /** Returns this accessor's type constant, i.e. POST_SubCapabilitySettingsType
           *
           * @return This accessor's type constant, i.e. POST_SubCapabilitySettingsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_SubCapabilitySettingsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_SubCapabilitySettingsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the passive optical search and track technique for tracking and search algorithm purposes. The technique
           * setting is used to identify the technique type with the enumeration and any possible setting value associated with it
           * with the optional fields. [Minimum occurrences: 0] [Maximum occurrences: 17]
           */
         typedef uci::base::BoundedList<uci::type::POST_TechniqueSettingType, uci::type::accessorType::pOST_TechniqueSettingType> TechniqueSettings;

         /** Returns the accessor that provides access to the complex content that is identified by the SubCapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubCapabilityID.
           */
         virtual const uci::type::SubCapabilityID_Type& getSubCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubCapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubCapabilityID.
           */
         virtual uci::type::SubCapabilityID_Type& getSubCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubCapabilityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubCapabilityID
           */
         virtual void setSubCapabilityID(const uci::type::SubCapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TechniqueSettings.
           *
           * @return The bounded list identified by TechniqueSettings.
           */
         virtual const uci::type::POST_SubCapabilitySettingsType::TechniqueSettings& getTechniqueSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TechniqueSettings.
           *
           * @return The bounded list identified by TechniqueSettings.
           */
         virtual uci::type::POST_SubCapabilitySettingsType::TechniqueSettings& getTechniqueSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TechniqueSettings.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTechniqueSettings(const uci::type::POST_SubCapabilitySettingsType::TechniqueSettings& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_SubCapabilitySettingsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_SubCapabilitySettingsType to copy from
           */
         POST_SubCapabilitySettingsType(const POST_SubCapabilitySettingsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_SubCapabilitySettingsType to the contents of the
           * POST_SubCapabilitySettingsType on the right hand side (rhs) of the assignment operator.POST_SubCapabilitySettingsType
           * [only available to derived classes]
           *
           * @param rhs The POST_SubCapabilitySettingsType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::POST_SubCapabilitySettingsType
           * @return A constant reference to this POST_SubCapabilitySettingsType.
           */
         const POST_SubCapabilitySettingsType& operator=(const POST_SubCapabilitySettingsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_SubCapabilitySettingsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_SubCapabilitySettingsType_h

