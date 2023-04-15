#include "culprit-framework/CommandBase.h"

using culprit::framework::CommandBase;

void CommandBase::Release() const { NotifyObservers(); }
