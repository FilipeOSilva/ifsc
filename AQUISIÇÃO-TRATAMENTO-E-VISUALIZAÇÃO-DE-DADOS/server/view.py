import streamlit as st
import pandas as pd
import datetime as dt
import dbDAO as db
import plotly.figure_factory as ff
import time

st.set_page_config(page_title='Volume de Água', page_icon=None, layout="wide", initial_sidebar_state="auto", menu_items=None)
st.title('Gerenciamento de Volume de Água')

db_file = 'dados.db'
dao = db.datasetDAO(db_file)
date_now = dt.datetime.now()
end_date = dt.datetime(date_now.year, date_now.month, date_now.day+1)
start_date = dt.datetime(end_date.year, end_date.month-1, end_date.day)

menu_header = st.empty()
col1, col2, col3 = st.columns(3)
graph_header = st.empty()
graph_primary = st.empty()
graph_secundary = st.empty()
col_graph_pump, col_graph_pump_vol = graph_secundary.columns(2)

with st.container():
    menu_header.header('Insira as informações:')

    with col1:
        date = st.selectbox('Selecione o período que deseja:', options= ['Período', 'Tempo Real'])
        data_vol = st.empty()

    with col2:
        date_init = st.date_input('Selecione a Data Inicial', start_date) 
        data_pump = st.empty()

    with col3:
        date_end = st.date_input('Selecione a Data Final', end_date)
        data_pump_vol = st.empty()


if date == 'Tempo Real':

    graph_header.empty()
    graph_primary.empty()
    graph_secundary.empty()
    graph_primary.line_chart()
    
    date_now = dt.datetime.now()

    df_old = dao.get_interval_data((date_now.timestamp() - 3600), date_now.timestamp())

    if df_old.empty:
        df_old = dao.get_last_info()

    while True:
        time.sleep(1)
        df = dao.get_last_info()

        if df.empty:
            graph_header.header('Nenhuma nova informação!')
            continue 
        
        df_old = pd.concat([df_old, df])
        
        df['nivel_bomba'] = df['nivel_bomba'] * 51
        df['status_bomba'] = df['status_bomba'] * 49

        with st.container():
            with col1:
                data_vol.metric('Variação do volume na ultima hora:','{:.2f} %'.format(df_old['volume'].mean()))

            with col2:
                data_pump.metric('Tempo de bomba ligado na ultima hora:','{:.2f} horas'.format(df_old['status_bomba'].sum()/(60*60)))

            with col3:
                data_pump_vol.metric('Tempo de falta de água na entrada da bomba na ultima hora:','{:.2f} horas'.format((len(df_old.index)-df_old['nivel_bomba'].sum())/(60*60)))
            
            graph_header.header('Variação do Volume, Status da Bomba e Nivel de entrada de água em Tempo Real')
            graph_primary.add_rows(df[['volume', 'status_bomba', 'nivel_bomba']])
        
        if (dt.datetime.timestamp(df_old.index.array[-1])-dt.datetime.timestamp(df_old.index.array[0])) >= 3600:
            df_old.drop(index=df_old.index.array[0:60], inplace=True)

else:
    date_init = (int(dt.datetime.strptime(str(date_init), '%Y-%m-%d').strftime('%s')))
    date_end = (int(dt.datetime.strptime(str(date_end), '%Y-%m-%d').strftime('%s')))

    df = dao.get_interval_data(date_init, date_end)

    if df.empty:
        graph_header.header('Não existem dados para o período Selecionado!')
    else:
        time_period = (dt.datetime.timestamp(df.index.array[-1])-dt.datetime.timestamp(df.index.array[0]))

        with st.container():
            with col1:
                data_vol.metric('Variação do volume no período:', '{:.2f} %'.format(df['volume'].mean()))

            with col2:
                data_pump.metric('Tempo de bomba ligado no período:', '{:.2f}'.format(df['status_bomba'].sum()/(time_period/3600)))

            with col3:
                data_pump_vol.metric('Falta de água na entrada da bomba de água no período:', '{:.2f}'.format((len(df.index)-df['nivel_bomba'].sum())/(time_period/3600)))

            graph_header.header('Variação do Volume no Período')
            graph_primary.area_chart(df[['volume']])

            col_graph_pump.header('Status da bomba no período')
            col_graph_pump.bar_chart(df, y=['status_bomba'], color=['#FF0000'])

            col_graph_pump_vol.header('Status se havia água para o funcionamento da bomba')
            col_graph_pump_vol.bar_chart(df, y=["nivel_bomba"], color=["#0000FF"])