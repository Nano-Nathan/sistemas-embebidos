import serial
import time
import threading
from flask import Flask
from flask import url_for
from flask import render_template
from flask import request


app = Flask(__name__)

#arduino = None
arduino = serial.Serial(
    # '/dev/ttyACM0',
    'COM8',
    baudrate=9600,
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    timeout=1,
    xonxoff=False,
    rtscts=False,
    write_timeout=10,
    dsrdtr=False,
    inter_byte_timeout=None,
    exclusive=None
)

buffer = ""
def thread_method ():
    global arduino
    global buffer

    #Actualiza el valor cada un segundo
    while (True and (arduino != None)):
        try:
            cadena = arduino.readline().decode("utf-8")

            if (cadena != ""):
                buffer = cadena
        except:
            continue

@app.route('/', methods=['GET'])
def tp3():
    #Inicio el thread para leer los datos del arduino
    background_thread = threading.Thread(target=thread_method)
    background_thread.daemon = True  # Esto asegura que el hilo se detenga cuando la aplicación Flask se detenga
    background_thread.start()

    return render_template('index.html')

#Ruta que retorna el tiempo
@app.route('/getValues', methods = ['GET'])
def getValues ():
    global buffer
    #Espera los datos del arduino
    while buffer == "":
        pass
    
    #Guarda los datos a enviar y revierte el buffer para avisar que ya lo leyo
    cadena = buffer
    buffer = ""

    #Retorna el valor del arduino
    return (str)(cadena)

#Ruta que envia el dato al arduino
@app.route('/sendOrder', methods = ['POST'])
def sendOrder ():
    global buffer

    value = request.form.get('value')

    #Si quiere realizar una acción pero aun no se obtuvieron resultados, no realiza nada
    if buffer == "":
        #Envia el dato al arduino si tuviese uno conectado
        if (arduino != None):
            arduino.write(value.encode("utf-8"))
        else:
            print("No se ha conectado un arduino")
    else:
        print("JUEGO EN EJECUCIÓN. ESPERE.")

    return ""