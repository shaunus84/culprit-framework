#include "culprit-framework/ContextBase.h"

#include "culprit-framework/Signals.h"

using culprit::framework::ContextBase;

void ContextBase::Initialise() {
  // Ensure a parents enter and exit signals are not in the accumulated map
  // before building.
  RemoveBind<EnterContextSignal>();
  RemoveBind<ExitContextSignal>();
  RemoveBind<PreUpdateContextSignal>();
  RemoveBind<UpdateContextSignal>();
  RemoveBind<PostUpdateContextSignal>();
  RemoveBind<ContextBase>();

  SetBindings();

  // If this context did not bind enter and exit context, bind them anyway.
  if (!HasBinding<EnterContextSignal>()) {
    BindSignal<EnterContextSignal>();
  }

  if (!HasBinding<ExitContextSignal>()) {
    BindSignal<ExitContextSignal>();
  }

  if (!HasBinding<PreUpdateContextSignal>()) {
    BindSignal<PreUpdateContextSignal>();
  }

  if (!HasBinding<UpdateContextSignal>()) {
    BindSignal<UpdateContextSignal>();
  }

  if (!HasBinding<PostUpdateContextSignal>()) {
    BindSignal<PostUpdateContextSignal>();
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

void ContextBase::HandleEvents(const void* pEvent) {
  for (auto& iterator : m_updatableObjects) {
    iterator.second->HandleEvents(pEvent);
  }

  for (auto& child : m_childContexts) {
    child.second->HandleEvents(pEvent);
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

  Resolve<PreUpdateContextSignal>()->Dispatch();

  for (auto& iterator : m_updatableObjects) {
    iterator.second->PreUpdate();
  }

  for (auto& child : m_childContexts) {
    child.second->PreUpdate();
  }
}

void ContextBase::Update(double deltaTime) {
  Resolve<UpdateContextSignal>()->Dispatch();

  for (auto& iterator : m_updatableObjects) {
    iterator.second->Update(deltaTime);
  }

  for (auto& child : m_childContexts) {
    child.second->Update(deltaTime);
  }
}

void ContextBase::PostUpdate() {
  Resolve<PostUpdateContextSignal>()->Dispatch();

  for (auto& iterator : m_updatableObjects) {
    iterator.second->PostUpdate();
  }

  for (auto& child : m_childContexts) {
    child.second->PostUpdate();
  }
}
