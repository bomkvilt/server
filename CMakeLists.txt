cmake_minimum_required(VERSION 3.7)
project(Server)

set(CMAKE_CXX_STANDARD 14)

#Boost setup
set(REQUIRED asio)
set(Boost_LIBRARY_DIR /usr/local/lib)
set(Boost_INCLUDE_DIR /usr/local/include)
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

#
set(SOURCE_FILES
        main.cpp
                                        Service.h
        Server.cpp                      Server.h
        ClientConnection.cpp            ClientConnection.h
        MIME/MIME_Detector.cpp          MIME/MIME_Detector.h
        Message/Message.cpp             Message/Message.h
        Location/Location.cpp           Location/Location.h
        Location/LocationHandler.cpp    Location/LocationHandler.h)
add_executable(Server ${SOURCE_FILES})

# Boost check status
if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(Server ${Boost_LIBRARIES})
else()
    message(FATAL_ERROR "Unable to found Boost!")
endif()
