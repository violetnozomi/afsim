// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Base__Reader_h
#define Uci__Base__Reader_h 1

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all base data types are declared
   namespace base {

      /** The Reader base class provides a mechanism to group reader objects into collections. */
      class Reader {
      public:

         /** The destructor */
         virtual ~Reader()
         { }


      protected:

         /** The constructor [only available to derived classes]. */
         Reader()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The Reader to copy from
           */
         Reader(const Reader& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this Reader to the contents of the Reader on the right hand side (rhs)
           * of the assignment operator.Reader [only available to derived classes]
           *
           * @param rhs The Reader on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::base::Reader
           * @return A constant reference to this Reader.
           */
         const Reader& operator=(const Reader& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // Reader


   } // Namespace: base
} // Namespace: uci

#endif // Uci__Base__Reader_h

