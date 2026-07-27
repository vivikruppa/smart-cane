# Desenvolvimento do projeto
O projeto foi desenvolvido incrementalmente, validando-se três módulos separados antes da integração (camera / api); (modelo OD); (TTS);  

## 1. Áreas de estudo e pesquisa
visão computacional;
sistemas embarcados;
síntese de voz;
formatos de áudio;
PCM, WAV e PDM;
APIs REST;
TTS; 
processamento local e em nuvem;
gerenciamento de memória no ESP32. 
### 1.1 Detecção de Objetos 
### 1.2 PDM 

## 2. Subprojetos e testes

### 2.1 Implementação do algoritmo para análise e reconhecimento de objetos e ambientes 

Esse subprojeto tem o objetivo de implementar um algoritmo de análise e reconhecimento de objetos e ambientes, para que este possa ser utilizado no ESP32S3, utilizando as capacidades de uma LLM atual de nivel básico.
Devido ao fato da implementação desse subprojeto ser muito mais complexa e dominante que o subprojeto 2, ele acaba influenciando muito mais o rumo que o Projeto como um todo toma em termos de software 

Devido a natureza inclusiva do projeto, foi optada por uma linguagem capaz de transmitir informação de forma mais rápida e capaz de se comunicar com os componentes externos de maneira simples, por isso a linguagem escolhida do nosso projeto foi C++, conforme o andamento da disciplina, também percebemos que a escolha foi ideal devido a pouca quantidade de memória que a linguagem consome quando comparada à outras alternativas como Micropython e CircuitPython.

Essa parte do projeto foi desenvolvido através de Arduino IDE e PlatformIO e seu desenvolvimento foi voltado a orientação de objetos, levando a criação de módulos distintos para tratar de cada uma das funcionalidades e requisitos presentes no projeto


### 2.2 Treinamento do modelo de Detecção de Objetos 

Inicialmente, o modelo de detecção de objetos foi idealizado com base nos resultados documentados no artigo ["Edge AI Smart Cane for Real-Time Indoor Localization and Vision-Based Obstacle Detection"](https://ieeexplore.ieee.org/document/11368785). No artigo, detalha-se a construção de um modelo capaz de detectar cinco classes totais (background, water fountain, stair, warning
stripe, trash bin, and warning sign), que realiza inferências locais, a partir de uma ESP32-CAM, e obtêm um F1-score de teste de 97.4%. Tal modelo foi elaborado a partir da pipeline de treinamento de TinyML, da plataforma Edge Impulse.

Baseando-se nos resultados do artigo e no tutorial ["TinyML Made Easy: Object Detection with XIAO ESP32S3 Sense"](https://www.hackster.io/mjrobot/tinyml-made-easy-object-detection-with-xiao-esp32s3-sense-6be28d), construímos variados modelos de detecção de objetos, seguindo a pipeline de treinamento da Edge Impulse, que consiste em: 

1. Coleta de dados / imagens brutas; 
2. Rotulação das imagens e definição de quais imagens serão utilizadas para treinamento e quais serão utilizadas para teste; 
3. Pré-processamento das imagens; 
4. Treinamento do modelo; 
5. Teste do modelo antes do deploy (Live Classification); 
6. Deploy no ESP32S3; 




### 2.3 Saída PDM pcom um sinal simples

Durante a aula do dia 08 de julho, testou-se 

### 2.4 Comunicação com uma API TTS 


## 3. Problemas encontrados 
Modelo de Detecção de Objetos 


## 4. Aprendizados

## 5. Referências
