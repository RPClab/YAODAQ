#pragma once

#include "Infos.hpp"
#include "toml.hpp"

class BoardInfos: public Infos
{
public:
  BoardInfos(const std::string& roomName = "", const std::string& rackName = "", const std::string& crateName = "", const std::string& name = "", const std::string& type = "", const toml::value& parameters = {},
             const toml::value& connectorParameters = {}, const Category& category=Category::UNKNOWN);
  BoardInfos(const Infos& infos, const toml::value& parameters = {}, const toml::value& connectorParameters = {});
  void setParameters(const toml::value& parameters)
  {
    m_Parameters=parameters;
  }
  void setConnectorParameters(const toml::value& parameters)
  {
    m_ConnectorParameters=parameters;
  }
  toml::value getParameters()
  {
    return m_Parameters;
  }
  toml::value getConnectorParameters()
  {
    return m_ConnectorParameters;
  }
private:
  toml::value m_ConnectorParameters;
  toml::value m_Parameters;
};
