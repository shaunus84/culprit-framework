#include <culprit-framework/CulpritFramework.h>

#include <memory>
#include <string>

#include "ClassDefinitions.h"
#include "ContextDefinitions.h"
#include "SignalDefinitions.h"
#include "gtest/gtest.h"

TEST(Resolving, CanResolveBoundObject) {
  auto context = std::make_shared<BasicContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Resolve<BaseTestModel>();
  ASSERT_EQ("default", testModel->phrase);
}

TEST(Resolving, CannotResolveUnboundObject) {
  auto context = std::make_shared<BasicContext>();
  context->Initialise();
  context->Enter();

  EXPECT_ANY_THROW(context->Resolve<UnboundDependency>());
}

TEST(Resolving, ResolveCreatesNewObjects) {
  auto context = std::make_shared<BasicContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Resolve<BaseTestModel>();
  testModel->phrase = "changed";
  ASSERT_EQ("changed", testModel->phrase);

  auto testModelAgain = context->Resolve<BaseTestModel>();
  ASSERT_EQ("default", testModelAgain->phrase);
  ASSERT_NE(testModel.get(), testModelAgain.get());
}

TEST(Resolving, ResolvedSingletonsAreTheSame) {
  auto context = std::make_shared<BasicContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Resolve<SingletonTestModel>();
  testModel->phrase = "changed";
  ASSERT_EQ("changed", testModel->phrase);

  auto testModelAgain = context->Resolve<SingletonTestModel>();
  ASSERT_EQ("changed", testModelAgain->phrase);
  ASSERT_EQ(testModel.get(), testModelAgain.get());
}

TEST(Resolving, SwappingContextClearsBindings) {
  auto context = std::make_shared<BasicContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Resolve<SingletonTestModel>();
  testModel->phrase = "changed";
  ASSERT_EQ("changed", testModel->phrase);

  context = std::make_shared<BasicContext>();
  context->Initialise();
  context->Enter();

  auto testModelAgain = context->Resolve<SingletonTestModel>();
  ASSERT_EQ("default", testModelAgain->phrase);
  ASSERT_NE(testModel, testModelAgain);
}

TEST(Resolving, CanBindOneTypeToAnother) {
  auto context = std::make_shared<ComplexContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Resolve<BaseTestModel>();
  ASSERT_EQ("child", testModel->phrase);
}

TEST(Resolving, CanBind1Dependency) {
  auto context = std::make_shared<ComplexContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Resolve<TestModelWith1Dependency>();
  ASSERT_EQ("the answer to life, ", testModel->phrase);
}

TEST(Resolving, CanBind2Dependency) {
  auto context = std::make_shared<ComplexContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Resolve<TestModelWith2Dependency>();
  ASSERT_EQ("the answer to life, the universe, ", testModel->phrase);
}

TEST(Resolving, CanBind3Dependency) {
  auto context = std::make_shared<ComplexContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Resolve<TestModelWith3Dependency>();
  ASSERT_EQ("the answer to life, the universe, and everything is... ",
            testModel->phrase);
}

TEST(Resolving, CanBindWithConstructorArguments) {
  auto context = std::make_shared<ComplexContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Resolve<TestModelWithArg>();
  ASSERT_EQ(42, testModel->theAnswer);
}

TEST(Resolving, CanBindSingleDependencyWithConstructorArguments) {
  auto context = std::make_shared<ComplexContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Resolve<TestModelWith1DependencyAndArg>();
  ASSERT_EQ("the answer to life, 42", testModel->phrase);
}

TEST(Resolving, CanBindMultipleDependenciesWithConstructorArguments) {
  auto context = std::make_shared<ComplexContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Resolve<TestModelWith3DependencyAndArg>();
  ASSERT_EQ("the answer to life, the universe, and everything is... 42",
            testModel->phrase);
}

TEST(Resolving, CanResolveIContext) {
  auto context = std::make_shared<BasicContext>();
  context->Initialise();
  context->Enter();

  auto resolvedContext = context->Resolve<ContextBase>();
  ASSERT_EQ(context.get(), resolvedContext.get());
}

TEST(ResolveInPlace, ResolveInPlace) {
  auto context = std::make_shared<ComplexContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Create<UnboundModel1::UnboundModel1_Creator>();
  ASSERT_EQ("the answer to life, the universe, and everything is... ",
            testModel->phrase);
}

TEST(ResolveInPlace, ResolveInPlaceWithUnboundDependenciesShouldFail) {
  auto context = std::make_shared<ComplexContext>();
  context->Initialise();
  context->Enter();

  EXPECT_ANY_THROW(context->Create<UnboundModel2::UnboundModel2_Creator>());
}

TEST(SignalsAndCommands, SignalExecutesCommand) {
  auto context = std::make_shared<SignalsWithCommandsContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Resolve<SingletonTestModel>();
  auto testSignal1 = context->Resolve<TestSignal1>();

  ASSERT_EQ("default", testModel->phrase);
  testSignal1->Dispatch();
  ASSERT_EQ("changed", testModel->phrase);
}

TEST(SignalsAndCommands, SignalExecutesMultipleCommands) {
  auto context = std::make_shared<SignalsWithCommandsContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Resolve<SingletonTestModel>();
  auto testSignal2 = context->Resolve<TestSignal2>();

  ASSERT_EQ("default", testModel->phrase);
  testSignal2->Dispatch();
  ASSERT_EQ("changed twice", testModel->phrase);
}

TEST(SignalsAndCommands, ACommandCanBeUsedByMultipleSignals) {
  auto context = std::make_shared<SignalsWithCommandsContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Resolve<SingletonTestModel>();
  auto signalWith1Arg = context->Resolve<SignalWith1Arg>();
  auto signalWith2Args = context->Resolve<SignalWith2Args>();

  ASSERT_EQ("default", testModel->phrase);
  signalWith1Arg->Dispatch(std::string("hello"));
  ASSERT_EQ("hello", testModel->phrase);

  signalWith2Args->Dispatch(std::string("hello "), std::string("world"));
  ASSERT_EQ("hello world", testModel->phrase);
}

TEST(SignalsAndCommands, CommandsWithArguments) {
  auto context = std::make_shared<SignalsWithCommandsContext>();
  context->Initialise();
  context->Enter();

  auto testModel = context->Resolve<SingletonTestModel>();
  auto testSignal3 = context->Resolve<TestSignal3>();

  ASSERT_EQ("default", testModel->phrase);
  testSignal3->Dispatch();
  ASSERT_EQ("command with arguments", testModel->phrase);
}

TEST(SignalAttachDetach, CanAttachFreeFunctions) {
  auto context = std::make_shared<SignalForAttachDetachContext>();
  context->Initialise();
  context->Enter();

  auto signal = context->Resolve<SignalForAttach>();
  signal->Attach(&CanAttachFreeFunctions_Func1);
  signal->Attach(&CanAttachFreeFunctions_Func2);

  ASSERT_EQ(0, can_attach_free_functions_func1_call_count);
  ASSERT_EQ(0, can_attach_free_functions_func2_call_count);
  signal->Dispatch();
  ASSERT_EQ(1, can_attach_free_functions_func1_call_count);
  ASSERT_EQ(1, can_attach_free_functions_func2_call_count);
}

TEST(SignalAttachDetach, CanDetachFreeFunctions) {
  auto context = std::make_shared<SignalForAttachDetachContext>();
  context->Initialise();
  context->Enter();

  auto signal = context->Resolve<SignalForAttach>();
  const int attachToken1 = signal->Attach(&CanDetachFreeFunctions_Func1);
  const int attachToken2 = signal->Attach(&CanDetachFreeFunctions_Func2);

  ASSERT_EQ(0, can_detach_free_functions_func1_call_count);
  ASSERT_EQ(0, can_detach_free_functions_func2_call_count);
  signal->Dispatch();
  ASSERT_EQ(1, can_detach_free_functions_func1_call_count);
  ASSERT_EQ(1, can_detach_free_functions_func2_call_count);
  signal->Detach(attachToken1);
  signal->Dispatch();
  ASSERT_EQ(1, can_detach_free_functions_func1_call_count);
  ASSERT_EQ(2, can_detach_free_functions_func2_call_count);
  signal->Detach(attachToken2);
  signal->Dispatch();
  ASSERT_EQ(1, can_detach_free_functions_func1_call_count);
  ASSERT_EQ(2, can_detach_free_functions_func2_call_count);
}

TEST(SignalAttachDetach, CanAttachMemberFunction) {
  auto context = std::make_shared<SignalForAttachDetachContext>();
  context->Initialise();
  context->Enter();

  AttachToSignalClass classToNotify;
  auto signal = context->Resolve<SignalForAttach>();
  signal->Attach<AttachToSignalClass>(&classToNotify,
                                      &AttachToSignalClass::function1);

  ASSERT_EQ("0", classToNotify.functionHistory);
  signal->Dispatch();
  ASSERT_EQ("01", classToNotify.functionHistory);
}

TEST(SignalAttachDetach, CanAttachMultipleMemberFunctions) {
  auto context = std::make_shared<SignalForAttachDetachContext>();
  context->Initialise();
  context->Enter();

  AttachToSignalClass classToNotify;
  auto signal = context->Resolve<SignalForAttach>();
  signal->Attach<AttachToSignalClass>(&classToNotify,
                                      &AttachToSignalClass::function1);
  signal->Attach<AttachToSignalClass>(&classToNotify,
                                      &AttachToSignalClass::function2);
  signal->Attach<AttachToSignalClass>(&classToNotify,
                                      &AttachToSignalClass::function3);

  ASSERT_EQ("0", classToNotify.functionHistory);
  signal->Dispatch();
  ASSERT_EQ("0123", classToNotify.functionHistory);
}

TEST(SignalAttachDetach, CanDetachMemberFunction) {
  auto context = std::make_shared<SignalForAttachDetachContext>();
  context->Initialise();
  context->Enter();

  AttachToSignalClass classToNotify;
  auto signal = context->Resolve<SignalForAttach>();
  signal->Attach<AttachToSignalClass>(&classToNotify,
                                      &AttachToSignalClass::function1);
  signal->Attach<AttachToSignalClass>(&classToNotify,
                                      &AttachToSignalClass::function2);
  const int attachToken = signal->Attach<AttachToSignalClass>(
      &classToNotify, &AttachToSignalClass::function3);

  ASSERT_EQ("0", classToNotify.functionHistory);
  signal->Dispatch();
  ASSERT_EQ("0123", classToNotify.functionHistory);
  signal->Detach(attachToken);
  signal->Dispatch();
  ASSERT_EQ("012312", classToNotify.functionHistory);
}

TEST(SignalAttachDetach, CanDetachAllMemberFunctionsOfClass) {
  auto context = std::make_shared<SignalForAttachDetachContext>();
  context->Initialise();
  context->Enter();

  AttachToSignalClass classToNotify;
  auto signal = context->Resolve<SignalForAttach>();
  signal->Attach<AttachToSignalClass>(&classToNotify,
                                      &AttachToSignalClass::function1);
  signal->Attach<AttachToSignalClass>(&classToNotify,
                                      &AttachToSignalClass::function2);
  signal->Attach<AttachToSignalClass>(&classToNotify,
                                      &AttachToSignalClass::function3);

  ASSERT_EQ("0", classToNotify.functionHistory);
  signal->Dispatch();
  ASSERT_EQ("0123", classToNotify.functionHistory);
  signal->DetachAll(&classToNotify);
  signal->Dispatch();
  ASSERT_EQ("0123", classToNotify.functionHistory);
}

TEST(SignalAttachDetach, DifferentInstancesOfSameClass) {
  auto context = std::make_shared<SignalForAttachDetachContext>();
  context->Initialise();
  context->Enter();

  AttachToSignalClass classToNotify;
  AttachToSignalClass anotherClassToNotify;
  auto signal = context->Resolve<SignalForAttach>();
  signal->Attach<AttachToSignalClass>(&classToNotify,
                                      &AttachToSignalClass::function1);
  signal->Attach<AttachToSignalClass>(&classToNotify,
                                      &AttachToSignalClass::function2);
  const int attachToken = signal->Attach<AttachToSignalClass>(
      &classToNotify, &AttachToSignalClass::function3);
  signal->Attach<AttachToSignalClass>(&anotherClassToNotify,
                                      &AttachToSignalClass::function1);
  signal->Attach<AttachToSignalClass>(&anotherClassToNotify,
                                      &AttachToSignalClass::function2);
  signal->Attach<AttachToSignalClass>(&anotherClassToNotify,
                                      &AttachToSignalClass::function3);

  ASSERT_EQ("0", classToNotify.functionHistory);
  ASSERT_EQ("0", anotherClassToNotify.functionHistory);
  signal->Dispatch();
  ASSERT_EQ("0123", classToNotify.functionHistory);
  ASSERT_EQ("0123", anotherClassToNotify.functionHistory);
  signal->Detach(attachToken);
  signal->Dispatch();
  ASSERT_EQ("012312", classToNotify.functionHistory);
  ASSERT_EQ("0123123", anotherClassToNotify.functionHistory);
  signal->DetachAll(&classToNotify);
  signal->Dispatch();
  ASSERT_EQ("012312", classToNotify.functionHistory);
  ASSERT_EQ("0123123123", anotherClassToNotify.functionHistory);
}

TEST(ChildContexts, EnterAndExitContextSignals) {
  std::shared_ptr<SingletonTestModel> singletonTestModel;

  auto context = std::make_shared<ParentContext>();
  context->Initialise();

  singletonTestModel = context->Resolve<SingletonTestModel>();

  ASSERT_EQ("default", singletonTestModel->phrase);
  context->Enter();
  ASSERT_EQ("default parent_context_enter", singletonTestModel->phrase);

  context->Exit();

  ASSERT_EQ("default parent_context_enter parent_context_exit",
            singletonTestModel->phrase);
}

TEST(ChildContexts, ChildContextsExitWithParent) {
  std::shared_ptr<SingletonTestModel> singletonTestModel;

  auto context = std::make_shared<ParentContext>();
  context->Initialise();
  context->Enter();

  singletonTestModel = context->Resolve<SingletonTestModel>();

  ASSERT_EQ("default parent_context_enter", singletonTestModel->phrase);
  auto childContext = context->AddChildContext<ChildContext>();
  childContext->Enter();
  ASSERT_EQ("default parent_context_enter child_context_enter",
            singletonTestModel->phrase);

  context->Exit();
  ASSERT_TRUE(singletonTestModel->phrase.find("child_context_exit") !=
              std::string::npos);
}

TEST(ChildContexts, ParentAndChildSingletonsAreTheSame) {
  auto context = std::make_shared<ParentContext>();
  context->Initialise();
  context->Enter();

  auto childContext = context->AddChildContext<ChildContext>();
  childContext->Enter();

  auto parentSingleton = context->Resolve<SingletonTestModel>();
  auto childSingleton = childContext->Resolve<SingletonTestModel>();

  ASSERT_EQ(parentSingleton.get(), childSingleton.get());
}

TEST(ChildContexts, ChildContextFunctions) {
  auto context = std::make_shared<ParentContext>();
  context->Initialise();
  context->Enter();

  auto singletonTestModel = context->Resolve<SingletonTestModel>();

  ASSERT_EQ("default parent_context_enter", singletonTestModel->phrase);

  auto childContext = context->AddChildContext<ChildContext>();
  childContext->Enter();

  ASSERT_EQ("default parent_context_enter child_context_enter",
            singletonTestModel->phrase);

  context->RemoveChildContext<ChildContext>();

  ASSERT_EQ(
      "default parent_context_enter child_context_enter child_context_exit",
      singletonTestModel->phrase);
}

TEST(ChildContexts, ChildContextCommands) {
  auto context = std::make_shared<ParentContext>();
  context->Initialise();
  context->Enter();

  auto singletonTestModel = context->Resolve<SingletonTestModel>();
  auto addChildSignal = context->Resolve<TestAddChildContextSignal>();
  auto removeChildSignal = context->Resolve<TestRemoveChildContextSignal>();

  addChildSignal->Dispatch();
  ASSERT_EQ("default parent_context_enter child_context_enter",
            singletonTestModel->phrase);
  removeChildSignal->Dispatch();
  ASSERT_EQ(
      "default parent_context_enter child_context_enter child_context_exit",
      singletonTestModel->phrase);
}

TEST(ContextStorage, CanStoreObject) {
  auto context = std::make_shared<BasicContext>();
  context->Initialise();
  context->Enter();

  EXPECT_ANY_THROW(context->GetFromStore<BaseTestModel>());

  auto model = std::make_shared<BaseTestModel>();
  context->Store<BaseTestModel>(model);

  EXPECT_NO_THROW(context->GetFromStore<BaseTestModel>());
}

TEST(ContextStorage, StoreAndRetrieveSameObject) {
  auto context = std::make_shared<BasicContext>();
  context->Initialise();
  context->Enter();

  auto model = std::make_shared<BaseTestModel>();
  context->Store<BaseTestModel>(model);
  auto storedModel = context->GetFromStore<BaseTestModel>();

  ASSERT_EQ(model.get(), storedModel.get());
}

TEST(ContextStorage, CanStoreResolvedSingleton) {
  auto context = std::make_shared<BasicContext>();
  context->Initialise();
  context->Enter();

  auto model = context->Resolve<SingletonTestModel>();
  EXPECT_NO_THROW(context->Store<SingletonTestModel>(model));
}

TEST(ContextStorage, CannotStoreUnresolvedSingleton) {
  auto context = std::make_shared<BasicContext>();
  context->Initialise();
  context->Enter();

  auto model = std::make_shared<SingletonTestModel>();
  ASSERT_DEATH(context->Store<SingletonTestModel>(model),
               "Key Type is singleton, but does not match bound singleton.");
}

TEST(ContextStorage, DeleteFromStore) {
  auto context = std::make_shared<BasicContext>();
  context->Initialise();
  context->Enter();

  EXPECT_ANY_THROW(context->GetFromStore<BaseTestModel>());
  EXPECT_ANY_THROW(context->DeleteFromStore<BaseTestModel>());

  auto model = std::make_shared<BaseTestModel>();
  context->Store<BaseTestModel>(model);
  auto storedModel = context->GetFromStore<BaseTestModel>();

  ASSERT_EQ(model.get(), storedModel.get());

  context->DeleteFromStore<BaseTestModel>();

  context->Update(0);

  EXPECT_ANY_THROW(context->GetFromStore<BaseTestModel>());
  EXPECT_ANY_THROW(context->DeleteFromStore<BaseTestModel>());
}

TEST(ConstCorrectness, CanResolveToConst) {
  auto context = std::make_shared<BasicContext>();
  context->Initialise();
  context->Enter();

  auto constTestModel = context->Resolve<const SingletonTestModel>();
  auto testModel = context->Resolve<SingletonTestModel>();

  ASSERT_EQ(constTestModel.get(), testModel.get());
}

TEST(Updating, AddRemoveExecuteUpdatableObject) {
  auto context = std::make_shared<BasicUpdatableContext>();
  context->Initialise();
  context->Enter();

  // time model is a singleton updated by testupdatable
  auto timeModel = context->Resolve<TimeModel>();

  // grab the updatable that was added in the command
  auto testUpdatable = context->GetUpdatable<TestUpdatable>();

  // grab a signal to launch another command to remove updatable
  auto removeUpdatableSignal = context->Resolve<RemoveTestUpdatableSignal>();

  // track time model
  while (timeModel->totalTime < 10.0) {
    context->Update(0.016f);
  }

  // remove updatable
  removeUpdatableSignal->Dispatch();

  // updatables get stored and removed after update
  context->Update(0);

  // check if actually removed
  ASSERT_ANY_THROW(context->GetUpdatable<TestUpdatable>());
}

TEST(SharedSignals, BoundSignalExecuteInParentAndChildCommandChain) {
  auto context = std::make_shared<ParentSharedSignalContext>();
  context->Initialise();
  context->Enter();

  auto testSignal = context->Resolve<SharedSignalTestSignal>();
  auto removeSignal = context->Resolve<RemoveSharedSignalTestChildSignal>();

  testSignal->Dispatch();

  removeSignal->Dispatch();

  testSignal->Dispatch();

  auto testModel = context->Resolve<SharedSignalTestModel>();

  ASSERT_TRUE(testModel->phrase.find("I got called in child context") !=
              std::string::npos);
}