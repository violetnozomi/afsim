// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

//! Note: UtBinder.hpp and the STL functional includes must be included outside
//! namespaces by implementing classes, this include method is due to namespaces
//! in different projects.

template<typename Signature>
class VaCallbackN;

template<typename R, typename... Args>
class VaCallbackN<R(Args...)> : public VaCallback
{
public:
   VaCallbackN()
      : VaCallback()
   {
   }
   VaCallbackN(const std::function<R(Args...)>& aFunc)
      : mFunc(aFunc)
   {
   }

   R operator()(Args... args) { return mFunc(args...); }

private:
   std::function<R(Args...)> mFunc;
};

template<typename Signature>
class VaCallbackListN;

//! A callback list.
//! The callback list contains the list of 'subscribers' to the callback list.
template<typename R, typename... Args>
class VaCallbackListN<R(Args...)> : public VaCallbackList
{
public:
   typedef VaCallbackN<R(Args...)> CallbackType;
   VaCallbackListN()
      : VaCallbackList()
   {
   }
   VaCallbackListN(const std::string& aType, const std::string& aName)
      : VaCallbackList(aType, aName)
   {
   }
   VaCallbackListN(const VaCallbackListN<R(Args...)>& aSrc)
      : VaCallbackList(aSrc)
   {
   }

   VaCallbackListN& operator[](VaCallbackGroupId aCallbackGroup)
   {
      VaCallbackList::operator[](aCallbackGroup);
      return *this;
   }

   //! Invoke the operator()
   //! The base class operator() invokes operator() on each subscriber and does not return
   //! any value. If the return type of the subscribers are non-void then the user must
   //! create a different callback function (with a different name, because you can't override
   //! based simply on return type.
   void operator()(Args... args)
   {
      ++mCallbackDepth;

      VaCallbackGroupId callbackGroupId = mCallbackGroupId;
      mCallbackGroupId                  = 0;

      ListType toDisconnect;

      BeginProfile();
      Execute(0, mCallbackList, callbackGroupId, toDisconnect, args...);
      EndProfile();

      --mCallbackDepth; // This must occur before the delayed disconnects.

      if (mCallbackDepth == 0)
      {
         // Process any delayed disconnects.
         for (auto it = toDisconnect.begin(); it != toDisconnect.end(); ++it)
         {
            (*it)->Disconnect();
         }
      }
   }

   void Execute(int aDepth, MapType& aCallbackMap, const VaCallbackGroupId& aCallbackGroupId, ListType& aToDisconnect, Args... args)
   {
      mTempCallbackList.clear();

      // Signal any subscribers that subscribed to this callback when other subscribers where signaled.
      if (!aCallbackMap.empty())
      {
         // No group specified, then send to everyone.
         // Group specified, send to group subscribers and subscribers with no group.
         if (aCallbackGroupId == 0)
         {
            for (auto it = aCallbackMap.begin(); it != aCallbackMap.end(); ++it)
            {
               for (auto jt = it->second.begin(); jt != it->second.end(); ++jt)
               {
                  if (!(*jt)->GetDisconnectFlag())
                  {
                     VaCallback::sCurrentGroupId = aCallbackGroupId;
                     BeginCallbackProfile();
                     static_cast<CallbackType*>(*jt)->operator()(args...);
                     EndCallbackProfile();
                  }
                  else
                  {
                     aToDisconnect.push_back(*jt);
                  }
               }
            }
            // signal everyone
         }
         else
         {
            auto specific = aCallbackMap.find(aCallbackGroupId);
            if (specific != aCallbackMap.end())
            {
               for (auto jt = specific->second.begin(); jt != specific->second.end(); ++jt)
               {
                  if (!(*jt)->GetDisconnectFlag())
                  {
                     VaCallback::sCurrentGroupId = aCallbackGroupId;
                     BeginCallbackProfile();
                     static_cast<CallbackType*>(*jt)->operator()(args...);
                     EndCallbackProfile();
                  }
                  else
                  {
                     aToDisconnect.push_back(*jt);
                  }
               }
            }
            auto unspecific = aCallbackMap.find(0);
            if (unspecific != aCallbackMap.end())
            {
               for (auto jt = unspecific->second.begin(); jt != unspecific->second.end(); ++jt)
               {
                  if (!(*jt)->GetDisconnectFlag())
                  {
                     VaCallback::sCurrentGroupId = aCallbackGroupId;
                     BeginCallbackProfile();
                     static_cast<CallbackType*>(*jt)->operator()(args...);
                     EndCallbackProfile();
                  }
                  else
                  {
                     aToDisconnect.push_back(*jt);
                  }
               }
            }
         }

         // Recurse
         if (aDepth < cMAX_RECURSION)
         {
            MapType tempCallbackList(mTempCallbackList);
            Execute(aDepth + 1, mTempCallbackList, aCallbackGroupId, aToDisconnect, args...);
         }
         else
         {
            auto out = ut::log::error() << "Callback system reached maximum recursion depth.";
            out.AddNote() << "Depth: " << cMAX_RECURSION;
         }
      }
   }

   //! Connect a new subscriber.
   //! @param aFunc The callback object to be subscribed.
   //! @return The callback object.
   CallbackType* Connect(const std::function<R(Args...)>& aFunc) { return Connect(Create(aFunc)); }

   //! Connect a new subscriber.
   //! @param aFunc The callback object to be subscribed.
   //! @return The callback object.
   CallbackType* Connect(const std::function<R(Args...)>& aFunc, VaCallbackGroupId aGroupId)
   {
      CallbackType* cbPtr = Create(aFunc);
      cbPtr->SetGroupId(aGroupId);
      return Connect(cbPtr);
   }

   //! Connect a new subscriber.
   //! @param aFunc The callback object to be subscribed.
   //! @return The callback object (same as aCallbackPtr).
   CallbackType* Connect(CallbackType* aCallbackPtr)
   {
      ConnectP(aCallbackPtr);
      return aCallbackPtr;
   }

   //! Connect a new subscriber.
   //! @param aCallbackPtr The callback object to be subscribed.
   //! @param aGroup The group id that the callback is associated with.
   //! @return The callback object (same as aCallbackPtr).
   //! @note The caller retains ownership of the callback object.
   CallbackType* Connect(CallbackType* aCallbackPtr, VaCallbackGroupId aGroupId)
   {
      aCallbackPtr->SetGroupId(aGroupId);
      ConnectP(aCallbackPtr);
      return aCallbackPtr;
   }


   //! Connect a new subscriber.
   //! @param aFunc The callback object to be subscribed.
   //! @return The callback object.
   template<typename CT>
   CallbackType* Connect(R (CT::*aFunc)(Args...), CT* aObjPtr)
   {
      return Connect(Create(UtStd::Bind(aFunc, aObjPtr)));
   }

   //! Connect a new subscriber.
   //! @param aFunc The callback object to be subscribed.
   //! @return The callback object.
   template<typename CT>
   CallbackType* Connect(R (CT::*aFunc)(Args...), CT* aObjPtr, VaCallbackGroupId aGroupId)
   {
      CallbackType* cbPtr = Create(UtStd::Bind(aFunc, aObjPtr));
      cbPtr->SetGroupId(aGroupId);
      return Connect(cbPtr);
   }

   //! Connect a new subscriber.
   //! @param aFunc The callback object to be subscribed.
   //! @return The callback object.
   CallbackType* Connect(R (*aFunc)(Args...)) { return Connect(Create(aFunc)); }

   //! Connect a new subscriber.
   //! @param aFunc The callback object to be subscribed.
   //! @return The callback object.
   CallbackType* Connect(R (*aFunc)(Args...), VaCallbackGroupId aGroupId)
   {
      CallbackType* cbPtr = Create(aFunc);
      cbPtr->SetGroupId(aGroupId);
      return Connect(cbPtr);
   }

   //! Create a callback object for a member function, but do not connect it to the callback list.
   //! @param aFunc The function to be invoked.
   //! @param aGroupId The group id that the calback is associated with.
   //! @return The created callback object.
   //! @note The caller is the owner of the callback object and responsible for deleting it.
   CallbackType* Create(const std::function<R(Args...)>& aFunc, VaCallbackGroupId aGroupId)
   {
      CallbackType* cbPtr = Create(aFunc);
      cbPtr->SetGroupId(aGroupId);
      return Connect(cbPtr);
   }

   //! Create a callback object for a function, but do not connect it to the callback list.
   //! @param aFunc The function to be invoked.
   //! @return The created callback object.
   //! @note The caller is the owner of the callback object and responsible for deleting it.
   CallbackType* Create(const std::function<R(Args...)>& aFunc)
   {
      CallbackType* callbackPtr = new CallbackType(aFunc);
      return callbackPtr;
   }

   void GetCallbacks(std::vector<CallbackType*>& aCallbacks) const
   {
      for (auto it = mCallbackList.begin(); it != mCallbackList.end(); ++it)
      {
         for (auto jt = it->second.begin(); jt != it->second.end(); ++jt)
         {
            aCallbacks.push_back((CallbackType*)(*jt));
         }
      }
   }
};
