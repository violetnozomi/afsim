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
/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
 */

#ifndef UTOCME_LINEFUNCTOR
#define UTOCME_LINEFUNCTOR 1

#include <iostream>

#include <osg/Notify>
#include <osg/PrimitiveSet>

namespace osg
{


/** Provides access to the lines that compose an \c osg::Drawable. If the \c
 *  Drawable is not composed of lines, the \c TriangleFunctor will convert
 *  the primitives to triangles whenever possible.
 *  <p>Notice that \c LineFunctor is a class template, and that it inherits
 *  from its template parameter \c T. This template parameter must implement
 *  <tt>T::operator() (const osg::Vec3 v1, const osg::Vec3 v2,
 *   bool treatVertexDataAsTemporary)</tt>, which will be called for every
 *  line segment when the functor is applied to a \c Drawable. Parameters \c v1 and \c
 *  v2 are the triangle vertices. The fourth parameter, \c
 *  treatVertexDataAsTemporary, indicates whether these vertices are coming from
 *  a "real" vertex array, or from a temporary vertex array, created by the \c
 *  LineFunctor from some other geometry representation.
 *  @see \c PrimitiveFunctor for general usage hints.
 */
template<class T>
class UtoCmeLineFunctor : public PrimitiveFunctor, public T
{
public:
   UtoCmeLineFunctor()
   {
      _vertexArraySize            = 0;
      _vertexArrayPtr             = nullptr;
      _modeCache                  = 0;
      _treatVertexDataAsTemporary = false;
   }

   ~UtoCmeLineFunctor() override {}

   void setTreatVertexDataAsTemporary(bool treatVertexDataAsTemporary)
   {
      _treatVertexDataAsTemporary = treatVertexDataAsTemporary;
   }
   bool getTreatVertexDataAsTemporary() const { return _treatVertexDataAsTemporary; }

   void setVertexArray(unsigned int, const Vec2*) override
   {
      notify(WARN) << "CmeLine Functor does not support Vec2* vertex arrays" << std::endl;
   }

   void setVertexArray(unsigned int count, const Vec3* vertices) override
   {
      _vertexArraySize = count;
      _vertexArrayPtr  = vertices;
   }

   void setVertexArray(unsigned int, const Vec4*) override
   {
      notify(WARN) << "CmeLine Functor does not support Vec4* vertex arrays" << std::endl;
   }

   void setVertexArray(unsigned int, const Vec2d*) override
   {
      notify(WARN) << "CmeLine Functor does not support Vec2d* vertex arrays" << std::endl;
   }

   void setVertexArray(unsigned int, const Vec3d*) override
   {
      notify(WARN) << "CmeLine Functor does not support Vec3d* vertex arrays" << std::endl;
   }

   void setVertexArray(unsigned int, const Vec4d*) override
   {
      notify(WARN) << "CmeLine Functor does not support Vec4d* vertex arrays" << std::endl;
   }

   void drawArrays(GLenum mode, GLint first, GLsizei count) override
   {
      if (_vertexArrayPtr == nullptr || count == 0)
      {
         notify(WARN) << "Attempting to do an intersection on an empty vertex array" << std::endl;
         return;
      }

      switch (mode)
      {
      case (GL_TRIANGLES):
      {
         const Vec3* vlast = &_vertexArrayPtr[first + count];
         for (const Vec3* vptr = &_vertexArrayPtr[first]; vptr < vlast; vptr += 3)
            this->operator()(*(vptr), *(vptr + 1), *(vptr + 2), _treatVertexDataAsTemporary);
         break;
      }
      case (GL_TRIANGLE_STRIP):
      {
         const Vec3* vptr = &_vertexArrayPtr[first];
         for (GLsizei i = 2; i < count; ++i, ++vptr)
         {
            if ((i % 2))
               this->operator()(*(vptr), *(vptr + 2), *(vptr + 1), _treatVertexDataAsTemporary);
            else
               this->operator()(*(vptr), *(vptr + 1), *(vptr + 2), _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_QUADS):
      {
         const Vec3* vptr = &_vertexArrayPtr[first];
         for (GLsizei i = 3; i < count; i += 4, vptr += 4)
         {
            this->operator()(*(vptr), *(vptr + 1), *(vptr + 2), _treatVertexDataAsTemporary);
            this->operator()(*(vptr), *(vptr + 2), *(vptr + 3), _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_QUAD_STRIP):
      {
         const Vec3* vptr = &_vertexArrayPtr[first];
         for (GLsizei i = 3; i < count; i += 2, vptr += 2)
         {
            this->operator()(*(vptr), *(vptr + 1), *(vptr + 2), _treatVertexDataAsTemporary);
            this->operator()(*(vptr + 1), *(vptr + 3), *(vptr + 2), _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
      case (GL_TRIANGLE_FAN):
      {
         const Vec3* vfirst = &_vertexArrayPtr[first];
         const Vec3* vptr   = vfirst + 1;
         for (GLsizei i = 2; i < count; ++i, ++vptr)
         {
            this->operator()(*(vfirst), *(vptr), *(vptr + 1), _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_LINES):
      {
         const Vec3* vlast = &_vertexArrayPtr[first + count];
         for (const Vec3* vptr = &_vertexArrayPtr[first]; vptr < vlast; vptr += 2)
            this->operator()(*(vptr), *(vptr + 1), _treatVertexDataAsTemporary);
         break;
      }
      case (GL_LINE_STRIP):
      {
         const Vec3* vptr = &_vertexArrayPtr[first];
         for (GLsizei i = 1; i < count; ++i, ++vptr)
            this->operator()(*(vptr), *(vptr + 1), _treatVertexDataAsTemporary);
         break;
      }
      case (GL_LINE_LOOP):
      {
         const Vec3* vfirst = &_vertexArrayPtr[first];
         const Vec3* vptr   = &_vertexArrayPtr[first];
         for (GLsizei i = 1; i < count; ++i, ++vptr)
            this->operator()(*(vptr), *(vptr + 1), _treatVertexDataAsTemporary);
         const Vec3* vlast = &_vertexArrayPtr[first + count - 1];
         this->      operator()(*(vfirst), *(vlast), _treatVertexDataAsTemporary);
         break;
      }
      case (GL_POINTS):
      {
         const Vec3* vlast = &_vertexArrayPtr[first + count];
         for (const Vec3* vptr = &_vertexArrayPtr[first]; vptr < vlast; vptr += 1)
            this->operator()(*(vptr), _treatVertexDataAsTemporary);
         break;
      }
      default:
         break;
      }
   }

   void drawElements(GLenum mode, GLsizei count, const GLubyte* indices) override
   {
      if (indices == nullptr || count == 0)
      {
         notify(WARN) << "Attempting to do an intersection on an empty vertex array" << std::endl;
         return;
      }

      typedef const GLubyte* IndexPointer;

      switch (mode)
      {
      case (GL_TRIANGLES):
      {
         IndexPointer ilast = &indices[count];
         for (IndexPointer iptr = indices; iptr < ilast; iptr += 3)
            this->operator()(_vertexArrayPtr[*iptr],
                             _vertexArrayPtr[*(iptr + 1)],
                             _vertexArrayPtr[*(iptr + 2)],
                             _treatVertexDataAsTemporary);
         break;
      }
      case (GL_TRIANGLE_STRIP):
      {
         IndexPointer iptr = indices;
         for (GLsizei i = 2; i < count; ++i, ++iptr)
         {
            if ((i % 2))
               this->operator()(_vertexArrayPtr[*(iptr)],
                                _vertexArrayPtr[*(iptr + 2)],
                                _vertexArrayPtr[*(iptr + 1)],
                                _treatVertexDataAsTemporary);
            else
               this->operator()(_vertexArrayPtr[*(iptr)],
                                _vertexArrayPtr[*(iptr + 1)],
                                _vertexArrayPtr[*(iptr + 2)],
                                _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_QUADS):
      {
         IndexPointer iptr = indices;
         for (GLsizei i = 3; i < count; i += 4, iptr += 4)
         {
            this->operator()(_vertexArrayPtr[*(iptr)],
                             _vertexArrayPtr[*(iptr + 1)],
                             _vertexArrayPtr[*(iptr + 2)],
                             _treatVertexDataAsTemporary);
            this->operator()(_vertexArrayPtr[*(iptr)],
                             _vertexArrayPtr[*(iptr + 2)],
                             _vertexArrayPtr[*(iptr + 3)],
                             _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_QUAD_STRIP):
      {
         IndexPointer iptr = indices;
         for (GLsizei i = 3; i < count; i += 2, iptr += 2)
         {
            this->operator()(_vertexArrayPtr[*(iptr)],
                             _vertexArrayPtr[*(iptr + 1)],
                             _vertexArrayPtr[*(iptr + 2)],
                             _treatVertexDataAsTemporary);
            this->operator()(_vertexArrayPtr[*(iptr + 1)],
                             _vertexArrayPtr[*(iptr + 3)],
                             _vertexArrayPtr[*(iptr + 2)],
                             _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
      case (GL_TRIANGLE_FAN):
      {
         IndexPointer iptr   = indices;
         const Vec3&  vfirst = _vertexArrayPtr[*iptr];
         ++iptr;
         for (GLsizei i = 2; i < count; ++i, ++iptr)
         {
            this->operator()(vfirst, _vertexArrayPtr[*(iptr)], _vertexArrayPtr[*(iptr + 1)], _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_LINES):
      {
         IndexPointer ilast = &indices[count];
         for (IndexPointer iptr = indices; iptr < ilast; iptr += 2)
            this->operator()(_vertexArrayPtr[*iptr], _vertexArrayPtr[*(iptr + 1)], _treatVertexDataAsTemporary);
         break;
      }
      case (GL_LINE_STRIP):
      {
         IndexPointer iptr = indices;
         for (GLsizei i = 1; i < count; ++i, ++iptr)
            this->operator()(_vertexArrayPtr[*(iptr)], _vertexArrayPtr[*(iptr + 1)], _treatVertexDataAsTemporary);
         break;
      }
      case (GL_LINE_LOOP):
      {
         IndexPointer iptr = indices;
         for (GLsizei i = 1; i < count; ++i, ++iptr)
            this->operator()(_vertexArrayPtr[*(iptr)], _vertexArrayPtr[*(iptr + 1)], _treatVertexDataAsTemporary);
         const Vec3& vfirst = _vertexArrayPtr[*(indices)];
         const Vec3& vlast  = _vertexArrayPtr[*(indices + count - 1)];
         this->      operator()(vfirst, vlast, _treatVertexDataAsTemporary);
         break;
      }
      case (GL_POINTS):
      {
         IndexPointer ilast = &indices[count];
         for (IndexPointer iptr = indices; iptr < ilast; iptr += 1)
            this->operator()(_vertexArrayPtr[*iptr], _treatVertexDataAsTemporary);
         break;
      }
      default:
         break;
      }
   }

   void drawElements(GLenum mode, GLsizei count, const GLushort* indices) override
   {
      if (indices == nullptr || count == 0)
      {
         notify(WARN) << "Attempting to do an intersection on an empty vertex array" << std::endl;
         return;
      }

      typedef const GLushort* IndexPointer;

      switch (mode)
      {
      case (GL_TRIANGLES):
      {
         IndexPointer ilast = &indices[count];
         for (IndexPointer iptr = indices; iptr < ilast; iptr += 3)
         {
            this->operator()(_vertexArrayPtr[*iptr],
                             _vertexArrayPtr[*(iptr + 1)],
                             _vertexArrayPtr[*(iptr + 2)],
                             _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_TRIANGLE_STRIP):
      {
         IndexPointer iptr = indices;
         for (GLsizei i = 2; i < count; ++i, ++iptr)
         {
            if ((i % 2))
               this->operator()(_vertexArrayPtr[*(iptr)],
                                _vertexArrayPtr[*(iptr + 2)],
                                _vertexArrayPtr[*(iptr + 1)],
                                _treatVertexDataAsTemporary);
            else
               this->operator()(_vertexArrayPtr[*(iptr)],
                                _vertexArrayPtr[*(iptr + 1)],
                                _vertexArrayPtr[*(iptr + 2)],
                                _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_QUADS):
      {
         IndexPointer iptr = indices;
         for (GLsizei i = 3; i < count; i += 4, iptr += 4)
         {
            this->operator()(_vertexArrayPtr[*(iptr)],
                             _vertexArrayPtr[*(iptr + 1)],
                             _vertexArrayPtr[*(iptr + 2)],
                             _treatVertexDataAsTemporary);
            this->operator()(_vertexArrayPtr[*(iptr)],
                             _vertexArrayPtr[*(iptr + 2)],
                             _vertexArrayPtr[*(iptr + 3)],
                             _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_QUAD_STRIP):
      {
         IndexPointer iptr = indices;
         for (GLsizei i = 3; i < count; i += 2, iptr += 2)
         {
            this->operator()(_vertexArrayPtr[*(iptr)],
                             _vertexArrayPtr[*(iptr + 1)],
                             _vertexArrayPtr[*(iptr + 2)],
                             _treatVertexDataAsTemporary);
            this->operator()(_vertexArrayPtr[*(iptr + 1)],
                             _vertexArrayPtr[*(iptr + 3)],
                             _vertexArrayPtr[*(iptr + 2)],
                             _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
      case (GL_TRIANGLE_FAN):
      {
         IndexPointer iptr   = indices;
         const Vec3&  vfirst = _vertexArrayPtr[*iptr];
         ++iptr;
         for (GLsizei i = 2; i < count; ++i, ++iptr)
         {
            this->operator()(vfirst, _vertexArrayPtr[*(iptr)], _vertexArrayPtr[*(iptr + 1)], _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_LINES):
      {
         IndexPointer ilast = &indices[count];
         for (IndexPointer iptr = indices; iptr < ilast; iptr += 2)
            this->operator()(_vertexArrayPtr[*iptr], _vertexArrayPtr[*(iptr + 1)], _treatVertexDataAsTemporary);
         break;
      }
      case (GL_LINE_STRIP):
      {
         IndexPointer iptr = indices;
         for (GLsizei i = 1; i < count; ++i, ++iptr)
            this->operator()(_vertexArrayPtr[*(iptr)], _vertexArrayPtr[*(iptr + 1)], _treatVertexDataAsTemporary);
         break;
      }
      case (GL_LINE_LOOP):
      {
         IndexPointer iptr = indices;
         for (GLsizei i = 1; i < count; ++i, ++iptr)
            this->operator()(_vertexArrayPtr[*(iptr)], _vertexArrayPtr[*(iptr + 1)], _treatVertexDataAsTemporary);
         const Vec3& vfirst = _vertexArrayPtr[*(indices)];
         const Vec3& vlast  = _vertexArrayPtr[*(indices + count - 1)];
         this->      operator()(vfirst, vlast, _treatVertexDataAsTemporary);
         break;
      }
      case (GL_POINTS):
      {
         IndexPointer ilast = &indices[count];
         for (IndexPointer iptr = indices; iptr < ilast; iptr += 1)
            this->operator()(_vertexArrayPtr[*iptr], _treatVertexDataAsTemporary);
         break;
      }
      default:
         break;
      }
   }

   void drawElements(GLenum mode, GLsizei count, const GLuint* indices) override
   {
      if (indices == nullptr || count == 0)
      {
         notify(WARN) << "Attempting to do an intersection on an empty vertex array" << std::endl;
         return;
      }

      typedef const GLuint* IndexPointer;

      switch (mode)
      {
      case (GL_TRIANGLES):
      {
         IndexPointer ilast = &indices[count];
         for (IndexPointer iptr = indices; iptr < ilast; iptr += 3)
            this->operator()(_vertexArrayPtr[*iptr],
                             _vertexArrayPtr[*(iptr + 1)],
                             _vertexArrayPtr[*(iptr + 2)],
                             _treatVertexDataAsTemporary);
         break;
      }
      case (GL_TRIANGLE_STRIP):
      {
         IndexPointer iptr = indices;
         for (GLsizei i = 2; i < count; ++i, ++iptr)
         {
            if ((i % 2))
               this->operator()(_vertexArrayPtr[*(iptr)],
                                _vertexArrayPtr[*(iptr + 2)],
                                _vertexArrayPtr[*(iptr + 1)],
                                _treatVertexDataAsTemporary);
            else
               this->operator()(_vertexArrayPtr[*(iptr)],
                                _vertexArrayPtr[*(iptr + 1)],
                                _vertexArrayPtr[*(iptr + 2)],
                                _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_QUADS):
      {
         IndexPointer iptr = indices;
         for (GLsizei i = 3; i < count; i += 4, iptr += 4)
         {
            this->operator()(_vertexArrayPtr[*(iptr)],
                             _vertexArrayPtr[*(iptr + 1)],
                             _vertexArrayPtr[*(iptr + 2)],
                             _treatVertexDataAsTemporary);
            this->operator()(_vertexArrayPtr[*(iptr)],
                             _vertexArrayPtr[*(iptr + 2)],
                             _vertexArrayPtr[*(iptr + 3)],
                             _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_QUAD_STRIP):
      {
         IndexPointer iptr = indices;
         for (GLsizei i = 3; i < count; i += 2, iptr += 2)
         {
            this->operator()(_vertexArrayPtr[*(iptr)],
                             _vertexArrayPtr[*(iptr + 1)],
                             _vertexArrayPtr[*(iptr + 2)],
                             _treatVertexDataAsTemporary);
            this->operator()(_vertexArrayPtr[*(iptr + 1)],
                             _vertexArrayPtr[*(iptr + 3)],
                             _vertexArrayPtr[*(iptr + 2)],
                             _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
      case (GL_TRIANGLE_FAN):
      {
         IndexPointer iptr   = indices;
         const Vec3&  vfirst = _vertexArrayPtr[*iptr];
         ++iptr;
         for (GLsizei i = 2; i < count; ++i, ++iptr)
         {
            this->operator()(vfirst, _vertexArrayPtr[*(iptr)], _vertexArrayPtr[*(iptr + 1)], _treatVertexDataAsTemporary);
         }
         break;
      }
      case (GL_LINES):
      {
         IndexPointer ilast = &indices[count];
         for (IndexPointer iptr = indices; iptr < ilast; iptr += 2)
            this->operator()(_vertexArrayPtr[*iptr], _vertexArrayPtr[*(iptr + 1)], _treatVertexDataAsTemporary);
         break;
      }
      case (GL_LINE_STRIP):
      {
         IndexPointer iptr = indices;
         for (GLsizei i = 1; i < count; ++i, ++iptr)
            this->operator()(_vertexArrayPtr[*(iptr)], _vertexArrayPtr[*(iptr + 1)], _treatVertexDataAsTemporary);
         break;
      }
      case (GL_LINE_LOOP):
      {
         IndexPointer iptr = indices;
         for (GLsizei i = 1; i < count; ++i, ++iptr)
            this->operator()(_vertexArrayPtr[*(iptr)], _vertexArrayPtr[*(iptr + 1)], _treatVertexDataAsTemporary);
         const Vec3& vfirst = _vertexArrayPtr[*(indices)];
         const Vec3& vlast  = _vertexArrayPtr[*(indices + count - 1)];
         this->      operator()(vfirst, vlast, _treatVertexDataAsTemporary);
         break;
      }
      case (GL_POINTS):
      {
         IndexPointer ilast = &indices[count];
         for (IndexPointer iptr = indices; iptr < ilast; iptr += 1)
            this->operator()(_vertexArrayPtr[*iptr], _treatVertexDataAsTemporary);
         break;
      }
      default:
         break;
      }
   }

protected:
   unsigned int _vertexArraySize;
   const Vec3*  _vertexArrayPtr;

   GLenum            _modeCache;
   std::vector<Vec3> _vertexCache;
   bool              _treatVertexDataAsTemporary;
};


} // namespace osg

#endif
