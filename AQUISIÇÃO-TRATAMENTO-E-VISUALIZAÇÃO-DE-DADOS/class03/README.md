## Ambiente para a atividade

Atividade desenvolvida durante aula para exemplificar a utilização das bibliotecas do MQTT e do sqlite

Para executar a atividade:
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

No terminal:
* Executar o servidor:
```
python server.py
```
* Executar a aplicação do cliente, onde os paramentos são os dados:
```
python client.py 1 10-10-23 4.5 85
```
* Executar a leitura do banco de dados:
```
python read-db.py 
```