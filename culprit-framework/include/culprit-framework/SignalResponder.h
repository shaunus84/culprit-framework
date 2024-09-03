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
  using ResolverMap = std::unordered_map<std::size_t, std::shared_ptr<CreatorFunction>>;
  using CommandResolverMap =
      std::unordered_map<std::size_t, std::shared_ptr<CreatorFunction>>;

 public:
  SignalResponder(const ResolverMap& resolverMap,
                  CommandResolverMap& commandResolverMap, std::size_t signalID)
      : m_resolverMap{resolverMap},
        m_commandResolverMap{commandResolverMap},
        m_signalID{signalID} {}

  void Respond();
  void OnCommandReleased();
  void AddCommand(std::size_t commandID);

 private:
  inline void ExecuteCommand();

 private:
  std::vector<std::size_t> m_commands;
  unsigned int m_commandIndex{0};

  std::shared_ptr<CommandBase> currentCommand;
  const ResolverMap& m_resolverMap;
  CommandResolverMap& m_commandResolverMap;

  std::size_t attachToken{0};
  std::size_t m_signalID{0};
};

}  // namespace framework
}  // namespace culprit
