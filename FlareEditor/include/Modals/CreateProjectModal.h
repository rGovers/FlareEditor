#pragma once

#include "Modals/Modal.h"

#include <functional>
#include <list>

class CreateProjectModal : public Modal
{
public:
    using Callback = std::function<void(const std::string_view&, const std::string_view&)>;

private:
    static constexpr uint32_t BufferSize = 4096;

    std::string            m_path;
    std::string            m_name;

    std::list<std::string> m_dirs;

    Callback               m_callback;

protected:

public:
    CreateProjectModal(Callback a_callback);
    virtual ~CreateProjectModal();

    virtual bool Update();
};