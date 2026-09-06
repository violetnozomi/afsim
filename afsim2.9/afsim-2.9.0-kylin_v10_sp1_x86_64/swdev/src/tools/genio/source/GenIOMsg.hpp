// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************


#ifndef GENIO_MSG_HPP
#define GENIO_MSG_HPP

#include "genio_export.h"

#include "GenMsg.hpp"


class GenI;
class GenO;

class GENIO_EXPORT GenIOMsg : public GenMsg
{
public:
   GenIOMsg(GenI& genIA);
   GenIOMsg(const GenMsg&, GenI& genIA);

   ~GenIOMsg() override;

   // Enumerations

   enum GenIOMsgType
   {
      TypeUnknown    = 0,
      TypeConnect    = 1,
      TypeStatus     = 2,
      TypeUnknownMax = 3
   };

   // Returns the name of the class
   static const char* GetGenIOMsgClassName(GenIOMsgType typeA);

   // Returns the name of the class
   const char* GetGenIOMsgClassName() const;

   // This function is intented to be defined by the user
   // The default for this class will be to return a zero
   int DoIt() override;

   // Accessors

   // Mutators

protected:
   // The constructor and several of the mutators are protected since
   // only a derived class should be constructing the base class or
   // modifying certain key pieces of information.

   GenIOMsg(GenIOMsgType typeA);
   GenIOMsg(double timeA, GenIOMsgType typeA);

private:
   GenIOMsg();

   int GetClassLength() const override;


   void GetClassData(GenI& genIA) override;
   void PutClassData(GenO& genOA) const override;

   // This is virtual because some GenIOMsg's have lengths that can be
   // dynamically updated and there is no other single hook that can
   // give us the length.
   // Derived classes will typically override this functions
   virtual int GetGenIOMsgClassLength() const;

   // Derived classes will typically override these functions
   virtual void GetGenIOMsgClassData(GenI& genIA);
   virtual void PutGenIOMsgClassData(GenO& genOA) const;

   void GetGenIOMsgData(GenI& genIA);

   static const int   GenIOMsgBaseLengthS;
   static const char* classNameS[];
};

// Accessors
inline const char* GenIOMsg::GetGenIOMsgClassName() const
{
   return classNameS[GetType()];
}


// Mutators


#endif
