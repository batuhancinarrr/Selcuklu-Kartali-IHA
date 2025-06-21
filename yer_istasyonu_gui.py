
# yer_istasyonu_gui.py
# PyQt5 tabanlı, LoRa ile haberleşen bir yer kontrol istasyonu arayüzü

import sys
import serial
import threading
from PyQt5.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QPushButton,
    QLabel, QTextEdit, QLineEdit, QHBoxLayout
)
from PyQt5.QtCore import QTimer

class YerIstasyonu(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Yer Kontrol İstasyonu")
        self.setGeometry(100, 100, 500, 400)

        self.ser = None
        self.serial_thread = None
        self.init_ui()
        self.connect_serial()

    def init_ui(self):
        layout = QVBoxLayout()

        self.status_label = QLabel("Durum: Bağlantı yok")
        layout.addWidget(self.status_label)

        self.output_area = QTextEdit()
        self.output_area.setReadOnly(True)
        layout.addWidget(self.output_area)

        # Komut girişi
        command_layout = QHBoxLayout()
        self.command_input = QLineEdit()
        self.command_input.setPlaceholderText("Komut gir (örn: STATUS)")
        self.send_button = QPushButton("Gönder")
        self.send_button.clicked.connect(self.send_command)
        command_layout.addWidget(self.command_input)
        command_layout.addWidget(self.send_button)

        layout.addLayout(command_layout)

        # Hızlı komutlar
        quick_layout = QHBoxLayout()
        self.addwp_btn = QPushButton("ADDWP (örnek)")
        self.addwp_btn.clicked.connect(lambda: self.send_command("ADDWP,41.0,29.0"))
        self.clearwp_btn = QPushButton("CLEARWP")
        self.clearwp_btn.clicked.connect(lambda: self.send_command("CLEARWP"))
        self.emergency_btn = QPushButton("EMERGENCY_LAND")
        self.emergency_btn.clicked.connect(lambda: self.send_command("EMERGENCY_LAND"))

        quick_layout.addWidget(self.addwp_btn)
        quick_layout.addWidget(self.clearwp_btn)
        quick_layout.addWidget(self.emergency_btn)
        layout.addLayout(quick_layout)

        self.setLayout(layout)

    def connect_serial(self):
        try:
            self.ser = serial.Serial('COM3', 9600, timeout=1)  # Gerekirse portu değiştir
            self.status_label.setText("Durum: Bağlandı (COM3)")
            self.serial_thread = threading.Thread(target=self.read_serial, daemon=True)
            self.serial_thread.start()
        except:
            self.status_label.setText("Durum: Seri port bağlantı hatası")

    def send_command(self, cmd=None):
        if cmd is None:
            cmd = self.command_input.text()
        if self.ser and self.ser.is_open:
            self.ser.write((cmd + '\n').encode())
            self.output_area.append("Gönderildi: " + cmd)

    def read_serial(self):
        while True:
            if self.ser and self.ser.in_waiting:
                line = self.ser.readline().decode(errors='ignore').strip()
                if line:
                    self.output_area.append("Drone: " + line)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = YerIstasyonu()
    window.show()
    sys.exit(app.exec_())
