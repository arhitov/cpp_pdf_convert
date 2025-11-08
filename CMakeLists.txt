cmake_minimum_required(VERSION 3.12)
project(pdf_convert)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Определяем разрядность
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_BITS 64)
    message(STATUS "🔍 Detected 64-bit architecture")
else()
    set(ARCH_BITS 32)
    message(FATAL_ERROR "🔍 Detected 32-bit architecture")
endif()


# --- LOCAL MuPDF CONFIGURATION ---
if(MSYS OR MINGW OR CYGWIN OR CMAKE_HOST_WIN32)
    message(STATUS "🎯 Detected Windows environment")
    set(MUPDF_PLATFORM "windows")
    # Корневая директория MuPDF
    set(MUPDF_ROOT ${CMAKE_SOURCE_DIR}/mupdf-source-windows)
    # Определяем путь к библиотеке
    set(MUPDF_BUILD_DIR ${MUPDF_ROOT}/build/release)
    set(MUPDF_LIBRARY ${MUPDF_BUILD_DIR}/libmupdf.a)  # В MinGW обычно .a, даже на Windows
    # Путь к заголовкам
    set(MUPDF_INCLUDE_DIR ${MUPDF_ROOT}/include)
elseif(UNIX AND NOT APPLE)
    message(STATUS "🎯 Detected Linux environment")
    set(MUPDF_PLATFORM "linux")
    # Корневая директория MuPDF
    set(MUPDF_ROOT ${CMAKE_SOURCE_DIR}/mupdf-source-linux)
    # Определяем путь к библиотеке
    set(MUPDF_BUILD_DIR ${MUPDF_ROOT}/build/release)
    set(MUPDF_LIBRARY ${MUPDF_BUILD_DIR}/libmupdf.a)
    # Путь к заголовкам
    set(MUPDF_INCLUDE_DIR ${MUPDF_ROOT}/include)
elseif(APPLE)
    message(STATUS "🎯 Detected macOS environment")
    set(MUPDF_PLATFORM "macos")
    # ... настройки для macOS
else()
    message(FATAL_ERROR "❌ Unsupported platform")
endif()

message(STATUS "📁 MuPDF root: ${MUPDF_ROOT}")

# Проверяем существование MuPDF
if(NOT EXISTS ${MUPDF_ROOT})
    message(FATAL_ERROR "❌ MuPDF not found at ${MUPDF_ROOT}")
endif()

if(NOT EXISTS ${MUPDF_LIBRARY})
    message(FATAL_ERROR "❌ MuPDF library not built at ${MUPDF_LIBRARY}")
endif()

message(STATUS "✅ Using local MuPDF:")
message(STATUS "   Platform: ${MUPDF_PLATFORM}")
message(STATUS "   Root: ${MUPDF_ROOT}")
message(STATUS "   Include: ${MUPDF_INCLUDE_DIR}")
message(STATUS "   Library: ${MUPDF_LIBRARY}")

# Создаем каталог bin
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Собираем все исходники
set(SOURCES
    main.cpp
    validators.cpp
    # Convert handler
    handlers/convert_to_img/handler.cpp
    handlers/convert_to_img/rules.cpp
    # Help handler
    handlers/help/handler.cpp
    handlers/help/rules.cpp
    # Test handler
    handlers/test/handler.cpp
    handlers/test/rules.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${MUPDF_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/handlers/convert_to_img  # Для #include "dto.h"
    ${CMAKE_SOURCE_DIR}/handlers/help  # Для #include "dto.h"
    ${CMAKE_SOURCE_DIR}/handlers/test  # Для #include "dto.h"
)

add_executable(pdf_convert ${SOURCES})

# Линкуем с MuPDF и системными библиотеками
if(MUPDF_PLATFORM STREQUAL "windows")
    # Для Windows (MinGW)
    message(STATUS "🔗 Linking for Windows (MinGW)")
    target_link_libraries(pdf_convert 
        # Основная библиотека MuPDF
        ${MUPDF_LIBRARY}
        
        # Внутренние библиотеки MuPDF (если есть)
        ${MUPDF_BUILD_DIR}/libmupdf-third.a
        ${MUPDF_BUILD_DIR}/libmupdf-threads.a
        
        # Системные библиотеки Windows
        gdi32
        user32
        shell32
        advapi32

        ws2_32
        comdlg32
        
        # # Дополнительные зависимости
        # zlibstatic  # или zlib для динамической линковки
        # lcms2
        # libjpeg
        # openjp2
        # jbig2dec
        # gumbo
        # freetype
        # harfbuzz
    )
elseif(MUPDF_PLATFORM STREQUAL "linux")
    # Для Linux
    message(STATUS "🔗 Linking for Linux")
    target_link_libraries(pdf_convert 
        # Основная библиотека MuPDF
        ${MUPDF_LIBRARY}
        
        # Внутренние библиотеки MuPDF (если есть)
        ${MUPDF_BUILD_DIR}/libmupdf-third.a
        ${MUPDF_BUILD_DIR}/libmupdf-threads.a
        
        # Системные зависимости
        m          # libm
        pthread    # libpthread
        
        # Библиотеки, которые использует MuPDF
        z          # zlib
        lcms2      # Little CMS
        jpeg       # JPEG
        openjp2    # OpenJPEG
        jbig2dec   # JBIG2
        gumbo      # HTML parser
        freetype   # FreeType
        harfbuzz   # HarfBuzz
        crypto     # OpenSSL
    )
endif()

# Выводим итоговую информацию
message(STATUS "")
message(STATUS "🎊 Configuration summary:")
message(STATUS "   Project: ${PROJECT_NAME}")
message(STATUS "   Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "   Architecture: ${ARCH_BITS}-bit")
message(STATUS "   Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}")
message(STATUS "")