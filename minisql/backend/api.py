# -*- coding: utf-8 -*-
from flask import request, redirect, session, jsonify
from database import clouddb
import pandas as pd
import hashlib
import time
import os
import datetime

def add_api_rouer(app):
    @app.route('/api/minisql', methods=['GET'])
    def minisql():
        sql = request.args.get('sql')
        sql = sql.replace('"', '\\"')
        # proc = os.popen('test ' + sql)
        proc = os.popen('sqlexecute \"' + sql + '\"')
        return proc.read()

