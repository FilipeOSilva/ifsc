import dbDAO

if __name__ == '__main__':
    db_file = 'dados.db'
    dao = dbDAO.datasetDAO(db_file)

    # Recuperar todas as leituras
    dataframe = dao.get_all_data()
    print('All readers:')
    for row in dataframe:
        print(row)

    dao.close()