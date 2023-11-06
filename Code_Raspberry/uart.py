import serial
import threading

# Cette fonction sera exécutée lorsqu'il y a des données disponibles sur le port série
def receive_data(ser):
    while True:
        if ser.in_waiting > 0:
            received_data = ser.readline().decode().strip()
            print("Données reçues:", received_data)

# Configuration de la communication UART
ser = serial.Serial(
    port='/dev/ttyAMA0',  # Assurez-vous d'utiliser le bon port UART de votre Raspberry Pi
    baudrate=115200,        # Vitesse de transmission en bauds
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1             # Délai d'attente pour la lecture
)

# Création d'un thread pour recevoir les données en arrière-plan
receive_thread = threading.Thread(target=receive_data, args=(ser,), daemon=True)
receive_thread.start()

try:
    while True:
        data_to_send = input("Entrez la commande à envoyer via UART: ")
        if data_to_send:
            ser.write(data_to_send.encode())  # Envoyer des données encodées en bytes

except KeyboardInterrupt:
    ser.close()  # Fermer la communication UART lorsqu'on interrompt le programme avec Ctrl+C

