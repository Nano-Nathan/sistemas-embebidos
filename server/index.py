import serial
import time
from flask import Flask
from flask import url_for
from flask import render_template
from flask import request

app = Flask(__name__)

arduino = serial.Serial(
    '/dev/ttyACM0',
    #'COM3',
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

@app.route('/', methods=['GET'])
@app.route('/<param>', methods=['GET'])
def index(param = None):
    arduino = serial.Serial(
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
    value = 256 * request.form.get('value') / 100

    print(id, value)
    sendData()

    return ""

b = True
def sendData ():
    global b

    if (arduino != None):
        if b:
            arduino.write(('1\n').encode("utf-8"))
        else:
            arduino.write(('0\n').encode("utf-8"))
        
        b = not b

        print("Datos enviados correctamente")
        #arduino.close()
    else:
        print("No se ha conectado a un arduino")

    return