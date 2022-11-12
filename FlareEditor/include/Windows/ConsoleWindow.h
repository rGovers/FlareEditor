#pragma once

#include "Window.h"

#include "Logger.h"

struct ConsoleMessage
{
    std::string Message;
    uint32_t Count;
    e_MessageType Type;
};

class ConsoleWindow : public Window
{
private:
    constexpr static int MaxMessages = 2048;

    constexpr static int DisplayMessageBit = 0;
    constexpr static int DisplayWarningBit = 1;
    constexpr static int DisplayErrorBit = 2;
    constexpr static int CollapseBit = 3;

    std::vector<ConsoleMessage> m_messages;

    unsigned char               m_flags;

    // bool                        m_displayMessage;
    // bool                        m_displayWarning;
    // bool                        m_displayError;
    // bool                        m_collapse;

protected:

public:
    ConsoleWindow();
    virtual ~ConsoleWindow();

    void AddMessage(const std::string_view& a_message, e_MessageType a_type);
    
    virtual void Update();
};