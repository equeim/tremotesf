qt5_create_translation(QM_FILES
    "${PROJECT_SOURCE_DIR}/src/torrentmodel.cpp"
    "${PROJECT_SOURCE_DIR}/qml"

    ${PROJECT_NAME}-ru.ts
)
add_custom_target(translations ALL DEPENDS ${QM_FILES})

install(FILES ${QM_FILES} DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}/translations")
