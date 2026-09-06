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

#ifndef GEN_I_FACTORY_HPP
#define GEN_I_FACTORY_HPP

/// \file  GenIFactory.hpp
/// \brief GenIFactory
/// \class GenIFactory GenIFactory.hpp
/// \brief This defines and interface for a message Factory.
///
/// The derived class should be able to create messages from a GenI stream via the virtual Create() method.\n
///
/// A message framework that inherits from GenMsg will work nicely with this class, however,
/// this class is a template, so any message framework can be used.  The only requirement
/// is the message framework have a function called "HandleReadError()" which is typically
/// used to print debug info from the message should there be a problem reading the message.

#include "GenI.hpp"
#include "UtLog.hpp"

template<typename MsgType>
class GenIFactory
{
public:
   /// Constructor
   GenIFactory();

   /// Destructor
   virtual ~GenIFactory();

   /// Get a message from a GenI stream.
   /// \param genIA A GenI derived class.
   /// \return A pointer to a user message. <b>The user is responsible for the pointer</b>
   MsgType* GetMsg(GenI& genIA);

private:
   /// This function must be defined by the deriving class.
   /// \param genIA A GenI derived class.
   /// \return A pointer to a user message.
   virtual MsgType* Create(GenI& genIA) = 0;
};

template<class MsgType>
GenIFactory<MsgType>::GenIFactory()
{
}

// Virtual
template<class MsgType>
GenIFactory<MsgType>::~GenIFactory()
{
}

template<class MsgType>
MsgType* GenIFactory<MsgType>::GetMsg(GenI& genIA)
{
   // Let deriving class create the message
   MsgType* ptr = Create(genIA);

   // Look for errors
   if (genIA.GetInputStatus() != GenBuf::NoError || ptr == nullptr)
   {
      if (ptr != nullptr)
      {
         // Give the message class a chance to write any error info
         auto out = ut::log::error() << "GenIFactory: Input status is not NoError.";
         ptr->HandleReadError(out);
      }

      delete ptr;
      ptr = nullptr;
      genIA.SetGetOffset(0, GenBuf::FromDataEnd);
   }
   return ptr;
}


#endif
