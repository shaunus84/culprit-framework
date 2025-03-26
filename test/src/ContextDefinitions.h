#pragma once
#include <culprit-framework/CulpritFramework.h>

#include "CommandDefinitions.h"
#include "SignalDefinitions.h"

using namespace culprit::framework;

class BasicContext : public ContextBase {
  void SetBindings() override {
    Bind<BaseTestModel>().To<BaseTestModel>();
    Bind<SingletonTestModel>().ToSingleton<SingletonTestModel>();
  }
};

class ComplexContext : public ContextBase {
  void SetBindings() override {
    Bind<BaseTestModel>().To<TestModelChild>();
    Bind<BoundDependency1>().To<BoundDependency1>();
    Bind<BoundDependency2>().To<BoundDependency2>();
    Bind<BoundDependency3>().To<BoundDependency3>();

    Bind<TestModelWith1Dependency>()
        .To<TestModelWith1Dependency, BoundDependency1>();
    Bind<TestModelWith2Dependency>()
        .To<TestModelWith2Dependency, BoundDependency1, BoundDependency2>();
    Bind<TestModelWith3Dependency>()
        .To<TestModelWith3Dependency, BoundDependency1, BoundDependency2,
            BoundDependency3>();

    Bind<TestModelWithArg>().To<TestModelWithArg>(42);
    Bind<TestModelWith1DependencyAndArg>()
        .To<TestModelWith1DependencyAndArg, BoundDependency1>(42);
    Bind<TestModelWith3DependencyAndArg>()
        .To<TestModelWith3DependencyAndArg, BoundDependency1, BoundDependency2,
            BoundDependency3>(42);
  }
};

class SignalsWithCommandsContext : public ContextBase {
  void SetBindings() override {
    Bind<SingletonTestModel>().ToSingleton<SingletonTestModel>();

    On<TestSignal1>().Do<TestCommand, SingletonTestModel>();

    On<TestSignal2>()
        .Do<TestCommand, SingletonTestModel>()
        .Do<AnotherTestCommand, SingletonTestModel>();

    On<TestSignal3>().Do<CommandWithArguments, SingletonTestModel>(
        "command with arguments");

    On<SignalWith1Arg>()
        .Do<CommandExpectingSignalArguments, SingletonTestModel>();

    On<SignalWith2Args>()
        .Do<CommandExpectingSignalArguments, SingletonTestModel>();
  }
};

class SignalForAttachDetachContext : public ContextBase {
  void SetBindings() override { BindSignal<SignalForAttach>(); }
};

class ChildContext : public ContextBase {
  void SetBindings() override {
    On<EnterContextSignal>()
        .Do<CommandOnChildContextEnter, SingletonTestModel>();

    On<ExitContextSignal>().Do<CommandOnChildContextExit, SingletonTestModel>();
  }
};

class ParentContext : public ContextBase {
  void SetBindings() override {
    Bind<SingletonTestModel>().ToSingleton<SingletonTestModel>();

    On<EnterContextSignal>()
        .Do<CommandOnParentContextEnter, SingletonTestModel>();

    On<ExitContextSignal>()
        .Do<CommandOnParentContextExit, SingletonTestModel>()
        .Do<RemoveContextCommand<ChildContext>, ContextBase>();

    On<TestAddChildContextSignal>()
        .Do<AddContextCommand<ChildContext>, ContextBase>();

    On<TestRemoveChildContextSignal>()
        .Do<RemoveContextCommand<ChildContext>, ContextBase>();
  }
};

class BasicUpdatableContext : public ContextBase {
  void SetBindings() override {
    Bind<TimeModel>().ToSingleton<TimeModel>();
    Bind<TestUpdatable>().To<TestUpdatable, TimeModel>();
    Bind<AnotherTestUpdatable>().To<AnotherTestUpdatable, TimeModel>();

    On<RemoveTestUpdatableSignal>()
        .Do<RemoveTestUpdatableCommand, ContextBase>();

    On<EnterContextSignal>()
        .Do<AddTestUpdatableCommand, ContextBase, TestUpdatable>();
  }
};

class ChildSharedSignalContext : public ContextBase {
  void SetBindings() override {
    On<SharedSignalTestSignal>()
        .Do<SharedSignalCommand, SharedSignalTestModel>();
  }
};

class ParentSharedSignalContext : public ContextBase {
  void SetBindings() override {
    BindSignal<SharedSignalTestSignal>();

    Bind<SharedSignalTestModel>().ToSingleton<SharedSignalTestModel>();
    On<EnterContextSignal>()
        .Do<AddContextCommand<ChildSharedSignalContext>, ContextBase>();

    On<RemoveSharedSignalTestChildSignal>()
        .Do<RemoveContextCommand<ChildSharedSignalContext>, ContextBase>();
  }
};
