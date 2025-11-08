cmake_minimum_required(VERSION 3.12)
project(pdf_convert)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- LOCAL MuPDF CONFIGURATION ---
# Корневая директория MuPDF
set(MUPDF_ROOT ${CMAKE_SOURCE_DIR}/mupdf-1.26.11-source)

# Путь к заголовкам (обычно в source/ или include/)
set(MUPDF_INCLUDE_DIR ${MUPDF_ROOT}/include)

# Путь к скомпилированной библиотеке
set(MUPDF_BUILD_DIR ${MUPDF_ROOT}/build/release)

# Проверяем существование MuPDF
if(NOT EXISTS ${MUPDF_ROOT})
    message(FATAL_ERROR "❌ MuPDF not found at ${MUPDF_ROOT}")
endif()

if(NOT EXISTS ${MUPDF_BUILD_DIR}/libmupdf.a)
    message(FATAL_ERROR "❌ MuPDF library not built at ${MUPDF_BUILD_DIR}/libmupdf.a\n   Build MuPDF first: cd ${MUPDF_ROOT} && make HAVE_X11=no HAVE_GLUT=no BUILD=release")
endif()

message(STATUS "✅ Using local MuPDF:")
message(STATUS "   Include: ${MUPDF_INCLUDE_DIR}")
message(STATUS "   Library: ${MUPDF_BUILD_DIR}/libmupdf.a")

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
    # Copy handler
    handlers/copy/handler.cpp
    handlers/copy/rules.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${MUPDF_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/handlers/conver_to_img  # Для #include "dto.h"
    ${CMAKE_SOURCE_DIR}/handlers/copy
)

add_executable(pdf_convert ${SOURCES})

# Линкуем с MuPDF и системными библиотеками
target_link_libraries(pdf_convert 
    # Основная библиотека MuPDF
    ${MUPDF_BUILD_DIR}/libmupdf.a
    
    # Внутренние библиотеки MuPDF (если есть)
    ${MUPDF_BUILD_DIR}/libmupdf-third.a
    ${MUPDF_BUILD_DIR}/libmupdf-threads.a
    
    # Системные зависимости (обязательно!)
    m          # libm
    pthread    # libpthread
    
    # Библиотеки, которые использует MuPDF
    z          # zlib (для compress.c)
    lcms2      # Little CMS (для color-lcms.c)
    jpeg       # JPEG
    openjp2    # OpenJPEG
    jbig2dec   # JBIG2
    gumbo      # HTML parser
    freetype   # FreeType
    harfbuzz   # HarfBuzz
    crypto     # OpenSSL (иногда)
)