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
#ifndef Uci__Type__POST_SubCapabilityType_h
#define Uci__Type__POST_SubCapabilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubCapabilityID_Type_h)
# include "uci/type/SubCapabilityID_Type.h"
#endif

#if !defined(Uci__Type__POST_SubCapabilityEnum_h)
# include "uci/type/POST_SubCapabilityEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__POST_TechniqueCapabilityType_h)
# include "uci/type/POST_TechniqueCapabilityType.h"
#endif

#if !defined(Uci__Type__POST_EnvironmentEnum_h)
# include "uci/type/POST_EnvironmentEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates an installed POST high-level subCapability which adds an additional level of granularity to complete the
        * mission using POST commands and tasks.
        */
      class POST_SubCapabilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_SubCapabilityType()
         { }

         /** Returns this accessor's type constant, i.e. POST_SubCapabilityType
           *
           * @return This accessor's type constant, i.e. POST_SubCapabilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_SubCapabilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_SubCapabilityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the passive optical search and track technique for tracking and search algorithm purposes. Each technique
           * setting requires the type with the enumeration and any possible minimum and maximum settings for that technique with
           * the optional fields. [Minimum occurrences: 0] [Maximum occurrences: 17]
           */
         typedef uci::base::BoundedList<uci::type::POST_TechniqueCapabilityType, uci::type::accessorType::pOST_TechniqueCapabilityType> TechniqueSettings;

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


         /** Returns the value of the enumeration that is identified by the Spectrum.
           *
           * @return The value of the enumeration identified by Spectrum.
           */
         virtual const uci::type::POST_SubCapabilityEnum& getSpectrum() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Spectrum.
           *
           * @return The value of the enumeration identified by Spectrum.
           */
         virtual uci::type::POST_SubCapabilityEnum& getSpectrum()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Spectrum.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSpectrum(const uci::type::POST_SubCapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Spectrum.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSpectrum(uci::type::POST_SubCapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TechniqueSettings.
           *
           * @return The bounded list identified by TechniqueSettings.
           */
         virtual const uci::type::POST_SubCapabilityType::TechniqueSettings& getTechniqueSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TechniqueSettings.
           *
           * @return The bounded list identified by TechniqueSettings.
           */
         virtual uci::type::POST_SubCapabilityType::TechniqueSettings& getTechniqueSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TechniqueSettings.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTechniqueSettings(const uci::type::POST_SubCapabilityType::TechniqueSettings& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Environment.
           *
           * @return The value of the enumeration identified by Environment.
           */
         virtual const uci::type::POST_EnvironmentEnum& getEnvironment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Environment.
           *
           * @return The value of the enumeration identified by Environment.
           */
         virtual uci::type::POST_EnvironmentEnum& getEnvironment()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Environment.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEnvironment(const uci::type::POST_EnvironmentEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Environment.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEnvironment(uci::type::POST_EnvironmentEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Environmentis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasEnvironment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getEnvironment will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::pOST_EnvironmentEnum This Accessor's accessor type
           */
         virtual void enableEnvironment(uci::base::accessorType::AccessorType type = uci::type::accessorType::pOST_EnvironmentEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearEnvironment()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_SubCapabilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_SubCapabilityType to copy from
           */
         POST_SubCapabilityType(const POST_SubCapabilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_SubCapabilityType to the contents of the
           * POST_SubCapabilityType on the right hand side (rhs) of the assignment operator.POST_SubCapabilityType [only available
           * to derived classes]
           *
           * @param rhs The POST_SubCapabilityType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::POST_SubCapabilityType
           * @return A constant reference to this POST_SubCapabilityType.
           */
         const POST_SubCapabilityType& operator=(const POST_SubCapabilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_SubCapabilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_SubCapabilityType_h

