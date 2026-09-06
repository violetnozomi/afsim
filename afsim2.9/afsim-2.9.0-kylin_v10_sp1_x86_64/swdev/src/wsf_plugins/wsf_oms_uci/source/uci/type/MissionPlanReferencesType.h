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
#ifndef Uci__Type__MissionPlanReferencesType_h
#define Uci__Type__MissionPlanReferencesType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PathID_Type_h)
# include "uci/type/PathID_Type.h"
#endif

#if !defined(Uci__Type__SegmentID_Type_h)
# include "uci/type/SegmentID_Type.h"
#endif

#if !defined(Uci__Type__PathConstraintsType_h)
# include "uci/type/PathConstraintsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** References an existing Mission Plan, or parts thereof. */
      class MissionPlanReferencesType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MissionPlanReferencesType()
         { }

         /** Returns this accessor's type constant, i.e. MissionPlanReferencesType
           *
           * @return This accessor's type constant, i.e. MissionPlanReferencesType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionPlanReferencesType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionPlanReferencesType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique ID of a Path within the MissionPlan that is being referenced. This element is used if only
           * specific Paths are being referenced, excluding others. If specified, all Segments of the Path are being referenced,
           * in addition to any given by the sibling SegmentID. If this element and the sibling SegmentID are not specified, then
           * all Paths and Segments of the MissionPlan are included in the reference. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PathID_Type, uci::type::accessorType::pathID_Type> PathID;

         /** Indicates the unique ID of a Segment within the MissionPlan that is being referenced in addition to any Segments
           * included in the sibling PathID. This element is used to reference specific Segments of a Path, excluding others in
           * the same Path. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SegmentID_Type, uci::type::accessorType::segmentID_Type> SegmentID;

         /** Further constrain a Mission Plan to a Time Window or list of specific segments. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PathConstraintsType, uci::type::accessorType::pathConstraintsType> Constraints;

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


         /** Returns the bounded list that is identified by the PathID.
           *
           * @return The bounded list identified by PathID.
           */
         virtual const uci::type::MissionPlanReferencesType::PathID& getPathID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PathID.
           *
           * @return The bounded list identified by PathID.
           */
         virtual uci::type::MissionPlanReferencesType::PathID& getPathID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PathID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPathID(const uci::type::MissionPlanReferencesType::PathID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SegmentID.
           *
           * @return The bounded list identified by SegmentID.
           */
         virtual const uci::type::MissionPlanReferencesType::SegmentID& getSegmentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SegmentID.
           *
           * @return The bounded list identified by SegmentID.
           */
         virtual uci::type::MissionPlanReferencesType::SegmentID& getSegmentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SegmentID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSegmentID(const uci::type::MissionPlanReferencesType::SegmentID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Constraints.
           *
           * @return The bounded list identified by Constraints.
           */
         virtual const uci::type::MissionPlanReferencesType::Constraints& getConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Constraints.
           *
           * @return The bounded list identified by Constraints.
           */
         virtual uci::type::MissionPlanReferencesType::Constraints& getConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Constraints.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setConstraints(const uci::type::MissionPlanReferencesType::Constraints& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionPlanReferencesType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionPlanReferencesType to copy from
           */
         MissionPlanReferencesType(const MissionPlanReferencesType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionPlanReferencesType to the contents of the
           * MissionPlanReferencesType on the right hand side (rhs) of the assignment operator.MissionPlanReferencesType [only
           * available to derived classes]
           *
           * @param rhs The MissionPlanReferencesType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::MissionPlanReferencesType
           * @return A constant reference to this MissionPlanReferencesType.
           */
         const MissionPlanReferencesType& operator=(const MissionPlanReferencesType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionPlanReferencesType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionPlanReferencesType_h

