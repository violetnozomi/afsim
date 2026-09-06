// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__EA_ResponseType_h
#define Uci__Type__EA_ResponseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EA_TaskEscortType_h)
# include "uci/type/EA_TaskEscortType.h"
#endif

#if !defined(Uci__Type__EA_TaskProtectedAssetsType_h)
# include "uci/type/EA_TaskProtectedAssetsType.h"
#endif

#if !defined(Uci__Type__EA_TaskThreatsType_h)
# include "uci/type/EA_TaskThreatsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EA_ResponseType sequence accessor class */
      class EA_ResponseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EA_ResponseType()
         { }

         /** Returns this accessor's type constant, i.e. EA_ResponseType
           *
           * @return This accessor's type constant, i.e. EA_ResponseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_ResponseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_ResponseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteRequirements.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteRequirements.
           */
         virtual const uci::type::EA_TaskEscortType& getRouteRequirements() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RouteRequirements.
           *
           * @return The acecssor that provides access to the complex content that is identified by RouteRequirements.
           */
         virtual uci::type::EA_TaskEscortType& getRouteRequirements()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RouteRequirements to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RouteRequirements
           */
         virtual void setRouteRequirements(const uci::type::EA_TaskEscortType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RouteRequirements exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RouteRequirements is emabled or not
           */
         virtual bool hasRouteRequirements() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RouteRequirements
           *
           * @param type = uci::type::accessorType::eA_TaskEscortType This Accessor's accessor type
           */
         virtual void enableRouteRequirements(uci::base::accessorType::AccessorType type = uci::type::accessorType::eA_TaskEscortType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RouteRequirements */
         virtual void clearRouteRequirements()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProtectedAssets.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProtectedAssets.
           */
         virtual const uci::type::EA_TaskProtectedAssetsType& getProtectedAssets() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProtectedAssets.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProtectedAssets.
           */
         virtual uci::type::EA_TaskProtectedAssetsType& getProtectedAssets()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProtectedAssets to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProtectedAssets
           */
         virtual void setProtectedAssets(const uci::type::EA_TaskProtectedAssetsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProtectedAssets exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProtectedAssets is emabled or not
           */
         virtual bool hasProtectedAssets() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProtectedAssets
           *
           * @param type = uci::type::accessorType::eA_TaskProtectedAssetsType This Accessor's accessor type
           */
         virtual void enableProtectedAssets(uci::base::accessorType::AccessorType type = uci::type::accessorType::eA_TaskProtectedAssetsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProtectedAssets */
         virtual void clearProtectedAssets()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Threats.
           *
           * @return The acecssor that provides access to the complex content that is identified by Threats.
           */
         virtual const uci::type::EA_TaskThreatsType& getThreats() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Threats.
           *
           * @return The acecssor that provides access to the complex content that is identified by Threats.
           */
         virtual uci::type::EA_TaskThreatsType& getThreats()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Threats to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Threats
           */
         virtual void setThreats(const uci::type::EA_TaskThreatsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Threats exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Threats is emabled or not
           */
         virtual bool hasThreats() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Threats
           *
           * @param type = uci::type::accessorType::eA_TaskThreatsType This Accessor's accessor type
           */
         virtual void enableThreats(uci::base::accessorType::AccessorType type = uci::type::accessorType::eA_TaskThreatsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Threats */
         virtual void clearThreats()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_ResponseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_ResponseType to copy from
           */
         EA_ResponseType(const EA_ResponseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_ResponseType to the contents of the EA_ResponseType on the
           * right hand side (rhs) of the assignment operator.EA_ResponseType [only available to derived classes]
           *
           * @param rhs The EA_ResponseType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::EA_ResponseType
           * @return A constant reference to this EA_ResponseType.
           */
         const EA_ResponseType& operator=(const EA_ResponseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_ResponseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_ResponseType_h

