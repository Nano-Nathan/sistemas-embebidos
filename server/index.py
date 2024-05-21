import serial
import time
import threading
from flask import Flask
from flask import url_for
from flask import render_template
from flask import request


app = Flask(__name__)

arduino = None
# arduino = serial.Serial(
#     '/dev/ttyACM0',
#     # 'COM3',
#     baudrate=9600,
#     bytesize=serial.EIGHTBITS,
#     parity=serial.PARITY_NONE,
#     stopbits=serial.STOPBITS_ONE,
#     timeout=1,
#     xonxoff=False,
#     rtscts=False,
#     write_timeout=10,
#     dsrdtr=False,
#     inter_byte_timeout=None,
#     exclusive=None
# )



#Thread para leer datos de un arduino
def do_nothing ():
    pass

buffer = 0
def thread_method (function=do_nothing):
    global arduino, buffer

    #Actualiza el valor cada un segundo
    while (True and (arduino != None)):
        try:
            cadena = arduino.readline().decode("utf-8")
            if (cadena != ""):
                buffer = cadena
                #Ejecuta una acción custom si debiese
                function()
        except:
            continue

################## METODOS TP1 ##############################

@app.route('/TP1', methods=['GET'])
#@app.route('/<param>', methods=['GET'])
def tp1(param = None):
    #Apago todos los leds
    arduino.write(("9\n0\n10\n0\n11\n0\n13\n0\n").encode("utf-8"))

    #Inicio el thread para leer los datos del arduino
    background_thread = threading.Thread(target=thread_method)
    background_thread.daemon = True  # Esto asegura que el hilo se detenga cuando la aplicación Flask se detenga
    background_thread.start()
    
    #return render_template('index.html', param = param)
    return render_template('TP1.html')

@app.route('/updateBar', methods = ['POST'])
def updateBar ():
    global arduino
    id = request.form.get('id') #Obtiene el numero de pin a actualizar
    value = 255 * int(request.form.get('value')) / 100 #Calcula el porcentaje de brillo del led

    #Envia el dato al arduino si tuviese uno conectado
    if (arduino != None):
        arduino.write((id + '\n' + (str)(value) + "\n").encode("utf-8"))
    else:
        print("No se ha conectado un arduino")
    return ""

#Ruta que enciende o apaga un led
@app.route('/onOff', methods = ['POST'])
def onOff ():
    global arduino
    id = request.form.get('id') #Obtiene el numero de id
    value = int(request.form.get('value')) #Obtiene si debe encender o apagar

    #Envia el dato al arduino si tuviese uno conectado
    if (arduino != None):
        arduino.write((id + '\n' + (str)(value) + "\n").encode("utf-8"))
    else:
        print("No se ha conectado un arduino")
    return ""

#Ruta que retorna el valor del sensor
@app.route('/getSensorValue', methods = ['GET'])
def getSensorValue ():
    global buffer
    return (str)(buffer)


################## METODOS TP2 ##############################
brightness = 0
more800 = False

#Metodo a ejecutar en el thread que lee el buffer
def to_do ():
    global buffer, brightness, more800
    #Si falla en la conversion del numero, significa que ha pasado o no la intensidad de 800
    try:
        brightness = int(buffer)
    except ValueError as e:
        more800 = (buffer == "active_12")

    return render_template('TP2.html', brightness=brightness, more800=more800)

@app.route('/TP2', methods=['GET'])
def tp2():
    global brightness, more800
    #Inicio el thread para leer los datos del arduino
    background_thread = threading.Thread(target=thread_method, args=(to_do,))
    background_thread.daemon = True  # Esto asegura que el hilo se detenga cuando la aplicación Flask se detenga
    background_thread.start()

    return render_template('TP2.html', brightness=brightness, more800=more800)

#Ruta que avisa detectar o no la luminosidad
@app.route('/detect', methods = ['POST'])
def detect ():
    value = int(request.form.get('value'))
    #Envia el dato al arduino si tuviese uno conectado
    if (arduino != None):
        arduino.write( ((str)(value) + "\n").encode("utf-8") )
    else:
        print("No se ha conectado un arduino")

    return ""


################## METODOS TP3 ##############################

buffer = "23/04/24-00:00:00"

@app.route('/', methods=['GET'])
@app.route('/TP3', methods=['GET'])
def tp3():
    #Inicio el thread para leer los datos del arduino
    background_thread = threading.Thread(target=thread_method)
    background_thread.daemon = True  # Esto asegura que el hilo se detenga cuando la aplicación Flask se detenga
    background_thread.start()

    return render_template('TP3.html')

#Ruta que retorna el tiempo
@app.route('/getTime', methods = ['GET'])
def getTime ():
    global buffer
    return (str)(buffer)

#Ruta que envia el dato al arduino
@app.route('/sendTime', methods = ['POST'])
def sendTime ():
    value = request.form.get('value')
    #Envia el dato al arduino si tuviese uno conectado
    if (arduino != None):
        arduino.write(value).encode("utf-8")
    else:
        print("No se ha conectado un arduino")

    return ""