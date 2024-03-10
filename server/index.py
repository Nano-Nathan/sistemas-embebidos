from flask import Flask
from flask import url_for
from flask import render_template
from flask import request

app = Flask(__name__)

led9 = 0
led10 = 0
led11 = 0

@app.route('/', methods=['GET'])
@app.route('/<param>', methods=['GET'])
def index(param = None):
    return render_template('index.html', param = param)

@app.route('/test', methods = ['POST'])
def test():
    nombre = request.form.get('nombre')
    correo = request.form.get('correo')
    
    return render_template('index.html')

@app.route('/updateBar', methods = ['POST'])
def updateBar ():
    id = request.form.get('id')
    value = request.form.get('value')

    print(id, value)

    return ""