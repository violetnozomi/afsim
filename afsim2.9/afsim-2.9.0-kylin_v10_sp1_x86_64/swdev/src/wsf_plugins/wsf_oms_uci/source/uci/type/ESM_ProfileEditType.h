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
#ifndef Uci__Type__ESM_ProfileEditType_h
#define Uci__Type__ESM_ProfileEditType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MDF_CommandType_h)
# include "uci/type/MDF_CommandType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ESM_ProfileEntryType_h)
# include "uci/type/ESM_ProfileEntryType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** ESM Profile for the current MDF contains Emitter Priority Bin and activation for each emitter. This structure allows
        * controlling the entries in the ESM Profile, only for the current MDF.
        */
      class ESM_ProfileEditType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ESM_ProfileEditType()
         { }

         /** Returns this accessor's type constant, i.e. ESM_ProfileEditType
           *
           * @return This accessor's type constant, i.e. ESM_ProfileEditType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eSM_ProfileEditType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ESM_ProfileEditType& accessor)
            throw(uci::base::UCIException) = 0;


         /** In a single ESM MDF, editing of emitter priority bin and/or activation can occur for multiple element in a message at
           * a time [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ESM_ProfileEntryType, uci::type::accessorType::eSM_ProfileEntryType> ESM_ProfileEntry;

         /** Returns the accessor that provides access to the complex content that is identified by the CurrentMDF.
           *
           * @return The acecssor that provides access to the complex content that is identified by CurrentMDF.
           */
         virtual const uci::type::MDF_CommandType& getCurrentMDF() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CurrentMDF.
           *
           * @return The acecssor that provides access to the complex content that is identified by CurrentMDF.
           */
         virtual uci::type::MDF_CommandType& getCurrentMDF()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CurrentMDF to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CurrentMDF
           */
         virtual void setCurrentMDF(const uci::type::MDF_CommandType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ESM_ProfileEntry.
           *
           * @return The bounded list identified by ESM_ProfileEntry.
           */
         virtual const uci::type::ESM_ProfileEditType::ESM_ProfileEntry& getESM_ProfileEntry() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ESM_ProfileEntry.
           *
           * @return The bounded list identified by ESM_ProfileEntry.
           */
         virtual uci::type::ESM_ProfileEditType::ESM_ProfileEntry& getESM_ProfileEntry()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ESM_ProfileEntry.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setESM_ProfileEntry(const uci::type::ESM_ProfileEditType::ESM_ProfileEntry& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ESM_ProfileEditType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ESM_ProfileEditType to copy from
           */
         ESM_ProfileEditType(const ESM_ProfileEditType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ESM_ProfileEditType to the contents of the ESM_ProfileEditType on
           * the right hand side (rhs) of the assignment operator.ESM_ProfileEditType [only available to derived classes]
           *
           * @param rhs The ESM_ProfileEditType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ESM_ProfileEditType
           * @return A constant reference to this ESM_ProfileEditType.
           */
         const ESM_ProfileEditType& operator=(const ESM_ProfileEditType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ESM_ProfileEditType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ESM_ProfileEditType_h

