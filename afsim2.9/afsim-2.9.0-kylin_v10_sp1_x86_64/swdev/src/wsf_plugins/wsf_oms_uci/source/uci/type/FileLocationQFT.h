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
#ifndef Uci__Type__FileLocationQFT_h
#define Uci__Type__FileLocationQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FileLocationID_Type_h)
# include "uci/type/FileLocationID_Type.h"
#endif

#if !defined(Uci__Type__FileMetadataID_Type_h)
# include "uci/type/FileMetadataID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the FileLocationQFT sequence accessor class */
      class FileLocationQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~FileLocationQFT()
         { }

         /** Returns this accessor's type constant, i.e. FileLocationQFT
           *
           * @return This accessor's type constant, i.e. FileLocationQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::fileLocationQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FileLocationQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FileLocationID_Type, uci::type::accessorType::fileLocationID_Type> FileLocationID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FileMetadataID_Type, uci::type::accessorType::fileMetadataID_Type> FileMetadataID;

         /** Returns the bounded list that is identified by the FileLocationID.
           *
           * @return The bounded list identified by FileLocationID.
           */
         virtual const uci::type::FileLocationQFT::FileLocationID& getFileLocationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FileLocationID.
           *
           * @return The bounded list identified by FileLocationID.
           */
         virtual uci::type::FileLocationQFT::FileLocationID& getFileLocationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FileLocationID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFileLocationID(const uci::type::FileLocationQFT::FileLocationID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FileMetadataID.
           *
           * @return The bounded list identified by FileMetadataID.
           */
         virtual const uci::type::FileLocationQFT::FileMetadataID& getFileMetadataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FileMetadataID.
           *
           * @return The bounded list identified by FileMetadataID.
           */
         virtual uci::type::FileLocationQFT::FileMetadataID& getFileMetadataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FileMetadataID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFileMetadataID(const uci::type::FileLocationQFT::FileMetadataID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FileLocationQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FileLocationQFT to copy from
           */
         FileLocationQFT(const FileLocationQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FileLocationQFT to the contents of the FileLocationQFT on the
           * right hand side (rhs) of the assignment operator.FileLocationQFT [only available to derived classes]
           *
           * @param rhs The FileLocationQFT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::FileLocationQFT
           * @return A constant reference to this FileLocationQFT.
           */
         const FileLocationQFT& operator=(const FileLocationQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FileLocationQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FileLocationQFT_h

