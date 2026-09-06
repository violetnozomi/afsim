// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#define DEFINE_DEBUG_MEMBERS1(C1)          \
   virtual const char* GetFieldName(int i) \
   {                                       \
      const char* names[] = {#C1};         \
      return names[i];                     \
   }
#define DEFINE_DEBUG_MEMBERS2(C1, C2)      \
   virtual const char* GetFieldName(int i) \
   {                                       \
      const char* names[] = {#C1, #C2};    \
      return names[i];                     \
   }
#define DEFINE_DEBUG_MEMBERS3(C1, C2, C3)    \
   virtual const char* GetFieldName(int i)   \
   {                                         \
      const char* names[] = {#C1, #C2, #C3}; \
      return names[i];                       \
   }
#define DEFINE_DEBUG_MEMBERS4(C1, C2, C3, C4)     \
   virtual const char* GetFieldName(int i)        \
   {                                              \
      const char* names[] = {#C1, #C2, #C3, #C4}; \
      return names[i];                            \
   }
#define DEFINE_DEBUG_MEMBERS5(C1, C2, C3, C4, C5)      \
   virtual const char* GetFieldName(int i)             \
   {                                                   \
      const char* names[] = {#C1, #C2, #C3, #C4, #C5}; \
      return names[i];                                 \
   }
#define DEFINE_DEBUG_MEMBERS6(C1, C2, C3, C4, C5, C6)       \
   virtual const char* GetFieldName(int i)                  \
   {                                                        \
      const char* names[] = {#C1, #C2, #C3, #C4, #C5, #C6}; \
      return names[i];                                      \
   }
#define DEFINE_DEBUG_MEMBERS7(C1, C2, C3, C4, C5, C6, C7)        \
   virtual const char* GetFieldName(int i)                       \
   {                                                             \
      const char* names[] = {#C1, #C2, #C3, #C4, #C5, #C6, #C7}; \
      return names[i];                                           \
   }
#define DEFINE_DEBUG_MEMBERS8(C1, C2, C3, C4, C5, C6, C7, C8)         \
   virtual const char* GetFieldName(int i)                            \
   {                                                                  \
      const char* names[] = {#C1, #C2, #C3, #C4, #C5, #C6, #C7, #C8}; \
      return names[i];                                                \
   }
#define DEFINE_DEBUG_MEMBERS9(C1, C2, C3, C4, C5, C6, C7, C8, C9)          \
   virtual const char* GetFieldName(int i)                                 \
   {                                                                       \
      const char* names[] = {#C1, #C2, #C3, #C4, #C5, #C6, #C7, #C8, #C9}; \
      return names[i];                                                     \
   }
#define DEFINE_DEBUG_MEMBERS10(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10)          \
   virtual const char* GetFieldName(int i)                                       \
   {                                                                             \
      const char* names[] = {#C1, #C2, #C3, #C4, #C5, #C6, #C7, #C8, #C9, #C10}; \
      return names[i];                                                           \
   }
#define DEFINE_DEBUG_MEMBERS11(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11)           \
   virtual const char* GetFieldName(int i)                                             \
   {                                                                                   \
      const char* names[] = {#C1, #C2, #C3, #C4, #C5, #C6, #C7, #C8, #C9, #C10, #C11}; \
      return names[i];                                                                 \
   }
#define DEFINE_DEBUG_MEMBERS12(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12)            \
   virtual const char* GetFieldName(int i)                                                   \
   {                                                                                         \
      const char* names[] = {#C1, #C2, #C3, #C4, #C5, #C6, #C7, #C8, #C9, #C10, #C11, #C12}; \
      return names[i];                                                                       \
   }
#define DEFINE_DEBUG_MEMBERS13(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13)             \
   virtual const char* GetFieldName(int i)                                                         \
   {                                                                                               \
      const char* names[] = {#C1, #C2, #C3, #C4, #C5, #C6, #C7, #C8, #C9, #C10, #C11, #C12, #C13}; \
      return names[i];                                                                             \
   }
#define DEFINE_DEBUG_MEMBERS14(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14)              \
   virtual const char* GetFieldName(int i)                                                               \
   {                                                                                                     \
      const char* names[] = {#C1, #C2, #C3, #C4, #C5, #C6, #C7, #C8, #C9, #C10, #C11, #C12, #C13, #C14}; \
      return names[i];                                                                                   \
   }
#define DEFINE_DEBUG_MEMBERS15(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15)               \
   virtual const char* GetFieldName(int i)                                                                     \
   {                                                                                                           \
      const char* names[] = {#C1, #C2, #C3, #C4, #C5, #C6, #C7, #C8, #C9, #C10, #C11, #C12, #C13, #C14, #C15}; \
      return names[i];                                                                                         \
   }
#define DEFINE_DEBUG_MEMBERS16(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16)                \
   virtual const char* GetFieldName(int i)                                                                           \
   {                                                                                                                 \
      const char* names[] = {#C1, #C2, #C3, #C4, #C5, #C6, #C7, #C8, #C9, #C10, #C11, #C12, #C13, #C14, #C15, #C16}; \
      return names[i];                                                                                               \
   }
#define DEFINE_DEBUG_MEMBERS17(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17)                 \
   virtual const char* GetFieldName(int i)                                                                                 \
   {                                                                                                                       \
      const char* names[] = {#C1, #C2, #C3, #C4, #C5, #C6, #C7, #C8, #C9, #C10, #C11, #C12, #C13, #C14, #C15, #C16, #C17}; \
      return names[i];                                                                                                     \
   }
#define DEFINE_DEBUG_MEMBERS18(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18)                  \
   virtual const char* GetFieldName(int i)                                                                                       \
   {                                                                                                                             \
      const char* names[] = {#C1, #C2, #C3, #C4, #C5, #C6, #C7, #C8, #C9, #C10, #C11, #C12, #C13, #C14, #C15, #C16, #C17, #C18}; \
      return names[i];                                                                                                           \
   }
#define DEFINE_DEBUG_MEMBERS19(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19) \
   virtual const char* GetFieldName(int i)                                                                           \
   {                                                                                                                 \
      const char* names[] =                                                                                          \
         {#C1, #C2, #C3, #C4, #C5, #C6, #C7, #C8, #C9, #C10, #C11, #C12, #C13, #C14, #C15, #C16, #C17, #C18, #C19};  \
      return names[i];                                                                                               \
   }
#define DEFINE_DEBUG_MEMBERS20(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20) \
   virtual const char* GetFieldName(int i)                                                                                \
   {                                                                                                                      \
      const char* names[] = {#C1,  #C2,  #C3,  #C4,  #C5,  #C6,  #C7,  #C8,  #C9,  #C10,                                  \
                             #C11, #C12, #C13, #C14, #C15, #C16, #C17, #C18, #C19, #C20};                                 \
      return names[i];                                                                                                    \
   }
#define DEFINE_DEBUG_MEMBERS21(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21) \
   virtual const char* GetFieldName(int i)                                                                                     \
   {                                                                                                                           \
      const char* names[] = {#C1,  #C2,  #C3,  #C4,  #C5,  #C6,  #C7,  #C8,  #C9,  #C10, #C11,                                 \
                             #C12, #C13, #C14, #C15, #C16, #C17, #C18, #C19, #C20, #C21};                                      \
      return names[i];                                                                                                         \
   }
#define DEFINE_DEBUG_MEMBERS22(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22) \
   virtual const char* GetFieldName(int i)                                                                                          \
   {                                                                                                                                \
      const char* names[] = {#C1,  #C2,  #C3,  #C4,  #C5,  #C6,  #C7,  #C8,  #C9,  #C10, #C11,                                      \
                             #C12, #C13, #C14, #C15, #C16, #C17, #C18, #C19, #C20, #C21, #C22};                                     \
      return names[i];                                                                                                              \
   }
#define DEFINE_DEBUG_MEMBERS23(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22, C23) \
   virtual const char* GetFieldName(int i)                                                                                               \
   {                                                                                                                                     \
      const char* names[] = {#C1,  #C2,  #C3,  #C4,  #C5,  #C6,  #C7,  #C8,  #C9,  #C10, #C11, #C12,                                     \
                             #C13, #C14, #C15, #C16, #C17, #C18, #C19, #C20, #C21, #C22, #C23};                                          \
      return names[i];                                                                                                                   \
   }
#define DEFINE_DEBUG_MEMBERS24(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22, C23, C24) \
   virtual const char* GetFieldName(int i)                                                                                                    \
   {                                                                                                                                          \
      const char* names[] = {#C1,  #C2,  #C3,  #C4,  #C5,  #C6,  #C7,  #C8,  #C9,  #C10, #C11, #C12,                                          \
                             #C13, #C14, #C15, #C16, #C17, #C18, #C19, #C20, #C21, #C22, #C23, #C24};                                         \
      return names[i];                                                                                                                        \
   }
#define DEFINE_DEBUG_MEMBERS25(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22, C23, C24, C25) \
   virtual const char* GetFieldName(int i)                                                                                                         \
   {                                                                                                                                               \
      const char* names[] = {#C1,  #C2,  #C3,  #C4,  #C5,  #C6,  #C7,  #C8,  #C9,  #C10, #C11, #C12, #C13,                                         \
                             #C14, #C15, #C16, #C17, #C18, #C19, #C20, #C21, #C22, #C23, #C24, #C25};                                              \
      return names[i];                                                                                                                             \
   }
#define DEFINE_DEBUG_MEMBERS26(C1,                                                                          \
                               C2,                                                                          \
                               C3,                                                                          \
                               C4,                                                                          \
                               C5,                                                                          \
                               C6,                                                                          \
                               C7,                                                                          \
                               C8,                                                                          \
                               C9,                                                                          \
                               C10,                                                                         \
                               C11,                                                                         \
                               C12,                                                                         \
                               C13,                                                                         \
                               C14,                                                                         \
                               C15,                                                                         \
                               C16,                                                                         \
                               C17,                                                                         \
                               C18,                                                                         \
                               C19,                                                                         \
                               C20,                                                                         \
                               C21,                                                                         \
                               C22,                                                                         \
                               C23,                                                                         \
                               C24,                                                                         \
                               C25,                                                                         \
                               C26)                                                                         \
   virtual const char* GetFieldName(int i)                                                                  \
   {                                                                                                        \
      const char* names[] = {#C1,  #C2,  #C3,  #C4,  #C5,  #C6,  #C7,  #C8,  #C9,  #C10, #C11, #C12, #C13,  \
                             #C14, #C15, #C16, #C17, #C18, #C19, #C20, #C21, #C22, #C23, #C24, #C25, #C26}; \
      return names[i];                                                                                      \
   }
#define DEFINE_DEBUG_MEMBERS27(C1,                                                                               \
                               C2,                                                                               \
                               C3,                                                                               \
                               C4,                                                                               \
                               C5,                                                                               \
                               C6,                                                                               \
                               C7,                                                                               \
                               C8,                                                                               \
                               C9,                                                                               \
                               C10,                                                                              \
                               C11,                                                                              \
                               C12,                                                                              \
                               C13,                                                                              \
                               C14,                                                                              \
                               C15,                                                                              \
                               C16,                                                                              \
                               C17,                                                                              \
                               C18,                                                                              \
                               C19,                                                                              \
                               C20,                                                                              \
                               C21,                                                                              \
                               C22,                                                                              \
                               C23,                                                                              \
                               C24,                                                                              \
                               C25,                                                                              \
                               C26,                                                                              \
                               C27)                                                                              \
   virtual const char* GetFieldName(int i)                                                                       \
   {                                                                                                             \
      const char* names[] = {#C1,  #C2,  #C3,  #C4,  #C5,  #C6,  #C7,  #C8,  #C9,  #C10, #C11, #C12, #C13, #C14, \
                             #C15, #C16, #C17, #C18, #C19, #C20, #C21, #C22, #C23, #C24, #C25, #C26, #C27};      \
      return names[i];                                                                                           \
   }
#define DEFINE_DEBUG_MEMBERS28(C1,                                                                                \
                               C2,                                                                                \
                               C3,                                                                                \
                               C4,                                                                                \
                               C5,                                                                                \
                               C6,                                                                                \
                               C7,                                                                                \
                               C8,                                                                                \
                               C9,                                                                                \
                               C10,                                                                               \
                               C11,                                                                               \
                               C12,                                                                               \
                               C13,                                                                               \
                               C14,                                                                               \
                               C15,                                                                               \
                               C16,                                                                               \
                               C17,                                                                               \
                               C18,                                                                               \
                               C19,                                                                               \
                               C20,                                                                               \
                               C21,                                                                               \
                               C22,                                                                               \
                               C23,                                                                               \
                               C24,                                                                               \
                               C25,                                                                               \
                               C26,                                                                               \
                               C27,                                                                               \
                               C28)                                                                               \
   virtual const char* GetFieldName(int i)                                                                        \
   {                                                                                                              \
      const char* names[] = {#C1,  #C2,  #C3,  #C4,  #C5,  #C6,  #C7,  #C8,  #C9,  #C10, #C11, #C12, #C13, #C14,  \
                             #C15, #C16, #C17, #C18, #C19, #C20, #C21, #C22, #C23, #C24, #C25, #C26, #C27, #C28}; \
      return names[i];                                                                                            \
   }
#define DEFINE_DEBUG_MEMBERS29(C1,                                                                                     \
                               C2,                                                                                     \
                               C3,                                                                                     \
                               C4,                                                                                     \
                               C5,                                                                                     \
                               C6,                                                                                     \
                               C7,                                                                                     \
                               C8,                                                                                     \
                               C9,                                                                                     \
                               C10,                                                                                    \
                               C11,                                                                                    \
                               C12,                                                                                    \
                               C13,                                                                                    \
                               C14,                                                                                    \
                               C15,                                                                                    \
                               C16,                                                                                    \
                               C17,                                                                                    \
                               C18,                                                                                    \
                               C19,                                                                                    \
                               C20,                                                                                    \
                               C21,                                                                                    \
                               C22,                                                                                    \
                               C23,                                                                                    \
                               C24,                                                                                    \
                               C25,                                                                                    \
                               C26,                                                                                    \
                               C27,                                                                                    \
                               C28,                                                                                    \
                               C29)                                                                                    \
   virtual const char* GetFieldName(int i)                                                                             \
   {                                                                                                                   \
      const char* names[] = {#C1,  #C2,  #C3,  #C4,  #C5,  #C6,  #C7,  #C8,  #C9,  #C10, #C11, #C12, #C13, #C14, #C15, \
                             #C16, #C17, #C18, #C19, #C20, #C21, #C22, #C23, #C24, #C25, #C26, #C27, #C28, #C29};      \
      return names[i];                                                                                                 \
   }
#define DEFINE_DEBUG_MEMBERS30(C1,                                                                                      \
                               C2,                                                                                      \
                               C3,                                                                                      \
                               C4,                                                                                      \
                               C5,                                                                                      \
                               C6,                                                                                      \
                               C7,                                                                                      \
                               C8,                                                                                      \
                               C9,                                                                                      \
                               C10,                                                                                     \
                               C11,                                                                                     \
                               C12,                                                                                     \
                               C13,                                                                                     \
                               C14,                                                                                     \
                               C15,                                                                                     \
                               C16,                                                                                     \
                               C17,                                                                                     \
                               C18,                                                                                     \
                               C19,                                                                                     \
                               C20,                                                                                     \
                               C21,                                                                                     \
                               C22,                                                                                     \
                               C23,                                                                                     \
                               C24,                                                                                     \
                               C25,                                                                                     \
                               C26,                                                                                     \
                               C27,                                                                                     \
                               C28,                                                                                     \
                               C29,                                                                                     \
                               C30)                                                                                     \
   virtual const char* GetFieldName(int i)                                                                              \
   {                                                                                                                    \
      const char* names[] = {#C1,  #C2,  #C3,  #C4,  #C5,  #C6,  #C7,  #C8,  #C9,  #C10, #C11, #C12, #C13, #C14, #C15,  \
                             #C16, #C17, #C18, #C19, #C20, #C21, #C22, #C23, #C24, #C25, #C26, #C27, #C28, #C29, #C30}; \
      return names[i];                                                                                                  \
   }
#define DEFINE_DEBUG_MEMBERS31(C1,                                                             \
                               C2,                                                             \
                               C3,                                                             \
                               C4,                                                             \
                               C5,                                                             \
                               C6,                                                             \
                               C7,                                                             \
                               C8,                                                             \
                               C9,                                                             \
                               C10,                                                            \
                               C11,                                                            \
                               C12,                                                            \
                               C13,                                                            \
                               C14,                                                            \
                               C15,                                                            \
                               C16,                                                            \
                               C17,                                                            \
                               C18,                                                            \
                               C19,                                                            \
                               C20,                                                            \
                               C21,                                                            \
                               C22,                                                            \
                               C23,                                                            \
                               C24,                                                            \
                               C25,                                                            \
                               C26,                                                            \
                               C27,                                                            \
                               C28,                                                            \
                               C29,                                                            \
                               C30,                                                            \
                               C31)                                                            \
   virtual const char* GetFieldName(int i)                                                     \
   {                                                                                           \
      const char* names[] = {#C1,  #C2,  #C3,  #C4,  #C5,  #C6,  #C7,  #C8,  #C9,  #C10, #C11, \
                             #C12, #C13, #C14, #C15, #C16, #C17, #C18, #C19, #C20, #C21, #C22, \
                             #C23, #C24, #C25, #C26, #C27, #C28, #C29, #C30, #C31};            \
      return names[i];                                                                         \
   }
#define DEFINE_MEMBERS1(C1)                                                     \
   FieldType1                                           C1;                     \
   static const unsigned int                            FieldCount = 1;         \
   MessageSizeCheck<___RequiredMessageSize, FieldType1> __MessageSizeChecker__; \
   template<class T>                                                            \
   void Transfer(T& ar)                                                         \
   {                                                                            \
      ar << C1;                                                                 \
   }                                                                            \
   DEFINE_DEBUG_MEMBERS1(C1)

#define DEFINE_MEMBERS2(C1, C2)                                                             \
   FieldType1                                                       C1;                     \
   FieldType2                                                       C2;                     \
   static const unsigned int                                        FieldCount = 2;         \
   MessageSizeCheck<___RequiredMessageSize, FieldType1, FieldType2> __MessageSizeChecker__; \
   template<class T>                                                                        \
   void Transfer(T& ar)                                                                     \
   {                                                                                        \
      ar << C1 << C2;                                                                       \
   }                                                                                        \
   DEFINE_DEBUG_MEMBERS2(C1, C2)

#define DEFINE_MEMBERS3(C1, C2, C3)                                                                     \
   FieldType1                                                                   C1;                     \
   FieldType2                                                                   C2;                     \
   FieldType3                                                                   C3;                     \
   static const unsigned int                                                    FieldCount = 3;         \
   MessageSizeCheck<___RequiredMessageSize, FieldType1, FieldType2, FieldType3> __MessageSizeChecker__; \
   template<class T>                                                                                    \
   void Transfer(T& ar)                                                                                 \
   {                                                                                                    \
      ar << C1 << C2 << C3;                                                                             \
   }                                                                                                    \
   DEFINE_DEBUG_MEMBERS3(C1, C2, C3)

#define DEFINE_MEMBERS4(C1, C2, C3, C4)                                                                             \
   FieldType1                                                                               C1;                     \
   FieldType2                                                                               C2;                     \
   FieldType3                                                                               C3;                     \
   FieldType4                                                                               C4;                     \
   static const unsigned int                                                                FieldCount = 4;         \
   MessageSizeCheck<___RequiredMessageSize, FieldType1, FieldType2, FieldType3, FieldType4> __MessageSizeChecker__; \
   template<class T>                                                                                                \
   void Transfer(T& ar)                                                                                             \
   {                                                                                                                \
      ar << C1 << C2 << C3 << C4;                                                                                   \
   }                                                                                                                \
   DEFINE_DEBUG_MEMBERS4(C1, C2, C3, C4)

#define DEFINE_MEMBERS5(C1, C2, C3, C4, C5)                                                                                     \
   FieldType1                                                                                           C1;                     \
   FieldType2                                                                                           C2;                     \
   FieldType3                                                                                           C3;                     \
   FieldType4                                                                                           C4;                     \
   FieldType5                                                                                           C5;                     \
   static const unsigned int                                                                            FieldCount = 5;         \
   MessageSizeCheck<___RequiredMessageSize, FieldType1, FieldType2, FieldType3, FieldType4, FieldType5> __MessageSizeChecker__; \
   template<class T>                                                                                                            \
   void Transfer(T& ar)                                                                                                         \
   {                                                                                                                            \
      ar << C1 << C2 << C3 << C4 << C5;                                                                                         \
   }                                                                                                                            \
   DEFINE_DEBUG_MEMBERS5(C1, C2, C3, C4, C5)

#define DEFINE_MEMBERS6(C1, C2, C3, C4, C5, C6)                                                                                             \
   FieldType1                C1;                                                                                                            \
   FieldType2                C2;                                                                                                            \
   FieldType3                C3;                                                                                                            \
   FieldType4                C4;                                                                                                            \
   FieldType5                C5;                                                                                                            \
   FieldType6                C6;                                                                                                            \
   static const unsigned int FieldCount = 6;                                                                                                \
   MessageSizeCheck<___RequiredMessageSize, FieldType1, FieldType2, FieldType3, FieldType4, FieldType5, FieldType6> __MessageSizeChecker__; \
   template<class T>                                                                                                                        \
   void Transfer(T& ar)                                                                                                                     \
   {                                                                                                                                        \
      ar << C1 << C2 << C3 << C4 << C5 << C6;                                                                                               \
   }                                                                                                                                        \
   DEFINE_DEBUG_MEMBERS6(C1, C2, C3, C4, C5, C6)

#define DEFINE_MEMBERS7(C1, C2, C3, C4, C5, C6, C7)                                                                             \
   FieldType1                C1;                                                                                                \
   FieldType2                C2;                                                                                                \
   FieldType3                C3;                                                                                                \
   FieldType4                C4;                                                                                                \
   FieldType5                C5;                                                                                                \
   FieldType6                C6;                                                                                                \
   FieldType7                C7;                                                                                                \
   static const unsigned int FieldCount = 7;                                                                                    \
   MessageSizeCheck<___RequiredMessageSize, FieldType1, FieldType2, FieldType3, FieldType4, FieldType5, FieldType6, FieldType7> \
      __MessageSizeChecker__;                                                                                                   \
   template<class T>                                                                                                            \
   void Transfer(T& ar)                                                                                                         \
   {                                                                                                                            \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7;                                                                             \
   }                                                                                                                            \
   DEFINE_DEBUG_MEMBERS7(C1, C2, C3, C4, C5, C6, C7)

#define DEFINE_MEMBERS8(C1, C2, C3, C4, C5, C6, C7, C8)                                                                                     \
   FieldType1                C1;                                                                                                            \
   FieldType2                C2;                                                                                                            \
   FieldType3                C3;                                                                                                            \
   FieldType4                C4;                                                                                                            \
   FieldType5                C5;                                                                                                            \
   FieldType6                C6;                                                                                                            \
   FieldType7                C7;                                                                                                            \
   FieldType8                C8;                                                                                                            \
   static const unsigned int FieldCount = 8;                                                                                                \
   MessageSizeCheck<___RequiredMessageSize, FieldType1, FieldType2, FieldType3, FieldType4, FieldType5, FieldType6, FieldType7, FieldType8> \
      __MessageSizeChecker__;                                                                                                               \
   template<class T>                                                                                                                        \
   void Transfer(T& ar)                                                                                                                     \
   {                                                                                                                                        \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8;                                                                                   \
   }                                                                                                                                        \
   DEFINE_DEBUG_MEMBERS8(C1, C2, C3, C4, C5, C6, C7, C8)

#define DEFINE_MEMBERS9(C1, C2, C3, C4, C5, C6, C7, C8, C9)                                                                                             \
   FieldType1                C1;                                                                                                                        \
   FieldType2                C2;                                                                                                                        \
   FieldType3                C3;                                                                                                                        \
   FieldType4                C4;                                                                                                                        \
   FieldType5                C5;                                                                                                                        \
   FieldType6                C6;                                                                                                                        \
   FieldType7                C7;                                                                                                                        \
   FieldType8                C8;                                                                                                                        \
   FieldType9                C9;                                                                                                                        \
   static const unsigned int FieldCount = 9;                                                                                                            \
   MessageSizeCheck<___RequiredMessageSize, FieldType1, FieldType2, FieldType3, FieldType4, FieldType5, FieldType6, FieldType7, FieldType8, FieldType9> \
      __MessageSizeChecker__;                                                                                                                           \
   template<class T>                                                                                                                                    \
   void Transfer(T& ar)                                                                                                                                 \
   {                                                                                                                                                    \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9;                                                                                         \
   }                                                                                                                                                    \
   DEFINE_DEBUG_MEMBERS9(C1, C2, C3, C4, C5, C6, C7, C8, C9)

#define DEFINE_MEMBERS10(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10)                                                                                                    \
   FieldType1                C1;                                                                                                                                     \
   FieldType2                C2;                                                                                                                                     \
   FieldType3                C3;                                                                                                                                     \
   FieldType4                C4;                                                                                                                                     \
   FieldType5                C5;                                                                                                                                     \
   FieldType6                C6;                                                                                                                                     \
   FieldType7                C7;                                                                                                                                     \
   FieldType8                C8;                                                                                                                                     \
   FieldType9                C9;                                                                                                                                     \
   FieldType10               C10;                                                                                                                                    \
   static const unsigned int FieldCount = 10;                                                                                                                        \
   MessageSizeCheck<___RequiredMessageSize, FieldType1, FieldType2, FieldType3, FieldType4, FieldType5, FieldType6, FieldType7, FieldType8, FieldType9, FieldType10> \
      __MessageSizeChecker__;                                                                                                                                        \
   template<class T>                                                                                                                                                 \
   void Transfer(T& ar)                                                                                                                                              \
   {                                                                                                                                                                 \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10;                                                                                               \
   }                                                                                                                                                                 \
   DEFINE_DEBUG_MEMBERS10(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10)

#define DEFINE_MEMBERS11(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11)                                                                                                            \
   FieldType1                C1;                                                                                                                                                  \
   FieldType2                C2;                                                                                                                                                  \
   FieldType3                C3;                                                                                                                                                  \
   FieldType4                C4;                                                                                                                                                  \
   FieldType5                C5;                                                                                                                                                  \
   FieldType6                C6;                                                                                                                                                  \
   FieldType7                C7;                                                                                                                                                  \
   FieldType8                C8;                                                                                                                                                  \
   FieldType9                C9;                                                                                                                                                  \
   FieldType10               C10;                                                                                                                                                 \
   FieldType11               C11;                                                                                                                                                 \
   static const unsigned int FieldCount = 11;                                                                                                                                     \
   MessageSizeCheck<___RequiredMessageSize, FieldType1, FieldType2, FieldType3, FieldType4, FieldType5, FieldType6, FieldType7, FieldType8, FieldType9, FieldType10, FieldType11> \
      __MessageSizeChecker__;                                                                                                                                                     \
   template<class T>                                                                                                                                                              \
   void Transfer(T& ar)                                                                                                                                                           \
   {                                                                                                                                                                              \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11;                                                                                                     \
   }                                                                                                                                                                              \
   DEFINE_DEBUG_MEMBERS11(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11)

#define DEFINE_MEMBERS12(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12)                                                                                                                    \
   FieldType1                C1;                                                                                                                                                               \
   FieldType2                C2;                                                                                                                                                               \
   FieldType3                C3;                                                                                                                                                               \
   FieldType4                C4;                                                                                                                                                               \
   FieldType5                C5;                                                                                                                                                               \
   FieldType6                C6;                                                                                                                                                               \
   FieldType7                C7;                                                                                                                                                               \
   FieldType8                C8;                                                                                                                                                               \
   FieldType9                C9;                                                                                                                                                               \
   FieldType10               C10;                                                                                                                                                              \
   FieldType11               C11;                                                                                                                                                              \
   FieldType12               C12;                                                                                                                                                              \
   static const unsigned int FieldCount = 12;                                                                                                                                                  \
   MessageSizeCheck<___RequiredMessageSize, FieldType1, FieldType2, FieldType3, FieldType4, FieldType5, FieldType6, FieldType7, FieldType8, FieldType9, FieldType10, FieldType11, FieldType12> \
      __MessageSizeChecker__;                                                                                                                                                                  \
   template<class T>                                                                                                                                                                           \
   void Transfer(T& ar)                                                                                                                                                                        \
   {                                                                                                                                                                                           \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12;                                                                                                           \
   }                                                                                                                                                                                           \
   DEFINE_DEBUG_MEMBERS12(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12)

#define DEFINE_MEMBERS13(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13)                                                                                                                            \
   FieldType1                C1;                                                                                                                                                                            \
   FieldType2                C2;                                                                                                                                                                            \
   FieldType3                C3;                                                                                                                                                                            \
   FieldType4                C4;                                                                                                                                                                            \
   FieldType5                C5;                                                                                                                                                                            \
   FieldType6                C6;                                                                                                                                                                            \
   FieldType7                C7;                                                                                                                                                                            \
   FieldType8                C8;                                                                                                                                                                            \
   FieldType9                C9;                                                                                                                                                                            \
   FieldType10               C10;                                                                                                                                                                           \
   FieldType11               C11;                                                                                                                                                                           \
   FieldType12               C12;                                                                                                                                                                           \
   FieldType13               C13;                                                                                                                                                                           \
   static const unsigned int FieldCount = 13;                                                                                                                                                               \
   MessageSizeCheck<___RequiredMessageSize, FieldType1, FieldType2, FieldType3, FieldType4, FieldType5, FieldType6, FieldType7, FieldType8, FieldType9, FieldType10, FieldType11, FieldType12, FieldType13> \
      __MessageSizeChecker__;                                                                                                                                                                               \
   template<class T>                                                                                                                                                                                        \
   void Transfer(T& ar)                                                                                                                                                                                     \
   {                                                                                                                                                                                                        \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13;                                                                                                                 \
   }                                                                                                                                                                                                        \
   DEFINE_DEBUG_MEMBERS13(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13)

#define DEFINE_MEMBERS14(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14)              \
   FieldType1                C1;                                                                   \
   FieldType2                C2;                                                                   \
   FieldType3                C3;                                                                   \
   FieldType4                C4;                                                                   \
   FieldType5                C5;                                                                   \
   FieldType6                C6;                                                                   \
   FieldType7                C7;                                                                   \
   FieldType8                C8;                                                                   \
   FieldType9                C9;                                                                   \
   FieldType10               C10;                                                                  \
   FieldType11               C11;                                                                  \
   FieldType12               C12;                                                                  \
   FieldType13               C13;                                                                  \
   FieldType14               C14;                                                                  \
   static const unsigned int FieldCount = 14;                                                      \
   MessageSizeCheck<___RequiredMessageSize,                                                        \
                    FieldType1,                                                                    \
                    FieldType2,                                                                    \
                    FieldType3,                                                                    \
                    FieldType4,                                                                    \
                    FieldType5,                                                                    \
                    FieldType6,                                                                    \
                    FieldType7,                                                                    \
                    FieldType8,                                                                    \
                    FieldType9,                                                                    \
                    FieldType10,                                                                   \
                    FieldType11,                                                                   \
                    FieldType12,                                                                   \
                    FieldType13,                                                                   \
                    FieldType14>                                                                   \
      __MessageSizeChecker__;                                                                      \
   template<class T>                                                                               \
   void Transfer(T& ar)                                                                            \
   {                                                                                               \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14; \
   }                                                                                               \
   DEFINE_DEBUG_MEMBERS14(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14)

#define DEFINE_MEMBERS15(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15)                \
   FieldType1                C1;                                                                          \
   FieldType2                C2;                                                                          \
   FieldType3                C3;                                                                          \
   FieldType4                C4;                                                                          \
   FieldType5                C5;                                                                          \
   FieldType6                C6;                                                                          \
   FieldType7                C7;                                                                          \
   FieldType8                C8;                                                                          \
   FieldType9                C9;                                                                          \
   FieldType10               C10;                                                                         \
   FieldType11               C11;                                                                         \
   FieldType12               C12;                                                                         \
   FieldType13               C13;                                                                         \
   FieldType14               C14;                                                                         \
   FieldType15               C15;                                                                         \
   static const unsigned int FieldCount = 15;                                                             \
   MessageSizeCheck<___RequiredMessageSize,                                                               \
                    FieldType1,                                                                           \
                    FieldType2,                                                                           \
                    FieldType3,                                                                           \
                    FieldType4,                                                                           \
                    FieldType5,                                                                           \
                    FieldType6,                                                                           \
                    FieldType7,                                                                           \
                    FieldType8,                                                                           \
                    FieldType9,                                                                           \
                    FieldType10,                                                                          \
                    FieldType11,                                                                          \
                    FieldType12,                                                                          \
                    FieldType13,                                                                          \
                    FieldType14,                                                                          \
                    FieldType15>                                                                          \
      __MessageSizeChecker__;                                                                             \
   template<class T>                                                                                      \
   void Transfer(T& ar)                                                                                   \
   {                                                                                                      \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15; \
   }                                                                                                      \
   DEFINE_DEBUG_MEMBERS15(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15)

#define DEFINE_MEMBERS16(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16)                  \
   FieldType1                C1;                                                                                 \
   FieldType2                C2;                                                                                 \
   FieldType3                C3;                                                                                 \
   FieldType4                C4;                                                                                 \
   FieldType5                C5;                                                                                 \
   FieldType6                C6;                                                                                 \
   FieldType7                C7;                                                                                 \
   FieldType8                C8;                                                                                 \
   FieldType9                C9;                                                                                 \
   FieldType10               C10;                                                                                \
   FieldType11               C11;                                                                                \
   FieldType12               C12;                                                                                \
   FieldType13               C13;                                                                                \
   FieldType14               C14;                                                                                \
   FieldType15               C15;                                                                                \
   FieldType16               C16;                                                                                \
   static const unsigned int FieldCount = 16;                                                                    \
   MessageSizeCheck<___RequiredMessageSize,                                                                      \
                    FieldType1,                                                                                  \
                    FieldType2,                                                                                  \
                    FieldType3,                                                                                  \
                    FieldType4,                                                                                  \
                    FieldType5,                                                                                  \
                    FieldType6,                                                                                  \
                    FieldType7,                                                                                  \
                    FieldType8,                                                                                  \
                    FieldType9,                                                                                  \
                    FieldType10,                                                                                 \
                    FieldType11,                                                                                 \
                    FieldType12,                                                                                 \
                    FieldType13,                                                                                 \
                    FieldType14,                                                                                 \
                    FieldType15,                                                                                 \
                    FieldType16>                                                                                 \
      __MessageSizeChecker__;                                                                                    \
   template<class T>                                                                                             \
   void Transfer(T& ar)                                                                                          \
   {                                                                                                             \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16; \
   }                                                                                                             \
   DEFINE_DEBUG_MEMBERS16(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16)

#define DEFINE_MEMBERS17(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17)                    \
   FieldType1                C1;                                                                                        \
   FieldType2                C2;                                                                                        \
   FieldType3                C3;                                                                                        \
   FieldType4                C4;                                                                                        \
   FieldType5                C5;                                                                                        \
   FieldType6                C6;                                                                                        \
   FieldType7                C7;                                                                                        \
   FieldType8                C8;                                                                                        \
   FieldType9                C9;                                                                                        \
   FieldType10               C10;                                                                                       \
   FieldType11               C11;                                                                                       \
   FieldType12               C12;                                                                                       \
   FieldType13               C13;                                                                                       \
   FieldType14               C14;                                                                                       \
   FieldType15               C15;                                                                                       \
   FieldType16               C16;                                                                                       \
   FieldType17               C17;                                                                                       \
   static const unsigned int FieldCount = 17;                                                                           \
   MessageSizeCheck<___RequiredMessageSize,                                                                             \
                    FieldType1,                                                                                         \
                    FieldType2,                                                                                         \
                    FieldType3,                                                                                         \
                    FieldType4,                                                                                         \
                    FieldType5,                                                                                         \
                    FieldType6,                                                                                         \
                    FieldType7,                                                                                         \
                    FieldType8,                                                                                         \
                    FieldType9,                                                                                         \
                    FieldType10,                                                                                        \
                    FieldType11,                                                                                        \
                    FieldType12,                                                                                        \
                    FieldType13,                                                                                        \
                    FieldType14,                                                                                        \
                    FieldType15,                                                                                        \
                    FieldType16,                                                                                        \
                    FieldType17>                                                                                        \
      __MessageSizeChecker__;                                                                                           \
   template<class T>                                                                                                    \
   void Transfer(T& ar)                                                                                                 \
   {                                                                                                                    \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16 << C17; \
   }                                                                                                                    \
   DEFINE_DEBUG_MEMBERS17(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17)

#define DEFINE_MEMBERS18(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18)              \
   FieldType1                C1;                                                                                       \
   FieldType2                C2;                                                                                       \
   FieldType3                C3;                                                                                       \
   FieldType4                C4;                                                                                       \
   FieldType5                C5;                                                                                       \
   FieldType6                C6;                                                                                       \
   FieldType7                C7;                                                                                       \
   FieldType8                C8;                                                                                       \
   FieldType9                C9;                                                                                       \
   FieldType10               C10;                                                                                      \
   FieldType11               C11;                                                                                      \
   FieldType12               C12;                                                                                      \
   FieldType13               C13;                                                                                      \
   FieldType14               C14;                                                                                      \
   FieldType15               C15;                                                                                      \
   FieldType16               C16;                                                                                      \
   FieldType17               C17;                                                                                      \
   FieldType18               C18;                                                                                      \
   static const unsigned int FieldCount = 18;                                                                          \
   MessageSizeCheck<___RequiredMessageSize,                                                                            \
                    FieldType1,                                                                                        \
                    FieldType2,                                                                                        \
                    FieldType3,                                                                                        \
                    FieldType4,                                                                                        \
                    FieldType5,                                                                                        \
                    FieldType6,                                                                                        \
                    FieldType7,                                                                                        \
                    FieldType8,                                                                                        \
                    FieldType9,                                                                                        \
                    FieldType10,                                                                                       \
                    FieldType11,                                                                                       \
                    FieldType12,                                                                                       \
                    FieldType13,                                                                                       \
                    FieldType14,                                                                                       \
                    FieldType15,                                                                                       \
                    FieldType16,                                                                                       \
                    FieldType17,                                                                                       \
                    FieldType18>                                                                                       \
      __MessageSizeChecker__;                                                                                          \
   template<class T>                                                                                                   \
   void Transfer(T& ar)                                                                                                \
   {                                                                                                                   \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16 << C17 \
         << C18;                                                                                                       \
   }                                                                                                                   \
   DEFINE_DEBUG_MEMBERS18(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18)

#define DEFINE_MEMBERS19(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19)         \
   FieldType1                C1;                                                                                       \
   FieldType2                C2;                                                                                       \
   FieldType3                C3;                                                                                       \
   FieldType4                C4;                                                                                       \
   FieldType5                C5;                                                                                       \
   FieldType6                C6;                                                                                       \
   FieldType7                C7;                                                                                       \
   FieldType8                C8;                                                                                       \
   FieldType9                C9;                                                                                       \
   FieldType10               C10;                                                                                      \
   FieldType11               C11;                                                                                      \
   FieldType12               C12;                                                                                      \
   FieldType13               C13;                                                                                      \
   FieldType14               C14;                                                                                      \
   FieldType15               C15;                                                                                      \
   FieldType16               C16;                                                                                      \
   FieldType17               C17;                                                                                      \
   FieldType18               C18;                                                                                      \
   FieldType19               C19;                                                                                      \
   static const unsigned int FieldCount = 19;                                                                          \
   MessageSizeCheck<___RequiredMessageSize,                                                                            \
                    FieldType1,                                                                                        \
                    FieldType2,                                                                                        \
                    FieldType3,                                                                                        \
                    FieldType4,                                                                                        \
                    FieldType5,                                                                                        \
                    FieldType6,                                                                                        \
                    FieldType7,                                                                                        \
                    FieldType8,                                                                                        \
                    FieldType9,                                                                                        \
                    FieldType10,                                                                                       \
                    FieldType11,                                                                                       \
                    FieldType12,                                                                                       \
                    FieldType13,                                                                                       \
                    FieldType14,                                                                                       \
                    FieldType15,                                                                                       \
                    FieldType16,                                                                                       \
                    FieldType17,                                                                                       \
                    FieldType18,                                                                                       \
                    FieldType19>                                                                                       \
      __MessageSizeChecker__;                                                                                          \
   template<class T>                                                                                                   \
   void Transfer(T& ar)                                                                                                \
   {                                                                                                                   \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16 << C17 \
         << C18 << C19;                                                                                                \
   }                                                                                                                   \
   DEFINE_DEBUG_MEMBERS19(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19)

#define DEFINE_MEMBERS20(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20)    \
   FieldType1                C1;                                                                                       \
   FieldType2                C2;                                                                                       \
   FieldType3                C3;                                                                                       \
   FieldType4                C4;                                                                                       \
   FieldType5                C5;                                                                                       \
   FieldType6                C6;                                                                                       \
   FieldType7                C7;                                                                                       \
   FieldType8                C8;                                                                                       \
   FieldType9                C9;                                                                                       \
   FieldType10               C10;                                                                                      \
   FieldType11               C11;                                                                                      \
   FieldType12               C12;                                                                                      \
   FieldType13               C13;                                                                                      \
   FieldType14               C14;                                                                                      \
   FieldType15               C15;                                                                                      \
   FieldType16               C16;                                                                                      \
   FieldType17               C17;                                                                                      \
   FieldType18               C18;                                                                                      \
   FieldType19               C19;                                                                                      \
   FieldType20               C20;                                                                                      \
   static const unsigned int FieldCount = 20;                                                                          \
   MessageSizeCheck<___RequiredMessageSize,                                                                            \
                    FieldType1,                                                                                        \
                    FieldType2,                                                                                        \
                    FieldType3,                                                                                        \
                    FieldType4,                                                                                        \
                    FieldType5,                                                                                        \
                    FieldType6,                                                                                        \
                    FieldType7,                                                                                        \
                    FieldType8,                                                                                        \
                    FieldType9,                                                                                        \
                    FieldType10,                                                                                       \
                    FieldType11,                                                                                       \
                    FieldType12,                                                                                       \
                    FieldType13,                                                                                       \
                    FieldType14,                                                                                       \
                    FieldType15,                                                                                       \
                    FieldType16,                                                                                       \
                    FieldType17,                                                                                       \
                    FieldType18,                                                                                       \
                    FieldType19,                                                                                       \
                    FieldType20>                                                                                       \
      __MessageSizeChecker__;                                                                                          \
   template<class T>                                                                                                   \
   void Transfer(T& ar)                                                                                                \
   {                                                                                                                   \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16 << C17 \
         << C18 << C19 << C20;                                                                                         \
   }                                                                                                                   \
   DEFINE_DEBUG_MEMBERS20(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20)

#define DEFINE_MEMBERS21(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21) \
   FieldType1                C1;                                                                                         \
   FieldType2                C2;                                                                                         \
   FieldType3                C3;                                                                                         \
   FieldType4                C4;                                                                                         \
   FieldType5                C5;                                                                                         \
   FieldType6                C6;                                                                                         \
   FieldType7                C7;                                                                                         \
   FieldType8                C8;                                                                                         \
   FieldType9                C9;                                                                                         \
   FieldType10               C10;                                                                                        \
   FieldType11               C11;                                                                                        \
   FieldType12               C12;                                                                                        \
   FieldType13               C13;                                                                                        \
   FieldType14               C14;                                                                                        \
   FieldType15               C15;                                                                                        \
   FieldType16               C16;                                                                                        \
   FieldType17               C17;                                                                                        \
   FieldType18               C18;                                                                                        \
   FieldType19               C19;                                                                                        \
   FieldType20               C20;                                                                                        \
   FieldType21               C21;                                                                                        \
   static const unsigned int FieldCount = 21;                                                                            \
   MessageSizeCheck<___RequiredMessageSize,                                                                              \
                    FieldType1,                                                                                          \
                    FieldType2,                                                                                          \
                    FieldType3,                                                                                          \
                    FieldType4,                                                                                          \
                    FieldType5,                                                                                          \
                    FieldType6,                                                                                          \
                    FieldType7,                                                                                          \
                    FieldType8,                                                                                          \
                    FieldType9,                                                                                          \
                    FieldType10,                                                                                         \
                    FieldType11,                                                                                         \
                    FieldType12,                                                                                         \
                    FieldType13,                                                                                         \
                    FieldType14,                                                                                         \
                    FieldType15,                                                                                         \
                    FieldType16,                                                                                         \
                    FieldType17,                                                                                         \
                    FieldType18,                                                                                         \
                    FieldType19,                                                                                         \
                    FieldType20,                                                                                         \
                    FieldType21>                                                                                         \
      __MessageSizeChecker__;                                                                                            \
   template<class T>                                                                                                     \
   void Transfer(T& ar)                                                                                                  \
   {                                                                                                                     \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16 << C17   \
         << C18 << C19 << C20 << C21;                                                                                    \
   }                                                                                                                     \
   DEFINE_DEBUG_MEMBERS21(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21)

#define DEFINE_MEMBERS22(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22) \
   FieldType1                C1;                                                                                              \
   FieldType2                C2;                                                                                              \
   FieldType3                C3;                                                                                              \
   FieldType4                C4;                                                                                              \
   FieldType5                C5;                                                                                              \
   FieldType6                C6;                                                                                              \
   FieldType7                C7;                                                                                              \
   FieldType8                C8;                                                                                              \
   FieldType9                C9;                                                                                              \
   FieldType10               C10;                                                                                             \
   FieldType11               C11;                                                                                             \
   FieldType12               C12;                                                                                             \
   FieldType13               C13;                                                                                             \
   FieldType14               C14;                                                                                             \
   FieldType15               C15;                                                                                             \
   FieldType16               C16;                                                                                             \
   FieldType17               C17;                                                                                             \
   FieldType18               C18;                                                                                             \
   FieldType19               C19;                                                                                             \
   FieldType20               C20;                                                                                             \
   FieldType21               C21;                                                                                             \
   FieldType22               C22;                                                                                             \
   static const unsigned int FieldCount = 22;                                                                                 \
   MessageSizeCheck<___RequiredMessageSize,                                                                                   \
                    FieldType1,                                                                                               \
                    FieldType2,                                                                                               \
                    FieldType3,                                                                                               \
                    FieldType4,                                                                                               \
                    FieldType5,                                                                                               \
                    FieldType6,                                                                                               \
                    FieldType7,                                                                                               \
                    FieldType8,                                                                                               \
                    FieldType9,                                                                                               \
                    FieldType10,                                                                                              \
                    FieldType11,                                                                                              \
                    FieldType12,                                                                                              \
                    FieldType13,                                                                                              \
                    FieldType14,                                                                                              \
                    FieldType15,                                                                                              \
                    FieldType16,                                                                                              \
                    FieldType17,                                                                                              \
                    FieldType18,                                                                                              \
                    FieldType19,                                                                                              \
                    FieldType20,                                                                                              \
                    FieldType21,                                                                                              \
                    FieldType22>                                                                                              \
      __MessageSizeChecker__;                                                                                                 \
   template<class T>                                                                                                          \
   void Transfer(T& ar)                                                                                                       \
   {                                                                                                                          \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16 << C17        \
         << C18 << C19 << C20 << C21 << C22;                                                                                  \
   }                                                                                                                          \
   DEFINE_DEBUG_MEMBERS22(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22)

#define DEFINE_MEMBERS23(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22, C23) \
   FieldType1                C1;                                                                                                   \
   FieldType2                C2;                                                                                                   \
   FieldType3                C3;                                                                                                   \
   FieldType4                C4;                                                                                                   \
   FieldType5                C5;                                                                                                   \
   FieldType6                C6;                                                                                                   \
   FieldType7                C7;                                                                                                   \
   FieldType8                C8;                                                                                                   \
   FieldType9                C9;                                                                                                   \
   FieldType10               C10;                                                                                                  \
   FieldType11               C11;                                                                                                  \
   FieldType12               C12;                                                                                                  \
   FieldType13               C13;                                                                                                  \
   FieldType14               C14;                                                                                                  \
   FieldType15               C15;                                                                                                  \
   FieldType16               C16;                                                                                                  \
   FieldType17               C17;                                                                                                  \
   FieldType18               C18;                                                                                                  \
   FieldType19               C19;                                                                                                  \
   FieldType20               C20;                                                                                                  \
   FieldType21               C21;                                                                                                  \
   FieldType22               C22;                                                                                                  \
   FieldType23               C23;                                                                                                  \
   static const unsigned int FieldCount = 23;                                                                                      \
   MessageSizeCheck<___RequiredMessageSize,                                                                                        \
                    FieldType1,                                                                                                    \
                    FieldType2,                                                                                                    \
                    FieldType3,                                                                                                    \
                    FieldType4,                                                                                                    \
                    FieldType5,                                                                                                    \
                    FieldType6,                                                                                                    \
                    FieldType7,                                                                                                    \
                    FieldType8,                                                                                                    \
                    FieldType9,                                                                                                    \
                    FieldType10,                                                                                                   \
                    FieldType11,                                                                                                   \
                    FieldType12,                                                                                                   \
                    FieldType13,                                                                                                   \
                    FieldType14,                                                                                                   \
                    FieldType15,                                                                                                   \
                    FieldType16,                                                                                                   \
                    FieldType17,                                                                                                   \
                    FieldType18,                                                                                                   \
                    FieldType19,                                                                                                   \
                    FieldType20,                                                                                                   \
                    FieldType21,                                                                                                   \
                    FieldType22,                                                                                                   \
                    FieldType23>                                                                                                   \
      __MessageSizeChecker__;                                                                                                      \
   template<class T>                                                                                                               \
   void Transfer(T& ar)                                                                                                            \
   {                                                                                                                               \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16 << C17             \
         << C18 << C19 << C20 << C21 << C22 << C23;                                                                                \
   }                                                                                                                               \
   DEFINE_DEBUG_MEMBERS23(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22, C23)

#define DEFINE_MEMBERS24(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22, C23, C24) \
   FieldType1                C1;                                                                                                        \
   FieldType2                C2;                                                                                                        \
   FieldType3                C3;                                                                                                        \
   FieldType4                C4;                                                                                                        \
   FieldType5                C5;                                                                                                        \
   FieldType6                C6;                                                                                                        \
   FieldType7                C7;                                                                                                        \
   FieldType8                C8;                                                                                                        \
   FieldType9                C9;                                                                                                        \
   FieldType10               C10;                                                                                                       \
   FieldType11               C11;                                                                                                       \
   FieldType12               C12;                                                                                                       \
   FieldType13               C13;                                                                                                       \
   FieldType14               C14;                                                                                                       \
   FieldType15               C15;                                                                                                       \
   FieldType16               C16;                                                                                                       \
   FieldType17               C17;                                                                                                       \
   FieldType18               C18;                                                                                                       \
   FieldType19               C19;                                                                                                       \
   FieldType20               C20;                                                                                                       \
   FieldType21               C21;                                                                                                       \
   FieldType22               C22;                                                                                                       \
   FieldType23               C23;                                                                                                       \
   FieldType24               C24;                                                                                                       \
   static const unsigned int FieldCount = 24;                                                                                           \
   MessageSizeCheck<___RequiredMessageSize,                                                                                             \
                    FieldType1,                                                                                                         \
                    FieldType2,                                                                                                         \
                    FieldType3,                                                                                                         \
                    FieldType4,                                                                                                         \
                    FieldType5,                                                                                                         \
                    FieldType6,                                                                                                         \
                    FieldType7,                                                                                                         \
                    FieldType8,                                                                                                         \
                    FieldType9,                                                                                                         \
                    FieldType10,                                                                                                        \
                    FieldType11,                                                                                                        \
                    FieldType12,                                                                                                        \
                    FieldType13,                                                                                                        \
                    FieldType14,                                                                                                        \
                    FieldType15,                                                                                                        \
                    FieldType16,                                                                                                        \
                    FieldType17,                                                                                                        \
                    FieldType18,                                                                                                        \
                    FieldType19,                                                                                                        \
                    FieldType20,                                                                                                        \
                    FieldType21,                                                                                                        \
                    FieldType22,                                                                                                        \
                    FieldType23,                                                                                                        \
                    FieldType24>                                                                                                        \
      __MessageSizeChecker__;                                                                                                           \
   template<class T>                                                                                                                    \
   void Transfer(T& ar)                                                                                                                 \
   {                                                                                                                                    \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16 << C17                  \
         << C18 << C19 << C20 << C21 << C22 << C23 << C24;                                                                              \
   }                                                                                                                                    \
   DEFINE_DEBUG_MEMBERS24(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22, C23, C24)

#define DEFINE_MEMBERS25(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22, C23, C24, C25) \
   FieldType1                C1;                                                                                                             \
   FieldType2                C2;                                                                                                             \
   FieldType3                C3;                                                                                                             \
   FieldType4                C4;                                                                                                             \
   FieldType5                C5;                                                                                                             \
   FieldType6                C6;                                                                                                             \
   FieldType7                C7;                                                                                                             \
   FieldType8                C8;                                                                                                             \
   FieldType9                C9;                                                                                                             \
   FieldType10               C10;                                                                                                            \
   FieldType11               C11;                                                                                                            \
   FieldType12               C12;                                                                                                            \
   FieldType13               C13;                                                                                                            \
   FieldType14               C14;                                                                                                            \
   FieldType15               C15;                                                                                                            \
   FieldType16               C16;                                                                                                            \
   FieldType17               C17;                                                                                                            \
   FieldType18               C18;                                                                                                            \
   FieldType19               C19;                                                                                                            \
   FieldType20               C20;                                                                                                            \
   FieldType21               C21;                                                                                                            \
   FieldType22               C22;                                                                                                            \
   FieldType23               C23;                                                                                                            \
   FieldType24               C24;                                                                                                            \
   FieldType25               C25;                                                                                                            \
   static const unsigned int FieldCount = 25;                                                                                                \
   MessageSizeCheck<___RequiredMessageSize,                                                                                                  \
                    FieldType1,                                                                                                              \
                    FieldType2,                                                                                                              \
                    FieldType3,                                                                                                              \
                    FieldType4,                                                                                                              \
                    FieldType5,                                                                                                              \
                    FieldType6,                                                                                                              \
                    FieldType7,                                                                                                              \
                    FieldType8,                                                                                                              \
                    FieldType9,                                                                                                              \
                    FieldType10,                                                                                                             \
                    FieldType11,                                                                                                             \
                    FieldType12,                                                                                                             \
                    FieldType13,                                                                                                             \
                    FieldType14,                                                                                                             \
                    FieldType15,                                                                                                             \
                    FieldType16,                                                                                                             \
                    FieldType17,                                                                                                             \
                    FieldType18,                                                                                                             \
                    FieldType19,                                                                                                             \
                    FieldType20,                                                                                                             \
                    FieldType21,                                                                                                             \
                    FieldType22,                                                                                                             \
                    FieldType23,                                                                                                             \
                    FieldType24,                                                                                                             \
                    FieldType25>                                                                                                             \
      __MessageSizeChecker__;                                                                                                                \
   template<class T>                                                                                                                         \
   void Transfer(T& ar)                                                                                                                      \
   {                                                                                                                                         \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16 << C17                       \
         << C18 << C19 << C20 << C21 << C22 << C23 << C24 << C25;                                                                            \
   }                                                                                                                                         \
   DEFINE_DEBUG_MEMBERS25(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22, C23, C24, C25)

#define DEFINE_MEMBERS26(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22, C23, C24, C25, C26) \
   FieldType1                C1;                                                                                                                  \
   FieldType2                C2;                                                                                                                  \
   FieldType3                C3;                                                                                                                  \
   FieldType4                C4;                                                                                                                  \
   FieldType5                C5;                                                                                                                  \
   FieldType6                C6;                                                                                                                  \
   FieldType7                C7;                                                                                                                  \
   FieldType8                C8;                                                                                                                  \
   FieldType9                C9;                                                                                                                  \
   FieldType10               C10;                                                                                                                 \
   FieldType11               C11;                                                                                                                 \
   FieldType12               C12;                                                                                                                 \
   FieldType13               C13;                                                                                                                 \
   FieldType14               C14;                                                                                                                 \
   FieldType15               C15;                                                                                                                 \
   FieldType16               C16;                                                                                                                 \
   FieldType17               C17;                                                                                                                 \
   FieldType18               C18;                                                                                                                 \
   FieldType19               C19;                                                                                                                 \
   FieldType20               C20;                                                                                                                 \
   FieldType21               C21;                                                                                                                 \
   FieldType22               C22;                                                                                                                 \
   FieldType23               C23;                                                                                                                 \
   FieldType24               C24;                                                                                                                 \
   FieldType25               C25;                                                                                                                 \
   FieldType26               C26;                                                                                                                 \
   static const unsigned int FieldCount = 26;                                                                                                     \
   MessageSizeCheck<___RequiredMessageSize,                                                                                                       \
                    FieldType1,                                                                                                                   \
                    FieldType2,                                                                                                                   \
                    FieldType3,                                                                                                                   \
                    FieldType4,                                                                                                                   \
                    FieldType5,                                                                                                                   \
                    FieldType6,                                                                                                                   \
                    FieldType7,                                                                                                                   \
                    FieldType8,                                                                                                                   \
                    FieldType9,                                                                                                                   \
                    FieldType10,                                                                                                                  \
                    FieldType11,                                                                                                                  \
                    FieldType12,                                                                                                                  \
                    FieldType13,                                                                                                                  \
                    FieldType14,                                                                                                                  \
                    FieldType15,                                                                                                                  \
                    FieldType16,                                                                                                                  \
                    FieldType17,                                                                                                                  \
                    FieldType18,                                                                                                                  \
                    FieldType19,                                                                                                                  \
                    FieldType20,                                                                                                                  \
                    FieldType21,                                                                                                                  \
                    FieldType22,                                                                                                                  \
                    FieldType23,                                                                                                                  \
                    FieldType24,                                                                                                                  \
                    FieldType25,                                                                                                                  \
                    FieldType26>                                                                                                                  \
      __MessageSizeChecker__;                                                                                                                     \
   template<class T>                                                                                                                              \
   void Transfer(T& ar)                                                                                                                           \
   {                                                                                                                                              \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16 << C17                            \
         << C18 << C19 << C20 << C21 << C22 << C23 << C24 << C25 << C26;                                                                          \
   }                                                                                                                                              \
   DEFINE_DEBUG_MEMBERS26(C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22, C23, C24, C25, C26)

#define DEFINE_MEMBERS27(C1,                                                                                           \
                         C2,                                                                                           \
                         C3,                                                                                           \
                         C4,                                                                                           \
                         C5,                                                                                           \
                         C6,                                                                                           \
                         C7,                                                                                           \
                         C8,                                                                                           \
                         C9,                                                                                           \
                         C10,                                                                                          \
                         C11,                                                                                          \
                         C12,                                                                                          \
                         C13,                                                                                          \
                         C14,                                                                                          \
                         C15,                                                                                          \
                         C16,                                                                                          \
                         C17,                                                                                          \
                         C18,                                                                                          \
                         C19,                                                                                          \
                         C20,                                                                                          \
                         C21,                                                                                          \
                         C22,                                                                                          \
                         C23,                                                                                          \
                         C24,                                                                                          \
                         C25,                                                                                          \
                         C26,                                                                                          \
                         C27)                                                                                          \
   FieldType1                C1;                                                                                       \
   FieldType2                C2;                                                                                       \
   FieldType3                C3;                                                                                       \
   FieldType4                C4;                                                                                       \
   FieldType5                C5;                                                                                       \
   FieldType6                C6;                                                                                       \
   FieldType7                C7;                                                                                       \
   FieldType8                C8;                                                                                       \
   FieldType9                C9;                                                                                       \
   FieldType10               C10;                                                                                      \
   FieldType11               C11;                                                                                      \
   FieldType12               C12;                                                                                      \
   FieldType13               C13;                                                                                      \
   FieldType14               C14;                                                                                      \
   FieldType15               C15;                                                                                      \
   FieldType16               C16;                                                                                      \
   FieldType17               C17;                                                                                      \
   FieldType18               C18;                                                                                      \
   FieldType19               C19;                                                                                      \
   FieldType20               C20;                                                                                      \
   FieldType21               C21;                                                                                      \
   FieldType22               C22;                                                                                      \
   FieldType23               C23;                                                                                      \
   FieldType24               C24;                                                                                      \
   FieldType25               C25;                                                                                      \
   FieldType26               C26;                                                                                      \
   FieldType27               C27;                                                                                      \
   static const unsigned int FieldCount = 27;                                                                          \
   MessageSizeCheck<___RequiredMessageSize,                                                                            \
                    FieldType1,                                                                                        \
                    FieldType2,                                                                                        \
                    FieldType3,                                                                                        \
                    FieldType4,                                                                                        \
                    FieldType5,                                                                                        \
                    FieldType6,                                                                                        \
                    FieldType7,                                                                                        \
                    FieldType8,                                                                                        \
                    FieldType9,                                                                                        \
                    FieldType10,                                                                                       \
                    FieldType11,                                                                                       \
                    FieldType12,                                                                                       \
                    FieldType13,                                                                                       \
                    FieldType14,                                                                                       \
                    FieldType15,                                                                                       \
                    FieldType16,                                                                                       \
                    FieldType17,                                                                                       \
                    FieldType18,                                                                                       \
                    FieldType19,                                                                                       \
                    FieldType20,                                                                                       \
                    FieldType21,                                                                                       \
                    FieldType22,                                                                                       \
                    FieldType23,                                                                                       \
                    FieldType24,                                                                                       \
                    FieldType25,                                                                                       \
                    FieldType26,                                                                                       \
                    FieldType27>                                                                                       \
      __MessageSizeChecker__;                                                                                          \
   template<class T>                                                                                                   \
   void Transfer(T& ar)                                                                                                \
   {                                                                                                                   \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16 << C17 \
         << C18 << C19 << C20 << C21 << C22 << C23 << C24 << C25 << C26 << C27;                                        \
   }                                                                                                                   \
   DEFINE_DEBUG_MEMBERS27(C1,                                                                                          \
                          C2,                                                                                          \
                          C3,                                                                                          \
                          C4,                                                                                          \
                          C5,                                                                                          \
                          C6,                                                                                          \
                          C7,                                                                                          \
                          C8,                                                                                          \
                          C9,                                                                                          \
                          C10,                                                                                         \
                          C11,                                                                                         \
                          C12,                                                                                         \
                          C13,                                                                                         \
                          C14,                                                                                         \
                          C15,                                                                                         \
                          C16,                                                                                         \
                          C17,                                                                                         \
                          C18,                                                                                         \
                          C19,                                                                                         \
                          C20,                                                                                         \
                          C21,                                                                                         \
                          C22,                                                                                         \
                          C23,                                                                                         \
                          C24,                                                                                         \
                          C25,                                                                                         \
                          C26,                                                                                         \
                          C27)

#define DEFINE_MEMBERS28(C1,                                                                                           \
                         C2,                                                                                           \
                         C3,                                                                                           \
                         C4,                                                                                           \
                         C5,                                                                                           \
                         C6,                                                                                           \
                         C7,                                                                                           \
                         C8,                                                                                           \
                         C9,                                                                                           \
                         C10,                                                                                          \
                         C11,                                                                                          \
                         C12,                                                                                          \
                         C13,                                                                                          \
                         C14,                                                                                          \
                         C15,                                                                                          \
                         C16,                                                                                          \
                         C17,                                                                                          \
                         C18,                                                                                          \
                         C19,                                                                                          \
                         C20,                                                                                          \
                         C21,                                                                                          \
                         C22,                                                                                          \
                         C23,                                                                                          \
                         C24,                                                                                          \
                         C25,                                                                                          \
                         C26,                                                                                          \
                         C27,                                                                                          \
                         C28)                                                                                          \
   FieldType1                C1;                                                                                       \
   FieldType2                C2;                                                                                       \
   FieldType3                C3;                                                                                       \
   FieldType4                C4;                                                                                       \
   FieldType5                C5;                                                                                       \
   FieldType6                C6;                                                                                       \
   FieldType7                C7;                                                                                       \
   FieldType8                C8;                                                                                       \
   FieldType9                C9;                                                                                       \
   FieldType10               C10;                                                                                      \
   FieldType11               C11;                                                                                      \
   FieldType12               C12;                                                                                      \
   FieldType13               C13;                                                                                      \
   FieldType14               C14;                                                                                      \
   FieldType15               C15;                                                                                      \
   FieldType16               C16;                                                                                      \
   FieldType17               C17;                                                                                      \
   FieldType18               C18;                                                                                      \
   FieldType19               C19;                                                                                      \
   FieldType20               C20;                                                                                      \
   FieldType21               C21;                                                                                      \
   FieldType22               C22;                                                                                      \
   FieldType23               C23;                                                                                      \
   FieldType24               C24;                                                                                      \
   FieldType25               C25;                                                                                      \
   FieldType26               C26;                                                                                      \
   FieldType27               C27;                                                                                      \
   FieldType28               C28;                                                                                      \
   static const unsigned int FieldCount = 28;                                                                          \
   MessageSizeCheck<___RequiredMessageSize,                                                                            \
                    FieldType1,                                                                                        \
                    FieldType2,                                                                                        \
                    FieldType3,                                                                                        \
                    FieldType4,                                                                                        \
                    FieldType5,                                                                                        \
                    FieldType6,                                                                                        \
                    FieldType7,                                                                                        \
                    FieldType8,                                                                                        \
                    FieldType9,                                                                                        \
                    FieldType10,                                                                                       \
                    FieldType11,                                                                                       \
                    FieldType12,                                                                                       \
                    FieldType13,                                                                                       \
                    FieldType14,                                                                                       \
                    FieldType15,                                                                                       \
                    FieldType16,                                                                                       \
                    FieldType17,                                                                                       \
                    FieldType18,                                                                                       \
                    FieldType19,                                                                                       \
                    FieldType20,                                                                                       \
                    FieldType21,                                                                                       \
                    FieldType22,                                                                                       \
                    FieldType23,                                                                                       \
                    FieldType24,                                                                                       \
                    FieldType25,                                                                                       \
                    FieldType26,                                                                                       \
                    FieldType27,                                                                                       \
                    FieldType28>                                                                                       \
      __MessageSizeChecker__;                                                                                          \
   template<class T>                                                                                                   \
   void Transfer(T& ar)                                                                                                \
   {                                                                                                                   \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16 << C17 \
         << C18 << C19 << C20 << C21 << C22 << C23 << C24 << C25 << C26 << C27 << C28;                                 \
   }                                                                                                                   \
   DEFINE_DEBUG_MEMBERS28(C1,                                                                                          \
                          C2,                                                                                          \
                          C3,                                                                                          \
                          C4,                                                                                          \
                          C5,                                                                                          \
                          C6,                                                                                          \
                          C7,                                                                                          \
                          C8,                                                                                          \
                          C9,                                                                                          \
                          C10,                                                                                         \
                          C11,                                                                                         \
                          C12,                                                                                         \
                          C13,                                                                                         \
                          C14,                                                                                         \
                          C15,                                                                                         \
                          C16,                                                                                         \
                          C17,                                                                                         \
                          C18,                                                                                         \
                          C19,                                                                                         \
                          C20,                                                                                         \
                          C21,                                                                                         \
                          C22,                                                                                         \
                          C23,                                                                                         \
                          C24,                                                                                         \
                          C25,                                                                                         \
                          C26,                                                                                         \
                          C27,                                                                                         \
                          C28)

#define DEFINE_MEMBERS29(C1,                                                                                           \
                         C2,                                                                                           \
                         C3,                                                                                           \
                         C4,                                                                                           \
                         C5,                                                                                           \
                         C6,                                                                                           \
                         C7,                                                                                           \
                         C8,                                                                                           \
                         C9,                                                                                           \
                         C10,                                                                                          \
                         C11,                                                                                          \
                         C12,                                                                                          \
                         C13,                                                                                          \
                         C14,                                                                                          \
                         C15,                                                                                          \
                         C16,                                                                                          \
                         C17,                                                                                          \
                         C18,                                                                                          \
                         C19,                                                                                          \
                         C20,                                                                                          \
                         C21,                                                                                          \
                         C22,                                                                                          \
                         C23,                                                                                          \
                         C24,                                                                                          \
                         C25,                                                                                          \
                         C26,                                                                                          \
                         C27,                                                                                          \
                         C28,                                                                                          \
                         C29)                                                                                          \
   FieldType1                C1;                                                                                       \
   FieldType2                C2;                                                                                       \
   FieldType3                C3;                                                                                       \
   FieldType4                C4;                                                                                       \
   FieldType5                C5;                                                                                       \
   FieldType6                C6;                                                                                       \
   FieldType7                C7;                                                                                       \
   FieldType8                C8;                                                                                       \
   FieldType9                C9;                                                                                       \
   FieldType10               C10;                                                                                      \
   FieldType11               C11;                                                                                      \
   FieldType12               C12;                                                                                      \
   FieldType13               C13;                                                                                      \
   FieldType14               C14;                                                                                      \
   FieldType15               C15;                                                                                      \
   FieldType16               C16;                                                                                      \
   FieldType17               C17;                                                                                      \
   FieldType18               C18;                                                                                      \
   FieldType19               C19;                                                                                      \
   FieldType20               C20;                                                                                      \
   FieldType21               C21;                                                                                      \
   FieldType22               C22;                                                                                      \
   FieldType23               C23;                                                                                      \
   FieldType24               C24;                                                                                      \
   FieldType25               C25;                                                                                      \
   FieldType26               C26;                                                                                      \
   FieldType27               C27;                                                                                      \
   FieldType28               C28;                                                                                      \
   FieldType29               C29;                                                                                      \
   static const unsigned int FieldCount = 29;                                                                          \
   MessageSizeCheck<___RequiredMessageSize,                                                                            \
                    FieldType1,                                                                                        \
                    FieldType2,                                                                                        \
                    FieldType3,                                                                                        \
                    FieldType4,                                                                                        \
                    FieldType5,                                                                                        \
                    FieldType6,                                                                                        \
                    FieldType7,                                                                                        \
                    FieldType8,                                                                                        \
                    FieldType9,                                                                                        \
                    FieldType10,                                                                                       \
                    FieldType11,                                                                                       \
                    FieldType12,                                                                                       \
                    FieldType13,                                                                                       \
                    FieldType14,                                                                                       \
                    FieldType15,                                                                                       \
                    FieldType16,                                                                                       \
                    FieldType17,                                                                                       \
                    FieldType18,                                                                                       \
                    FieldType19,                                                                                       \
                    FieldType20,                                                                                       \
                    FieldType21,                                                                                       \
                    FieldType22,                                                                                       \
                    FieldType23,                                                                                       \
                    FieldType24,                                                                                       \
                    FieldType25,                                                                                       \
                    FieldType26,                                                                                       \
                    FieldType27,                                                                                       \
                    FieldType28,                                                                                       \
                    FieldType29>                                                                                       \
      __MessageSizeChecker__;                                                                                          \
   template<class T>                                                                                                   \
   void Transfer(T& ar)                                                                                                \
   {                                                                                                                   \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16 << C17 \
         << C18 << C19 << C20 << C21 << C22 << C23 << C24 << C25 << C26 << C27 << C28 << C29;                          \
   }                                                                                                                   \
   DEFINE_DEBUG_MEMBERS29(C1,                                                                                          \
                          C2,                                                                                          \
                          C3,                                                                                          \
                          C4,                                                                                          \
                          C5,                                                                                          \
                          C6,                                                                                          \
                          C7,                                                                                          \
                          C8,                                                                                          \
                          C9,                                                                                          \
                          C10,                                                                                         \
                          C11,                                                                                         \
                          C12,                                                                                         \
                          C13,                                                                                         \
                          C14,                                                                                         \
                          C15,                                                                                         \
                          C16,                                                                                         \
                          C17,                                                                                         \
                          C18,                                                                                         \
                          C19,                                                                                         \
                          C20,                                                                                         \
                          C21,                                                                                         \
                          C22,                                                                                         \
                          C23,                                                                                         \
                          C24,                                                                                         \
                          C25,                                                                                         \
                          C26,                                                                                         \
                          C27,                                                                                         \
                          C28,                                                                                         \
                          C29)

#define DEFINE_MEMBERS30(C1,                                                                                           \
                         C2,                                                                                           \
                         C3,                                                                                           \
                         C4,                                                                                           \
                         C5,                                                                                           \
                         C6,                                                                                           \
                         C7,                                                                                           \
                         C8,                                                                                           \
                         C9,                                                                                           \
                         C10,                                                                                          \
                         C11,                                                                                          \
                         C12,                                                                                          \
                         C13,                                                                                          \
                         C14,                                                                                          \
                         C15,                                                                                          \
                         C16,                                                                                          \
                         C17,                                                                                          \
                         C18,                                                                                          \
                         C19,                                                                                          \
                         C20,                                                                                          \
                         C21,                                                                                          \
                         C22,                                                                                          \
                         C23,                                                                                          \
                         C24,                                                                                          \
                         C25,                                                                                          \
                         C26,                                                                                          \
                         C27,                                                                                          \
                         C28,                                                                                          \
                         C29,                                                                                          \
                         C30)                                                                                          \
   FieldType1                C1;                                                                                       \
   FieldType2                C2;                                                                                       \
   FieldType3                C3;                                                                                       \
   FieldType4                C4;                                                                                       \
   FieldType5                C5;                                                                                       \
   FieldType6                C6;                                                                                       \
   FieldType7                C7;                                                                                       \
   FieldType8                C8;                                                                                       \
   FieldType9                C9;                                                                                       \
   FieldType10               C10;                                                                                      \
   FieldType11               C11;                                                                                      \
   FieldType12               C12;                                                                                      \
   FieldType13               C13;                                                                                      \
   FieldType14               C14;                                                                                      \
   FieldType15               C15;                                                                                      \
   FieldType16               C16;                                                                                      \
   FieldType17               C17;                                                                                      \
   FieldType18               C18;                                                                                      \
   FieldType19               C19;                                                                                      \
   FieldType20               C20;                                                                                      \
   FieldType21               C21;                                                                                      \
   FieldType22               C22;                                                                                      \
   FieldType23               C23;                                                                                      \
   FieldType24               C24;                                                                                      \
   FieldType25               C25;                                                                                      \
   FieldType26               C26;                                                                                      \
   FieldType27               C27;                                                                                      \
   FieldType28               C28;                                                                                      \
   FieldType29               C29;                                                                                      \
   FieldType30               C30;                                                                                      \
   static const unsigned int FieldCount = 30;                                                                          \
   MessageSizeCheck<___RequiredMessageSize,                                                                            \
                    FieldType1,                                                                                        \
                    FieldType2,                                                                                        \
                    FieldType3,                                                                                        \
                    FieldType4,                                                                                        \
                    FieldType5,                                                                                        \
                    FieldType6,                                                                                        \
                    FieldType7,                                                                                        \
                    FieldType8,                                                                                        \
                    FieldType9,                                                                                        \
                    FieldType10,                                                                                       \
                    FieldType11,                                                                                       \
                    FieldType12,                                                                                       \
                    FieldType13,                                                                                       \
                    FieldType14,                                                                                       \
                    FieldType15,                                                                                       \
                    FieldType16,                                                                                       \
                    FieldType17,                                                                                       \
                    FieldType18,                                                                                       \
                    FieldType19,                                                                                       \
                    FieldType20,                                                                                       \
                    FieldType21,                                                                                       \
                    FieldType22,                                                                                       \
                    FieldType23,                                                                                       \
                    FieldType24,                                                                                       \
                    FieldType25,                                                                                       \
                    FieldType26,                                                                                       \
                    FieldType27,                                                                                       \
                    FieldType28,                                                                                       \
                    FieldType29,                                                                                       \
                    FieldType30>                                                                                       \
      __MessageSizeChecker__;                                                                                          \
   template<class T>                                                                                                   \
   void Transfer(T& ar)                                                                                                \
   {                                                                                                                   \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16 << C17 \
         << C18 << C19 << C20 << C21 << C22 << C23 << C24 << C25 << C26 << C27 << C28 << C29 << C30;                   \
   }                                                                                                                   \
   DEFINE_DEBUG_MEMBERS30(C1,                                                                                          \
                          C2,                                                                                          \
                          C3,                                                                                          \
                          C4,                                                                                          \
                          C5,                                                                                          \
                          C6,                                                                                          \
                          C7,                                                                                          \
                          C8,                                                                                          \
                          C9,                                                                                          \
                          C10,                                                                                         \
                          C11,                                                                                         \
                          C12,                                                                                         \
                          C13,                                                                                         \
                          C14,                                                                                         \
                          C15,                                                                                         \
                          C16,                                                                                         \
                          C17,                                                                                         \
                          C18,                                                                                         \
                          C19,                                                                                         \
                          C20,                                                                                         \
                          C21,                                                                                         \
                          C22,                                                                                         \
                          C23,                                                                                         \
                          C24,                                                                                         \
                          C25,                                                                                         \
                          C26,                                                                                         \
                          C27,                                                                                         \
                          C28,                                                                                         \
                          C29,                                                                                         \
                          C30)

#define DEFINE_MEMBERS31(C1,                                                                                           \
                         C2,                                                                                           \
                         C3,                                                                                           \
                         C4,                                                                                           \
                         C5,                                                                                           \
                         C6,                                                                                           \
                         C7,                                                                                           \
                         C8,                                                                                           \
                         C9,                                                                                           \
                         C10,                                                                                          \
                         C11,                                                                                          \
                         C12,                                                                                          \
                         C13,                                                                                          \
                         C14,                                                                                          \
                         C15,                                                                                          \
                         C16,                                                                                          \
                         C17,                                                                                          \
                         C18,                                                                                          \
                         C19,                                                                                          \
                         C20,                                                                                          \
                         C21,                                                                                          \
                         C22,                                                                                          \
                         C23,                                                                                          \
                         C24,                                                                                          \
                         C25,                                                                                          \
                         C26,                                                                                          \
                         C27,                                                                                          \
                         C28,                                                                                          \
                         C29,                                                                                          \
                         C30,                                                                                          \
                         C31)                                                                                          \
   FieldType1                C1;                                                                                       \
   FieldType2                C2;                                                                                       \
   FieldType3                C3;                                                                                       \
   FieldType4                C4;                                                                                       \
   FieldType5                C5;                                                                                       \
   FieldType6                C6;                                                                                       \
   FieldType7                C7;                                                                                       \
   FieldType8                C8;                                                                                       \
   FieldType9                C9;                                                                                       \
   FieldType10               C10;                                                                                      \
   FieldType11               C11;                                                                                      \
   FieldType12               C12;                                                                                      \
   FieldType13               C13;                                                                                      \
   FieldType14               C14;                                                                                      \
   FieldType15               C15;                                                                                      \
   FieldType16               C16;                                                                                      \
   FieldType17               C17;                                                                                      \
   FieldType18               C18;                                                                                      \
   FieldType19               C19;                                                                                      \
   FieldType20               C20;                                                                                      \
   FieldType21               C21;                                                                                      \
   FieldType22               C22;                                                                                      \
   FieldType23               C23;                                                                                      \
   FieldType24               C24;                                                                                      \
   FieldType25               C25;                                                                                      \
   FieldType26               C26;                                                                                      \
   FieldType27               C27;                                                                                      \
   FieldType28               C28;                                                                                      \
   FieldType29               C29;                                                                                      \
   FieldType30               C30;                                                                                      \
   FieldType31               C31;                                                                                      \
   static const unsigned int FieldCount = 31;                                                                          \
   MessageSizeCheck<___RequiredMessageSize,                                                                            \
                    FieldType1,                                                                                        \
                    FieldType2,                                                                                        \
                    FieldType3,                                                                                        \
                    FieldType4,                                                                                        \
                    FieldType5,                                                                                        \
                    FieldType6,                                                                                        \
                    FieldType7,                                                                                        \
                    FieldType8,                                                                                        \
                    FieldType9,                                                                                        \
                    FieldType10,                                                                                       \
                    FieldType11,                                                                                       \
                    FieldType12,                                                                                       \
                    FieldType13,                                                                                       \
                    FieldType14,                                                                                       \
                    FieldType15,                                                                                       \
                    FieldType16,                                                                                       \
                    FieldType17,                                                                                       \
                    FieldType18,                                                                                       \
                    FieldType19,                                                                                       \
                    FieldType20,                                                                                       \
                    FieldType21,                                                                                       \
                    FieldType22,                                                                                       \
                    FieldType23,                                                                                       \
                    FieldType24,                                                                                       \
                    FieldType25,                                                                                       \
                    FieldType26,                                                                                       \
                    FieldType27,                                                                                       \
                    FieldType28,                                                                                       \
                    FieldType29,                                                                                       \
                    FieldType30,                                                                                       \
                    FieldType31>                                                                                       \
      __MessageSizeChecker__;                                                                                          \
   template<class T>                                                                                                   \
   void Transfer(T& ar)                                                                                                \
   {                                                                                                                   \
      ar << C1 << C2 << C3 << C4 << C5 << C6 << C7 << C8 << C9 << C10 << C11 << C12 << C13 << C14 << C15 << C16 << C17 \
         << C18 << C19 << C20 << C21 << C22 << C23 << C24 << C25 << C26 << C27 << C28 << C29 << C30 << C31;            \
   }                                                                                                                   \
   DEFINE_DEBUG_MEMBERS31(C1,                                                                                          \
                          C2,                                                                                          \
                          C3,                                                                                          \
                          C4,                                                                                          \
                          C5,                                                                                          \
                          C6,                                                                                          \
                          C7,                                                                                          \
                          C8,                                                                                          \
                          C9,                                                                                          \
                          C10,                                                                                         \
                          C11,                                                                                         \
                          C12,                                                                                         \
                          C13,                                                                                         \
                          C14,                                                                                         \
                          C15,                                                                                         \
                          C16,                                                                                         \
                          C17,                                                                                         \
                          C18,                                                                                         \
                          C19,                                                                                         \
                          C20,                                                                                         \
                          C21,                                                                                         \
                          C22,                                                                                         \
                          C23,                                                                                         \
                          C24,                                                                                         \
                          C25,                                                                                         \
                          C26,                                                                                         \
                          C27,                                                                                         \
                          C28,                                                                                         \
                          C29,                                                                                         \
                          C30,                                                                                         \
                          C31)
