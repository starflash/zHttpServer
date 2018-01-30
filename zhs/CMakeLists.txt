aux_source_directory(./ SOURCE_FILES_)
aux_source_directory(./base SOURCE_FILES_BASE)
aux_source_directory(./env SOURCE_FILES_ENV)
aux_source_directory(./log SOURCE_FILES_LOG)
aux_source_directory(./misc SOURCE_FILES_MISC)
aux_source_directory(./net SOURCE_FILES_NET)
aux_source_directory(./net/http SOURCE_FILES_NET_HTTP)
aux_source_directory(./script SOURCE_FILES_SCRIPT)
aux_source_directory(./thread SOURCE_FILES_THREAD)
file (GLOB_RECURSE HEADER_FILES "*.h")
set(SOURCE_FILES
        ${HEADER_FILES}
        ${SOURCE_FILES_}
        ${SOURCE_FILES_BASE}
        ${SOURCE_FILES_ENV}
        ${SOURCE_FILES_LOG}
        ${SOURCE_FILES_MISC}
        ${SOURCE_FILES_NET}
        ${SOURCE_FILES_NET_HTTP}
        ${SOURCE_FILES_SCRIPT}
        ${SOURCE_FILES_THREAD})

add_library(zhs ${SOURCE_FILES})