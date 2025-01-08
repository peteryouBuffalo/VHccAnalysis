// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME Processor_dict
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
#include "src/Processor.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_Processor(void *p = nullptr);
   static void *newArray_Processor(Long_t size, void *p);
   static void delete_Processor(void *p);
   static void deleteArray_Processor(void *p);
   static void destruct_Processor(void *p);
   static void streamer_Processor(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Processor*)
   {
      ::Processor *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::Processor >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("Processor", ::Processor::Class_Version(), "src/Processor.h", 22,
                  typeid(::Processor), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::Processor::Dictionary, isa_proxy, 16,
                  sizeof(::Processor) );
      instance.SetNew(&new_Processor);
      instance.SetNewArray(&newArray_Processor);
      instance.SetDelete(&delete_Processor);
      instance.SetDeleteArray(&deleteArray_Processor);
      instance.SetDestructor(&destruct_Processor);
      instance.SetStreamerFunc(&streamer_Processor);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Processor*)
   {
      return GenerateInitInstanceLocal(static_cast<::Processor*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::Processor*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr Processor::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *Processor::Class_Name()
{
   return "Processor";
}

//______________________________________________________________________________
const char *Processor::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::Processor*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int Processor::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::Processor*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *Processor::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::Processor*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *Processor::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::Processor*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void Processor::Streamer(TBuffer &R__b)
{
   // Stream an object of class Processor.

   Reader::Streamer(R__b);
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_Processor(void *p) {
      return  p ? new(p) ::Processor : new ::Processor;
   }
   static void *newArray_Processor(Long_t nElements, void *p) {
      return p ? new(p) ::Processor[nElements] : new ::Processor[nElements];
   }
   // Wrapper around operator delete
   static void delete_Processor(void *p) {
      delete (static_cast<::Processor*>(p));
   }
   static void deleteArray_Processor(void *p) {
      delete [] (static_cast<::Processor*>(p));
   }
   static void destruct_Processor(void *p) {
      typedef ::Processor current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_Processor(TBuffer &buf, void *obj) {
      ((::Processor*)obj)->::Processor::Streamer(buf);
   }
} // end of namespace ROOT for class ::Processor

namespace {
  void TriggerDictionaryInitialization_Processor_dict_Impl() {
    static const char* headers[] = {
"src/Processor.h",
nullptr
    };
    static const char* includePaths[] = {
"//cvmfs/cms.cern.ch/el9_amd64_gcc12/external/py3-correctionlib/2.2.2-120738cfaaf3f7c1056fe67d97e25dac/lib/python3.9/site-packages/correctionlib/include",
"/cvmfs/cms.cern.ch/el9_amd64_gcc12/lcg/root/6.30.03-ca7ca986842b225f6fc22ae84d705ed8/include/",
"/uscms_data/d3/peteryou/boosted_new/CMSSW_14_0_6/src/VHccAnalysis/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "Processor_dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$src/Processor.h")))  Processor;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "Processor_dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "src/Processor.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"Processor", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("Processor_dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_Processor_dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_Processor_dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_Processor_dict() {
  TriggerDictionaryInitialization_Processor_dict_Impl();
}
