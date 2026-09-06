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
#ifndef Uci__Type__POST_ComponentStatusOutputProductSettingsType_h
#define Uci__Type__POST_ComponentStatusOutputProductSettingsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ProductOutputComponentStatusImageryType_h)
# include "uci/type/ProductOutputComponentStatusImageryType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_ComponentStatusOutputProductSettingsType sequence accessor class */
      class POST_ComponentStatusOutputProductSettingsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_ComponentStatusOutputProductSettingsType()
         { }

         /** Returns this accessor's type constant, i.e. POST_ComponentStatusOutputProductSettingsType
           *
           * @return This accessor's type constant, i.e. POST_ComponentStatusOutputProductSettingsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_ComponentStatusOutputProductSettingsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_ComponentStatusOutputProductSettingsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductSettings.
           */
         virtual const uci::type::ProductOutputComponentStatusImageryType& getProductSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductSettings.
           */
         virtual uci::type::ProductOutputComponentStatusImageryType& getProductSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductSettings
           */
         virtual void setProductSettings(const uci::type::ProductOutputComponentStatusImageryType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProductSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProductSettings is emabled or not
           */
         virtual bool hasProductSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProductSettings
           *
           * @param type = uci::type::accessorType::productOutputComponentStatusImageryType This Accessor's accessor type
           */
         virtual void enableProductSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::productOutputComponentStatusImageryType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProductSettings */
         virtual void clearProductSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AutoOutputProductSettings.
           *
           * @return The value of the simple primitive data type identified by AutoOutputProductSettings.
           */
         virtual xs::Boolean getAutoOutputProductSettings() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AutoOutputProductSettings.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAutoOutputProductSettings(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AutoOutputProductSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AutoOutputProductSettings is emabled or not
           */
         virtual bool hasAutoOutputProductSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AutoOutputProductSettings
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableAutoOutputProductSettings(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AutoOutputProductSettings */
         virtual void clearAutoOutputProductSettings()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_ComponentStatusOutputProductSettingsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_ComponentStatusOutputProductSettingsType to copy from
           */
         POST_ComponentStatusOutputProductSettingsType(const POST_ComponentStatusOutputProductSettingsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_ComponentStatusOutputProductSettingsType to the contents of
           * the POST_ComponentStatusOutputProductSettingsType on the right hand side (rhs) of the assignment
           * operator.POST_ComponentStatusOutputProductSettingsType [only available to derived classes]
           *
           * @param rhs The POST_ComponentStatusOutputProductSettingsType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::POST_ComponentStatusOutputProductSettingsType
           * @return A constant reference to this POST_ComponentStatusOutputProductSettingsType.
           */
         const POST_ComponentStatusOutputProductSettingsType& operator=(const POST_ComponentStatusOutputProductSettingsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_ComponentStatusOutputProductSettingsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_ComponentStatusOutputProductSettingsType_h

