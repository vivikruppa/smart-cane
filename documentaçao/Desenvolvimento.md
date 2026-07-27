# Desenvolvimento do projeto
O projeto foi desenvolvido incrementalmente, validando-se três módulos separados antes da integração (camera / api); (modelo OD); (TTS);  

## 1. Subprojetos e testes

### 1.1 Implementação do algoritmo para análise e reconhecimento de objetos e ambientes 

Esse subprojeto tem o objetivo de implementar um algoritmo de análise e reconhecimento de objetos e ambientes, para que este possa ser utilizado no ESP32S3, utilizando as capacidades de uma LLM atual de nivel básico.
Devido ao fato da implementação desse subprojeto ser muito mais complexa e dominante que o subprojeto 2, ele acaba influenciando muito mais o rumo que o Projeto como um todo toma em termos de software 

Devido a natureza inclusiva do projeto, foi optada por uma linguagem capaz de transmitir informação de forma mais rápida e capaz de se comunicar com os componentes externos de maneira simples, por isso a linguagem escolhida do nosso projeto foi C++, conforme o andamento da disciplina, também percebemos que a escolha foi ideal devido a pouca quantidade de memória que a linguagem consome quando comparada à outras alternativas como Micropython e CircuitPython.

Essa parte do projeto foi desenvolvido através de Arduino IDE e PlatformIO e seu desenvolvimento foi voltado a orientação de objetos, levando a criação de módulos distintos para tratar de cada uma das funcionalidades e requisitos presentes no projeto

### 1.2 Treinamento do modelo de Detecção de Objetos 

Nesse subprojeto, relatamos o desenvolvido de um modelo de detecção de objetos executado localmente no ESP32-S3. Essa abordagem permite avaliar uma alternativa com menor dependência de conexão com a internet, menor tempo de resposta e maior preservação das imagens capturadas, já que o processamento ocorre no próprio dispositivo.

Inicialmente, o modelo de detecção de objetos foi idealizado com base nos resultados documentados no artigo ["Edge AI Smart Cane for Real-Time Indoor Localization and Vision-Based Obstacle Detection"](https://ieeexplore.ieee.org/document/11368785). No artigo, detalha-se a construção de um modelo capaz de detectar cinco classes totais (background, water fountain, stair, warning
stripe, trash bin, and warning sign), que realiza inferências locais, a partir de uma ESP32-CAM, e obtêm um F1-score de teste de 97.4%. Tal modelo foi elaborado a partir da pipeline de treinamento de TinyML, da plataforma Edge Impulse.

Baseando-se nos resultados do artigo e no tutorial ["TinyML Made Easy: Object Detection with XIAO ESP32S3 Sense"](https://www.hackster.io/mjrobot/tinyml-made-easy-object-detection-with-xiao-esp32s3-sense-6be28d), construímos uma série de modelos de detecção de objetos, seguindo a pipeline de treinamento da Edge Impulse, que consiste em: 

1. Coleta de dados / imagens brutas; 
2. Rotulação das imagens e definição de quais imagens serão utilizadas para treinamento e quais serão utilizadas para teste; 
3. Pré-processamento das imagens; 
4. Treinamento do modelo; 
5. Teste do modelo antes do deploy (Live Classification); 
6. Deploy no ESP32S3; 

Dos modelos construídos através da plataforma, listamos abaixo aqueles que reproduziram resultados mais pertinentes para o nosso experimento: 

#### Modelo de detecção de uma classe (stairs)
O dataset utilizado para o treinamento desse modelo foi feito por nós, através da captura de 264 imagens pela câmera do módulo do ESP32S3 e rotulação feita a partir da ferramenta "AI labeling", nativa do Edge Impulse. Para o treinamento, as imagens foram configuradas para a dimensão de 320x320, com 50 épocas de treinamento, learning rate = 0,001 e utilizando a rede neural FOMO (Faster Objects, More Objects) MobileNetV2 0.35. O modelo apresentou performance muito abaixa do esperado, com zero sucesso para as métricas de Precisão, Recall e F1 Score no set de validação para teste. A hipótese para a ocorrência desse resultado está no dataset construído, que é considerado pequeno para tarefas de detecção de objetos. Desse experimento, decidimos que não criaríamos um dataset próprio, mas utilizaríamos datasets públicos, com imagens previamente rotuladas, dispostas em repositórios como Kaggle e Roboflow. 

#### Modelo de detecção de três classes (pothole, manhole, sign)
O dataset utilizado para o treinamento desse modelo consiste em 2,274 imagens adquiridas de um repositório público, correspondentes a classes pothole, manhole e sign, já rotuladas para a tarefa de detecção de objetos. Para o treinamento, as imagens foram configuradas para a dimensão de 96x96, com 60 épocas de treinamento, learning rate = 0,001 e utilizando a rede neural FOMO (Faster Objects, More Objects) MobileNetV2 0.35. A performance resultante do treinamento, avaliada a partir do set de validação, pode ser observada na imagem abaixo: 
<img width="575" height="896" alt="image" src="https://github.com/user-attachments/assets/4e0742e2-bc90-457d-a60a-d56105bbecd3" />

Uma limitação observada durante o treinamento desse modelo foi que, segundo regras impostas pela plataforma Edge Impulse, o tempo de treinamento é limitado a apenas uma hora. Dessa forma, os treinamentos consequentes do modelo foram prejudicados, já que não foi possível incluir novas imagens para cada classe, aumentando o tamanho do dataset, ou aumentar o número de épocas de treinamento, a fim de verificar melhoras na performance do modelo, pois ambas estratégias aumentariam o tempo de treinamento. Dessa experimento, decidimos que os próximos modelos treinados seriam limitados a apenas uma classe, como forma de permitir um maior número de imagens por classe, a fim de observar se o aumento do dataset permitiria uma melhora na performance do modelo.   

#### Modelo Only Sign (sign)
O dataset utilizado para o treinamento desse modelo consiste em 1,277 imagens adquiridas de um repositório público, sendo 879 destas correspondentes a classe sign, que é representada por uma placa de chão do tipo "Cuidado, Piso Molhada", tipicamente da cor amarela, e o restante representando a classe background, consistindo de placas que não são do tipo "Cuidado, Piso Molhado", como placas de trânsito. O dataset da classe sign, adquirido a partir de um repositório público, foi previmanete rotulado e enriquecido através da técnica de Data Augmentation. Para o treinamento, as imagens foram configuradas para a dimensão de 96x96, com 60 épocas de treinamento, learning rate = 0,001 e utilizando a rede neural FOMO (Faster Objects, More Objects) MobileNetV2 0.35. A performance resultante do treinamento, avaliada a partir do set de validação, pode ser observada na imagem abaixo:   
<img width="542" height="822" alt="image" src="https://github.com/user-attachments/assets/2cf5f798-5ee6-4329-8012-db45c4d883e7" /> 

O modelo escolhido para ser implementado no projeto final foi o Modelo Only Sign (que pode ser acessado através da plataforma Edge Impulse por: https://studio.edgeimpulse.com/public/954575/live). 

### 2.3 Saída PDM pcom um sinal simples

Durante a aula do dia 08 de julho, testou-se 

### 2.4 Comunicação com uma API TTS 


## 3. Problemas encontrados 
Modelo de Detecção de Objetos 


## 4. Aprendizados

## 5. Referências
