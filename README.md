# Smart Cane 
<div align="justify">
Este projeto consiste no desenvolvimento de um <strong>dispositivo assistivo baseado na placa Seeed Studio XIAO ESP32-S3 Sense</strong>, com o objetivo de auxiliar pessoas com deficiência visual na interpretação de informações presentes no ambiente. Para isso, o sistema utiliza uma câmera para capturar imagens e processá-las de acordo com diferentes modos de funcionamento, permitindo obter descrições de objetos, textos e características do espaço ao redor.
</div>

## 1. Resumo do Projeto

#### Objetivo

Este projeto apresenta o desenvolvimento de um protótipo de dispositivo assistivo voltado ao apoio de pessoas com deficiência visual na interpretação de informações presentes no ambiente. A proposta foi concebida como uma etapa inicial para uma possível integração futura a uma smart cane, explorando o uso de hardware embarcado de baixo custo, visão computacional e retorno auditivo como recursos de acessibilidade.

#### Funcionamento geral

O dispositivo é baseado na placa Seeed Studio XIAO ESP32-S3 Sense e utiliza uma câmera para capturar informações visuais do ambiente. A partir dessas imagens, o sistema pode gerar descrições de objetos, textos e características do espaço ao redor, além de executar um modelo local de detecção de objetos. As informações produzidas podem ser apresentadas ao usuário por meio de áudio.

Para mais detalhes, consulte a seção [Sistema Desenvolvido](#2-sistema-desenvolvido). 

#### Tecnologias utilizadas

Durante o desenvolvimento, foram estudados temas relacionados a sistemas embarcados, visão computacional, inteligência artificial em nuvem, TinyML, comunicação com APIs, síntese de voz, formatos digitais de áudio e gerenciamento de memória em microcontroladores.

Entre as principais tecnologias utilizadas estão a XIAO ESP32-S3 Sense, o framework Arduino com PlatformIO, o Gemini para análise de imagens, o Azure Speech para síntese de voz, o Edge Impulse para inferência local e o LittleFS para armazenamento temporário.

Para mais detalhes e justificativas sobre as tecnologias escolhidas, consulte a seção [Arquitetura de software](#23-arquitetura-de-software).

#### Resultados

Como resultado, foi obtido um protótipo funcional capaz de capturar imagens, produzir descrições textuais, convertê-las em áudio e executar inferências localmente. O projeto também permitiu avaliar limitações relacionadas ao tempo de resposta, à conectividade, aos recursos de memória e à integração entre diferentes módulos de software.

Para mais detalhes, consulte a seção [Resultados](#3-resultados).

## 2. Sistema Desenvolvido

O sistema desenvolvido possui uma arquitetura híbrida e modular, combinando processamento realizado localmente no dispositivo com serviços executados em nuvem. A placa Seeed Studio XIAO ESP32-S3 Sense atua como unidade central do protótipo, sendo responsável pelo controle da câmera, seleção dos modos de funcionamento, conexão com a rede Wi-Fi, comunicação com serviços externos, armazenamento temporário de arquivos e reprodução do áudio.

O sistema oferece diferentes modos de captura e análise de imagem que utilizam a câmera para capturar uma imagem posteriormente enviada ao Gemini para geração de uma descrição textual. Cada modo aplica uma configuração específica à câmera e utiliza um prompt correspondente ao tipo de informação desejada:

1. **Apoio à mobilidade e identificação de riscos**

   O primeiro modo é voltado ao auxílio durante o deslocamento do usuário. A imagem capturada é analisada com prioridade para obstáculos, degraus, buracos, portas, passagens e outros elementos que possam representar risco imediato.

   O sistema também busca indicar a direção mais livre para seguir, produzindo uma resposta curta e objetiva para facilitar a compreensão por áudio.

2. **Leitura e análise de objetos próximos**

   O segundo modo é destinado à análise de objetos posicionados próximos à câmera. Sua configuração procura favorecer a captura de detalhes e textos presentes em embalagens, placas, documentos ou outros objetos.

   O Gemini recebe uma solicitação para identificar o objeto principal, verificar a presença de texto e descrever seu estado ou suas características mais relevantes.

3. **Descrição geral do ambiente**

   O terceiro modo utiliza uma configuração voltada à captura de cenas mais amplas. Seu objetivo é oferecer uma descrição geral do ambiente, destacando objetos, locais, pessoas ou acontecimentos relevantes presentes na imagem.

O quarto modo utiliza um fluxo diferente dos demais. Em vez de enviar a imagem para um serviço externo, o dispositivo executa localmente um modelo de visão computacional desenvolvido no Edge Impulse: 

4. **Inferência local**

   Nesse modo, a imagem é capturada, preparada e fornecida ao classificador embarcado, que retorna as classes detectadas, suas probabilidades e, quando aplicável, a localização dos objetos na imagem. Na versão atual, o resultado da inferência é exibido no Monitor Serial e ainda não é encaminhado ao sistema de áudio.
   
### 2.1 Fluxo de processamento

A arquitetura do sistema contempla três fluxos principais: 
1. **Fluxo de imagem para texto:** a imagem capturada pela câmera é convertida para Base64 e enviada ao modelo Gemini, juntamente com um prompt correspondente ao modo selecionado. O serviço processa a imagem e retorna uma descrição textual.

2. **Fluxo de texto para áudio:** a descrição produzida pelo Gemini é encaminhada ao Azure Speech, responsável pela síntese de voz. O áudio retornado em formato WAV é armazenado temporariamente no LittleFS, interpretado pelo módulo de reprodução e enviado pela saída PDM.

3. **Fluxo de inferência local:** a imagem capturada é processada diretamente no ESP32-S3 por meio de um modelo desenvolvido no Edge Impulse. Nesse fluxo, não há envio da imagem para serviços externos.

### 2.2 Arquitetura de Hardware

Os componentes utilizados para a elaboração do protótipo são descritos na tabela abaixo: 

| Componente | Função no projeto |
|---|---|
| Seeed Studio XIAO ESP32-S3 Sense | Processamento central, conectividade Wi-Fi e controle dos periféricos |
| Câmera do módulo Sense | Captura de imagens para processamento em nuvem e inferência local |
| Interface de áudio PDM | Transmissão digital das amostras de áudio |
| Fones ou caixa de som utilizada nos testes | Reprodução da resposta auditiva |
| Cabo USB | Alimentação, programação e comunicação com o Monitor Serial |
| Jumpers e conexões | Interligação entre a placa e a saída de áudio |

As conexões executadas são ilustradas na tabela abiaxo e no diagrama: 
| Sinal | Pino da XIAO ESP32-S3 |
|---|---|
| Dados PDM | GPIO 1 |
| Clock PDM | GPIO 2 |
| Terra | GND |

#### Placa principal e Sistema de captura de imagem
A unidade central do protótipo é a 
[Seeed Studio XIAO ESP32-S3 Sense](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/#xiao-esp32-s3-sense-front). Sua escolha esteve relacionada principalmente à integração entre o microcontrolador ESP32-S3 e o módulo de câmera, às dimensões compactas e à disponibilidade de conectividade Wi-Fi. 

A câmera integrada ao módulo Sense atua como principal dispositivo de entrada do sistema. Nos modos 1, 2 e 3, ela é configurada para capturar imagens em formato JPEG e resolução QXGA, utilizando dois buffers de imagem. A escolha do formato JPEG reduz o tamanho dos dados antes do envio ao serviço de análise em nuvem. Antes de cada captura, o software aplica configurações específicas de exposição e balanço de branco conforme o modo selecionado. Após os ajustes, o sistema aguarda brevemente a estabilização do sensor antes de capturar a imagem.

Já no modo de inferência local, a câmera utiliza uma configuração diferente, com resolução QVGA e armazenamento do frame buffer em PSRAM. A imagem JPEG capturada é convertida para RGB888 e redimensionada para as dimensões exigidas pelo modelo do Edge Impulse antes da execução da inferência.

#### Circuito de saída de áudio

A saída auditiva do protótipo é realizada por meio de uma interface PDM controlada pelo ESP32-S3. O circuito utiliza linhas digitais de dados e clock, além de uma referência comum de terra, para encaminhar o sinal ao dispositivo de reprodução utilizado nos testes.

A escolha dessa abordagem permitiu desenvolver a reprodução de áudio diretamente a partir dos recursos disponíveis no microcontrolador, sem a adoção do módulo amplificador com DAC inicialmente considerado. O áudio recebido do serviço de síntese de voz é convertido pelo software em amostras PCM, que são transmitidas pelo periférico configurado para a saída PDM.

Para mais detalhes sobre a técnica PDM e referências utilizadas, consulte a sessão 

### 2.3 Arquitetura de Software
<div align="justify">
O software foi organizado de forma modular, separando as responsabilidades de captura de imagem, comunicação com serviços externos, tratamento de texto, geração e reprodução de áudio e inferência local. O arquivo main.cpp atua como controlador central, inicializando os recursos do sistema, recebendo a seleção do modo e acionando os módulos necessários para cada fluxo. A tabela seguinte detalha a responsabilidade de cada módulo:
</div>

| Módulo | Responsabilidade |
|---|---|
| `main.cpp` | Controla a inicialização do sistema, recebe o modo selecionado e coordena a execução dos demais módulos. |
| `Camera` | Inicializa e configura a câmera, aplica os modos de captura, registra a fotografia e converte a imagem para Base64. |
| `WifiManager` | Gerencia a conexão Wi-Fi, envia a imagem e o prompt ao Gemini e conduz o início do fluxo de síntese de voz. |
| `Utilities` | Armazena os prompts utilizados nos diferentes modos e extrai o texto útil da resposta retornada pelo Gemini. |
| `AzureTtsClient` | Envia o texto ao Azure Speech e grava o áudio retornado em um arquivo WAV no LittleFS. |
| `WavPlayer` | Abre o arquivo WAV, interpreta seu cabeçalho e encaminha os dados PCM para reprodução. |
| `PdmOutput` | Configura o periférico de áudio e transmite as amostras PCM por meio da saída PDM. |
| `SignInference` | Captura a imagem e executa localmente o modelo de detecção desenvolvido no Edge Impulse. |
| `LittleFS` | Armazena temporariamente o arquivo WAV gerado pelo serviço de síntese de voz. |
| `secrets.h` | Mantém as credenciais da rede Wi-Fi e das APIs separadas do código principal. |

<div align="justify">
Nos modos baseados em serviços de nuvem, o main.cpp cria e configura o módulo de câmera conforme o modo escolhido. A imagem capturada é convertida para Base64 e entregue ao WifiManager, que monta e envia a requisição ao Gemini. A resposta textual é tratada com auxílio de Utilities e enviada ao AzureTtsClient. O áudio retornado é salvo temporariamente no LittleFS, interpretado pelo WavPlayer e encaminhado ao PdmOutput, responsável pela reprodução. 

O modo 4 utiliza um fluxo independente. O main.cpp inicializa o módulo SignInference, que controla a câmera, prepara a imagem e executa o classificador do Edge Impulse localmente. Na versão atual, os resultados são apresentados pelo Monitor Serial e ainda não são encaminhados ao módulo de áudio.
</div>

Exemplos que vale explicar depois:
<strong> 
por que a imagem é convertida para Base64;
por que usamos JSON nas requisições;
por que o áudio é recebido em WAV PCM;
por que usamos LittleFS;
por que o arquivo é temporário e removido após a reprodução;
por que existe um módulo WavPlayer separado do PdmOutput;
por que parte do processamento ocorre na nuvem;
por que existe também inferência local com Edge Impulse;
por que escolhemos Azure Speech para TTS;
por que passamos grandes String por referência;
por que foi necessário dividir o projeto em módulos de teste antes da integração.
</strong> 

## 3. Resultados

O desenvolvimento foi conduzido de forma incremental, com a criação e validação de módulos separados para captura de imagem, comunicação Wi-Fi, processamento por inteligência artificial, síntese de voz, armazenamento e reprodução de áudio. Após os testes individuais, esses componentes foram integrados em um único sistema. Como resultado, foi obtido um protótipo capaz de capturar imagens, gerar descrições textuais e reproduzi-las em áudio, além de executar inferências localmente. O sistema ainda apresenta limitações relacionadas ao tempo de resposta, à dependência de conexão com a internet e à integração entre alguns modos de funcionamento.


#### Funcionalidades implementadas
Ao final do desenvolvimento, foram implementadas e integradas as seguintes funcionalidades:

- conexão da XIAO ESP32-S3 Sense a uma rede Wi-Fi;
- seleção de diferentes modos de funcionamento;
- configuração da câmera conforme o modo selecionado;
- captura de imagens em formato JPEG;
- conversão das imagens para Base64;
- envio de imagens e prompts ao Gemini;
- recebimento e tratamento das descrições textuais;
- envio do texto ao Azure Speech;
- recebimento e armazenamento temporário do áudio em formato WAV;
- leitura dos dados PCM presentes no arquivo;
- reprodução do áudio por meio da saída PDM;
- remoção do arquivo temporário após a reprodução;
- execução local de um modelo de visão computacional desenvolvido no Edge Impulse.

O fluxo de processamento em nuvem foi validado de forma completa. Durante os testes, o dispositivo capturou imagens, enviou os dados ao Gemini e recebeu descrições coerentes com as cenas observadas. As descrições retornadas foram encaminhadas ao Azure Speech e convertidas em arquivos de áudio reproduzidos pelo protótipo.

A integração com o LittleFS permitiu armazenar temporariamente o arquivo WAV recebido. Após a leitura e reprodução das amostras PCM pela saída PDM, o arquivo foi removido, liberando novamente o espaço de armazenamento.

Os testes também demonstraram que os modos de descrição conseguem alterar o foco da resposta por meio das configurações de câmera e dos prompts enviados ao Gemini. Dessa forma, o sistema pode priorizar informações relacionadas à mobilidade, objetos próximos ou características gerais do ambiente.

#### Desempenho e tempos de resposta

Para avaliar o comportamento do sistema, foram adicionadas medições de tempo às principais etapas do fluxo. Em uma execução de teste, foram obtidos os seguintes valores:

| Etapa do processamento | Tempo observado |
|---|---:|
| Configuração da câmera | 261 ms |
| Captura e conversão para Base64 | 312 ms |
| Requisição ao Gemini | 9,547 s |
| Geração e download do áudio pelo Azure Speech | 37,892 s |
| Reprodução do áudio | 17,372 s |
| Fluxo completo | 65,808 s |

Os valores correspondem a uma execução específica e podem variar de acordo com a qualidade da conexão, a disponibilidade dos serviços, o tamanho da imagem, a extensão do texto gerado e a duração do áudio.

As medições indicam que a captura e a preparação da imagem representam uma parcela pequena do tempo total. A maior latência ocorre na síntese de voz, no download do arquivo e na reprodução do áudio.

#### Limitações
O hardware desenvolvido corresponde a um protótipo de bancada destinado à validação das funções de captura, processamento e reprodução de áudio. Nesta etapa, não foram desenvolvidos a estrutura mecânica, a alimentação portátil, os controles físicos nem a forma de fixação em uma bengala.

Assim, o projeto valida principalmente a viabilidade do sistema eletrônico e de software. A integração em um dispositivo portátil e ergonomicamente adequado permanece como uma etapa futura.

## 4. Documentação Complementar 

O sistema foi desenvolvido de forma incremental, com testes separados dos módulos antes da integração. Mais detalhes estão disponíveis na
[documentação de desenvolvimento](docs/desenvolvimento.md). 
