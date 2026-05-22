#pragma once

#include "cstdint"
#include "string"

class Document
{
  private:
    uint64_t id_;
    std::string name_;
    std::string content_;

  public:
    Document(uint64_t id, std::string name, std::string content);

    uint64_t get_id() const;
    const std::string& get_name() const;
    const std::string& get_content() const;
};