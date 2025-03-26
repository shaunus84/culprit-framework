#pragma once

using namespace culprit::framework;

#include <culprit-framework/IUpdatable.h>

///
/// Resolve Dependencies
///
class BoundDependency1 {
 public:
  std::string phrase = "the answer to life, ";
};
class BoundDependency2 {
 public:
  std::string phrase = "the universe, ";
};
class BoundDependency3 {
 public:
  std::string phrase = "and everything is... ";
};

class UnboundDependency {};

///
/// Test Models
///
class BaseTestModel {
 public:
  BaseTestModel() : phrase("default"), theAnswer(0) {}

  std::string phrase;
  int theAnswer;
};

class SingletonTestModel : public BaseTestModel {};

class TestModelChild : public BaseTestModel {
 public:
  TestModelChild() { phrase = "child"; }
};

class TestModelWith1Dependency : public BaseTestModel {
 public:
  TestModelWith1Dependency(std::shared_ptr<BoundDependency1> a) {
    phrase = a->phrase;
  }
};

class TestModelWith2Dependency : public BaseTestModel {
 public:
  TestModelWith2Dependency(std::shared_ptr<BoundDependency1> a,
                           std::shared_ptr<BoundDependency2> b) {
    phrase = a->phrase + b->phrase;
  }
};

class TestModelWith3Dependency : public BaseTestModel {
 public:
  TestModelWith3Dependency(std::shared_ptr<BoundDependency1> a,
                           std::shared_ptr<BoundDependency2> b,
                           std::shared_ptr<BoundDependency3> c) {
    phrase = a->phrase + b->phrase + c->phrase;
  }
};

class TestModelWithArg : public BaseTestModel {
 public:
  TestModelWithArg(int answer) { theAnswer = answer; }
};

class TestModelWith1DependencyAndArg : public BaseTestModel {
 public:
  TestModelWith1DependencyAndArg(std::shared_ptr<BoundDependency1> a,
                                 int answer) {
    theAnswer = answer;
    phrase = a->phrase + std::to_string(theAnswer);
  }
};

class TestModelWith3DependencyAndArg : public BaseTestModel {
 public:
  TestModelWith3DependencyAndArg(std::shared_ptr<BoundDependency1> a,
                                 std::shared_ptr<BoundDependency2> b,
                                 std::shared_ptr<BoundDependency3> c,
                                 int answer) {
    theAnswer = answer;
    phrase = a->phrase + b->phrase + c->phrase + std::to_string(theAnswer);
  }
};

class UnboundModel1 : public BaseTestModel {
 public:
  using UnboundModel1_Creator = creator<UnboundModel1, BoundDependency1,
                                        BoundDependency2, BoundDependency3>;

  UnboundModel1(std::shared_ptr<BoundDependency1> a,
                std::shared_ptr<BoundDependency2> b,
                std::shared_ptr<BoundDependency3> c) {
    phrase = a->phrase + b->phrase + c->phrase;
  }
};

class UnboundModel2 : public BaseTestModel {
 public:
  using UnboundModel2_Creator = creator<UnboundModel2, UnboundDependency>;

  UnboundModel2(std::shared_ptr<UnboundDependency> a) {
    phrase = "should be impossible! As cannot resolve UnboundDependency";
  }
};

class TimeModel {
 public:
  double totalTime;

  TimeModel() { totalTime = 0.0f; }
};

class TestUpdatable : public IUpdatable<TestUpdatable> {
 public:
  TestUpdatable(std::shared_ptr<TimeModel> timeModel)
      : m_pTimeModel{timeModel} {}

  void HandleEvents(const void* pEvent) {}
  void PreUpdate() {}
  void Update(double deltaTime) { m_pTimeModel->totalTime += deltaTime; }
  void PostUpdate() {}

 private:
  std::shared_ptr<TimeModel> m_pTimeModel;
};

class AnotherTestUpdatable : public IUpdatable<AnotherTestUpdatable> {
 public:
  AnotherTestUpdatable(std::shared_ptr<TimeModel> timeModel)
      : m_pTimeModel{timeModel} {}

  void HandleEvents(const void* pEvent) {}
  void PreUpdate() {}
  void Update(double deltaTime) { m_pTimeModel->totalTime += deltaTime; }
  void PostUpdate() {}

 private:
  std::shared_ptr<TimeModel> m_pTimeModel;
};

class SharedSignalTestModel : public BaseTestModel {};
