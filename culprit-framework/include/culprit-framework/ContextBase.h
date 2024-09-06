#pragma once

#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "CommandBase.h"
#include "Creator.hpp"
#include "IUpdatable.h"
#include "Signal.hpp"
#include "SignalResponder.h"
#include "UniqueKeyGenerator.h"

namespace culprit {
namespace framework {

template <typename T>
using remove_const_t = typename std::remove_const<T>::type;

template <typename T>
inline void ignore_result(const T& /* unused result */) {}

template <typename T, const size_t N = 0>
struct store_key {};

template <class Key>
class BindFacade;

template <class Key>
class OnSignalFacade;

using type_identifier = std::size_t;

class ContextBase : public std::enable_shared_from_this<ContextBase> {
  using CreatorFunction = std::function<std::shared_ptr<void>()>;
  using InstanceMap = std::unordered_map<type_identifier, std::shared_ptr<void>>;
  using ResolverMap = std::unordered_map<type_identifier, std::shared_ptr<CreatorFunction>>;
  using CommandResolverMap =
      std::unordered_map<type_identifier, std::shared_ptr<CreatorFunction>>;
  using CommandMap = std::unordered_map<type_identifier, std::shared_ptr<SignalResponder>>;
  using StoredObjects = std::unordered_map<type_identifier, std::shared_ptr<void>>;
  using UpdatableObjects = std::unordered_map<type_identifier, std::shared_ptr<IUpdatable>>;

  template <typename>
  friend class BindFacade;
  template <typename>
  friend class OnSignalFacade;

 public:
  virtual ~ContextBase() = default;

  void Initialise();
  void Enter();
  void HandleEvents(const void* pEvent);
  void PreUpdate();
  void Update(double deltaTime);
  void PostUpdate();
  void Exit();

  template <class T>
  std::shared_ptr<T> Resolve();

  template <>
  inline std::shared_ptr<ContextBase> Resolve<ContextBase>();

  template <class Dependencies, class... Args>
  decltype(auto) Create(Args&&... args);

  template <class Context>
  std::shared_ptr<Context> AddChildContext();

  template <class Context>
  std::shared_ptr<Context> GetChildContext() const;

  template <class Context>
  bool RemoveChildContext();

  template <class Key, const type_identifier N = 0>
  void Store(std::shared_ptr<Key> value);

  template <class Key, const type_identifier N = 0>
  void StoreShared(std::shared_ptr<Key> value);

  template <class Key, const type_identifier N = 0>
  decltype(auto) GetFromStore();

  template <class Key, const type_identifier N = 0>
  decltype(auto) GetFromSharedStore();

  template <class Key, const type_identifier N = 0>
  bool HasStored();

  template <class Key, const type_identifier N = 0>
  bool HasSharedStored();

  template <class Key, const type_identifier N = 0>
  void DeleteFromStore();

  template <class Key, const type_identifier N = 0>
  void DeleteFromSharedStore();

  template <class Key>
  void AddUpdatable(std::shared_ptr<Key> value);

  template <class Key>
  decltype(auto) GetUpdatable();

  template <class Key>
  bool HasUpdatable();

  template <class Key>
  void RemoveUpdatable();

 protected:
  virtual void SetBindings() = 0;

  template <class Key>
  BindFacade<Key> Bind();

  void PopulateFromParent(const ContextBase& other) {
    m_instanceMap = other.m_instanceMap;
    m_resolverMap = other.m_resolverMap;

    // share stored objects
    m_sharedStoredObjects = other.m_sharedStoredObjects;

    // We don't want to share any other maps, especially commands
  }

  template <class Key>
  void BindSignal();

  template <class Key>
  void RemoveBind();

  template <class T>
  bool HasBinding() const;

  template <class Signal>
  OnSignalFacade<Signal> On();

 private:
  template <class Key>
  std::shared_ptr<Key> Resolve(type_identifier keyID);

  template <class Dependencies, class... Args>
  decltype(auto) ResolveInPlace(Args&&... args);

  template <class Value, class... Dependencies, class... Args>
  decltype(auto) Expand(creator<Dependencies...>, Args&&... args);

  template <class Value, class... Dependencies, class... Args>
  decltype(auto) DoResolveInPlace(Args&&... args);

  template <class Key, class... Dependencies, class... Args>
  void BindCommand(Args&&... args);

  template <class Key, class Value, class... Dependencies, class... Args>
  void To(Args&&... args);

  template <class Key, class Value, class... Dependencies, class... Args>
  void ToSingleton(Args&&... args);

  template <class Key, class Value, class... Dependencies, class... Args>
  void Do(Args&&... args);

  void Build();

 private:
  std::unordered_map<type_identifier, std::shared_ptr<ContextBase>>
      m_childContexts;

  ResolverMap m_resolverMap;
  InstanceMap m_instanceMap;
  UpdatableObjects m_updatableObjects;
  std::unordered_set<type_identifier> m_toRemoveUpdatableObjects;
  CommandMap m_commandMap;
  StoredObjects m_storedObjects;
  StoredObjects m_sharedStoredObjects;
  std::unordered_set<type_identifier> m_toRemoveStoredObjects;
  std::unordered_set<type_identifier> m_toRemoveSharedStoredObjects;
  CommandResolverMap m_commandResolverMap;

  std::unordered_map<type_identifier, std::vector<type_identifier>>
      m_attachedCommands;

  std::vector<type_identifier> asSingletonKeys;
};

// ----- Facades ----- //
template <class Key>
class BindFacade {
 public:
  explicit BindFacade(ContextBase& context) : _context(context) {}

  template <class Value, class... Dependencies, class... Args>
  void To(Args&&... args) {
    constexpr bool validKeyValue = std::is_base_of<Key, Value>();
    constexpr bool validValueKey = std::is_base_of<Value, Key>();

    static_assert(validKeyValue || validValueKey,
                  "Value is not derived type of Key");

    _context.To<Key, Value, Dependencies...>(std::forward<Args>(args)...);
  }

  template <class Value, class... Dependencies, class... Args>
  void ToSingleton(Args&&... args) {
    constexpr bool validKeyValue = std::is_base_of<Key, Value>();
    constexpr bool validValueKey = std::is_base_of<Value, Key>();

    static_assert(validKeyValue || validValueKey,
                  "Value is not derived type of Key");

    _context.ToSingleton<Key, Value, Dependencies...>(
        std::forward<Args>(args)...);
  }

 private:
  ContextBase& _context;
};

template <class Key>
class OnSignalFacade {
 public:
  explicit OnSignalFacade(ContextBase& context) : _context(context) {}

  template <class Value, class... Dependencies, class... Args>
  OnSignalFacade<Key>& Do(Args&&... args) {
    _context.Do<Key, Value, Dependencies...>(std::forward<Args>(args)...);
    return *this;
  }

 private:
  ContextBase& _context;
};
// ----- End Facades ----- //

// ----- Add/Remove Context Commands ----- //
template <class Context>
class RemoveContextCommand : public CommandBase {
 public:
  RemoveContextCommand(std::shared_ptr<ContextBase> context)
      : m_context(context) {}

  void Execute(std::shared_ptr<SignalBase> signal) override {
    m_context->RemoveChildContext<Context>();
    Release();
  }

 private:
  std::shared_ptr<ContextBase> m_context;
};

template <class Context>
class AddContextCommand : public CommandBase {
 public:
  AddContextCommand(std::shared_ptr<ContextBase> context)
      : m_context(context) {}

  void Execute(std::shared_ptr<SignalBase> signal) override {
    auto childContext = m_context->AddChildContext<Context>();
    childContext->Enter();
    Release();
  }

 private:
  std::shared_ptr<ContextBase> m_context;
};
// ----- End Add/Remove Context Commands ----- //

// ----- ContextBase ----- //
template <class Key>
BindFacade<Key> ContextBase::Bind() {
  const auto keyID = UniqueKeyGenerator::Get<Key>();

  static_assert(!std::is_base_of<CommandBase, Key>(),
                "Attempting to Bind 'Command' type: Commands are bound to "
                "Signals with 'On<Signal>().Do<Command>()'");
  static_assert(!std::is_base_of<SignalBase, Key>(),
                "Attempting to Bind 'Signal' type: Signals are bound with "
                "'BindSignal()' or "
                "'On<Signal>().Do<Command>()'");
  static_assert(!std::is_base_of<ContextBase, Key>(),
                "Attempting to Bind 'ContextBase' type: ContextBase can be "
                "resolved, but it cannot be "
                "manually bound.");
  assert((ignore_result("Attempting to bind already bound key."),
          m_resolverMap.count(keyID) == 0));

  m_resolverMap.insert(std::make_pair(keyID, nullptr));

  return BindFacade<Key>(*this);
}

template <class Key>
void ContextBase::BindSignal() {
  const auto signalID = UniqueKeyGenerator::Get<Key>();
  static_assert(std::is_base_of<SignalBase, Key>(),
                "Attempting to use 'BindSignal' with non-Signal type.");
  assert((ignore_result("Attempting to bind already bound key."),
          m_resolverMap.count(signalID) == 0));

  asSingletonKeys.push_back(signalID);

  CreatorFunction del = [this, signalID]() -> std::shared_ptr<Key> {
    const auto instanceFind = m_instanceMap.find(signalID);
    if (instanceFind != m_instanceMap.end()) {
      return std::static_pointer_cast<Key>(instanceFind->second);
    }
    std::shared_ptr<Key> signal = std::make_shared<Key>();
    m_instanceMap.insert(std::make_pair(signalID, signal));
    return signal;
  };

  m_resolverMap.insert(std::make_pair(
      signalID, std::make_shared<CreatorFunction>(std::move(del))));
}

template <class Key, class Value, class... Dependencies, class... Args>
void ContextBase::To(Args&&... args) {
  const auto keyID = UniqueKeyGenerator::Get<Key>();

  static_assert(!std::is_base_of<CommandBase, Value>(),
                "Attempting to Bind 'Command' type: Commands are bound to "
                "Signals with 'On<Signal>().Do<Command>()'");
  static_assert(!std::is_base_of<SignalBase, Value>(),
                "Attempting to Bind 'Signal' type: Signals are bound with "
                "'BindSignal()' or "
                "'On<Signal>().Do<Command>()'");
  static_assert(!std::is_base_of<ContextBase, Value>(),
                "Attempting to Bind 'ContextBase' type: ContextBase can be "
                "resolved, but it cannot be "
                "manually bound.");
  assert((ignore_result("Key not found."), m_resolverMap.count(keyID) > 0));

  auto del = [this, args...]() -> std::shared_ptr<Value> {
    return std::make_shared<Value>(this->Resolve<Dependencies>()..., args...);
  };

  m_resolverMap[keyID] = std::make_shared<CreatorFunction>(std::move(del));
}

template <class Key, class Value, class... Dependencies, class... Args>
void ContextBase::ToSingleton(Args&&... args) {
  const auto keyID = UniqueKeyGenerator::Get<Key>();

  static_assert(!std::is_base_of<CommandBase, Value>(),
                "Attempting to Bind 'Command' type: Commands are bound to "
                "Signals with 'On<Signal>().Do<Command>()'");
  static_assert(!std::is_base_of<SignalBase, Value>(),
                "Attempting to Bind 'Signal' type: Signals are bound with "
                "'BindSignal()' or "
                "'On<Signal>().Do<Command>()'");
  static_assert(!std::is_base_of<ContextBase, Value>(),
                "Attempting to Bind 'ContextBase' type: ContextBase can be "
                "resolved, but it cannot be "
                "manually bound.");
  assert((ignore_result("Key not found."), m_resolverMap.count(keyID) > 0));

  asSingletonKeys.push_back(keyID);

  auto del = [this, keyID, args...]() -> std::shared_ptr<Value> {
    m_instanceMap.insert(std::make_pair(
        keyID,
        std::make_shared<Value>(this->Resolve<Dependencies>()..., args...)));
    return nullptr;
  };

  m_resolverMap[keyID] = std::make_shared<CreatorFunction>(std::move(del));
}

template <class Key>
void ContextBase::RemoveBind() {
  const auto bindID = UniqueKeyGenerator::Get<Key>();

  m_instanceMap.erase(bindID);
  m_resolverMap.erase(bindID);
  m_commandMap.erase(bindID);
}

template <class Signal>
OnSignalFacade<Signal> ContextBase::On() {
  const auto signalID = UniqueKeyGenerator::Get<Signal>();

  static_assert(std::is_base_of<SignalBase, Signal>(),
                "Attempting to use 'On' with non-Signal type.");

  // no signals of this type in resolver map
  if (m_resolverMap.count(signalID) == 0) {
    asSingletonKeys.push_back(signalID);
    // add a signal responder to the command map, this is storage for attached
    // method to signal
    m_commandMap.insert(std::make_pair(
        signalID, std::make_shared<SignalResponder>(
                      m_resolverMap, m_commandResolverMap, signalID)));

    // create the resolver for this signal type
    auto del = [this, signalID]() -> std::shared_ptr<Signal> {
      const auto instanceFind = m_instanceMap.find(signalID);
      if (instanceFind != m_instanceMap.end()) {
        return std::static_pointer_cast<Signal>(instanceFind->second);
      }

      std::shared_ptr<Signal> signal = std::make_shared<Signal>();
      // attach a signal responder
      const auto findResult = m_commandMap.find(signalID);
      if (findResult->second) {
        signal->Attach(static_cast<SignalResponder*>(findResult->second.get()),
                       &SignalResponder::Respond);
      }
      m_instanceMap.insert(std::make_pair(signalID, signal));
      return signal;
    };

    m_resolverMap[signalID] = std::make_shared<CreatorFunction>(std::move(del));
  } else  // this signal exists already and has a resolver
  {
    // Make and insert a new signal responder to the command map
    m_commandMap.insert(std::make_pair(
        signalID, std::make_shared<SignalResponder>(
                      m_resolverMap, m_commandResolverMap, signalID)));
    const auto findResult = m_commandMap.find(signalID);
    if (findResult->second) {
      // resolve the signal
      const auto& signalCreatorFunction = *m_resolverMap.at(signalID);
      const auto triggeringSignal =
          std::static_pointer_cast<SignalBase>(signalCreatorFunction());

      // attach the new signal responder to it
      const auto attachID = triggeringSignal->Attach(
          static_cast<SignalResponder*>(findResult->second.get()),
          &SignalResponder::Respond);

      // track that we have attached a command chain to this signal in case this
      // context can be dynamically removed
      m_attachedCommands[signalID].push_back(attachID);
    }
  }

  return OnSignalFacade<Signal>(*this);
}

template <class Key, class Value, class... Dependencies, class... Args>
void ContextBase::Do(Args&&... args) {
  const auto signalID = UniqueKeyGenerator::Get<Key>();
  const auto commandID = UniqueKeyGenerator::Get<Value>();

  static_assert(std::is_base_of<CommandBase, Value>(),
                "Attempting to use 'Do' with non-Command type.");

  auto it = this->m_commandMap.find(signalID);
  if (it != this->m_commandMap.end()) {
    BindCommand<Value, Dependencies...>(std::forward<Args>(args)...);
    // add name for look up, always mapped to same type name
    it->second->AddCommand(commandID);
  }
}

template <class Key>
bool ContextBase::HasBinding() const {
  return m_resolverMap.find(UniqueKeyGenerator::Get<Key>()) !=
         m_resolverMap.end();
}

template <class Key>
std::shared_ptr<Key> ContextBase::Resolve() {
  return Resolve<Key>(UniqueKeyGenerator::Get<remove_const_t<Key>>());
}

template <>
std::shared_ptr<ContextBase> ContextBase::Resolve<ContextBase>() {
  return shared_from_this();
}

template <class Key>
std::shared_ptr<Key> ContextBase::Resolve(type_identifier keyID) {
  // <Key> is a singleton and there is an instance in the map. Return it
  const auto instance_iterator = m_instanceMap.find(keyID);
  if (instance_iterator != m_instanceMap.end()) {
    return std::static_pointer_cast<Key>(instance_iterator->second);
  }

  const auto resolver_iterator = m_resolverMap.find(keyID);
  if (resolver_iterator != m_resolverMap.end()) {
    const auto& instance_factory_function = *resolver_iterator->second;
    if (std::find(asSingletonKeys.begin(), asSingletonKeys.end(), keyID) !=
        asSingletonKeys.end()) {
      // <Key> is a singleton, but there was no instance in the map. Create one,
      // store it in the map, and return it.
      instance_factory_function();
      return std::static_pointer_cast<Key>(m_instanceMap.at(keyID));
    }

    // <Key> is not a singleton. Create one and return it
    return std::static_pointer_cast<Key>(instance_factory_function());
  }

  std::string name = typeid(Key).name();
  throw std::runtime_error("No resolve available for : " +
                           std::string(typeid(Key).name()));
}

template <class Dependencies, class... Args>
decltype(auto) ContextBase::ResolveInPlace(Args&&... args) {
  return Expand<typename Dependencies::createdType>(
      typename Dependencies::deps{}, std::forward<Args>(args)...);
}

template <class Value, class... Dependencies, class... Args>
decltype(auto) ContextBase::Expand(creator<Dependencies...>, Args&&... args) {
  return DoResolveInPlace<Value, Dependencies...>(std::forward<Args>(args)...);
}

template <class Value, class... Dependencies, class... Args>
decltype(auto) ContextBase::DoResolveInPlace(Args&&... args) {
  return std::make_shared<Value>((Resolve<Dependencies>())...,
                                 std::forward<Args>(args)...);
}

template <class Key, class... Dependencies, class... Args>
void ContextBase::BindCommand(Args&&... args) {
  const auto commandID = UniqueKeyGenerator::Get<Key>();

  // if we don't know how create this command then make a new resolver
  if (m_commandResolverMap.find(commandID) == m_commandResolverMap.end()) {
    m_commandResolverMap.insert(std::make_pair(commandID, nullptr));

    auto del = [this, args...]() -> std::shared_ptr<Key> {
      return std::make_shared<Key>(Resolve<Dependencies>()..., args...);
    };

    m_commandResolverMap[commandID] =
        std::make_shared<CreatorFunction>(std::move(del));
  }
}

template <class Dependencies, class... Args>
decltype(auto) ContextBase::Create(Args&&... args) {
  return this->Expand<typename Dependencies::createdType>(
      typename Dependencies::deps{}, std::forward<Args>(args)...);
}

template <class Context>
std::shared_ptr<Context> ContextBase::AddChildContext() {
  const auto contextKey = UniqueKeyGenerator::Get<Context>();

  static_assert(std::is_base_of<ContextBase, Context>(),
                "Child context must be ContextBase type");
  assert((ignore_result("Cannot add the same child context twice."),
          m_childContexts.count(contextKey) == 0));

  auto child = std::make_shared<Context>();
  child->PopulateFromParent(*this);
  child->Initialise();
  m_childContexts.insert(std::make_pair(contextKey, child));
  return child;
}

template <class Context>
std::shared_ptr<Context> ContextBase::GetChildContext() const {
  auto storedResult = m_childContexts.find(UniqueKeyGenerator::Get<Context>());
  if (storedResult == m_childContexts.end()) {
    throw std::runtime_error("No child context of type " +
                             std::string(typeid(Context).name()));
  }

  return std::static_pointer_cast<Context>(storedResult->second);
}

template <class Context>
bool ContextBase::RemoveChildContext() {
  const auto contextKey = UniqueKeyGenerator::Get<Context>();

  static_assert(std::is_base_of<ContextBase, Context>(),
                "Child context must be ContextBase type");

  auto contextResult = m_childContexts.find(contextKey);
  if (contextResult == m_childContexts.end()) {
    return false;
  }

  contextResult->second->Exit();

  for (auto& child : contextResult->second->m_childContexts) {
    std::cout << "Context of type " << std::string(typeid(child).name())
              << " was not removed" << std::endl;
    child.second->Exit();
  }

  m_childContexts.erase(contextKey);
  return true;
}

template <class Key, const type_identifier N>
void ContextBase::Store(std::shared_ptr<Key> value) {
  static_assert(!std::is_base_of<CommandBase, Key>(),
                "Cannot store Command type");
  static_assert(!std::is_base_of<SignalBase, Key>(),
                "Cannot store Signal type");
  static_assert(!std::is_base_of<ContextBase, Key>(),
                "Cannot store Context type");
  const bool value_is_singleton_type =
      m_instanceMap.count(UniqueKeyGenerator::Get<Key>()) > 0;
  const bool value_matches_bound_singleton =
      value_is_singleton_type && value.get() == Resolve<Key>().get();
  const bool store_is_valid =
      !value_is_singleton_type || value_matches_bound_singleton;
  assert((ignore_result(
              "Key Type is singleton, but does not match bound singleton."),
          store_is_valid));

  m_storedObjects.insert(std::make_pair(
      UniqueKeyGenerator::Get<store_key<Key, N>>(), std::move(value)));
}

template <class Key, const type_identifier N>
void ContextBase::StoreShared(std::shared_ptr<Key> value) {
  static_assert(!std::is_base_of<CommandBase, Key>(),
                "Cannot store Command type");
  static_assert(!std::is_base_of<SignalBase, Key>(),
                "Cannot store Signal type");
  static_assert(!std::is_base_of<ContextBase, Key>(),
                "Cannot store Context type");
  const bool value_is_singleton_type =
      m_instanceMap.count(UniqueKeyGenerator::Get<Key>()) > 0;
  const bool value_matches_bound_singleton =
      value_is_singleton_type && value.get() == Resolve<Key>().get();
  const bool store_is_valid =
      !value_is_singleton_type || value_matches_bound_singleton;
  assert((ignore_result(
              "Key Type is singleton, but does not match bound singleton."),
          store_is_valid));

  m_sharedStoredObjects.insert(std::make_pair(
      UniqueKeyGenerator::Get<store_key<Key, N>>(), std::move(value)));
}

template <class Key, const type_identifier N>
decltype(auto) ContextBase::GetFromStore() {
  auto storedResult =
      m_storedObjects.find(UniqueKeyGenerator::Get<store_key<Key, N>>());
  if (storedResult == m_storedObjects.end()) {
    throw std::runtime_error("No stored object of type " +
                             std::string(typeid(Key).name()));
  }

  return std::static_pointer_cast<Key>(storedResult->second);
}

template <class Key, const type_identifier N>
decltype(auto) ContextBase::GetFromSharedStore() {
  auto storedResult =
      m_sharedStoredObjects.find(UniqueKeyGenerator::Get<store_key<Key, N>>());
  if (storedResult == m_sharedStoredObjects.end()) {
    throw std::runtime_error("No stored object of type " +
                             std::string(typeid(Key).name()));
  }

  return std::static_pointer_cast<Key>(storedResult->second);
}

template <class Key, const type_identifier N>
void ContextBase::DeleteFromStore() {
  auto storedResult =
      m_storedObjects.find(UniqueKeyGenerator::Get<store_key<Key, N>>());
  if (storedResult == m_storedObjects.end()) {
    throw std::runtime_error("No stored object of type " +
                             std::string(typeid(Key).name()));
  }

  m_toRemoveStoredObjects.insert(UniqueKeyGenerator::Get<store_key<Key, N>>());
}

template <class Key, const type_identifier N>
void ContextBase::DeleteFromSharedStore() {
  auto storedResult =
      m_sharedStoredObjects.find(UniqueKeyGenerator::Get<store_key<Key, N>>());
  if (storedResult == m_sharedStoredObjects.end()) {
    throw std::runtime_error("No stored object of type " +
                             std::string(typeid(Key).name()));
  }

  m_toRemoveSharedStoredObjects.insert(UniqueKeyGenerator::Get<store_key<Key, N>>());
}

template <class Key, const type_identifier N>
bool ContextBase::HasStored() {
  auto storedResult =
      m_storedObjects.find(UniqueKeyGenerator::Get<store_key<Key, N>>());
  return storedResult != m_storedObjects.end();
}

template <class Key, const type_identifier N>
bool ContextBase::HasSharedStored() {
  auto storedResult =
      m_sharedStoredObjects.find(UniqueKeyGenerator::Get<store_key<Key, N>>());
  return storedResult != m_sharedStoredObjects.end();
}

template <class Key>
bool ContextBase::HasUpdatable() {
  auto updatableResult =
      m_updatableObjects.find(UniqueKeyGenerator::Get<Key>());
  return updatableResult != m_updatableObjects.end();
}

template <class Key>
void ContextBase::AddUpdatable(std::shared_ptr<Key> value) {
  static_assert(std::is_base_of<IUpdatable, Key>(),
                "Attempting to use 'AddUpdatable' with non-Updatable type.");
  assert((ignore_result("Attempting to bind already bound key."),
          m_updatableObjects.count(UniqueKeyGenerator::Get<Key>()) == 0));

  m_updatableObjects.insert(
      std::make_pair(UniqueKeyGenerator::Get<Key>(), std::move(value)));
}

template <class Key>
decltype(auto) ContextBase::GetUpdatable() {
  auto updatableResult =
      m_updatableObjects.find(UniqueKeyGenerator::Get<Key>());
  if (updatableResult == m_updatableObjects.end()) {
    throw std::runtime_error("No stored updatable of type " +
                             std::string(typeid(Key).name()));
  }

  return std::static_pointer_cast<Key>(updatableResult->second);
}

template <class Key>
void ContextBase::RemoveUpdatable() {
  auto updatableResult =
      m_updatableObjects.find(UniqueKeyGenerator::Get<Key>());
  if (updatableResult == m_updatableObjects.end()) {
    throw std::runtime_error("No stored updatable of type " +
                             std::string(typeid(Key).name()));
  }
  m_toRemoveUpdatableObjects.insert(UniqueKeyGenerator::Get<Key>());
}
// ----- End ContextBase ----- //

}  // namespace framework
}  // namespace culprit
