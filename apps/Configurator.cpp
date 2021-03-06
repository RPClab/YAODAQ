#include "Configurator.hpp"

#include "CLI/CLI.hpp"
#include "ProgramInfos.hpp"
#include "spdlog.h"
//#include "fmt/core.h"
//#include "fmt/format.h"
/*#include "chrono.h"
#include "color.h"
#include "compile.h"
#include "format-inl.h"
#include "locale.h"
#include "os.h"
#include "ostream.h"
#include "posix.h"
#include "printf.h"
#include "ranges.h"*/

int main(int argc, char** argv)
{
  ProgramInfos infos;
  infos.Logo();
  Interrupt interrupt;
  CLI::App  app{"Websocket Server"};
  int       port{8282};
  app.add_option("-p,--port", port, "Port to listen")->check(CLI::Range(0, 65535));
  std::string host{ix::SocketServer::kDefaultHost};
  app.add_option("-i,--ip", host, "IP of the server")->check(CLI::ValidIPV4);
  int backlog{ix::SocketServer::kDefaultTcpBacklog};
  app.add_option("-b,--backlog", backlog, "Backlog")->check(CLI::Range(0, 5));
  std::size_t maxConnections{ix::SocketServer::kDefaultMaxConnections};
  app.add_option("-m,--max", maxConnections, "Maximun connections")->check(CLI::PositiveNumber);
  int handshakeTimeoutSecs{3};
  app.add_option("-t,--timeout", handshakeTimeoutSecs, "Timeout in seconds")->check(CLI::PositiveNumber);
  try
  {
    app.parse(argc, argv);
  }
  catch(const CLI::ParseError& e)
  {
    return app.exit(e);
  }
  bool         stop{false};
  char         answer{'a'};
  Configurator configurator(port, host, backlog, maxConnections, handshakeTimeoutSecs);
  configurator.listen();
  configurator.start();
  fmt::print("Websocket server started on ip {0} port {1} !\n", host, port);
  fmt::print("Type q and ENTER to stop it !\n");
  do
  {
    std::cin >> answer;
    if(answer == 'q' || answer == 'Q') stop = true;
  } while(stop == false);
  configurator.stop();
  fmt::print("Bye !\n");
  return interrupt.wait();
}
