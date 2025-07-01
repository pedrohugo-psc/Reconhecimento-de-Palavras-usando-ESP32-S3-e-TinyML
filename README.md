# Sistema Embarcado para Reconhecimento de Palavras usando ESP32-S3 e TinyML

## 📝 Descrição do Trabalho  
Este projeto implementa um sistema embarcado **offline** para detecção de palavras-chave em português utilizando técnicas de **TinyML**. O firmware reconhece comandos de voz capturados por um microfone e controla um LED RGB integrado ao ESP32-S3 sem dependência de nuvem, garantindo:  
- ⚡ **Baixa latência** (resposta em tempo real)  
- 🔋 **Eficiência energética**  
- 🔒 **Privacidade** (processamento 100% local)  

O modelo de ML foi treinado e otimizado na plataforma Edge Impulse, quantizado para eficiência e embarcado no microcontrolador. Validações confirmaram desempenho satisfatório em hardware com recursos limitados.

---

## 🧩 Hardware  
| Componente       | Especificações/Modelo | Função                          |
|------------------|------------------------|---------------------------------|
| **Microcontrolador** | ESP32-S3              | Execução do modelo TinyML e controle geral |
| **Microfone**    | INMP441                | Captura de áudio (16-bit/16 kHz) |
| **LED**          | RGB interno (ESP32-S3) | Feedback visual dos comandos |

---

## ⚙️ Funcionalidades  
| Comando de Voz | Ação no LED RGB              | 
|----------------|------------------------------|
| `"zero"`       | Desliga o LED                | 
| `"um"`         | Acende o LED (branco fixo)   | 
| `"dois"`       | Muda para cor aleatória      | 
| `"três"`       | Inicia efeito de piscar      |

---

## 🔌 Conexões  
| Pino INMP441 | Pino ESP32-S3 | Função               | 
|--------------|---------------|----------------------|
| **VDD**      | 3.3V          | Alimentação          | 
| **GND**      | GND           | Terra                | 
| **L/R**      | GND           | Canal esquerdo       | 
| **SD**       | GPIO 10       | Dados (`DATA_PIN`)   | 
| **SCK**      | GPIO 12       | Clock (`SCK_PIN`)    | 
| **WS**       | GPIO 11       | Word Select (`WS_PIN`) |  

> **Importante:**  
> - Alimente o INMP441 com **3.3V** (5V danifica o componente)  
> - Conexões definidas no código:  
>   ```c
>   #define DATA_PIN 10
>   #define SCK_PIN 12
>   #define WS_PIN 11
>   ```

---

## ⚡ Fluxo do Sistema  
1. **Captura de Áudio**  
   - Microfone INMP441 coleta áudio bruto via interface I²S
   
2. **Pré-processamento**  
   - Framework AFE aplica filtros (cancelamento de eco, supressão de ruído)

3. **Buffer Circular**  
   - Armazena 16.000 amostras (1 segundo de áudio @16kHz)

4. **Inferência TinyML**  
   - Modelo embarcado classifica em tempo real:
     - `"zero"`, `"um"`, `"dois"`, `"três"`, `"desconhecido"`, `"ruído"`

5. **Controle do LED**  
   - Aciona ações conforme comando detectado (acurácia > 85%)

---

## 📦 Dependências  
- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/) (v5.1+)  
- Biblioteca [Edge Impulse](https://edgeimpulse.com/)  
- Driver [esp-sr](https://github.com/espressif/esp-sr) (Audio Front-End)

---
