# Sistema-de-Monitoramento-de-Sinais-Vitais-com-IoT
O Sistema de Monitoramento de Sinais Vitais utilizando IoT tem como objetivo simular a leitura de sinais biológicos por meio de um potenciômetro, representando um sensor real como o MAX30102. O ESP32 realiza a leitura contínua do valor analógico, calcula uma estimativa de BPM (batimentos por minuto) e envia esses dados a um broker MQTT na nuvem.

Descrição do Projeto

O Sistema de Monitoramento de Sinais Vitais utilizando IoT tem como objetivo simular a leitura de sinais biológicos por meio de um potenciômetro, representando um sensor real como o MAX30102. O ESP32 realiza a leitura contínua do valor analógico, calcula uma estimativa de BPM (batimentos por minuto) e envia esses dados a um broker MQTT na nuvem.

O sistema também permite o acionamento remoto de um buzzer por meio de mensagens MQTT enviadas pelo cliente MQTTX. No simulador Wokwi, o buzzer não mantém o som ao trocar de aba, mas o comando é reconhecido e a latência permanece próxima de zero.

O projeto demonstra princípios fundamentais de IoT: coleta de dados, processamento embarcado, comunicação com a nuvem e acionamento remoto.

Funcionamento do Sistema
	1.O potenciômetro gera um valor analógico simulando um sinal biológico.
	2.O ESP32 converte o valor lido em uma estimativa de BPM.
	3.O valor calculado é publicado via MQTT para o broker.
	4.O ESP32 permanece inscrito em um tópico de comando.
	5.Quando o comando recebido é “1”, o buzzer é ligado; quando é “0”, é desligado.
	6.A latência entre comando e resposta pode ser medida no MQTTX.
	7.Todo o sistema funciona dentro do ambiente simulado do Wokwi.

Para reproduzir o projeto, basta abrir o circuito no Wokwi, executar o firmware e acompanhar os dados pelo MQTTX.

Software e Documentação de Código

O firmware foi desenvolvido em C++ utilizando a Arduino IDE pela sua simplicidade e compatibilidade com o ESP32.

Bibliotecas utilizadas:
WiFi.h – gerencia a conexão Wi-Fi no simulador
PubSubClient.h – implementa o protocolo MQTT
Arduino.h – funções básicas da plataforma

Arquivos inclusos no repositório:
Código-fonte principal (.ino)
Diagrama de montagem do circuito
Fluxograma do funcionamento
Diagrama de arquitetura do sistema

O código contém comentários explicativos sobre leitura do potenciômetro, conversão de BPM, envio dos dados ao broker e controle do buzzer.

Hardware Utilizado (Simulado no Wokwi)

ESP32 DevKit V1 – microcontrolador principal
Potenciômetro – simula um sensor MAX30102
LED RGB – atuador visual opcional
Buzzer – atuador sonoro controlado via MQTT
Jumpers virtuais – conexões internas do simulador
Protoboard virtual – estrutura de montagem
Resistores (apenas em hardware real, não usados no simulador)
Cabo USB-C (somente necessário em montagem física)

No ambiente simulado nenhuma alimentação física é necessária.

Interfaces, Protocolos e Módulos de Comunicação

Interfaces utilizadas:
Entrada analógica do potenciômetro via ADC
Saída digital para controle do buzzer
Conexão Wi-Fi simulada pelo Wokwi

Protocolos utilizados:
TCP/IP
MQTT (porta 1883, sem TLS devido à limitação do simulador)

Bibliotecas e módulos:
Módulo Wi-Fi interno do ESP32
Biblioteca PubSubClient para o MQTT

Comunicação via Internet e MQTT

A comunicação MQTT foi realizada utilizando o broker público HiveMQ, ideal para testes e protótipos por não exigir autenticação.

Servidor: broker.hivemq.com
Porta: 1883
Cliente recomendado: MQTTX

Tópicos:
Publicação de BPM: breno/iot/bpm
Assinatura para controle do buzzer: breno/iot/cmd

Formato da mensagem enviada:
{
“bpm”: 75,
“adc_raw”: 2100,
“dt_ms”: 120
}

Comandos aceitos pelo buzzer:
1 para ligar
0 para desligar

Arquitetura do Sistema

A arquitetura é composta por três camadas:

Camada simulada: ESP32, potenciômetro, LED RGB e buzzer.
Camada de comunicação: Wi-Fi simulado, MQTT sem TLS, publicação e assinatura de tópicos.
Camada de aplicação: MQTTX exibindo dados e enviando comandos.

Autor

Breno Oliveira de Souza
