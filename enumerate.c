#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sqlite3.h>

void enumerate(char *fname) {
	sqlite3 *db;

	if (sqlite3_open(fname, &db) != SQLITE_OK) {
		fprintf(stderr, "%s: %s\n", fname,  sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
	}

	char *sql = "SELECT zoom_level, tile_column, tile_row from tiles;";

	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "%s: select failed: %s\n", fname, sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		long long zoom = sqlite3_column_int(stmt, 0);
		long long x = sqlite3_column_int(stmt, 1);
		long long y = sqlite3_column_int(stmt, 2);

		y = (1LL << zoom) - 1 - y;
		printf("%s %lld %lld %lld\n", fname, zoom, x, y);
	}

	sqlite3_finalize(stmt);

        if (sqlite3_close(db) != SQLITE_OK) {
                fprintf(stderr, "%s: could not close database: %s\n", fname, sqlite3_errmsg(db));
                exit(EXIT_FAILURE);
        }
}

void usage(char **argv) {
	fprintf(stderr, "Usage: %s file.mbtiles ...\n", argv[0]);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
	extern int optind;
	//extern char *optarg;
	int i;

	while ((i = getopt(argc, argv, "")) != -1) {
		usage(argv);
	}

	if (optind >= argc) {
		usage(argv);
	}

	for (i = optind; i < argc; i++) {
		enumerate(argv[i]);
	}

	return 0;
}
