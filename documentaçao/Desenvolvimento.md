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

O modelo escolhido para ser implementado no projeto final foi o Modelo Only Sign (que pode ser acessado através da plataforma Edge Impulse por: https://studio.edgeimpulse.com/public/954575/live). A etapa de inferência local foi validada separadamente por meio da Arduino IDE, utilizando a biblioteca exportada automaticamente pelo Edge Impulse. Após a adaptação dos pinos da câmera para a XIAO ESP32-S3 Sense e o ajuste da configuração de memória do modelo, a câmera foi inicializada corretamente e o sistema passou a executar inferências contínuas em aproximadamente 142 ms. Durante os testes, o modelo foi capaz de identificar a classe sign e gerar as respectivas coordenadas da caixa delimitadora. Quando nenhum objeto era reconhecido acima do limiar de confiança, o sistema apenas informava a ausência de detecções, assim como ilustrado em captura da imagem pelo Serial Monitor: 

<img width="685" height="353" alt="image" src="https://github.com/user-attachments/assets/ed6ef1ec-0fff-4877-b472-6db89017bef4" />

Além do teste de inferência realizado localmente na XIAO ESP32-S3 Sense, o modelo também foi avaliado por meio da plataforma [SenseCraft-Web-Toolkit](https://seeed-studio.github.io/SenseCraft-Web-Toolkit/#/setup/process). Nesse ambiente, foram comparadas imagens contendo o objeto de interesse e imagens que não deveriam gerar detecção. Em uma das situações, o modelo identificou corretamente uma placa de aviso de piso molhado, indicando que conseguiu reconhecer características presentes nas imagens de treinamento. Entretanto, também foram observadas detecções em imagens sem a presença desse objeto, sugerindo a ocorrência de falsos positivos. Esse comportamento indica que o modelo ainda apresenta limitações de generalização e pode estar associando elementos visuais semelhantes, como cores, formatos ou padrões de fundo, à classe treinada.

![Detecção correta da placa de piso molhado](documentaçao/deteccao-correta-sensecraft.png)

![Exemplo de possível falso positivo](documentaçao/falso-positivo-sensecraft.png)

### 2.3 Saída PDM com um sinal simples
 
#### Justificativa para a escolha do PDM e Fluxo de Funcionamento

O projeto utiliza a XIAO ESP32-S3 Sense, cujo microcontrolador não possui DAC analógico interno. Isso significa que as amostras digitais de áudio não podem ser convertidas diretamente pelo próprio chip em uma tensão analógica convencional. Como alternativa, foi utilizado o periférico I2S do ESP32-S3 em modo PDM TX (a partir da referência teórica em: https://atomic14.substack.com/p/esp32-s3-no-dac).  Nesse modo, o microcontrolador recebe amostras PCM e as converte em um fluxo PDM, formado por pulsos digitais cuja densidade representa a amplitude do áudio. A escolha do PDM permitiu aproveitar um recurso já disponível no ESP32-S3 e reduzir a necessidade de componentes externos dedicados à conversão de áudio.

No sistema, o áudio é representado inicialmente por amostras PCM. Essas amostras são encaminhadas ao periférico I2S, configurado em modo PDM TX. O próprio hardware efetua a conversão de amostras PCM para amostras PDM. O sinal PDM ainda é digital, pois alterna entre níveis lógicos baixo e alto. A recuperação do áudio ocorre pela suavização desses pulsos, de modo que sua densidade média represente uma tensão variável correspondente ao sinal sonoro.

#### Experimento inicial com a reprodução de WAV 

Foi realizado um teste de reprodução de áudio a partir de um arquivo WAV. Para essa etapa, foi utilizado como referência [o código disponibilizado pelo Atomic14 no projeto `esp32-pdm-audio`](https://github.com/atomic14/esp32-pdm-audio/tree/main). O código original realiza a leitura de um arquivo `sample.wav` armazenado no sistema de arquivos da placa, extrai suas amostras PCM em blocos e as encaminha a uma classe de saída de áudio configurada para PDM. O código foi testado em aula, juntamente com a construção de um circuito que conectava o ESP32-S3 com um auto-falante capaz de filtrar em amplificar o sinal em PDM. 

A partir desse exemplo, foram realizados testes e adaptações para a XIAO ESP32-S3 Sense e para a organização modular adotada no projeto. O código de saída PDM foi separado em funções próprias, permitindo posteriormente reutilizar o mesmo fluxo para reproduzir os arquivos WAV gerados pelo serviço de síntese de voz.

#### Melhorias: utilização do módulo MAX98357A

Uma alternativa ao uso direto do PDM seria utilizar o [módulo MAX98357A](https://www.makerguides.com/pt/playing-audio-with-esp32-and-max98357-pt/). Nesse caso, o ESP32-S3 enviaria as amostras PCM pelo protocolo I2S padrão. O MAX98357A receberia esse áudio digital e realizaria internamente a conversão e a amplificação necessárias para alimentar um alto-falante. O PDM utilizado no protótipo apresenta limitações relacionadas à necessidade de filtragem, ao baixo nível de potência disponível no GPIO e à impossibilidade de alimentar diretamente um alto-falante passivo de baixa impedância. O uso do MAX98357A poderia contornar parte dessas limitações, pois o módulo já possui amplificação integrada e foi projetado especificamente para acionar alto-falantes.

