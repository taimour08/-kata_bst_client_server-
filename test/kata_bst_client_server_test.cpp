#include "gtest/gtest.h"
#include "../src/kata_bst_client_server.h" 

// Client command: insert 5
// Server response: <No response needed>
EXPECT_EQ("insert 5", ""); // Test the command sent by the client
EXPECT_EQ(serverResponse, ""); // Test the server's response
