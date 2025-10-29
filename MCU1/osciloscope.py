import serial
import time
import matplotlib.pyplot as plt
import numpy as np
from collections import deque

class Oscilloscope9600Hz:
    def __init__(self, port='COM3', baudrate=9600, buffer_size=1000):
        self.ser = serial.Serial(port, baudrate, timeout=1)
        self.buffer_size = buffer_size
        self.data = deque(maxlen=buffer_size)
        self.timestamps = deque(maxlen=buffer_size)
        
        # Configurar gráfica
        plt.ion()
        self.fig, self.ax = plt.subplots(figsize=(12, 6))
        self.line, = self.ax.plot([], [], 'b-', linewidth=1)
        self.ax.set_ylim(0, 4095)
        self.ax.set_xlim(0, buffer_size/9600)  # Escala de tiempo en segundos
        self.ax.set_title('STM32F446 - ADC a 9600 Hz')
        self.ax.set_xlabel('Tiempo (s)')
        self.ax.set_ylabel('Valor ADC')
        self.ax.grid(True, alpha=0.3)
        
        self.sample_count = 0
        self.start_time = time.time()
        self.last_display_time = time.time()
        
        print(f"Osciloscopio 9600Hz iniciado - {port} @ {baudrate} baud")
        print("Cerrando la ventana para detener...")
    
    def update(self):
        """Actualizar gráfica con nuevos datos"""
        bytes_available = self.ser.in_waiting
        if bytes_available >= 2:
            # Leer pares de bytes (little-endian)
            pairs_to_read = bytes_available // 2
            for _ in range(pairs_to_read):
                data = self.ser.read(2)
                if len(data) == 2:
                    adc_value = data[0] | (data[1] << 8)
                    
                    # Agregar dato al buffer con timestamp
                    self.data.append(adc_value)
                    self.timestamps.append(time.time() - self.start_time)
                    self.sample_count += 1
            
            # Actualizar gráfica periódicamente
            current_time = time.time()
            if current_time - self.last_display_time > 0.1:  # Cada 100ms
                if len(self.data) > 0:
                    self.line.set_data(list(self.timestamps), list(self.data))
                    self.ax.relim()
                    self.ax.autoscale_view(scalex=True, scaley=False)
                    self.fig.canvas.draw()
                    self.fig.canvas.flush_events()
                    
                    # Mostrar estadísticas
                    elapsed = current_time - self.start_time
                    rate = self.sample_count / elapsed
                    voltage = self.data[-1] * 3.3 / 4095
                    print(f"Muestras: {self.sample_count:6d} | ADC: {self.data[-1]:4d} | Voltaje: {voltage:.2f}V | Tasa: {rate:.1f}Hz")
                    
                    self.last_display_time = current_time
    
    def run(self):
        """Ejecutar osciloscopio"""
        try:
            while True:
                self.update()
                plt.pause(0.01)
        except KeyboardInterrupt:
            print("\nCerrando osciloscopio...")
        finally:
            self.ser.close()
            plt.close()

# Script simple para ver datos
def monitor_9600hz(port='COM3', baudrate=9600):
    """Monitoreo simple de datos a 9600Hz"""
    try:
        ser = serial.Serial(port, baudrate, timeout=1)
        print(f"Monitor 9600Hz - {port} @ {baudrate} baud")
        print("Esperando datos...")
        print("-" * 60)
        
        sample_count = 0
        start_time = time.time()
        last_display = time.time()
        
        try:
            while True:
                if ser.in_waiting >= 2:
                    data = ser.read(2)
                    adc_value = data[0] | (data[1] << 8)
                    sample_count += 1
                    
                    # Mostrar cada 100 muestras
                    if sample_count % 100 == 0:
                        current_time = time.time()
                        elapsed = current_time - start_time
                        rate = sample_count / elapsed
                        voltage = adc_value * 3.3 / 4095
                        
                        print(f"Muestra: {sample_count:6d} | ADC: {adc_value:4d} | Voltaje: {voltage:.3f}V | Tasa: {rate:.1f}Hz")
                
                time.sleep(0.001)  # Pausa mínima
                
        except KeyboardInterrupt:
            elapsed = time.time() - start_time
            print(f"\nFinalizado - {sample_count} muestras en {elapsed:.1f}s")
        
        ser.close()
        
    except serial.SerialException as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    port = input("Puerto COM (ej: COM3): ").strip()
    
    print("\nSelecciona modo:")
    print("1. Monitor simple (consola)")
    print("2. Osciloscopio (gráfico)")
    
    choice = input("Opción (1/2): ").strip()
    
    if choice == "2":
        scope = Oscilloscope9600Hz(port=port, baudrate=9600)
        scope.run()
    else:
        monitor_9600hz(port)