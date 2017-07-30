cmake_minimum_required(VERSION 3.7)
project(Server)

set(CMAKE_CXX_STANDARD 17)


#
set(SOURCE_FILES
        main.cpp                        Service.h
        Server.cpp                      Server.h
        ClientConnection.cpp            ClientConnection.h
        MIME/MIME_Detector.cpp          MIME/MIME_Detector.h
        Message/Message.cpp             Message/Message.h
        Location/Location.cpp           Location/Location.h
        Location/LocationHandler.cpp    Location/LocationHandler.h)
add_executable(Server ${SOURCE_FILES})


#Boost setup
set(Boost_LIBRARY_DIR /usr/local/lib)
set(Boost_INCLUDE_DIR /usr/local/include/)
find_package(Boost COMPONENTS REQUIRED system filesystem thread)

target_link_libraries(Server ${Boost_LIBRARIES})


#OpenSSl
set(OPENSSL_LIBRARY_DIR /usr/local/lib)
set(OPENSSL_INCLUDE_DIR /usr/local/include)
find_package(OpenSSL)

target_link_libraries(Server ${OPENSSL_LIBRARIES})


#
set(CMAKE_CXX_FLAGS "-Wall -lssl -lcrypto")
add_definitions(${CMAKE_CXX_FLAGS})


