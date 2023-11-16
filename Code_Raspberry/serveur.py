from flask import Flask
from flask import jsonify
from flask import *

import json

app = Flask(__name__)

@app.route('/TP_reaseau')
def hello_world():
    return 'Hello, World!\n'

welcome = "Welcome to 3ESE API!"

@app.route('/api/welcome/')
def api_welcome():
    return welcome

@app.route('/api/welcome/<int:x>', methods=['GET','POST', 'PATCH', 'DELETE', 'PUT'])
@app.route('/api/welcome', methods=['GET','POST', 'DELETE'])
def api_welcome_index(index=None, x=None):
	print(welcome)
	resp = {
		"method": request.method,
		"url": request.url,
		"args": request.args,
		"header": dict(request.headers),
		"message": welcome,
		"x": None,
	}

	if request.method == 'POST':
		print('POST')
		try:
			data = request.get_json()
			if "sentence" in data:
				resp["message"] = data["sentence"]
				return jsonify(resp), 201 # Create
			else:
				return jsonify(message="Missing arg sentence"), 204
		except:
			return jsonify(message="Error parsing JSON"), 400  # Bad Request
	elif request.method == 'GET':
		print('GET')
		if x != None:
			resp["x"] = x
			resp["letter"] = resp["message"][x]
			return jsonify(resp), 200
		else:
			return jsonify(resp), 200
	elif request.method == 'PUT':
		print('PUT')
		data = request.get_json()
		if x != None and "word" in data:
			resp["message"] = resp["message"][:x] + data["word"] + resp["message"][x:]
			return jsonify(resp), 200
		else:
			return jsonify(message="Missing x"), 204
	elif request.method == 'PATCH':
		print('PATCH')
		if x != None and "letter" in data:
			resp["message"][x] = data["letter"]
			return jsonify(resp), 200
		else:
			return jsonify(message="Missing x"), 204
	elif request.method == 'DELETE':
		print('DELETE')
		if x != None:
			resp["message"] = resp["message"][:x] + resp["message"][x + 1:]
			return jsonify(resp), 200
		else:
			resp["message"] = ""
			return jsonify(resp), 200


@app.errorhandler(404)
def page_not_found(error):
    return render_template('page_not_found.html'), 404

@app.route('/api/request/', methods=['GET', 'POST'])
@app.route('/api/request/<path>', methods=['GET','POST'])
def api_request(path=None):
    resp = {
            "method":   request.method,
            "url" :  request.url,
            "path" : path,
            "args": request.args,
            "headers": dict(request.headers),
    }
    if request.method == 'POST':
        resp["POST"] = {
                "data" : request.get_json(),
                }
    return jsonify(resp)
