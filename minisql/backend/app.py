from flask import Flask, render_template, request, redirect, session
from api import add_api_rouer
from flask_cors import CORS
import os

app = Flask(__name__)
app.secret_key = os.urandom(24)
add_api_rouer(app)
CORS(app, resources=r'/*', supports_credentials=True)

@app.route('/')
def index():
	if session.get('usr_name'):
	    return render_template('index.html')
	else:
		return redirect("/login")

# @app.route('/login')
# def mylogin():
#     if request.method == 'GET':
#         return render_template('login.html')
#     if request.method == 'POST':
#         if request.form.get('username') == 'anwen':
#             session['user'] = request.form.get('username')
#             return redirect('/')


    

if __name__ == '__main__':
    app.run(port=15000)
