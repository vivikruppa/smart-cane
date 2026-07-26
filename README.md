# Smart Cane 

## 1. Resumo do Projeto 
(Contém as partes de proposta, Contexto, motivação, Objetivo) 

## 1. Resumo do Projeto

Este projeto consiste no desenvolvimento de um dispositivo assistivo baseado na placa Seeed Studio XIAO ESP32-S3 Sense, com o objetivo de auxiliar pessoas com deficiência visual na interpretação de informações presentes no ambiente. Para isso, o sistema utiliza uma câmera para capturar imagens e processá-las de acordo com diferentes modos de funcionamento, permitindo obter descrições de objetos, textos e características do espaço ao redor.

O processamento é realizado por meio de duas abordagens. Na primeira, a imagem capturada é convertida para Base64 e enviada ao modelo Gemini, que gera uma descrição textual conforme o modo selecionado. Em seguida, o texto é encaminhado ao serviço Azure Speech, responsável pela síntese de voz. O áudio retornado em formato WAV é armazenado temporariamente no sistema de arquivos LittleFS e reproduzido pela placa por meio de uma saída PDM. Na segunda abordagem, o dispositivo utiliza um modelo de visão computacional desenvolvido na plataforma Edge Impulse para realizar a detecção de objetos localmente no ESP32-S3.

O desenvolvimento foi conduzido de forma incremental, com a criação e validação de módulos separados para captura de imagem, comunicação Wi-Fi, processamento por inteligência artificial, síntese de voz, armazenamento e reprodução de áudio. Após os testes individuais, esses componentes foram integrados em um único sistema. Como resultado, foi obtido um protótipo capaz de capturar imagens, gerar descrições textuais e reproduzi-las em áudio, além de executar inferências localmente. O sistema ainda apresenta limitações relacionadas ao tempo de resposta, à dependência de conexão com a internet e à integração entre alguns modos de funcionamento.

## 2. Sistema Desenvolvido 
(Será responsável por explicar a Arquitetura do sistema (tanto o hardware como o software?) e o Fluxo de Processamento

### 2.1 Fluxo de processamento
A arquitetura do sistema é híbrida e modular, combinando processamento embarcado e serviços em nuvem. A placa XIAO ESP32-S3 Sense atua como elemento central, sendo responsável pela captura das imagens, seleção dos modos, comunicação com serviços externos, armazenamento temporário e reprodução de áudio. Nos modos de descrição, as imagens são enviadas ao Gemini para geração de texto, que posteriormente é convertido em áudio pelo Azure Speech. No modo de inferência local, um modelo do Edge Impulse é executado diretamente no ESP32-S3. O software é dividido em módulos responsáveis por câmera, conectividade, processamento das respostas, síntese de voz, armazenamento, reprodução WAV, saída PDM e inferência local.

### 3.2 Hardware

### 3.3 Arquitetura de Software
A arquitetura de software é híbrida, pois combina processamento local no ESP32-S3 com processamento realizado por serviços em nuvem. Nos modos 1, 2 e 3, o dispositivo captura uma imagem localmente, envia seus dados ao Gemini e recebe uma descrição textual. Essa descrição é então enviada ao Azure Speech, que retorna um arquivo de áudio reproduzido pela placa. No modo 4, o processamento da imagem é realizado localmente por um modelo do Edge Impulse.
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

Nos modos baseados em serviços de nuvem, o main.cpp cria e configura o módulo de câmera conforme o modo escolhido. A imagem capturada é convertida para Base64 e entregue ao WifiManager, que monta e envia a requisição ao Gemini. A resposta textual é tratada com auxílio de Utilities e enviada ao AzureTtsClient. O áudio retornado é salvo temporariamente no LittleFS, interpretado pelo WavPlayer e encaminhado ao PdmOutput, responsável pela reprodução


## 3. Resultados
### 3.1 Funcionalidades implementadas / Modos de Funcionamento  
### 3.2 Desempenho e tempo de resposta
### 3.3 Limitações

## 4. Como reproduzir o projeto

## 5. Conclusão

## 6. Documentação complementar

## 7. Referências

