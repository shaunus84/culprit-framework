#pragma once
#include <culprit-framework/CulpritFramework.h>

#include "SignalDefinitions.h"

using namespace culprit::framework;

class TestCommand : public CommandBase {
 public:
  TestCommand(std::shared_ptr<SingletonTestModel> model) : _model(model) {}

  void Execute(std::shared_ptr<SignalBase> signal) override {
    _model->phrase = "changed";
    Release();
  }

 private:
  std::shared_ptr<SingletonTestModel> _model;
};

class AnotherTestCommand : public CommandBase {
 public:
  AnotherTestCommand(std::shared_ptr<SingletonTestModel> model)
      : _model(model) {}

  void Execute(std::shared_ptr<SignalBase> signal) override {
    _model->phrase += " twice";
    Release();
  }

 private:
  std::shared_ptr<SingletonTestModel> _model;
};

class CommandOnParentContextEnter : public CommandBase {
 public:
  CommandOnParentContextEnter(std::shared_ptr<SingletonTestModel> model)
      : _model(model) {}

  void Execute(std::shared_ptr<SignalBase> signal) override {
    _model->phrase += " parent_context_enter";
    Release();
  }

 private:
  std::shared_ptr<SingletonTestModel> _model;
};

class CommandOnParentContextExit : public CommandBase {
 public:
  CommandOnParentContextExit(std::shared_ptr<SingletonTestModel> model)
      : _model(model) {}

  void Execute(std::shared_ptr<SignalBase> signal) override {
    _model->phrase += " parent_context_exit";
    Release();
  }

 private:
  std::shared_ptr<SingletonTestModel> _model;
};

class CommandOnChildContextEnter : public CommandBase {
 public:
  CommandOnChildContextEnter(std::shared_ptr<SingletonTestModel> model)
      : _model(model) {}

  void Execute(std::shared_ptr<SignalBase> signal) override {
    _model->phrase += " child_context_enter";
    Release();
  }

 private:
  std::shared_ptr<SingletonTestModel> _model;
};

class CommandOnChildContextExit : public CommandBase {
 public:
  CommandOnChildContextExit(std::shared_ptr<SingletonTestModel> model)
      : _model(model) {}

  void Execute(std::shared_ptr<SignalBase> signal) override {
    _model->phrase += " child_context_exit";
    Release();
  }

 private:
  std::shared_ptr<SingletonTestModel> _model;
};

class CommandWithArguments : public CommandBase {
 public:
  CommandWithArguments(std::shared_ptr<SingletonTestModel> model,
                       std::string newPhrase)
      : _model(model), _newPhrase(newPhrase) {}

  void Execute(std::shared_ptr<SignalBase> signal) override {
    _model->phrase = _newPhrase;
    Release();
  }

 private:
  std::shared_ptr<SingletonTestModel> _model;
  std::string _newPhrase;
};

class CommandExpectingSignalArguments : public CommandBase {
 public:
  CommandExpectingSignalArguments(std::shared_ptr<SingletonTestModel> model)
      : _model(model) {}

  void Execute(std::shared_ptr<SignalBase> signal) override {
    auto signal1Arg = std::dynamic_pointer_cast<SignalWith1Arg>(signal);
    if (signal1Arg != nullptr) {
      _model->phrase = signal1Arg->GetParam<0>();
      Release();
      return;
    }

    auto signal2Arg = std::dynamic_pointer_cast<SignalWith2Args>(signal);
    if (signal2Arg != nullptr) {
      _model->phrase = signal2Arg->GetParam<0>() + signal2Arg->GetParam<1>();
      Release();
      return;
    }
  }

 private:
  std::shared_ptr<SingletonTestModel> _model;
};

class AddTestUpdatableCommand : public CommandBase {
 public:
  AddTestUpdatableCommand(std::shared_ptr<ContextBase> context,
                          std::shared_ptr<TestUpdatable> updatable)
      : m_pContext{context}, m_pTestUpdatable{updatable} {}

  void Execute(std::shared_ptr<SignalBase> signal) override {
    m_pContext->AddUpdatable<TestUpdatable>(m_pTestUpdatable);
    Release();
  }

 private:
  std::shared_ptr<TestUpdatable> m_pTestUpdatable;
  std::shared_ptr<ContextBase> m_pContext;
};

class RemoveTestUpdatableCommand : public CommandBase {
 public:
  RemoveTestUpdatableCommand(std::shared_ptr<ContextBase> context)
      : m_pContext{context} {}

  void Execute(std::shared_ptr<SignalBase> signal) override {
    m_pContext->RemoveUpdatable<TestUpdatable>();
    Release();
  }

 private:
  std::shared_ptr<ContextBase> m_pContext;
};

class SharedSignalCommand : public CommandBase {
 public:
  SharedSignalCommand(
      std::shared_ptr<SharedSignalTestModel> sharedSignalTestModel)
      : m_pSharedSignalTestModel{sharedSignalTestModel} {}

  void Execute(std::shared_ptr<SignalBase> signal) override {
    m_pSharedSignalTestModel->phrase += " I got called in child context";
    Release();
  }

 private:
  std::shared_ptr<SharedSignalTestModel> m_pSharedSignalTestModel;
};
