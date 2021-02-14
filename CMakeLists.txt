cmake_minimum_required (VERSION 3.0)

project (ifinvaders)

find_package(Qt1 REQUIRED CONFIG)
find_package(X11 REQUIRED)

qt1_wrap_moc(MOC_FILES
    SOURCES
        gameboard.h
        gamewindow.h
)
add_executable(ifinvaders
    barricade.cpp
    gameboard.cpp
    gamewindow.cpp
    invader.cpp
    main.cpp
    shuttle.cpp
    stl_score.cpp
    ${MOC_FILES}
)
target_link_libraries(ifinvaders
    Qt::Qt1
    X11::X11
)
