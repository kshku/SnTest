set(HEADERFILES
    api.h
    logger.h
    sntest.h
)

set(SRCS
    sntest.c
    logger.c
)

set(INCLUDE_BASE "${PROJECT_SOURCE_DIR}/test/include/sntest")

list(TRANSFORM HEADERFILES PREPEND "${INCLUDE_BASE}/")

target_sources(sntest PRIVATE ${HEADERFILES} ${SRCS})

