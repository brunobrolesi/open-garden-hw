if(${IDF_TARGET} STREQUAL esp8266)
    set(req esp8266)
elseif(${IDF_VERSION_MAJOR} STREQUAL 4 AND ${IDF_VERSION_MINOR} STREQUAL 1 AND ${IDF_VERSION_PATCH} STREQUAL 3)
    set(req driver)
else()
    set(req driver)
endif()

idf_component_register(
    SRCS tds.c
    INCLUDE_DIRS .
    REQUIRES ${req}
)
