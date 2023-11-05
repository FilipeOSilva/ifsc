# PFL0084 - AQUISIÇÃO, TRATAMENTO E VISUALIZAÇÃO DE DADOS

## Ambiente para executar o trabalho

Para executar o trabalho, a forma mais simples é:
* Acessar a pasta do trabalho
```
cd server
```
* Criar um ambiente
```
python -m venv ./venv
```
* Acessar o ambiente:
```
source ./venv/bin/activate
```
* Instalar as "bibliotecas" requiridas
```
pip install -r requirements.txt
```
* Executar o broker MQTT:
```
python server.py
```
* Executar o dashboard:
```
streamlit run view.py
```
