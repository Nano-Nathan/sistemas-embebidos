import serial
import time
from flask import Flask
from flask import url_for
from flask import render_template
from flask import request

app = Flask(__name__)

arduino = None

@app.route('/', methods=['GET'])
@app.route('/<param>', methods=['GET'])
def index(param = None):
    arduino = serial.Serial(
        port='/dev/ttyACM0',
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
    return render_template('index.html', param = param)

@app.route('/updateBar', methods = ['POST'])
def updateBar ():
    id = request.form.get('id')
    value = request.form.get('value')

    print(id, value)
    sendData()

    return ""

def sendData ():
    if (arduino != None):
        arduino.write(('Hola Arduino!').encode("utf-8"))
        print("Datos enviados correctamente")
        arduino.close()
    else:
        print("No se ha conectado a un arduino")

    return