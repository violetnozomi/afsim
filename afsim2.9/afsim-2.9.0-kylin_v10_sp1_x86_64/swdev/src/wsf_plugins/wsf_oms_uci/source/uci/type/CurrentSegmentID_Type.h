// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__CurrentSegmentID_Type_h
#define Uci__Type__CurrentSegmentID_Type_h 1

#if !defined(Uci__Type__SegmentID_Type_h)
# include "uci/type/SegmentID_Type.h"
#endif

#if !defined(Uci__Type__LoiterProgressType_h)
# include "uci/type/LoiterProgressType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CurrentSegmentID_Type sequence accessor class */
      class CurrentSegmentID_Type : public virtual uci::type::SegmentID_Type {
      public:

         /** The destructor */
         virtual ~CurrentSegmentID_Type()
         { }

         /** Returns this accessor's type constant, i.e. CurrentSegmentID_Type
           *
           * @return This accessor's type constant, i.e. CurrentSegmentID_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::currentSegmentID_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CurrentSegmentID_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LoiterProgress.
           *
           * @return The acecssor that provides access to the complex content that is identified by LoiterProgress.
           */
         virtual const uci::type::LoiterProgressType& getLoiterProgress() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LoiterProgress.
           *
           * @return The acecssor that provides access to the complex content that is identified by LoiterProgress.
           */
         virtual uci::type::LoiterProgressType& getLoiterProgress()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LoiterProgress to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LoiterProgress
           */
         virtual void setLoiterProgress(const uci::type::LoiterProgressType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LoiterProgress exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LoiterProgress is emabled or not
           */
         virtual bool hasLoiterProgress() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LoiterProgress
           *
           * @param type = uci::type::accessorType::loiterProgressType This Accessor's accessor type
           */
         virtual void enableLoiterProgress(uci::base::accessorType::AccessorType type = uci::type::accessorType::loiterProgressType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LoiterProgress */
         virtual void clearLoiterProgress()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CurrentSegmentID_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CurrentSegmentID_Type to copy from
           */
         CurrentSegmentID_Type(const CurrentSegmentID_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CurrentSegmentID_Type to the contents of the CurrentSegmentID_Type
           * on the right hand side (rhs) of the assignment operator.CurrentSegmentID_Type [only available to derived classes]
           *
           * @param rhs The CurrentSegmentID_Type on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::CurrentSegmentID_Type
           * @return A constant reference to this CurrentSegmentID_Type.
           */
         const CurrentSegmentID_Type& operator=(const CurrentSegmentID_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CurrentSegmentID_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CurrentSegmentID_Type_h

