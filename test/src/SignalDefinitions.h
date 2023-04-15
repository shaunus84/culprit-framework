#pragma once
#include <culprit-framework/CulpritFramework.h>

using namespace culprit::framework;

///
int can_attach_free_functions_func1_call_count = 0;
void CanAttachFreeFunctions_Func1() {
  ++can_attach_free_functions_func1_call_count;
}

int can_attach_free_functions_func2_call_count = 0;
void CanAttachFreeFunctions_Func2() {
  ++can_attach_free_functions_func2_call_count;
}

///
int can_detach_free_functions_func1_call_count = 0;
void CanDetachFreeFunctions_Func1() {
  ++can_detach_free_functions_func1_call_count;
}

int can_detach_free_functions_func2_call_count = 0;
void CanDetachFreeFunctions_Func2() {
  ++can_detach_free_functions_func2_call_count;
}

class AttachToSignalClass {
 public:
  void function1() { functionHistory.append("1"); }
  void function2() { functionHistory.append("2"); }
  void function3() { functionHistory.append("3"); }

  std::string functionHistory = "0";
};

class TestSignal1 : public Signal<> {};
class TestSignal2 : public Signal<> {};
class TestSignal3 : public Signal<> {};
class SignalWith1Arg : public Signal<std::string> {};
class SignalWith2Args : public Signal<std::string, std::string> {};
class SignalForAttach : public Signal<> {};
class TestAddChildContextSignal : public Signal<> {};
class TestRemoveChildContextSignal : public Signal<> {};
class RemoveTestUpdatableSignal : public Signal<> {};
class SharedSignalTestSignal : public Signal<> {};
class RemoveSharedSignalTestChildSignal : public Signal<> {};
