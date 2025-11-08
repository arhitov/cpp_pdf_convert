#!/bin/bash

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
BIN_DIR="${PROJECT_DIR}/bin"
BIN_NAME="pdf_convert"
MUPDF_DIR="${PROJECT_DIR}/mupdf-source"
BUILD_TYPE="release"

echo "🔨 Building App..."

# Определяем ОС
detect_os() {
    case "$(uname -s)" in
        Linux*)
            if grep -qi "microsoft" /proc/version 2>/dev/null; then
                echo "wsl"
            else
                echo "linux"
            fi
            ;;
        MINGW*)     echo "mingw" ;;
        MSYS*)      echo "msys" ;;
        CYGWIN*)    echo "cygwin" ;;
        Darwin*)    echo "macos" ;;
        *)          echo "unknown" ;;
    esac
}

OS_TYPE=$(detect_os)
echo "🎯 Запуск в: $OS_TYPE"

case "$OS_TYPE" in
    "linux")
    # "linux"|"macos"|"wsl")
        echo "🐧 LInux"
        MUPDF_DIR="$MUPDF_DIR-linux"
        BUILD_DIR="$BUILD_DIR-linux"
        ;;
    "mingw")
    # "mingw"|"msys"|"cygwin")
        echo "🪟 Windows (MinGW)"
        MUPDF_DIR="$MUPDF_DIR-windows"
        BUILD_DIR="$BUILD_DIR-windows"
        BIN_NAME="$BIN_NAME.exe"
        # Дополнительные настройки для Windows
        if command -v pacman >/dev/null 2>&1; then
            echo "📦 Обнаружен pacman (MSYS2)"
        fi
        ;;
    *)
        echo "⚠️  Неизвестная система!"
        exit 1
        ;;
esac

# Проверяем MuPDF
if [ ! -d "$MUPDF_DIR" ]; then
    echo "❌ Error: MuPDF not found at $MUPDF_DIR"
    exit 1
fi

echo "Cleaning build directory and binary file..."
rm -rf "${BUILD_DIR}/*" 2>/dev/null || true
unlink "${BIN_DIR}/${BIN_NAME}" 2>/dev/null || true

# Сборка
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"
cmake .. && cmake --build .
# x86_64-w64-mingw32-g++ -static \
#     -I"$MUPDF_DIR/include" \
#     -DFZ_DLL_IMPORT="" \
#     -DFZ_DLL_EXPORT="" \
#     main.cpp \
#     -Wl,--start-group \
#     "$MUPDF_DIR/build/$BUILD_TYPE/libmupdf.a" \
#     "$MUPDF_DIR/build/$BUILD_TYPE/libmupdf-third.a" \
#     -Wl,--end-group \
#     -lz -lm \
#     -lgdi32 -lws2_32 -lcomdlg32 -ladvapi32 -luser32 -lkernel32 \
#     -Wl,--allow-multiple-definition \
#     -o "$BIN_NAME"

cd "$PROJECT_DIR"

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Build complete! Binary: ${BIN_DIR}/${BIN_NAME}"
    echo ""

    # Проверяем, что файл создан
    if [ -f "${BIN_DIR}/${BIN_NAME}" ]; then
        echo "📦 Binary info:"
        ls -lh "${BIN_DIR}/${BIN_NAME}"
        echo ""
        
        echo "🧪 Testing..."
        mkdir -p "${PROJECT_DIR}/stubs/out"
        unlink "${PROJECT_DIR}/stubs/out/image01.jpg"
        case "$OS_TYPE" in
            "linux")
                "${BIN_DIR}/${BIN_NAME}" "${PROJECT_DIR}/stubs/01.pdf" 1 300 "${PROJECT_DIR}/stubs/out/image01.jpg"
                if [ -f "${PROJECT_DIR}/stubs/out/image01.jpg" ]; then
                    echo "✅ Test convert pdf to image successful!"
                else
                    echo "❌ Test convert pdf to image fatal!"
                fi
                ;;
            "mingw")
                "${BIN_DIR}/${BIN_NAME}" "${PROJECT_DIR}/stubs/01.pdf" 1 300 "${PROJECT_DIR}/stubs/out/image01.jpg"
                if [ -f "${PROJECT_DIR}/stubs/out/image01.jpg" ]; then
                    echo "✅ Test convert pdf to image successful!"
                else
                    echo "❌ Test convert pdf to image fatal!"
                fi
                ;;
        esac
        # # Создаем тестовые файлы в корне проекта
        
        # echo "🧪 Testing..."
        # 
    else
        echo "❌ Error: Binary not found at ${BIN_DIR}/${BIN_NAME}"
        exit 1
    fi
else
    echo "❌ Build failed!"
    exit 1
fi