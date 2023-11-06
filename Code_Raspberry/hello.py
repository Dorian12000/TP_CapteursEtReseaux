from flask import Flask
app = Flask(__name__)

@app.route('/TP_reaseau')
def hello_world():
    return 'Hello, World!\n'
