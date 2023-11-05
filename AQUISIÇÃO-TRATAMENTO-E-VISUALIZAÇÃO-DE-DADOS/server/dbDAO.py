import sqlite3
import time
import pandas as pd
import datetime as dt

class datasetDAO:
    def __init__(self, db_file):
        self.conn = sqlite3.connect(db_file)
        self.create_table()

    def create_table(self):
        cursor = self.conn.cursor()
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS dados (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                data_local TEXT,
                data_esp TEXT,
                tensao REAL,
                nivel REAL,
                dispositivo INTEGER,
                status_bomba INTEGER,
                nivel_bomba INTEGER
            )
        ''')
        self.conn.commit()

    def insert_data(self, date, volt, nivel, dev, pump, level_pump):
        ut = time.time()

        cursor = self.conn.cursor()
        cursor.execute('''
            INSERT INTO dados (data_local, data_esp, tensao, nivel, dispositivo, status_bomba, nivel_bomba)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        ''', (ut, date, volt, nivel, dev, pump, level_pump))
        self.conn.commit()

    def get_all_data(self):
        cursor = self.conn.cursor()
        cursor.execute('SELECT * FROM dados')
        return cursor.fetchall()

    def get_last_info(self):
        cursor = self.conn.cursor()
        cursor.execute('SELECT * FROM dados WHERE dados.id = (select max(id) from dados)')
        return self.preper_ret(cursor.fetchall())

    def get_interval_data(self, init, end):
        cursor = self.conn.cursor()
        cursor.execute('SELECT * FROM dados WHERE dados.data_local >= ? AND dados.data_local <= ?', (init, end))
        return self.preper_ret(cursor.fetchall())

    def close(self):
        self.conn.close()

    def preper_ret(self, data):
        
        if len(data) == 0:
            df = pd.DataFrame(columns=['id', 'data_local', 'data_esp', 'tensao', 'volume', 'dispositivo', 'status_bomba', 'nivel_bomba'])
        else:
            df = pd.DataFrame(data)
            df.columns = ['id', 'data_local', 'data_esp', 'tensao', 'volume', 'dispositivo', 'status_bomba', 'nivel_bomba']

        df = df.astype({"data_local": float})
        df['data_local'] = [dt.datetime.fromtimestamp(x) for x in df['data_local']]
        df.set_index('data_local', inplace=True)

        return df

