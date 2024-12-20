# HidroMonitor
Um sistema de medição de nível de água, com ESP32, LoRa e aplicação web para visualização.

# Sensor de Nível de Água com ESP32, LoRa, Mosquitto MQTT e Django

## Descrição do Projeto
Este projeto tem como objetivo monitorar o nível de água utilizando um sensor hidrostático do tipo sonda, com comunicação baseada em ESP32, LoRa e Mosquitto MQTT. Os dados coletados são enviados para uma aplicação web desenvolvida com Django e armazenados em um banco de dados PostgreSQL.

### Principais Características:
- Monitoramento de níveis de água em tempo real.
- Interface web com tabelas e gráficos para visualização dos dados.
- Suporte a múltiplos sensores instalados em diferentes locais.
- Histórico de medições e alertas.
- Comunicação via protocolo MQTT com mensagens em formato JSON.
- Funcionalidades extras como notificações por e-mail e exportação de dados em CSV.

---

## Estrutura do Projeto
```plaintext
sensor_nivel_agua/
├── manage.py
├── sensorweb/
    ├── __init__.py
    ├── asgi.py
    ├── settings.py
    ├── urls.py
    └── wsgi.py
```

---

## Requisitos de Instalação

### Pré-requisitos
1. **Microsoft Visual C++ Build Tools**
   - Faça o download e instale o software [aqui](https://visualstudio.microsoft.com/visual-cpp-build-tools/).
   - Durante a instalação, selecione a opção "Desenvolvimento de C++ para Desktop" para incluir os compiladores necessários.

2. **Python e Bibliotecas**
   - Python 3.8 ou superior.
   - Instale as bibliotecas necessárias listadas no arquivo `requirements.txt`.

3. **PostgreSQL e pgAdmin**
   - Configure o PostgreSQL como banco de dados e utilize o pgAdmin para gerenciá-lo.

4. **Mosquitto MQTT**
   - Instale o Mosquitto MQTT Broker para comunicação entre os dispositivos ESP32 e a aplicação Django.

5. **ESP32 e LoRa**
   - Configure o ESP32 com o módulo LoRa para enviar dados coletados pelo sensor.
   

---

## Configuração do Ambiente
### Passo 1: Configuração do Banco de Dados
- Crie um banco de dados no PostgreSQL.
- Atualize as configurações do banco de dados no arquivo `settings.py` do Django:
```python
DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.postgresql',
        'NAME': 'sensor_db',
        'USER': 'usuario',
        'PASSWORD': 'senha',
        'HOST': 'localhost',
        'PORT': '5432',
    }
}
```

### Passo 2: Configuração do Mosquitto MQTT
- Instale o Mosquitto MQTT Broker:
  ```bash
  sudo apt install mosquitto mosquitto-clients
  ```
- Configure o broker MQTT conforme necessário.

### Passo 3: Configuração do ESP32
- Os arquivos estão na branch "esp-code" 
- Carregue o firmware no ESP32 para leitura do sensor e envio de dados via MQTT.
- Certifique-se de configurar os parâmetros de rede LoRa e as credenciais MQTT.

---

## Inicialização do Projeto
1. **Configuração da Aplicação Web**
   - Acesse o diretório do projeto:
     ```bash
     cd \web-app\HidroMonitor
     ```
   - Migre as tabelas para o banco de dados:
     ```bash
     python manage.py migrate
     ```
   - Inicie o servidor Django:
     ```bash
     python manage.py runserver
     ```

2. **Verificação da Comunicação MQTT**
   - Teste a publicação de dados com o Mosquitto MQTT Client:
     ```bash
     mosquitto_pub -h test.mosquitto.org -t "esp32/sensor" -m "{\"tensao\": 2.0}"
     ```

3. **Acesso à Interface Web**
   - Acesse a aplicação em [http://127.0.0.1:8000](http://127.0.0.1:8000).

