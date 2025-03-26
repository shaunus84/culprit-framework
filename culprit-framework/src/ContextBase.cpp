#include "culprit-framework/ContextBase.h"

#include "culprit-framework/Signals.h"

using culprit::framework::ContextBase;

namespace {
auto swapAndPop = [](auto& vec, size_t index) -> void {
  size_t lastIndex = vec.size() - 1;
  if (index != lastIndex) {
    std::swap(vec[index], vec[lastIndex]);
    vec.pop_back();
    return;  // A swap occurred, so the element at 'index' is new.
  }
  vec.pop_back();  // No swap; we simply removed the last element.
};
}  // namespace

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
  for (auto& iterator : m_eventHandlingList) {
    iterator(pEvent);
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
    // get the element that is currently set to this updatable
    auto updatableIndex = m_updatableObjects.at(updatableKey).first;

    // get the element we are going to swap it with, whichever element is set
    // to, the last element in the vector.
    auto lastElement = std::find_if(
        m_updatableObjects.begin(), m_updatableObjects.end(), [&](auto& pair) {
          return pair.second.first == m_preUpdateList.size() - 1;
        });

    // if we found it the we can swap everything and then remove from update
    // list
    if (lastElement != m_updatableObjects.end()) {
      lastElement->second.first = updatableIndex;

      swapAndPop(m_preUpdateList, updatableIndex);
      swapAndPop(m_updateList, updatableIndex);
      swapAndPop(m_eventHandlingList, updatableIndex);
      swapAndPop(m_postUpdateList, updatableIndex);
    }

    m_updatableObjects.erase(updatableKey);
  }
  m_toRemoveUpdatableObjects.clear();

  Resolve<PreUpdateContextSignal>()->Dispatch();

  for (auto& iterator : m_preUpdateList) {
    iterator();
  }

  for (auto& child : m_childContexts) {
    child.second->PreUpdate();
  }
}

void ContextBase::Update(double deltaTime) {
  Resolve<UpdateContextSignal>()->Dispatch();

  for (auto& iterator : m_updateList) {
    iterator(deltaTime);
  }

  for (auto& child : m_childContexts) {
    child.second->Update(deltaTime);
  }
}

void ContextBase::PostUpdate() {
  Resolve<PostUpdateContextSignal>()->Dispatch();

  for (auto& iterator : m_postUpdateList) {
    iterator();
  }

  for (auto& child : m_childContexts) {
    child.second->PostUpdate();
  }
}
