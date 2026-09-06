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

#if !defined(_UTOCALLBACK_HPP_)
#define _UTOCALLBACK_HPP_

#ifdef _WIN32
#pragma warning(disable : 4786)
#endif //  _WIN32

#include <map>
#include <memory>
#include <vector>

namespace Utok
{
////////////////////////////////////////////////////////
// class Handler
class Handler
{
public:
   virtual ~Handler() {}
   virtual void operator()(void* data)  = 0;
   virtual bool isEqual(Handler*) const = 0;
};

////////////////////////////////////////////////////////
// template Handler_fun_t
template<typename Fn>
class Handler_fun_t : public Utok::Handler
{
public:
   typedef Handler_fun_t<Fn> myType;
   Handler_fun_t(void* data, Fn o)
      : data_(data)
      , op_(o)
   {
   }
   void operator()(void* data) override { op_(data_); }
   bool isEqual(Utok::Handler* h) const override
   {
      myType* myt = dynamic_cast<myType*>(h);
      if (myt == nullptr)
         return false;
      return ((data_ == myt->data_) && (op_ == myt->op_));
   }
   void* data_;
   Fn    op_;
};

inline Utok::Handler* makeHandler(void* p, void (*f)(void*))
{
   return new Handler_fun_t<void (*)(void*)>(p, f);
}

////////////////////////////////////////////////////////
// template Handler_fun1_t
template<typename P1>
class Handler_fun1_t : public Utok::Handler
{
public:
   typedef Handler_fun1_t<P1> myType;
   explicit Handler_fun1_t(void* p, void (*o)(void*, P1))
      : data_(p)
      , op_(o)
   {
   }
   void operator()(void* data) override { op_(data_, (P1)data); }
   bool isEqual(Utok::Handler* h) const override
   {
      myType* myt = dynamic_cast<myType*>(h);
      if (myt == 0)
         return false;
      return ((data_ == myt->data_) && (op_ == myt->op_));
   }
   void* data_;
   void (*op_)(void*, P1); // the function pointer
};

template<typename P1>
Utok::Handler* makeHandler(void* o, void (*f)(P1))
{
   return new Handler_fun1_t<P1>(o, f);
}

////////////////////////////////////////////////////////
// template Handler_fun2_t
template<typename P1, typename P2>
class Handler_fun2_t : public Utok::Handler
{
public:
   typedef Handler_fun2_t<P1, P2> myType;
   struct ParamType
   {
      P1 first;
      P2 second;
   };
   Handler_fun2_t(void* p, void (*o)(void*, P1, P2))
      : data_(p)
      , op_(o)
   {
   }
   void operator()(void* data) override
   {
      ParamType* p = (ParamType*)data;
      op_(data_, p->first, p->second);
   }
   bool isEqual(Utok::Handler* h) const override
   {
      myType* myt = dynamic_cast<myType*>(h);
      if (myt == 0)
         return false;
      return ((data_ == myt->data_) && (op_ == myt->op_));
   }
   void* data_;
   void (*op_)(void*, P1, P2); // the function pointer
};

template<typename P1, typename P2>
Utok::Handler* makeHandler(void* o, void (*f)(P1, P2))
{
   return new Handler_fun2_t<P1, P2>(o, f);
}

////////////////////////////////////////////////////////
// template Handler_fun3_t
template<typename P1, typename P2, typename P3>
class Handler_fun3_t : public Utok::Handler
{
public:
   typedef Handler_fun3_t<P1, P2, P3> myType;
   struct ParamType
   {
      P1 first;
      P2 second;
      P3 third;
   };
   Handler_fun3_t(void* p, void (*o)(void*, P1, P2, P3))
      : data_(p)
      , op_(o)
   {
   }
   void operator()(void* data) override
   {
      ParamType* p = (ParamType*)data;
      op_(data_, p->first, p->second, p->third);
   }
   bool isEqual(Utok::Handler* h) const override
   {
      myType* myt = dynamic_cast<myType*>(h);
      if (myt == 0)
         return false;
      return ((data_ == myt->data_) && (op_ == myt->op_));
   }
   void* data_;
   void (*op_)(void*, P1, P2, P3); // the function pointer
};

template<typename P1, typename P2, typename P3>
Utok::Handler* makeHandler(void* o, void (*f)(P1, P2, P3))
{
   return new Handler_fun3_t<P1, P2, P3>(o, f);
}

////////////////////////////////////////////////////////
// template Handler_memfun_t
template<typename U>
class Handler_memfun_t : public Utok::Handler
{
public:
   typedef Handler_memfun_t<U> myType;
   Handler_memfun_t(U* o, void (U::*f)())
      : obj_(o)
      , memfun_(f)
   {
   }
   void operator()(void* data) override { (obj_->*memfun_)(); }
   bool isEqual(Utok::Handler* h) const override
   {
      myType* myt = dynamic_cast<myType*>(h);
      if (myt == nullptr)
         return false;
      return ((obj_ == myt->obj_) && (memfun_ == myt->memfun_));
   }
   U* obj_;
   void (U::*memfun_)(); // the member function pointer
};

template<typename U>
Utok::Handler* makeHandler(U* o, void (U::*f)())
{
   return new Handler_memfun_t<U>(o, f);
}

////////////////////////////////////////////////////////
// template Handler_memfun1_t
template<class U, typename P1>
class Handler_memfun1_t : public Utok::Handler
{
public:
   typedef Handler_memfun1_t<U, P1> myType;
   Handler_memfun1_t(U* o, void (U::*f)(P1))
      : obj_(o)
      , memfun_(f)
   {
   }
   void operator()(void* data) override { (obj_->*memfun_)((P1)data); }
   bool isEqual(Utok::Handler* h) const override
   {
      myType* myt = dynamic_cast<myType*>(h);
      if (myt == nullptr)
         return false;
      return ((obj_ == myt->obj_) && (memfun_ == myt->memfun_));
   }
   U* obj_;
   void (U::*memfun_)(P1); // the member function pointer
};

template<typename U, typename P1>
Utok::Handler* makeHandler(U* o, void (U::*f)(P1))
{
   return new Handler_memfun1_t<U, P1>(o, f);
}

////////////////////////////////////////////////////////
// template Handler_memfun2_t
template<typename U, typename P1, typename P2>
class Handler_memfun2_t : public Utok::Handler
{
public:
   typedef Handler_memfun2_t<U, P1, P2> myType;
   struct ParamType
   {
      P1 first;
      P2 second;
   };
   Handler_memfun2_t(U* o, void (U::*f)(P1, P2))
      : obj_(o)
      , memfun_(f)
   {
   }
   void operator()(void* data) override
   {
      ParamType* p = (ParamType*)data;
      (obj_->*memfun_)(p->first, p->second);
   }
   bool isEqual(Utok::Handler* h) const override
   {
      myType* myt = dynamic_cast<myType*>(h);
      if (myt == 0)
         return false;
      return ((obj_ == myt->obj_) && (memfun_ == myt->memfun_));
   }
   U* obj_;
   void (U::*memfun_)(P1, P2); // the member function pointer
};

template<typename U, typename P1, typename P2>
Utok::Handler* makeHandler(U* o, void (U::*f)(P1, P2))
{
   return new Handler_memfun2_t<U, P1, P2>(o, f);
}

////////////////////////////////////////////////////////
// template Handler_memfun3_t
template<typename U, typename P1, typename P2, typename P3>
class Handler_memfun3_t : public Utok::Handler
{
public:
   typedef Handler_memfun3_t<U, P1, P2, P3> myType;
   struct ParamType
   {
      P1 first;
      P2 second;
      P3 third;
   };
   Handler_memfun3_t(U* o, void (U::*f)(P1, P2, P3))
      : obj_(o)
      , memfun_(f)
   {
   }
   void operator()(void* data) override
   {
      ParamType* p = (ParamType*)data;
      (obj_->*memfun_)(p->first, p->second, p->third);
   }
   bool isEqual(Utok::Handler* h) const override
   {
      myType* myt = dynamic_cast<myType*>(h);
      if (myt == 0)
         return false;
      return ((obj_ == myt->obj_) && (memfun_ == myt->memfun_));
   }
   U* obj_;
   void (U::*memfun_)(P1, P2, P3); // the member function pointer
};

template<typename U, typename P1, typename P2, typename P3>
Utok::Handler* makeHandler(U* o, void (U::*f)(P1, P2, P3))
{
   return new Handler_memfun3_t<U, P1, P2, P3>(o, f);
}
} // namespace Utok

class UtoCallbackBasicStorage;
////////////////////////////////////////////////////////
// template UtoCallback
template<class T, class Storage = UtoCallbackBasicStorage>
class UtoCallback : protected Storage
{
public:
   typedef std::pair<int, Utok::Handler*> handle_type;

   /* Names dependent on template parameters are bound when the template
    * is parsed, names not dependent on template paramters are looked up
    * at instantiation.  Non-dependent names will be resolved without
    * templates, in this case any base class names.  typedef and using will
    * scope those base class names so they will be dependent on template
    * parameters and looked up at instantiation.
    * http://gcc.gnu.org/onlinedocs/gcc/Name-lookup.html
    */
   using Storage::add;
   using Storage::remove;
   using Storage::send;

   template<int n>
   struct Event0
   {
      typedef void ParamType;
      enum
      {
         VALUE = n
      };
   };

   template<int n, typename P1>
   struct Event1
   {
      typedef P1 ParamType;
      enum
      {
         VALUE = n
      };
   };

   template<int n, typename P1, typename P2>
   struct Event2
   {
      struct ParamType
      {
         P1 first;
         P2 second;
      };
      enum
      {
         VALUE = n
      };
   };

   template<int n, typename P1, typename P2, typename P3>
   struct Event3
   {
      struct ParamType
      {
         P1 first;
         P2 second;
         P3 third;
      };
      enum
      {
         VALUE = n
      };
   };

   template<int n, typename U>
   int connect(Event0<n>, U* obj, void (U::*fn)())
   {
      return add(n, Utok::makeHandler(obj, fn));
   }

   template<int n, typename P1, typename U>
   int connect(Event1<n, P1>, U* obj, void (U::*fn)(P1))
   {
      return add(n, Utok::makeHandler(obj, fn));
   }

   template<int n, typename P1, typename P2, typename U>
   int connect(Event2<n, P1, P2>, U* obj, void (U::*fn)(P1, P2))
   {
      return add(n, Utok::makeHandler(obj, fn));
   }

   template<int n, typename P1, typename P2, typename P3, typename U>
   int connect(Event3<n, P1, P2, P3>, U* obj, void (U::*fn)(P1, P2, P3))
   {
      return add(n, Utok::makeHandler(obj, fn));
   }

   template<int n>
   int connect(Event0<n>, void* data, void (*fn)(void*))
   {
      return add(n, Utok::makeHandler(data, fn));
   }

   template<int n, typename P1>
   int connect(Event1<n, P1>, void* data, void (*fn)(void*, P1))
   {
      return add(n, Utok::makeHandler(data, fn));
   }

   template<int n, typename P1, typename P2>
   int connect(Event2<n, P1, P2>, void* data, void (*fn)(void*, P1, P2))
   {
      return add(n, Utok::makeHandler(data, fn));
   }

   template<int n, typename P1, typename P2, typename P3>
   int connect(Event3<n, P1, P2, P3>, void* data, void (*fn)(void*, P1, P2, P3))
   {
      return add(n, Utok::makeHandler(data, fn));
   }

   template<int n, typename U>
   int disconnect(Event0<n>, U* obj, void (U::*fn)())
   {
      std::unique_ptr<Utok::Handler> h(Utok::makeHandler(obj, fn));
      return remove(n, h.get());
   }

   template<int n, typename P1, typename U>
   int disconnect(Event1<n, P1>, U* obj, void (U::*fn)(P1))
   {
      std::unique_ptr<Utok::Handler> h(Utok::makeHandler(obj, fn));
      return remove(n, h.get());
   }

   template<int n, typename P1, typename P2, typename U>
   int disconnect(Event2<n, P1, P2>, U* obj, void (U::*fn)(P1, P2))
   {
      std::unique_ptr<Utok::Handler> h(Utok::makeHandler(obj, fn));
      return remove(n, h.get());
   }

   template<int n, typename P1, typename P2, typename P3, typename U>
   int disconnect(Event3<n, P1, P2, P3>, U* obj, void (U::*fn)(P1, P2, P3))
   {
      std::unique_ptr<Utok::Handler> h(Utok::makeHandler(obj, fn));
      return remove(n, h.get());
   }

   template<int n>
   int disconnect(Event0<n>, void* data, void (*fn)(void*))
   {
      std::unique_ptr<Utok::Handler> h(Utok::makeHandler(data, fn));
      return remove(n, h.get());
   }

   template<int n, typename P1>
   int disconnect(Event1<n, P1>, void* data, void (*fn)(void*, P1))
   {
      std::unique_ptr<Utok::Handler> h(Utok::makeHandler(data, fn));
      return remove(n, h.get());
   }

   template<int n, typename P1, typename P2>
   int disconnect(Event2<n, P1, P2>, void* data, void (*fn)(void*, P1, P2))
   {
      std::unique_ptr<Utok::Handler> h(Utok::makeHandler(data, fn));
      return remove(n, h.get());
   }

   template<int n, typename P1, typename P2, typename P3>
   int disconnect(Event3<n, P1, P2, P3>, void* data, void (*fn)(void*, P1, P2, P3))
   {
      std::unique_ptr<Utok::Handler> h(Utok::makeHandler(data, fn));
      return remove(n, h.get());
   }

protected:
   UtoCallback() {}
   virtual ~UtoCallback() {}

   template<int n>
   int sendEvent(Event0<n>)
   {
      return send(n, nullptr);
   }

   template<int n, typename P>
   int sendEvent(Event1<n, P>, P p1)
   {
      return send(n, (void*)p1);
   }

   template<int n, typename P1, typename P2>
   int sendEvent(Event2<n, P1, P2>, P1 p1, P2 p2)
   {
      typedef Event2<n, P1, P2> E;
      typename E::ParamType     pt;
      pt.first  = p1;
      pt.second = p2;
      return send(n, (void*)&pt);
   }

   template<int n, typename P1, typename P2, typename P3>
   int sendEvent(Event3<n, P1, P2, P3>, P1 p1, P2 p2, P3 p3)
   {
      typedef Event3<n, P1, P2, P3> E;
      typename E::ParamType         pt;
      pt.first  = p1;
      pt.second = p2;
      pt.third  = p3;
      return send(n, (void*)&pt);
   }
};

////////////////////////////////////////////////////////
// class UtoCallbackBasicStorage
class UtoCallbackBasicStorage
{
public:
   typedef Utok::Handler Handler;

   UtoCallbackBasicStorage();
   ~UtoCallbackBasicStorage();

   int add(int n, Handler* handler);
   int remove(int n, Handler* handler);
   int send(int n, void* data);
   int clear();

protected:
private:
   typedef std::vector<Handler*>     HandlerVec;
   typedef std::map<int, HandlerVec> HandlerMap;

   HandlerMap handlers_;
   int        busyEvent_;
   bool       removed_;
};

#if 0
// Usage:
class foo : public UtoCallback<foo, BasicPolicy> {
   typedef UtoCallback::Event1<1,int> Event1;
   typedef UtoCallback::Event1<2,foo*> Event2;
   ...
   void someFunc() {
      sendEvent(Event1(), 9999);
      sendEvent(Event2(), this);
   }
} a;

class bar {
   void bar() {
      a.connect(foo::Event1(), this, &bar::f);
   }
   void ~bar() {
      a.disconnect(foo::Event1(), this, &bar::f);
   }

   void f(foo* obj);
};
#endif

#endif
