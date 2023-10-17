import sqlite3

class datasetDAO:
    def __init__(self, db_file):
        self.conn = sqlite3.connect(db_file)
        self.create_table()

    def create_table(self):
        cursor = self.conn.cursor()
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS dados (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                data_esp TEXT,
                tensao REAL,
                nivel REAL,
                dispositivo INTEGER
            )
        ''')
        self.conn.commit()

    def insert_data(self, date, volt, nivel, dev):
        cursor = self.conn.cursor()
        cursor.execute('''
            INSERT INTO dados (data_esp, tensao, nivel, dispositivo)
            VALUES (?, ?, ?, ?)
        ''', (date, volt, nivel, dev))
        self.conn.commit()

    def get_all_data(self):
        cursor = self.conn.cursor()
        cursor.execute('SELECT * FROM dados')
        return cursor.fetchall()

    def close(self):
        self.conn.close()