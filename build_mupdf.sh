#!/bin/bash

VERSION="1.24.0"
ARCHIVE_NAME="mupdf-$VERSION-source"
ARCHIVE="$ARCHIVE_NAME.tar.gz"
MUPDF_DIR="mupdf-source"
BUILD_TYPE="release"
PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "🔨 Building MuPDF..."

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
        ;;
    "mingw")
    # "mingw"|"msys"|"cygwin")
        echo "🪟 Windows (MinGW)"
        MUPDF_DIR="$MUPDF_DIR-windows"
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

echo "🔍 Проверка архива $ARCHIVE..."

# Проверка существования архива
if [ ! -f "$ARCHIVE" ]; then
    echo "❌ Ошибка: Файл $ARCHIVE не найден в текущей директории."
    echo "📁 Текущая директория: $(pwd)"
    exit 1
fi

# Проверка типа файла
if ! file "$ARCHIVE" | grep -q "gzip compressed data"; then
    echo "❌ Ошибка: $ARCHIVE не является gzip архивом"
    exit 1
fi

echo "✅ Архив найден и проверен"

# --- Проверка зависимостей ---
echo "🔍 Checking dependencies..."
case "$OS_TYPE" in
    "linux")
        # Проверяем, установлены ли системные библиотеки
        for lib in lcms2 jpeg openjp2 jbig2dec gumbo freetype harfbuzz; do
            if ! ldconfig -p | grep -q $lib; then
                echo "⚠️  Library $lib not found. Installing..."
                sudo apt-get install -y lib${lib}-dev
            else
                echo "✅ $lib found"
            fi
        done
        ;;
    "mingw")
        ;;
esac

# Проверка существования целевой директории
UNPACK=1
if [ -d "$MUPDF_DIR" ]; then
    echo "⚠️  Внимание: Директория $MUPDF_DIR уже существует."
    read -p "🗑️  Удалить существующую директорию(y) или оставить и не распоковывать(N)? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        rm -rf "$MUPDF_DIR"
        echo "✅ Существующая директория удалена"
    else
        UNPACK=0
        echo "🚫 Распаковка отменена. Используем уже распаковынные файлы."
    fi
fi

if [ $UNPACK -eq 1 ]; then
    # Распаковка архива в указанную директорию
    echo "📦 Распаковка $ARCHIVE в $MUPDF_DIR..."
    rm -rf "$MUPDF_DIR"
    rm -rf "$ARCHIVE_NAME"
    tar -xzf "$ARCHIVE"
    mv "$ARCHIVE_NAME" "$MUPDF_DIR"

    if [ $? -eq 0 ]; then
        # Показываем содержимое распакованной директории
        if [ -d "$MUPDF_DIR" ]; then
            echo "🎉 Распаковка успешно завершена!"
        else
            echo "⚠️  Предупреждение: Директория $MUPDF_DIR не найдена."
            echo "📁 Содержимое текущей директории:"
            ls -la
            exit 1
        fi
    else
        echo "❌ Ошибка при распаковке архива!"
        exit 1
    fi
fi

if [ ! -d "$MUPDF_DIR" ]; then
    echo "❌ Error: MuPDF directory not found: $MUPDF_DIR"
    exit 1
fi

cd "$MUPDF_DIR"


# 🔥 ПОЛНАЯ ОЧИСТКА - удаляем ВЕСЬ каталог build
echo "🧹 Cleaning build directory..."
# Удаляем ранее выполненную сборку MuPDF
rm -rf "${MUPDF_DIR:?}/build/${BUILD_TYPE:?}"

# Очищаем предыдущую сборку
echo "🧹 Cleaning previous build..."
make clean 2>/dev/null || true

# Компилируем MuPDF с MinGW
echo "📦 Building MuPDF..."
case "$OS_TYPE" in
    "linux")
    # "linux"|"macos"|"wsl")
        make \
            HAVE_X11=no \
            HAVE_GLUT=no \
            prefix=/usr/local \
            build=$BUILD_TYPE -j$(nproc)
        ;;
    "mingw")
    # "mingw"|"msys"|"cygwin")
        # AR=x86_64-w64-mingw32-ar \
        # make USE_SYSTEM_LIBS=no \
        #      CC=x86_64-w64-mingw32-gcc \
        #      AR=ar \
        #      build=release -j4
        make \
            USE_SYSTEM_LIBS=no \
            CC=x86_64-w64-mingw32-gcc \
            CXX=x86_64-w64-mingw32-g++ \
            AR=ar \
            OS=WIN64 \
            HAVE_X11=no \
            HAVE_GLUT=no \
            build=$BUILD_TYPE -j4
        ;;
esac

cd "$PROJECT_DIR"

FAILED=0
if [ $? -eq 0 ]; then
    echo "🔍 Checking build..."
    
    # Проверяем, что библиотеки созданы
    if [ ! -f "$MUPDF_DIR/build/$BUILD_TYPE/libmupdf.a" ]; then
        echo "❌📚 Main library: libmupdf.a"
        FAILED=1
    fi
    if [ ! -f "$MUPDF_DIR/build/$BUILD_TYPE/libmupdf-third.a" ]; then
        echo "❌📚 Third-party library: libmupdf-third.a"
        FAILED=1
    fi
    if [ ! -f "$MUPDF_DIR/build/$BUILD_TYPE/libmupdf-threads.a" ]; then
        echo "❌📚 Third-party library: libmupdf-threads.a"
        FAILED=1
    fi
else
    FAILED=1
fi

if [ $FAILED -eq 1 ]; then
    echo "❌ Failed to build MuPDF"
else
    echo "✅ MuPDF built successfully!"
fi
