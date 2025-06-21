import serial
import threading
import tkinter as tk
from tkinter import ttk
import base64
from PIL import Image, ImageTk
import io

class LoRaReceiver:
    def __init__(self, port, baudrate=9600):
        self.ser = serial.Serial(port, baudrate, timeout=1)
        self.last_packet_id = 0
        self.root = tk.Tk()
        self.root.title("LoRa Yer Ýstasyonu")
        self.telemetry_text = tk.StringVar()
        self.image_label = ttk.Label(self.root)
        self.image_label.pack()
        self.telemetry_label = ttk.Label(self.root, textvariable=self.telemetry_text, font=("Arial", 14))
        self.telemetry_label.pack()
        self.running = True

    def calculate_checksum(self, data):
        checksum = 0
        for ch in data:
            checksum ^= ord(ch)
        return checksum

    def read_loop(self):
        while self.running:
            if self.ser.in_waiting:
                line = self.ser.readline().decode('utf-8', errors='ignore').strip()
                if not line:
                    continue

                parts = line.split("|")
                if len(parts) != 3:
                    continue

                packet_id, payload, checksum_str = parts
                try:
                    packet_id = int(packet_id)
                    checksum_recv = int(checksum_str)
                except:
                    continue

                checksum_calc = self.calculate_checksum(payload)
                if checksum_calc != checksum_recv:
                    print(f"Paket {packet_id} checksum hatasý.")
                    continue

                if packet_id != self.last_packet_id + 1:
                    print(f"Paket sýra hatasý. Beklenen: {self.last_packet_id + 1}, Gelen: {packet_id}")

                self.last_packet_id = packet_id
                self.process_payload(payload)

    def process_payload(self, payload):
        if payload.startswith("voltaj:"):
            self.telemetry_text.set(payload)
        elif payload.startswith("IMG:"):
            b64data = payload[4:]
            self.show_image(b64data)

    def show_image(self, b64data):
        try:
            img_bytes = base64.b64decode(b64data)
            image = Image.open(io.BytesIO(img_bytes))
            image = image.resize((320, 240))
            photo = ImageTk.PhotoImage(image)
            self.image_label.configure(image=photo)
            self.image_label.image = photo
        except Exception as e:
            print("Görüntü iþlenirken hata:", e)

    def start(self):
        threading.Thread(target=self.read_loop, daemon=True).start()
        self.root.protocol("WM_DELETE_WINDOW", self.stop)
        self.root.mainloop()

    def stop(self):
        self.running = False
        self.ser.close()
        self.root.destroy()

if __name__ == "__main__":
    port_name = "COM3"  # Seri portunuzu buraya yazýn (örneðin: "COM3" veya "/dev/ttyUSB0")
    receiver = LoRaReceiver(port_name)
    receiver.start()
