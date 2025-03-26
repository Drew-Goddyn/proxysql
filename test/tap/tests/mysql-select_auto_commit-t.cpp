#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>

#include <string>
#include <sstream>
#include "mysql.h"

#include "tap.h"
#include "command_line.h"
#include "utils.h"

int main(int argc, char** argv) {
	CommandLine cl;

	if (cl.getEnv())
		return exit_status();

	plan(6);

	MYSQL* mysql = mysql_init(NULL);
	if (!mysql)
		return exit_status();

	if (!mysql_real_connect(mysql, cl.host, cl.username, cl.password, NULL, cl.port, NULL, 0)) {
		fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(mysql));
		return exit_status();
	}

	MYSQL_QUERY(mysql, "USE test");

	const char* states[] = {
		"SELECT @@autocommit",
		"SET autocommit = 0",
		"SELECT @@autocommit",
		"SET autocommit = 1",
		"SELECT @@autocommit"
	};

	const char* expected[] = {"1", "0", "1"};
	int expected_idx = 0;

	for (int i = 0; i < 5; ++i) {
		diag("Running query: %s", states[i]);
		if (mysql_query(mysql, states[i]) != 0) {
			fprintf(stderr, "Query failed: %s\n", mysql_error(mysql));
			return exit_status();
		}

		if (strncmp(states[i], "SELECT", 6) == 0) {
			MYSQL_RES* res = mysql_store_result(mysql);
			if (!res) {
				ok(false, "No result returned for SELECT @@autocommit");
				continue;
			}

			MYSQL_ROW row;
			unsigned long long num_rows = mysql_num_rows(res);
			ok(num_rows == 1, "mysql_num_rows() , expected: 1 , actual: %llu", num_rows);

			row = mysql_fetch_row(res);
			if (row && row[0]) {
				ok(strcmp(row[0], expected[expected_idx]) == 0, "@@autocommit expected: \"%s\" , actual: \"%s\"", expected[expected_idx], row[0]);
				expected_idx++;
			} else {
				ok(false, "No row or value returned from SELECT @@autocommit");
			}
			mysql_free_result(res);
		}
	}

	mysql_close(mysql);
	return exit_status();
}