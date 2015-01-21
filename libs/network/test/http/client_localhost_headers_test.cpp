//
//          Copyright Patrick Heeb 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE http 1.0 localhost tests

#include <boost/config/warning_disable.hpp>
#include <boost/config.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/network/include/http/client.hpp>
#include <boost/range.hpp>
#include <boost/range/iterator.hpp>
#include <boost/cast.hpp>
#include <string>
#include <fstream>
#include <iostream>

#include "http_test_server.hpp"
#include "client_types.hpp"

using std::cout;
using std::endl;

namespace {
const std::string base_url = "http://localhost:8000";

struct running_server_fixture {
  // NOTE: Can't use BOOST_REQUIRE_MESSAGE here, as Boost.Test data structures
  // are not fully set up when the global fixture runs.
  running_server_fixture() {
    if (!server.start())
      cout << "Failed to start HTTP server for test!" << endl;
  }

  ~running_server_fixture() {
    if (!server.stop()) cout << "Failed to stop HTTP server for test!" << endl;
  }

  http_test_server server;
};
}

BOOST_GLOBAL_FIXTURE(running_server_fixture);

using namespace boost::network::http;

BOOST_AUTO_TEST_CASE_TEMPLATE(http_client_headers_test, client, client_types) {
  typename client::request request(base_url + "/cgi-bin/echo_headers_ws.py");
  typename client::response response;
  BOOST_REQUIRE_NO_THROW(response = client().post(request));
  BOOST_REQUIRE(headers(response).count("Content-type") != 0);
  typename headers_range<typename client::response>::type range =
      headers(response)["Content-type"];

  BOOST_CHECK(boost::begin(range)->first == "Content-type");
  BOOST_CHECK(boost::begin(range)->second == "text/plain; charset=us-ascii");

  BOOST_REQUIRE(headers(response).count("X-Cpp-Netlib") != 0);
  headers_range<http::client::response>::type range2 =
      headers(response)["X-Cpp-Netlib"];
  BOOST_CHECK(boost::begin(range2)->first == "X-Cpp-Netlib");
  BOOST_CHECK(boost::begin(range2)->second == "rocks!");

}
