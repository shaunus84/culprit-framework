#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace culprit {
namespace framework {
class CommandBase;

class SignalResponder {
  using CreatorFunction = std::function<std::shared_ptr<void>()>;
  using ResolverMap = std::unordered_map<int, std::shared_ptr<CreatorFunction>>;
  using CommandResolverMap =
      std::unordered_map<int, std::shared_ptr<CreatorFunction>>;

 public:
  SignalResponder(const ResolverMap& resolverMap,
                  CommandResolverMap& commandResolverMap, int signalID)
      : m_resolverMap{resolverMap},
        m_commandResolverMap{commandResolverMap},
        m_signalID{signalID} {}

  void Respond();
  void OnCommandReleased();
  void AddCommand(int commandID);

 private:
  inline void ExecuteCommand();

 private:
  std::vector<int> m_commands;
  unsigned int m_commandIndex{0};

  std::shared_ptr<CommandBase> currentCommand;
  const ResolverMap& m_resolverMap;
  CommandResolverMap& m_commandResolverMap;

  int attachToken{0};
  int m_signalID{0};
};

}  // namespace framework
}  // namespace culprit
