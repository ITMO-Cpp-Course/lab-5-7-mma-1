#include "Document.hpp"
#include <utility>

Document::Document(uint64_t id, std::string name, std::string content)
    : id_(id), name_(std::move(name)), content_(std::move(content))
{
}

uint64_t Document::get_id() const
{
    return id_;
}

const std::string& Document::get_name() const
{
    return name_;
}

const std::string& Document::get_content() const
{
    return content_;
}
