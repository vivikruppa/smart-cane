//
// Created by wenoysd on 01/06/2026.
//

#include "Utilities.h"
#include <iostream>


char* Utilities::getCharInput()
{
    char* temp = new char[65];
    std::cin >> temp;
    return temp;
}


Utilities::Utilities()
{
    nightPrompt = "Analise a imagem para auxiliar a mobilidade de uma pessoa com deficiência "
    "visual. Informe apenas obstáculos e suas distâncias aproximadas, riscos, degraus, buracos, portas, "
    "passagens e a direção mais livre para seguir. Priorize o que estiver "
    "próximo e puder causar risco imediato. Responda em no máximo duas frases "
    "curtas, sem títulos, listas, markdown ou explicações adicionais.";

    shortPrompt =  "Identifique o principal objeto próximo da câmera. Caso exista texto "
    "legível, leia apenas as informações mais importantes. Informe também a "
    "posição aproximada do objeto. Responda em no máximo três frases curtas, "
    "sem títulos, listas, markdown ou explicações adicionais.";

    landscapePrompt =  "Descreva de forma breve o ambiente ao redor para uma pessoa com deficiência "
    "visual. Informe o tipo de ambiente, os principais objetos, pessoas e pontos "
    "de referência, indicando posições como esquerda, direita, frente ou fundo. "
    "Responda em no máximo três frases curtas, sem títulos, listas, markdown ou "
    "explicações adicionais.";

}


String Utilities::extractCleanText(String rawPayload)
{
    // 1. Find the exact start of the text value
    int textStart = rawPayload.indexOf("\"text\": \"");
    if (textStart == -1) return "Error: Could not find text block.";

    textStart += 9; // Move the index past "text": " to the actual first letter

    // 2. Find where the thought signature starts
    int thoughtStart = rawPayload.indexOf("\"thoughtSignature\"");

    int textEnd;
    if (thoughtStart != -1) {
        // Find the closing quote and comma (",) right before thoughtSignature
        textEnd = rawPayload.lastIndexOf("\",", thoughtStart);
    } else {
        // Failsafe just in case thoughtSignature isn't in a future response
        textEnd = rawPayload.indexOf("\",\n", textStart);
    }

    // 3. Extract exactly what is between those two points
    if (textEnd == -1 || textEnd <= textStart) return "Error: Parsing failed.";

    String cleanText = rawPayload.substring(textStart, textEnd);

    // 4. The API sends newlines as literal '\' and 'n'. We translate them back to real newlines.
    cleanText.replace("\\n", "\n");
    // The API escapes quotes inside the text. Translate them back.
    cleanText.replace("\\\"", "\"");

    return cleanText;
};
