SET(DDATABSE_DATABSES_SQLITE_SOURCES
	"SQLiteDatabase.cpp"
)

add_subdirectory(Tables)

foreach(file ${DDATABASES_DATABASES_SQLITE_TABLES_SOURCES})
	set(DDATABSE_DATABSES_SQLITE_SOURCES ${DDATABSE_DATABSES_SQLITE_SOURCES} "Tables/${file}")
endforeach()

set(DDATABSE_DATABSES_SQLITE_SOURCES ${DDATABSE_DATABSES_SQLITE_SOURCES} PARENT_SCOPE)
