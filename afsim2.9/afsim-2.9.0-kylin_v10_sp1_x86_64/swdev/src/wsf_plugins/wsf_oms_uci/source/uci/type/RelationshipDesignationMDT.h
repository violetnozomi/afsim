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
#ifndef Uci__Type__RelationshipDesignationMDT_h
#define Uci__Type__RelationshipDesignationMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__RelationshipDesignationID_Type_h)
# include "uci/type/RelationshipDesignationID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__GeoLocatedObjectType_h)
# include "uci/type/GeoLocatedObjectType.h"
#endif

#if !defined(Uci__Type__RelationshipType_h)
# include "uci/type/RelationshipType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RelationshipDesignationMDT sequence accessor class */
      class RelationshipDesignationMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RelationshipDesignationMDT()
         { }

         /** Returns this accessor's type constant, i.e. RelationshipDesignationMDT
           *
           * @return This accessor's type constant, i.e. RelationshipDesignationMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::relationshipDesignationMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RelationshipDesignationMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RelationshipID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RelationshipID.
           */
         virtual const uci::type::RelationshipDesignationID_Type& getRelationshipID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RelationshipID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RelationshipID.
           */
         virtual uci::type::RelationshipDesignationID_Type& getRelationshipID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RelationshipID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RelationshipID
           */
         virtual void setRelationshipID(const uci::type::RelationshipDesignationID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReportingSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReportingSystemID.
           */
         virtual const uci::type::SystemID_Type& getReportingSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReportingSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReportingSystemID.
           */
         virtual uci::type::SystemID_Type& getReportingSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReportingSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReportingSystemID
           */
         virtual void setReportingSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReportingSystemID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReportingSystemID is emabled or not
           */
         virtual bool hasReportingSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReportingSystemID
           *
           * @param type = uci::type::accessorType::systemID_Type This Accessor's accessor type
           */
         virtual void enableReportingSystemID(uci::base::accessorType::AccessorType type = uci::type::accessorType::systemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReportingSystemID */
         virtual void clearReportingSystemID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Source.
           *
           * @return The acecssor that provides access to the complex content that is identified by Source.
           */
         virtual const uci::type::GeoLocatedObjectType& getSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Source.
           *
           * @return The acecssor that provides access to the complex content that is identified by Source.
           */
         virtual uci::type::GeoLocatedObjectType& getSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Source to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Source
           */
         virtual void setSource(const uci::type::GeoLocatedObjectType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Destination.
           *
           * @return The acecssor that provides access to the complex content that is identified by Destination.
           */
         virtual const uci::type::GeoLocatedObjectType& getDestination() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Destination.
           *
           * @return The acecssor that provides access to the complex content that is identified by Destination.
           */
         virtual uci::type::GeoLocatedObjectType& getDestination()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Destination to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Destination
           */
         virtual void setDestination(const uci::type::GeoLocatedObjectType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Relationship.
           *
           * @return The acecssor that provides access to the complex content that is identified by Relationship.
           */
         virtual const uci::type::RelationshipType& getRelationship() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Relationship.
           *
           * @return The acecssor that provides access to the complex content that is identified by Relationship.
           */
         virtual uci::type::RelationshipType& getRelationship()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Relationship to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Relationship
           */
         virtual void setRelationship(const uci::type::RelationshipType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Relationship exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Relationship is emabled or not
           */
         virtual bool hasRelationship() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Relationship
           *
           * @param type = uci::type::accessorType::relationshipType This Accessor's accessor type
           */
         virtual void enableRelationship(uci::base::accessorType::AccessorType type = uci::type::accessorType::relationshipType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Relationship */
         virtual void clearRelationship()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RelationshipDesignationMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RelationshipDesignationMDT to copy from
           */
         RelationshipDesignationMDT(const RelationshipDesignationMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RelationshipDesignationMDT to the contents of the
           * RelationshipDesignationMDT on the right hand side (rhs) of the assignment operator.RelationshipDesignationMDT [only
           * available to derived classes]
           *
           * @param rhs The RelationshipDesignationMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::RelationshipDesignationMDT
           * @return A constant reference to this RelationshipDesignationMDT.
           */
         const RelationshipDesignationMDT& operator=(const RelationshipDesignationMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RelationshipDesignationMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RelationshipDesignationMDT_h

