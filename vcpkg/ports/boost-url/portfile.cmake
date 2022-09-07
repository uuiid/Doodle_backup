# Automatically generated by scripts/boost/generate-ports.ps1

vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO CPPAlliance/url
        REF 83f34cc79c0905357cbe7faad3430a727c67c061
        SHA512 bf5e8cc98515eda5f135aff4723fec24a5c35dfe43013d7bd90639a8a0800d3c6da0592ced9ec51148ee09b2ea50c1aeab59c4b68e54c45a802bda6a0a39c0ec
        HEAD_REF master
)

include(${CURRENT_INSTALLED_DIR}/share/boost-vcpkg-helpers/boost-modular-headers.cmake)
boost_modular_headers(SOURCE_PATH ${SOURCE_PATH})