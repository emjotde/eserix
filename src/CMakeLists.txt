
add_library(srx OBJECT
        annotation_item.cpp
        cutter.cpp
        escaping.cpp
        object_cache.cpp
        psi_exception.cpp
        regexp.cpp
        srx_rules.cpp
        srx_segmenter.cpp
        string_frag.cpp
        string_helpers.cpp
        xml_property_tree.cpp
        zvalue.cpp
)

add_executable(eserix eserix.cpp $<TARGET_OBJECTS:srx>)
add_executable(eserix_tei eserix_tei.cpp $<TARGET_OBJECTS:srx>)

target_link_libraries(eserix ${EXT_LIBS})
target_link_libraries(eserix_tei ${EXT_LIBS})

set_target_properties(eserix 
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set_target_properties(eserix_tei
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
