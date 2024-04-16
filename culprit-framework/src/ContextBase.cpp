#include "culprit-framework/ContextBase.h"

#include "culprit-framework/Signals.h"

using culprit::framework::ContextBase;

void ContextBase::Initialise() {
  // Ensure a parents enter and exit signals are not in the accumulated map
  // before building.
  RemoveBind<EnterContextSignal>();
  RemoveBind<ExitContextSignal>();
  RemoveBind<ContextBase>();

  SetBindings();

  // If this context did not bind enter and exit context, bind them anyway.
  if (!HasBinding<EnterContextSignal>()) {
    BindSignal<EnterContextSignal>();
  }

  if (!HasBinding<ExitContextSignal>()) {
    BindSignal<ExitContextSignal>();
  }

  Build();
}

void ContextBase::Enter() { Resolve<EnterContextSignal>()->Dispatch(); }

void ContextBase::Exit() {
  Resolve<ExitContextSignal>()->Dispatch();

  for (const auto& attached : m_attachedCommands) {
    auto signal = std::static_pointer_cast<Notifier>(
        m_instanceMap.at(attached.first));  // get the signal
    for (const auto& command : attached.second) {
      signal->Detach(command);
    }
  }

  for (auto& storedKey : m_toRemoveStoredObjects) {
    m_storedObjects.erase(storedKey);
  }
  m_toRemoveStoredObjects.clear();

  for (auto& sharedStoredKey : m_toRemoveSharedStoredObjects) {
    m_sharedStoredObjects.erase(sharedStoredKey);
  }
  m_toRemoveSharedStoredObjects.clear();

  for (auto& updatableKey : m_toRemoveUpdatableObjects) {
    m_updatableObjects.erase(updatableKey);
  }
  m_toRemoveUpdatableObjects.clear();
}

void ContextBase::Build() {
  for (const auto& keys : asSingletonKeys) {
    const auto& creatorFunction = (*m_resolverMap.at(keys));
    (void)creatorFunction();
  }
}

void ContextBase::PreUpdate() {
  for (auto& storedKey : m_toRemoveStoredObjects) {
    m_storedObjects.erase(storedKey);
  }
  m_toRemoveStoredObjects.clear();

  for (auto& updatableKey : m_toRemoveUpdatableObjects) {
    m_updatableObjects.erase(updatableKey);
  }
  m_toRemoveUpdatableObjects.clear();

  for (auto& iterator : m_updatableObjects) {
    iterator.second->PreUpdate();
  }

  for (auto& child : m_childContexts) {
    child.second->PreUpdate();
  }
}

void ContextBase::Update(double deltaTime) {
  for (auto& iterator : m_updatableObjects) {
    iterator.second->Update(deltaTime);
  }

  for (auto& child : m_childContexts) {
    child.second->Update(deltaTime);
  }
}

void ContextBase::PostUpdate() {
  for (auto& iterator : m_updatableObjects) {
    iterator.second->PostUpdate();
  }

  for (auto& child : m_childContexts) {
    child.second->PostUpdate();
  }
}
