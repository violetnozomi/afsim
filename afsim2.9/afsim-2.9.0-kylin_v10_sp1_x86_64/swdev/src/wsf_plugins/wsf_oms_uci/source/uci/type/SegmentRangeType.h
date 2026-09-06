// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__SegmentRangeType_h
#define Uci__Type__SegmentRangeType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SegmentID_Type_h)
# include "uci/type/SegmentID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Specify a contiguous range of segments with a path. */
      class SegmentRangeType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SegmentRangeType()
         { }

         /** Returns this accessor's type constant, i.e. SegmentRangeType
           *
           * @return This accessor's type constant, i.e. SegmentRangeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::segmentRangeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SegmentRangeType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BeginSegmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by BeginSegmentID.
           */
         virtual const uci::type::SegmentID_Type& getBeginSegmentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BeginSegmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by BeginSegmentID.
           */
         virtual uci::type::SegmentID_Type& getBeginSegmentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the BeginSegmentID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by BeginSegmentID
           */
         virtual void setBeginSegmentID(const uci::type::SegmentID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BeginSegmentID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BeginSegmentID is emabled or not
           */
         virtual bool hasBeginSegmentID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BeginSegmentID
           *
           * @param type = uci::type::accessorType::segmentID_Type This Accessor's accessor type
           */
         virtual void enableBeginSegmentID(uci::base::accessorType::AccessorType type = uci::type::accessorType::segmentID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BeginSegmentID */
         virtual void clearBeginSegmentID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EndSegmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EndSegmentID.
           */
         virtual const uci::type::SegmentID_Type& getEndSegmentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EndSegmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EndSegmentID.
           */
         virtual uci::type::SegmentID_Type& getEndSegmentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EndSegmentID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EndSegmentID
           */
         virtual void setEndSegmentID(const uci::type::SegmentID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EndSegmentID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EndSegmentID is emabled or not
           */
         virtual bool hasEndSegmentID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EndSegmentID
           *
           * @param type = uci::type::accessorType::segmentID_Type This Accessor's accessor type
           */
         virtual void enableEndSegmentID(uci::base::accessorType::AccessorType type = uci::type::accessorType::segmentID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EndSegmentID */
         virtual void clearEndSegmentID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SegmentRangeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SegmentRangeType to copy from
           */
         SegmentRangeType(const SegmentRangeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SegmentRangeType to the contents of the SegmentRangeType on the
           * right hand side (rhs) of the assignment operator.SegmentRangeType [only available to derived classes]
           *
           * @param rhs The SegmentRangeType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::SegmentRangeType
           * @return A constant reference to this SegmentRangeType.
           */
         const SegmentRangeType& operator=(const SegmentRangeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SegmentRangeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SegmentRangeType_h

