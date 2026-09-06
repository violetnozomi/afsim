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
#ifndef Uci__Type__MissionPlanActionReferenceType_h
#define Uci__Type__MissionPlanActionReferenceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Type__ActionID_Type_h)
# include "uci/type/ActionID_Type.h"
#endif

#if !defined(Uci__Type__SegmentID_Type_h)
# include "uci/type/SegmentID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionPlanActionReferenceType sequence accessor class */
      class MissionPlanActionReferenceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MissionPlanActionReferenceType()
         { }

         /** Returns this accessor's type constant, i.e. MissionPlanActionReferenceType
           *
           * @return This accessor's type constant, i.e. MissionPlanActionReferenceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionPlanActionReferenceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionPlanActionReferenceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual const uci::type::MissionPlanID_Type& getMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual uci::type::MissionPlanID_Type& getMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionPlanID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionPlanID
           */
         virtual void setMissionPlanID(const uci::type::MissionPlanID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActionID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActionID.
           */
         virtual const uci::type::ActionID_Type& getActionID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActionID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActionID.
           */
         virtual uci::type::ActionID_Type& getActionID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ActionID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ActionID
           */
         virtual void setActionID(const uci::type::ActionID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActionID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActionID is emabled or not
           */
         virtual bool hasActionID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActionID
           *
           * @param type = uci::type::accessorType::actionID_Type This Accessor's accessor type
           */
         virtual void enableActionID(uci::base::accessorType::AccessorType type = uci::type::accessorType::actionID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActionID */
         virtual void clearActionID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PathSegmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by PathSegmentID.
           */
         virtual const uci::type::SegmentID_Type& getPathSegmentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PathSegmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by PathSegmentID.
           */
         virtual uci::type::SegmentID_Type& getPathSegmentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PathSegmentID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PathSegmentID
           */
         virtual void setPathSegmentID(const uci::type::SegmentID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PathSegmentID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PathSegmentID is emabled or not
           */
         virtual bool hasPathSegmentID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PathSegmentID
           *
           * @param type = uci::type::accessorType::segmentID_Type This Accessor's accessor type
           */
         virtual void enablePathSegmentID(uci::base::accessorType::AccessorType type = uci::type::accessorType::segmentID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PathSegmentID */
         virtual void clearPathSegmentID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionPlanActionReferenceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionPlanActionReferenceType to copy from
           */
         MissionPlanActionReferenceType(const MissionPlanActionReferenceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionPlanActionReferenceType to the contents of the
           * MissionPlanActionReferenceType on the right hand side (rhs) of the assignment operator.MissionPlanActionReferenceType
           * [only available to derived classes]
           *
           * @param rhs The MissionPlanActionReferenceType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::MissionPlanActionReferenceType
           * @return A constant reference to this MissionPlanActionReferenceType.
           */
         const MissionPlanActionReferenceType& operator=(const MissionPlanActionReferenceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionPlanActionReferenceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionPlanActionReferenceType_h

