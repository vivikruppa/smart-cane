# Smart Cane 

## 1. Resumo do Projeto 
(Contém as partes de proposta, Contexto, motivação, Objetivo) 

Este projeto propõe o desenvolvimento de um dispositivo assistivo baseado na placa Seeed Studio XIAO ESP32-S3 Sense, com o objetivo de auxiliar pessoas com deficiência visual na interpretação de informações presentes no ambiente. O sistema utiliza uma câmera para capturar imagens e processá-las, a partir de duas abordagens distintas. Na primeira, a imagem é enviada ao modelo Gemini, que produz uma descrição textual conforme o modo selecionado. Em seguida, o texto é encaminhado ao serviço Azure Speech, responsável pela síntese de voz. O áudio gerado é armazenado temporariamente no sistema de arquivos da placa e reproduzido por meio de uma saída PDM. Na segunda abordagem, o dispositivo utiliza um modelo de visão computacional desenvolvido no Edge Impulse para realizar inferências localmente no ESP32-S3, sem depender do processamento da imagem por um serviço externo.

## 2. Sistema Desenvolvido 
(Será responsável por explicar a Arquitetura do sistema (tanto o hardware como o software?) e o Fluxo de Processamento

### 3.1 Arquitetura
### 3.2 Hardware
### 3.3 Software
### 3.4 Modos de funcionamento
### 3.5 Fluxo de processamento

## 3. Resultados
### 3.1 Funcionalidades implementadas
### 3.2 Desempenho e tempo de resposta
### 3.3 Limitações

## 4. Como reproduzir o projeto

## 5. Conclusão

## 6. Documentação complementar

## 7. Referências

