#include "gtest/gtest.h"
#include "../src/kata_bst_client_server.h" 

// Client command: insert 5
// Server response: <No response needed>
EXPECT_EQ("insert 5", ""); // Test the command sent by the client
EXPECT_EQ(serverResponse, ""); // Test the server's response

// Client command: delete 5
// Server response: <No response needed>
EXPECT_EQ("delete 5", ""); // Test the command sent by the client
EXPECT_EQ(serverResponse, ""); // Test the server's response

// Client command: find 5
// Server response: found
EXPECT_EQ("find 5", ""); // Test the command sent by the client
EXPECT_EQ(serverResponse, "found"); // Test the server's response

// Client command: find 7
// Server response: not found
EXPECT_EQ("find 7", ""); // Test the command sent by the client
EXPECT_EQ(serverResponse, "not found"); // Test the server's response

// Client command: insert 5
// Server response: ERROR: 5 already exists in BST
EXPECT_EQ("insert 5", ""); // Test the command sent by the client
EXPECT_EQ(serverResponse, "ERROR: 5 already exists in BST"); // Test the server's response

// Client command: delete 7
// Server response: ERROR: 7 not found in BST
EXPECT_EQ("delete 7", ""); // Test the command sent by the client
EXPECT_EQ(serverResponse, "ERROR: 7 not found in BST"); // Test the server's response





