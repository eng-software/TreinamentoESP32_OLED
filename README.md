 # Treinamento: Display OLED I2C com ESP32 usando ESP-IDF 5.3.0 + LVGL (via VSCode)

## 📦 Pré-requisitos
### Hardware
- ESP32 AM-032

### Software
- Visual Studio Code
- ESP-IDF 5.3.0 (instalado e configurado)
- Python 3.9+
  

## 🧱 Etapa 1 – Criar Projeto Base no VSCode
Vamos criar a base do projeto que configura e compila o básico para apenas  iniciar a placa  
- Crie uma pasta chamada **HandsonESP32IDF** em **Documentos**
- Abra o VSCode.  
- Vá em *File → Open folder*  e selecione a pasta criada  **HandsonESP32IDF**  
- Clique no ícone da *Espressif*
- Clique em **Select current ESP-IDF version** e aguarde alguns segundos até aparecer uma lista no centro da tela
- No centro da tela irá exibir a lista de versões dispiníveis. Selecione **Version: v5.3.0**
- Pressione *F1* → digite *ESP-IDF: Create Project from Extension Template*.  
- Selecione **Use current folder**
- Selecione **template-app**
- Assim que carregar o novo projeto, clique no ícone da *Espressif*
- Em seguida,  clique em **Build project** e aguarde finalizar a compilação

## 🧱 Etapa 2 - Scanner I2C
Essa etapa iremos configurar a porta I2C e construir um scanner para identificar os dispositivos I2C presentes

- No *Explorer* do VSCcode abra o arquivo **main.c**
- E insira o seguinte código:
```c++
#include "driver/i2c_master.h"
#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "example";

#define I2C_BUS_PORT            0
#define EXAMPLE_PIN_NUM_SDA     5
#define EXAMPLE_PIN_NUM_SCL     4

/**
 * @brief Entry point of the application.
 *
 * This function configures the I2C master mode and scans the bus for devices.
 * The bus is configured to use GPIO 5 for SDA and GPIO 4 for SCL, and the
 * clock speed is set to 100000 Hz. The scan starts from address 1 and goes
 * to address 126 (inclusive). If a device is found at an address, a message
 * is printed to the console with the address of the device.
 */
void app_main() 
{
    //---------------------------------
    // I2C Scanner
    //---------------------------------
    ESP_LOGI(TAG, "Initialize I2C bus");    
    i2c_master_bus_handle_t i2c_bus = NULL;    
    i2c_master_bus_config_t bus_config = 
    {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_BUS_PORT,
        .sda_io_num = EXAMPLE_PIN_NUM_SDA,
        .scl_io_num = EXAMPLE_PIN_NUM_SCL,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

    printf("Scanning I2C bus...\n");
    for (int i = 1; i < 127; i++) 
    {
        esp_err_t err = i2c_master_probe(i2c_bus, i, -1);
        if (err == ESP_OK) 
        {
            printf("Found device at 0x%02x\n", i);                
        }                    
    }
    i2c_del_master_bus(i2c_bus);
}
```
- Clique no ícone da *Espressif*
- Em seguida, clique em **Build project** e aguarde finalizar a compilação  
- Conecte o Kit na porta USB
- Clique em **ESP-IDF: Select Flash Method**
- No centro da tele selecione a opção **UART**
- Clique em **Select Port to Use** 
- No centro da tele selecione a porta serial correspondente ao Kit, exemplo **COM2**
- Clique em **Select Monitor Port to Use** 
- No centro da tele selecione a porta serial correspondente ao Kit, exemplo **COM2**
- No Kit, mantenha o botão ***BOOT*** pressionado e clique em **Flash Device**
- Assim que o *download* começar, libere o botão ***BOOT***
- Quando finalizar o *download* clique em **Monitor Device**
- Pressione o botão ***EN*** para reiniciar a placa
- O códido do scanner irá executar e irá encontrar o dispositivo ***0x3C*** , que é o display **OLED SSD1306**  
