#pragma once

namespace culprit {
namespace framework {
class IUpdatable {
 public:
  virtual ~IUpdatable() {}
  virtual void Update(double deltaTime) = 0;
};
}  // namespace framework
}  // namespace culprit
