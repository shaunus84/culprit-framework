#pragma once

#include <memory>

#include "Notifier.hpp"
#include "Signal.hpp"

namespace culprit {
namespace framework {

class CommandBase : public Notifier {
 public:
  virtual void Execute(std::shared_ptr<SignalBase> signal) = 0;
  void Release() const;
};
}  // namespace framework
}  // namespace culprit
