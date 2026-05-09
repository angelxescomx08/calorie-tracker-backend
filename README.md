# calorie-tracker-backend

Backend en C++ usando el framework [Drogon](https://github.com/drogonframework/drogon).

---

## Requisitos previos (Windows)

| Herramienta | Versión mínima | Descarga |
|-------------|---------------|---------|
| Visual Studio 2022 | con "Desarrollo para el escritorio con C++" | https://visualstudio.microsoft.com/ |
| CMake | 3.15+ | https://cmake.org/download/ |
| Git | cualquiera | https://git-scm.com/ |
| vcpkg | cualquiera | ver abajo |

---

## 1. Instalar vcpkg

Abre **Developer Command Prompt for VS 2022** o **PowerShell** y ejecuta:

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
```

Agrega `C:\vcpkg` a tu variable de entorno `PATH`.

---

## 2. Instalar Drogon con vcpkg

```powershell
vcpkg install drogon:x64-windows
```

Esto instala Drogon y todas sus dependencias automáticamente (trantor, jsoncpp, zlib, openssl, etc.). Puede tardar varios minutos la primera vez.

---

## 3. Compilar el proyecto

```powershell
# Desde la raíz del proyecto
cmake -B build -S . `
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DCMAKE_BUILD_TYPE=Release

cmake --build build --config Release
```

El ejecutable queda en `build\Release\calorie-tracker-backend.exe`.

---

## 4. Ejecutar

```powershell
.\build\Release\calorie-tracker-backend.exe
```

Abre tu navegador en [http://localhost:8080](http://localhost:8080) y verás:

```
Hello, World!
```

---

## Estructura del proyecto

```
calorie-tracker-backend/
├── main.cpp          # Punto de entrada, rutas HTTP
├── CMakeLists.txt    # Configuración de build
└── README.md         # Este archivo
```

---

## Notas

- El servidor escucha en el puerto **8080** por defecto.
- Para cambiar el puerto edita la línea `addListener("0.0.0.0", 8080)` en `main.cpp`.
- Drogon requiere **C++17** o superior.
- Si usas Visual Studio directamente, abre la carpeta del proyecto (File → Open → Folder) y VS detectará el `CMakeLists.txt` automáticamente. Asegúrate de configurar el cmake toolchain en `CMakeSettings.json`:

```json
{
  "cmakeToolchain": "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
}
```
