// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtoCallback.hpp"

#include <algorithm>

UtoCallbackBasicStorage::UtoCallbackBasicStorage()
   : busyEvent_(-1)
   , removed_(false)
{
}

UtoCallbackBasicStorage::~UtoCallbackBasicStorage()
{
   // clear all the data
   clear();
}

int UtoCallbackBasicStorage::add(int n, Handler* handler)
{
   HandlerMap::iterator it = handlers_.find(n);
   if (it == handlers_.end())
   {
      it = handlers_.insert(HandlerMap::value_type(n, HandlerVec())).first;
   }
   // check that the <handler> is not already in the vector
   HandlerVec& vec = it->second;
   if (std::find(vec.begin(), vec.end(), handler) == vec.end())
   {
      vec.push_back(handler);
      return 0;
   }

   return -1;
}

int UtoCallbackBasicStorage::remove(int n, Handler* handler)
{
   int                  ret = -1; // default to error
   HandlerMap::iterator it  = handlers_.find(n);
   if (it != handlers_.end())
   {
      // if called while an event is 'busy' then simply NULL the handler.
      if (busyEvent_ == n)
      {
         removed_ = true;
         HandlerVec::iterator hdlr_it, end(it->second.end());
         for (hdlr_it = it->second.begin(); hdlr_it != end; ++hdlr_it)
         {
            // compare handlers to find match.
            if (handler->isEqual(*hdlr_it))
            {
               delete (*hdlr_it);
               *hdlr_it = nullptr;

               ret = 0;
               // only one connection removed, so all done
               break;
            }
         }
      }

      // otherwise the event is not busy so remove the handler.
      else
      {
         HandlerVec::iterator hdlr_it, end(it->second.end());
         for (hdlr_it = it->second.begin(); hdlr_it != end;)
         {
            // compare handlers to find match.
            if (handler->isEqual(*hdlr_it))
            {
               delete (*hdlr_it);
               it->second.erase(hdlr_it++);

               ret = 0;
               // only one connection removed, so all done
               break;
            }
            else
               ++hdlr_it;
         }
      }
   }

   return ret;
}

int UtoCallbackBasicStorage::send(int n, void* data)
{
   // check for busy event and prevent nested callbacks (for now...)
   if (busyEvent_ != -1)
      return -1;

   HandlerMap::iterator it = handlers_.find(n);
   if (it != handlers_.end())
   {
      busyEvent_ = n;
      HandlerVec::iterator hdlr_it, end(it->second.end());
      for (hdlr_it = it->second.begin(); hdlr_it != end; ++hdlr_it)
      {
         // check for NULL handler
         if (*hdlr_it)
         {
            (**hdlr_it)(data);
         }
      }

      // remove any handlers that were set to NULL.
      if (removed_)
      {
         it->second.erase(std::remove(it->second.begin(), it->second.end(), (Utok::Handler*)nullptr), it->second.end());
         removed_ = false;
      }
      busyEvent_ = -1;
   }

   return 0;
}

int UtoCallbackBasicStorage::clear()
{
   for (HandlerMap::iterator itBegin = handlers_.begin(), itEnd = handlers_.end(); itBegin != itEnd; ++itBegin)
   {
      // clear all the vector data
      for (HandlerVec::iterator itVecBegin = itBegin->second.begin(), itVecEnd = itBegin->second.end();
           itVecBegin != itVecEnd;
           ++itVecBegin)
      {
         // release the handler
         delete *itVecBegin;
      }
   }

   // clear the map
   handlers_.clear();

   return 0;
}
