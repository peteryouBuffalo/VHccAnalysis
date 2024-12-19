// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME Reader_dict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "ROOT/RConfig.hxx"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "src/Reader.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_Reader(void *p = nullptr);
   static void *newArray_Reader(Long_t size, void *p);
   static void delete_Reader(void *p);
   static void deleteArray_Reader(void *p);
   static void destruct_Reader(void *p);
   static void streamer_Reader(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Reader*)
   {
      ::Reader *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::Reader >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("Reader", ::Reader::Class_Version(), "src/Reader.h", 22,
                  typeid(::Reader), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::Reader::Dictionary, isa_proxy, 16,
                  sizeof(::Reader) );
      instance.SetNew(&new_Reader);
      instance.SetNewArray(&newArray_Reader);
      instance.SetDelete(&delete_Reader);
      instance.SetDeleteArray(&deleteArray_Reader);
      instance.SetDestructor(&destruct_Reader);
      instance.SetStreamerFunc(&streamer_Reader);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Reader*)
   {
      return GenerateInitInstanceLocal(static_cast<::Reader*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::Reader*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr Reader::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *Reader::Class_Name()
{
   return "Reader";
}

//______________________________________________________________________________
const char *Reader::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::Reader*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int Reader::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::Reader*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *Reader::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::Reader*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *Reader::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::Reader*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void Reader::Streamer(TBuffer &R__b)
{
   // Stream an object of class Reader.

   TSelector::Streamer(R__b);
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_Reader(void *p) {
      return  p ? new(p) ::Reader : new ::Reader;
   }
   static void *newArray_Reader(Long_t nElements, void *p) {
      return p ? new(p) ::Reader[nElements] : new ::Reader[nElements];
   }
   // Wrapper around operator delete
   static void delete_Reader(void *p) {
      delete (static_cast<::Reader*>(p));
   }
   static void deleteArray_Reader(void *p) {
      delete [] (static_cast<::Reader*>(p));
   }
   static void destruct_Reader(void *p) {
      typedef ::Reader current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_Reader(TBuffer &buf, void *obj) {
      ((::Reader*)obj)->::Reader::Streamer(buf);
   }
} // end of namespace ROOT for class ::Reader

namespace {
  void TriggerDictionaryInitialization_Reader_dict_Impl() {
    static const char* headers[] = {
"src/Reader.h",
nullptr
    };
    static const char* includePaths[] = {
"/cvmfs/cms.cern.ch/el9_amd64_gcc12/lcg/root/6.30.03-ca7ca986842b225f6fc22ae84d705ed8/include/",
"/uscms_data/d3/peteryou/boosted_new/CMSSW_14_0_6/src/VHccAnalysis/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "Reader_dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$src/Reader.h")))  Reader;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "Reader_dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "src/Reader.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"Reader", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("Reader_dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_Reader_dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_Reader_dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_Reader_dict() {
  TriggerDictionaryInitialization_Reader_dict_Impl();
}
