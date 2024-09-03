#include "culprit-framework/SignalResponder.h"

#include "culprit-framework/CommandBase.h"

using culprit::framework::SignalResponder;

void SignalResponder::Respond() {
  m_commandIndex = 0;

  ExecuteCommand();
}

void SignalResponder::OnCommandReleased() {
  currentCommand->Detach(attachToken);
  currentCommand.reset();
  m_commandIndex++;

  ExecuteCommand();
}

void SignalResponder::ExecuteCommand() {
  if (m_commandIndex < m_commands.size()) {
    const auto commandCreatorFunction =
        *m_commandResolverMap.at(m_commands[m_commandIndex]);
    currentCommand =
        std::static_pointer_cast<CommandBase>(commandCreatorFunction());
    attachToken = currentCommand->Attach<SignalResponder>(
        this, &SignalResponder::OnCommandReleased);

    const auto signalCreatorFunction = *m_resolverMap.at(m_signalID);
    const auto triggeringSignal =
        std::static_pointer_cast<SignalBase>(signalCreatorFunction());
    currentCommand->Execute(triggeringSignal);
  }
}

void SignalResponder::AddCommand(
    std::size_t commandID) {
  m_commands.push_back(commandID);
}
