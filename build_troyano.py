import subprocess
import os
import sys

def build():
    """Compila el código C++ a un ejecutable .exe"""

    # Ruta del compilador (ajusta según tu instalación)
    if 'g++' in os.environ.get('PATH', ''):
        compiler = 'g++'
    else:
        compiler = r'C:\MinGW\bin\g++.exe'

    print("Compilando troyano.exe...")

    # Comando de compilación
    cmd = [
        compiler,
        "main.cpp",
        "-o", "troyano.exe",
        "-std=c++17",
        "-O2",
        "-mconsole",
        "-static",            # Estático total: incrusta libstdc++, libgcc y winpthread
        "-lkernel32",
        "-luser32",
        "-lgdi32",
        "-lshell32",
        "-ladvapi32",
        "-lwininet",
        "-lnetapi32",
        "-lwinpthread",
        "-lntdll"
    ]

    try:
        result = subprocess.run(cmd, check=True)
        print("✅ Compilación exitosa!")

        # Verificar que el archivo se creó
        if os.path.exists("troyano.exe"):
            size = os.path.getsize("troyano.exe")
            print(f"📦 Archivo generado: troyano.exe ({size / 1024:.2f} KB)")
            return True
    except subprocess.CalledProcessError as e:
        print(f"❌ Error de compilación: {e}")
        return False
    except FileNotFoundError:
        print("❌ No se encontró el compilador g++. Asegúrate de instalar MinGW.")
        return False

if __name__ == "__main__":
    build()