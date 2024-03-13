import serial
import time
from flask import Flask
from flask import url_for
from flask import render_template
from flask import request

app = Flask(__name__)

arduino = None
# arduino = serial.Serial(
#     # '/dev/ttyACM0',
#     'COM3',
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

@app.route('/', methods=['GET'])
@app.route('/<param>', methods=['GET'])
def index(param = None):
    return render_template('index.html', param = param)

@app.route('/updateBar', methods = ['POST'])
def updateBar ():
    global arduino

    id = request.form.get('id') #Obtiene el numero de pin a actualizar
    value = 256 * int(request.form.get('value')) / 100 #Calcula el porcentaje de brillo del led
    print (value)
    #Envia el dato al arduino si tuviese uno conectado
    if (arduino != None):
        arduino.write((id + '\n' + value + "\n").encode("utf-8"))
        print("Datos enviados correctamente")
    else:
        print("No se ha conectado un arduino")
    return ""