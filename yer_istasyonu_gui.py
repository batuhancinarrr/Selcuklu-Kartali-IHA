
import tkinter as tk
from tkinter import ttk
import serial
import threading
import csv
from datetime import datetime

lora = serial.Serial("/dev/serial0", 9600)
root = tk.Tk()
root.title("İHA Yer İstasyonu")
root.geometry("600x400")

telemetry_text = tk.StringVar()
ttk.Label(root, textvariable=telemetry_text, font=("Courier", 10), justify="left").pack(pady=10)

def read_telemetry():
    with open("log.csv", "w", newline='') as logfile:
        writer = csv.writer(logfile)
        writer.writerow(["Zaman", "Alt", "Batarya"])
        while True:
            if lora.in_waiting:
                line = lora.readline().decode(errors='ignore').strip()
                if line.startswith("$TEL"):
                    data = line.split(",")
                    now = datetime.now().strftime("%H:%M:%S")
                    try:
                        alt = data[1].split(":")[1]
                        batt = data[2].split(":")[1]
                        telemetry = f"Zaman: {now}\nİrtifa: {alt} m\nBatarya: {batt}"
                        telemetry_text.set(telemetry)
                        writer.writerow([now, alt, batt])
                        logfile.flush()
                    except:
                        continue

def send_command(cmd):
    lora.write(f"$CMD,{cmd}\n".encode())

btn_frame = ttk.Frame(root)
btn_frame.pack(pady=10)
ttk.Button(btn_frame, text="Kalkış", command=lambda: send_command("TAKEOFF")).pack(side="left", padx=5)
ttk.Button(btn_frame, text="İniş", command=lambda: send_command("LAND")).pack(side="left", padx=5)
ttk.Button(btn_frame, text="Yüksekliği 120m yap", command=lambda: send_command("SETALT:120")).pack(side="left", padx=5)

threading.Thread(target=read_telemetry, daemon=True).start()
root.mainloop()
