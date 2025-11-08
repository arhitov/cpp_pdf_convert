#!/bin/bash

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
BIN_DIR="${PROJECT_DIR}/bin"
MUPDF_DIR="${PROJECT_DIR}/mupdf-1.26.11-source"

# Проверяем MuPDF
if [ ! -d "$MUPDF_DIR" ]; then
    echo "❌ Error: MuPDF not found at $MUPDF_DIR"
    exit 1
fi

# --- Проверка зависимостей ---
echo "🔍 Checking dependencies..."

# Проверяем, установлены ли системные библиотеки
for lib in lcms2 jpeg openjp2 jbig2dec gumbo freetype harfbuzz; do
    if ! ldconfig -p | grep -q $lib; then
        echo "⚠️  Library $lib not found. Installing..."
        sudo apt-get install -y lib${lib}-dev
    else
        echo "✅ $lib found"
    fi
done

# Удалём ранее выполненую сборку MuPDF
rm -rf "${MUPDF_DIR:?}/build/release"

# Проверяем, собран ли MuPDF, если нет - собираем
if [ ! -f "$MUPDF_DIR/build/release/libmupdf.a" ]; then
    echo "⚠️  MuPDF not built. Building..."
    cd "$MUPDF_DIR"
    
    # Очищаем предыдущую сборку
    make clean 2>/dev/null
    
    # Собираем MuPDF
    echo "Building MuPDF..."
    make HAVE_X11=no HAVE_GLUT=no BUILD=release prefix=/usr/local -j$(nproc)
    
    if [ $? -ne 0 ]; then
        echo "❌ Failed to build MuPDF"
        exit 1
    fi
    
    echo "✅ MuPDF built successfully"
    cd "$PROJECT_DIR"
fi


# 🔥 ПОЛНАЯ ОЧИСТКА - удаляем ВЕСЬ каталог build
echo "Cleaning build directory..."
rm -rf "${BUILD_DIR:?}"
mkdir -p "${BUILD_DIR}"
mkdir -p "${BIN_DIR}"

# Сборка
cd "${BUILD_DIR}"
cmake .. && cmake --build .

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Build complete! Binary: ${BIN_DIR}/pdf_convert"
    echo ""
    
    # Проверяем, что файл создан
    if [ -f "${BIN_DIR}/pdf_convert" ]; then
        echo "📦 Binary info:"
        ls -lh "${BIN_DIR}/pdf_convert"
        echo ""
        
        # Создаем тестовые файлы в корне проекта
        cd "${PROJECT_DIR}"
        mkdir -p stubs/out
        
        echo "🧪 Testing..."
        "${BIN_DIR}/pdf_convert" stubs/01.pdf 1 300 stubs/out/image01.jpg
    else
        echo "❌ Error: Binary not found at ${BIN_DIR}/pdf_convert"
        exit 1
    fi
else
    echo "❌ Build failed!"
    exit 1
fi